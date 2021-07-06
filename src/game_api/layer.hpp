#pragma once
#include "entity.hpp"

struct Layer
{
    size_t __vftable;
    struct
    {
        Entity** begin;
        size_t b;
        int cap;
        int size;
    } items_;

    Entity* spawn_entity(size_t id, float x, float y, bool screen, float vx, float vy, bool snap);

    Entity* spawn_entity_snap_to_floor(size_t id, float x, float y);

    Entity* spawn_entity_over(size_t id, Entity* overlay, float x, float y);

    Entity* spawn_door(float x, float y, uint8_t w, uint8_t l, uint8_t t);

    Entity* spawn_apep(float x, float y, bool right);

    Entity* get_grid_entity_at(float x, float y);

    std::vector<Entity*> items() const;
};
