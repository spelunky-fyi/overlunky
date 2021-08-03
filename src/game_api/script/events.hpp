#pragma once

#include <cstdint>
#include <string_view>

class Entity;

void pre_level_generation();
void post_room_generation();
void post_level_generation();

bool pre_tile_code_spawn(std::string_view tile_code, float x, float y, int layer);
void post_tile_code_spawn(std::string_view tile_code, float x, float y, int layer);

Entity* pre_entity_spawn(std::uint32_t entity_type, float x, float y, int layer, Entity* overlay, int spawn_type_flags);
void post_entity_spawn(Entity* entity, int spawn_type_flags);
