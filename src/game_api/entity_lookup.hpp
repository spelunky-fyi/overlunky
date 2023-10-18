#pragma once

#include <cstdint>
#include <vector>

#include "aliases.hpp"
#include "math.hpp"

struct Layer;

int32_t get_grid_entity_at(float x, float y, LAYER layer);
std::vector<uint32_t> get_entities();
std::vector<uint32_t> get_entities_by(std::vector<ENT_TYPE> entity_types, uint32_t mask, LAYER layer);
std::vector<uint32_t> get_entities_by(ENT_TYPE entity_type, uint32_t mask, LAYER layer);
std::vector<uint32_t> get_entities_by_type(std::vector<ENT_TYPE> entity_types);
std::vector<uint32_t> get_entities_by_type(ENT_TYPE entity_type);
std::vector<uint32_t> get_entities_by_mask(uint32_t mask);
std::vector<uint32_t> get_entities_by_layer(LAYER layer);
std::vector<uint32_t> get_entities_at(std::vector<ENT_TYPE> entity_types, uint32_t mask, float x, float y, LAYER layer, float radius);
std::vector<uint32_t> get_entities_at(ENT_TYPE entity_type, uint32_t mask, float x, float y, LAYER layer, float radius);
std::vector<uint32_t> get_entities_overlapping_hitbox(std::vector<ENT_TYPE> entity_types, uint32_t mask, AABB hitbox, LAYER layer);
std::vector<uint32_t> get_entities_overlapping_hitbox(ENT_TYPE entity_type, uint32_t mask, AABB hitbox, LAYER layer);
std::vector<uint32_t> get_entities_overlapping(std::vector<ENT_TYPE> entity_types, uint32_t mask, float sx, float sy, float sx2, float sy2, LAYER layer);
std::vector<uint32_t> get_entities_overlapping(ENT_TYPE entity_type, uint32_t mask, float sx, float sy, float sx2, float sy2, LAYER layer);
std::vector<uint32_t> get_entities_overlapping_by_pointer(std::vector<ENT_TYPE> entity_types, uint32_t mask, float sx, float sy, float sx2, float sy2, Layer* layer);
std::vector<uint32_t> get_entities_overlapping_by_pointer(ENT_TYPE entity_type, uint32_t mask, float sx, float sy, float sx2, float sy2, Layer* layer);
bool entity_has_item_uid(uint32_t uid, uint32_t item_uid);
bool entity_has_item_type(uint32_t uid, std::vector<ENT_TYPE> entity_types);
bool entity_has_item_type(uint32_t uid, ENT_TYPE entity_type);
std::vector<uint32_t> entity_get_items_by(uint32_t uid, std::vector<ENT_TYPE> entity_types, uint32_t mask);
std::vector<uint32_t> entity_get_items_by(uint32_t uid, ENT_TYPE entity_type, uint32_t mask);
std::vector<uint32_t> get_entities_by_draw_depth(uint8_t draw_depth, LAYER l);
std::vector<uint32_t> get_entities_by_draw_depth(std::vector<uint8_t> draw_depths, LAYER l);
std::vector<ENT_TYPE> get_proper_types(std::vector<ENT_TYPE> ent_types);
