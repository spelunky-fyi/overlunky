#include "events.hpp"

#include "lua_backend.hpp"

void pre_level_generation()
{
    LuaBackend::for_each_backend(
        [&](LuaBackend& backend)
        {
            backend.pre_level_generation();
            return true;
        });
}
void post_room_generation()
{
    LuaBackend::for_each_backend(
        [&](LuaBackend& backend)
        {
            backend.post_room_generation();
            return true;
        });
}
void post_level_generation()
{
    LuaBackend::for_each_backend(
        [&](LuaBackend& backend)
        {
            backend.post_level_generation();
            return true;
        });
}

bool pre_tile_code_spawn(std::string_view tile_code, float x, float y, int layer)
{
    bool block_spawn{false};
    LuaBackend::for_each_backend(
        [&](LuaBackend& backend)
        {
            block_spawn = backend.pre_level_gen_spawn(tile_code, x, y, layer);
            return !block_spawn;
        });
    return block_spawn;
}
void post_tile_code_spawn(std::string_view tile_code, float x, float y, int layer)
{

    LuaBackend::for_each_backend(
        [&](LuaBackend& backend)
        {
            backend.post_level_gen_spawn(tile_code, x, y, layer);
            return true;
        });
}

Entity* pre_entity_spawn(std::uint32_t entity_type, float x, float y, int layer, Entity* overlay, int spawn_type_flags)
{
    Entity* spawned_ent{nullptr};
    LuaBackend::for_each_backend(
        [=, &spawned_ent](LuaBackend& backend)
        {
            spawned_ent = backend.pre_entity_spawn(entity_type, x, y, layer, overlay, spawn_type_flags);
            return spawned_ent == nullptr;
        });
    return spawned_ent;
}
void post_entity_spawn(Entity* entity, int spawn_type_flags)
{
    LuaBackend::for_each_backend(
        [=](LuaBackend& backend)
        {
            backend.post_entity_spawn(entity, spawn_type_flags);
            return true;
        });
}
