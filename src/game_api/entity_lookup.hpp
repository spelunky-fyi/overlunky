#pragma once

#include <cstdint>
#include <vector>

#include "aliases.hpp"
#include "math.hpp"

struct Layer;

int32_t get_grid_entity_at(float x, float y, LAYER layer);

std::vector<uint32_t> get_entities_overlapping_grid(float x, float y, LAYER layer);

std::vector<uint32_t> get_entities_by(std::vector<ENT_TYPE> entity_types, uint32_t mask, LAYER layer);

inline std::vector<uint32_t> get_entities()
{
    return get_entities_by({}, 0, LAYER::BOTH);
}
inline std::vector<uint32_t> get_entities_by(ENT_TYPE entity_type, uint32_t mask, LAYER layer)
{
    return get_entities_by(std::vector<ENT_TYPE>{entity_type}, mask, layer);
}
inline std::vector<uint32_t> get_entities_by_type(std::vector<ENT_TYPE> entity_types)
{
    return get_entities_by(std::move(entity_types), 0, LAYER::BOTH);
}
inline std::vector<uint32_t> get_entities_by_type(ENT_TYPE entity_type)
{
    return get_entities_by(std::vector<ENT_TYPE>{entity_type}, 0, LAYER::BOTH);
}
inline std::vector<uint32_t> get_entities_by_mask(uint32_t mask)
{
    return get_entities_by({}, mask, LAYER::BOTH);
}
inline std::vector<uint32_t> get_entities_by_layer(LAYER layer)
{
    return get_entities_by({}, 0, layer);
}
std::vector<uint32_t> get_entities_at(std::vector<ENT_TYPE> entity_types, uint32_t mask, float x, float y, LAYER layer, float radius);
inline std::vector<uint32_t> get_entities_at(ENT_TYPE entity_type, uint32_t mask, float x, float y, LAYER layer, float radius)
{
    return get_entities_at(std::vector<ENT_TYPE>{entity_type}, mask, x, y, layer, radius);
}
std::vector<uint32_t> get_entities_overlapping_hitbox(std::vector<ENT_TYPE> entity_types, uint32_t mask, AABB hitbox, LAYER layer);
inline std::vector<uint32_t> get_entities_overlapping_hitbox(ENT_TYPE entity_type, uint32_t mask, AABB hitbox, LAYER layer)
{
    return get_entities_overlapping_hitbox(std::vector<ENT_TYPE>{entity_type}, mask, hitbox, layer);
}
inline std::vector<uint32_t> get_entities_overlapping(std::vector<ENT_TYPE> entity_types, uint32_t mask, float sx, float sy, float sx2, float sy2, LAYER layer)
{
    return get_entities_overlapping_hitbox(std::move(entity_types), mask, {sx, sy2, sx2, sy}, layer);
}
inline std::vector<uint32_t> get_entities_overlapping(ENT_TYPE entity_type, uint32_t mask, float sx, float sy, float sx2, float sy2, LAYER layer)
{
    return get_entities_overlapping_hitbox(std::vector<ENT_TYPE>{entity_type}, mask, {sx, sy2, sx2, sy}, layer);
}
std::vector<uint32_t> get_entities_overlapping_by_pointer(std::vector<ENT_TYPE> entity_types, uint32_t mask, float sx, float sy, float sx2, float sy2, Layer* layer);
inline std::vector<uint32_t> get_entities_overlapping_by_pointer(ENT_TYPE entity_type, uint32_t mask, float sx, float sy, float sx2, float sy2, Layer* layer)
{
    return get_entities_overlapping_by_pointer(std::vector<ENT_TYPE>{entity_type}, mask, sx, sy, sx2, sy2, layer);
}
bool entity_has_item_uid(uint32_t uid, uint32_t item_uid);
bool entity_has_item_type(uint32_t uid, std::vector<ENT_TYPE> entity_types);
inline bool entity_has_item_type(uint32_t uid, ENT_TYPE entity_type)
{
    return entity_has_item_type(uid, std::vector<ENT_TYPE>{entity_type});
}
std::vector<uint32_t> entity_get_items_by(uint32_t uid, std::vector<ENT_TYPE> entity_types, uint32_t mask);
inline std::vector<uint32_t> entity_get_items_by(uint32_t uid, ENT_TYPE entity_type, uint32_t mask)
{
    return entity_get_items_by(uid, std::vector<ENT_TYPE>{entity_type}, mask);
}
std::vector<uint32_t> get_entities_by_draw_depth(std::vector<uint8_t> draw_depths, LAYER l);
inline std::vector<uint32_t> get_entities_by_draw_depth(uint8_t draw_depth, LAYER l)
{
    return get_entities_by_draw_depth(std::vector<uint8_t>{draw_depth}, l);
}
std::vector<ENT_TYPE> get_proper_types(std::vector<ENT_TYPE> ent_types);
