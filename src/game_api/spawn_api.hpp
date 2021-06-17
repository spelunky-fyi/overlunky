#pragma once

#include <cstdint>

using SpawnTypeFlags = int;
enum SpawnType : SpawnTypeFlags
{
    SPAWN_TYPE_LEVEL_GEN = 1 << 0,
    SPAWN_TYPE_SCRIPT = 1 << 1,
    SPAWN_TYPE_SYSTEMIC = 1 << 2,
    SPAWN_TYPE_NUM_FLAGS = 3,
    SPAWN_TYPE_ANY = SPAWN_TYPE_LEVEL_GEN | SPAWN_TYPE_SCRIPT | SPAWN_TYPE_SYSTEMIC
};

void spawn_liquid(uint32_t entity_type, float x, float y);
int32_t spawn_entity(uint32_t entity_type, float x, float y, bool s, float vx, float vy, bool snap);
int32_t spawn_entity_abs(uint32_t entity_type, float x, float y, int layer, float vx, float vy);
int32_t spawn_entity_abs_nonreplaceable(uint32_t entity_type, float x, float y, int layer, float vx, float vy);
int32_t spawn_entity_over(uint32_t item_uid, uint32_t over_uid, float x, float y);
int32_t spawn_door(float x, float y, uint8_t w, uint8_t l, uint8_t t);
int32_t spawn_door_abs(float x, float y, int layer, uint8_t w, uint8_t l, uint8_t t);
void spawn_backdoor(float x, float y);
void spawn_backdoor_abs(float x, float y);

void push_spawn_type_flags(SpawnTypeFlags flags);
void pop_spawn_type_flags(SpawnTypeFlags flags);

void init_spawn_hooks();
