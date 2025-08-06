#include "spawn_lua.hpp"

#include "aliases.hpp"
#include "entity.hpp"
#include "entity_lookup.hpp"
#include "script/lua_backend.hpp"
#include "spawn_api.hpp"

#include <cstdint>
#include <sol/sol.hpp>
#include <vector>

namespace NSpawn
{
void register_usertypes(sol::state& lua)
{
    auto spawn_liquid = sol::overload(
        static_cast<void (*)(ENT_TYPE, float, float)>(::spawn_liquid),
        static_cast<void (*)(ENT_TYPE, float, float, float, float, uint32_t, uint32_t)>(::spawn_liquid_ex),
        static_cast<void (*)(ENT_TYPE, float, float, float, float, uint32_t, uint32_t, float)>(::spawn_liquid));
    /// Spawn liquids, always spawns in the front layer, will have fun effects if `entity_type` is not a liquid (only the short version, without velocity etc.).
    /// Don't overuse this, you are still restricted by the liquid pool sizes and thus might crash the game.
    /// `liquid_flags` - not much known about, 2 - will probably crash the game, 3 - pause_physics, 6-12 is probably agitation, surface_tension etc. set to 0 to ignore
    /// `amount` - it will spawn amount x amount (so 1 = 1, 2 = 4, 3 = 6 etc.), `blobs_separation` is optional
    lua["spawn_liquid"] = spawn_liquid;
    /// Spawn an entity in position with some velocity and return the uid of spawned entity.
    /// Uses level coordinates with [LAYER.FRONT](#LAYER) and LAYER.BACK, but player-relative coordinates with LAYER.PLAYER(n), where (n) is a player number (1-4).
    lua["spawn_entity"] = spawn_entity_abs;
    /// Short for [spawn_entity](#spawn_entity).
    lua["spawn"] = spawn_entity_abs;
    /// Spawns an entity directly on the floor below the tile at the given position.
    /// Use this to avoid the little fall that some entities do when spawned during level gen callbacks.
    lua["spawn_entity_snapped_to_floor"] = spawn_entity_snap_to_floor;
    /// Short for [spawn_entity_snapped_to_floor](#spawn_entity_snapped_to_floor).
    lua["spawn_on_floor"] = spawn_entity_snap_to_floor;
    /// Spawn a grid entity, such as floor or traps, that snaps to the grid.
    lua["spawn_grid_entity"] = spawn_entity_snap_to_grid;
    /// Same as `spawn_entity` but does not trigger any pre-entity-spawn callbacks, so it will not be replaced by another script
    lua["spawn_entity_nonreplaceable"] = spawn_entity_abs_nonreplaceable;
    /// Short for [spawn_entity_nonreplaceable](#spawn_entity_nonreplaceable).
    lua["spawn_critical"] = spawn_entity_abs_nonreplaceable;
    /// Spawn a door to another world, level and theme and return the uid of spawned entity.
    /// Uses level coordinates with LAYER.FRONT and LAYER.BACK, but player-relative coordinates with LAYER.PLAYERn
    lua["spawn_door"] = spawn_door_abs;
    /// Short for [spawn_door](#spawn_door).
    lua["door"] = spawn_door_abs;
    /// Spawn a door to backlayer.
    lua["spawn_layer_door"] = spawn_backdoor_abs;
    /// Short for [spawn_layer_door](#spawn_layer_door).
    lua["layer_door"] = spawn_backdoor_abs;
    /// Spawns apep with the choice if it going left or right, if you want the game to choose use regular spawn functions with `ENT_TYPE.MONS_APEP_HEAD`
    lua["spawn_apep"] = spawn_apep;

    auto spawn_tree = sol::overload(
        static_cast<int32_t (*)(float, float, LAYER)>(::spawn_tree),
        static_cast<int32_t (*)(float, float, LAYER, uint16_t)>(::spawn_tree));
    /// Spawns and grows a tree
    lua["spawn_tree"] = spawn_tree;

    auto spawn_mushroom = sol::overload(
        static_cast<int32_t (*)(float, float, LAYER)>(::spawn_mushroom),
        static_cast<int32_t (*)(float, float, LAYER, uint16_t)>(::spawn_mushroom));
    /// Spawns and grows mushroom, height relates to the trunk, without it, it will roll the game default 3-5 height
    /// Regardless, if there is not enough space, it will spawn shorter one or if there is no space even for the smallest one, it will just not spawn at all
    /// Returns uid of the base or -1 if it wasn't able to spawn
    lua["spawn_mushroom"] = spawn_mushroom;

    auto spawn_unrolled_player_rope = sol::overload(
        static_cast<int32_t (*)(float, float, LAYER, TEXTURE)>(::spawn_unrolled_player_rope),
        static_cast<int32_t (*)(float, float, LAYER, TEXTURE, uint16_t)>(::spawn_unrolled_player_rope));

    /// Spawns an already unrolled rope as if created by player
    lua["spawn_unrolled_player_rope"] = spawn_unrolled_player_rope;

    /// NoDoc
    /// Spawns an impostor lake, `top_threshold` determines how much space on top is rendered as liquid but does not have liquid physics, fill that space with real liquid
    /// There needs to be other liquid in the level for the impostor lake to be visible, there can only be one impostor lake in the level
    lua["spawn_impostor_lake"] = spawn_impostor_lake;
    /// NoDoc
    /// Fixes the bounds of impostor lakes in the liquid physics engine to match the bounds of the impostor lake entities.
    lua["fix_impostor_lake_positions"] = fix_impostor_lake_positions;
    /// Spawn a player in given location, if player of that slot already exist it will spawn clone, the game may crash as this is very unexpected situation
    /// If you want to respawn a player that is a ghost, set in his Inventory `health` to above 0, and `time_of_death` to 0 and call this function, the ghost entity will be removed automatically
    lua["spawn_player"] = spawn_player;
    /// Spawn the PlayerGhost entity, it will not move and not be connected to any player, you can then use [steal_input](#steal_input) and send_input to control it
    /// or change it's `player_inputs` to the `input` of real player so he can control it directly
    lua["spawn_playerghost"] = spawn_playerghost;
    /// Add a callback for a spawn of specific entity types or mask. Set `mask` to `MASK.ANY` to ignore that.
    /// This is run before the entity is spawned, spawn your own entity and return its uid to replace the intended spawn.
    /// In many cases replacing the intended entity won't have the intended effect or will even break the game, so use only if you really know what you're doing.
    /// <br/>The callback signature is optional<int> pre_entity_spawn(ENT_TYPE entity_type, float x, float y, int layer, Entity overlay_entity, SPAWN_TYPE spawn_flags)
    lua["set_pre_entity_spawn"] = [](sol::function cb, SPAWN_TYPE flags, ENTITY_MASK mask, sol::variadic_args entity_types) -> CallbackId
    {
        std::vector<ENT_TYPE> types;
        sol::type va_type = entity_types.get_type();
        if (va_type == sol::type::number)
        {
            types = std::vector<uint32_t>(entity_types.begin(), entity_types.end());
        }
        else if (va_type == sol::type::table)
        {
            types = entity_types.get<std::vector<uint32_t>>(0);
        }
        std::vector<ENT_TYPE> proper_types = get_proper_types(std::move(types));

        auto backend = LuaBackend::get_calling_backend();
        backend->pre_entity_spawn_callbacks.push_back(EntitySpawnCallback{backend->cbcount, mask, std::move(proper_types), flags, std::move(cb)});
        return backend->cbcount++;
    };
    /// Add a callback for a spawn of specific entity types or mask. Set `mask` to `MASK.ANY` to ignore that.
    /// This is run right after the entity is spawned but before and particular properties are changed, e.g. owner or velocity.
    /// <br/>The callback signature is nil post_entity_spawn(Entity ent, SPAWN_TYPE spawn_flags)
    lua["set_post_entity_spawn"] = [](sol::function cb, SPAWN_TYPE flags, ENTITY_MASK mask, sol::variadic_args entity_types) -> CallbackId
    {
        std::vector<ENT_TYPE> types;
        sol::type va_type = entity_types.get_type();
        if (va_type == sol::type::number)
        {
            types = std::vector<uint32_t>(entity_types.begin(), entity_types.end());
        }
        else if (va_type == sol::type::table)
        {
            types = entity_types.get<std::vector<uint32_t>>(0);
        }
        std::vector<ENT_TYPE> proper_types = get_proper_types(std::move(types));

        auto backend = LuaBackend::get_calling_backend();
        backend->post_entity_spawn_callbacks.push_back(EntitySpawnCallback{backend->cbcount, mask, std::move(proper_types), flags, std::move(cb)});
        return backend->cbcount++;
    };

    /// Spawn a Shopkeeper in the coordinates and make the room their shop. Returns the Shopkeeper uid. Also see [spawn_roomowner](#spawn_roomowner).
    // lua["spawn_shopkeeper"] = [](float x, float y, LAYER layer, ROOM_TEMPLATE room_template = ROOM_TEMPLATE.SHOP) -> uint32_t
    lua["spawn_shopkeeper"] = sol::overload(
        [](float x, float y, LAYER layer)
        {
            return spawn_shopkeeper(x, y, layer);
        },
        [](float x, float y, LAYER layer, ROOM_TEMPLATE room_template)
        {
            return spawn_shopkeeper(x, y, layer, room_template);
        });

    /// Spawn a RoomOwner (or a few other like [CavemanShopkeeper](#CavemanShopkeeper)) in the coordinates and make them own the room, optionally changing the room template. Returns the RoomOwner uid.
    // lua["spawn_roomowner"] = [](ENT_TYPE owner_type, float x, float y, LAYER layer, ROOM_TEMPLATE room_template = -1) -> uint32_t
    lua["spawn_roomowner"] = sol::overload(
        [](ENT_TYPE owner_type, float x, float y, LAYER layer)
        {
            return spawn_roomowner(owner_type, x, y, layer);
        },
        [](ENT_TYPE owner_type, float x, float y, LAYER layer, int16_t room_template)
        {
            return spawn_roomowner(owner_type, x, y, layer, room_template);
        });

    /// Spawn an entity of `entity_type` attached to some other entity `over_uid`, in offset `x`, `y`
    lua["spawn_entity_over"] = spawn_entity_over;
    /// Short for [spawn_entity_over](#spawn_entity_over)
    lua["spawn_over"] = spawn_entity_over;
    /// Spawn a companion (hired hand, player character, eggplant child)
    lua["spawn_companion"] = spawn_companion;

    lua.create_named_table(
        "SPAWN_TYPE",
        "LEVEL_GEN",
        SPAWN_TYPE_LEVEL_GEN,
        "LEVEL_GEN_TILE_CODE",
        SPAWN_TYPE_LEVEL_GEN_TILE_CODE,
        "LEVEL_GEN_PROCEDURAL",
        SPAWN_TYPE_LEVEL_GEN_PROCEDURAL,
        "LEVEL_GEN_FLOOR_SPREADING",
        SPAWN_TYPE_LEVEL_GEN_FLOOR_SPREADING,
        "LEVEL_GEN_GENERAL",
        SPAWN_TYPE_LEVEL_GEN_GENERAL,
        "SCRIPT",
        SPAWN_TYPE_SCRIPT,
        "SYSTEMIC",
        SPAWN_TYPE_SYSTEMIC,
        "ANY",
        SPAWN_TYPE_ANY);
    /* SPAWN_TYPE
    // LEVEL_GEN
    // For any spawn happening during level generation, even if the call happened from the Lua API during a tile code callback.
    // LEVEL_GEN_TILE_CODE
    // Similar to LEVEL_GEN but only triggers on tile code spawns.
    // LEVEL_GEN_PROCEDURAL
    // Similar to LEVEL_GEN but only triggers on random level spawns, like snakes or bats.
    // LEVEL_GEN_FLOOR_SPREADING
    // Includes solid floor type spreading (i.e. floorstyled bleeding to existing generic floor) but also corner filling of empty tiles.
    // LEVEL_GEN_GENERAL
    // Covers all spawns during level gen that are not covered by the other two.
    // SCRIPT
    // Runs for any spawn happening through a call from the Lua API, also during level generation.
    // SYSTEMIC
    // Covers all other spawns, such as items from crates or the player throwing bombs.
    // ANY
    // Covers all of the above.
    */
}
}; // namespace NSpawn
