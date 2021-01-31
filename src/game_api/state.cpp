#include "state.hpp"

size_t function_start(size_t off) {
    off &= ~0xf;
    while (read_u8(off - 1) != 0xcc) {
        off -= 0x10;
    }
    return off;
}

size_t get_dark() {
    ONCE(size_t) {
        auto memory = Memory::get();
        auto addr_dark = memory.after_bundle;
        addr_dark =
            find_inst(memory.exe(), "\x44\xC5\x80\xA0\x0E\x0A\x00\x00\xFD"s,
                      memory.after_bundle);
        DEBUG("0x%x", addr_dark);
        return res = memory.at_exe(addr_dark) + 9;
    }
}

size_t get_zoom() {
    ONCE(size_t) {
        auto memory = Memory::get();
        auto addr_zoom = memory.after_bundle;
        for (int _ = 0; _ < 3; _++) {
            addr_zoom = find_inst(memory.exe(), "\x48\x8B\x48\x10\xC7\x81"s,
                                  addr_zoom + 1);
            DEBUG("0x%x", addr_zoom);
        }
        return res = memory.at_exe(addr_zoom) + 10;
    }
}

size_t get_zoom_shop() {
    ONCE(size_t) {
        auto memory = Memory::get();
        auto addr_zoom = memory.after_bundle;
        for (int _ = 0; _ < 2; _++) {
            addr_zoom = find_inst(memory.exe(), "\x48\x8B\x48\x10\xC7\x81"s,
                                  addr_zoom + 1);
            DEBUG("0x%x", addr_zoom);
        }
        return res = memory.at_exe(addr_zoom) + 10;
    }
}

size_t get_damage() {
    ONCE(size_t) {
        // TODO: get vtable of character and calculate the offset
        auto memory = Memory::get();
        auto off = memory.after_bundle;
        while (true) {
            // sub rsp, 0x90; xor reg, reg
            off = find_inst(memory.exe(), "\x48\x81\xEC\x90\x00\x00\x00"s,
                            off + 1);
            if (read_u8(memory.at_exe(off + 7)) == 0x33 ||
                read_u8(memory.at_exe(off + 8)) == 0x33) {
                break;
            }
        }

        return res = function_start(memory.at_exe(off));
    }
}

size_t get_insta() {
    ONCE(size_t) {
        auto memory = Memory::get();
        auto off = memory.after_bundle;
        off = find_inst(memory.exe(),
                        "\x40\x53\x56\x41\x54\x41\x55\x48\x83\xEC\x58"s,
                        off + 1);  // Spel2.exe+21E37920
        return res = function_start(memory.at_exe(off));
    }
}

size_t get_camera() {
    ONCE(size_t) {
        auto memory = Memory::get();
        auto off = find_inst(memory.exe(), "\xC7\x87\xCC\x00\x00\x00\x00\x00"s,
                             memory.after_bundle);
        off = find_inst(memory.exe(), "\xF3\x0F\x11\x05"s, off) + 1;
        return res = memory.at_exe(decode_pc(memory.exe(), off));
    }
}

State &State::get() {
    static State STATE;
    static bool INIT;
    if (!INIT) {
        auto memory = Memory::get();
        // Global state pointer
        auto exe = memory.exe();
        auto start = memory.after_bundle;
        auto location = memory.at_exe(decode_pc(
            exe, find_inst(exe, "\x48\x8B\x05"s,
                           find_inst(exe, "\x32\x01\x74"s, start) - 0x100)));
        // The offset of items field
        auto off_items = decode_imm(
            exe, find_inst(exe, "\x3C\x11\x41\x0F\x95\xC0"s, start) + 11);
        auto off_layers = decode_imm(
            exe, find_inst(exe, "\xC6\x80\x58\x44\x06\x00\x01"s, start) - 7);
        auto off_send = find_inst(exe, "\x45\x8D\x41\x50"s, start) + 12;
        write_mem_prot(memory.at_exe(off_send), "\x31\xC0\x31\xD2\x90\x90"s,
                       true);
        auto addr_damage = get_damage();
        auto addr_insta = get_insta();
        // log::debug!("0x{:x}, insta: 0x{:x}", addr_damage damage; addr_insta);
        auto addr_zoom = get_zoom();
        auto addr_zoom_shop = get_zoom_shop();
        auto addr_dark = get_dark();
        STATE = State{
            location,   off_items, off_layers,     addr_damage,
            addr_insta, addr_zoom, addr_zoom_shop, addr_dark,
        };
        INIT = true;
    }
    return STATE;
}

size_t State::ptr() {
    auto p = read_u64(location) + heap_base();
    // log::debug!("{:x?}" State; p);
    return p;
}

std::pair<float, float> State::click_position(float x, float y) {
    uint8_t screen = read_u8(ptr() + 0x10);
    float cz = read_f32(get_zoom());
    if (screen < 12 || screen == 13 || screen > 14) cz = 13.5;
    auto cx = read_f32(get_camera());
    auto cy = read_f32(get_camera() + 4);
    auto rx = cx + ZF * cz * x;
    auto ry = cy + (ZF / 16.0 * 9.0) * cz * y;
    return {rx, ry};
}

std::pair<float, float> State::screen_position(float x, float y) {
    uint8_t screen = read_u8(ptr() + 0x10);
    float cz = read_f32(get_zoom());
    if (screen < 12 || screen == 13 || screen > 14) cz = 13.5;
    auto cx = read_f32(get_camera());
    auto cy = read_f32(get_camera() + 4);
    auto rx = (x - cx) / cz / ZF;
    auto ry = (y - cy) / cz / (ZF / 16.0 * 9.0);
    return {rx, ry};
}

float State::get_zoom_level() { return read_f32(get_zoom()); }
