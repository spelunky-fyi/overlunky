#pragma once

#include <cstdint>

void spawn_liquid(uint32_t entity_type, float x, float y);
int32_t spawn_entity(uint32_t entity_type, float x, float y, bool s, float vx, float vy, bool snap);
int32_t spawn_entity_abs(uint32_t entity_type, float x, float y, int layer, float vx, float vy);
int32_t spawn_entity_over(uint32_t item_uid, uint32_t over_uid, float x, float y);
int32_t spawn_door(float x, float y, uint8_t w, uint8_t l, uint8_t t);
int32_t spawn_door_abs(float x, float y, int layer, uint8_t w, uint8_t l, uint8_t t);
void spawn_backdoor(float x, float y);
void spawn_backdoor_abs(float x, float y);

void init_spawn_hooks();
