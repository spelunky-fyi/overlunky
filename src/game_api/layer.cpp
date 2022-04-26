#include "layer.hpp"

#include "entities_floors.hpp"
#include "entity.hpp"
#include "memory.hpp"
#include "rpc.hpp"
#include "state.hpp"

Entity* Layer::spawn_entity(ENT_TYPE id, float x, float y, bool screen, float vx, float vy, bool snap)
{
    if (id == 0)
        return nullptr;

    using LoadItem = Entity*(Layer*, ENT_TYPE, float, float, bool);
    static auto load_item = (LoadItem*)get_address("load_item");

    float min_speed_check = 0.01f;
    if (!screen && snap)
    {
        x = round(x);
        y = round(y);
    }
    else if (screen)
    {
        auto state = State::get();
        std::tie(x, y) = state.click_position(x, y);
        min_speed_check = 0.04f;
        if (snap && abs(vx) + abs(vy) <= min_speed_check)
        {
            x = round(x);
            y = round(y);
        }
    }

    Entity* spawned = load_item(this, id, x, y, false);
    if (abs(vx) + abs(vy) > min_speed_check && spawned->is_movable())
    {
        auto movable = (Movable*)spawned;
        movable->velocityx = vx;
        movable->velocityy = vy;
    }

    // DEBUG("Spawned {}", fmt::ptr(spawned));
    return spawned;
}

void snap_to_floor(Entity* ent, float y)
{
    ent->y = y + ent->hitboxy - ent->offsety;
    Entity* overlay = ent->overlay;
    while (overlay != nullptr)
    {
        ent->y -= overlay->y;
        overlay = overlay->overlay;
    }
}

Entity* Layer::spawn_entity_snap_to_floor(ENT_TYPE id, float x, float y)
{
    const EntityDB* type = get_type(id);
    const float y_center = roundf(y) - 0.5f;
    const float snapped_y = y_center + type->rect_collision.hitboxy - type->rect_collision.offsety;
    Entity* ent = spawn_entity(id, x, snapped_y, false, 0.0f, 0.0f, false);
    if ((type->search_flags & 0x700) == 0)
    {
        snap_to_floor(ent, y_center);
    }
    return ent;
}

Entity* Layer::spawn_entity_over(ENT_TYPE id, Entity* overlay, float x, float y)
{
    using SpawnEntityFun = Entity*(EntityFactory*, ENT_TYPE, float, float, bool, Entity*, bool);
    static auto spawn_entity_raw = (SpawnEntityFun*)get_address("spawn_entity");
    using AddToLayer = void(Layer*, Entity*);
    static auto add_to_layer = (AddToLayer*)get_address("add_to_layer");
    using AddItemPtr = void(Entity*, Entity*, bool);
    static auto add_item_ptr = (AddItemPtr*)get_address("add_item_ptr");

    Entity* ent = spawn_entity_raw(entity_factory(), id, x, y, is_back_layer, overlay, true);

    const auto param_5 = true;
    if (((bool*)this)[0x64490] == false && param_5 == false)
    {
        add_item_ptr(((Entity**)this)[0x64440 / 0x8], ent, false);
    }
    else
    {
        add_to_layer(this, ent);
    }
    return ent;
}

Entity* Layer::get_grid_entity_at(float x, float y)
{
    const uint32_t ix = static_cast<uint32_t>(x + 0.5f);
    const uint32_t iy = static_cast<uint32_t>(y + 0.5f);
    if (ix < g_level_max_x && iy < g_level_max_y)
    {
        return grid_entities[iy][ix];
    }
    return nullptr;
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
    door->as<ExitDoor>()->world = w;
    door->as<ExitDoor>()->level = l;
    door->as<ExitDoor>()->theme = t;
    door->as<ExitDoor>()->special_door = true;
    spawn_entity(to_id("ENT_TYPE_LOGICAL_PLATFORM_SPAWNER"), round(x), round(y - 1.0f), false, 0.0, 0.0, true); // TODO: not needed if there is a floor below door
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
