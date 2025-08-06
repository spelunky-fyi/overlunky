#pragma once

#include <cstdint>     // for uint16_t, uint8_t, uint32_t
#include <optional>    // for optional
#include <string>      // for u16string, string
#include <string_view> // for string_view

#include "aliases.hpp"             // for JournalPageType
#include "handle_lua_function.hpp" // for handle_function
#include "heap_base.hpp"           // for HeapBase
#include "lua_backend.hpp"         // for ON
#include "rpc.hpp"                 // for get_frame_count

class Entity;
class JournalPage;
struct AABB;
struct LevelGenRoomData;
struct HudData;
struct Hud;

template <bool stop_propagation = false, class... ArgsT>
bool backend_pre(LuaBackend& backend, ON event, ArgsT... args)
{
    bool skip{false};
    if (!backend.get_enabled())
        return skip;

    auto now = HeapBase::get().frame_count();
    for (auto& [id, callback] : backend.callbacks)
    {
        if (backend.is_callback_cleared(id))
            continue;

        if (callback.screen == event)
        {
            auto _scope = backend.set_current_callback(-1, id, CallbackType::Normal);
            skip |= handle_function<bool>(&backend, callback.func, args...).value_or(false);
            callback.lastRan = now;
            if constexpr (stop_propagation)
                if (skip)
                    return skip;
        }
    }

    return skip;
}

template <class... ArgsT>
void backend_post(LuaBackend& backend, ON event, ArgsT... args)
{
    if (!backend.get_enabled())
        return;

    auto now = HeapBase::get().frame_count();
    for (auto& [id, callback] : backend.callbacks)
    {
        if (backend.is_callback_cleared(id))
            continue;

        if (callback.screen == event)
        {
            auto _scope = backend.set_current_callback(-1, id, CallbackType::Normal);
            handle_function<void>(&backend, callback.func, args...);
            callback.lastRan = now;
        }
    }
}

template <bool stop_propagation = false, class... ArgsT>
bool pre_event(ON event, ArgsT... args)
{
    bool return_val = false;
    LuaBackend::for_each_backend(
        [=, &return_val, &args...](LuaBackend::LockedBackend backend)
        {
            if (backend_pre<stop_propagation>(*backend, event, args...))
            {
                return_val = true;
                return false;
            }
            return true;
        },
        stop_propagation);
    return return_val;
}

template <class... ArgsT>
void post_event(ON event, ArgsT... args)
{
    LuaBackend::for_each_backend(
        [&](LuaBackend::LockedBackend backend)
        {
            backend_post(*backend, event, args...);
            return true;
        });
}

void pre_load_level_files();
bool pre_load_screen();
bool pre_init_level();
bool pre_init_layer(LAYER layer);
bool pre_unload_level();
bool pre_unload_layer(LAYER layer);
bool pre_save_state(int slot, StateMemory* saved);
bool pre_load_state(int slot, StateMemory* loaded);
void pre_copy_state_event(HeapBase from, HeapBase to);
bool pre_spawn_backlayer_rooms(uint32_t start_x, uint32_t start_y, uint32_t limit_width, uint32_t limit_height);
void post_spawn_backlayer_rooms(uint32_t start_x, uint32_t start_y, uint32_t limit_width, uint32_t limit_height);

void post_load_screen();
void post_init_layer(LAYER layer);
void post_unload_layer(LAYER layer);
void post_room_generation();
void post_level_generation();
void post_save_state(int slot, StateMemory* saved);
void post_load_state(int slot, StateMemory* loaded);

void on_death_message(STRINGID stringid);
std::optional<bool> pre_get_feat(FEAT feat);
bool pre_set_feat(FEAT feat);

std::string pre_get_random_room(int x, int y, uint8_t layer, uint16_t room_template);
std::optional<LevelGenRoomData> pre_handle_room_tiles(LevelGenRoomData room_data, int x, int y, uint16_t room_template);

bool pre_tile_code_spawn(std::string_view tile_code, float x, float y, int layer, uint16_t room_template);
void post_tile_code_spawn(std::string_view tile_code, float x, float y, int layer, uint16_t room_template);

Entity* pre_entity_spawn(std::uint32_t entity_type, float x, float y, int layer, Entity* overlay, int spawn_type_flags);
void post_entity_spawn(Entity* entity, int spawn_type_flags);

bool pre_entity_instagib(Entity* victim);

bool trigger_vanilla_render_callbacks(ON event);
bool trigger_vanilla_render_blur_callbacks(ON event, float blur_amount);
bool trigger_vanilla_render_hud_callbacks(ON event, Hud* hud);
bool trigger_vanilla_render_layer_callbacks(ON event, uint8_t layer);
bool trigger_vanilla_render_draw_depth_callbacks(ON event, uint8_t draw_depth, const AABB& bbox);
bool trigger_vanilla_render_journal_page_callbacks(ON event, JournalPageType page_type, JournalPage* page);

std::u16string pre_speach_bubble(Entity* entity, char16_t* buffer);
std::u16string pre_toast(char16_t* buffer);

void update_backends();

bool pre_load_journal_chapter(uint8_t chapter);
std::vector<uint32_t> post_load_journal_chapter(uint8_t chapter, const std::vector<uint32_t>& pages);
