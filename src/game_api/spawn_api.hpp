#pragma once

#include <cstdint> // for int32_t, uint32_t, uint8_t, uint16_t, int16_t

#include "aliases.hpp" // for LAYER, ENT_TYPE, SPAWN_TYPE, ROOM_TEMPLATE

class Entity;
struct AABB;

enum SpawnType : SPAWN_TYPE
{
    SPAWN_TYPE_LEVEL_GEN_TILE_CODE = 1 << 0,
    SPAWN_TYPE_LEVEL_GEN_PROCEDURAL = 1 << 1,
    SPAWN_TYPE_LEVEL_GEN_FLOOR_SPREADING = 1 << 2,
    SPAWN_TYPE_LEVEL_GEN_GENERAL = 1 << 3,
    SPAWN_TYPE_LEVEL_GEN = SPAWN_TYPE_LEVEL_GEN_PROCEDURAL | SPAWN_TYPE_LEVEL_GEN_TILE_CODE | SPAWN_TYPE_LEVEL_GEN_FLOOR_SPREADING | SPAWN_TYPE_LEVEL_GEN_GENERAL,
    SPAWN_TYPE_SCRIPT = 1 << 4,
    SPAWN_TYPE_SYSTEMIC = 1 << 5,
    SPAWN_TYPE_NUM_FLAGS = 6,
    SPAWN_TYPE_ANY = SPAWN_TYPE_LEVEL_GEN | SPAWN_TYPE_SCRIPT | SPAWN_TYPE_SYSTEMIC
};

void spawn_liquid(ENT_TYPE entity_type, float x, float y);
void spawn_liquid(ENT_TYPE entity_type, float x, float y, float velocityx, float velocityy, uint32_t liquid_flags, uint32_t amount, float blobs_separation);
void spawn_liquid_ex(ENT_TYPE entity_type, float x, float y, float velocityx, float velocityy, uint32_t liquid_flags, uint32_t amount);
int32_t spawn_entity_abs(ENT_TYPE entity_type, float x, float y, LAYER layer, float vx, float vy);
int32_t spawn_entity_snap_to_floor(ENT_TYPE entity_type, float x, float y, LAYER layer);
int32_t spawn_entity_snap_to_grid(ENT_TYPE entity_type, float x, float y, LAYER layer);
int32_t spawn_entity_abs_nonreplaceable(ENT_TYPE entity_type, float x, float y, LAYER layer, float vx, float vy);
int32_t spawn_entity_over(ENT_TYPE entity_type, uint32_t over_uid, float x, float y);
int32_t spawn_door_abs(float x, float y, LAYER layer, uint8_t w, uint8_t l, uint8_t t);
void spawn_backdoor_abs(float x, float y);

int32_t spawn_apep(float x, float y, LAYER layer, bool right);

int32_t spawn_tree(float x, float y, LAYER layer, uint16_t height);
inline int32_t spawn_tree(float x, float y, LAYER layer)
{
    return spawn_tree(x, y, layer, 0);
}
int32_t spawn_mushroom(float x, float y, LAYER l, uint16_t height);
inline int32_t spawn_mushroom(float x, float y, LAYER l)
{
    return spawn_mushroom(x, y, l, 0);
}
int32_t spawn_unrolled_player_rope(float x, float y, LAYER layer, TEXTURE texture);
int32_t spawn_unrolled_player_rope(float x, float y, LAYER layer, TEXTURE texture, uint16_t max_length);

Entity* spawn_impostor_lake(AABB aabb, LAYER layer, ENT_TYPE impostor_type, float top_threshold);
void setup_impostor_lake(Entity* lake_imposter, AABB aabb, float top_threshold);
void fix_impostor_lake_positions();

void push_spawn_type_flags(SPAWN_TYPE flags);
void pop_spawn_type_flags(SPAWN_TYPE flags);

void init_spawn_hooks();

int32_t spawn_player(int8_t player_slot, std::optional<float> x, std::optional<float> y, std::optional<LAYER> layer);
int32_t spawn_companion(ENT_TYPE companion_type, float x, float y, LAYER layer);
int32_t spawn_shopkeeper(float x, float y, LAYER layer, ROOM_TEMPLATE room_template = 65);
int32_t spawn_roomowner(ENT_TYPE owner_type, float x, float y, LAYER layer, int16_t room_template = -1);
int32_t spawn_playerghost(ENT_TYPE char_type, float x, float y, LAYER layer);
