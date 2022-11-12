#pragma once

#include <cstddef>    // for size_t
#include <cstdint>    // for uint32_t, int32_t, uint64_t, uint8_t
#include <functional> // for less
#include <map>        // for map
#include <new>        // for operator new
#include <utility>    // for find, pair
#include <vector>     // for allocator, vector

#include "aliases.hpp" // for ENT_TYPE

class Entity;

inline constexpr uint32_t g_level_max_x = 0x56;
inline constexpr uint32_t g_level_max_y = 0x7e;

struct EntityList
{
    Entity** ent_list;
    uint32_t* uid_list;
    uint32_t cap;
    uint32_t size;

    template <class T>
    struct Range
    {
        T* values;
        uint32_t size;

        T* begin()
        {
            return values;
        }
        T* end()
        {
            return values + size;
        }
        T* begin() const
        {
            return values;
        }
        T* end() const
        {
            return values + size;
        }
        T* cbegin() const
        {
            return begin();
        }
        T* cend() const
        {
            return end();
        }

        T& front()
        {
            return *values;
        }
        T& back()
        {
            return values[size - 1];
        }
        const T& front() const
        {
            return *values;
        }
        const T& back() const
        {
            return values[size - 1];
        }

        bool contains(const T& val) const
        {
            return std::find(begin(), end(), val) != end();
        }
    };
    using EntityRange = Range<Entity*>;
    using ConstEntityRange = Range<Entity* const>;
    using UidRange = Range<uint32_t>;
    using ConstUidRange = Range<const uint32_t>;

    bool empty()
    {
        return size == 0;
    }

    EntityRange entities()
    {
        return {ent_list, size};
    }
    ConstEntityRange entities() const
    {
        return {ent_list, size};
    }
    UidRange uids()
    {
        return {uid_list, size};
    }
    ConstUidRange uids() const
    {
        return {uid_list, size};
    }

    bool contains(Entity* ent) const
    {
        return entities().contains(ent);
    }
    bool contains(uint32_t uid) const
    {
        return uids().contains(uid);
    }

    std::pair<Entity*&, uint32_t&> operator[](const uint32_t idx) const
    {
        return {ent_list[idx], uid_list[idx]};
    }
};

struct Layer
{
    bool is_back_layer;
    // int8_t padding[7];

    EntityList all_entities;
    // char + fx + mons + item + logical + mount + activefloor + BG (excluding BG_SHOP, BG_LEVEL_*)
    EntityList unknown_entities1;
    size_t unknown1;
    // key is the mask
    std::map<uint32_t, EntityList> entities_by_mask;

    EntityList entities_by_unknown[647]; // could be more, not sure what for, each holds like 1 entity for split second
    char stuff0[0xB778];                 // unknown, maybe more of the array above?

    std::map<int32_t, size_t> unknown_map; // some movable and liquids and something else maybe?, key is uid

    Entity* grid_entities[g_level_max_y][g_level_max_x];
    EntityList entities_overlaping_grid[g_level_max_y][g_level_max_x]; // static entities (like midbg, decorations) that overlap this grid position

    EntityList unknown_entities2;
    EntityList entities_by_draw_depth[53];
    EntityList unknown_entities3;        // debris, explosions, laserbeams etc. ?
    EntityList unknown_entities4;        // explosions, laserbeams, BG_LEVEL_*_SOOT ? only for short time while there are spawned?
    std::vector<Entity*> unknown_vector; // add_to_layer uses this
    size_t unknown6;                     // MysteryLayerPointer1 in plugin
    // List of items that were destroyed and are waiting to have the dtor called
    // and then be returned to the entity pool
    EntityList expired_entities;
    bool is_layer_loading;
    bool unknown14;
    uint8_t unknown15;
    uint8_t unknown16;
    uint32_t unknown17;
    uint32_t unknown18;
    uint32_t unknown19;
    size_t entity_items_begin; // begin of the memory that holds the items of entities, maybe vector?
    size_t unknown21;
    size_t unknown22;
    bool unknown23;
    bool layer_freeze; // locking mechanism?
    uint8_t unknown25;
    uint8_t unknown26;
    uint32_t unknown27;
    uint64_t unknown28;
    uint64_t unknown29;
    uint64_t unknown30;
    uint64_t unknown31;
    uint64_t unknown32;
    uint32_t unknown33;
    uint32_t unknown34;
    size_t unknown35; // maybe vector?
    size_t unknown36;
    size_t unknown37;
    bool unknown38;
    bool unknown39;
    uint8_t unknown40;
    uint8_t unknown41;
    uint32_t unknown42;
    uint64_t unknown43;
    uint64_t unknown44;
    uint64_t unknown45;
    uint64_t unknown46; // next layer below

    Entity* spawn_entity(ENT_TYPE id, float x, float y, bool screen, float vx, float vy, bool snap);

    Entity* spawn_entity_snap_to_floor(ENT_TYPE id, float x, float y);

    Entity* spawn_entity_over(ENT_TYPE id, Entity* overlay, float x, float y);

    Entity* spawn_door(float x, float y, uint8_t w, uint8_t l, uint8_t t);

    Entity* spawn_apep(float x, float y, bool right);

    Entity* get_grid_entity_at(float x, float y);

    Entity* get_entity_at(float x, float y, uint32_t search_flags, uint32_t include_flags, uint32_t exclude_flags, uint32_t one_of_flags);

    void move_grid_entity(Entity* ent, float x, float y, Layer* dest_layer);
    void move_grid_entity(Entity* ent, uint32_t x, uint32_t y, Layer* dest_layer);
};
