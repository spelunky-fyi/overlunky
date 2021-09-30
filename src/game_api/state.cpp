#include "state.hpp"
#include "game_manager.hpp"
#include "level_api.hpp"
#include "logger.h"
#include "render_api.hpp"
#include "spawn_api.hpp"
#include "virtual_table.hpp"

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
    write_mem_prot(get_address("write_load_opt"), "\x90\x90"s, true);
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
        DEBUG("TODO: patterns for level_gen and spawn_hooks");
        //STATE.ptr()->level_gen->init();
        //init_spawn_hooks();
        init_render_api_hooks();
        get_is_init() = true;
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
    float cx = ptr()->camera->calculated_focus_x;
    float cy = ptr()->camera->calculated_focus_y;
    float rx = cx + ZF * cz * x;
    float ry = cy + (ZF / 16.0f * 9.0f) * cz * y;
    return {rx, ry};
}

std::pair<float, float> State::screen_position(float x, float y)
{
    float cz = get_zoom_level();
    float cx = ptr()->camera->calculated_focus_x;
    float cy = ptr()->camera->calculated_focus_y;
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
    // no idea what triggers the fourth instance of writing 13.5 to current zoom level (see pattern for camp, it's the first match that is skipped)

    // overwrite the current value
    auto zla = get_zoom_level_address();
    if (zla != 0)
    {
        write_mem_prot(zla, level_str, true);
    }
}

void State::godmode(bool g)
{
    auto memory = Memory::get();
    auto addr_damage = memory.at_exe(get_virtual_function_address(VTABLE_OFFSET::CHAR_ANA_SPELUNKY, 48));
    auto addr_insta = get_address("insta_gib");

    static char original_damage_instruction = 0;
    static char original_instagib_instruction = 0;
    if (original_damage_instruction == 0)
    {
        original_damage_instruction = read_u8(addr_damage);
        original_instagib_instruction = read_u8(addr_insta);
    }

    // log::debug!("God {:?}" mode; g);
    if (g)
    {
        write_mem_prot(addr_damage, ("\xC3"s), true);
        write_mem_prot(addr_insta, ("\xC3"s), true);
    }
    else
    {
        write_mem_prot(addr_damage, std::string(&original_damage_instruction, 1), true);
        write_mem_prot(addr_insta, std::string(&original_instagib_instruction, 1), true);
    }
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
    float cx = ptr()->camera->calculated_focus_x;
    float cy = ptr()->camera->calculated_focus_y;
    return {cx, cy};
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
    return gm->tmp->savedata.decode();
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
