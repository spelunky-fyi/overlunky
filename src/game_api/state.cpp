#include "state.hpp"
#include "game_manager.hpp"
#include "level_api.hpp"
#include "logger.h"
#include "render_api.hpp"
#include "spawn_api.hpp"
#include "strings.hpp"
#include "virtual_table.hpp"

#include <detours.h>

uint16_t StateMemory::get_correct_ushabti() // returns animation_frame of ushabti
{
    return (correct_ushabti + (correct_ushabti / 10) * 2);
}
void StateMemory::set_correct_ushabti(uint16_t animation_frame)
{
    correct_ushabti = static_cast<uint8_t>(animation_frame - (animation_frame / 12) * 2);
}

inline bool& get_is_init()
{
    static bool is_init{false};
    return is_init;
}

void do_write_load_opt()
{
    size_t write_load_addr = get_address("write_load_opt");
    if (write_load_addr > 0ull)
    {
        write_mem_prot(write_load_addr, "\x90\x90"sv, true);
    }
    else
    {
        assert(get_address("write_load_opt_fixed") > 0ull);
    }
}
bool& get_write_load_opt()
{
    static bool allowed{true};
    return allowed;
}
void State::set_write_load_opt(bool write_load_opt)
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

State& State::get()
{
    static State STATE;
    if (!get_is_init())
    {
        if (get_write_load_opt())
        {
            do_write_load_opt();
        }
        auto addr_location = get_address("state_location");
        STATE = State{addr_location};
        STATE.ptr()->level_gen->init();
        init_spawn_hooks();
        init_render_api_hooks();
        get_is_init() = true;
        strings_init();
    }
    return STATE;
}

StateMemory* State::ptr() const
{
    OnHeapPointer<StateMemory> p(read_u64(location));
    return p.decode();
}

StateMemory* State::ptr_local() const
{
    OnHeapPointer<StateMemory> p(read_u64(location));
    return p.decode_local();
}

std::pair<float, float> State::click_position(float x, float y)
{
    float cz = get_zoom_level();
    auto [cx, cy] = get_camera_position();
    float rx = cx + ZF * cz * x;
    float ry = cy + (ZF / 16.0f * 9.0f) * cz * y;
    return {rx, ry};
}

std::pair<float, float> State::screen_position(float x, float y)
{
    float cz = get_zoom_level();
    auto [cx, cy] = get_camera_position();
    float rx = (x - cx) / cz / ZF;
    float ry = (y - cy) / cz / (ZF / 16.0f * 9.0f);
    return {rx, ry};
}

size_t State::get_zoom_level_address()
{
    size_t obj1 = get_address("zoom_level");

    size_t obj2 = read_u64(obj1);
    if (obj2 == 0)
    {
        return 0;
    }

    size_t obj3 = read_u64(obj2 + 0x10);
    if (obj3 == 0)
    {
        return 0;
    }
    return obj3 + get_address("zoom_level_offset");
}

float State::get_zoom_level()
{
    static size_t offset = 0;
    if (offset == 0)
    {
        auto addr = get_zoom_level_address();
        if (addr == 0)
        {
            return 13.5;
        }
        offset = addr;
    }
    return read_f32(offset);
}

void State::zoom(float level)
{
    auto roomx = ptr()->w;
    if (level == 0.0)
    {
        switch (roomx)
        {
        case 1:
            level = 9.50f;
            break;
        case 2:
            level = 16.29f;
            break;
        case 3:
            level = 23.08f;
            break;
        case 4:
            level = 29.87f;
            break;
        case 5:
            level = 36.66f;
            break;
        case 6:
            level = 43.45f;
            break;
        case 7:
            level = 50.24f;
            break;
        case 8:
            level = 57.03f;
            break;
        default:
            level = 13.5f;
        }
    }

    const auto level_str = to_le_bytes(level);

    // overwrite the defaults
    write_mem_prot(get_address("default_zoom_level"), level_str, true);
    write_mem_prot(get_address("default_zoom_level_shop"), level_str, true);
    write_mem_prot(get_address("default_zoom_level_camp"), level_str, true);
    write_mem_prot(get_address("default_zoom_level_telescope"), level_str, true);

    // overwrite the current value
    auto zla = get_zoom_level_address();
    if (zla != 0)
    {
        write_mem_prot(zla, level_str, true);
    }
}

void StateMemory::force_current_theme(uint32_t t)
{
    if (t > 0 && t < 19)
    {
        auto state = State::get().ptr();
        if (t == 10 && !state->level_gen->theme_cosmicocean->sub_theme)
            state->level_gen->theme_cosmicocean->sub_theme = state->level_gen->theme_dwelling; // just set it to something, can't edit this atm
        state->current_theme = state->level_gen->themes[t - 1];
    }
}

static bool g_godmode_player_active = false;
static bool g_godmode_companions_active = false;

bool is_active_player(Entity* e)
{
    auto state = State::get();
    for (uint8_t i = 0; i < MAX_PLAYERS; i++)
    {
        auto player = state.items()->player(i);
        if (player && player == e)
        {
            return true;
        }
    }
    return false;
}

using OnDamageFun = void(Entity*, Entity*, int8_t, uint32_t, float*, float*, uint16_t, uint8_t);
OnDamageFun* g_on_damage_trampoline{nullptr};
void on_damage(Entity* victim, Entity* damage_dealer, int8_t damage_amount, uint32_t unknown1, float* velocities, float* unknown2, uint16_t stun_amount, uint8_t iframes)
{
    if (g_godmode_player_active && is_active_player(victim))
    {
        return;
    }
    if (g_godmode_companions_active && !is_active_player(victim) && (victim->type->search_flags & 1) == 1)
    {
        return;
    }

    // because Player::on_damage is always hooked here, we have to check whether a script has hooked this function as well
    EntityHooksInfo& _hook_info = victim->get_hooks();
    bool skip_orig = false;
    for (auto& [id, backend_on_damage] : _hook_info.on_damage)
    {
        if (backend_on_damage(victim, damage_dealer, damage_amount, velocities[0], velocities[1], stun_amount, iframes))
        {
            skip_orig = true;
        }
    }

    if (!skip_orig)
    {
        g_on_damage_trampoline(victim, damage_dealer, damage_amount, unknown1, velocities, unknown2, stun_amount, iframes);
    }
}

using OnInstaGibFun = void(Entity*, size_t);
OnInstaGibFun* g_on_instagib_trampoline{nullptr};
void on_instagib(Entity* victim, size_t unknown)
{
    if (g_godmode_player_active && is_active_player(victim))
    {
        return;
    }
    if (g_godmode_companions_active && !is_active_player(victim) && (victim->type->search_flags & 1) == 1)
    {
        return;
    }

    // because on_instagib is only hooked here, we have to check whether a script has hooked this function as well
    EntityHooksInfo& _hook_info = victim->get_hooks();
    bool skip_orig = false;
    for (auto& [id, backend_on_player_instagib] : _hook_info.on_player_instagib)
    {
        if (backend_on_player_instagib(victim))
        {
            skip_orig = true;
        }
    }

    // The instagib function needs to be called when the entity is dead, otherwise the death screen will never be opened
    if (victim->as<Movable>()->health == 0)
    {
        skip_orig = false;
    }

    if (!skip_orig)
    {
        g_on_instagib_trampoline(victim, unknown);
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

void State::godmode(bool g)
{
    hook_godmode_functions();
    g_godmode_player_active = g;
}

void State::godmode_companions(bool g)
{
    hook_godmode_functions();
    g_godmode_companions_active = g;
}

void State::darkmode(bool g)
{
    static size_t addr_dark = 0;
    static char original_instructions[2] = {0};
    if (addr_dark == 0)
    {
        addr_dark = get_address("force_dark_level");
        original_instructions[0] = read_u8(addr_dark);
        original_instructions[1] = read_u8(addr_dark + 1);
    }
    if (g)
    {
        write_mem_prot(addr_dark, ("\x90\x90"s), true);
    }
    else
    {
        write_mem_prot(addr_dark, std::string(original_instructions, 2), true);
    }
}

std::pair<float, float> State::get_camera_position()
{
    static size_t addr = 0;
    if (addr == 0)
    {
        addr = get_address("camera_position");
    }

    auto cx = (float*)addr;
    auto cy = (float*)(addr + 4);
    return {*cx, *cy};
}

void State::set_camera_position(float cx, float cy)
{
    auto camera = ptr()->camera;
    camera->focused_entity_uid = -1;
    camera->focus_x = cx;
    camera->focus_y = cy;
}

void State::warp(uint8_t w, uint8_t l, uint8_t t)
{
    //if (ptr()->screen < 11 || ptr()->screen > 20)
    //    return;
    if (ptr()->items->player_count < 1)
    {
        ptr()->items->player_select_slots[0].activated = true;
        ptr()->items->player_select_slots[0].character = savedata()->players[0] + to_id("ENT_TYPE_CHAR_ANA_SPELUNKY");
        ptr()->items->player_select_slots[0].texture_id = savedata()->players[0] + 285; //TODO: magic numbers
        ptr()->items->player_count = 1;
    }
    ptr()->world_next = w;
    ptr()->level_next = l;
    ptr()->theme_next = t;
    if (ptr()->world_start < 1 || ptr()->level_start < 1 || ptr()->theme_start < 1 || ptr()->theme == 17)
    {
        ptr()->world_start = w;
        ptr()->level_start = l;
        ptr()->theme_start = t;
        ptr()->quest_flags = 1;
    }
    if (t != 17)
    {
        ptr()->screen_next = 12;
    }
    else
    {
        ptr()->screen_next = 11;
    }
    ptr()->fadeout = 5;
    ptr()->fadein = 5;
    ptr()->win_state = 0;
    ptr()->loading = 1;
}

void State::set_seed(uint32_t seed)
{
    if (ptr()->screen < 11 || ptr()->screen > 20)
        return;
    ptr()->seed = seed;
    ptr()->world_start = 1;
    ptr()->level_start = 1;
    ptr()->theme_start = 1;
    ptr()->world_next = 1;
    ptr()->level_next = 1;
    ptr()->theme_next = 1;
    ptr()->quest_flags = 0x1e | 0x41;
    ptr()->screen_next = 12;
    ptr()->fadeout = 5;
    ptr()->fadein = 5;
    ptr()->loading = 1;
}

SaveData* State::savedata()
{
    auto gm = get_game_manager();
    return gm->save_related->savedata.decode();
}

Entity* State::find(uint32_t uid)
{
    // Ported from MauveAlert's python code in the CAT tracker
    auto mask = ptr()->uid_to_entity_mask;
    uint32_t target_uid_plus_one = uid + 1;
    uint32_t cur_index = target_uid_plus_one & mask;
    while (true)
    {
        auto entry = ptr()->uid_to_entity_data[cur_index];
        if (entry.uid_plus_one == target_uid_plus_one)
        {
            return entry.entity;
        }

        if (entry.uid_plus_one == 0)
        {
            return nullptr;
        }

        if ((target_uid_plus_one & mask) > (entry.uid_plus_one & mask))
        {
            return nullptr;
        }

        cur_index = (cur_index + 1) & mask;
    }
}
