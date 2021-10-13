#include "level_api.hpp"

#include "entities_monsters.hpp"
#include "entity.hpp"
#include "game_allocator.hpp"
#include "layer.hpp"
#include "logger.h"
#include "memory.hpp"
#include "prng.hpp"
#include "rpc.hpp"
#include "spawn_api.hpp"
#include "util.hpp"
#include "vtable_hook.hpp"

#include "script/events.hpp"

#include <array>
#include <numbers>
#include <string_view>
#include <tuple>

#include <Windows.h>
#include <detours.h>

std::uint32_t g_last_tile_code_id;
std::uint32_t g_last_community_tile_code_id;
std::uint32_t g_current_tile_code_id;

std::uint32_t g_last_chance_id;
std::uint32_t g_last_community_chance_id;
std::uint32_t g_current_chance_id;

std::unordered_map<std::uint32_t, std::string_view> g_tile_code_id_to_name;
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
    thread_local StateMemory* state_ptr = State::get().ptr_local();
    auto [ix, iy] = state_ptr->level_gen->get_room_index(x, y);
    return state_ptr->level_gen->flipped_rooms->rooms[ix + iy * 8];
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
        std::vector<uint32_t> entities_neighbour = get_entities_overlapping_by_pointer({}, 0, x - 0.5f, y - 0.5f, x + 0.5f, y + 0.5f, layer);
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
    CommunityTileCode{"bat", "ENT_TYPE_MONS_BAT"},
    CommunityTileCode{"skeleton", "ENT_TYPE_MONS_SKELETON"},
    CommunityTileCode{"redskeleton", "ENT_TYPE_MONS_REDSKELETON"},
    CommunityTileCode{"lizard", "ENT_TYPE_MONS_HORNEDLIZARD"},
    CommunityTileCode{"mole", "ENT_TYPE_MONS_MOLE"},
    CommunityTileCode{"monkey", "ENT_TYPE_MONS_MONKEY"},
    CommunityTileCode{"firebug", "ENT_TYPE_MONS_FIREBUG"},
    CommunityTileCode{"vampire", "ENT_TYPE_MONS_VAMPIRE"},
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
    CommunityTileCode{"cobweb", "ENT_TYPE_ITEM_WEB"},
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
    CommunityTileCode{"spider", "ENT_TYPE_MONS_SPIDER"},
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
            Entity* anchor = layer->spawn_entity_over(anchor_id, web, 0.0f, 0.0f);
            *(uint32_t*)((size_t)anchor + sizeof(Movable)) = spider->uid;
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
            hook_vtable<void(Entity*, Entity*)>(
                slidingwall, [](Entity*, Entity*, void (*)(Entity*, Entity*)) {}, 26);
        },
    },
    CommunityTileCode{"spikeball_trap", "ENT_TYPE_FLOOR_SPIKEBALL_CEILING"},
    CommunityTileCode{
        "spikeball_no_bounce",
        "ENT_TYPE_ACTIVEFLOOR_UNCHAINED_SPIKEBALL",
        [](const CommunityTileCode& self, float x, float y, Layer* layer)
        {
            Entity* spikeball = layer->spawn_entity(self.entity_id, x, y, false, 0.0f, 0.0f, true);
            *(bool*)((size_t)spikeball + sizeof(Movable)) = true;
        },
    },
    CommunityTileCode{"boulder", "ENT_TYPE_ACTIVEFLOOR_BOULDER"},
    CommunityTileCode{"apep", "ENT_TYPE_MONS_APEP_HEAD"},
    CommunityTileCode{
        "apep_left",
        "ENT_TYPE_MONS_APEP_HEAD",
        []([[maybe_unused]] const CommunityTileCode& self, float x, float y, Layer* layer)
        {
            layer->spawn_apep(x, y, is_room_flipped(x, y));
        }},
    CommunityTileCode{
        "apep_right",
        "ENT_TYPE_MONS_APEP_HEAD",
        []([[maybe_unused]] const CommunityTileCode& self, float x, float y, Layer* layer)
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

            Entity* olmite = layer->spawn_entity_snap_to_floor(self.entity_id, x, y);

            std::vector<uint32_t> entities_above = get_entities_overlapping_by_pointer({}, 0x4, x - 0.1f, y + 0.9f, x + 0.1f, y + 1.1f, layer);
            for (uint32_t uid : entities_above)
            {
                if (Entity* ent = get_entity_ptr(uid))
                {
                    if (ent->type->id == helmet_id || ent->type->id == naked_id || ent->type->id == self.entity_id)
                    {
                        *(bool*)((size_t)ent + 0x151) = true;
                        *(bool*)((size_t)olmite + 0x151) = true;
                        *(uint32_t*)((size_t)olmite + 0x154) = ent->uid;

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
    CommunityTileCode{
        "movable_spikes",
        "ENT_TYPE_ITEM_SPIKES",
        [](const CommunityTileCode& self, float x, float y, Layer* layer)
        {
            auto do_spawn = [=]()
            {
                std::vector<uint32_t> entities_neighbour = get_entities_overlapping_by_pointer({}, 0, x - 0.5f, y - 1.5f, x + 0.5f, y - 0.5f, layer);
                if (!entities_neighbour.empty())
                {
                    layer->spawn_entity_over(self.entity_id, get_entity_ptr(entities_neighbour.front()), 0.0f, 1.0f);
                }
            };
            g_attachee_requiring_entities.push_back({{{x, y - 1}}, do_spawn});
        }},
    //CommunityTileCode{
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

auto g_DefaultTestFunc = [](float x, float y, Layer* layer)
{
    if (!layer->get_grid_entity_at(x, y))
    {
        if (Entity* floor = layer->get_grid_entity_at(x, y - 1.0f))
        {
            static auto entrance = to_id("ENT_TYPE_FLOOR_DOOR_ENTRANCE");
            return floor->type->id != entrance && (floor->type->properties_flags & (1 << 20)) != 0; // Can spawn monsters on top
        }
    }
    return false;
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
    std::vector<std::pair<float, float>> transient_valid_positions;
};
std::mutex g_extra_spawn_logic_providers_lock;
std::uint32_t g_current_extra_spawn_id{0};
std::vector<ExtraSpawnLogicProviderImpl> g_extra_spawn_logic_providers;

std::vector<std::pair<uint16_t, RoomTemplateType>> g_room_template_types;
std::vector<std::pair<uint16_t, std::pair<uint32_t, uint32_t>>> g_room_template_sizes;

// Used for making custom machine rooms work
std::optional<uint16_t> g_overridden_room_template;

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

    pre_level_generation();
    g_level_gen_trampoline(level_gen_sys, param_2, param_3);
    post_level_generation();

    g_replace_level_loads = false;
    g_levels_to_load.clear();
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

    if (tile_code > g_last_tile_code_id && tile_code < g_last_community_tile_code_id)
    {
        auto* layer_ptr = State::get().ptr_local()->layers[layer];
        const CommunityTileCode& community_tile_code = g_community_tile_codes[tile_code - g_last_tile_code_id - 1];
        community_tile_code.func(community_tile_code, x, y, layer_ptr);
    }
    else
    {
        uint16_t pretend_room_template = room_template;
        switch (self->data->get_room_template_type(room_template))
        {
        default:
        case RoomTemplateType::None:
            break;
        case RoomTemplateType::Entrance:
            pretend_room_template = 5;
            break;
        case RoomTemplateType::Exit:
            pretend_room_template = 7;
            break;
        case RoomTemplateType::Shop:
            pretend_room_template = 65;
            break;
        }
        g_handle_tile_code_trampoline(self, tile_code, pretend_room_template, x, y, layer);
    }

    post_tile_code_spawn(tile_code_name, x, y, layer, room_template);

    if (!g_floor_requiring_entities.empty())
    {
        Entity* floor{nullptr};
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
                            auto* layer_ptr = State::get().ptr_local()->layers[layer];
                            floor = layer_ptr->get_grid_entity_at(x, y);
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
    // cache, ghistroom
    if (room_template >= 14 && room_template <= 15)
    {
        room_width = 5;
        room_height = 5;
    }
    // chunk_ground, chunk_air
    else if (room_template >= 16 && room_template <= 17)
    {
        room_width = 5;
        room_height = 3;
    }
    // chunk_door
    else if (room_template == 18)
    {
        room_width = 6;
        room_height = 3;
    }
    // bigroom
    else if (room_template >= 102 && room_template <= 106)
    {
        room_width = 20;
        room_height = 16;
    }
    // wideroom
    else if (room_template >= 107 && room_template <= 108)
    {
        room_width = 20;
        room_height = 8;
    }
    // tallroom
    else if (room_template >= 109 && room_template <= 111)
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
    const auto room_template = State::get().ptr_local()->level_gen->data->get_room_template(room_template_name);
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

using DoExtraSpawns = void(ThemeInfo*, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, std::uint8_t);
DoExtraSpawns* g_do_extra_spawns_trampoline{nullptr};
void do_extra_spawns(ThemeInfo* theme, std::uint32_t border_width, std::uint32_t border_height, std::uint32_t level_width, std::uint32_t level_height, std::uint8_t layer)
{
    g_do_extra_spawns_trampoline(theme, border_width, border_height, level_width, level_height, layer);

    PRNG& prng = PRNG::get_local();

    std::lock_guard lock{g_extra_spawn_logic_providers_lock};
    if (!g_extra_spawn_logic_providers.empty())
    {
        for (ExtraSpawnLogicProviderImpl& provider : g_extra_spawn_logic_providers)
        {
            provider.transient_valid_positions.clear();
        }

        for (std::uint32_t ix = border_width; ix != level_width; ix++)
        {
            const float x = static_cast<float>(ix);
            for (std::uint32_t iy = border_height; iy != level_height; iy++)
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

        for (ExtraSpawnLogicProviderImpl& provider : g_extra_spawn_logic_providers)
        {
            auto& valid_pos = provider.transient_valid_positions;
            while (!valid_pos.empty() && provider.transient_num_remaining_spawns[layer] > 0)
            {
                const auto random_idx = static_cast<std::size_t>(prng.internal_random_index(valid_pos.size(), PRNG::EXTRA_SPAWNS));
                const auto idx = random_idx < valid_pos.size() ? random_idx : 0;
                const auto [x, y] = valid_pos[idx];
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
    if (g_overridden_room_template == std::nullopt)
    {
        const int32_t flat_room_idx = room_idx_x + room_idx_y * 8;
        const uint16_t room_template = level_gen->rooms_frontlayer->rooms[flat_room_idx];
        if (level_gen->data->get_room_template_type(room_template) == RoomTemplateType::MachineRoom)
        {
            // Revert the room template the next time room data will be collected
            g_overridden_room_template = room_template;
            // Set it to any machine room for now, this will branch in g_generate_room_trampoline to allow for large levels
            level_gen->rooms_frontlayer->rooms[flat_room_idx] = 109;
        }
    }
    g_generate_room_trampoline(level_gen, room_idx_x, room_idx_y);
    if (g_overridden_room_template)
    {
        const int32_t flat_room_idx = room_idx_x + room_idx_y * 8;
        State::get().ptr()->level_gen->rooms_frontlayer->rooms[flat_room_idx] = g_overridden_room_template.value();
        g_overridden_room_template.reset();
    }
}

using GatherRoomData = void(LevelGenData*, byte, int room_x, int, bool, uint8_t*, uint8_t*, size_t, uint8_t*, uint8_t*, uint8_t*, uint8_t*);
GatherRoomData* g_gather_room_data_trampoline{nullptr};
void gather_room_data(LevelGenData* tile_storage, byte param_2, int room_idx_x, int room_idx_y, bool hard_level, uint8_t* param_6, uint8_t* param_7, size_t param_8, uint8_t* param_9, uint8_t* param_10, uint8_t* out_room_width, uint8_t* out_room_height)
{
    if (g_overridden_room_template.has_value())
    {
        const int32_t flat_room_idx = room_idx_x + room_idx_y * 8;
        State::get().ptr()->level_gen->rooms_frontlayer->rooms[flat_room_idx] = g_overridden_room_template.value();
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
    LevelGenRoomData room_data{};
    std::memcpy(room_data.front_layer.data(), front_room_data, 10 * 8);
    if (dual_room)
    {
        room_data.back_layer.emplace();
        std::memcpy(room_data.back_layer.value().data(), front_room_data, 10 * 8);
    }
    std::optional<LevelGenRoomData> changed_data = pre_handle_room_tiles(room_data, room_idx_x, room_idx_y, room_template);
    if (changed_data)
    {
        front_room_data = &changed_data->front_layer;
        dual_room = changed_data->back_layer.has_value();
        back_room_data = dual_room ? &changed_data->back_layer.value() : nullptr;
    }

    g_spawn_room_from_tile_codes_trampoline(level_gen_data, room_idx_x, room_idx_y, front_room_data, back_room_data, param_6, dual_room, room_template);
}

using TestChance = bool(LevelGenData**, std::uint32_t chance_id);
TestChance* g_test_chance{nullptr};

struct SpawnInfo
{
    void* ptr0;
    void* ptr1;
    float x;
    float y;
};
bool handle_chance(SpawnInfo* spawn_info)
{
    auto level_gen_data = State::get().ptr()->level_gen->data;

    uint8_t layer = 0;
    auto* layer_ptr = State::get().layer_local(layer);
    for (const CommunityChance& community_chance : g_community_chances)
    {
        if (community_chance.test_func(community_chance, spawn_info->x, spawn_info->y, layer_ptr))
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
            if (chance_provider.provider.is_valid(spawn_info->x, spawn_info->y, layer))
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
    // Scan tile codes to know what id to start at
    {
        // Getting the last id like this in case the game decides to skip some ids so that last_id != tile_codes.size()
        auto max_id = 0u;
        for (auto& [name, def] : tile_codes)
        {
            max_id = std::max(def.id, max_id);
            g_tile_code_id_to_name[def.id] = name;
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
        assert(!get_tile_code(std::string{community_tile_code.tile_code}).has_value());
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

    // Add new community chances
    for (auto& community_chance : g_community_chances)
    {
        assert(!get_chance(std::string{community_chance.chance}).has_value());
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
                "\x50"                         //PUSH       RAX
                "\x51"                         //PUSH       RCX
                "\x52"                         //PUSH       RDX
                "\x41\x50"                     //PUSH       R8
                "\x41\x51"                     //PUSH       R9
                "\x41\x52"                     //PUSH       R10
                "\x41\x53"                     //PUSH       R11
                "\x41\x53"                     //PUSH       R11
                "\x48\x8d\x8d\xa0\x02\x00\x00" //LEA        RCX, [RBP + 0x2a0] == room_template_name
                "\x48\x8d\x95\x58\x05\x00\x00" //LEA        RDX, [RBP + 0x558] == room_width
                "\x4c\x8d\x85\xb8\x05\x00\x00" //LEA        R8, [RBP + 0x5b8] == room_height
                "\x48\xb8{}"                   //MOV        RAX, &get_room_size
                "\xff\xd0"                     //CALL       RAX
                "\x41\x5b"                     //POP        R11
                "\x41\x5b"                     //POP        R11
                "\x41\x5a"                     //POP        R10
                "\x41\x59"                     //POP        R9
                "\x41\x58"                     //POP        R8
                "\x5a"                         //POP        RDX
                "\x59"                         //POP        RCX
                "\x58"                         //POP        RAX
                "\x44\x8b\xad\xd4\x05\x00\x00" //MOV        R13D,dword ptr [RBP + 0x5d4]
                "\x41\x83\xe5\x08"             //AND        R13D,0x8
                "\x48\x8b\x85\x50\x05\x00\x00" //MOV        RAX,qword ptr [RBP + 0x550]
                "\x48\x85\xc0"                 //TEST       RAX,RAX
                "\x74\x13"                     //JZ         FIRST_JUMP
                "\x49\x89\xc4"                 //MOV        R12,RAX
                "\x48\x8b\x9d\x60\x05\x00\x00" //MOV        RBX,qword ptr [RBP + 0x560]
                "\x48\x8b\xb5\x48\x05\x00\x00" //MOV        RSI,qword ptr [RBP + 0x548]
                "\xeb\x0c"                     //JMP        SECOND_JUMP
                /*FIST_JUMP*/ "\x48\xb8{}"     //MOV        RAX, get_room_size_first_jump
                "\xff\xe0"                     //JMP        RAX
                /*SECOND_JUMP*/ "\x48\xb8{}"   //MOV        RAX, get_room_size_second_jump
                "\xff\xe0"sv,                  //JMP        RAX
                to_le_bytes(get_room_size_addr),
                to_le_bytes(get_room_size_first_jump),
                to_le_bytes(get_room_size_second_jump));

            g_get_room_size_redirect = ExecutableMemory{redirect_code};

            std::string code = fmt::format(
                "\x48\xb8{}"  //MOV         RAX, g_get_room_size_redirect.get()
                "\xff\xe0"sv, //JMP         RAX
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

        const LONG error = DetourTransactionCommit();
        if (error != NO_ERROR)
        {
            DEBUG("Failed hooking HandleTileCode: {}\n", error);
        }
    }

    g_test_chance = (TestChance*)get_address("level_gen_test_spawn_chance");
}

std::optional<std::uint32_t> LevelGenData::get_tile_code(const std::string& tile_code)
{
    auto it = tile_codes.find((game_string&)tile_code);
    if (it != tile_codes.end())
    {
        return it->second.id;
    }
    return {};
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
    return it->second.id;
}

std::optional<uint8_t> LevelGenData::get_short_tile_code(ShortTileCodeDef short_tile_code_def)
{
    for (auto [i, def] : short_tile_codes)
    {
        if (def == short_tile_code_def)
        {
            return i;
        }
    }
    return std::nullopt;
}
std::optional<ShortTileCodeDef> LevelGenData::get_short_tile_code_def(uint8_t short_tile_code)
{
    auto it = short_tile_codes.find(short_tile_code);
    if (it != short_tile_codes.end())
    {
        return it->second;
    }
    return {};
}
void LevelGenData::change_short_tile_code(uint8_t short_tile_code, ShortTileCodeDef short_tile_code_def)
{
    short_tile_codes[short_tile_code] = short_tile_code_def;
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

std::optional<std::uint32_t> LevelGenData::get_chance(const std::string& chance)
{
    {
        auto it = monster_chances.find((game_string&)chance);
        if (it != monster_chances.end())
        {
            return it->second.id;
        }
    }
    {
        auto it = trap_chances.find((game_string&)chance);
        if (it != trap_chances.end())
        {
            return it->second.id;
        }
    }
    return {};
}
std::uint32_t LevelGenData::define_chance(std::string chance)
{
    if (auto existing = get_chance(chance))
    {
        return existing.value();
    }

    // We use only monster chances to define new stuff, keep an eye out for whether this is dangerous
    auto [it, success] = monster_chances.emplace(chance.c_str(), ChanceDef{g_current_chance_id});
    g_current_chance_id++;

    g_monster_chance_id_to_name[it->second.id] = it->first;
    return it->second.id;
}

std::uint32_t LevelGenData::register_chance_logic_provider(std::uint32_t chance_id, SpawnLogicProvider provider)
{
    if (provider.is_valid == nullptr)
    {
        provider.is_valid = [](float x, float y, uint8_t layer)
        {
            return g_DefaultTestFunc(x, y, State::get().layer_local(layer));
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
            return g_DefaultTestFunc(x, y, State::get().layer_local(layer));
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

std::optional<std::uint16_t> LevelGenData::get_room_template(const std::string& room_template)
{
    auto it = room_templates.find((game_string&)room_template);
    if (it != room_templates.end())
    {
        return it->second.id;
    }
    return {};
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
RoomTemplateType LevelGenData::get_room_template_type(std::uint16_t room_template)
{
    auto it = std::find_if(g_room_template_types.begin(), g_room_template_types.end(), [room_template](auto& t)
                           { return t.first == room_template; });
    if (it != g_room_template_types.end())
    {
        return it->second;
    }
    return RoomTemplateType::None;
}

using DoProceduralSpawnFun = void(ThemeInfo*, SpawnInfo*);
void LevelGenSystem::init()
{
    data->init();

    for (ThemeInfo* theme : themes)
    {
        using PopulateLevelFun = void(ThemeInfo * self, uint64_t param_2, uint64_t param_3, uint64_t param_4);
        hook_vtable<PopulateLevelFun>(
            theme_dwelling, [](ThemeInfo* self, uint64_t param_2, uint64_t param_3, uint64_t param_4, PopulateLevelFun* original)
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
            },
            0xd);
        hook_vtable<DoProceduralSpawnFun>(
            theme, [](ThemeInfo* self, SpawnInfo* spawn_info, DoProceduralSpawnFun* original)
            {
                push_spawn_type_flags(SPAWN_TYPE_LEVEL_GEN_PROCEDURAL);
                OnScopeExit pop{[]
                                { pop_spawn_type_flags(SPAWN_TYPE_LEVEL_GEN_PROCEDURAL); }};

                if (handle_chance(spawn_info))
                {
                    return;
                }
                original(self, spawn_info);
            },
            0x33);
    }
}

std::pair<int, int> LevelGenSystem::get_room_index(float x, float y)
{
    return std::pair<int, int>{
        static_cast<int>(std::ceil(x - 3.5f)) / 10,
        static_cast<int>(std::ceil(121.5f - y)) / 8};
}
std::pair<float, float> LevelGenSystem::get_room_pos(uint32_t x, uint32_t y)
{
    return std::pair<float, float>{
        static_cast<float>(x * 10) + 2.5f,
        122.5f - static_cast<float>(y * 8)};
}
std::optional<uint16_t> LevelGenSystem::get_room_template(uint32_t x, uint32_t y, uint8_t l)
{
    auto state = State::get();
    auto* state_ptr = state.ptr_local();

    if (x < 0 || y < 0 || x >= state_ptr->w || y >= state_ptr->h)
        return std::nullopt;

    LevelGenRooms* level_rooms = rooms[l];
    return level_rooms->rooms[x + y * 8];
}
bool LevelGenSystem::set_room_template(uint32_t x, uint32_t y, int l, uint16_t room_template)
{
    auto state = State::get();
    auto* state_ptr = state.ptr_local();

    if (x < 0 || y < 0 || x >= state_ptr->w || y >= state_ptr->h)
        return false;

    LevelGenRooms* level_rooms = rooms[l];
    level_rooms->rooms[x + y * 8] = room_template;

    // Unset machine room origin flag if it is a machine room so there's no accidental origins left within the machine room
    if (data->get_room_template_type(room_template) == RoomTemplateType::MachineRoom)
    {
        machine_room_origin->rooms[x + y * 8] = false;
    }

    return true;
}

bool LevelGenSystem::is_room_flipped(uint32_t x, uint32_t y)
{
    auto* state_ptr = State::get().ptr_local();

    if (x < 0 || y < 0 || x >= state_ptr->w || y >= state_ptr->h)
        return false;

    return flipped_rooms->rooms[x + y * 8];
}
bool LevelGenSystem::mark_as_machine_room_origin(uint32_t x, uint32_t y, uint8_t /*l*/)
{
    auto* state_ptr = State::get().ptr_local();

    if (x < 0 || y < 0 || x >= state_ptr->w || y >= state_ptr->h)
        return false;

    machine_room_origin->rooms[x + y * 8] = true;

    return true;
}
bool LevelGenSystem::mark_as_set_room(uint32_t x, uint32_t y, uint8_t l, bool is_set_room)
{
    auto* state_ptr = State::get().ptr_local();

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

std::string_view LevelGenSystem::get_room_template_name(uint16_t room_template)
{
    for (const auto& [name, room_tpl] : data->room_templates)
    {
        if (room_tpl.id == room_template)
        {
            return name;
        }
    }
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

uint32_t LevelGenSystem::get_procedural_spawn_chance(uint32_t chance_id)
{
    if (g_monster_chance_id_to_name.contains(chance_id))
    {
        LevelChanceDef this_chances = get_or_emplace_level_chance(data->level_monster_chances, chance_id);
        if (!this_chances.chances.empty())
        {
            auto* state = State::get().ptr();
            if (this_chances.chances.size() >= state->level)
            {
                return this_chances.chances[state->level];
            }
            else
            {
                return this_chances.chances[0];
            }
        }
    }

    if (g_trap_chance_id_to_name.contains(chance_id))
    {
        LevelChanceDef& this_chances = get_or_emplace_level_chance(data->level_trap_chances, chance_id);
        if (!this_chances.chances.empty())
        {
            auto* state = State::get().ptr();
            if (this_chances.chances.size() >= state->level)
            {
                return this_chances.chances[state->level];
            }
            else
            {
                return this_chances.chances[0];
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

bool default_spawn_is_valid(float x, float y, uint8_t layer)
{
    return g_DefaultTestFunc(x, y, State::get().layer_local(layer));
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

int8_t get_co_subtheme()
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
void force_co_subtheme(int8_t subtheme)
{
    static size_t offset = 0;
    if (offset == 0)
    {
        offset = get_address("cosmic_ocean_subtheme");
    }

    // There isn't enough room to overwrite the result of the random number generation with a `mov r8, <subtheme>`
    // so we overwrite the start of the random number generator with this instruction and then jump to where the
    // subtheme index is used to create the subtheme pointer
    if (subtheme >= 0 && subtheme <= 7)
    {
        uint8_t replacement[] = {0x41, 0xB8, (uint8_t)subtheme, 0x00, 0x00, 0x00, 0xEB, 0x1F, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
        write_mem_prot(offset, replacement, true);
    }
    else if (subtheme == -1)
    {
        write_mem_prot(offset, "\x4C\x8B\x00\x4C\x8B\x48\x08\x48\xBA\x4B\x57\x4C\x4F\x80"sv, true);
    }
}
