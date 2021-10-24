#include "events.hpp"
#include "strings.hpp"

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

std::string pre_get_random_room(int x, int y, uint8_t layer, uint16_t room_template)
{
    std::string manual_room_data{};
    LuaBackend::for_each_backend(
        [=, &manual_room_data](LuaBackend& backend)
        {
            auto this_data = backend.pre_get_random_room(x, y, layer, room_template);
            if (!this_data.empty())
            {
                manual_room_data = std::move(this_data);
                return false;
            }
            return true;
        });
    return manual_room_data;
}
std::optional<LevelGenRoomData> pre_handle_room_tiles(LevelGenRoomData room_data, int x, int y, uint16_t room_template)
{
    std::optional<LevelGenRoomData> modded_room_data{std::nullopt};
    LuaBackend::for_each_backend(
        [=, &room_data, &modded_room_data](LuaBackend& backend)
        {
            auto [stop_callback, this_modded_room_data] = backend.pre_handle_room_tiles(room_data, x, y, room_template);
            if (this_modded_room_data)
            {
                room_data = this_modded_room_data.value();
                modded_room_data = this_modded_room_data;
            }
            return !stop_callback;
        });
    return modded_room_data;
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

std::u16string pre_speach_bubble(Entity* entity, char16_t* buffer)
{
    std::u16string new_string{};
    bool return_empty_str = false;
    LuaBackend::for_each_backend(
        [=, &new_string, &return_empty_str](LuaBackend& backend)
        {
            auto this_data = backend.pre_speach_bubble(entity, buffer);
            if (this_data.empty())
                return_empty_str = true;

            if (new_string.empty() || new_string == no_return_str)
            {
                new_string = std::move(this_data);
            }
            return true;
        });
    return return_empty_str ? u"" : new_string;
}

std::u16string pre_toast(char16_t* buffer)
{
    std::u16string new_string{};
    bool return_empty_str = false;
    LuaBackend::for_each_backend(
        [=, &new_string, &return_empty_str](LuaBackend& backend)
        {
            auto this_data = backend.pre_toast(buffer);
            if (this_data.empty())
                return_empty_str = true;

            if (new_string.empty() || new_string == no_return_str)
            {
                new_string = std::move(this_data);
            }
            return true;
        });
    return return_empty_str ? u"" : new_string;
}
