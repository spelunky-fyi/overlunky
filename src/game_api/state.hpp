#pragma once

#include "items.hpp"
#include "layer.hpp"
#include "memory.hpp"
#include "thread_utils.hpp"

const float ZF = 0.737;

class Layer;

struct State {
    size_t location;
    size_t off_items;
    size_t off_layers;
    size_t addr_damage;
    size_t addr_insta;
    size_t addr_zoom;
    size_t addr_zoom_shop;
    size_t addr_dark;

    static State &get();

    size_t ptr();

    Layer *layer(uint8_t index) {
        return (Layer *)(read_u64(ptr() + off_layers + index * 8));
    }

    Items *items() {
        auto pointer = read_u64(ptr() + off_items);
        return (Items *)(pointer);
    }

    void godmode(bool g) {
        // log::debug!("God {:?}" mode; g);
        if (g) {
            write_mem_prot(addr_damage, ("\xC3"s), true);
            write_mem_prot(addr_insta, ("\xC3"s), true);
        } else {
            write_mem_prot(addr_damage, ("\x48"s), true);
            write_mem_prot(addr_insta, ("\x40"s), true);
        }
    }

    void darkmode(bool g) {
        // log::debug!("God {:?}" mode; g);
        if (g) {
            write_mem_prot(addr_dark, ("\x90\x90"s), true);
        } else {
            write_mem_prot(addr_dark, ("\xEB\x2E"s), true);
        }
    }

    void zoom(float level) {
        auto memory = Memory::get();

        // This technically sets camp zoom but not interactively :(
        // auto addr_zoom = find_inst(memory.exe(), &hex!("C7 80 E8 04 08 00"),
        // memory.after_bundle); write_mem_prot(memory.at_exe(addr_zoom + 6),
        // to_le_bytes(level), true); addr_zoom = memory.after_bundle;

        auto roomx_addr = ptr() + 0x48;
        uint8_t roomx = read_u8(roomx_addr);
        if (level == 0.0) {
            switch (roomx) {
                case 1:
                    level = 9.50;
                    break;
                case 2:
                    level = 16.29;
                    break;
                case 3:
                    level = 23.08;
                    break;
                case 4:
                    level = 29.87;
                    break;
                case 5:
                    level = 36.66;
                    break;
                case 6:
                    level = 43.45;
                    break;
                case 7:
                    level = 50.24;
                    break;
                case 8:
                    level = 57.03;
                    break;
                default:
                    level = 13.5;
            }
        }
        write_mem_prot(addr_zoom, to_le_bytes(level), true);
        write_mem_prot(addr_zoom_shop, to_le_bytes(level), true);
    }

    std::pair<float, float> click_position(float x, float y);
    std::pair<float, float> screen_position(float x, float y);
    float get_zoom_level();

    uint8_t flags() { return read_u32(ptr() + 0xa0e); }

    void set_flags(uint32_t f) { write_mem(ptr() + 0xa0e, to_le_bytes(f)); }

    void set_pause(uint8_t p) { write_mem(ptr() + 0x32, to_le_bytes(p)); }

    Entity *find(uint32_t id) {
        // Find item by unique id
        // TODO: rename::find
        return state_find_item(ptr(), id);
    }
};

struct StateMemory {
    size_t p00;
    uint32_t screen_last;
    uint32_t screen;
    uint32_t screen_next;
    int32_t i14;
    size_t p18;
    int32_t i20;
    int32_t i24;
    int32_t i28;
    int32_t i2c;
    uint8_t ingame;
    uint8_t playing;
    uint8_t pause;
    uint8_t b33;
    int32_t i34;
    int32_t i38;
    int32_t i3c;
    int32_t i40;
    int32_t i44;
    uint32_t w;
    uint32_t h;
    int8_t kali_favor;
    int8_t kali_status;
    int8_t kali_altars_destroyed;
    uint8_t b4f;
    int32_t i50;
    int32_t i54;
    int32_t i58;
    uint32_t feedcode;
    uint32_t time_total;
    uint8_t world;
    uint8_t world_next;
    uint8_t level;
    uint8_t level_next;
    int32_t i6c;
    int32_t i70;
    uint8_t theme;
    uint8_t theme_next;
    uint8_t b72;
    uint8_t b73;
    int32_t i74;
    uint8_t shoppie_aggro;
    uint8_t shoppie_aggro_levels;
    uint8_t merchant_aggro;
    uint8_t merchant_pad;
    uint8_t b7c;
    uint8_t b7d;
    uint8_t kills_npc;
    uint8_t level_count;
    uint8_t pad84[0x96c];
    uint32_t journal_flags;
    int32_t i9f0;
    int32_t i9f4;
    uint32_t time_last_level;
    uint32_t time_level;
    int32_t ia00;
    int32_t ia04;
    uint32_t hud_flags;
};
