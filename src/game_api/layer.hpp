#pragma once

class Entity;

struct EntityList
{
    Entity** ent_list;
    uint32_t* uids;
    uint32_t cap;
    uint32_t size;

    constexpr bool empty()
    {
        return !!size;
    };

    constexpr Entity** begin()
    {
        return ent_list;
    };
    constexpr Entity** end()
    {
        return ent_list + size;
    };
    constexpr Entity* at(uint32_t idx)
    {
        return ent_list[idx];
    };
    constexpr Entity* front()
    {
        return *ent_list;
    };
    constexpr Entity* back()
    {
        return ent_list[size - 1];
    };

    constexpr uint32_t* uid_begin()
    {
        return uids;
    };
    constexpr uint32_t* uid_end()
    {
        return uids + size;
    };

    constexpr bool contains(Entity* ent)
    {
        return (std::find(begin(), end(), ent) != end());
    };
    constexpr bool contains(uint32_t uid)
    {
        return (std::find(uid_begin(), uid_end(), uid) != uid_end());
    };

    std::pair<Entity*, uint32_t> operator[](const uint32_t idx) const
    {
        return std::make_pair(ent_list[idx], uids[idx]);
    };
};

struct Layer
{
    bool is_back_layer;
    // int8_t padding[7];
    EntityList all_entities;
    //char + fx + mons + item + logical + mount + activefloor + BG (excluding BG_SHOP, BG_LEVEL_*)
    EntityList unknown_entities1;
    size_t unknown1;
    std::map<int, int> unknown2;

    char stuff0[0xF430]; // Just wanna get to grid_entities

    Entity* grid_entities[0x7e][0x56];

    char stuff1[0x3FD08]; // Just wanna get to expired_items_

    EntityList unknown_entities2; // debris, explosions, laserbeams etc. ?
    EntityList unknown_entities3; // explosions, laserbeams, BG_LEVEL_*_SOOT ? only for short time while there are spawned?
    size_t unknown3;
    size_t unknown4;
    size_t unknown5;
    size_t unknown6;
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
    size_t entity_items_begin; // begin of the memory that holds the items of entities
    size_t unknown21;
    size_t unknown22;
    bool unknown23;
    bool layer_freeze; //locking mechanism?
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
    size_t unknown35;
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
    uint64_t unknown46; //next layer below

    Entity* spawn_entity(size_t id, float x, float y, bool screen, float vx, float vy, bool snap);

    Entity* spawn_entity_snap_to_floor(size_t id, float x, float y);

    Entity* spawn_entity_over(size_t id, Entity* overlay, float x, float y);

    Entity* spawn_door(float x, float y, uint8_t w, uint8_t l, uint8_t t);

    Entity* spawn_apep(float x, float y, bool right);

    Entity* get_grid_entity_at(float x, float y);
};
