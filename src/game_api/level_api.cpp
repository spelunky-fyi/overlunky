#include "level_api.hpp"

#include "entity.hpp"
#include "game_allocator.hpp"
#include "layer.hpp"
#include "logger.h"
#include "memory.hpp"
#include "rpc.hpp"
#include "script.hpp"
#include "spawn_api.hpp"
#include "util.hpp"
#include "vtable_hook.hpp"

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
            static const auto chain_id = to_id("ENT_TYPE_ITEM_PUNISHCHAIN");

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
        [](const CommunityTileCode& self, float x, float y, Layer* layer)
        {
            layer->spawn_apep(x, y, false);
        }},
    CommunityTileCode{
        "apep_right",
        "ENT_TYPE_MONS_APEP_HEAD",
        [](const CommunityTileCode& self, float x, float y, Layer* layer)
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
    ChanceValidPlacementFunc* test_func = [](const CommunityChance& self, float x, float y, Layer* layer)
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
        [](const CommunityChance& self, float x, float y, Layer* layer)
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
        [](const CommunityChance& self, float x, float y, Layer* layer)
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
    ChanceLogicProvider provider;
};
std::mutex g_chance_logic_providers_lock;
std::uint32_t g_current_chance_logic_provider_id{0};
std::vector<ChanceLogicProviderImpl> g_chance_logic_providers;

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
    auto state = State::get().ptr();
    g_level_gen_trampoline(level_gen_sys, param_2);

    SpelunkyScript::for_each_script(
        [&](SpelunkyScript& script)
        {
            script.post_level_generation();
            return true;
        });
}

using GenRoomsFun = void(ThemeInfo*);
GenRoomsFun* g_gen_rooms_trampoline{nullptr};
void gen_rooms(ThemeInfo* theme)
{
    g_gen_rooms_trampoline(theme);

    SpelunkyScript::for_each_script(
        [&](SpelunkyScript& script)
        {
            script.post_room_generation();
            return true;
        });
}

using HandleTileCodeFun = void(LevelGenSystem*, std::uint32_t, std::uint64_t, float, float, std::uint8_t);
HandleTileCodeFun* g_handle_tile_code_trampoline{nullptr};
void handle_tile_code(LevelGenSystem* _this, std::uint32_t tile_code, std::uint64_t _ull_0, float x, float y, std::uint8_t layer)
{
    push_spawn_type_flags(SPAWN_TYPE_LEVEL_GEN_TILE_CODE);
    OnScopeExit pop{[]
                    { pop_spawn_type_flags(SPAWN_TYPE_LEVEL_GEN_TILE_CODE); }};

    std::string_view tile_code_name = g_tile_code_id_to_name[tile_code];

    {
        bool block_spawn = false;
        SpelunkyScript::for_each_script(
            [&](SpelunkyScript& script)
            {
                block_spawn = script.pre_level_gen_spawn(tile_code_name, x, y, layer);
                if (block_spawn)
                    return false;
                return true;
            });
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
        g_handle_tile_code_trampoline(_this, tile_code, _ull_0, x, y, layer);
    }

    SpelunkyScript::for_each_script(
        [&](SpelunkyScript& script)
        {
            script.post_level_gen_spawn(tile_code_name, x, y, layer);
            return true;
        });

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

using TestChance = bool(LevelGenData**, std::uint32_t chance_id);
TestChance* g_test_chance{nullptr};

struct SpawnInfo
{
    void* ptr0;
    void* ptr1;
    float x;
    float y;
};
bool handle_chance(ThemeInfo* theme, SpawnInfo* spawn_info)
{
    push_spawn_type_flags(SPAWN_TYPE_LEVEL_GEN_PROCEDURAL);
    OnScopeExit pop{[]
                    { pop_spawn_type_flags(SPAWN_TYPE_LEVEL_GEN_PROCEDURAL); }};

    auto level_gen_data = State::get().ptr()->level_gen->data;

    int layer = 0;
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
            fun_start = decode_call(find_inst(exe, "\xe8"s, fun_start));
            g_level_gen_trampoline = (LevelGenFun*)memory.at_exe(fun_start);

            fun_start = find_inst(exe, "\x48\x0f\x44\xcf\x48\x8b\x49\x6c"s, fun_start);
            fun_start = find_inst(exe, "\x48\x0f\x44\xcf\x48\x8b\x49\x6c"s, fun_start + 1);
            fun_start = decode_call(find_inst(exe, "\xe8"s, fun_start));
            g_gen_rooms_trampoline = (GenRoomsFun*)memory.at_exe(fun_start);
        }

        {
            auto fun_start = find_inst(exe, "\x44\x0f\xb7\xc5\xf3\x0f\x11\x7c\x24\x20"s, after_bundle);
            fun_start = decode_call(find_inst(exe, "\xe8"s, fun_start));
            g_handle_tile_code_trampoline = (HandleTileCodeFun*)memory.at_exe(fun_start);
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
        auto fun_start = decode_call(find_inst(exe, "\xe8"s, off));
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

std::uint32_t LevelGenData::register_chance_logic_provider(std::uint32_t chance_id, ChanceLogicProvider provider)
{
    if (provider.is_valid == nullptr)
    {
        provider.is_valid = [](float x, float y, int layer)
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

using GenerateRoomsFun = void(ThemeInfo*);
using DoProceduralSpawnFun = void(ThemeInfo*, SpawnInfo*);
void LevelGenSystem::init()
{
    data->init();

    for (ThemeInfo* theme : themes)
    {
        hook_vtable<DoProceduralSpawnFun>(
            theme, [](ThemeInfo* self, SpawnInfo* spawn_info, DoProceduralSpawnFun* original)
            {
                if (handle_chance(self, spawn_info))
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
std::pair<float, float> LevelGenSystem::get_room_pos(int x, int y)
{
    return std::pair<float, float>{
        static_cast<float>(x * 10) + 2.5f,
        122.5f - static_cast<float>(y * 8)};
}
std::optional<uint16_t> LevelGenSystem::get_room_template(int x, int y, int l)
{
    auto state = State::get();
    auto* state_ptr = state.ptr_local();

    if (x < 0 || y < 0 || x >= state_ptr->w || y >= state_ptr->h)
        return std::nullopt;

    if (l < 0)
    {
        auto player = state.items()->player(abs(l) - 1);
        if (player == nullptr)
            return std::nullopt;
        l = player->layer;
    }

    if (l >= 2)
        return std::nullopt;

    LevelGenRooms* level_rooms = rooms[l];
    return level_rooms->rooms[x + y * 8];
}
bool LevelGenSystem::set_room_template(int x, int y, int l, uint16_t room_template)
{
    auto state = State::get();
    auto* state_ptr = state.ptr_local();

    if (x < 0 || y < 0 || x >= state_ptr->w || y >= state_ptr->h)
        return false;

    if (l < 0)
    {
        auto player = state.items()->player(abs(l) - 1);
        if (player == nullptr)
            return false;
        l = player->layer;
    }

    if (l >= 2)
        return false;

    LevelGenRooms* level_rooms = rooms[l];
    level_rooms->rooms[x + y * 8] = room_template;

    static auto udjat_top = data->room_templates().at("udjattop").id;
    if (l == 1)
    {
        backlayer_room_exists->rooms[x + y * 8] = room_template != 0 && room_template != udjat_top;
        if (room_template == udjat_top)
        {
            rooms_meta_26->rooms[x + y * 8] = false;
        }
    }
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
        return (EmplaceLevelChance)memory.at_exe(decode_call(off));
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
        offset = memory.at_exe(find_inst(memory.exe(), " 48 C1 E0 03 48 C1 E8 20 49 89 48 08 48 98"s, memory.after_bundle));
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
