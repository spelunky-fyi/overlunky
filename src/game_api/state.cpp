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
#include "game_api.hpp"                          // for GameAPI
#include "game_manager.hpp"                      // for get_game_manager, GameManager, SaveR...
#include "game_patches.hpp"                      //
#include "items.hpp"                             // for Items, SelectPlayerSlot
#include "level_api.hpp"                         // for LevelGenSystem, LevelGenSystem::(ano...
#include "logger.h"                              // for DEBUG
#include "memory.hpp"                            // for write_mem_prot, memory_read
#include "movable.hpp"                           // for Movable
#include "movable_behavior.hpp"                  // for init_behavior_hooks
#include "render_api.hpp"                        // for init_render_api_hooks
#include "rpc.hpp"                               // for lowbias32
#include "savedata.hpp"                          // for SaveData
#include "screen.hpp"                            // for Screen
#include "script/events.hpp"                     // for pre_entity_instagib
#include "script/lua_vm.hpp"                     // for get_lua_vm
#include "script/usertypes/theme_vtable_lua.hpp" // for NThemeVTables
#include "search.hpp"                            // for get_address
#include "sound_manager.hpp"                     // for SoundManager
#include "spawn_api.hpp"                         // for init_spawn_hooks
#include "steam_api.hpp"                         // for init_achievement_hooks
#include "strings.hpp"                           // for strings_init
#include "virtual_table.hpp"                     // for get_virtual_function_address, VTABLE...
#include "vtable_hook.hpp"                       // for hook_vtable

static uint64_t global_frame_count{0};
static uint64_t global_update_count{0};
static bool g_forward_blocked_events{false};

bool API::get_forward_events()
{
    return g_forward_blocked_events;
}
uint64_t API::get_global_frame_count()
{
    return global_frame_count;
};
uint64_t API::get_global_update_count()
{
    return global_update_count;
};

StateMemory* get_state_ptr()
{
    return HeapBase::get().state();
}
void LiquidPhysics::remove_liquid_oob()
{
    auto state = HeapBase::get().state();
    if (!state->liquid_physics)
        return;

    for (const auto& it : state->liquid_physics->pools)
    {
        if (it.physics_engine == nullptr || it.physics_engine->pause_physics)
            continue;

        for (uint32_t i = 0; i < it.physics_engine->entity_count; ++i)
        {
            auto liquid_coordinates = it.physics_engine->entity_coordinates + i;
            if (liquid_coordinates->y < 0                      // y < 0
                || liquid_coordinates->x < 0                   // x < 0
                || liquid_coordinates->x > g_level_max_x       // x > g_level_max_x
                || liquid_coordinates->y > g_level_max_y + 16) // y > g_level_max_y
            {
                if (!*(it.physics_engine->unknown61 + i)) // just some bs
                    continue;

                const auto ent = **(it.physics_engine->unknown61 + i);
                ent->kill(true, nullptr);
            }
        }
    }
}

inline bool& get_is_init()
{
    static bool is_init{false};
    return is_init;
}

inline bool& get_do_hooks()
{
    static bool do_hooks{true};
    return do_hooks;
}
void API::set_do_hooks(bool do_hooks)
{
    if (get_is_init())
    {
        DEBUG("Too late to disable hooks...");
    }
    else
    {
        get_do_hooks() = do_hooks;
    }
}

void do_write_load_opt()
{
    write_mem_prot(get_address("write_load_opt"), "\x90\x90"sv, true);
}
bool& get_write_load_opt()
{
    static bool allowed{true};
    return allowed;
}
void API::set_write_load_opt(bool write_load_opt)
{
    if (get_is_init())
    {
        if (write_load_opt && !get_write_load_opt())
        {
            do_write_load_opt();
        }
        else if (!write_load_opt && get_write_load_opt())
        {
            DEBUG("Can not unwrite the load optimization...");
        }
    }
    else
    {
        get_write_load_opt() = write_load_opt;
    }
}

static bool g_godmode_player_active = false;
static bool g_godmode_companions_active = false;

void API::godmode(bool g)
{
    g_godmode_player_active = g;
}

void API::godmode_companions(bool g)
{
    g_godmode_companions_active = g;
}

static bool is_active_player(Entity* e)
{
    auto items = HeapBase::get().state()->items;
    for (uint8_t i = 0; i < MAX_PLAYERS; i++)
    {
        auto player = items->players[i];
        if (player && player == e)
        {
            return true;
        }
    }
    return false;
}

using OnDamageFun = bool(Entity*, Entity*, int8_t, uint32_t, float*, uint8_t, uint16_t, uint8_t, bool);
OnDamageFun* g_on_damage_trampoline{nullptr};
bool on_damage(Entity* victim, Entity* damage_dealer, int8_t damage_amount, uint32_t unknown1, float* velocities, uint8_t unknown2, uint16_t stun_amount, uint8_t iframes, bool unknown3)
{
    if (g_godmode_player_active && is_active_player(victim))
    {
        return false;
    }
    if (g_godmode_companions_active && !is_active_player(victim) && (victim->type->search_flags & ENTITY_MASK::PLAYER) == ENTITY_MASK::PLAYER)
    {
        return false;
    }

    return g_on_damage_trampoline(victim, damage_dealer, damage_amount, unknown1, velocities, unknown2, stun_amount, iframes, unknown3);
}

using OnInstaGibFun = void(Entity*, bool, size_t);
OnInstaGibFun* g_on_instagib_trampoline{nullptr};
void on_instagib(Entity* victim, bool destroy_corpse, size_t param_3)
{
    if (g_godmode_player_active && is_active_player(victim))
    {
        return;
    }
    if (g_godmode_companions_active && !is_active_player(victim) && (victim->type->search_flags & ENTITY_MASK::PLAYER) == ENTITY_MASK::PLAYER)
    {
        return;
    }

    const bool skip_orig = pre_entity_instagib(victim) && !(victim->as<Movable>()->health == 0);

    if (!skip_orig)
    {
        g_on_instagib_trampoline(victim, destroy_corpse, param_3);
    }
}

void hook_godmode_functions()
{
    static bool functions_hooked = false;
    if (!functions_hooked)
    {
        auto& memory = Memory::get();
        auto addr_damage = memory.at_exe(get_virtual_function_address(VTABLE_OFFSET::CHAR_ANA_SPELUNKY, 48));
        auto addr_insta = get_address("insta_gib");

        g_on_damage_trampoline = (OnDamageFun*)addr_damage;
        g_on_instagib_trampoline = (OnInstaGibFun*)addr_insta;

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());

        DetourAttach((void**)&g_on_damage_trampoline, &on_damage);
        DetourAttach((void**)&g_on_instagib_trampoline, &on_instagib);

        const LONG error = DetourTransactionCommit();
        if (error != NO_ERROR)
        {
            DEBUG("Failed hooking on_damage/instagib: {}\n", error);
        }

        functions_hooked = true;
    }
}

struct ThemeHookImpl
{
    template <class FunT, class HookFunT>
    struct lua_wrapper;
    template <class... ArgsT, class HookFunT>
    struct lua_wrapper<void(ArgsT...), HookFunT>
    {
        static auto make(HookFunT* fun)
        {
            return [=](ArgsT... args)
            {
                thread_local bool tester;
                tester = true;
                fun(args..., [](ArgsT...)
                    { tester = false; });
                return tester;
            };
        }
    };

    template <class FunT, size_t Index, class HookFunT>
    void hook(ThemeInfo* theme, HookFunT* fun)
    {
        if (get_do_hooks())
        {
            auto& vtable = NThemeVTables::get_theme_info_vtable(get_lua_vm());
            vtable.set_pre<FunT, Index>(theme, vtable.reserve_callback_id(theme), lua_wrapper<FunT, HookFunT>::make(fun));
        }
        else
        {
            hook_vtable<FunT, Index>(theme, fun);
        }
    }
};

static float get_zoom_level()
{
    auto game_api = GameAPI::get();
    return game_api->get_current_zoom();
}

Vec2 API::click_position(float x, float y)
{
    float cz = get_zoom_level();
    auto [cx, cy] = Camera::get_position();
    float rx = cx + ZF * cz * x;
    float ry = cy + (ZF / 16.0f * 9.0f) * cz * y;
    return {rx, ry};
}

Vec2 API::screen_position(float x, float y)
{
    float cz = get_zoom_level();
    auto [cx, cy] = Camera::get_position();
    float rx = (x - cx) / cz / ZF;
    float ry = (y - cy) / cz / (ZF / 16.0f * 9.0f);
    return {rx, ry};
}

void API::zoom(float level)
{
    auto roomx = HeapBase::get().state()->w;
    if (level == 0.0)
    {
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

void API::zoom_reset()
{
    recover_mem("zoom");
    auto game_api = GameAPI::get();
    game_api->set_zoom(std::nullopt, 13.5f);
}

void StateMemory::force_current_theme(THEME t)
{
    if (t > 0 && t < 19)
    {
        auto state = HeapBase::get().state();
        if (t == 10 && !state->level_gen->theme_cosmicocean->sub_theme)
            state->level_gen->theme_cosmicocean->sub_theme = state->level_gen->theme_dwelling; // just set it to something, can't edit this atm
        state->current_theme = state->level_gen->themes[t - 1];
    }
}

void API::darkmode(bool g)
{
    static const size_t addr_dark = get_address("force_dark_level");

    if (g)
    {
        write_mem_recoverable("darkmode", addr_dark, "\x90\x90"sv, true);
    }
    else
    {
        recover_mem("darkmode");
    }
}

Vec2 Camera::get_position()
{
    // = adjusted_focus_x/y - (adjusted_focus_x/y - calculated_focus_x/y) * (render frame-game frame difference)
    static const auto addr = (float*)get_address("camera_position");
    auto cx = *addr;
    auto cy = *(addr + 1);
    return {cx, cy};
}

void Camera::set_position(float cx, float cy)
{
    static const auto addr = (float*)get_address("camera_position"); // probably not needed
    focus_x = cx;
    focus_y = cy;
    adjusted_focus_x = cx;
    adjusted_focus_y = cy;
    calculated_focus_x = cx;
    calculated_focus_y = cy;
    *addr = cx;
    *(addr + 1) = cy;
}

void Camera::update_position()
{
    static const size_t offset = get_address("update_camera_position");
    typedef void update_camera_func(Camera*);
    static update_camera_func* ucf = (update_camera_func*)(offset);
    ucf(this);
    calculated_focus_x = adjusted_focus_x;
    calculated_focus_y = adjusted_focus_y;
}

void StateMemory::warp(uint8_t set_world, uint8_t set_level, uint8_t set_theme)
{
    // if (screen < 11 || screen > 20)
    //     return;
    auto gm = get_game_manager();
    if (items->player_count < 1)
    {
        auto savedata = gm->save_related->savedata.decode_local();
        items->player_select_slots[0].activated = true;
        items->player_select_slots[0].character = savedata->players[0] + to_id("ENT_TYPE_CHAR_ANA_SPELUNKY");
        items->player_select_slots[0].texture_id = savedata->players[0] + 285; // TODO: magic numbers
        items->player_count = 1;
    }
    world_next = set_world;
    level_next = set_level;
    theme_next = set_theme;
    if (world_start < 1 || level_start < 1 || theme_start < 1 || theme == 17)
    {
        world_start = set_world;
        level_start = set_level;
        theme_start = set_theme;
        quest_flags = 1;
    }
    if (set_theme != 17)
    {
        screen_next = 12;
    }
    else
    {
        screen_next = 11;
    }
    win_state = 0;
    loading = 1;

    if (gm->main_menu_music)
    {
        gm->main_menu_music->kill(false);
        gm->main_menu_music = nullptr;
    }
}

void StateMemory::set_seed(uint32_t set_seed)
{
    if (screen < 11 || screen > 20)
        return;
    seed = set_seed;
    world_start = 1;
    level_start = 1;
    theme_start = 1;
    world_next = 1;
    level_next = 1;
    theme_next = 1;
    quest_flags = 0x1e | 0x41;
    screen_next = 12;
    loading = 1;
}

Entity* StateMemory::get_entity(uint32_t uid) const
{
    // Ported from MauveAlert's python code in the CAT tracker

    // -1 (0xFFFFFFFF) is used as a null-like value for uids.
    if (uid == ~0)
    {
        return nullptr;
    }

    const uint32_t mask = uid_to_entity_mask;
    const uint32_t target_uid_plus_one = lowbias32(uid + 1);
    uint32_t cur_index = target_uid_plus_one & mask;
    while (true)
    {
        auto entry = uid_to_entity_data[cur_index];
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

LiquidPhysicsEngine* LiquidPhysics::get_correct_liquid_engine(ENT_TYPE liquid_type) const
{
    static const ENT_TYPE LIQUID_WATER = to_id("ENT_TYPE_LIQUID_WATER"sv);
    static const ENT_TYPE LIQUID_COARSE_WATER = to_id("ENT_TYPE_LIQUID_COARSE_WATER"sv);
    static const ENT_TYPE LIQUID_LAVA = to_id("ENT_TYPE_LIQUID_LAVA"sv);
    static const ENT_TYPE LIQUID_STAGNANT_LAVA = to_id("ENT_TYPE_LIQUID_STAGNANT_LAVA"sv);
    static const ENT_TYPE LIQUID_COARSE_LAVA = to_id("ENT_TYPE_LIQUID_COARSE_LAVA"sv);
    if (liquid_type == LIQUID_WATER)
    {
        return water_physics_engine;
    }
    else if (liquid_type == LIQUID_COARSE_WATER)
    {
        return coarse_water_physics_engine;
    }
    else if (liquid_type == LIQUID_LAVA)
    {
        return lava_physics_engine;
    }
    else if (liquid_type == LIQUID_STAGNANT_LAVA)
    {
        return stagnant_lava_physics_engine;
    }
    else if (liquid_type == LIQUID_COARSE_LAVA)
    {
        return coarse_lava_physics_engine;
    }
    return nullptr;
}

using OnStateUpdate = void(StateMemory*);
OnStateUpdate* g_state_update_trampoline{nullptr};
void StateUpdate(StateMemory* s)
{
    global_update_count++;
    static const auto bucket = Bucket::get();
    if (bucket->blocked_event)
    {
        pre_event(ON::PRE_UPDATE);
        post_event(ON::BLOCKED_UPDATE);
        return;
    }
    static const auto pa = bucket->pause_api;
    auto block = pre_event(ON::PRE_UPDATE);
    if ((!g_forward_blocked_events || !pa->last_instance) && pa->event(PAUSE_TYPE::PRE_UPDATE))
        block = true;
    if (!block)
    {
        g_state_update_trampoline(s);
        post_event(ON::POST_UPDATE);
    }
    else
    {
        post_event(ON::BLOCKED_UPDATE);
        if (g_forward_blocked_events)
        {
            bucket->blocked_event = true;
            g_state_update_trampoline(s);
            bucket->blocked_event = false;
        }
    }
    update_backends();
}

void init_state_update_hook()
{
    g_state_update_trampoline = (OnStateUpdate*)get_address("state_refresh");
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach((void**)&g_state_update_trampoline, &StateUpdate);

    const LONG error = DetourTransactionCommit();
    if (error != NO_ERROR)
    {
        DEBUG("Failed hooking state_refresh stuff: {}\n", error);
    }
}

using OnProcessInput = void(void*);
OnProcessInput* g_process_input_trampoline{nullptr};
void ProcessInput(void* s)
{
    static bool had_focus;
    static const auto bucket = Bucket::get();
    static const auto gm = get_game_manager();
    if (bucket->blocked_event)
    {
        pre_event(ON::PRE_PROCESS_INPUT);
        post_event(ON::BLOCKED_PROCESS_INPUT);
        return;
    }
    static const auto pa = bucket->pause_api;
    if ((!g_forward_blocked_events || !pa->last_instance) && pa->pre_input())
        return;
    auto block = pre_event(ON::PRE_PROCESS_INPUT);
    if ((!g_forward_blocked_events || !pa->last_instance) && pa->event(PAUSE_TYPE::PRE_PROCESS_INPUT))
        block = true;
    if (!block || (gm->game_props->game_has_focus && !had_focus))
    {
        g_process_input_trampoline(s);
        post_event(ON::POST_PROCESS_INPUT);
    }
    else
    {
        post_event(ON::BLOCKED_PROCESS_INPUT);
        if (g_forward_blocked_events)
        {
            bucket->blocked_event = true;
            g_process_input_trampoline(s);
            bucket->blocked_event = false;
        }
    }
    if (!g_forward_blocked_events || !pa->last_instance)
        pa->post_input();
    had_focus = gm->game_props->game_has_focus;
}

void init_process_input_hook()
{
    g_process_input_trampoline = (OnProcessInput*)get_address("process_input");
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach((void**)&g_process_input_trampoline, &ProcessInput);

    const LONG error = DetourTransactionCommit();
    if (error != NO_ERROR)
    {
        DEBUG("Failed hooking process_input stuff: {}\n", error);
    }
}

using OnGameLoop = void(void* a, float b, void* c);
OnGameLoop* g_game_loop_trampoline{nullptr};
void GameLoop(void* a, float b, void* c)
{
    static const auto bucket = Bucket::get();
    static const auto pa = bucket->pause_api;
    auto frame_main = HeapBase::get_main().frame_count();

    if (global_frame_count < frame_main)
        global_frame_count = frame_main;
    else
        global_frame_count++;

    if (bucket->blocked_event)
    {
        pre_event(ON::PRE_GAME_LOOP);
        post_event(ON::BLOCKED_GAME_LOOP);
        return;
    }

    if (!g_forward_blocked_events || !pa->last_instance)
        pa->pre_loop();
    auto block = pre_event(ON::PRE_GAME_LOOP);
    if ((!g_forward_blocked_events || !pa->last_instance) && pa->event(PAUSE_TYPE::PRE_GAME_LOOP))
        block = true;
    if (!block)
    {
        g_game_loop_trampoline(a, b, c);
        post_event(ON::POST_GAME_LOOP);
    }
    else
    {
        post_event(ON::BLOCKED_GAME_LOOP);
        if (g_forward_blocked_events)
        {
            bucket->blocked_event = true;
            g_game_loop_trampoline(a, b, c);
            bucket->blocked_event = false;
        }
    }
    if (!g_forward_blocked_events || !pa->last_instance)
        pa->post_loop();
}

void init_game_loop_hook()
{
    g_game_loop_trampoline = (OnGameLoop*)get_address("game_loop");
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach((void**)&g_game_loop_trampoline, &GameLoop);

    const LONG error = DetourTransactionCommit();
    if (error != NO_ERROR)
    {
        DEBUG("Failed hooking game_loop stuff: {}\n", error);
    }
}

uint8_t enum_to_layer(const LAYER layer, Vec2& player_position)
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
        auto state = HeapBase::get().state();
        auto player = state->items->player(static_cast<uint8_t>(std::abs((int)layer) - 1));
        if (player != nullptr)
        {
            player_position = player->abs_position();
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
        auto state = HeapBase::get().state();
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
        proper_type->gravity_direction = 1.0f;
        proper_type->droplets_spawn_chance = 1000;
        proper_type->droplets_enabled = true;
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
    auto index = static_cast<uint32_t>(idx);
    if (index > 27 || logic_indexed[index] == nullptr)
        return;

    delete logic_indexed[index];
    logic_indexed[index] = nullptr;
}

void LogicList::stop_logic(Logic* log)
{
    if (log == nullptr)
        return;

    auto idx = static_cast<uint32_t>(log->logic_index);
    delete log;
    logic_indexed[idx] = nullptr;
}

void LogicMagmamanSpawn::remove_spawn(uint32_t x, uint32_t y)
{
    std::erase_if(magmaman_positions, [x, y](MagmamanSpawnPosition& m_pos)
                  { return (m_pos.x == x && m_pos.y == y); });
}

void API::init(SoundManager* sound_manager)
{
    if (!get_is_init())
    {
        get_is_init() = true;
        if (get_write_load_opt())
        {
            do_write_load_opt();
        }

        if (get_do_hooks())
        {
            HeapBase::get_main().level_gen()->init();
            init_spawn_hooks();
            init_behavior_hooks();
            init_render_api_hooks();
            init_achievement_hooks();
            hook_godmode_functions();
            strings_init();
            init_state_update_hook();
            init_process_input_hook();
            init_game_loop_hook();
            init_heap_clone_hook();

            auto bucket = Bucket::get();
            bucket->count++;
            if (!bucket->patches_applied)
            {
                bucket->patches_applied = true;
                bucket->forward_blocked_events = true;
                DEBUG("Applying patches");
                patch_tiamat_kill_crash();
                patch_orbs_limit();
                patch_olmec_kill_crash();
                patch_liquid_OOB();
                patch_ushabti_error();
                patch_entering_closed_door_crash();
            }
            else
            {
                DEBUG("Not applying patches, someone has already done it");
                if (bucket->forward_blocked_events)
                    g_forward_blocked_events = true;
            }
        }
    }

    if (sound_manager)
        get_lua_vm(sound_manager);
}
void API::post_init()
{
    if (get_is_init())
    {
        HeapBase::get().level_gen()->hook_themes(ThemeHookImpl{});
    }
}

std::vector<Player*> StateMemory::get_players()
{
    std::vector<Player*> found;
    found.reserve(4);
    for (uint8_t i = 0; i < MAX_PLAYERS; i++)
    {
        auto player = items->players[i];
        if (player)
            found.push_back(player);
    }
    return found;
}
