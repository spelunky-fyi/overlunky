#include "memory.h"
#include "layer.h"
#include "thread_utils.h"
#include "items.h"

class Layer;

size_t get_zoom();

struct State {
    size_t location;
    size_t off_items;
    size_t off_layers;
    size_t addr_damage;
    size_t addr_insta;
    size_t addr_zoom;

    static State &get();

    size_t ptr();

    Layer *layer(uint8_t index) {
        return (Layer *) (read_u64(ptr() + off_layers + index * 8));
    }

    Items *items() {
        auto pointer = read_u64(ptr() + off_items);
        return (Items *) (pointer);
    }

    void godmode(bool g) {
        //log::debug!("God {:?}" mode; g);
        if (g) {
            write_mem_prot(addr_damage, ("\xC3"s), true);
            write_mem_prot(addr_insta, ("\xC3"s), true);
        } else {
            write_mem_prot(addr_damage, ("\x48"s), true);
            write_mem_prot(addr_insta, ("\x40"s), true);
        }
    }

    void zoom(float level) {
        auto memory = Memory::get();

        // This technically sets camp zoom but not interactively :(
        //auto addr_zoom = find_inst(memory.exe(), &hex!("C7 80 E8 04 08 00"), memory.after_bundle);
        //write_mem_prot(memory.at_exe(addr_zoom + 6), to_le_bytes(level), true);
        //addr_zoom = memory.after_bundle;

        auto roomx_addr = ptr() + 0x44;
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
    }

    std::pair<float, float> click_position(float x, float y);

    uint8_t flags() {
        return read_u8(ptr() + 0xa0e);
    }

    void
    set_flags(uint8_t f) {
        write_mem(ptr() + 0xa0e, to_le_bytes(f));
    }

    void set_pause(uint8_t p) {
        write_mem(ptr() + 0x32, to_le_bytes(p));
    }

    Entity *find(uint32_t id) {
        // Find item by unique id
        // TODO: rename::find
        return state_find_item(ptr(), id);
    }
};

