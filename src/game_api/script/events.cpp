#include "events.hpp"

void pre_load_level_files()
{
    LuaBackend::for_each_backend(
        [&](LuaBackend& backend)
        {
            backend.pre_load_level_files();
            return true;
        });
}
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

bool pre_tile_code_spawn(std::string_view tile_code, float x, float y, int layer, uint16_t room_template)
{
    bool block_spawn{false};
    LuaBackend::for_each_backend(
        [&](LuaBackend& backend)
        {
            block_spawn = backend.pre_tile_code(tile_code, x, y, layer, room_template);
            return !block_spawn;
        });
    return block_spawn;
}
void post_tile_code_spawn(std::string_view tile_code, float x, float y, int layer, uint16_t room_template)
{

    LuaBackend::for_each_backend(
        [&](LuaBackend& backend)
        {
            backend.post_tile_code(tile_code, x, y, layer, room_template);
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

void trigger_vanilla_render_callbacks(ON event)
{
    LuaBackend::for_each_backend(
        [&](LuaBackend& backend)
        {
            backend.process_vanilla_render_callbacks(event);
            return true;
        });
}

void trigger_vanilla_render_draw_depth_callbacks(ON event, uint8_t draw_depth, const AABB& bbox)
{
    LuaBackend::for_each_backend(
        [&](LuaBackend& backend)
        {
            backend.process_vanilla_render_draw_depth_callbacks(event, draw_depth, bbox);
            return true;
        });
}
