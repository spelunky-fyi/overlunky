#include "layer.hpp"

#include "entities_floors.hpp"
#include "entity.hpp"
#include "logger.h"
#include "rpc.hpp"
#include "state.hpp"

using LoadItem = size_t (*)(Layer*, size_t, float, float, bool);
using LoadItemOver = Entity* (*)(Layer*, size_t, Entity*, float, float, bool);
LoadItemOver get_load_item_over()
{
    ONCE(LoadItemOver)
    {
        auto memory = Memory::get();
        auto off = find_inst(memory.exe(), "\xBA\x51\x00\x00\x00\x48\x8B"s, memory.after_bundle);
        off = find_inst(memory.exe(), "\xE8"s, off + 5);
        off = find_inst(memory.exe(), "\xE8"s, off + 5);
        return res = (LoadItemOver)memory.at_exe(Memory::decode_call(off));
    }
}

Entity* Layer::spawn_entity(size_t id, float x, float y, bool screen, float vx, float vy, bool snap)
{
    if (id == 0)
        return nullptr;
    auto load_item = (LoadItem)get_address("load_item");
    size_t addr;
    Entity* spawned;
    float min_speed_check = 0.01f;
    if (!screen)
    {
        if (snap)
        {
            x = round(x);
            y = round(y);
        }
        addr = load_item(this, id, x, y, false);
        spawned = (Entity*)(addr);
    }
    else
    {
        auto state = State::get();
        auto [rx, ry] = state.click_position(x, y);
        min_speed_check = 0.04f;
        if (snap && abs(vx) + abs(vy) <= min_speed_check)
        {
            rx = round(rx);
            ry = round(ry);
        }
        addr = load_item(this, id, rx, ry, false);
        spawned = (Entity*)(addr);
    }
    if (abs(vx) + abs(vy) > min_speed_check && spawned->is_movable())
    {
        auto movable = (Movable*)spawned;
        movable->velocityx = vx;
        movable->velocityy = vy;
    }
    DEBUG("Spawned {:x}", addr);
    return spawned;
}

void snap_to_floor(Entity* ent, float y)
{
    ent->y = y - ent->hitboxy + ent->offsety;
    Entity* overlay = ent->overlay;
    while (overlay != nullptr)
    {
        ent->y -= overlay->y;
        overlay = overlay->overlay;
    }
}

Entity* Layer::spawn_entity_snap_to_floor(size_t id, float x, float y)
{
    const EntityDB* type = get_type(static_cast<uint32_t>(id));
    const float y_center = (float)roundf(y) - 0.5f;
    const float snapped_y = type->rect_collision.up_plus_down - type->rect_collision.up_minus_down + y_center;
    Entity* ent = spawn_entity(id, x, snapped_y, false, 0.0f, 0.0f, false);
    if ((type->search_flags & 0x700) == 0)
    {
        snap_to_floor(ent, y_center);
    }
    return ent;
}

Entity* Layer::spawn_entity_over(size_t id, Entity* overlay, float x, float y)
{
    using SpawnEntityFun = Entity*(EntityFactory*, std::uint32_t, float, float, bool, Entity*, bool);
    static auto spawn_entity_raw = (SpawnEntityFun*)get_address("spawn_entity");
    static auto fun_22872fe0 = (void (*)(void*, Entity*))get_address("fun_22872fe0");
    static auto fun_2286f240 = (void (*)(void*, Entity*, bool))get_address("fun_2286f240");

    Entity* ent = spawn_entity_raw(entity_factory(), id, x, y, *(bool*)this, overlay, false);
    if (*((bool*)this + 0x64490))
    {
        fun_22872fe0(this, ent);
    }
    else
    {
        fun_2286f240((void*)((size_t)this + 0x64440), ent, false);
    }
    return ent;
}

Entity* Layer::get_grid_entity_at(float x, float y)
{
    const uint32_t ix = static_cast<uint32_t>(x + 0.5f);
    const uint32_t iy = static_cast<uint32_t>(y + 0.5f);
    return grid_entities[ix][iy];
}

Entity* Layer::spawn_door(float x, float y, uint8_t w, uint8_t l, uint8_t t)
{
    auto screen = State::get().ptr()->screen_next;
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
    static_cast<ExitDoor*>(door)->set_target(w, l, t);
    spawn_entity(to_id("ENT_TYPE_LOGICAL_PLATFORM_SPAWNER"), round(x), round(y - 1.0f), false, 0.0, 0.0, true);
    return door;
}

Entity* Layer::spawn_apep(float x, float y, bool right)
{
    static const auto head_id = to_id("ENT_TYPE_MONS_APEP_HEAD");
    static const auto body_id = to_id("ENT_TYPE_MONS_APEP_BODY");
    static const auto facing_left_flag = 1 << 16;

    Entity* apep_head = spawn_entity(head_id, x, y, false, 0.0f, 0.0f, true);
    const bool facing_left = apep_head->flags & facing_left_flag;

    // Not pointing correct direction
    if (facing_left == right)
    {
        apep_head->flags = right
                               ? apep_head->flags & ~facing_left_flag
                               : apep_head->flags | facing_left_flag;

        int current_uid = apep_head->uid;
        do
        {
            auto body_parts = entity_get_items_by(current_uid, {}, 0);
            int temp = current_uid;
            for (auto body_part_uid : body_parts)
            {
                Entity* body_part = get_entity_ptr(body_part_uid);
                body_part->flags = right
                                       ? body_part->flags & ~facing_left_flag
                                       : body_part->flags | facing_left_flag;
                body_part->x *= -1.0f;
                if (body_part->type->id == body_id)
                {
                    current_uid = body_part_uid;
                }
            }
            if (temp == current_uid)
            {
                break;
            }
        } while (true);
    }

    return apep_head;
}

std::vector<Entity*> Layer::items() const
{
    std::vector<Entity*> res(items_.size);
    Entity** ptr = items_.begin;
    std::copy(ptr, ptr + res.size(), &res[0]);
    return res;
}
