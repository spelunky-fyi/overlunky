#pragma once

#include "aliases.hpp"
#include "math.hpp"

#include <cstdint>

class Entity;

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
int32_t spawn_entity(ENT_TYPE entity_type, float x, float y, bool s, float vx, float vy, bool snap);
int32_t spawn_entity_abs(ENT_TYPE entity_type, float x, float y, LAYER layer, float vx, float vy);
int32_t spawn_entity_snap_to_floor(ENT_TYPE entity_type, float x, float y, LAYER layer);
int32_t spawn_entity_snap_to_grid(ENT_TYPE entity_type, float x, float y, LAYER layer);
int32_t spawn_entity_abs_nonreplaceable(ENT_TYPE entity_type, float x, float y, LAYER layer, float vx, float vy);
int32_t spawn_entity_over(ENT_TYPE entity_type, uint32_t over_uid, float x, float y);
int32_t spawn_door(float x, float y, uint8_t w, uint8_t l, uint8_t t);
int32_t spawn_door_abs(float x, float y, LAYER layer, uint8_t w, uint8_t l, uint8_t t);
void spawn_backdoor(float x, float y);
void spawn_backdoor_abs(float x, float y);

int32_t spawn_apep(float x, float y, LAYER layer, bool right);

void spawn_tree(float x, float y, LAYER layer);

Entity* spawn_impostor_lake(AABB aabb, LAYER layer, ENT_TYPE impostor_type, float top_threshold);
void setup_impostor_lake(Entity* lake_imposter, AABB aabb, float top_threshold);

void push_spawn_type_flags(SPAWN_TYPE flags);
void pop_spawn_type_flags(SPAWN_TYPE flags);

void init_spawn_hooks();

void spawn_player(int8_t player_slot, float x, float y);
int32_t spawn_companion(ENT_TYPE companion_type, float x, float y, LAYER layer);
int32_t spawn_shopkeeper(float x, float y, LAYER layer, ROOM_TEMPLATE room_template = 65);
int32_t spawn_roomowner(ENT_TYPE owner_type, float x, float y, LAYER layer, int16_t room_template = -1);
