#include "state.hpp"

#include <Windows.h>   // for GetCurrentThread, LONG, NO_ERROR
#include <cmath>       // for abs
#include <cstdlib>     // for size_t, abs
#include <detours.h>   // for DetourAttach, DetourTransactionBegin
#include <functional>  // for _Func_class, function
#include <new>         // for operator new
#include <string>      // for allocator, operator""sv, operator""s
#include <type_traits> // for move

#include "bucket.hpp"                            // for Bucket
#include "containers/custom_allocator.hpp"       //
#include "entities_chars.hpp"                    // for Player
#include "entity.hpp"                            // for to_id, Entity, HookWithId, EntityDB
#include "entity_hooks_info.hpp"                 // for Player
#include "game_api.hpp"                          //
#include "game_manager.hpp"                      // for get_game_manager, GameManager, SaveR...
#include "game_patches.hpp"                      //
#include "items.hpp"                             // for Items, SelectPlayerSlot
#include "level_api.hpp"                         // for LevelGenSystem, LevelGenSystem::(ano...
#include "logger.h"                              // for DEBUG
#include "memory.hpp"                            // for write_mem_prot, memory_read
#include "mod_api.hpp"                           // for savedata
#include "movable.hpp"                           // for Movable
#include "movable_behavior.hpp"                  // for init_behavior_hooks
#include "render_api.hpp"                        // for init_render_api_hooks
#include "savedata.hpp"                          // for SaveData
#include "screen.hpp"                            // for Screen
#include "script/lua_vm.hpp"                     // for get_lua_vm
#include "script/usertypes/theme_vtable_lua.hpp" // for NThemeVTables
#include "search.hpp"                            // for get_address
#include "sound_manager.hpp"                     //
#include "spawn_api.hpp"                         // for init_spawn_hooks
#include "steam_api.hpp"                         // for init_achievement_hooks
#include "strings.hpp"                           // for strings_init
#include "thread_utils.hpp"                      // for OnHeapPointer
#include "virtual_table.hpp"                     // for get_virtual_function_address, VTABLE...
#include "vtable_hook.hpp"                       // for hook_vtable

uint16_t StateMemory::get_correct_ushabti() // returns animation_frame of ushabti
{
    return (correct_ushabti + (correct_ushabti / 10) * 2);
}
void StateMemory::set_correct_ushabti(uint16_t animation_frame)
{
    correct_ushabti = static_cast<uint8_t>(animation_frame - (animation_frame / 12) * 2);
}
StateMemory* get_state_ptr()
{
    return State::ptr();
}
void fix_liquid_out_of_bounds()
{
    auto state = State::ptr();
    if (!state || !state->liquid_physics)
        return;

    for (const auto& it : state->liquid_physics->pools)
    {
        if (it.physics_engine && !it.physics_engine->pause_physics)
        {
            for (uint32_t i = 0; i < it.physics_engine->entity_count; ++i)
            {
                auto liquid_coordinates = it.physics_engine->entity_coordinates + i;
                if (liquid_coordinates->second < 0                      // y < 0
                    || liquid_coordinates->first < 0                    // x < 0
                    || liquid_coordinates->first > g_level_max_x        // x > g_level_max_x
                    || liquid_coordinates->second > g_level_max_y + 16) // y > g_level_max_y
                {
                    if (!*(it.physics_engine->unknown61 + i)) // just some bs
                        continue;

                    const auto ent = **(it.physics_engine->unknown61 + i);
                    ent->kill(true, nullptr);
                }
            }
        }
    }
}

StateMemory* State::ptr_main()
{
    return &reinterpret_cast<State*>(heap_base())->state;
}

StateMemory* State::ptr()
{
    return ptr_local();
}

StateMemory* State::ptr_local()
{
    State* state = reinterpret_cast<State*>(local_heap_base());
    return state != nullptr ? &state->state : nullptr;
}

State* State::get_main()
{
    return reinterpret_cast<State*>(heap_base());
}

State* State::get()
{
    return reinterpret_cast<State*>(local_heap_base());
}

float get_zoom_level()
{
    auto game_api = GameAPI::get();
    return game_api->get_current_zoom();
}

std::pair<float, float> StateMemory::click_position(float x, float y)
{
    float cz = get_zoom_level();
    auto [cx, cy] = get_camera_position();
    float rx = cx + ZF * cz * x;
    float ry = cy + (ZF / 16.0f * 9.0f) * cz * y;
    return {rx, ry};
}

std::pair<float, float> StateMemory::screen_position(float x, float y)
{
    float cz = get_zoom_level();
    auto [cx, cy] = get_camera_position();
    float rx = (x - cx) / cz / ZF;
    float ry = (y - cy) / cz / (ZF / 16.0f * 9.0f);
    return {rx, ry};
}

void State::zoom(float level)
{
    if (level == 0.0)
    {
        auto roomx = ptr()->w;
        switch (roomx)
        {
        case 1:
            level = 9.522f;
            break;
        case 2:
            level = 16.324f;
            break;
        case 3:
            level = 23.126f;
            break;
        case 4:
            level = 29.928f;
            break;
        case 5:
            level = 36.730f;
            break;
        case 6:
            level = 43.532f;
            break;
        case 7:
            level = 50.334f;
            break;
        case 8:
            level = 57.135f;
            break;
        default:
            level = 13.5f;
        }
    }

    static const auto zoom_level = get_address("default_zoom_level");
    static const auto zoom_shop = get_address("default_zoom_level_shop");
    static const auto zoom_camp = get_address("default_zoom_level_camp");
    static const auto zoom_telescope = get_address("default_zoom_level_telescope");

    // overwrite the defaults
    write_mem_recoverable<float>("zoom", zoom_level, level, true);
    write_mem_recoverable<float>("zoom", zoom_shop, level, true);
    write_mem_recoverable<float>("zoom", zoom_camp, level, true);
    write_mem_recoverable<float>("zoom", zoom_telescope, level, true);

    // overwrite the current value
    auto game_api = GameAPI::get();
    game_api->set_zoom(std::nullopt, level);
}

void State::zoom_reset()
{
    recover_mem("zoom");
    auto game_api = GameAPI::get();
    game_api->set_zoom(std::nullopt, 13.5f);
}

void StateMemory::force_current_theme(THEME t)
{
    if (t > 0 && t < 19)
    {
        if (t == 10 && !this->level_gen->theme_cosmicocean->sub_theme)
            this->level_gen->theme_cosmicocean->sub_theme = this->level_gen->theme_dwelling; // just set it to something, can't edit this atm
        this->current_theme = this->level_gen->themes[t - 1];
    }
}

std::pair<float, float> StateMemory::get_camera_position()
{
    static const auto addr = (float*)get_address("camera_position");
    auto cx = *addr;
    auto cy = *(addr + 1);
    return {cx, cy};
}

void StateMemory::set_camera_position(float cx, float cy)
{
    static const auto addr = (float*)get_address("camera_position");
    auto cam = this->camera;
    cam->focus_x = cx;
    cam->focus_y = cy;
    cam->adjusted_focus_x = cx;
    cam->adjusted_focus_y = cy;
    cam->calculated_focus_x = cx;
    cam->calculated_focus_y = cy;
    *addr = cx;
    *(addr + 1) = cy;
}

void StateMemory::warp(uint8_t target_world, uint8_t target_level, uint8_t target_theme)
{
    // if (ptr()->screen < 11 || ptr()->screen > 20)
    //     return;
    if (this->items->player_count < 1)
    {
        this->items->player_select_slots[0].activated = true;
        this->items->player_select_slots[0].character = ModAPI::savedata()->players[0] + to_id("ENT_TYPE_CHAR_ANA_SPELUNKY");
        this->items->player_select_slots[0].texture_id = ModAPI::savedata()->players[0] + 285; // TODO: magic numbers
        this->items->player_count = 1;
    }
    this->world_next = target_world;
    this->level_next = target_level;
    this->theme_next = target_theme;
    if (this->world_start < 1 || this->level_start < 1 || this->theme_start < 1 || this->theme == 17)
    {
        this->world_start = target_world;
        this->level_start = target_level;
        this->theme_start = target_theme;
        this->quest_flags = 1;
    }
    if (target_theme != 17)
    {
        this->screen_next = 12;
    }
    else
    {
        this->screen_next = 11;
    }
    this->win_state = 0;
    this->loading = 1;

    static auto gm = get_game_manager();
    if (gm->main_menu_music)
    {
        gm->main_menu_music->kill(false);
        gm->main_menu_music = nullptr;
    }
}

void StateMemory::set_seed(uint32_t new_seed)
{
    if (this->screen < 11 || this->screen > 20)
        return;
    this->seed = new_seed;
    this->world_start = 1;
    this->level_start = 1;
    this->theme_start = 1;
    this->world_next = 1;
    this->level_next = 1;
    this->theme_next = 1;
    this->quest_flags = 0x1e | 0x41;
    this->screen_next = 12;
    this->loading = 1;
}

uint32_t lowbias32(uint32_t x)
{
    x ^= x >> 16;
    x *= 0x7feb352d;
    x ^= x >> 15;
    x *= 0x846ca68b;
    x ^= x >> 16;
    return x;
}
uint32_t lowbias32_r(uint32_t x)
{
    x ^= x >> 16;
    x *= 0x43021123U;
    x ^= x >> 15 ^ x >> 30;
    x *= 0x1d69e2a5U;
    x ^= x >> 16;
    return x;
}
Entity* StateMemory::find(uint32_t uid)
{
    // Ported from MauveAlert's python code in the CAT tracker

    // -1 (0xFFFFFFFF) is used as a null-like value for uids.
    if (uid == ~0)
    {
        return nullptr;
    }

    const uint32_t mask = this->uid_to_entity_mask;
    const uint32_t target_uid_plus_one = lowbias32(uid + 1);
    uint32_t cur_index = target_uid_plus_one & mask;
    while (true)
    {
        auto entry = this->uid_to_entity_data[cur_index];
        if (entry.uid_plus_one == target_uid_plus_one)
        {
            return entry.entity;
        }

        if (entry.uid_plus_one == 0)
        {
            return nullptr;
        }

        if (((cur_index - target_uid_plus_one) & mask) > ((cur_index - entry.uid_plus_one) & mask))
        {
            return nullptr;
        }

        cur_index = (cur_index + (uint32_t)1) & mask;
    }
}

LiquidPhysicsEngine* StateMemory::get_correct_liquid_engine(ENT_TYPE liquid_type)
{
    static const ENT_TYPE LIQUID_WATER = to_id("ENT_TYPE_LIQUID_WATER"sv);
    static const ENT_TYPE LIQUID_COARSE_WATER = to_id("ENT_TYPE_LIQUID_COARSE_WATER"sv);
    static const ENT_TYPE LIQUID_LAVA = to_id("ENT_TYPE_LIQUID_LAVA"sv);
    static const ENT_TYPE LIQUID_STAGNANT_LAVA = to_id("ENT_TYPE_LIQUID_STAGNANT_LAVA"sv);
    static const ENT_TYPE LIQUID_COARSE_LAVA = to_id("ENT_TYPE_LIQUID_COARSE_LAVA"sv);
    if (liquid_type == LIQUID_WATER)
    {
        return this->liquid_physics->water_physics_engine;
    }
    else if (liquid_type == LIQUID_COARSE_WATER)
    {
        return this->liquid_physics->coarse_water_physics_engine;
    }
    else if (liquid_type == LIQUID_LAVA)
    {
        return this->liquid_physics->lava_physics_engine;
    }
    else if (liquid_type == LIQUID_STAGNANT_LAVA)
    {
        return this->liquid_physics->stagnant_lava_physics_engine;
    }
    else if (liquid_type == LIQUID_COARSE_LAVA)
    {
        return this->liquid_physics->coarse_lava_physics_engine;
    }
    return nullptr;
}

std::vector<int64_t> State::read_prng()
{
    std::vector<int64_t> prng_vec;
    size_t prng_addr = reinterpret_cast<size_t>(&this->prng);
    for (int i = 0; i < 20; ++i)
    {
        prng_vec.push_back(memory_read<int64_t>(prng_addr + 8 * static_cast<size_t>(i)));
    }
    return prng_vec;
}

uint8_t enum_to_layer(const LAYER layer, std::pair<float, float>& player_position)
{
    if (layer == LAYER::FRONT)
    {
        player_position = {0.0f, 0.0f};
        return 0;
    }
    else if (layer == LAYER::BACK)
    {
        player_position = {0.0f, 0.0f};
        return 1;
    }
    else if ((int)layer < -MAX_PLAYERS)
        return 0;
    else if (layer < LAYER::FRONT)
    {
        auto state = State::ptr();
        auto player = state->items->player(static_cast<uint8_t>(std::abs((int)layer) - 1));
        if (player != nullptr)
        {
            player_position = player->position();
            return player->layer;
        }
    }
    return 0;
}

uint8_t enum_to_layer(const LAYER layer)
{
    if (layer == LAYER::FRONT)
        return 0;
    else if (layer == LAYER::BACK)
        return 1;
    else if ((int)layer < -MAX_PLAYERS)
        return 0;
    else if (layer < LAYER::FRONT)
    {
        auto state = State::ptr();
        auto player = state->items->player(static_cast<uint8_t>(std::abs((int)layer) - 1));
        if (player != nullptr)
        {
            return player->layer > 1 ? 0 : player->layer;
        }
    }
    return 0;
}

Logic* LogicList::start_logic(LOGIC idx)
{
    if ((uint32_t)idx > 27 || logic_indexed[(uint32_t)idx] != nullptr)
        return nullptr;

    int size = 0;
    VTABLE_OFFSET offset = VTABLE_OFFSET::NONE;
    switch (idx)
    {
    case LOGIC::GHOST:
    {
        offset = VTABLE_OFFSET::LOGIC_GHOST_TRIGGER;
        size = sizeof(Logic);
        break;
    }
    case LOGIC::TUN_AGGRO:
    {
        offset = VTABLE_OFFSET::LOGIC_TUN_AGGRO;
        size = sizeof(Logic);
        break;
    }
    case LOGIC::DUAT_BOSSES:
    {
        offset = VTABLE_OFFSET::LOGIC_DUAT_BOSSES_TRIGGER;
        size = sizeof(Logic);
        break;
    }
    case LOGIC::DISCOVERY_INFO:
    {
        offset = VTABLE_OFFSET::LOGIC_DISCOVERY_INFO;
        size = sizeof(Logic);
        break;
    }
    case LOGIC::BLACK_MARKET:
    {
        offset = VTABLE_OFFSET::LOGIC_BLACK_MARKET;
        size = sizeof(Logic);
        break;
    }
    case LOGIC::JELLYFISH:
    {
        offset = VTABLE_OFFSET::LOGIC_COSMIC_OCEAN;
        size = sizeof(Logic);
        break;
    }
    case LOGIC::ARENA_3:
    {
        offset = VTABLE_OFFSET::LOGIC_ARENA_3;
        size = sizeof(Logic);
        break;
    }
    case LOGIC::SPEEDRUN:
    {
        offset = VTABLE_OFFSET::LOGIC_BASECAMP_SPEEDRUN;
        size = sizeof(LogicBasecampSpeedrun);
        break;
    }
    case LOGIC::GHOST_TOAST:
    {
        offset = VTABLE_OFFSET::LOGIC_GHOST_TOAST_TRIGGER;
        size = sizeof(LogicGhostToast);
        break;
    }
    case LOGIC::WATER_BUBBLES:
    {
        offset = VTABLE_OFFSET::LOGIC_WATER_RELATED;
        size = sizeof(LogicUnderwaterBubbles);
        break;
    }
    case LOGIC::APEP:
    {
        offset = VTABLE_OFFSET::LOGIC_APEP_TRIGGER;
        size = sizeof(LogicApepTrigger);
        break;
    }
    case LOGIC::COG_SACRIFICE:
    {
        offset = VTABLE_OFFSET::LOGIC_CITY_OF_GOLD_ANKH_SACRIFICE;
        size = sizeof(LogicCOGAnkhSacrifice);
        break;
    }
    case LOGIC::BUBBLER:
    {
        offset = VTABLE_OFFSET::LOGIC_TIAMAT;
        size = sizeof(LogicTiamatBubbles);
        break;
    }
    case LOGIC::ARENA_1:
    {
        offset = VTABLE_OFFSET::LOGIC_ARENA_1;
        size = sizeof(LogicArena1);
        break;
    }
    case LOGIC::ARENA_ALIEN_BLAST:
    {
        offset = VTABLE_OFFSET::LOGIC_ARENA_ALIEN_BLAST;
        size = sizeof(LogicArenaAlienBlast);
        break;
    }
    case LOGIC::ARENA_LOOSE_BOMBS:
    {
        offset = VTABLE_OFFSET::LOGIC_ARENA_LOOSE_BOMBS;
        size = sizeof(LogicArenaLooseBombs);
        break;
    }
    case LOGIC::TUTORIAL:
    {
        offset = VTABLE_OFFSET::LOGIC_TUTORIAL;
        size = sizeof(LogicTutorial);
        break;
    }
    case LOGIC::OUROBOROS:
    {
        offset = VTABLE_OFFSET::LOGIC_OUROBOROS;
        size = sizeof(LogicOuroboros);
        break;
    }
    case LOGIC::PLEASURE_PALACE:
    {
        offset = VTABLE_OFFSET::LOGIC_TUSK_PLEASURE_PALACE;
        size = sizeof(LogicTuskPleasurePalace);
        break;
    }
    case LOGIC::MAGMAMAN_SPAWN:
    {
        offset = VTABLE_OFFSET::LOGIC_VOLCANA_RELATED;
        size = sizeof(LogicMagmamanSpawn);
        break;
    }
    case LOGIC::PRE_CHALLENGE:
    {
        offset = VTABLE_OFFSET::LOGIC_TUN_PRE_CHALLENGE;
        size = sizeof(LogicTunPreChallenge);
        break;
    }
    case LOGIC::MOON_CHALLENGE:
    {
        offset = VTABLE_OFFSET::LOGIC_TUN_MOON_CHALLENGE;
        size = sizeof(LogicMoonChallenge);
        break;
    }
    case LOGIC::SUN_CHALLENGE:
    {
        offset = VTABLE_OFFSET::LOGIC_TUN_SUN_CHALLENGE;
        size = sizeof(LogicSunChallenge);
        break;
    }
    case LOGIC::TIAMAT_CUTSCENE:
    {
        offset = VTABLE_OFFSET::LOGIC_TIAMAT_CUTSCENE;
        size = sizeof(LogicTiamatCutscene);
        break;
    }
    case LOGIC::DICESHOP:
    {
        offset = VTABLE_OFFSET::LOGIC_DICESHOP;
        size = sizeof(LogicDiceShop);
        break;
    }
    case LOGIC::OLMEC_CUTSCENE:
    {
        offset = VTABLE_OFFSET::LOGIC_OLMEC_CUTSCENE;
        size = sizeof(LogicOlmecCutscene);
        break;
    }
    case LOGIC::STAR_CHALLENGE:
    {
        offset = VTABLE_OFFSET::LOGIC_TUN_STAR_CHALLENGE;
        size = sizeof(LogicStarChallenge);
        break;
    }
    case LOGIC::ARENA_2:
        // offset = VTABLE_OFFSET::LOGIC_ARENA_2;
        // size = ?;
    default:
        return nullptr;
    }
    static auto first_table_entry = get_address("virtual_functions_table");

    auto addr = (size_t*)custom_malloc(size);
    std::memset(addr, 0, size); // just in case

    *addr = first_table_entry + (size_t)offset * 8; // set up vtable
    Logic* new_logic = (Logic*)addr;
    new_logic->logic_index = idx;

    // set up logic that is not possible to initialize thru the API
    if (idx == LOGIC::WATER_BUBBLES)
    {
        auto proper_type = (LogicUnderwaterBubbles*)new_logic;
        proper_type->unknown1 = 1.0f;
        proper_type->unknown2 = 1000;
        proper_type->unknown3 = true;
    }
    else if (idx == LOGIC::OUROBOROS)
    {
        auto proper_type = (LogicOuroboros*)new_logic;
        proper_type->sound = construct_soundmeta(0x51, false);
        // proper_type->sound->start(); // it needs something more
        // game stores the pointer in a special temp memory or something
    }
    else if (idx == LOGIC::PLEASURE_PALACE)
    {
        auto proper_type = (LogicTuskPleasurePalace*)new_logic;
        proper_type->unknown4 = 1552; // magic?
    }

    logic_indexed[(uint32_t)idx] = new_logic;
    return new_logic;
}

void LogicList::stop_logic(LOGIC idx)
{
    if ((uint32_t)idx > 27 || logic_indexed[(uint32_t)idx] == nullptr)
        return;

    delete logic_indexed[(uint32_t)idx];
    logic_indexed[(uint32_t)idx] = nullptr;
}

void LogicList::stop_logic(Logic* log)
{
    if (log == nullptr)
        return;

    auto idx = log->logic_index;
    delete log;
    logic_indexed[(uint32_t)idx] = nullptr;
}

void LogicMagmamanSpawn::add_spawn(uint32_t x, uint32_t y)
{
    magmaman_positions.emplace_back(x, y);
}

void LogicMagmamanSpawn::remove_spawn(uint32_t x, uint32_t y)
{
    for (auto it = magmaman_positions.begin(); it < magmaman_positions.end(); ++it)
    {
        if (it->x == x && it->y == y)
        {
            magmaman_positions.erase(it);
        }
    }
}

void update_camera_position()
{
    auto camera = State::ptr()->camera;
    static const size_t offset = get_address("update_camera_position");
    typedef void update_camera_func(Camera*);
    static update_camera_func* ucf = (update_camera_func*)(offset);
    ucf(camera);
    camera->calculated_focus_x = camera->adjusted_focus_x;
    camera->calculated_focus_y = camera->adjusted_focus_y;
}
