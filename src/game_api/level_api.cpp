#include "level_api.hpp"

#include "entity.hpp"
#include "layer.hpp"
#include "logger.h"
#include "memory.hpp"
#include "rpc.hpp"
#include "script.hpp"

#include <array>
#include <string_view>

#include <Windows.h>
#include <detours.h>

std::uint32_t g_last_tile_code_id;
std::uint32_t g_last_community_tile_code_id;
std::uint32_t g_current_tile_code_id;

std::unordered_map<std::uint32_t, std::string_view> g_IdToName;

struct FloorRequiringEntity
{
    float x;
    float y;
    std::int32_t uid;
    bool handled;
};
std::vector<FloorRequiringEntity> g_floor_requiring_entities;

struct CommunityTileCode;
using TileCodeFunc = void(const CommunityTileCode& self, float x, float y, int layer);

struct CommunityTileCode
{
    std::string_view tile_code;
    std::string_view entity_type;
    TileCodeFunc* func = [](const CommunityTileCode& self, float x, float y, int layer) {
        auto* layer_ptr = State::get().layer(layer);
        layer_ptr->spawn_entity(self.entity_id, x, y, false, 0.0f, 0.0f, true);
    };
    std::uint32_t entity_id;
    std::uint32_t tile_code_id;
};
std::array g_community_tile_codes{
    CommunityTileCode{
        "totem_trap",
        "ENT_TYPE_FLOOR_TOTEM_TRAP",
        [](const CommunityTileCode& self, float x, float y, int layer) {
            auto* layer_ptr = State::get().layer(layer);
            Entity* bottom = layer_ptr->spawn_entity(self.entity_id, x, y, false, 0.0f, 0.0f, true);
            layer_ptr->spawn_entity_over(self.entity_id, bottom, 0.0f, 1.0f);
            g_floor_requiring_entities.push_back({x, y - 1.0f, bottom->uid});
        },
    },
    CommunityTileCode{
        "lion_trap",
        "ENT_TYPE_FLOOR_LION_TRAP",
        [](const CommunityTileCode& self, float x, float y, int layer) {
            auto* layer_ptr = State::get().layer(layer);
            Entity* bottom = layer_ptr->spawn_entity(self.entity_id, x, y, false, 0.0f, 0.0f, true);
            layer_ptr->spawn_entity_over(self.entity_id, bottom, 0.0f, 1.0f);
            g_floor_requiring_entities.push_back({x, y - 1.0f, bottom->uid});
        },
    },
    CommunityTileCode{"cog_door", "ENT_TYPE_FLOOR_DOOR_COG"},
    CommunityTileCode{"dustwall", "ENT_TYPE_FLOOR_DUSTWALL"},
    CommunityTileCode{"bat", "ENT_TYPE_MONS_BAT"},
    CommunityTileCode{"skeleton", "ENT_TYPE_MONS_SKELETON"},
    CommunityTileCode{"lizard", "ENT_TYPE_MONS_HORNEDLIZARD"},
    CommunityTileCode{"mole", "ENT_TYPE_MONS_MOLE"},
    CommunityTileCode{"monkey", "ENT_TYPE_MONS_MONKEY"},
    CommunityTileCode{"firebug", "ENT_TYPE_MONS_FIREBUG"},
    CommunityTileCode{"vampire", "ENT_TYPE_MONS_VAMPIRE"},
    CommunityTileCode{"osrirs", "ENT_TYPE_MONS_OSIRIS_HEAD"},
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
    CommunityTileCode{"boombox", "ENT_TYPE_ITEM_BOOMBOX"},
    CommunityTileCode{"eggplant", "ENT_TYPE_ITEM_EGGPLANT"},
    CommunityTileCode{"gold_bar", "ENT_TYPE_ITEM_GOLDBAR"},
    CommunityTileCode{"diamond", "ENT_TYPE_ITEM_DIAMOND"},
    CommunityTileCode{"emerald", "ENT_TYPE_ITEM_EMERALD"},
    CommunityTileCode{"sapphire", "ENT_TYPE_ITEM_SAPPHIRE"},
    CommunityTileCode{"ruby", "ENT_TYPE_ITEM_RUBY"},
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
    CommunityTileCode{"damsel_monty", "ENT_TYPE_MONS_PET_DOG"},
    CommunityTileCode{"damsel_percy", "ENT_TYPE_MONS_PET_CAT"},
    CommunityTileCode{"damsel_poochi", "ENT_TYPE_MONS_PET_HAMSTER"},
    CommunityTileCode{"rope", "ENT_TYPE_ITEM_UNROLLED_ROPE"},
    CommunityTileCode{"cosmic_orb", "ENT_TYPE_ITEM_FLOATING_ORB"},
    CommunityTileCode{"monkey_gold", "ENT_TYPE_MONS_GOLDMONKEY"},
    CommunityTileCode{"altar_duat", "ENT_TYPE_FLOOR_DUAT_ALTAR"},
    CommunityTileCode{"spikeball", "ENT_TYPE_ACTIVEFLOOR_UNCHAINED_SPIKEBALL"},
};

using HandleTileCodeFun = void(LevelGenSystem*, std::uint32_t, std::uint64_t, float, float, std::uint8_t);
HandleTileCodeFun* g_handle_tile_code_trampoline{nullptr};
void handle_tile_code(LevelGenSystem* _this, std::uint32_t tile_code, std::uint64_t _ull_0, float x, float y, std::uint8_t layer)
{
    std::string_view tile_code_name = g_IdToName[tile_code];

    {
        bool block_spawn = false;
        SpelunkyScript::for_each_script([&](SpelunkyScript& script) {
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
        const CommunityTileCode& community_tile_code = g_community_tile_codes[tile_code - g_last_tile_code_id - 1];
        community_tile_code.func(community_tile_code, x, y, layer);
    }
    else
    {
        g_handle_tile_code_trampoline(_this, tile_code, _ull_0, x, y, layer);
    }

    SpelunkyScript::for_each_script([&](SpelunkyScript& script) {
        script.post_level_gen_spawn(tile_code_name, x, y, layer);
        return true;
    });

    if (!g_floor_requiring_entities.empty())
    {
        std::optional<std::vector<std::uint32_t>> floors;
        for (auto& pending_entity : g_floor_requiring_entities)
        {
            if (std::abs(pending_entity.x - x) < 0.01f && std::abs(pending_entity.y - y) < 0.01f)
            {
                if (auto* entity = get_entity_ptr(pending_entity.uid))
                {
                    if (!floors)
                    {
                        floors = get_entities_overlapping(0, 0x100 /*MASK.FLOOR*/, x - 0.1f, y - 0.1f, x + 0.1f, y + 0.1f, layer);
                    }

                    if (!floors->empty())
                    {
                        entity->overlay = get_entity_ptr(floors->at(0));
                        entity->x = 0.0f;
                        entity->y = 1.0f;
                        pending_entity.handled = true;
                    }
                }
            }
        }

        g_floor_requiring_entities.erase(std::remove_if(g_floor_requiring_entities.begin(), g_floor_requiring_entities.end(), [](const FloorRequiringEntity& ent) { return ent.handled || get_entity_ptr(ent.uid) == nullptr; }), g_floor_requiring_entities.end());
    }
}

void LevelGenSystem::init()
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

        auto off = find_inst(exe, "\x0f\xb6\xbc\x24\x28\x01\x00\x00"s, after_bundle) - 0x30;
        auto fun_start = find_inst(exe, "\x4c\x8b\xdc"s, off);

        g_handle_tile_code_trampoline = (HandleTileCodeFun*)memory.at_exe(fun_start);

        DetourRestoreAfterWith();

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());

        DetourAttach((void**)&g_handle_tile_code_trampoline, handle_tile_code);

        const LONG error = DetourTransactionCommit();
        if (error != NO_ERROR)
        {
            DEBUG("Failed hooking HandleTileCode: {}\n", error);
        }
    }
}

std::optional<std::uint32_t> LevelGenSystem::get_tile_code(const std::string& tile_code)
{
    auto& tile_codes_map = tile_codes();
    auto it = tile_codes_map.find(tile_code);
    if (it != tile_codes_map.end())
    {
        return it->second.id;
    }
    return {};
}
std::uint32_t LevelGenSystem::define_tile_code(std::string tile_code)
{
    if (auto existing = get_tile_code(tile_code))
    {
        return existing.value();
    }

    auto& tile_code_map = *(std::unordered_map<std::string, TileCodeDef>*)((size_t)this + 0x88);

    // TODO: This should be forwarded to the instantiation of this operator in Spel2.exe to avoid CRT mismatch in Debug mode
    auto [it, success] = tile_code_map.emplace(std::move(tile_code), TileCodeDef{g_current_tile_code_id});
    g_current_tile_code_id++;

    g_IdToName[it->second.id] = it->first;
    return it->second.id;
}
