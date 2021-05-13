#include "layer.hpp"

#include "logger.h"
#include "state.hpp"

struct Layer;
using LoadItem = size_t (*)(Layer*, size_t, float, float);

LoadItem get_load_item()
{
    ONCE(LoadItem)
    {
        auto memory = Memory::get();
        auto exe = memory.exe();
        auto needle = "\xBA\xB9\x01\x00\x00"s;
        auto off = find_inst(exe, needle, memory.after_bundle);
        off = find_inst(exe, needle, off + 5);
        off = find_inst(exe, needle, off + 5);
        off = find_inst(exe, "\xE8"s, off + 5);

        return res = (LoadItem)memory.at_exe(decode_call(off));
    }
}

using LoadItemOver = Entity* (*)(Layer*, size_t, Entity*, float, float, bool);

LoadItemOver get_load_item_over()
{
    ONCE(LoadItemOver)
    {
        auto memory = Memory::get();
        auto off = find_inst(memory.exe(), "\xBA\x51\x00\x00\x00\x48\x8B"s, memory.after_bundle);
        off = find_inst(memory.exe(), "\xE8"s, off + 5);
        off = find_inst(memory.exe(), "\xE8"s, off + 5);
        return res = (LoadItemOver)memory.at_exe(decode_call(off));
    }
}

Entity* Layer::spawn_entity(size_t id, float x, float y, bool s, float vx, float vy, bool snap)
{
    if (id == 0)
        return nullptr;
    auto load_item = (get_load_item());
    if (!s)
    {
        if (snap)
        {
            x = round(x);
            y = round(y);
        }
        auto addr = load_item(this, id, x, y);
        if (abs(vx) + abs(vy) > 0.01)
        {
            write_mem(addr + 0x100, to_le_bytes(vx));
            write_mem(addr + 0x104, to_le_bytes(vy));
        }
        DEBUG("Spawned {:x}", addr);
        return (Entity*)(addr);
    }
    else
    {
        auto state = State::get();
        auto [rx, ry] = state.click_position(x, y);
        if (snap && abs(vx) + abs(vy) <= 0.04)
        {
            rx = round(rx);
            ry = round(ry);
        }
        auto addr = load_item(this, id, rx, ry);
        if (abs(vx) + abs(vy) > 0.04)
        {
            write_mem(addr + 0x100, to_le_bytes(vx));
            write_mem(addr + 0x104, to_le_bytes(vy));
        }
        DEBUG("Spawned {:x}", addr);
        return (Entity*)(addr);
    }
}

Entity* Layer::spawn_entity_over(size_t id, Entity* overlay, float x, float y)
{
    if (id == 0)
        return nullptr;
    auto load_item_over = (get_load_item_over());

    return load_item_over(this, id, overlay, x, y, true);
}

Entity* Layer::spawn_door(float x, float y, uint8_t w, uint8_t l, uint8_t t)
{
    uint8_t screen = State::get().ptr()->screen_next;
    Entity* door;
    switch (screen)
    {
    case 11:
    {
        DEBUG("In camp, spawning starting exit");
        door = spawn_entity(to_id("ENT_TYPE_FLOOR_DOOR_STARTING_EXIT"), round(x), round(y), false, 0.0, 0.0, true);
        break;
    }
    case 12:
    {
        DEBUG("In game, spawning regular exit");
        door = spawn_entity(to_id("ENT_TYPE_FLOOR_DOOR_EXIT"), round(x), round(y), false, 0.0, 0.0, true);
        break;
    }
    default:
        return nullptr;
    };
    static_cast<Door*>(door)->set_target(w, l, t);
    spawn_entity(to_id("ENT_TYPE_LOGICAL_PLATFORM_SPAWNER"), round(x), round(y - 1.0), false, 0.0, 0.0, true);
    return door;
}

std::vector<Entity*> Layer::items() const
{
    std::vector<Entity*> res(items_.size);
    Entity** ptr = items_.begin;
    std::copy(ptr, ptr + res.size(), &res[0]);
    return res;
}
