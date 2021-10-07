#pragma once
#include "entity.hpp"

struct Layer
{
    bool is_back_layer;
    struct
    {
        Entity** begin;
        size_t b;
        int cap;
        int size;
    } items_;
    char stuff0[0xF460]; // Just wanna get to grid_entities
    Entity* grid_entities[0x56][0x7e];
    char stuff1[0x3FD10]; // Just wanna get to expired_items_
    // List of items that were destroyed and are waiting to have the dtor called
    // and then be returned to the entity pool
    struct
    {
        Entity** begin;
        size_t b;
        int cap;
        int size;
    } expired_items_;

    Entity* spawn_entity(size_t id, float x, float y, bool screen, float vx, float vy, bool snap);

    Entity* spawn_entity_snap_to_floor(size_t id, float x, float y);

    Entity* spawn_entity_over(size_t id, Entity* overlay, float x, float y);

    Entity* spawn_door(float x, float y, uint8_t w, uint8_t l, uint8_t t);

    Entity* spawn_apep(float x, float y, bool right);

    Entity* get_grid_entity_at(float x, float y);

    std::vector<Entity*> items() const;
};
