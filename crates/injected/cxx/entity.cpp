#include "entity.hpp"
#include "injected-dll/src/db.rs.h"

const int NAME_TO_INDEX = 0x381c0;

struct EntityInstance;

struct Rect
{
    int32_t masks;
    float up_minus_down;
    float side;
    float up_plus_down;
    char field_10;
    char field_11;
    __int16 field_12;
};

enum RepeatType: uint8_t {
    NoRepeat,
    Linear,
    BackAndForth,
};

struct Animation {
    int32_t texture;
    int32_t count;
    int32_t interval;
    uint8_t key;
    RepeatType repeat;
};

struct Entity
{
    // Creates an instance of this entity
    EntityInstance *(*create_func)();
    void (*destroy_func)(Entity *);
    int field_10;
    // Entity id (ENT_...)
    int32_t id;
    int field_18;
    float width;
    float height;
    char field_24;
    char field_25;
    int16_t field_26;
    Rect rectCollision;
    int field_3C;
    int field_40;
    int field_44;
    int64_t field_48;
    int field_50;
    float friction;
    float elasticity;
    float weight;
    char field_60;
    float acceleration;
    float maxSpeed;
    float sprintFactor;
    float jump;
    float field_74[4];
    int texture;
    int technique;
    int tileX;
    int tileY;
    char damage;
    char life;
    char field_96;
    char field_97;
    char field_98;
    int description;
    int field_A0;
    int field_A4;
    float field_A8;
    int field_AC;
    std::unordered_map<uint8_t, Animation> animations;
    float attachOffsetX;
    float attachOffsetY;
    char init;

    void verify_size()
    {
        static_assert(sizeof(Entity) == 0x100, "Check size of Entity!");
    }
};

#include <cstdio>
rust::Vec<EntityItem>
list_entities(size_t map_ptr)
{
    if (!map_ptr)
        return {};

    using EntityMap = std::unordered_map<std::string, uint16_t>;
    auto map = reinterpret_cast<EntityMap *>(map_ptr + NAME_TO_INDEX);
    auto entities = reinterpret_cast<Entity *>(map_ptr);

    rust::Vec<EntityItem> result;
    for (const auto &kv : *map)
    {
        result.push_back(EntityItem{kv.first, kv.second});
        // Entity *entity = &entities[kv.second];
        // printf("%d %d %d\n", entity->id, (entity->map)[0], (entity->map)[1]);
    }

    return result;
}
