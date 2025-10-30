#include "level_api.hpp"

#include <Windows.h>     // for memchr, GetCurrentThread, LONG, NO_...
#include <array>         // for array, _Array_iterator, _Array_cons...
#include <assert.h>      // for assert
#include <cmath>         // for ceil, abs
#include <cstddef>       // for byte
#include <cstdlib>       // for size_t, abs
#include <cstring>       // for memcpy, memchr
#include <detours.h>     // for DetourAttach, DetourTransactionBegin
#include <fmt/format.h>  // for check_format_string, format, vformat
#include <iterator>      // for back_insert_iterator, back_inserter
#include <list>          // for _List_iterator, _List_const_iterator
#include <memory>        // for unique_ptr, make_unique
#include <mutex>         // for lock_guard, mutex
#include <numbers>       // for pi_v
#include <string_view>   // for string_view
#include <tuple>         // for tie, tuple
#include <unordered_map> // for unordered_map, _Umap_traits<>::allo...

#include "bucket.hpp"                // for Bucket
#include "entities_activefloors.hpp" //
#include "entities_items.hpp"        //
#include "entities_monsters.hpp"     // for GHOST_BEHAVIOR, GHOST_BEHAVIOR::MED...
#include "entity.hpp"                // for Entity, get_entity_ptr, Enti...
#include "entity_db.hpp"             // for to_id
#include "entity_lookup.hpp"         // for get_entities_overlapping_by_pointer ...
#include "layer.hpp"                 // for Layer, g_level_max_y, g_level_max_x
#include "logger.h"                  // for DEBUG
#include "memory.hpp"                // for to_le_bytes, write_mem_prot, Execut...
#include "movable.hpp"               // for Movable
#include "prng.hpp"                  // for PRNG, PRNG::EXTRA_SPAWNS
#include "rpc.hpp"                   // for attach_entity, get_entities_overlap...
#include "script/events.hpp"         // for post_load_screen, pre_load_screen
#include "search.hpp"                // for get_address
#include "spawn_api.hpp"             // for pop_spawn_type_flags, push_spawn_ty...
#include "state.hpp"                 // for StateMemory
#include "util.hpp"                  // for OnScopeExit, trim
#include "vtable_hook.hpp"           // for hook_vtable

std::uint32_t g_last_tile_code_id;
std::uint32_t g_last_community_tile_code_id;
std::uint32_t g_current_tile_code_id;

std::uint32_t g_last_chance_id;
std::uint32_t g_last_community_chance_id;
std::uint32_t g_current_chance_id;

std::unordered_map<std::uint32_t, std::string_view> g_tile_code_id_to_name;
std::unordered_map<std::string_view, std::uint32_t> g_name_to_tile_code_id;
std::unordered_map<std::uint32_t, std::string_view> g_monster_chance_id_to_name;
std::unordered_map<std::uint32_t, std::string_view> g_trap_chance_id_to_name;

struct FloorRequiringEntity
{
    struct Position
    {
        float x;
        float y;
        std::optional<float> angle;
    };
    std::vector<Position> pos;
    std::int32_t uid;
    bool handled;
};
std::vector<FloorRequiringEntity> g_floor_requiring_entities;
struct PendingEntitySpawn
{
    struct Position
    {
        float x;
        float y;
        std::optional<float> angle;
    };
    std::vector<Position> pos;
    std::function<void()> try_spawn;
    bool handled;
};
std::vector<PendingEntitySpawn> g_attachee_requiring_entities;

struct CommunityTileCode;

using TileCodeFunc = void(const CommunityTileCode& self, float x, float y, Layer* layer);

bool is_room_flipped(float x, float y)
{
    thread_local StateMemory* state_ptr = HeapBase::get().state();
    auto [ix, iy] = state_ptr->level_gen->get_room_index(x, y);
    return state_ptr->level_gen->flipped_rooms->rooms[ix + iy * 8ull];
}

struct CommunityTileCode
{
    std::string_view tile_code;
    std::string_view entity_type;
    TileCodeFunc* func = [](const CommunityTileCode& self, float x, float y, Layer* layer)
    {
        layer->spawn_entity_snap_to_floor(self.entity_id, x, y);
    };
    std::uint32_t entity_id;
    std::uint32_t tile_code_id;
};
void g_spawn_not_snapped_to_floor(const CommunityTileCode& self, float x, float y, Layer* layer)
{
    layer->spawn_entity(self.entity_id, x, y, false, 0, 0, false);
};
template <GHOST_BEHAVIOR behavior>
auto g_spawn_ghost = [](const CommunityTileCode& self, float x, float y, Layer* layer)
{
    Ghost* ghost = layer->spawn_entity(self.entity_id, x, y, false, 0.0f, 0.0f, true)->as<Ghost>();
    ghost->ghost_behaviour = behavior;
};
template <int offset_x, int offset_y, int angle_in_multiples_of_pi_halves, bool ignore_flip = false>
void g_spawn_eggsac(const CommunityTileCode& self, float x, float y, Layer* layer)
{
    if constexpr (!ignore_flip)
    {
        if (is_room_flipped(x, y))
        {
            constexpr int new_angle = []()
            {
                switch (angle_in_multiples_of_pi_halves)
                {
                case 0:
                    return 2;
                case 2:
                    return 0;
                case 1:
                    return -1;
                case -1:
                    return 1;
                }
            }();
            g_spawn_eggsac<-offset_x, offset_y, new_angle, true>(self, x, y, layer);
            return;
        }
    }

    auto do_spawn = [=]()
    {
        Entity* eggsac = layer->spawn_entity(self.entity_id, x, y, false, 0.0f, 0.0f, true);
        eggsac->angle = angle_in_multiples_of_pi_halves * std::numbers::pi_v<float> / 2.0f;
        return eggsac;
    };

    const float nx = x + static_cast<float>(offset_x);
    const float ny = y + static_cast<float>(offset_y);
    if constexpr (offset_x < 0 || offset_y > 0)
    {
        if (Entity* neighbour = layer->get_grid_entity_at(nx, ny))
        {
            Entity* eggsac = do_spawn();
            attach_entity(neighbour, eggsac);
        }
    }
    else
    {
        Entity* eggsac = do_spawn();
        g_floor_requiring_entities.push_back({{{nx, ny}}, eggsac->uid});
    }
};
template <int offset_x, int offset_y, bool ignore_flip = false>
void g_spawn_punishball_attach(const CommunityTileCode& self, float x, float y, Layer* layer)
{
    if constexpr (!ignore_flip)
    {
        if (is_room_flipped(x, y))
        {
            g_spawn_punishball_attach<-offset_x, offset_y, true>(self, x, y, layer);
            return;
        }
    }

    x += static_cast<float>(offset_x);
    y += static_cast<float>(offset_y);
    auto do_spawn = [=]()
    {
        std::vector<uint32_t> entities_neighbour = get_entities_overlapping_by_pointer({}, ENTITY_MASK::ANY, x - 0.5f, y - 0.5f, x + 0.5f, y + 0.5f, layer);
        if (!entities_neighbour.empty())
        {
            get_entity_ptr(attach_ball_and_chain(entities_neighbour.front(), -static_cast<float>(offset_x), -static_cast<float>(offset_y)));
            return;
        }
        layer->spawn_entity(self.entity_id, x, y, false, 0.0f, 0.0f, true);
    };

    if constexpr (offset_x < 0 || offset_y > 0)
    {
        do_spawn();
    }
    else
    {
        g_attachee_requiring_entities.push_back({{{x, y}}, do_spawn});
    }
};
std::array g_community_tile_codes{
    // Wave 1
    CommunityTileCode{
        "totem_trap",
        "ENT_TYPE_FLOOR_TOTEM_TRAP",
        [](const CommunityTileCode& self, float x, float y, Layer* layer)
        {
            Entity* bottom = layer->spawn_entity(self.entity_id, x, y, false, 0.0f, 0.0f, true);
            layer->spawn_entity_over(self.entity_id, bottom, 0.0f, 1.0f);
            g_floor_requiring_entities.push_back({{{x, y - 1.0f}}, bottom->uid});
        },
    },
    CommunityTileCode{
        "lion_trap",
        "ENT_TYPE_FLOOR_LION_TRAP",
        [](const CommunityTileCode& self, float x, float y, Layer* layer)
        {
            Entity* bottom = layer->spawn_entity(self.entity_id, x, y, false, 0.0f, 0.0f, true);
            layer->spawn_entity_over(self.entity_id, bottom, 0.0f, 1.0f);
            g_floor_requiring_entities.push_back({{{x, y - 1.0f}}, bottom->uid});
        },
    },
    CommunityTileCode{"cog_door", "ENT_TYPE_FLOOR_DOOR_COG"},
    CommunityTileCode{"dustwall", "ENT_TYPE_FLOOR_DUSTWALL"},
    CommunityTileCode{"bat", "ENT_TYPE_MONS_BAT", g_spawn_not_snapped_to_floor},
    CommunityTileCode{"bat_flying", "ENT_TYPE_MONS_BAT"},
    CommunityTileCode{"skeleton", "ENT_TYPE_MONS_SKELETON"},
    CommunityTileCode{"redskeleton", "ENT_TYPE_MONS_REDSKELETON"},
    CommunityTileCode{"lizard", "ENT_TYPE_MONS_HORNEDLIZARD"},
    CommunityTileCode{"mole", "ENT_TYPE_MONS_MOLE"},
    CommunityTileCode{"monkey", "ENT_TYPE_MONS_MONKEY"},
    CommunityTileCode{"firebug", "ENT_TYPE_MONS_FIREBUG"},
    CommunityTileCode{"vampire", "ENT_TYPE_MONS_VAMPIRE", g_spawn_not_snapped_to_floor},
    CommunityTileCode{
        "vampire_flying",
        "ENT_TYPE_MONS_VAMPIRE",
        [](const CommunityTileCode& self, float x, float y, Layer* layer)
        {
            if (Movable* vampire = (Movable*)layer->spawn_entity(self.entity_id, x, y, false, 0.0f, 0.0f, true))
            {
                vampire->move_state = 9;
            }
        }},
    CommunityTileCode{
        "vlad_flying",
        "ENT_TYPE_MONS_VLAD",
        [](const CommunityTileCode& self, float x, float y, Layer* layer)
        {
            if (Movable* vlad = (Movable*)layer->spawn_entity(self.entity_id, x, y, false, 0.0f, 0.0f, true))
            {
                vlad->move_state = 9;
            }
        }},
    CommunityTileCode{"osiris", "ENT_TYPE_MONS_OSIRIS_HEAD"},
    CommunityTileCode{"anubis2", "ENT_TYPE_MONS_ANUBIS2"},
    CommunityTileCode{"assassin", "ENT_TYPE_MONS_FEMALE_JIANGSHI"},
    CommunityTileCode{"yeti_king", "ENT_TYPE_MONS_YETIKING"},
    CommunityTileCode{"yeti_queen", "ENT_TYPE_MONS_YETIQUEEN"},
    CommunityTileCode{"bee", "ENT_TYPE_MONS_BEE"},
    CommunityTileCode{"bee_queen", "ENT_TYPE_MONS_QUEENBEE"},
    CommunityTileCode{"frog", "ENT_TYPE_MONS_FROG"},
    CommunityTileCode{"frog_orange", "ENT_TYPE_MONS_FIREFROG"},
    CommunityTileCode{"hundun", "ENT_TYPE_MONS_HUNDUN"},
    CommunityTileCode{"scarab", "ENT_TYPE_MONS_SCARAB"},
    CommunityTileCode{"cosmic_jelly", "ENT_TYPE_MONS_MEGAJELLYFISH"},
    CommunityTileCode{"ghost", "ENT_TYPE_MONS_GHOST", g_spawn_ghost<GHOST_BEHAVIOR::SAD>},
    CommunityTileCode{"ghost_med_sad", "ENT_TYPE_MONS_GHOST_MEDIUM_SAD", g_spawn_ghost<GHOST_BEHAVIOR::MEDIUM_SAD>},
    CommunityTileCode{"ghost_med_happy", "ENT_TYPE_MONS_GHOST_MEDIUM_HAPPY", g_spawn_ghost<GHOST_BEHAVIOR::MEDIUM_HAPPY>},
    CommunityTileCode{"ghost_small_angry", "ENT_TYPE_MONS_GHOST_SMALL_ANGRY", g_spawn_ghost<GHOST_BEHAVIOR::SMALL_ANGRY>},
    CommunityTileCode{"ghost_small_sad", "ENT_TYPE_MONS_GHOST_SMALL_SAD", g_spawn_ghost<GHOST_BEHAVIOR::SMALL_SAD>},
    CommunityTileCode{"ghost_small_surprised", "ENT_TYPE_MONS_GHOST_SMALL_SURPRISED", g_spawn_ghost<GHOST_BEHAVIOR::SMALL_SURPRISED>},
    CommunityTileCode{"ghost_small_happy", "ENT_TYPE_MONS_GHOST_SMALL_HAPPY", g_spawn_ghost<GHOST_BEHAVIOR::SMALL_HAPPY>},
    CommunityTileCode{"leaf", "ENT_TYPE_ITEM_LEAF"},
    CommunityTileCode{"udjat_key", "ENT_TYPE_ITEM_LOCKEDCHEST_KEY"},
    CommunityTileCode{"tutorial_speedrun_sign", "ENT_TYPE_ITEM_SPEEDRUN_SIGN"},
    CommunityTileCode{"tutorial_menu_sign", "ENT_TYPE_ITEM_BASECAMP_TUTORIAL_SIGN"},
    CommunityTileCode{"eggplant", "ENT_TYPE_ITEM_EGGPLANT"},
    CommunityTileCode{"gold_bar", "ENT_TYPE_ITEM_GOLDBAR"},
    CommunityTileCode{"diamond", "ENT_TYPE_ITEM_DIAMOND"},
    CommunityTileCode{"emerald", "ENT_TYPE_ITEM_EMERALD"},
    CommunityTileCode{"sapphire", "ENT_TYPE_ITEM_SAPPHIRE"},
    CommunityTileCode{"ruby", "ENT_TYPE_ITEM_RUBY"},
    CommunityTileCode{"rope", "ENT_TYPE_ITEM_PICKUP_ROPE"},
    CommunityTileCode{"rope_pile", "ENT_TYPE_ITEM_PICKUP_ROPEPILE"},
    CommunityTileCode{"bomb_bag", "ENT_TYPE_ITEM_PICKUP_BOMBBAG"},
    CommunityTileCode{"bomb_box", "ENT_TYPE_ITEM_PICKUP_BOMBBOX"},
    CommunityTileCode{"bomb", "ENT_TYPE_ITEM_BOMB"},
    CommunityTileCode{"giantfood", "ENT_TYPE_ITEM_PICKUP_GIANTFOOD"},
    CommunityTileCode{"elixir", "ENT_TYPE_ITEM_PICKUP_ELIXIR"},
    CommunityTileCode{"seeded_run_unlocker", "ENT_TYPE_ITEM_PICKUP_SEEDEDRUNSUNLOCKER"},
    CommunityTileCode{"specs", "ENT_TYPE_ITEM_PICKUP_SPECTACLES"},
    CommunityTileCode{"climbing_gloves", "ENT_TYPE_ITEM_PICKUP_CLIMBINGGLOVES"},
    CommunityTileCode{"pitchers_mitt", "ENT_TYPE_ITEM_PICKUP_PITCHERSMITT"},
    CommunityTileCode{"shoes_spring", "ENT_TYPE_ITEM_PICKUP_SPRINGSHOES"},
    CommunityTileCode{"shoes_spike", "ENT_TYPE_ITEM_PICKUP_SPIKESHOES"},
    CommunityTileCode{"paste", "ENT_TYPE_ITEM_PICKUP_PASTE"},
    CommunityTileCode{"compass", "ENT_TYPE_ITEM_PICKUP_COMPASS"},
    CommunityTileCode{"compass_alien", "ENT_TYPE_ITEM_PICKUP_SPECIALCOMPASS"},
    CommunityTileCode{"parachute", "ENT_TYPE_ITEM_PICKUP_PARACHUTE"},
    CommunityTileCode{"udjat_eye", "ENT_TYPE_ITEM_PICKUP_UDJATEYE"},
    CommunityTileCode{"kapala", "ENT_TYPE_ITEM_PICKUP_KAPALA"},
    CommunityTileCode{"hedjet", "ENT_TYPE_ITEM_PICKUP_HEDJET"},
    CommunityTileCode{"crown", "ENT_TYPE_ITEM_PICKUP_CROWN"},
    CommunityTileCode{"eggplant_crown", "ENT_TYPE_ITEM_PICKUP_EGGPLANTCROWN"},
    CommunityTileCode{"true_crown", "ENT_TYPE_ITEM_PICKUP_TRUECROWN"},
    CommunityTileCode{"tablet", "ENT_TYPE_ITEM_PICKUP_TABLETOFDESTINY"},
    CommunityTileCode{"bone_key", "ENT_TYPE_ITEM_PICKUP_SKELETON_KEY"},
    CommunityTileCode{"playerbag", "ENT_TYPE_ITEM_PICKUP_PLAYERBAG"},
    CommunityTileCode{"cape", "ENT_TYPE_ITEM_CAPE"},
    CommunityTileCode{"vlads_cape", "ENT_TYPE_ITEM_VLADS_CAPE"},
    CommunityTileCode{"back_jetpack", "ENT_TYPE_ITEM_JETPACK"},
    CommunityTileCode{"back_telepack", "ENT_TYPE_ITEM_TELEPORTER_BACKPACK"},
    CommunityTileCode{"back_hoverpack", "ENT_TYPE_ITEM_HOVERPACK"},
    CommunityTileCode{"back_powerpack", "ENT_TYPE_ITEM_POWERPACK"},
    CommunityTileCode{"gun_webgun", "ENT_TYPE_ITEM_WEBGUN"},
    CommunityTileCode{"gun_shotgun", "ENT_TYPE_ITEM_SHOTGUN"},
    CommunityTileCode{"gun_freezeray", "ENT_TYPE_ITEM_FREEZERAY"},
    CommunityTileCode{"camera", "ENT_TYPE_ITEM_CAMERA"},
    CommunityTileCode{"teleporter", "ENT_TYPE_ITEM_TELEPORTER"},
    CommunityTileCode{"boomerang", "ENT_TYPE_ITEM_BOOMERANG"},
    CommunityTileCode{"machete", "ENT_TYPE_ITEM_MACHETE"},
    CommunityTileCode{"excalibur", "ENT_TYPE_ITEM_EXCALIBUR"},
    CommunityTileCode{"excalibur_broken", "ENT_TYPE_ITEM_BROKENEXCALIBUR"},
    CommunityTileCode{"excalibur_stone_empty", "ENT_TYPE_FLOOR_EXCALIBUR_STONE"},
    CommunityTileCode{"scepter", "ENT_TYPE_ITEM_SCEPTER"},
    CommunityTileCode{"clonegun", "ENT_TYPE_ITEM_CLONEGUN"},
    CommunityTileCode{"shield_wooden", "ENT_TYPE_ITEM_WOODEN_SHIELD"},
    CommunityTileCode{"shield_metal", "ENT_TYPE_ITEM_METAL_SHIELD"},
    CommunityTileCode{"udjat_target", "ENT_TYPE_LOGICAL_BLACKMARKET_DOOR"},
    CommunityTileCode{"mount_rockdog", "ENT_TYPE_MOUNT_ROCKDOG"},
    CommunityTileCode{"mount_axolotl", "ENT_TYPE_MOUNT_AXOLOTL"},
    CommunityTileCode{"mount_qilin", "ENT_TYPE_MOUNT_QILIN"},
    CommunityTileCode{"humphead", "ENT_TYPE_MONS_GIANTFISH"},
    CommunityTileCode{"present", "ENT_TYPE_ITEM_PRESENT"},
    CommunityTileCode{"forcefield_horizontal", "ENT_TYPE_FLOOR_HORIZONTAL_FORCEFIELD"},
    CommunityTileCode{"forcefield_horizontal_top", "ENT_TYPE_FLOOR_HORIZONTAL_FORCEFIELD_TOP"},
    CommunityTileCode{"pet_monty", "ENT_TYPE_MONS_PET_DOG"},
    CommunityTileCode{"pet_percy", "ENT_TYPE_MONS_PET_CAT"},
    CommunityTileCode{"pet_poochi", "ENT_TYPE_MONS_PET_HAMSTER"},
    CommunityTileCode{"rope_unrolled", "ENT_TYPE_ITEM_UNROLLED_ROPE"},
    CommunityTileCode{"cosmic_orb", "ENT_TYPE_ITEM_FLOATING_ORB"},
    CommunityTileCode{"monkey_gold", "ENT_TYPE_MONS_GOLDMONKEY"},
    CommunityTileCode{"altar_duat", "ENT_TYPE_FLOOR_DUAT_ALTAR"},
    CommunityTileCode{"spikeball", "ENT_TYPE_ACTIVEFLOOR_UNCHAINED_SPIKEBALL"},
    CommunityTileCode{"cobweb", "ENT_TYPE_ITEM_WEB", g_spawn_not_snapped_to_floor},
    // Wave 2
    CommunityTileCode{
        "eggsac",
        "ENT_TYPE_ITEM_EGGSAC",
        [](const CommunityTileCode& self, float x, float y, Layer* layer)
        {
            Entity* eggsac = layer->spawn_entity(self.entity_id, x, y, false, 0.0f, 0.0f, true);
            if (Entity* left = layer->get_grid_entity_at(x - 1.0f, y))
            {
                attach_entity(left, eggsac);
                eggsac->angle = -std::numbers::pi_v<float> / 2.0f;
            }
            else if (Entity* top = layer->get_grid_entity_at(x, y + 1.0f))
            {
                attach_entity(top, eggsac);
                eggsac->angle = std::numbers::pi_v<float>;
            }
            else
            {
                g_floor_requiring_entities.push_back({{{x + 1.0f, y, std::numbers::pi_v<float> / 2.0f}, {x, y - 1.0f}}, eggsac->uid});
            }
        },
    },
    CommunityTileCode{"eggsac_left", "ENT_TYPE_ITEM_EGGSAC", g_spawn_eggsac<-1, 0, -1>},
    CommunityTileCode{"eggsac_top", "ENT_TYPE_ITEM_EGGSAC", g_spawn_eggsac<0, 1, 2>},
    CommunityTileCode{"eggsac_right", "ENT_TYPE_ITEM_EGGSAC", g_spawn_eggsac<1, 0, 1>},
    CommunityTileCode{"eggsac_bottom", "ENT_TYPE_ITEM_EGGSAC", g_spawn_eggsac<0, -1, 0>},
    CommunityTileCode{"grub", "ENT_TYPE_MONS_GRUB"},
    CommunityTileCode{"spider", "ENT_TYPE_MONS_SPIDER", g_spawn_not_snapped_to_floor},
    CommunityTileCode{"spider_falling", "ENT_TYPE_MONS_SPIDER"},
    CommunityTileCode{
        "spider_hanging",
        "ENT_TYPE_MONS_HANGSPIDER",
        [](const CommunityTileCode& self, float x, float y, Layer* layer)
        {
            static const auto web_id = to_id("ENT_TYPE_ITEM_WEB");
            static const auto anchor_id = to_id("ENT_TYPE_ITEM_HANGANCHOR");
            static const auto strand_id = to_id("ENT_TYPE_ITEM_HANGSTRAND");

            Entity* spider = layer->spawn_entity(self.entity_id, x, y, false, 0.0f, 0.0f, true);
            layer->spawn_entity_over(strand_id, spider, 0.0f, 0.0f);

            Entity* web = layer->spawn_entity(web_id, x, y, false, 0.0f, 0.0f, true);
            HangAnchor* anchor = layer->spawn_entity_over(anchor_id, web, 0.0f, 0.0f)->as<HangAnchor>();
            anchor->spider_uid = spider->uid;
        },
    },
    CommunityTileCode{"skull_drop_trap", "ENT_TYPE_ITEM_SKULLDROPTRAP"},
    CommunityTileCode{"lava_pot", "ENT_TYPE_ITEM_LAVAPOT"},
    CommunityTileCode{"proto_shopkeeper", "ENT_TYPE_MONS_PROTOSHOPKEEPER"},
    CommunityTileCode{"shopkeeper_clone", "ENT_TYPE_MONS_SHOPKEEPERCLONE"},
    CommunityTileCode{"tadpole", "ENT_TYPE_MONS_TADPOLE"},
    CommunityTileCode{"ghist_present", "ENT_TYPE_ITEM_GHIST_PRESENT"},
    CommunityTileCode{
        "palace_sign",
        "ENT_TYPE_DECORATION_PALACE_SIGN",
        [](const CommunityTileCode& self, float x, float y, Layer* layer)
        {
            static const auto floor_id = to_id("ENT_TYPE_FLOOR_GENERIC");

            Entity* floor = layer->spawn_entity(floor_id, x, y, false, 0.0f, 0.0f, true);
            layer->spawn_entity_over(self.entity_id, floor, 0.0f, 0.0f);
        },
    },
    CommunityTileCode{"critter_dungbeetle", "ENT_TYPE_MONS_CRITTERDUNGBEETLE"},
    CommunityTileCode{"critter_butterfly", "ENT_TYPE_MONS_CRITTERBUTTERFLY"},
    CommunityTileCode{"critter_snail", "ENT_TYPE_MONS_CRITTERSNAIL"},
    CommunityTileCode{"critter_fish", "ENT_TYPE_MONS_CRITTERFISH"},
    CommunityTileCode{"critter_crab", "ENT_TYPE_MONS_CRITTERCRAB"},
    CommunityTileCode{"critter_locust", "ENT_TYPE_MONS_CRITTERLOCUST"},
    CommunityTileCode{"critter_penguin", "ENT_TYPE_MONS_CRITTERPENGUIN"},
    CommunityTileCode{"critter_firefly", "ENT_TYPE_MONS_CRITTERFIREFLY"},
    CommunityTileCode{"critter_drone", "ENT_TYPE_MONS_CRITTERDRONE"},
    CommunityTileCode{"bubble_platform", "ENT_TYPE_ACTIVEFLOOR_BUBBLE_PLATFORM"},
    CommunityTileCode{"punishball", "ENT_TYPE_ITEM_PUNISHBALL"},
    CommunityTileCode{"punishball_attach", "ENT_TYPE_ITEM_PUNISHBALL", g_spawn_punishball_attach<-1, 0>},
    CommunityTileCode{"giant_fly", "ENT_TYPE_MONS_GIANTFLY"},
    CommunityTileCode{"flying_fish", "ENT_TYPE_MONS_FISH"},
    CommunityTileCode{"crabman", "ENT_TYPE_MONS_CRABMAN"},
    CommunityTileCode{
        "slidingwall",
        "ENT_TYPE_ACTIVEFLOOR_SLIDINGWALL",
        [](const CommunityTileCode& self, float x, float y, Layer* layer)
        {
            Entity* slidingwall = layer->spawn_entity(self.entity_id, x, y, false, 0.0f, 0.0f, true);
            // hook the function that dereferences the top part of the trap (which is nullptr right now)
            hook_vtable<void(Entity*, Entity*), 26>(
                slidingwall, [](Entity*, Entity*, void (*)(Entity*, Entity*)) {});
        },
    },
    CommunityTileCode{"spikeball_trap", "ENT_TYPE_FLOOR_SPIKEBALL_CEILING"},
    CommunityTileCode{
        "spikeball_no_bounce",
        "ENT_TYPE_ACTIVEFLOOR_UNCHAINED_SPIKEBALL",
        [](const CommunityTileCode& self, float x, float y, Layer* layer)
        {
            UnchainedSpikeBall* spikeball = layer->spawn_entity(self.entity_id, x, y, false, 0.0f, 0.0f, true)->as<UnchainedSpikeBall>();
            spikeball->bounce = true;
        },
    },
    CommunityTileCode{"boulder", "ENT_TYPE_ACTIVEFLOOR_BOULDER"},
    CommunityTileCode{"apep", "ENT_TYPE_MONS_APEP_HEAD"},
    CommunityTileCode{"apep_left", "ENT_TYPE_MONS_APEP_HEAD", []([[maybe_unused]] const CommunityTileCode& self, float x, float y, Layer* layer)
                      {
                          layer->spawn_apep(x, y, is_room_flipped(x, y));
                      }},
    CommunityTileCode{"apep_right", "ENT_TYPE_MONS_APEP_HEAD", []([[maybe_unused]] const CommunityTileCode& self, float x, float y, Layer* layer)
                      {
                          layer->spawn_apep(x, y, !is_room_flipped(x, y));
                      }},
    CommunityTileCode{"olmite_naked", "ENT_TYPE_MONS_OLMITE_NAKED"},
    CommunityTileCode{"olmite_helmet", "ENT_TYPE_MONS_OLMITE_HELMET"},
    CommunityTileCode{
        "olmite_armored",
        "ENT_TYPE_MONS_OLMITE_BODYARMORED",
        [](const CommunityTileCode& self, float x, float y, Layer* layer)
        {
            static const auto helmet_id = to_id("ENT_TYPE_MONS_OLMITE_HELMET");
            static const auto naked_id = to_id("ENT_TYPE_MONS_OLMITE_NAKED");

            Olmite* olmite = layer->spawn_entity_snap_to_floor(self.entity_id, x, y)->as<Olmite>();

            std::vector<uint32_t> entities_above = get_entities_overlapping_by_pointer({}, ENTITY_MASK::MONSTER, x - 0.1f, y + 0.9f, x + 0.1f, y + 1.1f, layer);
            for (uint32_t uid : entities_above)
            {
                if (Entity* ent = get_entity_ptr(uid))
                {
                    if (ent->type->id == helmet_id || ent->type->id == naked_id || ent->type->id == self.entity_id)
                    {
                        ent->as<Olmite>()->in_stack = true;
                        olmite->in_stack = true;
                        olmite->on_top_uid = ent->uid;

                        static constexpr float offset[]{0.0f, 0.64f};
                        stack_entities(olmite->uid, ent->uid, offset);
                    }
                }
            }
        },
    },
    // Wave 3
    CommunityTileCode{"punishball_attach_left", "ENT_TYPE_ITEM_PUNISHBALL", g_spawn_punishball_attach<-1, 0>},
    CommunityTileCode{"punishball_attach_right", "ENT_TYPE_ITEM_PUNISHBALL", g_spawn_punishball_attach<1, 0>},
    CommunityTileCode{"punishball_attach_top", "ENT_TYPE_ITEM_PUNISHBALL", g_spawn_punishball_attach<0, 1>},
    CommunityTileCode{"punishball_attach_bottom", "ENT_TYPE_ITEM_PUNISHBALL", g_spawn_punishball_attach<0, -1>},
    CommunityTileCode{"critter_slime", "ENT_TYPE_MONS_CRITTERSLIME"},
    CommunityTileCode{"skull", "ENT_TYPE_ITEM_SKULL"},
    CommunityTileCode{"venom", "ENT_TYPE_ITEM_ACIDSPIT", [](const CommunityTileCode& self, float x, float y, Layer* layer)
                      {
                          layer->spawn_entity(self.entity_id, x, y - 1, false, 0, 0, false);
                      }},
    CommunityTileCode{"arrow_wooden", "ENT_TYPE_ITEM_WOODEN_ARROW"},
    CommunityTileCode{"arrow_metal", "ENT_TYPE_ITEM_METAL_ARROW"},
    CommunityTileCode{"arrow_wooden_poison", "ENT_TYPE_ITEM_WOODEN_ARROW", [](const CommunityTileCode& self, float x, float y, Layer* layer)
                      {
                          Arrow* arrow = layer->spawn_entity_snap_to_floor(self.entity_id, x, y)->as<Arrow>();
                          arrow->poison_arrow(true);
                      }},
    CommunityTileCode{"arrow_metal_poison", "ENT_TYPE_ITEM_METAL_ARROW", [](const CommunityTileCode& self, float x, float y, Layer* layer)
                      {
                          Arrow* arrow = layer->spawn_entity_snap_to_floor(self.entity_id, x, y)->as<Arrow>();
                          arrow->poison_arrow(true);
                      }},
    CommunityTileCode{"movable_spikes", "ENT_TYPE_ITEM_SPIKES", [](const CommunityTileCode& self, float x, float y, Layer* layer)
                      {
                          auto do_spawn = [=]()
                          {
                              std::vector<uint32_t> entities_neighbour = get_entities_overlapping_by_pointer({}, ENTITY_MASK::ANY, x - 0.5f, y - 1.5f, x + 0.5f, y - 0.5f, layer);
                              if (!entities_neighbour.empty())
                              {
                                  layer->spawn_entity_over(self.entity_id, get_entity_ptr(entities_neighbour.front()), 0.0f, 1.0f);
                              }
                          };
                          g_attachee_requiring_entities.push_back({{{x, y - 1}}, do_spawn});
                      }},
    CommunityTileCode{"boombox", "ENT_TYPE_ITEM_BOOMBOX"},
    // CommunityTileCode{
    //    "lake_imposter",
    //    "ENT_TYPE_LIQUID_IMPOSTOR_LAKE",
    //    [](const CommunityTileCode& self, float x, float y, [[maybe_unused]] Layer* layer)
    //    {
    //        layer->spawn_entity(self.entity_id, x, y, false, 0, 0, true);
    //    },
    //},
};

struct CommunityChance;

using ChanceFunc = void(const CommunityChance& self, float x, float y, Layer* layer);
using ChanceValidPlacementFunc = bool(const CommunityChance& self, float x, float y, Layer* layer);

auto g_MaskTestFunc = [](float x, float y, Layer* layer, ENTITY_MASK flags)
{
    if (!layer->get_grid_entity_at(x, y))
    {
        if (!layer->get_entity_at(x, y - 0.229f, flags, 0, 0, 0))
            return true;
    }
    return false;
};

auto g_SolidTestFunc = [](float x, float y, Layer* layer)
{
    if (Entity* floor = layer->get_grid_entity_at(x, y))
    {
        return (floor->flags & (1 << 2)) != 0; // Solid
    }
    return false;
};

auto g_SafeTestFunc = [](float x, float y, Layer* layer)
{
    if (Entity* floor = layer->get_grid_entity_at(x, y))
    {
        if ((floor->flags & (1 << 1)) != 0)
        {
            if ((floor->flags & (1 << 5)) == 0)
            {
                return false;
            }
        }
    }
    return true;
};
ENUM_CLASS_FLAGS(POS_TYPE);

auto g_PositionTestFunc = [](float x, float y, Layer* layer, POS_TYPE flags)
{
    ENTITY_MASK default_mask = ENTITY_MASK::LIQUID | ENTITY_MASK::FLOOR | ENTITY_MASK::ACTIVEFLOOR;
    ENTITY_MASK empty_mask = ENTITY_MASK::LIQUID | ENTITY_MASK::FLOOR | ENTITY_MASK::PLAYER | ENTITY_MASK::MOUNT | ENTITY_MASK::MONSTER |
                             ENTITY_MASK::ITEM | ENTITY_MASK::ACTIVEFLOOR | ENTITY_MASK::ROPE | ENTITY_MASK::EXPLOSION;

    if ((flags & POS_TYPE::DEFAULT) == POS_TYPE::DEFAULT)
        flags = POS_TYPE::FLOOR | POS_TYPE::SAFE | POS_TYPE::EMPTY;

    if ((flags & POS_TYPE::WATER) == POS_TYPE::WATER || (flags & POS_TYPE::LAVA) == POS_TYPE::LAVA)
    {
        default_mask = default_mask ^ ENTITY_MASK::LIQUID;
        empty_mask = empty_mask ^ ENTITY_MASK::LIQUID;
    }

    if ((flags & POS_TYPE::FLOOR) == POS_TYPE::FLOOR)
    {
        if (!g_MaskTestFunc(x, y, layer, default_mask) || !g_SolidTestFunc(x, y - 1.0f, layer))
            return false;
    }
    if ((flags & POS_TYPE::CEILING) == POS_TYPE::CEILING)
    {
        if (!g_MaskTestFunc(x, y, layer, default_mask) || !g_SolidTestFunc(x, y + 1.0f, layer))
            return false;
    }
    if ((flags & POS_TYPE::AIR) == POS_TYPE::AIR)
    {
        if (!g_MaskTestFunc(x, y, layer, default_mask))
            return false;
    }
    if ((flags & POS_TYPE::WALL) == POS_TYPE::WALL)
    {
        if (!g_MaskTestFunc(x, y, layer, default_mask) || (!g_SolidTestFunc(x + 1.0f, y, layer) && !g_SolidTestFunc(x - 1.0f, y, layer)))
            return false;
    }
    if ((flags & POS_TYPE::WALL_LEFT) == POS_TYPE::WALL_LEFT)
    {
        if (!g_MaskTestFunc(x, y, layer, default_mask) || !g_SolidTestFunc(x - 1.0f, y, layer))
            return false;
    }
    if ((flags & POS_TYPE::WALL_RIGHT) == POS_TYPE::WALL_RIGHT)
    {
        if (!g_MaskTestFunc(x, y, layer, default_mask) || !g_SolidTestFunc(x + 1.0f, y, layer))
            return false;
    }
    if ((flags & POS_TYPE::ALCOVE) == POS_TYPE::ALCOVE)
    {
        if (!g_MaskTestFunc(x, y, layer, default_mask) || !g_SolidTestFunc(x, y + 1.0f, layer) || !g_SolidTestFunc(x, y - 1.0f, layer) || (g_SolidTestFunc(x + 1.0f, y, layer) == g_SolidTestFunc(x - 1.0f, y, layer)))
            return false;
    }
    if ((flags & POS_TYPE::PIT) == POS_TYPE::PIT)
    {
        if (!g_MaskTestFunc(x, y, layer, default_mask) || !g_SolidTestFunc(x - 1.0f, y, layer) || !g_SolidTestFunc(x + 1.0f, y, layer) || !g_SolidTestFunc(x, y - 1.0f, layer) || g_SolidTestFunc(x, y + 1.0f, layer))
            return false;
    }
    if ((flags & POS_TYPE::HOLE) == POS_TYPE::HOLE)
    {
        if (!g_MaskTestFunc(x, y, layer, default_mask) || !g_SolidTestFunc(x - 1.0f, y, layer) || !g_SolidTestFunc(x + 1.0f, y, layer) || !g_SolidTestFunc(x, y - 1.0f, layer) || !g_SolidTestFunc(x, y + 1.0f, layer))
            return false;
    }
    if ((flags & POS_TYPE::WATER) == POS_TYPE::WATER)
    {
        if (!g_MaskTestFunc(x, y, layer, ENTITY_MASK::FLOOR | ENTITY_MASK::ACTIVEFLOOR) || g_MaskTestFunc(x, y, layer, ENTITY_MASK::WATER))
            return false;
    }
    if ((flags & POS_TYPE::LAVA) == POS_TYPE::LAVA)
    {
        if (!g_MaskTestFunc(x, y, layer, ENTITY_MASK::FLOOR | ENTITY_MASK::ACTIVEFLOOR) || g_MaskTestFunc(x, y, layer, ENTITY_MASK::LAVA))
            return false;
    }
    auto box = AABB(x - 0.49f, y + 0.49f, x + 0.49f, y - 0.49f);
    auto layer_num = (LAYER)(layer->is_back_layer ? 1 : 0);
    if ((flags & POS_TYPE::SAFE) == POS_TYPE::SAFE)
    {
        if (!g_MaskTestFunc(x, y, layer, ENTITY_MASK::FLOOR | ENTITY_MASK::ACTIVEFLOOR | ENTITY_MASK::WATER) || !g_SafeTestFunc(x - 1.0f, y, layer) || !g_SafeTestFunc(x + 1.0f, y, layer) || !g_SafeTestFunc(x, y - 1.0f, layer) || !g_SafeTestFunc(x, y + 1.0f, layer))
            return false;
        if (is_inside_active_shop_room(x, y, layer_num))
            return false;
    }
    if ((flags & POS_TYPE::EMPTY) == POS_TYPE::EMPTY)
    {
        if (get_entities_overlapping_hitbox(0, empty_mask, box, layer_num).size() > 0)
            return false;
    }
    if ((flags & POS_TYPE::SOLID) == POS_TYPE::SOLID)
    {
        if (!layer->get_entity_at(x, y, ENTITY_MASK::FLOOR | ENTITY_MASK::ACTIVEFLOOR, 0, 0, 0))
            return false;
        if (Entity* floor = layer->get_grid_entity_at(x, y))
            return (floor->flags & (1 << 2)) != 0;
    }
    return true;
};

auto g_DefaultTestFunc = [](float x, float y, Layer* layer)
{
    return g_PositionTestFunc(x, y, layer, POS_TYPE::FLOOR | POS_TYPE::SAFE | POS_TYPE::EMPTY);
};

struct CommunityChance
{
    std::string_view chance;
    std::string_view entity_type;
    ChanceFunc* spawn_func = [](const CommunityChance& self, float x, float y, Layer* layer)
    {
        layer->spawn_entity_snap_to_floor(self.entity_id, x, y);
    };
    ChanceValidPlacementFunc* test_func = []([[maybe_unused]] const CommunityChance& self, float x, float y, Layer* layer)
    {
        return g_DefaultTestFunc(x, y, layer);
    };
    std::uint32_t entity_id;
    std::uint32_t chance_id;
};
std::array g_community_chances{
    // wave 1
    CommunityChance{"red_skeleton", "ENT_TYPE_MONS_REDSKELETON"},
    CommunityChance{
        "walltorch",
        "ENT_TYPE_ITEM_WALLTORCH",
        [](const CommunityChance& self, float x, float y, Layer* layer)
        {
            layer->spawn_entity(self.entity_id, x, y, false, 0.0f, 0.0f, false);
        },
        []([[maybe_unused]] const CommunityChance& self, float x, float y, Layer* layer)
        {
            return !layer->get_grid_entity_at(x, y);
        }},
    CommunityChance{
        "litwalltorch",
        "ENT_TYPE_ITEM_LITWALLTORCH",
        [](const CommunityChance& self, float x, float y, Layer* layer)
        {
            layer->spawn_entity(self.entity_id, x, y, false, 0.0f, 0.0f, false);
        },
        []([[maybe_unused]] const CommunityChance& self, float x, float y, Layer* layer)
        {
            return !layer->get_grid_entity_at(x, y);
        }},
    CommunityChance{"elevator", "ENT_TYPE_ACTIVEFLOOR_ELEVATOR"},
    CommunityChance{"add_gold_bar", "ENT_TYPE_ITEM_GOLDBAR"},
    CommunityChance{"add_gold_bars", "ENT_TYPE_ITEM_GOLDBARS"},
    CommunityChance{"diamond", "ENT_TYPE_ITEM_DIAMOND"},
    CommunityChance{"emerald", "ENT_TYPE_ITEM_EMERALD"},
    CommunityChance{"sapphire", "ENT_TYPE_ITEM_SAPPHIRE"},
    CommunityChance{"ruby", "ENT_TYPE_ITEM_RUBY"},
};
// List of alternative names for existing game and community tile codes.
// Map of new_name: existing_name
std::unordered_map<std::string_view, std::string_view> g_community_tile_code_aliases{
    {"skeleton_key", "bone_key"},
    {"udjat_chest", "lockedchest"},
    {"quillback", "cavemanboss"},
    {"furniture_chair_looking_left", "chair_looking_left"},
    {"furniture_chair_looking_right", "chair_looking_right"},
    {"furniture_construction_sign", "construction_sign"},
    {"furniture_diningtable", "diningtable"},
    {"furniture_dresser", "dresser"},
    {"furniture_sidetable", "sidetable"},
    {"furniture_singlebed", "singlebed"},
    {"thin_ice", "thinice"},
    {"sparrow", "thief"},
    {"proto_generator", "alien_generator"},
    {"hundun_spikes", "crushing_elevator"},
    {"eggplup", "jumpdog"},
    {"tun", "merchant"},
    {"van_horsing", "oldhunter"},
    {"pangxie", "crabman"},
    {"forcefield_timed", "timed_forcefield"},
    {"powder_keg_timed", "timed_powder_keg"},
    {"spikes_upsidedown", "upsidedown_spikes"},
};

struct ChanceLogicProviderImpl
{
    std::uint32_t id;
    std::uint32_t chance_id;
    SpawnLogicProvider provider;
};
std::mutex g_chance_logic_providers_lock;
std::uint32_t g_current_chance_logic_provider_id{0};
std::vector<ChanceLogicProviderImpl> g_chance_logic_providers;

struct ExtraSpawnLogicProviderImpl
{
    std::uint32_t extra_spawn_id;
    std::uint32_t num_extra_spawns_frontlayer;
    std::uint32_t num_extra_spawns_backlayer;
    SpawnLogicProvider provider;

    union
    {
        std::uint32_t transient_num_remaining_spawns[2];
        struct
        {
            std::uint32_t transient_num_remaining_spawns_frontlayer;
            std::uint32_t transient_num_remaining_spawns_backlayer;
        };
    };
    std::vector<Vec2> transient_valid_positions;
};
std::mutex g_extra_spawn_logic_providers_lock;
std::uint32_t g_current_extra_spawn_id{0};
std::vector<ExtraSpawnLogicProviderImpl> g_extra_spawn_logic_providers;

std::vector<std::pair<uint16_t, RoomTemplateType>> g_room_template_types;
std::vector<std::pair<uint16_t, std::pair<uint32_t, uint32_t>>> g_room_template_sizes;

// Used for making custom machine rooms work
std::optional<uint16_t> g_overridden_room_templates[2];

// Used for making custom any room template work
LevelGenRooms g_CustomRoomShims[2];

// Used for making per-room shop types possible
std::array<std::array<std::optional<SHOP_TYPE>, 16>, 8> g_CustomShopTypes[2]{};

// Some select room templates
enum class RoomTemplate : uint16_t
{
    Entrance = 5,
    Exit = 7,
    Cache = 14,
    GhistRoom = 15,
    ChunkGround = 16,
    ChunkAir = 17,
    ChunkDoor = 18,
    Shop = 65,
    MachineBigroomPath = 102,
    FeelingTomb = 106,
    MachineWideroomPath = 107,
    MachineWideroomSide = 108,
    MachineTallroomPath = 109,
    CoffinFrog = 111,
};
constexpr uint16_t to_uint(RoomTemplate room_template) noexcept
{
    return static_cast<uint16_t>(room_template);
}

// Stores manually created rooms for the duration of level gen
struct ManualRoomData
{
    std::string room_data;
    RoomData template_data;
};
std::vector<std::unique_ptr<ManualRoomData>> g_manual_room_datas;

bool g_replace_level_loads{false};
std::vector<std::string> g_levels_to_load;

using LevelGenFun = void(LevelGenSystem*, float, size_t);
LevelGenFun* g_level_gen_trampoline{nullptr};
void level_gen(LevelGenSystem* level_gen_sys, float param_2, size_t param_3)
{
    push_spawn_type_flags(SPAWN_TYPE_LEVEL_GEN_GENERAL);
    OnScopeExit pop{[]
                    { pop_spawn_type_flags(SPAWN_TYPE_LEVEL_GEN_GENERAL); }};

    g_manual_room_datas.clear();

    g_CustomRoomShims[0] = {};
    g_CustomRoomShims[1] = {};

    g_CustomShopTypes[0] = {};
    g_CustomShopTypes[1] = {};

    if (!pre_event(ON::PRE_LEVEL_GENERATION))
        g_level_gen_trampoline(level_gen_sys, param_2, param_3);
    post_level_generation();

    for (size_t i = 0; i < sizeof(LevelGenRooms) / sizeof(uint16_t); i++)
    {
        for (size_t j = 0; j < 2; j++)
        {
            const uint16_t shimmed_room_template = g_CustomRoomShims[j].rooms[i];
            if (shimmed_room_template != 0)
            {
                level_gen_sys->rooms[j]->rooms[i] = level_gen_sys->data->get_pretend_room_template(shimmed_room_template);
            }
        }
    }

    g_replace_level_loads = false;
    g_levels_to_load.clear();
}

using LoadScreenFun = void(StateMemory*, size_t, size_t);
LoadScreenFun* g_load_screen_trampoline{nullptr};
void load_screen(StateMemory* state, size_t param_2, size_t param_3)
{
    if (pre_load_screen())
        return;
    g_load_screen_trampoline(state, param_2, param_3);
    post_load_screen();
}

using UnloadLayerFun = void(Layer*);
UnloadLayerFun* g_unload_layer_trampoline{nullptr};
void unload_layer(Layer* layer)
{
    if (!layer->is_back_layer && pre_unload_level())
        return;
    if (pre_unload_layer((LAYER)layer->is_back_layer))
        return;
    g_unload_layer_trampoline(layer);
    post_unload_layer((LAYER)layer->is_back_layer);
    if (layer->is_back_layer)
        post_event(ON::POST_LEVEL_DESTRUCTION);
}

using InitLayerFun = void(Layer*);
InitLayerFun* g_init_layer_trampoline{nullptr};
void load_layer(Layer* layer)
{
    if (!layer->is_back_layer)
        pre_init_level();
    pre_init_layer((LAYER)layer->is_back_layer);
    g_init_layer_trampoline(layer);
    post_init_layer((LAYER)layer->is_back_layer);
    if (layer->is_back_layer)
        post_event(ON::POST_LEVEL_CREATION);
}

using HandleTileCodeFun = void(LevelGenSystem*, std::uint32_t, std::uint64_t, float, float, std::uint8_t);
HandleTileCodeFun* g_handle_tile_code_trampoline{nullptr};
void handle_tile_code(LevelGenSystem* self, std::uint32_t tile_code, std::uint16_t room_template, float x, float y, std::uint8_t layer)
{
    push_spawn_type_flags(SPAWN_TYPE_LEVEL_GEN_TILE_CODE);
    OnScopeExit pop{[]
                    { pop_spawn_type_flags(SPAWN_TYPE_LEVEL_GEN_TILE_CODE); }};

    std::string_view tile_code_name = g_tile_code_id_to_name[tile_code];

    {
        const bool block_spawn = pre_tile_code_spawn(tile_code_name, x, y, layer, room_template);
        if (block_spawn)
        {
            tile_code = g_last_tile_code_id;
        }
    }

    if (g_community_tile_code_aliases.find(tile_code_name) != g_community_tile_code_aliases.end())
    {
        std::string_view tile_code_alias_name = g_community_tile_code_aliases[tile_code_name];
        std::uint32_t tile_code_alias = g_name_to_tile_code_id[tile_code_alias_name];
        tile_code = tile_code_alias;
    }

    if (tile_code > g_last_tile_code_id && tile_code < g_last_community_tile_code_id)
    {
        auto* layer_ptr = HeapBase::get().state()->layers[layer];
        const CommunityTileCode& community_tile_code = g_community_tile_codes[tile_code - g_last_tile_code_id - 1];
        community_tile_code.func(community_tile_code, x, y, layer_ptr);
    }
    else
    {
        const uint16_t pretend_room_template = self->data->get_pretend_room_template(room_template);

        const auto [ix, iy] = self->get_room_index(x, y);
        const int32_t flat_room_idx = ix + iy * 8;
        if (pretend_room_template != room_template)
        {
            self->rooms_frontlayer->rooms[flat_room_idx] = pretend_room_template;
        }

        g_handle_tile_code_trampoline(self, tile_code, pretend_room_template, x, y, layer);

        if (pretend_room_template != room_template)
        {
            self->rooms_frontlayer->rooms[flat_room_idx] = room_template;
        }
    }

    post_tile_code_spawn(tile_code_name, x, y, layer, room_template);

    if (!g_floor_requiring_entities.empty())
    {
        Entity* floor{nullptr};
        auto state = HeapBase::get().state();
        for (auto& pending_entity : g_floor_requiring_entities)
        {
            for (const auto& pos : pending_entity.pos)
            {
                if (std::abs(pos.x - x) < 0.01f && std::abs(pos.y - y) < 0.01f)
                {
                    if (auto* entity = get_entity_ptr(pending_entity.uid))
                    {
                        if (floor == nullptr)
                        {
                            floor = state->layers[layer]->get_grid_entity_at(x, y);
                        }

                        if (floor != nullptr)
                        {
                            attach_entity(floor, entity);
                            if (pos.angle)
                            {
                                entity->angle = pos.angle.value();
                            }
                            pending_entity.handled = true;
                            break;
                        }
                    }
                }
            }
        }

        std::erase_if(g_floor_requiring_entities, [](const FloorRequiringEntity& ent)
                      { return ent.handled || get_entity_ptr(ent.uid) == nullptr; });
    }

    if (!g_attachee_requiring_entities.empty())
    {
        for (auto& pending_spawn : g_attachee_requiring_entities)
        {
            for (const auto& pos : pending_spawn.pos)
            {
                if (std::abs(pos.x - x) < 0.01f && std::abs(pos.y - y) < 0.01f)
                {
                    pending_spawn.try_spawn();
                    pending_spawn.handled = true;
                }
            }
        }

        std::erase_if(g_attachee_requiring_entities, [](const PendingEntitySpawn& ent)
                      { return ent.handled; });
    }
}

using SetupLevelFiles = void(LevelGenData*, const char*, bool);
SetupLevelFiles* g_setup_level_files_trampoline{nullptr};
void setup_level_files(LevelGenData* level_gen_data, const char* level_file_name, bool load_generic)
{
    pre_load_level_files();
    g_setup_level_files_trampoline(level_gen_data, level_file_name, load_generic);
}

ExecutableMemory g_get_room_size_redirect;
void get_room_size(uint16_t room_template, uint32_t& room_width, uint32_t& room_height)
{
    if (room_template >= to_uint(RoomTemplate::Cache) && room_template <= to_uint(RoomTemplate::GhistRoom))
    {
        room_width = 5;
        room_height = 5;
    }
    else if (room_template >= to_uint(RoomTemplate::ChunkGround) && room_template <= to_uint(RoomTemplate::ChunkAir))
    {
        room_width = 5;
        room_height = 3;
    }
    else if (room_template == to_uint(RoomTemplate::ChunkDoor))
    {
        room_width = 6;
        room_height = 3;
    }
    else if (room_template >= to_uint(RoomTemplate::MachineBigroomPath) && room_template <= to_uint(RoomTemplate::FeelingTomb))
    {
        room_width = 20;
        room_height = 16;
    }
    else if (room_template >= to_uint(RoomTemplate::MachineWideroomPath) && room_template <= to_uint(RoomTemplate::MachineWideroomSide))
    {
        room_width = 20;
        room_height = 8;
    }
    else if (room_template >= to_uint(RoomTemplate::MachineTallroomPath) && room_template <= to_uint(RoomTemplate::CoffinFrog))
    {
        room_width = 10;
        room_height = 16;
    }
    else
    {
        auto it = std::find_if(g_room_template_sizes.begin(), g_room_template_sizes.end(), [room_template](auto& t)
                               { return t.first == room_template; });
        // custom
        if (it != g_room_template_sizes.end())
        {
            std::tie(room_width, room_height) = it->second;
        }
        // default
        else
        {
            room_width = 10;
            room_height = 8;
        }
    }
}
void get_room_size(const char* room_template_name, uint32_t& room_width, uint32_t& room_height)
{
    const auto room_template = HeapBase::get().level_gen()->data->get_room_template(room_template_name);
    if (!room_template)
    {
        DEBUG("Unkown room_template name {}", room_template_name);
    }
    else
    {
        get_room_size(room_template.value(), room_width, room_height);
    }
}

using LoadLevelFile = void(LevelGenData*, const char*);
LoadLevelFile* g_load_level_file_trampoline{nullptr};
void load_level_file(LevelGenData* level_gen_data, const char* level_file_name)
{
    if (!g_levels_to_load.empty())
    {
        for (const std::string& level_file : g_levels_to_load)
        {
            g_load_level_file_trampoline(level_gen_data, level_file.c_str());
        }
        g_levels_to_load.clear();
    }

    if (!g_replace_level_loads)
    {
        g_load_level_file_trampoline(level_gen_data, level_file_name);
    }
}

using DoExtraSpawns = void(ThemeInfo*, std::uint32_t, std::uint32_t, std::uint32_t, std::uint8_t);
DoExtraSpawns* g_do_extra_spawns_trampoline{nullptr};
void do_extra_spawns(ThemeInfo* theme, std::uint32_t border_size, std::uint32_t level_width, std::uint32_t level_height, std::uint8_t layer)
{
    g_do_extra_spawns_trampoline(theme, border_size, level_width, level_height, layer);

    std::lock_guard lock{g_extra_spawn_logic_providers_lock};
    if (!g_extra_spawn_logic_providers.empty())
    {
        for (ExtraSpawnLogicProviderImpl& provider : g_extra_spawn_logic_providers)
        {
            provider.transient_valid_positions.clear();
        }

        for (std::uint32_t ix = border_size; ix != level_width; ix++)
        {
            const float x = static_cast<float>(ix);
            for (std::uint32_t iy = border_size; iy != level_height; iy++)
            {
                const float y = 122.0f - static_cast<float>(iy);
                for (ExtraSpawnLogicProviderImpl& provider : g_extra_spawn_logic_providers)
                {
                    if (provider.transient_num_remaining_spawns[layer] > 0)
                    {
                        if (provider.provider.is_valid(x, y, layer))
                        {
                            provider.transient_valid_positions.push_back({x, y});
                        }
                    }
                }
            }
        }
        PRNG* prng = HeapBase::get().prng();
        for (ExtraSpawnLogicProviderImpl& provider : g_extra_spawn_logic_providers)
        {
            auto& valid_pos = provider.transient_valid_positions;
            while (!valid_pos.empty() && provider.transient_num_remaining_spawns[layer] > 0)
            {
                const auto random_idx = static_cast<std::size_t>(prng->internal_random_index(valid_pos.size(), PRNG::EXTRA_SPAWNS));
                const auto idx = random_idx < valid_pos.size() ? random_idx : 0;
                const auto& [x, y] = valid_pos[idx];
                provider.provider.do_spawn(x, y, layer);

                valid_pos.erase(valid_pos.begin() + idx);
                provider.transient_num_remaining_spawns[layer]--;
            }
        }
    }
}

using GenerateRoom = void(LevelGenSystem*, int32_t, int32_t);
GenerateRoom* g_generate_room_trampoline{nullptr};
void generate_room(LevelGenSystem* level_gen, int32_t room_idx_x, int32_t room_idx_y)
{
    const int32_t flat_room_idx = room_idx_x + room_idx_y * 8;

    const uint16_t room_templates[2]{
        level_gen->rooms[0]->rooms[flat_room_idx],
        level_gen->rooms[1]->rooms[flat_room_idx],
    };

    for (size_t j = 0; j < 2; j++)
    {
        const uint16_t room_template = room_templates[j];
        if (g_overridden_room_templates[j] == std::nullopt)
        {
            if (level_gen->data->get_room_template_type(room_template) == RoomTemplateType::MachineRoom)
            {
                // Revert the room template the next time room data will be collected
                g_overridden_room_templates[j] = room_template;
                // Set it to any machine room for now, this will branch in g_generate_room_trampoline to allow for large levels
                level_gen->rooms[j]->rooms[flat_room_idx] = to_uint(RoomTemplate::MachineBigroomPath);
            }
        }

        {
            const uint16_t pretend_room_template = level_gen->data->get_pretend_room_template(room_template);
            if (pretend_room_template != room_template)
            {
                g_CustomRoomShims[j].rooms[flat_room_idx] = room_template;
            }
        }
    }

    g_generate_room_trampoline(level_gen, room_idx_x, room_idx_y);

    for (size_t j = 0; j < 2; j++)
    {
        if (g_overridden_room_templates[j])
        {
            level_gen->rooms_frontlayer->rooms[flat_room_idx] = g_overridden_room_templates[j].value();
            g_overridden_room_templates[j].reset();
        }
    }
}

using GatherRoomData = void(LevelGenData*, byte, int room_x, int, bool, uint8_t*, uint8_t*, size_t, uint8_t*, uint8_t*, uint8_t*, uint8_t*);
GatherRoomData* g_gather_room_data_trampoline{nullptr};
void gather_room_data(LevelGenData* tile_storage, byte param_2, int room_idx_x, int room_idx_y, bool hard_level, uint8_t* param_6, uint8_t* param_7, size_t param_8, uint8_t* param_9, uint8_t* param_10, uint8_t* out_room_width, uint8_t* out_room_height)
{
    const auto* level_gen = HeapBase::get().level_gen();
    for (size_t j = 0; j < 2; j++)
    {
        if (g_overridden_room_templates[j].has_value())
        {
            const int32_t flat_room_idx = room_idx_x + room_idx_y * 8;
            level_gen->rooms_frontlayer->rooms[flat_room_idx] = g_overridden_room_templates[j].value();
        }
    }
    g_gather_room_data_trampoline(tile_storage, param_2, room_idx_x, room_idx_y, hard_level, param_6, param_7, param_8, param_9, param_10, out_room_width, out_room_height);
}

using GetRandomRoomData = RoomData*(LevelGenData*, uint16_t, bool, bool, bool, int32_t, int32_t);
GetRandomRoomData* g_get_random_room_data_trampoline{nullptr};
RoomData* get_random_room_data(LevelGenData* tile_storage, uint16_t room_template, bool hard_level, bool can_not_have, uint8_t layer, int room_idx_x, int room_idx_y)
{
    std::string room_override = pre_get_random_room(room_idx_x, room_idx_y, layer, room_template);
    if (!room_override.empty())
    {
        room_override = std::string{trim(room_override)};
        std::erase(room_override, '\n');
        std::erase(room_override, '\r');

        uint32_t width, height;
        get_room_size(room_template, width, height);
        if (width * height == room_override.size())
        {
            auto template_data = std::make_unique<ManualRoomData>();
            template_data->room_data = std::move(room_override);
            template_data->template_data = RoomData{
                .room_width = static_cast<uint8_t>(width),
                .room_height = static_cast<uint8_t>(height),
                .room_data = template_data->room_data.c_str()};
            g_manual_room_datas.push_back(std::move(template_data));
            return &g_manual_room_datas.back()->template_data;
        }
    }

    RoomData* room_data = g_get_random_room_data_trampoline(tile_storage, room_template, hard_level, can_not_have, layer, room_idx_x, room_idx_y);
    return room_data;
}

using SpawnRoomFromTileCodes = void(LevelGenData*, int, int, SingleRoomData*, SingleRoomData*, uint16_t, bool, uint16_t);
SpawnRoomFromTileCodes* g_spawn_room_from_tile_codes_trampoline{nullptr};
void spawn_room_from_tile_codes(LevelGenData* level_gen_data, int room_idx_x, int room_idx_y, SingleRoomData* front_room_data, SingleRoomData* back_room_data, uint16_t param_6, bool dual_room, uint16_t room_template)
{
    auto level_gen = HeapBase::get().level_gen();

    std::optional<SHOP_TYPE> before[2];
    for (size_t i = 0; i < 2; i++)
    {
        const std::optional<SHOP_TYPE> custom_type = g_CustomShopTypes[i][room_idx_x][room_idx_y];
        if (custom_type != std::nullopt)
        {
            before[i] = level_gen->shop_type;
            level_gen->shop_types[i] = custom_type.value();
        }
    }

    LevelGenRoomData room_data{};
    std::memcpy(room_data.front_layer.data(), front_room_data, 10 * 8ull);
    if (dual_room)
    {
        room_data.back_layer.emplace();
        std::memcpy(room_data.back_layer.value().data(), front_room_data, 10 * 8ull);
    }
    std::optional<LevelGenRoomData> changed_data = pre_handle_room_tiles(room_data, room_idx_x, room_idx_y, room_template);
    if (changed_data)
    {
        front_room_data = &changed_data->front_layer;
        dual_room = changed_data->back_layer.has_value();
        back_room_data = dual_room ? &changed_data->back_layer.value() : nullptr;
    }

    g_spawn_room_from_tile_codes_trampoline(level_gen_data, room_idx_x, room_idx_y, front_room_data, back_room_data, param_6, dual_room, room_template);

    for (size_t i = 0; i < 2; i++)
    {
        const std::optional<SHOP_TYPE> before_type = before[i];
        if (before_type != std::nullopt)
        {
            level_gen->shop_types[i] = before_type.value();
        }
    }
}

using TestChance = bool(LevelGenData**, std::uint32_t chance_id);
TestChance* g_test_chance{nullptr};

bool handle_chance(SpawnInfo* spawn_info)
{

    const uint8_t layer = 0; // only handles the front layer, backlayer is hardcoded
    auto* layer_ptr = HeapBase::get().state()->layer(layer);
    LevelGenSystem* level_gen = HeapBase::get().level_gen();
    auto level_gen_data = level_gen->data;
    for (const CommunityChance& community_chance : g_community_chances)
    {
        if (level_gen->get_procedural_spawn_chance(community_chance.chance_id) != 0 && community_chance.test_func(community_chance, spawn_info->x, spawn_info->y, layer_ptr))
        {
            if (g_test_chance(&level_gen_data, community_chance.chance_id))
            {
                community_chance.spawn_func(community_chance, spawn_info->x, spawn_info->y, layer_ptr);
                return true;
            }
        }
    }
    {
        std::lock_guard lock{g_chance_logic_providers_lock};
        for (const ChanceLogicProviderImpl& chance_provider : g_chance_logic_providers)
        {
            if (level_gen->get_procedural_spawn_chance(chance_provider.chance_id) != 0 && chance_provider.provider.is_valid(spawn_info->x, spawn_info->y, layer))
            {
                if (g_test_chance(&level_gen_data, chance_provider.chance_id))
                {
                    chance_provider.provider.do_spawn(spawn_info->x, spawn_info->y, layer);
                    return true;
                }
            }
        }
    }
    return false;
}

void LevelGenData::init()
{
    {
        assert(get_room_template("entrance") == to_uint(RoomTemplate::Entrance));
        assert(get_room_template("exit") == to_uint(RoomTemplate::Exit));
        assert(get_room_template("cache") == to_uint(RoomTemplate::Cache));
        assert(get_room_template("ghistroom") == to_uint(RoomTemplate::GhistRoom));
        assert(get_room_template("chunk_air") == to_uint(RoomTemplate::ChunkAir));
        assert(get_room_template("chunk_ground") == to_uint(RoomTemplate::ChunkGround));
        assert(get_room_template("chunk_door") == to_uint(RoomTemplate::ChunkDoor));
        assert(get_room_template("shop") == to_uint(RoomTemplate::Shop));
        assert(get_room_template("machine_bigroom_path") == to_uint(RoomTemplate::MachineBigroomPath));
        assert(get_room_template("feeling_tomb") == to_uint(RoomTemplate::FeelingTomb));
        assert(get_room_template("machine_wideroom_path") == to_uint(RoomTemplate::MachineWideroomPath));
        assert(get_room_template("machine_wideroom_side") == to_uint(RoomTemplate::MachineWideroomSide));
        assert(get_room_template("machine_tallroom_path") == to_uint(RoomTemplate::MachineTallroomPath));
        assert(get_room_template("coffin_frog") == to_uint(RoomTemplate::CoffinFrog));
    }

    {
        g_room_template_types.push_back({to_uint(RoomTemplate::MachineBigroomPath), RoomTemplateType::VanillaMachineRoom});
        g_room_template_types.push_back({to_uint(RoomTemplate::FeelingTomb), RoomTemplateType::VanillaMachineRoom});
        g_room_template_types.push_back({to_uint(RoomTemplate::MachineWideroomPath), RoomTemplateType::VanillaMachineRoom});
        g_room_template_types.push_back({to_uint(RoomTemplate::MachineWideroomSide), RoomTemplateType::VanillaMachineRoom});
        g_room_template_types.push_back({to_uint(RoomTemplate::MachineTallroomPath), RoomTemplateType::VanillaMachineRoom});
        g_room_template_types.push_back({to_uint(RoomTemplate::CoffinFrog), RoomTemplateType::VanillaMachineRoom});
    }

    // Scan tile codes to know what id to start at
    {
        // Getting the last id like this in case the game decides to skip some ids so that last_id != tile_codes.size()
        auto max_id = 0u;
        for (auto& [name, def] : tile_codes)
        {
            max_id = std::max(def.id, max_id);
            g_tile_code_id_to_name[def.id] = name;
            g_name_to_tile_code_id[name] = def.id;
        }

        // The game uses last id to check if the tilecode is valid using a != instead of a <
        // So we can safely use anything larger than last tile id
        g_last_tile_code_id = max_id + 1;
        g_current_tile_code_id = g_last_tile_code_id + 1;
    }

    // Scan chances to know what id to start at
    {
        auto max_id = 0u;

        // Getting the last id like this in case the game decides to skip some ids so that last_id != chances.size()
        for (auto& [name, def] : monster_chances)
        {
            max_id = std::max(def.id, max_id);
            g_monster_chance_id_to_name[def.id] = name;
        }

        // Getting the last id like this in case the game decides to skip some ids so that last_id != chances.size()
        for (auto& [name, def] : trap_chances)
        {
            max_id = std::max(def.id, max_id);
            g_trap_chance_id_to_name[def.id] = name;
        }

        // The game doesn't centrally handle chances so we can use whatever id
        g_current_chance_id = max_id + 1;
    }

    // Add new community tile codes
    for (auto& community_tile_code : g_community_tile_codes)
    {
        // If another module already inserted this we can skip it
        if (!get_tile_code(std::string{community_tile_code.tile_code}).has_value())
        {
            community_tile_code.tile_code_id = define_tile_code(std::string{community_tile_code.tile_code});
            const auto entity_id = to_id(community_tile_code.entity_type);
            if (entity_id < 0)
            {
                community_tile_code.entity_id = to_id("ENT_TYPE_ITEM_BOMB");
            }
            else
            {
                community_tile_code.entity_id = entity_id;
            }
        }
    }

    for (auto& community_tile_code_alias : g_community_tile_code_aliases)
    {
        std::string_view tile_code_name = community_tile_code_alias.first;
        if (!get_tile_code(std::string{tile_code_name}).has_value())
        {
            define_tile_code(std::string{tile_code_name});
        }
    }

    // Add new community chances
    for (auto& community_chance : g_community_chances)
    {
        // If another module already inserted this we can skip it
        if (!get_chance(std::string{community_chance.chance}).has_value())
        {
            community_chance.chance_id = define_chance(std::string{community_chance.chance});
            const auto entity_id = to_id(community_chance.entity_type);
            if (entity_id < 0)
            {
                community_chance.entity_id = to_id("ENT_TYPE_ITEM_BOMB");
            }
            else
            {
                community_chance.entity_id = entity_id;
            }
        }
    }

    // Remember this for fast access later
    g_last_community_tile_code_id = g_current_tile_code_id;
    g_last_community_chance_id = g_current_chance_id;

    {
        if (const size_t get_room_size_begin = get_address("get_room_size_begin"))
        {
            const size_t get_room_size_end = get_address("get_room_size_end");

            const size_t get_room_size_first_jump = get_address("get_room_size_first_jump");
            const size_t get_room_size_second_jump = get_address("get_room_size_second_jump");

            const size_t get_room_size_addr = (size_t) static_cast<void (*)(const char*, uint32_t&, uint32_t&)>(&get_room_size);

            const std::string redirect_code = fmt::format(
                "\x50"                         // PUSH       RAX
                "\x51"                         // PUSH       RCX
                "\x52"                         // PUSH       RDX
                "\x41\x50"                     // PUSH       R8
                "\x41\x51"                     // PUSH       R9
                "\x41\x52"                     // PUSH       R10
                "\x41\x53"                     // PUSH       R11
                "\x41\x53"                     // PUSH       R11
                "\x48\x8d\x8d\xa0\x02\x00\x00" // LEA        RCX, [RBP + 0x2a0] == room_template_name
                "\x48\x8d\x95\x78\x05\x00\x00" // LEA        RDX, [RBP + 0x578] == room_width
                "\x4c\x8d\x85\xe0\x05\x00\x00" // LEA        R8, [RBP + 0x5e0] == room_height
                "\x48\xb8{}"                   // MOV        RAX, &get_room_size
                "\xff\xd0"                     // CALL       RAX
                "\x41\x5b"                     // POP        R11
                "\x41\x5b"                     // POP        R11
                "\x41\x5a"                     // POP        R10
                "\x41\x59"                     // POP        R9
                "\x41\x58"                     // POP        R8
                "\x5a"                         // POP        RDX
                "\x59"                         // POP        RCX
                "\x58"                         // POP        RAX
                                               // Original Code Begin
                "\x44\x8b\xbd\xe4\x05\x00\x00" // MOV        R15D,dword ptr [RBP + 0x5e4]
                "\x41\x83\xe7\x08"             // AND        R15D,0x8
                "\x4c\x8b\xb5\x70\x05\x00\x00" // MOV        R14,qword ptr [RBP + 0x570]
                "\x4d\x85\xf6"                 // TEST       R14,R14
                "\x74\x09"                     // JZ         FIRST_JUMP
                "\x48\x8b\xbd\x60\x05\x00\x00" // MOV        RDI,qword ptr [RBP + 0x560]
                "\xeb\x0c"                     // JMP        SECOND_JUMP
                                               // Original Code End
                /*FIST_JUMP*/ "\x48\xb8{}"     // MOV        RAX, get_room_size_first_jump
                "\xff\xe0"                     // JMP        RAX
                /*SECOND_JUMP*/ "\x48\xb8{}"   // MOV        RAX, get_room_size_second_jump
                "\xff\xe0"sv,                  // JMP        RAX
                to_le_bytes(get_room_size_addr),
                to_le_bytes(get_room_size_first_jump),
                to_le_bytes(get_room_size_second_jump));

            g_get_room_size_redirect = ExecutableMemory{redirect_code};

            std::string code = fmt::format(
                "\x48\xb8{}"  // MOV         RAX, g_get_room_size_redirect.get()
                "\xff\xe0"sv, // JMP         RAX
                to_le_bytes((size_t)g_get_room_size_redirect.get()));

            // Fill with nop, code is not performance-critical either way
            const size_t original_code_size = get_room_size_end - get_room_size_begin;
            code.resize(original_code_size, '\x90');

            write_mem_prot(get_room_size_begin, code, true);
        }

        g_level_gen_trampoline = (LevelGenFun*)get_address("level_gen_entry"sv);
        g_handle_tile_code_trampoline = (HandleTileCodeFun*)get_address("level_gen_handle_tile_code"sv);
        g_setup_level_files_trampoline = (SetupLevelFiles*)get_address("level_gen_setup_level_files"sv);
        g_load_level_file_trampoline = (LoadLevelFile*)get_address("level_gen_load_level_file"sv);
        g_do_extra_spawns_trampoline = (DoExtraSpawns*)get_address("level_gen_do_extra_spawns"sv);
        g_generate_room_trampoline = (GenerateRoom*)get_address("level_gen_generate_room"sv);
        g_gather_room_data_trampoline = (GatherRoomData*)get_address("level_gen_gather_room_data"sv);
        g_get_random_room_data_trampoline = (GetRandomRoomData*)get_address("level_gen_get_random_room_data"sv);
        g_spawn_room_from_tile_codes_trampoline = (SpawnRoomFromTileCodes*)get_address("level_gen_spawn_room_from_tile_codes"sv);

        g_load_screen_trampoline = (LoadScreenFun*)get_address("load_screen_func"sv);
        g_unload_layer_trampoline = (UnloadLayerFun*)get_address("unload_layer"sv);
        g_init_layer_trampoline = (InitLayerFun*)get_address("init_layer"sv);

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());

        DetourAttach((void**)&g_level_gen_trampoline, level_gen);
        DetourAttach((void**)&g_handle_tile_code_trampoline, handle_tile_code);
        DetourAttach((void**)&g_setup_level_files_trampoline, setup_level_files);
        DetourAttach((void**)&g_load_level_file_trampoline, load_level_file);
        DetourAttach((void**)&g_do_extra_spawns_trampoline, do_extra_spawns);
        DetourAttach((void**)&g_generate_room_trampoline, generate_room);
        DetourAttach((void**)&g_gather_room_data_trampoline, gather_room_data);
        DetourAttach((void**)&g_get_random_room_data_trampoline, get_random_room_data);
        DetourAttach((void**)&g_spawn_room_from_tile_codes_trampoline, spawn_room_from_tile_codes);

        DetourAttach((void**)&g_load_screen_trampoline, load_screen);
        DetourAttach((void**)&g_unload_layer_trampoline, unload_layer);
        DetourAttach((void**)&g_init_layer_trampoline, load_layer);

        const LONG error = DetourTransactionCommit();
        if (error != NO_ERROR)
        {
            DEBUG("Failed hooking LevelGenData stuff: {}\n", error);
        }
    }

    g_test_chance = (TestChance*)get_address("level_gen_test_spawn_chance");
}

std::uint32_t LevelGenData::define_tile_code(std::string tile_code)
{
    if (auto existing = get_tile_code(tile_code))
    {
        return existing.value();
    }

    auto [it, success] = tile_codes.emplace(tile_code.c_str(), TileCodeDef{g_current_tile_code_id});
    g_current_tile_code_id++;

    g_tile_code_id_to_name[it->second.id] = it->first;
    g_name_to_tile_code_id[it->first] = it->second.id;
    return it->second.id;
}

std::optional<uint8_t> LevelGenData::define_short_tile_code(ShortTileCodeDef short_tile_code_def)
{
    // Try all printable chars, note that all chars are allowed since we won't need to parse this anymore
    // Might even be allowed to use non-printable chars, TBD
    // Also check existing short tile codes for an exact match
    std::optional<uint8_t> smallest_match;
    for (uint8_t i = 0x20; i < 0x7f; i++)
    {
        if (!short_tile_codes.contains(i))
        {
            if (!smallest_match.has_value())
            {
                smallest_match = i;
            }
        }
        else if (short_tile_codes[i] == short_tile_code_def)
        {
            return i;
        }
    }

    if (smallest_match.has_value())
    {
        short_tile_codes[smallest_match.value()] = short_tile_code_def;
        return smallest_match;
    }

    return std::nullopt;
}

std::pair<const game_string, ChanceDef>& get_or_emplace_chance(game_unordered_map<game_string, ChanceDef>& chances, game_string chance_name)
{
    struct ChanceNode
    {
        void* ptr0;
        void* ptr1;
        std::pair<const game_string, ChanceDef> value;
    };
    using EmplaceChance = ChanceNode**(void*, std::pair<ChanceNode*, bool>*, game_string&);
    static EmplaceChance* emplace_level_chance = (EmplaceChance*)get_address("level_gen_emplace_chance");

    std::pair<ChanceNode*, bool> node;
    emplace_level_chance((void*)&chances, &node, chance_name);

    return node.first->value;
}

std::uint32_t LevelGenData::define_chance(std::string chance)
{
    if (auto existing = get_chance(chance))
    {
        return existing.value();
    }

    // We use only monster chances to define new stuff, keep an eye out for whether this is dangerous
    auto& [chance_str, this_chance] = get_or_emplace_chance(monster_chances, chance.c_str());
    this_chance.id = g_current_chance_id;
    g_current_chance_id++;

    g_monster_chance_id_to_name[this_chance.id] = chance_str;
    return this_chance.id;
}

std::uint32_t LevelGenData::register_chance_logic_provider(std::uint32_t chance_id, SpawnLogicProvider provider)
{
    if (provider.is_valid == nullptr)
    {
        provider.is_valid = [](float x, float y, uint8_t layer)
        {
            return g_DefaultTestFunc(x, y, HeapBase::get().state()->layers[layer]);
        };
    }

    std::uint32_t provider_id = g_current_chance_logic_provider_id++;
    {
        std::lock_guard lock{g_chance_logic_providers_lock};
        g_chance_logic_providers.push_back({provider_id, chance_id, std::move(provider)});
    }
    return provider_id;
}
void LevelGenData::unregister_chance_logic_provider(std::uint32_t provider_id)
{
    std::lock_guard lock{g_chance_logic_providers_lock};
    std::erase_if(g_chance_logic_providers, [provider_id](const ChanceLogicProviderImpl& provider)
                  { return provider.id == provider_id; });
}

std::uint32_t LevelGenData::define_extra_spawn(std::uint32_t num_spawns_front_layer, std::uint32_t num_spawns_back_layer, SpawnLogicProvider provider)
{
    if (provider.is_valid == nullptr)
    {
        provider.is_valid = [](float x, float y, uint8_t layer)
        {
            return g_DefaultTestFunc(x, y, HeapBase::get().state()->layers[layer]);
        };
    }

    std::uint32_t extra_spawn_id = g_current_extra_spawn_id++;
    {
        std::lock_guard lock{g_extra_spawn_logic_providers_lock};
        g_extra_spawn_logic_providers.push_back({extra_spawn_id, num_spawns_front_layer, num_spawns_back_layer, std::move(provider)});
    }
    return extra_spawn_id;
}
void LevelGenData::set_num_extra_spawns(std::uint32_t extra_spawn_id, std::uint32_t num_spawns_front_layer, std::uint32_t num_spawns_back_layer)
{
    std::lock_guard lock{g_extra_spawn_logic_providers_lock};
    auto it = std::find_if(g_extra_spawn_logic_providers.begin(), g_extra_spawn_logic_providers.end(), [extra_spawn_id](const ExtraSpawnLogicProviderImpl& provider)
                           { return provider.extra_spawn_id == extra_spawn_id; });
    if (it != g_extra_spawn_logic_providers.end())
    {
        it->num_extra_spawns_frontlayer = num_spawns_front_layer;
        it->num_extra_spawns_backlayer = num_spawns_back_layer;
    }
}
std::pair<std::uint32_t, std::uint32_t> LevelGenData::get_missing_extra_spawns(std::uint32_t extra_spawn_id)
{
    std::lock_guard lock{g_extra_spawn_logic_providers_lock};
    auto it = std::find_if(g_extra_spawn_logic_providers.begin(), g_extra_spawn_logic_providers.end(), [extra_spawn_id](const ExtraSpawnLogicProviderImpl& provider)
                           { return provider.extra_spawn_id == extra_spawn_id; });
    if (it != g_extra_spawn_logic_providers.end())
    {
        return {
            it->num_extra_spawns_frontlayer - it->transient_num_remaining_spawns_frontlayer,
            it->num_extra_spawns_backlayer - it->transient_num_remaining_spawns_backlayer,
        };
    }
    return {};
}
void LevelGenData::undefine_extra_spawn(std::uint32_t extra_spawn_id)
{
    std::lock_guard lock{g_extra_spawn_logic_providers_lock};
    std::erase_if(g_extra_spawn_logic_providers, [extra_spawn_id](const ExtraSpawnLogicProviderImpl& provider)
                  { return provider.extra_spawn_id == extra_spawn_id; });
}

std::uint16_t LevelGenData::define_room_template(std::string room_template, RoomTemplateType type)
{
    if (auto existing = get_room_template(room_template))
    {
        return existing.value();
    }

    auto [it, success] = room_templates.emplace(std::move(room_template), RoomTemplateDef{(uint16_t)room_templates.size()});

    if (type != RoomTemplateType::None)
    {
        g_room_template_types.push_back({it->second.id, type});
        if (type == RoomTemplateType::MachineRoom)
        {
            g_room_template_sizes.push_back({it->second.id, {10, 8}});
        }
    }
    return it->second.id;
}
bool LevelGenData::set_room_template_size(std::uint16_t room_template, uint16_t width, uint16_t height)
{
    auto it = std::find_if(g_room_template_sizes.begin(), g_room_template_sizes.end(), [room_template](auto& t)
                           { return t.first == room_template; });
    if (it != g_room_template_sizes.end())
    {
        it->second = {width, height};
        return true;
    }
    return false;
}
RoomTemplateType LevelGenData::get_room_template_type(std::uint16_t room_template) const
{
    auto it = std::find_if(g_room_template_types.begin(), g_room_template_types.end(), [room_template](auto& t)
                           { return t.first == room_template; });
    if (it != g_room_template_types.end())
    {
        return it->second;
    }
    return RoomTemplateType::None;
}
uint16_t LevelGenData::get_pretend_room_template(std::uint16_t room_template) const
{
    switch (get_room_template_type(room_template))
    {
    default:
    case RoomTemplateType::None:
        return room_template;
    case RoomTemplateType::Entrance:
        return to_uint(RoomTemplate::Entrance);
    case RoomTemplateType::Exit:
        return to_uint(RoomTemplate::Exit);
    case RoomTemplateType::Shop:
        return to_uint(RoomTemplate::Shop);
    }
}

uint32_t ThemeInfo::get_aux_id() const
{
    thread_local const LevelGenSystem* level_gen_system = HeapBase::get().level_gen();
    for (size_t i = 0; i < std::size(level_gen_system->themes); i++)
    {
        if (level_gen_system->themes[i] == this)
        {
            return uint32_t(i + 1);
        }
    }

    return 0;
}

void LevelGenSystem::init()
{
    data->init();

    for (auto theme : themes)
    {
        if (theme == theme_arena) // no reason to?
            continue;

        hook_vtable<void(ThemeInfo*), 0x15>( // spawn_transition
            theme,
            [](ThemeInfo* th, void (*original)(ThemeInfo*))
            {
                push_spawn_type_flags(SPAWN_TYPE_LEVEL_GEN_GENERAL);
                OnScopeExit pop{[]
                                { pop_spawn_type_flags(SPAWN_TYPE_LEVEL_GEN_GENERAL); }};

                if (!pre_event(ON::PRE_LEVEL_GENERATION))
                    original(th);
                post_event(ON::POST_LEVEL_GENERATION);
            });
    }
}

void LevelGenSystem::populate_level_hook(ThemeInfo* self, uint64_t param_2, uint64_t param_3, uint64_t param_4, PopulateLevelFun* original)
{
    post_room_generation();

    {
        std::lock_guard lock{g_extra_spawn_logic_providers_lock};
        for (ExtraSpawnLogicProviderImpl& provider : g_extra_spawn_logic_providers)
        {
            provider.transient_num_remaining_spawns_frontlayer = provider.num_extra_spawns_frontlayer;
            provider.transient_num_remaining_spawns_backlayer = provider.num_extra_spawns_backlayer;
        }
    }

    original(self, param_2, param_3, param_4);
}
// void LevelGenSystem::populate_transition_hook([[maybe_unused]] ThemeInfo* self, [[maybe_unused]] PopulateTransitionFun* original)
//{
//     push_spawn_type_flags(SPAWN_TYPE_LEVEL_GEN_GENERAL);
//     OnScopeExit pop{[]
//                     { pop_spawn_type_flags(SPAWN_TYPE_LEVEL_GEN_GENERAL); }};
//     pre_level_generation();
//     original(self);
//     post_level_generation();
// }
void LevelGenSystem::do_procedural_spawn_hook(ThemeInfo* self, SpawnInfo* spawn_info, DoProceduralSpawnFun* original)
{
    push_spawn_type_flags(SPAWN_TYPE_LEVEL_GEN_PROCEDURAL);
    OnScopeExit pop{[]
                    { pop_spawn_type_flags(SPAWN_TYPE_LEVEL_GEN_PROCEDURAL); }};

    if (handle_chance(spawn_info))
    {
        return;
    }

    original(self, spawn_info);
}

std::pair<int, int> LevelGenSystem::get_room_index(float x, float y)
{
    return {static_cast<int>(std::ceil(x - 3.5f)) / 10, static_cast<int>(std::ceil(121.5f - y)) / 8};
}
Vec2 LevelGenSystem::get_room_pos(uint32_t x, uint32_t y)
{
    return {static_cast<float>(x * 10) + 2.5f, 122.5f - static_cast<float>(y * 8)};
}
std::optional<uint16_t> LevelGenSystem::get_room_template(uint32_t x, uint32_t y, uint8_t l) const
{
    auto* state_ptr = HeapBase::get().state();

    if (x < 0 || y < 0 || x >= state_ptr->w || y >= state_ptr->h)
        return std::nullopt;

    const int32_t flat_room_idx = x + y * 8;
    if (g_CustomRoomShims[l].rooms[flat_room_idx] != 0)
    {
        return g_CustomRoomShims[l].rooms[flat_room_idx];
    }

    LevelGenRooms* level_rooms = rooms[l];
    return level_rooms->rooms[x + y * 8];
}
bool LevelGenSystem::set_room_template(uint32_t x, uint32_t y, int l, uint16_t room_template)
{
    auto* state_ptr = HeapBase::get().state();

    if (x < 0 || y < 0 || x >= state_ptr->w || y >= state_ptr->h)
        return false;

    LevelGenRooms* level_rooms = rooms[l];
    level_rooms->rooms[x + y * 8] = room_template;

    // Unset machine room origin flag if it is a machine room so there's no accidental origins left within the machine room
    const RoomTemplateType type = data->get_room_template_type(room_template);
    if (type == RoomTemplateType::MachineRoom || type == RoomTemplateType::VanillaMachineRoom)
    {
        machine_room_origin->rooms[x + y * 8] = false;
    }

    return true;
}

bool LevelGenSystem::is_room_flipped(uint32_t x, uint32_t y) const
{
    auto* state_ptr = HeapBase::get().state();

    if (x < 0 || y < 0 || x >= state_ptr->w || y >= state_ptr->h)
        return false;

    return flipped_rooms->rooms[x + y * 8];
}
bool LevelGenSystem::is_machine_room_origin(uint32_t x, uint32_t y) const
{
    auto* state_ptr = HeapBase::get().state();

    if (x < 0 || y < 0 || x >= state_ptr->w || y >= state_ptr->h)
        return false;

    return machine_room_origin->rooms[x + y * 8];
}
bool LevelGenSystem::mark_as_machine_room_origin(uint32_t x, uint32_t y, uint8_t /*l*/)
{
    auto* state_ptr = HeapBase::get().state();

    if (x < 0 || y < 0 || x >= state_ptr->w || y >= state_ptr->h)
        return false;

    machine_room_origin->rooms[x + y * 8] = true;

    return true;
}
bool LevelGenSystem::mark_as_set_room(uint32_t x, uint32_t y, uint8_t l, bool is_set_room)
{
    auto* state_ptr = HeapBase::get().state();

    if (x < 0 || y < 0 || x >= state_ptr->w || y >= state_ptr->h)
        return false;

    if (l == 0)
    {
        set_room_front_layer->rooms[x + y * 8] = is_set_room;
    }
    else
    {
        set_room_back_layer->rooms[x + y * 8] = is_set_room;
    }

    return true;
}

bool LevelGenSystem::set_shop_type(uint32_t x, uint32_t y, uint8_t l, SHOP_TYPE _shop_type)
{
    auto* state_ptr = HeapBase::get().state();

    if (x < 0 || y < 0 || x >= state_ptr->w || y >= state_ptr->h)
        return false;

    g_CustomShopTypes[l][x][y] = _shop_type;
    return true;
}

std::string_view LevelGenSystem::get_room_template_name(uint16_t room_template) const
{
    for (const auto& [name, room_tpl] : data->room_templates)
        if (room_tpl.id == room_template)
            return name;

    return "invalid";
}

LevelChanceDef& get_or_emplace_level_chance(game_unordered_map<std::uint32_t, LevelChanceDef>& level_chances, uint32_t chance_id)
{
    struct LevelChanceNode
    {
        void* ptr0;
        void* ptr1;
        std::pair<uint32_t, LevelChanceDef> value;
    };
    using EmplaceLevelChance = LevelChanceNode**(void*, std::pair<LevelChanceNode*, bool>*, uint32_t);
    static EmplaceLevelChance* emplace_level_chance = (EmplaceLevelChance*)get_address("level_gen_emplace_level_chance");

    std::pair<LevelChanceNode*, bool> node;
    emplace_level_chance((void*)&level_chances, &node, chance_id);

    return node.first->value.second;
}

std::optional<std::string_view> LevelGenSystem::get_procedural_spawn_chance_name(uint32_t chance_id)
{
    if (g_monster_chance_id_to_name.contains(chance_id))
        return g_monster_chance_id_to_name[chance_id];
    if (g_trap_chance_id_to_name.contains(chance_id))
        return g_trap_chance_id_to_name[chance_id];
    return std::nullopt;
}

uint32_t LevelGenSystem::get_procedural_spawn_chance(uint32_t chance_id) const
{
    if (g_monster_chance_id_to_name.contains(chance_id))
    {
        LevelChanceDef& this_chances = get_or_emplace_level_chance(data->level_monster_chances, chance_id);
        if (!this_chances.chances.empty())
        {
            auto* state = HeapBase::get().state();
            if (this_chances.chances.size() >= state->level && state->level > 0)
            {
                return this_chances.chances[state->level - 1];
            }
            else if (this_chances.chances.size() == 1)
            {
                return this_chances.chances[0];
            }
            else
            {
                return 0;
            }
        }
    }

    if (g_trap_chance_id_to_name.contains(chance_id))
    {
        LevelChanceDef& this_chances = get_or_emplace_level_chance(data->level_trap_chances, chance_id);
        if (!this_chances.chances.empty())
        {
            auto* state = HeapBase::get().state();
            if (this_chances.chances.size() >= state->level && state->level > 0)
            {
                return this_chances.chances[state->level - 1];
            }
            else if (this_chances.chances.size() == 1)
            {
                return this_chances.chances[0];
            }
            else
            {
                return 0;
            }
        }
    }

    return 0;
}
bool LevelGenSystem::set_procedural_spawn_chance(uint32_t chance_id, uint32_t inverse_chance)
{
    if (g_monster_chance_id_to_name.contains(chance_id))
    {
        LevelChanceDef& this_chances = get_or_emplace_level_chance(data->level_monster_chances, chance_id);
        if (inverse_chance == 0)
        {
            this_chances.chances.clear();
        }
        else
        {
            this_chances.chances = {inverse_chance};
        }
        return true;
    }

    if (g_trap_chance_id_to_name.contains(chance_id))
    {
        LevelChanceDef& this_chances = get_or_emplace_level_chance(data->level_trap_chances, chance_id);
        if (inverse_chance == 0)
        {
            this_chances.chances.clear();
        }
        else
        {
            this_chances.chances = {inverse_chance};
        }
        return true;
    }

    return false;
}

bool default_spawn_is_valid(float x, float y, LAYER layer)
{
    return g_DefaultTestFunc(x, y, HeapBase::get().state()->layer(layer));
}

bool position_is_valid(float x, float y, LAYER layer, POS_TYPE flags)
{
    return g_PositionTestFunc(x, y, HeapBase::get().state()->layer(layer), flags);
}

void override_next_levels(std::vector<std::string> next_levels)
{
    g_levels_to_load = std::move(next_levels);
    g_replace_level_loads = true;
}
void add_next_levels(std::vector<std::string> next_levels)
{
    std::move(next_levels.begin(), next_levels.end(), std::back_inserter(g_levels_to_load));
}

COSUBTHEME get_co_subtheme()
{
    auto state = get_state_ptr();
    if (state->theme != 10)
    {
        return -2;
    }

    ThemeInfo* theme = state->level_gen->theme_cosmicocean->sub_theme;
    if (theme == state->level_gen->theme_dwelling)
    {
        return 0;
    }
    else if (theme == state->level_gen->theme_jungle)
    {
        return 1;
    }
    else if (theme == state->level_gen->theme_volcana)
    {
        return 2;
    }
    else if (theme == state->level_gen->theme_tidepool)
    {
        return 3;
    }
    else if (theme == state->level_gen->theme_temple)
    {
        return 4;
    }
    else if (theme == state->level_gen->theme_icecaves)
    {
        return 5;
    }
    else if (theme == state->level_gen->theme_neobabylon)
    {
        return 6;
    }
    else if (theme == state->level_gen->theme_sunkencity)
    {
        return 7;
    }

    return -2;
}

void force_co_subtheme(COSUBTHEME subtheme)
{
    static size_t offset = get_address("cosmic_ocean_subtheme");

    // There isn't enough room to overwrite the result of the random number generation with a `mov r8, <subtheme>`
    // so we overwrite the start of the random number generator with this instruction and then jump to where the
    // subtheme index is used to create the subtheme pointer
    if (subtheme >= 0 && subtheme <= 7)
    {
        uint8_t replacement[] = {0x41, 0xB8, (uint8_t)subtheme, 0x00, 0x00, 0x00, 0xEB, 0x1F, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
        write_mem_recoverable("force_co_subtheme", offset, replacement, true);
    }
    else if (subtheme == -1)
    {
        recover_mem("force_co_subtheme");
    }
}

void grow_vines(LAYER l, uint32_t max_length, AABB area, bool destroy_broken)
{
    area.abs();

    const auto state = HeapBase::get().state();
    const static auto grow_vine = to_id("ENT_TYPE_FLOOR_GROWABLE_VINE");
    const static auto tree_vine = to_id("ENT_TYPE_FLOOR_VINE_TREE_TOP");
    const static auto vine = to_id("ENT_TYPE_FLOOR_VINE");
    const auto actual_layer = enum_to_layer(l);

    const uint32_t start_x = static_cast<uint32_t>(area.left + 0.5f);
    uint32_t end_x = static_cast<uint32_t>(area.right + 0.5f);
    uint32_t start_y = static_cast<uint32_t>(area.top + 0.5f);
    uint32_t end_y = static_cast<uint32_t>(area.bottom + 0.5f);

    if (start_x >= g_level_max_x || end_y >= g_level_max_y)
        return;

    if (end_x == 0)
        end_x = state->w * 10 + 6;

    if (end_x >= g_level_max_x)
        end_x = g_level_max_x - 1;

    if (start_y == 0 || start_y >= g_level_max_y)
        start_y = g_level_max_y - 1;

    if (end_y == 0)
        end_y = g_level_max_y - (state->h * 8 + 6);

    if (end_y >= g_level_max_y) // in case of overflow when someone puts weird state->h value
        end_y = 0;

    for (uint32_t i_x = start_x; i_x <= end_x; ++i_x)
    {
        for (int i_y = start_y; i_y >= static_cast<int>(end_y); --i_y) // go from top to bottom
        {
            auto test_ent = state->layers[actual_layer]->grid_entities[i_y][i_x];
            if (!test_ent || !test_ent->type)
                continue;

            if (test_ent->type->id == grow_vine || test_ent->type->id == tree_vine)
            {
                if (i_y - 2 < 0 || state->layers[actual_layer]->grid_entities[i_y - 1][i_x] != nullptr || state->layers[actual_layer]->grid_entities[i_y - 2][i_x] != nullptr)
                {
                    if (destroy_broken)
                        test_ent->destroy();

                    continue;
                }
                if (test_ent->type->id == grow_vine)
                {
                    test_ent->animation_frame = 4;
                }
                --i_y;
                int32_t last_uid = -1;
                const int max = (i_y - (int)max_length) < 0 ? 0 : i_y - max_length;
                for (; i_y > max && (i_y - 1) >= 0; --i_y)
                {
                    if (state->layers[actual_layer]->grid_entities[i_y - 1][i_x] != nullptr)
                    {
                        break;
                    }
                    last_uid = spawn_entity_snap_to_grid(vine, static_cast<float>(i_x), static_cast<float>(i_y), (LAYER)actual_layer);
                }
                if (const auto last_piece = get_entity_ptr(last_uid))
                {
                    last_piece->animation_frame = 28;
                }
            }
        }
    }
    if (l == LAYER::BOTH)
    {
        grow_vines(LAYER::BACK, max_length, area, destroy_broken);
    }
}

void grow_poles(LAYER l, uint32_t max_length, AABB area, bool destroy_broken)
{
    area.abs();

    const auto state = HeapBase::get().state();
    const static auto grow_pole = to_id("ENT_TYPE_FLOOR_GROWABLE_CLIMBING_POLE");
    const static auto pole = to_id("ENT_TYPE_FLOOR_CLIMBING_POLE");
    const auto actual_layer = enum_to_layer(l);

    const uint32_t start_x = static_cast<uint32_t>(area.left + 0.5f);
    uint32_t end_x = static_cast<uint32_t>(area.right + 0.5f);
    uint32_t start_y = static_cast<uint32_t>(area.bottom + 0.5f);
    uint32_t end_y = static_cast<uint32_t>(area.top + 0.5f);

    if (start_x >= g_level_max_x || start_y >= g_level_max_y)
        return;

    if (end_x == 0)
        end_x = state->w * 10 + 6;

    if (end_x >= g_level_max_x)
        end_x = g_level_max_x - 1;

    if (end_y == 0 || end_y >= g_level_max_y)
        end_y = g_level_max_y - 1;

    if (start_y == 0)
        start_y = g_level_max_y - (state->h * 8 + 6);

    if (start_y >= g_level_max_y) // in case of overflow when someone puts weird state->h value
        start_y = 0;

    for (uint32_t i_x = start_x; i_x <= end_x; ++i_x)
    {
        for (uint32_t i_y = start_y; i_y <= end_y; ++i_y)
        {
            auto test_ent = state->layers[actual_layer]->grid_entities[i_y][i_x];
            if (!test_ent || !test_ent->type)
                continue;

            if (test_ent->type->id == grow_pole)
            {
                if (i_y + 2 >= g_level_max_y || state->layers[actual_layer]->grid_entities[i_y + 1][i_x] != nullptr || state->layers[actual_layer]->grid_entities[i_y + 2][i_x] != nullptr)
                {
                    if (destroy_broken)
                        test_ent->destroy();

                    continue;
                }
                test_ent->destroy();
                test_ent = get_entity_ptr(spawn_entity_snap_to_grid(pole, static_cast<float>(i_x), static_cast<float>(i_y), (LAYER)actual_layer));
                if (!test_ent)
                    continue;

                test_ent->animation_frame = 40;

                ++i_y;
                int32_t last_uid = -1;
                for (uint32_t max = i_y + max_length; i_y < max && (i_y + 1) < g_level_max_y; ++i_y)
                {
                    if (state->layers[actual_layer]->grid_entities[i_y + 1][i_x] != nullptr)
                    {
                        break;
                    }
                    last_uid = spawn_entity_snap_to_grid(pole, static_cast<float>(i_x), static_cast<float>(i_y), (LAYER)actual_layer);
                }
                if (const auto last_piece = get_entity_ptr(last_uid))
                {
                    last_piece->animation_frame = 4;
                }
            }
        }
    }
    if (l == LAYER::BOTH)
    {
        grow_poles(LAYER::BACK, max_length, area, destroy_broken);
    }
}

bool grow_chain_and_blocks()
{
    const auto state = HeapBase::get().state();
    return grow_chain_and_blocks(state->w * 10 + 6, state->h * 8 + 6);
}

bool grow_chain_and_blocks(uint32_t x, uint32_t y)
{
    using GrowChainAndBlocks = bool(uint32_t, uint32_t);
    static auto grow_fun = (GrowChainAndBlocks*)get_address("grow_chain_and_blocks");
    return grow_fun(x, y);
}

void do_load_screen()
{
    static auto load_screen_fun = (LoadScreenFun*)get_address("load_screen_func");
    const auto state = HeapBase::get().state();
    if (pre_load_screen())
        return;
    load_screen_fun(state, 0, 0);
    post_load_screen();
}

std::span<const std::pair<std::string_view, uint16_t>> LevelGenData::get_missing_room_templates()
{
    constexpr static const std::array<std::pair<std::string_view, uint16_t>, 8> missing_templates = {{
        {"empty_backlayer", 9},
        {"boss_arena", 22},
        {"shop_jail_backlayer", 44},
        {"waddler", 86},
        {"ghistshop_backlayer", 87},
        {"challange_entrance_backlayer", 90},
        {"blackmarket", 119},
        {"mothership_room", 126},
    }};
    return {missing_templates.begin(), missing_templates.end()};
}
