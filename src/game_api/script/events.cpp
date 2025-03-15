#include "events.hpp"

#include <fmt/format.h> // for check_format_string, format, vformat
#include <functional>   // for _Func_impl_no_alloc<>::_Mybase
#include <new>          // for operator new
#include <type_traits>  // for move
#include <utility>      // for max, pair, min

#include "bucket.hpp"
#include "constants.hpp"          // for no_return_str
#include "level_api_types.hpp"    // for LevelGenRoomData
#include "rpc.hpp"                // for game_log, get_adventure_seed
#include "savestate.hpp"          // for invalidate_save_slots
#include "script/lua_backend.hpp" // for LuaBackend, ON, LuaBackend::PreHan...
#include "settings_api.hpp"       // for restore_original_settings
#include "state.hpp"              // for StateMemory, State

class JournalPage;
struct AABB;

auto g_level_loaded = false;

void pre_load_level_files()
{
    LuaBackend::for_each_backend(
        [&](LuaBackend::LockedBackend backend)
        {
            backend->pre_load_level_files();
            return true;
        });
}
bool pre_load_screen()
{
    static int64_t prev_seed = 0;
    auto state = State::get().ptr();
    if (state->screen_next == 12 && (state->quest_flags & 1) != 0)
    {
        if ((state->quest_flags & (1U << 6)) > 0)
        {
            auto seed = state->seed;
            if (seed != prev_seed)
                game_log(fmt::format("Seeded Seed: {:X}", seed));
            prev_seed = seed;
        }
        else
        {
            auto seed = get_adventure_seed(false);
            if (seed.second != prev_seed)
                game_log(fmt::format("{} Seed: {:X} {:X}", ((state->quest_flags & (1U << 7)) > 0 && state->screen == 28) ? "Daily" : "Adventure", (uint64_t)seed.first, (uint64_t)seed.second));
            prev_seed = seed.second;
            auto bucket = Bucket::get();
            bucket->adventure_seed = std::make_pair(seed.first, seed.second);
        }
    }

    if (state->screen_next != 5)
        unlock_settings();

    bool block{false};
    LuaBackend::for_each_backend(
        [&](LuaBackend::LockedBackend backend)
        {
            block = backend->pre_load_screen();
            return !block;
        });

    if (state->screen_next == 5 && !block)
        restore_original_settings();

    return block;
}
bool pre_unload_level()
{
    bool block{false};
    LuaBackend::for_each_backend(
        [&](LuaBackend::LockedBackend backend)
        {
            block = backend->pre_unload_level();
            return !block;
        });
    if (!block)
    {
        g_level_loaded = false;
        invalidate_save_slots();
    }
    return block;
}
bool pre_init_level()
{
    bool block{false};
    LuaBackend::for_each_backend(
        [&](LuaBackend::LockedBackend backend)
        {
            block = backend->pre_init_level();
            return !block;
        });
    if (!block)
        g_level_loaded = true;
    return block;
}
bool pre_unload_layer(LAYER layer)
{
    bool block{false};
    LuaBackend::for_each_backend(
        [&](LuaBackend::LockedBackend backend)
        {
            block = backend->pre_unload_layer(layer);
            return !block;
        });
    return block;
}
bool pre_init_layer(LAYER layer)
{
    bool block{false};
    LuaBackend::for_each_backend(
        [&](LuaBackend::LockedBackend backend)
        {
            block = backend->pre_init_layer(layer);
            return !block;
        });
    return block;
}

bool pre_save_state(int slot, StateMemory* saved)
{
    bool block{false};
    LuaBackend::for_each_backend(
        [&](LuaBackend::LockedBackend backend)
        {
            block = backend->pre_save_state(slot, saved);
            return !block;
        });
    return block;
}

bool pre_load_state(int slot, StateMemory* loaded)
{
    bool block{false};
    LuaBackend::for_each_backend(
        [&](LuaBackend::LockedBackend backend)
        {
            block = backend->pre_load_state(slot, loaded);
            return !block;
        });
    return block;
}

void post_room_generation()
{
    LuaBackend::for_each_backend(
        [&](LuaBackend::LockedBackend backend)
        {
            backend->post_room_generation();
            return true;
        });
}
void post_level_generation()
{
    LuaBackend::for_each_backend(
        [&](LuaBackend::LockedBackend backend)
        {
            backend->post_level_generation();
            return true;
        });
}
void post_init_layer(LAYER layer)
{
    LuaBackend::for_each_backend(
        [&](LuaBackend::LockedBackend backend)
        {
            backend->post_init_layer(layer);
            return true;
        });
}
void post_load_screen()
{
    LuaBackend::for_each_backend(
        [&](LuaBackend::LockedBackend backend)
        {
            backend->post_load_screen();
            return true;
        });
}
void post_unload_layer(LAYER layer)
{
    LuaBackend::for_each_backend(
        [&](LuaBackend::LockedBackend backend)
        {
            backend->post_unload_layer(layer);
            return true;
        });
}
void post_save_state(int slot, StateMemory* saved)
{
    LuaBackend::for_each_backend(
        [&](LuaBackend::LockedBackend backend)
        {
            backend->post_save_state(slot, saved);
            return true;
        });
}
void post_load_state(int slot, StateMemory* loaded)
{
    LuaBackend::for_each_backend(
        [&](LuaBackend::LockedBackend backend)
        {
            backend->post_load_state(slot, loaded);
            return true;
        });
}
void on_death_message(STRINGID stringid)
{
    LuaBackend::for_each_backend(
        [&](LuaBackend::LockedBackend backend)
        {
            backend->on_death_message(stringid);
            return true;
        });
}

std::string pre_get_random_room(int x, int y, uint8_t layer, uint16_t room_template)
{
    std::string manual_room_data{};
    LuaBackend::for_each_backend(
        [=, &manual_room_data](LuaBackend::LockedBackend backend)
        {
            auto this_data = backend->pre_get_random_room(x, y, layer, room_template);
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
        [=, &room_data, &modded_room_data](LuaBackend::LockedBackend backend)
        {
            auto [stop_callback, this_modded_room_data] = backend->pre_handle_room_tiles(room_data, x, y, room_template);
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
        [&](LuaBackend::LockedBackend backend)
        {
            block_spawn = backend->pre_tile_code(tile_code, x, y, layer, room_template);
            return !block_spawn;
        });
    return block_spawn;
}
void post_tile_code_spawn(std::string_view tile_code, float x, float y, int layer, uint16_t room_template)
{

    LuaBackend::for_each_backend(
        [&](LuaBackend::LockedBackend backend)
        {
            backend->post_tile_code(tile_code, x, y, layer, room_template);
            return true;
        });
}

Entity* pre_entity_spawn(std::uint32_t entity_type, float x, float y, int layer, Entity* overlay, int spawn_type_flags)
{
    Entity* spawned_ent{nullptr};
    LuaBackend::for_each_backend(
        [=, &spawned_ent](LuaBackend::LockedBackend backend)
        {
            spawned_ent = backend->pre_entity_spawn(entity_type, x, y, layer, overlay, spawn_type_flags);
            return spawned_ent == nullptr;
        });
    return spawned_ent;
}
void post_entity_spawn(Entity* entity, int spawn_type_flags)
{
    LuaBackend::for_each_backend(
        [=](LuaBackend::LockedBackend backend)
        {
            backend->post_entity_spawn(entity, spawn_type_flags);
            return true;
        });
}

bool pre_entity_instagib(Entity* victim)
{
    bool skip{false};
    LuaBackend::for_each_backend(
        [&](LuaBackend::LockedBackend backend)
        {
            skip |= backend->pre_entity_instagib(victim);
            return true;
        });
    return skip;
}

bool trigger_vanilla_render_callbacks(ON event)
{
    bool skip{false};
    LuaBackend::for_each_backend(
        [&](LuaBackend::LockedBackend backend)
        {
            skip |= backend->process_vanilla_render_callbacks(event);
            return true;
        });
    return skip;
}

bool trigger_vanilla_render_blur_callbacks(ON event, float blur_amount)
{
    bool skip{false};
    LuaBackend::for_each_backend(
        [&](LuaBackend::LockedBackend backend)
        {
            skip |= backend->process_vanilla_render_blur_callbacks(event, blur_amount);
            return true;
        });
    return skip;
}

bool trigger_vanilla_render_hud_callbacks(ON event, Hud* hud)
{
    bool skip{false};
    LuaBackend::for_each_backend(
        [&](LuaBackend::LockedBackend backend)
        {
            skip |= backend->process_vanilla_render_hud_callbacks(event, hud);
            return true;
        });
    return skip;
}

bool trigger_vanilla_render_layer_callbacks(ON event, uint8_t layer)
{
    bool skip{false};
    LuaBackend::for_each_backend(
        [&](LuaBackend::LockedBackend backend)
        {
            skip |= backend->process_vanilla_render_layer_callbacks(event, layer);
            return true;
        });
    return skip;
}

bool trigger_vanilla_render_draw_depth_callbacks(ON event, uint8_t draw_depth, const AABB& bbox)
{
    bool skip{false};
    LuaBackend::for_each_backend(
        [&](LuaBackend::LockedBackend backend)
        {
            skip |= backend->process_vanilla_render_draw_depth_callbacks(event, draw_depth, bbox);
            return true;
        });
    return skip;
}

bool trigger_vanilla_render_journal_page_callbacks(ON event, JournalPageType page_type, JournalPage* page)
{
    bool skip{false};
    LuaBackend::for_each_backend(
        [&](LuaBackend::LockedBackend backend)
        {
            skip |= backend->process_vanilla_render_journal_page_callbacks(event, page_type, page);
            return true;
        });
    return skip;
}

std::u16string pre_speach_bubble(Entity* entity, char16_t* buffer)
{
    std::u16string new_string{no_return_str};
    bool return_empty_str = false;
    LuaBackend::for_each_backend(
        [=, &new_string, &return_empty_str](LuaBackend::LockedBackend backend)
        {
            auto this_data = backend->pre_speach_bubble(entity, buffer);
            if (this_data.empty() && new_string == no_return_str)
                return_empty_str = true;

            if (new_string == no_return_str)
            {
                new_string = std::move(this_data);
            }
            return true;
        });
    return return_empty_str ? u"" : new_string;
}

std::u16string pre_toast(char16_t* buffer)
{
    std::u16string new_string{no_return_str};
    bool return_empty_str = false;
    LuaBackend::for_each_backend(
        [=, &new_string, &return_empty_str](LuaBackend::LockedBackend backend)
        {
            auto this_data = backend->pre_toast(buffer);
            if (this_data.empty() && new_string == no_return_str)
                return_empty_str = true;

            if (new_string == no_return_str)
            {
                new_string = std::move(this_data);
            }
            return true;
        });
    return return_empty_str ? u"" : new_string;
}

void update_backends()
{
    LuaBackend::for_each_backend(
        [=](LuaBackend::LockedBackend backend)
        {
            backend->update();
            return true;
        });
}

bool pre_load_journal_chapter(uint8_t chapter)
{
    bool return_value = false;
    LuaBackend::for_each_backend(
        [=, &chapter, &return_value](LuaBackend::LockedBackend backend)
        {
            return_value = backend->pre_load_journal_chapter(chapter);
            if (return_value)
                return false;

            return true;
        });

    return return_value;
}

std::vector<uint32_t> post_load_journal_chapter(uint8_t chapter, const std::vector<uint32_t>& pages)
{
    std::vector<uint32_t> new_pages;
    LuaBackend::for_each_backend(
        [=, &new_pages, &pages](LuaBackend::LockedBackend backend)
        {
            auto returned_pages = backend->post_load_journal_chapter(chapter, pages);
            if (!returned_pages.empty())
            {
                new_pages = std::move(returned_pages);
            }
            return true;
        });

    return new_pages;
}

std::optional<bool> pre_get_feat(FEAT feat)
{
    std::optional<bool> got;
    bool block{false};
    LuaBackend::for_each_backend(
        [&](LuaBackend::LockedBackend backend)
        {
            std::optional<bool> ret = backend->pre_get_feat(feat);
            if (ret.has_value())
            {
                block = true;
                got = ret;
            }
            return !block;
        });
    return got;
}

bool pre_set_feat(FEAT feat)
{
    bool block{false};
    LuaBackend::for_each_backend(
        [&](LuaBackend::LockedBackend backend)
        {
            block = backend->pre_set_feat(feat);
            return !block;
        });
    return block;
}

void pre_copy_state_event(StateMemory* from, StateMemory* to)
{
    LuaBackend::for_each_backend(
        [&](LuaBackend::LockedBackend backend)
        {
            backend->pre_copy_state(from, to);
            return true;
        });
}
