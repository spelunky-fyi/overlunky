#include "state.hpp"
#include "game_manager.hpp"
#include "logger.h"

size_t get_dark()
{
    ONCE(size_t)
    {
        auto memory = Memory::get();
        auto addr_dark = memory.after_bundle;
        addr_dark = find_inst(memory.exe(), "\x44\xC5\x80\xA0\x12\x0A\x00\x00\xFD"s, memory.after_bundle);
        DEBUG("addr_dark: {}", addr_dark);
        return res = memory.at_exe(addr_dark) + 9;
    }
}

size_t get_zoom()
{
    ONCE(size_t)
    {
        auto memory = Memory::get();
        auto addr_zoom = memory.after_bundle;
        for (int _ = 0; _ < 3; _++)
        {
            addr_zoom = find_inst(memory.exe(), "\x48\x8B\x48\x10\xC7\x81"s, addr_zoom + 1);
            DEBUG("addr_zoom: {}", addr_zoom);
        }
        return res = memory.at_exe(addr_zoom) + 10;
    }
}

size_t get_zoom_shop()
{
    ONCE(size_t)
    {
        auto memory = Memory::get();
        auto addr_zoom = memory.after_bundle;
        for (int _ = 0; _ < 2; _++)
        {
            addr_zoom = find_inst(memory.exe(), "\x48\x8B\x48\x10\xC7\x81"s, addr_zoom + 1);
            DEBUG("addr_zoom: {}", addr_zoom);
        }
        return res = memory.at_exe(addr_zoom) + 10;
    }
}

size_t get_location()
{
    ONCE(size_t)
    {
        auto memory = Memory::get();
        auto exe = memory.exe();
        auto start = memory.after_bundle;
        auto location = find_inst(exe, "\x49\x0F\x44\xC0"s, start);
        location = find_inst(exe, "\x49\x0F\x44\xC0"s, location + 1);
        location = memory.at_exe(decode_pc(exe, find_inst(exe, "\x48\x8B"s, location - 0x10)));
        return res = location;
    }
}
size_t get_damage()
{
    ONCE(size_t)
    {
        // TODO: get vtable of character and calculate the offset
        auto memory = Memory::get();
        auto off = memory.after_bundle;
        while (true)
        {
            // sub rsp, 0x90; xor reg, reg
            off = find_inst(memory.exe(), "\x48\x81\xEC\x90\x00\x00\x00"s, off + 1);
            if (read_u8(memory.at_exe(off + 7)) == 0x33 || read_u8(memory.at_exe(off + 8)) == 0x33)
            {
                break;
            }
        }

        return res = function_start(memory.at_exe(off));
    }
}

size_t get_insta()
{
    ONCE(size_t)
    {
        auto memory = Memory::get();
        auto off = memory.after_bundle;
        off = find_inst(memory.exe(), "\x40\x53\x56\x41\x54\x41\x55\x48\x83\xEC\x68"s,
                        off + 1); // Spel2.exe+21E37920
        return res = function_start(memory.at_exe(off));
    }
}

size_t get_camera()
{
    ONCE(size_t)
    {
        auto memory = Memory::get();
        auto off = find_inst(memory.exe(), "\xC7\x87\xCC\x00\x00\x00\x00\x00"s, memory.after_bundle);
        off = find_inst(memory.exe(), "\xF3\x0F\x11\x05"s, off) + 1;
        return res = memory.at_exe(decode_pc(memory.exe(), off));
    }
}

inline bool& get_is_init()
{
    static bool is_init{false};
    return is_init;
}

void do_write_load_opt()
{
    auto memory = Memory::get();
    auto exe = memory.exe();
    auto start = memory.after_bundle;
    auto off_send = find_inst(exe, "\x45\x8D\x41\x50"s, start) + 9;
    write_mem_prot(memory.at_exe(off_send), "\x31\xC0\x31\xD2\x90\x90"s, true);
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
        auto addr_location = get_location();
        auto addr_damage = get_damage();
        auto addr_insta = get_insta();
        auto addr_zoom = get_zoom();
        auto addr_zoom_shop = get_zoom_shop();
        auto addr_dark = get_dark();
        STATE = State{addr_location, addr_damage, addr_insta, addr_zoom, addr_zoom_shop, addr_dark};
        get_is_init() = true;
    }
    return STATE;
}

StateMemory* State::ptr() const
{
    OnHeapPointer<StateMemory> p(read_u64(location));
    // log::debug!("{:x?}" State; p);
    return p.decode();
}

std::pair<float, float> State::click_position(float x, float y)
{
    uint8_t screen = ptr()->screen;
    float cz = read_f32(get_zoom());
    if (screen < 12 || screen == 13 || screen > 14)
        cz = 13.5;
    float cx = read_f32(get_camera());
    float cy = read_f32(get_camera() + 4);
    float rx = cx + ZF * cz * x;
    float ry = cy + (ZF / 16.0 * 9.0) * cz * y;
    return {rx, ry};
}

std::pair<float, float> State::screen_position(float x, float y)
{
    uint8_t screen = ptr()->screen;
    float cz = read_f32(get_zoom());
    if (screen < 12 || screen == 13 || screen > 14)
        cz = 13.5;
    float cx = read_f32(get_camera());
    float cy = read_f32(get_camera() + 4);
    float rx = (x - cx) / cz / ZF;
    float ry = (y - cy) / cz / (ZF / 16.0 * 9.0);
    return {rx, ry};
}

float State::get_zoom_level()
{
    return read_f32(get_zoom());
}

std::pair<float, float> State::get_camera_position()
{
    float cx = read_f32(get_camera());
    float cy = read_f32(get_camera() + 4);
    return {cx, cy};
}

void State::set_camera_position(float cx, float cy)
{
    write_mem_prot(get_camera(), to_le_bytes(cx), true);
    write_mem_prot(get_camera() + 4, to_le_bytes(cy), true);
}

void State::warp(uint8_t w, uint8_t l, uint8_t t)
{
    if (ptr()->screen < 11 || ptr()->screen > 13)
        return;
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
    ptr()->loading = 1;
}

void State::set_seed(uint32_t seed)
{
    if (ptr()->screen < 11 || ptr()->screen > 13)
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
