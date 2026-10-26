#include "liquid_engine.hpp"

#include "aliases.hpp"
#include "entities_liquids.hpp"
#include "entity.hpp"
#include "game_patches.hpp"
#include "memory.hpp"
#include "search.hpp"
#include "state.hpp"

uint8_t get_liquid_layer()
{
    static auto addr = get_address("check_if_collides_with_liquid_layer");
    return memory_read<uint8_t>(addr);
}

void update_liquid_collision_at(float x, float y, bool add, std::optional<LAYER> layer)
{
    using UpdateLiquidCollision = void(LiquidPhysics*, int32_t, int32_t, uint8_t);
    static UpdateLiquidCollision* RemoveLiquidCollision_fun = (UpdateLiquidCollision*)get_address("remove_from_liquid_collision_map");
    static UpdateLiquidCollision* AddLiquidCollision_fun = (UpdateLiquidCollision*)get_address("add_to_liquid_collision_map");
    auto state = get_state_ptr();
    uint8_t actual_layer = enum_to_layer(layer.value_or(LAYER::FRONT));

    if (add)
        AddLiquidCollision_fun(state->liquid_physics, static_cast<int32_t>(std::round(x)), static_cast<int32_t>(std::round(y)), actual_layer);
    else
        RemoveLiquidCollision_fun(state->liquid_physics, static_cast<int32_t>(std::round(x)), static_cast<int32_t>(std::round(y)), actual_layer);
}

void add_entity_to_liquid_collision(uint32_t uid, bool add)
{
    using AddEntityLiquidCollision = void(LiquidPhysics*, Entity*, uint8_t);
    static AddEntityLiquidCollision* add_entity_liquid_collision = (AddEntityLiquidCollision*)get_address("add_movable_to_liquid_collision_map");
    auto state = get_state_ptr();
    auto entity = get_entity_ptr(uid);
    if (!entity)
        return;

    auto map = state->liquid_physics->push_blocks;
    if (!map)
        return;

    auto it = map->find(uid);

    // if it already exists we can't add it again, since it will create the collision struct anyway and just overwrite the pointer to it in the map
    // the actual collision struct is held somewhere else, unrelated to this map
    if (add && it == map->end())
        add_entity_liquid_collision(state->liquid_physics, entity, entity->layer);
    else if (!add && it != map->end())
    {
        // very illegal, don't do this, we can because we're professionals xd
        // game loops thru the map and checks if uid still exists, if not, it removes the collision
        // which is some bigger struct held in some weird container, and the function is doing other stuff, so this is the easiest way besides killing the entity
        auto key = const_cast<uint32_t*>(&it->first);
        *key = ~0u;
    }
}

std::pair<uint8_t, uint8_t> get_liquids_at(float x, float y, LAYER layer)
{
    uint8_t actual_layer = enum_to_layer(layer);
    LiquidPhysics* liquid_physics = HeapBase::get().liquid_physics();
    // if (y > 125.5f || y < .0f || x > 85.5f || x < .0f) // Original check by the game, can result in accessing the array out of bounds
    //     return 0;
    if (actual_layer != get_liquid_layer() || y < .0f || x < .0f)
        return {0, 0};

    uint32_t ix = static_cast<int>((x + 0.5f) / 0.3333333f);
    uint32_t iy = static_cast<int>((y + 0.5f) / 0.3333333f);
    if (iy >= (g_level_max_y * 3) || ix >= (g_level_max_x * 3))
        return {0, 0};

    auto& liquids_at = (*liquid_physics->liquids_by_third_of_tile)[iy][ix];
    return {liquids_at.water, liquids_at.lava};
}

void move_liquid_abs(uint32_t uid, float x, float y, float vx, float vy)
{
    auto entity = get_entity_ptr(uid)->as<Liquid>();
    if (entity)
    {
        auto liquid_engine = HeapBase::get().liquid_physics()->get_correct_liquid_engine(entity->type->id);
        if (liquid_engine)
        {
            liquid_engine->entity_coordinates[*entity->liquid_id] = {x, y};
            liquid_engine->entity_velocities[*entity->liquid_id] = {vx, vy};
        }
    }
}
