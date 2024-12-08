#pragma once

#include <array>
#include <cstddef>    // for size_t
#include <cstdint>    // for uint32_t, int32_t, uint64_t, uint8_t
#include <functional> // for less
#include <map>        // for map
#include <new>        // for operator new
#include <set>        //
#include <utility>    // for find, pair
#include <vector>     // for allocator, vector

#include "aliases.hpp" // for ENT_TYPE

class Entity;

inline constexpr uint32_t g_level_max_x = 0x56;
inline constexpr uint32_t g_level_max_y = 0x7e;

// Is always ordered by the uid
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

    bool empty() const
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

    void insert(Entity*, bool);

    std::pair<Entity*&, uint32_t&> operator[](const uint32_t idx) const
    {
        return {ent_list[idx], uid_list[idx]};
    }
};

struct EntityRegions
{
    EntityList** entity_lists;
    // pointers to entities from entities_by_region array

    uint8_t size;
    uint8_t cap;
};

struct Layer
{
    bool is_back_layer;
    // int8_t padding[7];

    EntityList all_entities;
    // char + fx + mons + item + logical + mount + activefloor + BG (excluding BG_SHOP, BG_LEVEL_*)
    EntityList unknown_entities1;
    EntityRegions* unknown1; // players in motion?

    std::map<uint32_t, EntityList> entities_by_mask; // key is the mask

    // 4x4 block areas (the edge ones extend to infinity?), each probably contains different mask entities
    EntityList entities_by_region1[31][21];
    EntityList entities_by_region2[31][21]; // Active floors ?
    EntityList entities_by_region3[31][21];
    EntityList entities_by_region4[31][21];

    std::map<int32_t, EntityRegions> entity_regions; // key is uid, all entities except FX, FLOOR, DECORATION, BG, SHADOW and LOGICAL

    Entity* grid_entities[g_level_max_y][g_level_max_x];
    EntityList entities_overlapping_grid[g_level_max_y][g_level_max_x]; // static entities (like midbg, decorations) that overlap this grid position

    EntityList unknown_entities2;
    std::array<EntityList, 53> entities_by_draw_depth;
    EntityList unknown_entities2a;
    EntityList unknown_entities3;        // debris, explosions, laserbeams etc. expired_entities ?
    EntityList unknown_entities4;        // explosions, laserbeams, BG_LEVEL_*_SOOT ? only for short time while there are spawned?
    std::vector<Entity*> unknown_vector; // add_to_layer uses this
    std::set<float>* unknown6;           // triggered by floor entity destruction? needs more testing
    EntityList unknown_entities5;
    bool is_layer_loading;
    bool unknown14;

    // probably just padding
    // uint8_t unknown15;
    // uint8_t unknown16;
    // uint32_t unknown17;

    Entity* spawn_entity(ENT_TYPE id, float x, float y, bool screen, float vx, float vy, bool snap);
    Entity* spawn_entity_snap_to_floor(ENT_TYPE id, float x, float y);
    Entity* spawn_entity_over(ENT_TYPE id, Entity* overlay, float x, float y);
    Entity* spawn_door(float x, float y, uint8_t w, uint8_t l, uint8_t t);
    Entity* spawn_apep(float x, float y, bool right);

    Entity* get_grid_entity_at(float x, float y) const;

    EntityList* get_entities_overlapping_grid_at(float x, float y) const;

    Entity* get_entity_at(float x, float y, uint32_t search_flags, uint32_t include_flags, uint32_t exclude_flags, uint32_t one_of_flags);

    void move_grid_entity(Entity* ent, float x, float y, Layer* dest_layer);
    void move_grid_entity(Entity* ent, uint32_t x, uint32_t y, Layer* dest_layer);

    void destroy_grid_entity(Entity* ent);
};
