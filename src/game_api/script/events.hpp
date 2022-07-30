#pragma once

#include <cstdint>     // for uint16_t, uint8_t, uint32_t
#include <optional>    // for optional
#include <string>      // for u16string, string
#include <string_view> // for string_view

#include "aliases.hpp"     // for JournalPageType
#include "lua_backend.hpp" // for ON

class Entity;
class JournalPage;
struct AABB;
struct LevelGenRoomData;

void pre_load_level_files();
void pre_level_generation();
bool pre_load_screen();
void post_room_generation();
void post_level_generation();
void post_load_screen();
void on_death_message(STRINGID stringid);

std::string pre_get_random_room(int x, int y, uint8_t layer, uint16_t room_template);
std::optional<LevelGenRoomData> pre_handle_room_tiles(LevelGenRoomData room_data, int x, int y, uint16_t room_template);

bool pre_tile_code_spawn(std::string_view tile_code, float x, float y, int layer, uint16_t room_template);
void post_tile_code_spawn(std::string_view tile_code, float x, float y, int layer, uint16_t room_template);

Entity* pre_entity_spawn(std::uint32_t entity_type, float x, float y, int layer, Entity* overlay, int spawn_type_flags);
void post_entity_spawn(Entity* entity, int spawn_type_flags);

void trigger_vanilla_render_callbacks(ON event);
void trigger_vanilla_render_draw_depth_callbacks(ON event, uint8_t draw_depth, const AABB& bbox);
void trigger_vanilla_render_journal_page_callbacks(ON event, JournalPageType page_type, JournalPage* page);

std::u16string pre_speach_bubble(Entity* entity, char16_t* buffer);
std::u16string pre_toast(char16_t* buffer);

void update_backends(StateMemory* s);
