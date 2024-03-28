#include <detours.h> // for DetourAttach, DetourTransactionBegin

#include "bucket.hpp"         // for Bucket
#include "entities_chars.hpp" // for Player
#include "game_manager.hpp"
#include "game_patches.hpp"
#include "items.hpp"
#include "memory.hpp"
#include "mod_api.hpp"
#include "movable.hpp"
#include "movable_behavior.hpp"
#include "script/events.hpp"                     // for pre_entity_instagib
#include "script/lua_vm.hpp"                     // for get_lua_vm
#include "script/usertypes/theme_vtable_lua.hpp" // for NThemeVTables
#include "search.hpp"
#include "spawn_api.hpp"
#include "state.hpp"
#include "steam_api.hpp"
#include "strings.hpp"
#include "virtual_table.hpp" // for VTABLE_OFFSET

static bool g_forward_blocked_events{false};

static int64_t global_update_count{0};
static int64_t global_frame_count{0};

int64_t get_global_frame_count()
{
    return global_frame_count;
}
int64_t get_global_update_count()
{
    return global_update_count;
}

bool get_forward_events()
{
    return g_forward_blocked_events;
}

static bool g_godmode_player_active = false;
static bool g_godmode_companions_active = false;

void ModAPI::godmode(bool g)
{
    g_godmode_player_active = g;
}

void ModAPI::godmode_companions(bool g)
{
    g_godmode_companions_active = g;
}

void ModAPI::darkmode(bool g)
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

bool is_active_player(Entity* e)
{
    auto state = State::ptr();
    for (uint8_t i = 0; i < MAX_PLAYERS; i++)
    {
        auto player = state->items->player(i);
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
    if (g_godmode_companions_active && !is_active_player(victim) && (victim->type->search_flags & 1) == 1)
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
    if (g_godmode_companions_active && !is_active_player(victim) && (victim->type->search_flags & 1) == 1)
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
        auto memory = Memory::get();
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
void ModAPI::set_do_hooks(bool do_hooks)
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
void ModAPI::set_write_load_opt(bool write_load_opt)
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

    auto main_frame_count = State::get_main()->frame_count;
    if (global_frame_count < main_frame_count)
        global_frame_count = main_frame_count;
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

void ModAPI::init(class SoundManager* sound_manager)
{
    // ModAPI::get();
    if (!get_is_init())
    {
        if (get_write_load_opt())
        {
            do_write_load_opt();
        }

        get_is_init() = true;

        if (get_do_hooks())
        {
            State::ptr_main()->level_gen->init();
            init_spawn_hooks();
            init_behavior_hooks();
            init_render_api_hooks();
            init_achievement_hooks();
            hook_godmode_functions();
            strings_init();
            init_state_update_hook();
            init_process_input_hook();
            init_game_loop_hook();

            auto bucket = Bucket::get();
            bucket->count++;
            if (!bucket->patches_applied)
            {
                DEBUG("Applying patches");
                patch_tiamat_kill_crash();
                patch_orbs_limit();
                patch_olmec_kill_crash();
                patch_liquid_OOB();
                patch_ushabti_error();
                patch_entering_closed_door_crash();
                bucket->patches_applied = true;
                bucket->forward_blocked_events = true;
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
void ModAPI::post_init()
{
    if (get_is_init())
    {
        StateMemory& state{*State::ptr_main()};
        state.level_gen->hook_themes(ThemeHookImpl{});
    }
}

SaveData* ModAPI::savedata()
{
    auto gm = get_game_manager();
    return gm->save_related->savedata.decode();
}
