#pragma once

#include <cstdint>
#include <string_view>

class Entity;

void pre_load_level_files();
void pre_level_generation();
void post_room_generation();
void post_level_generation();

bool pre_tile_code_spawn(std::string_view tile_code, float x, float y, int layer, uint16_t room_template);
void post_tile_code_spawn(std::string_view tile_code, float x, float y, int layer, uint16_t room_template);

Entity* pre_entity_spawn(std::uint32_t entity_type, float x, float y, int layer, Entity* overlay, int spawn_type_flags);
void post_entity_spawn(Entity* entity, int spawn_type_flags);
