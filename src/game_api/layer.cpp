#include "layer.hpp"

#include <cmath>   // for round, roundf
#include <cstdint> // for uint32_t, uint8_t
#include <cstdlib> // for abs
#include <tuple>   // for tie, tuple

#include "entities_floors.hpp" // for ExitDoor
#include "entity.hpp"          // for Entity, to_id, EntityDB, entity_factory
#include "logger.h"            // for DEBUG
#include "movable.hpp"         // for Movable
#include "rpc.hpp"             // for update_liquid_collision_at
#include "search.hpp"          // for get_address
#include "state.hpp"           // for StateMemory, API

struct EntityFactory;

Entity* Layer::spawn_entity(ENT_TYPE id, float x, float y, bool screen, float vx, float vy, bool snap)
{
    if (id == 0)
        return nullptr;

    using SpawnEntity = Entity*(Layer*, ENT_TYPE, float, float, bool);
    static auto spawn = (SpawnEntity*)get_address("spawn_entity");

    float min_speed_check = 0.01f;
    if (!screen && snap)
    {
        x = round(x);
        y = round(y);
    }
    else if (screen)
    {
        std::tie(x, y) = API::click_position(x, y);
        min_speed_check = 0.04f;
        if (snap && abs(vx) + abs(vy) <= min_speed_check)
        {
            x = round(x);
            y = round(y);
        }
    }

    Entity* spawned = spawn(this, id, x, y, false);
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
    const float snapped_y = y_center + type->default_collision_info.rect.hitboxy - type->default_collision_info.rect.offsety;
    Entity* ent = spawn_entity(id, x, snapped_y, false, 0.0f, 0.0f, false);
    constexpr auto test_mask = ENTITY_MASK::FLOOR | ENTITY_MASK::BG | ENTITY_MASK::DECORATION;
    if (!(type->search_flags & test_mask))
    {
        snap_to_floor(ent, y_center);
    }
    return ent;
}

Entity* Layer::spawn_entity_over(ENT_TYPE id, Entity* overlay, float x, float y)
{
    using SpawnEntityFun = Entity*(EntityFactory*, ENT_TYPE, float, float, bool, Entity*, bool);
    static auto spawn_entity_raw = (SpawnEntityFun*)get_address("create_entity");
    using AddToLayer = void(Layer*, Entity*);
    static auto add_to_layer = (AddToLayer*)get_address("add_to_layer");

    Entity* ent = spawn_entity_raw(entity_factory(), id, x, y, is_back_layer, overlay, true);

    if (is_layer_loading == false)
    {
        unknown_entities4.insert(ent, false);
    }
    else
    {
        add_to_layer(this, ent);
    }
    return ent;
}

EntityList* Layer::get_entities_overlapping_grid_at(float x, float y) const
{
    const uint32_t ix = static_cast<uint32_t>(std::round(x));
    const uint32_t iy = static_cast<uint32_t>(std::round(y));
    if (ix < g_level_max_x && iy < g_level_max_y)
    {
        return const_cast<EntityList*>(&entities_overlapping_grid[iy][ix]);
    }
    return nullptr;
}

Entity* Layer::get_grid_entity_at(float x, float y) const
{
    const uint32_t ix = static_cast<uint32_t>(std::round(x));
    const uint32_t iy = static_cast<uint32_t>(std::round(y));
    if (ix < g_level_max_x && iy < g_level_max_y)
    {
        return grid_entities[iy][ix];
    }
    return nullptr;
}

Entity* Layer::get_entity_at(float x, float y, ENTITY_MASK search_flags, uint32_t include_flags, uint32_t exclude_flags, uint32_t one_of_flags)
{
    using get_entity_at_impl_fun = Entity*(Layer*, float, float, ENTITY_MASK, size_t, size_t, size_t);
    static auto get_entity_at_impl = (get_entity_at_impl_fun*)get_address("layer_get_entity_at"sv);
    return get_entity_at_impl(this, x, y, search_flags, include_flags, exclude_flags, one_of_flags);
}

Entity* Layer::spawn_door(float x, float y, uint8_t w, uint8_t l, uint8_t t)
{
    auto screen = get_state_ptr()->screen_next;
    Entity* door;
    switch (screen)
    {
    case 11:
    {
        door = spawn_entity(to_id("ENT_TYPE_FLOOR_DOOR_STARTING_EXIT"), round(x), round(y), false, 0.0, 0.0, true);
        break;
    }
    case 12:
    {
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
    constexpr auto facing_left_flag = 1 << 16;

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
            auto body_parts = apep_head->items;
            int temp = current_uid;
            for (auto body_part : body_parts.entities())
            {
                body_part->flags = right
                                       ? body_part->flags & ~facing_left_flag
                                       : body_part->flags | facing_left_flag;
                body_part->x *= -1.0f;
                if (body_part->type->id == body_id)
                {
                    current_uid = body_part->uid;
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

void Layer::move_grid_entity(Entity* ent, float x, float y, Layer* dest_layer)
{
    move_grid_entity(ent, static_cast<uint32_t>(std::round(x)), static_cast<uint32_t>(std::round(y)), dest_layer);
}

void Layer::move_grid_entity(Entity* ent, uint32_t x, uint32_t y, Layer* dest_layer)
{
    if (ent)
    {
        const auto pos = ent->abs_position();
        const uint32_t current_grid_x = static_cast<uint32_t>(std::round(pos.x));
        const uint32_t current_grid_y = static_cast<uint32_t>(std::round(pos.y));
        if (current_grid_x < g_level_max_x && current_grid_y < g_level_max_y)
        {
            if (grid_entities[current_grid_y][current_grid_x] == ent)
                grid_entities[current_grid_y][current_grid_x] = nullptr;
        }
        if (x < g_level_max_x && y < g_level_max_y)
        {
            dest_layer->grid_entities[y][x] = ent;
        }
        for (auto item_ent : ent->items.entities())
        {
            move_grid_entity(item_ent, x + item_ent->x, y + item_ent->y, dest_layer);
        }
    }
}

void Layer::destroy_grid_entity(Entity* ent)
{
    if (ent)
    {
        auto items = ent->items.entities();
        for (auto ptr = items.cend(); ptr != items.cbegin();)
        {
            ptr--;
            Entity* item_ent = *ptr;
            if (!item_ent->is_player()) // if not player
            {
                destroy_grid_entity(item_ent);
            }
        }

        const auto pos = ent->abs_position();
        const uint32_t current_grid_x = static_cast<uint32_t>(std::round(pos.x));
        const uint32_t current_grid_y = static_cast<uint32_t>(std::round(pos.y));
        if (current_grid_x < g_level_max_x && current_grid_y < g_level_max_y)
        {
            if (grid_entities[current_grid_y][current_grid_x] == ent)
            {
                grid_entities[current_grid_y][current_grid_x] = nullptr;
                update_liquid_collision_at(pos.x, pos.y, false);
            }
        }

        ent->flags |= 1U << (29 - 1); // set DEAD flag to prevent certain stuff like gold nuggets drop or particles from entities such as spikes
        ent->destroy();
    }
}

void EntityList::insert(Entity* ent, bool b)
{
    using EntityListInsert = void(EntityList*, Entity*, bool);
    static EntityListInsert* list_insert = (EntityListInsert*)get_address("entitylist_insert");
    list_insert(this, ent, b);
}
