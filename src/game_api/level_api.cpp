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

std::unordered_map<std::uint32_t, std::string_view> g_IdToName;

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
            layer->spawn_entity_over(anchor_id, web, 0.0f, 0.0f);
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
    CommunityTileCode{
        "lake_imposter",
        "ENT_TYPE_LIQUID_IMPOSTOR_LAKE",
        [](const CommunityTileCode& self, float x, float y, [[maybe_unused]] Layer* layer)
        {
            spawn_liquid(self.entity_id, x, y);
        },
    },
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
};

//#define HOOK_LOAD_ITEM
#ifdef HOOK_LOAD_ITEM
using LoadItemFun = void*(Layer*, std::uint32_t, float, float, bool);
LoadItemFun* g_load_item_trampoline{nullptr};
void* load_item(Layer* _this, std::uint32_t entity_id, float x, float y, bool some_bool)
{
    return g_load_item_trampoline(_this, entity_id, x, y, some_bool);
}
#endif

using HandleTileCodeFun = void(LevelGenSystem*, std::uint32_t, std::uint64_t, float, float, std::uint8_t);
HandleTileCodeFun* g_handle_tile_code_trampoline{nullptr};
void handle_tile_code(LevelGenSystem* _this, std::uint32_t tile_code, std::uint64_t _ull_0, float x, float y, std::uint8_t layer)
{
    push_spawn_type_flags(SPAWN_TYPE_LEVEL_GEN);
    OnScopeExit pop{[]
                    { pop_spawn_type_flags(SPAWN_TYPE_LEVEL_GEN); }};

    std::string_view tile_code_name = g_IdToName[tile_code];

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

void LevelGenData::init()
{
    {
        auto& tile_codes_map = tile_codes();

        // Getting the last id like this in case the game decides to skip some ids so that last_id != tile_codes.size()
        auto max_id = 0u;
        for (auto& [name, def] : tile_codes_map)
        {
            max_id = std::max(def.id, max_id);
            g_IdToName[def.id] = name;
        }

        // The game uses last id to check if the tilecode is valid using a != instead of a <
        // So we can safely use anything larger than last tile id
        g_last_tile_code_id = max_id + 1;
    }

    g_current_tile_code_id = g_last_tile_code_id + 1;

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

    // Remember this for fast access later
    g_last_community_tile_code_id = g_current_tile_code_id;

    {
        auto memory = Memory::get();
        auto exe = memory.exe();
        auto after_bundle = memory.after_bundle;

        auto off = find_inst(exe, "\x44\x0f\xb7\xc5\xf3\x0f\x11\x7c\x24\x20"s, after_bundle);
        auto fun_start = decode_pc(exe, find_inst(exe, "\xe8"s, off), 1);

        g_handle_tile_code_trampoline = (HandleTileCodeFun*)memory.at_exe(fun_start);

        DetourRestoreAfterWith();

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());

#ifdef HOOK_LOAD_ITEM
        auto load_item_off = find_inst(exe, "\x48\x89\x5c\x24\x10\x48\x89\x6c\x24\x18\x56\x57\x41\x56\x48\x83\xec\x60\x48\x8b\xf1\x0f\xb6\x01\xc6\x44\x24\x30\x00\x48\xc7\x44\x24\x28\x00\x00\x00\x00\x88\x44\x24\x20"s, after_bundle);
        g_load_item_trampoline = (LoadItemFun*)memory.at_exe(load_item_off);
        DetourAttach((void**)&g_load_item_trampoline, load_item);
#endif

        DetourAttach((void**)&g_handle_tile_code_trampoline, handle_tile_code);

        const LONG error = DetourTransactionCommit();
        if (error != NO_ERROR)
        {
            DEBUG("Failed hooking HandleTileCode: {}\n", error);
        }
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
    auto& tile_code_map = *(mutable_tile_code_map_t*)((size_t)this + 0x88);

    // TODO: This should be forwarded to the instantiation of this operator in Spel2.exe to avoid CRT mismatch in Debug mode
    auto [it, success] = tile_code_map.emplace(std::move(tile_code), TileCodeDef{g_current_tile_code_id});
    g_current_tile_code_id++;

    g_IdToName[it->second.id] = it->first;
    return it->second.id;
}
