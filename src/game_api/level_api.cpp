#include "level_api.hpp"

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

struct CommunityTileCode;
using TileCodeFunc = void(const CommunityTileCode& self, float x, float y, Layer* layer);

struct CommunityTileCode
{
    std::string_view tile_code;
    std::string_view entity_type;
    TileCodeFunc* func = [](const CommunityTileCode& self, float x, float y, Layer* layer)
    {
        layer->spawn_entity(self.entity_id, x, y, false, 0.0f, 0.0f, false);
    };
    std::uint32_t entity_id;
    std::uint32_t tile_code_id;
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
    CommunityTileCode{"ghost", "ENT_TYPE_MONS_GHOST"},
    CommunityTileCode{"ghost_med_sad", "ENT_TYPE_MONS_GHOST_MEDIUM_SAD"},
    CommunityTileCode{"ghost_med_happy", "ENT_TYPE_MONS_GHOST_MEDIUM_HAPPY"},
    CommunityTileCode{"ghost_small_angry", "ENT_TYPE_MONS_GHOST_SMALL_ANGRY"},
    CommunityTileCode{"ghost_small_sad", "ENT_TYPE_MONS_GHOST_SMALL_SAD"},
    CommunityTileCode{"ghost_small_surprised", "ENT_TYPE_MONS_GHOST_SMALL_SURPRISED"},
    CommunityTileCode{"ghost_small_happy", "ENT_TYPE_MONS_GHOST_SMALL_HAPPY"},
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
    CommunityTileCode{
        "eggsac_left",
        "ENT_TYPE_ITEM_EGGSAC",
        [](const CommunityTileCode& self, float x, float y, Layer* layer)
        {
            if (Entity* left = layer->get_grid_entity_at(x - 1.0f, y))
            {
                Entity* eggsac = layer->spawn_entity(self.entity_id, x, y, false, 0.0f, 0.0f, true);
                eggsac->angle = -std::numbers::pi_v<float> / 2.0f;
                attach_entity(left, eggsac);
            }
        },
    },
    CommunityTileCode{
        "eggsac_top",
        "ENT_TYPE_ITEM_EGGSAC",
        [](const CommunityTileCode& self, float x, float y, Layer* layer)
        {
            if (Entity* top = layer->get_grid_entity_at(x, y + 1.0f))
            {
                Entity* eggsac = layer->spawn_entity(self.entity_id, x, y, false, 0.0f, 0.0f, true);
                eggsac->angle = std::numbers::pi_v<float>;
                attach_entity(top, eggsac);
            }
        },
    },
    CommunityTileCode{
        "eggsac_right",
        "ENT_TYPE_ITEM_EGGSAC",
        [](const CommunityTileCode& self, float x, float y, Layer* layer)
        {
            Entity* eggsac = layer->spawn_entity(self.entity_id, x, y, false, 0.0f, 0.0f, true);
            eggsac->angle = std::numbers::pi_v<float> / 2.0f;
            g_floor_requiring_entities.push_back({{{x + 1.0f, y}}, eggsac->uid});
        },
    },
    CommunityTileCode{
        "eggsac_bottom",
        "ENT_TYPE_ITEM_EGGSAC",
        [](const CommunityTileCode& self, float x, float y, Layer* layer)
        {
            Entity* eggsac = layer->spawn_entity(self.entity_id, x, y, false, 0.0f, 0.0f, true);
            g_floor_requiring_entities.push_back({{{x, y - 1.0f}}, eggsac->uid});
        },
    },
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
    //CommunityTileCode{
    //    "lake_imposter",
    //    "ENT_TYPE_LIQUID_IMPOSTOR_LAKE",
    //    [](const CommunityTileCode& self, float x, float y, [[maybe_unused]] Layer* layer)
    //    {
    //        layer->spawn_entity(self.entity_id, x, y, false, 0, 0, true);
    //    },
    //},
    CommunityTileCode{"bubble_platform", "ENT_TYPE_ACTIVEFLOOR_BUBBLE_PLATFORM"},
    CommunityTileCode{"punishball", "ENT_TYPE_ITEM_PUNISHBALL"},
    CommunityTileCode{
        "punishball_attach",
        "ENT_TYPE_ITEM_PUNISHBALL",
        [](const CommunityTileCode& self, float x, float y, Layer* layer)
        {
            std::vector<uint32_t> entities_left = get_entities_overlapping_by_pointer(0, 0, x - 1.5f, y - 0.5f, x - 0.5f, y + 0.5f, layer);
            if (!entities_left.empty())
            {
                get_entity_ptr(attach_ball_and_chain(entities_left.front(), 1.0f, 0.0f));
                return;
            }

            layer->spawn_entity(self.entity_id, x, y, false, 0.0f, 0.0f, true);
        },
    },
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
                slidingwall, [](Entity*, Entity*, void (*)(Entity*, Entity*)) {}, 25);
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
            layer->spawn_apep(x, y, false);
        }},
    CommunityTileCode{
        "apep_right",
        "ENT_TYPE_MONS_APEP_HEAD",
        []([[maybe_unused]] const CommunityTileCode& self, float x, float y, Layer* layer)
        {
            layer->spawn_apep(x, y, true);
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

            Entity* olmite = layer->spawn_entity(self.entity_id, x, y, false, 0.0f, 0.0f, true);

            std::vector<uint32_t> entities_above = get_entities_overlapping_by_pointer(0, 0x4, x - 0.1f, y + 0.9f, x + 0.1f, y + 1.1f, layer);
            for (uint32_t uid : entities_above)
            {
                if (Entity* ent = get_entity_ptr(uid))
                {
                    if (ent->type->id == helmet_id || ent->type->id == naked_id || ent->type->id == self.entity_id)
                    {
                        *(bool*)((size_t)ent + 0x149) = true;
                        *(bool*)((size_t)olmite + 0x149) = true;
                        *(uint32_t*)((size_t)olmite + 0x14c) = ent->uid;

                        float offset[]{0.0f, 0.64f};
                        stack_entities(olmite->uid, ent->uid, offset);
                    }
                }
            }
        },
    },
    //CommunityTileCode{
    //    "telefloor_left",
    //    "ENT_TYPE_FLOOR_TELEPORTINGBORDER",
    //    [](const CommunityTileCode& self, float x, float y, Layer* layer)
    //    {
    //        Entity* telefloor = layer->spawn_entity(self.entity_id, x, y, false, 0.0f, 0.0f, true);
    //        *(uint8_t*)((size_t)telefloor + sizeof(Entity) + sizeof(uint32_t[4])) = 1;
    //    },
    //},
    //CommunityTileCode{
    //    "telefloor_top",
    //    "ENT_TYPE_FLOOR_TELEPORTINGBORDER",
    //    [](const CommunityTileCode& self, float x, float y, Layer* layer)
    //    {
    //        Entity* telefloor = layer->spawn_entity(self.entity_id, x, y, false, 0.0f, 0.0f, true);
    //        *(uint8_t*)((size_t)telefloor + sizeof(Entity) + sizeof(uint32_t[4])) = 3;
    //    },
    //},
    //CommunityTileCode{
    //    "telefloor_right",
    //    "ENT_TYPE_FLOOR_TELEPORTINGBORDER",
    //    [](const CommunityTileCode& self, float x, float y, Layer* layer)
    //    {
    //        Entity* telefloor = layer->spawn_entity(self.entity_id, x, y, false, 0.0f, 0.0f, true);
    //        *(uint8_t*)((size_t)telefloor + sizeof(Entity) + sizeof(uint32_t[4])) = 0;
    //    },
    //},
    //CommunityTileCode{
    //    "telefloor_bottom",
    //    "ENT_TYPE_FLOOR_TELEPORTINGBORDER",
    //    [](const CommunityTileCode& self, float x, float y, Layer* layer)
    //    {
    //        Entity* telefloor = layer->spawn_entity(self.entity_id, x, y, false, 0.0f, 0.0f, true);
    //        *(uint8_t*)((size_t)telefloor + sizeof(Entity) + sizeof(uint32_t[4])) = 2;
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

bool g_replace_level_loads{false};
std::vector<std::string> g_levels_to_load;

//#define HOOK_LOAD_ITEM
#ifdef HOOK_LOAD_ITEM
using LoadItemFun = void*(Layer*, std::uint32_t, float, float, bool);
LoadItemFun* g_load_item_trampoline{nullptr};
void* load_item(Layer* _this, std::uint32_t entity_id, float x, float y, bool some_bool)
{
    return g_load_item_trampoline(_this, entity_id, x, y, some_bool);
}
#endif

using LevelGenFun = void(LevelGenSystem*, float);
LevelGenFun* g_level_gen_trampoline{nullptr};
void level_gen(LevelGenSystem* level_gen_sys, float param_2)
{
    push_spawn_type_flags(SPAWN_TYPE_LEVEL_GEN_GENERAL);
    OnScopeExit pop{[]
                    { pop_spawn_type_flags(SPAWN_TYPE_LEVEL_GEN_GENERAL); }};

    pre_level_generation();
    g_level_gen_trampoline(level_gen_sys, param_2);
    post_level_generation();

    g_replace_level_loads = false;
    g_levels_to_load.clear();
}

using GenRoomsFun = void(ThemeInfo*);
GenRoomsFun* g_gen_rooms_trampoline{nullptr};
void gen_rooms(ThemeInfo* theme)
{
    g_gen_rooms_trampoline(theme);
    post_room_generation();

    {
        std::lock_guard lock{g_extra_spawn_logic_providers_lock};
        for (ExtraSpawnLogicProviderImpl& provider : g_extra_spawn_logic_providers)
        {
            provider.transient_num_remaining_spawns_frontlayer = provider.num_extra_spawns_frontlayer;
            provider.transient_num_remaining_spawns_backlayer = provider.num_extra_spawns_backlayer;
        }
    }
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

        g_floor_requiring_entities.erase(std::remove_if(g_floor_requiring_entities.begin(), g_floor_requiring_entities.end(), [](const FloorRequiringEntity& ent)
                                                        { return ent.handled || get_entity_ptr(ent.uid) == nullptr; }),
                                         g_floor_requiring_entities.end());
    }
}

using SetupLevelFiles = void(LevelGenData*, const char*, bool);
SetupLevelFiles* g_setup_level_files_trampoline{nullptr};
void setup_level_files(LevelGenData* level_gen_data, const char* level_file_name, bool load_generic)
{
    pre_load_level_files();
    g_setup_level_files_trampoline(level_gen_data, level_file_name, load_generic);
}

void get_room_size(uint32_t room_template, uint32_t& room_width, uint32_t& room_height)
{
    // cache, ghistroom
    if (room_template >= 14 && room_template <= 15)
    {
        room_width = 5;
        room_height = 5;
    }
    // chunk
    else if (room_template >= 16 && room_template <= 18)
    {
        room_width = 5;
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
    else if (room_template >= 109 && room_template <= 110)
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

using GenerateRoom = void(LevelGenSystem*, int, int);
GenerateRoom* g_generate_room_trampoline{nullptr};
void generate_room(LevelGenSystem* level_gen, int room_idx_x, int room_idx_y)
{
    const int32_t flat_room_idx = room_idx_x + room_idx_y * 8;
    const uint16_t room_template = level_gen->rooms_frontlayer->rooms[flat_room_idx];
    switch (level_gen->data->get_room_template_type(room_template))
    {
    default:
        break;
    case RoomTemplateType::MachineRoom:
        // Revert the roomtemplate the next time room data will be collected
        g_overridden_room_template = room_template;
        level_gen->rooms_frontlayer->rooms[flat_room_idx] = 109;
        break;
    }
    g_generate_room_trampoline(level_gen, room_idx_x, room_idx_y);
}

using GatherRoomData = void(LevelGenData*, byte, int room_x, int, bool, uint8_t*, uint8_t*, size_t, uint8_t*, uint8_t*, uint8_t*, uint8_t*);
GatherRoomData* g_gather_room_data_trampoline{nullptr};
void gather_room_data(LevelGenData* tile_storage, byte param_2, int room_idx_x, int room_idx_y, bool hard_level, uint8_t* param_6, uint8_t* param_7, size_t param_8, uint8_t* param_9, uint8_t* param_10, uint8_t* out_room_width, uint8_t* out_room_height)
{
    // TODO: Remove debugging code
    //auto datas = tile_storage->room_template_datas();
    //auto rooms = datas[State::get().ptr()->level_gen->rooms_frontlayer->rooms[room_idx_x + room_idx_y * 8]];
    //auto rooms_2 = datas[107];
    //auto rooms_3 = datas[109];
    //auto rooms_4 = datas[267];

    //auto set_rooms = tile_storage->setroom_datas();

    //auto room_template = State::get().ptr()->level_gen->rooms_frontlayer->rooms[room_idx_x + room_idx_y * 8];
    //(void)room_template;

    if (g_overridden_room_template.has_value())
    {
        const int32_t flat_room_idx = room_idx_x + room_idx_y * 8;
        State::get().ptr()->level_gen->rooms_frontlayer->rooms[flat_room_idx] = g_overridden_room_template.value();
        g_overridden_room_template.reset();
    }
    g_gather_room_data_trampoline(tile_storage, param_2, room_idx_x, room_idx_y, hard_level, param_6, param_7, param_8, param_9, param_10, out_room_width, out_room_height);
}

using SpawnRoomFromTileCodes = void(LevelGenData*, int, int, RoomData*, RoomData*, uint16_t, bool, uint16_t);
SpawnRoomFromTileCodes* g_spawn_room_from_tile_codes_trampoline{nullptr};
void spawn_room_from_tile_codes(LevelGenData* level_gen_data, int room_idx_x, int room_idx_y, RoomData* room_data, RoomData* back_room_data, uint16_t param_6, bool dual_room, uint16_t room_template)
{
    // TODO: Remove debugging code
    //const std::string_view name = State::get().ptr()->level_gen->get_room_template_name(room_template);
    //(void)name;

    // TODO: Add hooks to allow manipulating room data
    g_spawn_room_from_tile_codes_trampoline(level_gen_data, room_idx_x, room_idx_y, room_data, back_room_data, param_6, dual_room, room_template);
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
    push_spawn_type_flags(SPAWN_TYPE_LEVEL_GEN_PROCEDURAL);
    OnScopeExit pop{[]
                    { pop_spawn_type_flags(SPAWN_TYPE_LEVEL_GEN_PROCEDURAL); }};

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
        auto& tile_codes_map = tile_codes();

        // Getting the last id like this in case the game decides to skip some ids so that last_id != tile_codes.size()
        auto max_id = 0u;
        for (auto& [name, def] : tile_codes_map)
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

        {
            auto& chances_map = monster_chances();

            // Getting the last id like this in case the game decides to skip some ids so that last_id != chances.size()
            for (auto& [name, def] : chances_map)
            {
                max_id = std::max(def.id, max_id);
                g_monster_chance_id_to_name[def.id] = name;
            }
        }

        {
            auto& chances_map = trap_chances();

            // Getting the last id like this in case the game decides to skip some ids so that last_id != chances.size()
            for (auto& [name, def] : chances_map)
            {
                max_id = std::max(def.id, max_id);
                g_trap_chance_id_to_name[def.id] = name;
            }
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
        auto memory = Memory::get();
        auto exe = memory.exe();
        auto after_bundle = memory.after_bundle;

        {
            auto fun_start = find_inst(exe, "\x48\x8b\x8e\xb8\x12\x00\x00"s, after_bundle);
            fun_start = find_inst(exe, "\x48\x8b\x8e\xb8\x12\x00\x00"s, fun_start);
            fun_start = Memory::decode_call(find_inst(exe, "\xe8"s, fun_start));
            g_level_gen_trampoline = (LevelGenFun*)memory.at_exe(fun_start);

            fun_start = find_inst(exe, "\x48\x0f\x44\xcf\x48\x8b\x49\x6c"s, fun_start);
            fun_start = find_inst(exe, "\x48\x0f\x44\xcf\x48\x8b\x49\x6c"s, fun_start + 1);
            fun_start = Memory::decode_call(find_inst(exe, "\xe8"s, fun_start));
            g_gen_rooms_trampoline = (GenRoomsFun*)memory.at_exe(fun_start);
        }

        {
            auto fun_start = find_inst(exe, "\x44\x0f\xb7\xc5\xf3\x0f\x11\x7c\x24\x20"s, after_bundle);
            fun_start = Memory::decode_call(find_inst(exe, "\xe8"s, fun_start));
            g_handle_tile_code_trampoline = (HandleTileCodeFun*)memory.at_exe(fun_start);
        }

        {
            auto fun_start = find_inst(exe, "\x4c\x8b\xf1\xc6\x81\x40\x01\x00\x00\x01"s, after_bundle);
            fun_start = Memory::decode_call(find_inst(exe, "\xe8"s, fun_start));
            g_setup_level_files_trampoline = (SetupLevelFiles*)memory.at_exe(fun_start);
        }

        {
            auto fun_start = find_inst(exe, "\x49\x8d\x40\x01\x48\x89\x03"s, after_bundle);
            fun_start = Memory::decode_call(find_inst(exe, "\xe8"s, fun_start));
            g_load_level_file_trampoline = (LoadLevelFile*)memory.at_exe(fun_start);

            {
                const void* get_room_size_addr = &get_room_size;
                std::string code = fmt::format(
                    /// Breakpoint for debugging
                    //"\xcc"
                    /// Push all volatile registers
                    "\x50"     // push   rax
                    "\x51"     // push   rcx
                    "\x52"     // push   rdx
                    "\x41\x50" // push   r8
                    "\x41\x51" // push   r9
                    "\x41\x52" // push   r10
                    "\x41\x53" // push   r11
                    /// Setup call
                    "\x89\xd9"                     // mov    ecx, ebx
                    "\x48\x8d\x95\x38\xff\xff\xff" // lea    rdx, [rbp-0x100+0x38]
                    "\x4c\x8d\x85\x3c\xff\xff\xff" // lea    r8, [rbp-0x100+0x3c]
                    /// Do the call with an absolute address
                    "\x48\xb8{}" // mov    rax, &get_room_size
                    "\xff\xd0"   // call   rax
                    /// Recover volatile registers
                    "\x41\x5b" // pop    r11
                    "\x41\x5a" // pop    r10
                    "\x41\x59" // pop    r9
                    "\x41\x58" // pop    r8
                    "\x5a"     // pop    rdx
                    "\x59"     // pop    rcx
                    "\x58"     // pop    rax
                    /// Move room width into its expected register
                    "\x48\x8B\x74\x24\x38" // mov    rsi, QWORD PTR[rsp + 0x38]
                    /// Setup some registers for the next loop iteration
                    "\x44\x8b\x74\x24\x48" // mov    r14d, DWORD PTR[rsp + 0x48]
                    "\x44\x8b\x7c\x24\x4c" // mov    r15d, DWORD PTR[rsp + 0x4c]
                    "\x48\x8b\x5c\x24\x40" // mov    rbx, QWORD PTR[rsp + 0x40]
                    "\x4c\x8b\x54\x24\x30" // mov    r10, QWORD PTR[rsp + 0x30]
                    "\x4c\x8b\x5c\x24\x28" // mov    r11, QWORD PTR[rsp + 0x28]
                    "\x8b\x7c\x24\x20"     // mov    edi, DWORD PTR[rsp + 0x20]
                    ,
                    to_le_bytes(get_room_size_addr));

                // function start, expected at 0x220addd0
                const size_t get_room_size_off = (size_t)g_load_level_file_trampoline + 0x13ad; // at 0x220af17d
                const size_t get_room_size_size = 0x220af32c - 0x220af17d;                      // until 0x220af32c

                // Fill with nop, code is not performance-critical either way
                code.resize(get_room_size_size, '\x90');

                write_mem_prot(get_room_size_off, std::move(code), true);

                // Replace MessageBox call with a breakpoint
                std::string breakpoint = "\x90\x90\xcc"s;
                breakpoint.resize(0x33, '\x90');
                write_mem_prot(memory.at_exe(0x220af42b), std::move(breakpoint), true);
                ////write_mem_prot(memory.at_exe(0x220af42b), "\xcc", true);
            }
        }

        {
            auto fun_start = find_inst(exe, "\x44\x88\x64\x24\x28\x44\x89\x7c\x24\x20"s, after_bundle);
            fun_start = find_inst(exe, "\x44\x88\x64\x24\x28\x44\x89\x7c\x24\x20"s, fun_start + 1);
            fun_start = Memory::decode_call(find_inst(exe, "\xe8"s, fun_start));
            g_do_extra_spawns_trampoline = (DoExtraSpawns*)memory.at_exe(fun_start);
        }

        // TODO: Get offsets from patterns
        {
            g_generate_room_trampoline = (GenerateRoom*)memory.at_exe(0x220cb640);
        }

        {
            g_gather_room_data_trampoline = (GatherRoomData*)memory.at_exe(0x220acc70);
        }

        {
            g_spawn_room_from_tile_codes_trampoline = (SpawnRoomFromTileCodes*)memory.at_exe(0x220cb460);
        }

        DetourRestoreAfterWith();

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());

#ifdef HOOK_LOAD_ITEM
        auto load_item_off = find_inst(exe, "\x48\x89\x5c\x24\x10\x48\x89\x6c\x24\x18\x56\x57\x41\x56\x48\x83\xec\x60\x48\x8b\xf1\x0f\xb6\x01\xc6\x44\x24\x30\x00\x48\xc7\x44\x24\x28\x00\x00\x00\x00\x88\x44\x24\x20"s, after_bundle);
        g_load_item_trampoline = (LoadItemFun*)memory.at_exe(load_item_off);
        DetourAttach((void**)&g_load_item_trampoline, load_item);
#endif

        DetourAttach((void**)&g_level_gen_trampoline, level_gen);
        DetourAttach((void**)&g_gen_rooms_trampoline, gen_rooms);
        DetourAttach((void**)&g_handle_tile_code_trampoline, handle_tile_code);
        DetourAttach((void**)&g_setup_level_files_trampoline, setup_level_files);
        DetourAttach((void**)&g_load_level_file_trampoline, load_level_file);
        DetourAttach((void**)&g_do_extra_spawns_trampoline, do_extra_spawns);
        DetourAttach((void**)&g_generate_room_trampoline, generate_room);
        DetourAttach((void**)&g_gather_room_data_trampoline, gather_room_data);
        DetourAttach((void**)&g_spawn_room_from_tile_codes_trampoline, spawn_room_from_tile_codes);

        const LONG error = DetourTransactionCommit();
        if (error != NO_ERROR)
        {
            DEBUG("Failed hooking HandleTileCode: {}\n", error);
        }
    }

    {
        auto memory = Memory::get();
        auto exe = memory.exe();
        auto after_bundle = memory.after_bundle;

        auto off = find_inst(exe, "\xba\xee\x00\x00\x00\x48\x8d\x0c\x18"s, after_bundle);
        auto fun_start = Memory::decode_call(find_inst(exe, "\xe8"s, off));
        g_test_chance = (TestChance*)memory.at_exe(fun_start);
    }
}

std::optional<std::uint32_t> LevelGenData::get_tile_code(const std::string& tile_code)
{
    auto& tile_codes_map = tile_codes();
    auto it = tile_codes_map.find(tile_code);
    if (it != tile_codes_map.end())
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

    using string_t = std::basic_string<char, std::char_traits<char>, game_allocator<char>>;
    using map_value_t = std::pair<const string_t, TileCodeDef>;
    using map_allocator_t = game_allocator<map_value_t>;
    using mutable_tile_code_map_t = std::unordered_map<string_t, TileCodeDef, std::hash<string_t>, std::equal_to<string_t>, map_allocator_t>;
    auto& tile_code_map = (mutable_tile_code_map_t&)tile_codes();

    auto [it, success] = tile_code_map.emplace(std::move(tile_code), TileCodeDef{g_current_tile_code_id});
    g_current_tile_code_id++;

    g_tile_code_id_to_name[it->second.id] = it->first;
    return it->second.id;
}

std::optional<std::uint32_t> LevelGenData::get_chance(const std::string& chance)
{
    {
        auto& chances_map = monster_chances();
        auto it = chances_map.find(chance);
        if (it != chances_map.end())
        {
            return it->second.id;
        }
    }
    {
        auto& chances_map = trap_chances();
        auto it = chances_map.find(chance);
        if (it != chances_map.end())
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

    using string_t = std::basic_string<char, std::char_traits<char>, game_allocator<char>>;
    using map_value_t = std::pair<const string_t, ChanceDef>;
    using map_allocator_t = game_allocator<map_value_t>;
    using mutable_chance_map_t = std::unordered_map<string_t, ChanceDef, std::hash<string_t>, std::equal_to<string_t>, map_allocator_t>;

    // We use only monster chances to define new stuff, keep an eye out for whether this is dangerous
    auto& chance_map = (mutable_chance_map_t&)monster_chances();

    auto [it, success] = chance_map.emplace(std::move(chance), ChanceDef{g_current_chance_id});
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
    {
        auto& room_templates_map = room_templates();
        auto it = room_templates_map.find(room_template);
        if (it != room_templates_map.end())
        {
            return it->second.id;
        }
    }
    return {};
}
std::uint16_t LevelGenData::define_room_template(std::string room_template, RoomTemplateType type)
{
    if (auto existing = get_room_template(room_template))
    {
        return existing.value();
    }

    using string_t = std::basic_string<char, std::char_traits<char>, game_allocator<char>>;
    using map_value_t = std::pair<const string_t, RoomTemplateDef>;
    using map_allocator_t = game_allocator<map_value_t>;
    using mutable_room_template_map_t = std::unordered_map<string_t, RoomTemplateDef, std::hash<string_t>, std::equal_to<string_t>, map_allocator_t>;
    auto& room_template_map = (mutable_room_template_map_t&)room_templates();

    auto [it, success] = room_template_map.emplace(std::move(room_template), RoomTemplateDef{(uint16_t)room_template_map.size()});

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
        hook_vtable<DoProceduralSpawnFun>(
            theme, [](ThemeInfo* self, SpawnInfo* spawn_info, DoProceduralSpawnFun* original)
            {
                if (handle_chance(spawn_info))
                {
                    return;
                }
                original(self, spawn_info);
            },
            0x32);
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
std::optional<uint16_t> LevelGenSystem::get_room_template(uint32_t x, uint32_t y, LAYER l)
{
    auto state = State::get();
    auto* state_ptr = state.ptr_local();

    if (x < 0 || y < 0 || x >= state_ptr->w || y >= state_ptr->h)
        return std::nullopt;

    uint8_t layer = enum_to_layer(l);

    LevelGenRooms* level_rooms = rooms[layer];
    return level_rooms->rooms[x + y * 8];
}
bool LevelGenSystem::set_room_template(uint32_t x, uint32_t y, LAYER l, uint16_t room_template)
{
    auto state = State::get();
    auto* state_ptr = state.ptr_local();

    if (x < 0 || y < 0 || x >= state_ptr->w || y >= state_ptr->h)
        return false;

    uint8_t layer = enum_to_layer(l);

    LevelGenRooms* level_rooms = rooms[layer];
    level_rooms->rooms[x + y * 8] = room_template;

    return true;
}
bool LevelGenSystem::mark_as_machine_room_origin(uint32_t x, uint32_t y, LAYER /*l*/)
{
    auto state = State::get();
    auto* state_ptr = state.ptr_local();

    if (x < 0 || y < 0 || x >= state_ptr->w || y >= state_ptr->h)
        return false;

    //uint8_t layer = enum_to_layer(l);

    LevelGenRoomsMeta* level_rooms = machine_room_origin;
    level_rooms->rooms[x + y * 8] = true;

    return true;
}

std::string_view LevelGenSystem::get_room_template_name(uint16_t room_template)
{
    for (const auto& [name, room_tpl] : data->room_templates())
    {
        if (room_tpl.id == room_template)
        {
            return name;
        }
    }
    return "invalid";
}

struct MutableLevelChanceDef
{
    std::vector<uint32_t, game_allocator<uint32_t>> chances;
};
MutableLevelChanceDef& get_or_emplace_level_chance(std::unordered_map<std::uint32_t, LevelChanceDef>& level_chances, uint32_t chance_id)
{
    struct LevelChanceNode
    {
        void* ptr0;
        void* ptr1;
        std::pair<uint32_t, MutableLevelChanceDef> value;
    };
    using EmplaceLevelChance = LevelChanceNode** (*)(void*, std::pair<LevelChanceNode*, bool>*, uint32_t*);
    static EmplaceLevelChance emplace_level_chance = []()
    {
        auto memory = Memory::get();
        auto off = find_inst(memory.exe(), "\x49\x8d\x8d\x70\x13\x00\x00"s, memory.after_bundle);
        off = find_inst(memory.exe(), "\xe8"s, off);
        return (EmplaceLevelChance)memory.at_exe(Memory::decode_call(off));
    }();

    std::pair<LevelChanceNode*, bool> node;
    emplace_level_chance((void*)&level_chances, &node, &chance_id);

    return node.first->value.second;
}

uint32_t LevelGenSystem::get_procedural_spawn_chance(uint32_t chance_id)
{
    if (g_monster_chance_id_to_name.contains(chance_id))
    {
        MutableLevelChanceDef& this_chances = get_or_emplace_level_chance((std::unordered_map<std::uint32_t, LevelChanceDef>&)data->level_monster_chances(), chance_id);
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
        MutableLevelChanceDef& this_chances = get_or_emplace_level_chance((std::unordered_map<std::uint32_t, LevelChanceDef>&)data->level_trap_chances(), chance_id);
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
        MutableLevelChanceDef& this_chances = get_or_emplace_level_chance((std::unordered_map<std::uint32_t, LevelChanceDef>&)data->level_monster_chances(), chance_id);
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
        MutableLevelChanceDef& this_chances = get_or_emplace_level_chance((std::unordered_map<std::uint32_t, LevelChanceDef>&)data->level_trap_chances(), chance_id);
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

bool default_is_valid_spawn(float x, float y, uint8_t layer)
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
        auto memory = Memory::get();
        offset = memory.at_exe(find_inst(memory.exe(), "\x48\xC1\xE0\x03\x48\xC1\xE8\x20\x49\x89\x48\x08\x48\x98"s, memory.after_bundle));
    }
    if (subtheme >= 0 && subtheme <= 7)
    {
        uint8_t replacement[] = {0xB8, (uint8_t)subtheme, 0x00, 0x00, 0x00, 0x90, 0x90, 0x90};
        std::string replacement_s = std::string((char*)replacement, sizeof(replacement));
        write_mem_prot(offset, replacement_s, true);
    }
    else if (subtheme == -1)
    {
        write_mem_prot(offset, "\x48\xC1\xE0\x03\x48\xC1\xE8\x20"s, true);
    }
}
