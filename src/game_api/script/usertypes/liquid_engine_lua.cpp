#include "liquid_engine_lua.hpp"

#include "heap_base.hpp"
#include "liquid_engine.hpp" // for LiquidPhysicsEngine

namespace NLiquidEngine
{
void register_usertypes(sol::state& lua)
{
    /// Used in LiquidPool
    lua.new_usertype<LiquidPhysicsEngine>(
        "LiquidPhysicsEngine",
        "pause",
        &LiquidPhysicsEngine::pause_physics,
        "gravity",
        &LiquidPhysicsEngine::gravity,
        "cohesion",
        &LiquidPhysicsEngine::cohesion,
        "elasticity",
        &LiquidPhysicsEngine::agitation,
        "size",
        &LiquidPhysicsEngine::blob_size,
        "weight",
        &LiquidPhysicsEngine::weight,
        "count",
        &LiquidPhysicsEngine::entity_count);

    /// Used in LiquidPool
    lua.new_usertype<LiquidPhysicsParams>(
        "LiquidPhysicsParams",
        "gravity",
        &LiquidPhysicsParams::gravity,
        "cohesion",
        &LiquidPhysicsParams::cohesion,
        "elasticity",
        &LiquidPhysicsParams::agitation);

    /// Used in LiquidPhysics
    lua.new_usertype<LiquidPool>(
        "LiquidPool",
        "default",
        &LiquidPool::physics_defaults,
        "engine",
        &LiquidPool::physics_engine);

    /// Use LIQUID_POOL enum for the index<br/>
    /// Used in StateMemory
    lua.new_usertype<LiquidPhysics>(
        "LiquidPhysics",
        "pools",
        &LiquidPhysics::pools);

    lua.create_named_table(
        "LIQUID_POOL",
        "WATER",
        1,
        "COARSE_WATER",
        2,
        "LAVA",
        3,
        "COARSE_LAVA",
        4,
        "STAGNANT_LAVA",
        5);
    /// Removes all liquid that is about to go out of bounds, this would normally crash the game, but playlunky/overlunky patch this bug.
    /// The patch however does not destroy the liquids that fall pass the level bounds,
    /// so you may still want to use this function if you spawn a lot of liquid that may fall out of the level
    lua["fix_liquid_out_of_bounds"] = []()
    { HeapBase::get().liquid_physics()->remove_liquid_oob() /**/; };

    /// Updates the floor collisions used by the liquids, set add to false to remove tile of collision, set to true to add one
    /// optional `layer` parameter to be used when liquid was moved to back layer using [set_liquid_layer](#set_liquid_layer)
    lua["update_liquid_collision_at"] = update_liquid_collision_at;

    /// Optimized function to check for the amount of liquids at a certain position, by accessing a 2d array of liquids by third of a tile. Try the `liquids.lua` example to know better how it works.
    /// Returns a pair of water and lava, in that order.
    /// Water blobs increase the number by 2 on the grid, while lava blobs increase it by 3. The maximum is usually 6.
    /// Coarse water increase the number by 3, coarse and stagnant lava by 6. Combinations of both normal and coarse can make the number higher than 6.
    lua["get_liquids_at"] = get_liquids_at;

    /// Get the current layer that the liquid is spawn in. Related function [set_liquid_layer](#set_liquid_layer)
    lua["get_liquid_layer"] = get_liquid_layer;

    /// Attach liquid collision to entity by uid (this is what the push blocks use)
    /// Collision is based on the entity's hitbox, collision is removed when the entity is destroyed (bodies of killed entities will still have the collision)
    /// Use only for entities that can move around, (for static prefer [update_liquid_collision_at](#update_liquid_collision_at) )
    /// If entity is in back layer and liquid in the front, there will be no collision created, also collision is not destroyed when entity changes layers, so you have to handle that yourself
    lua["add_entity_to_liquid_collision"] = add_entity_to_liquid_collision;
}
} // namespace NLiquidEngine
