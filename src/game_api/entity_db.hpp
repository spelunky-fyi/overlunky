#pragma once

#include <cstddef>       // for size_t
#include <cstdint>       // for uint8_t, uint32_t, int32_t, uint16_t, int64_t
#include <functional>    // for function, equal_to
#include <span>          // for span
#include <string>        // for allocator, string
#include <string_view>   // for string_view
#include <tuple>         // for tuple
#include <type_traits>   // for move
#include <unordered_map> // for _Umap_traits<>::allocator_type, unordered_map
#include <utility>       // for pair
#include <vector>        // for vector

#include "aliases.hpp"                       // for ENT_TYPE, LAYER, TEXTURE, STRINGID
#include "color.hpp"                         // for Color
#include "containers/game_unordered_map.hpp" // for game_unordered_map
#include "containers/identity_hasher.hpp"    // for identity_hasher
#include "entity_structs.hpp"                // for CollisionInfo
#include "layer.hpp"                         // for EntityList
#include "math.hpp"                          // for AABB
#include "thread_utils.hpp"                  // for OnHeapPointer

struct RenderInfo;
struct Texture;

// Creates an instance of this entity
using EntityCreate = Entity* (*)();
using EntityDestroy = void (*)(Entity*);

struct EntityDB
{
    EntityCreate create_func;
    EntityDestroy destroy_func;
    int32_t field_10;
    ENT_TYPE id;
    /// MASK
    uint32_t search_flags;
    float width;
    float height;
    uint8_t draw_depth;
    uint8_t default_b3f; // value gets copied into entity.b3f along with draw_depth etc (RVA 0x21F30CC4)
    int16_t field_26;
    union
    {
        struct
        {
            float default_offsetx;
            float default_offsety;
            float default_hitboxx;
            float default_hitboxy;
            SHAPE default_shape;
            bool default_hitbox_enabled;
            uint8_t default_b82;
            uint8_t default_b83;
        };
        CollisionInfo default_collision_info;
    };
    /// MASK, will only call collision2 when colliding with entities that match this mask.
    int32_t collision2_mask;
    /// MASK used for collision with floors.
    int32_t collision_mask;
    int32_t field_44;
    int32_t default_flags;
    int32_t default_more_flags;
    int32_t properties_flags;
    float friction;
    float elasticity;
    float weight;
    uint8_t field_60;
    float acceleration;
    float max_speed;
    float sprint_factor;
    float jump;
    union
    {
        Color default_color;
        struct
        {
            float glow_red;
            float glow_green;
            float glow_blue;
            float glow_alpha;
        };
    };
    int32_t texture_id;
    int32_t technique;
    int32_t tile_x;
    int32_t tile_y;
    uint8_t damage;
    uint8_t life;
    /// Favor for sacrificing alive. Halved when dead (health == 0).
    uint8_t sacrifice_value;
    uint8_t blood_content;
    bool leaves_corpse_behind;
    uint8_t field_99;
    uint8_t field_9A;
    uint8_t field_9B;
    STRINGID description;
    int32_t sound_killed_by_player;
    int32_t sound_killed_by_other;
    float field_a8;
    int32_t field_AC;
    game_unordered_map<uint8_t, Animation, identity_hasher<>> animations;
    float default_special_offsetx;
    float default_special_offsety;
    uint8_t init;

    EntityDB(const EntityDB& other);
    EntityDB(const ENT_TYPE other);
};

using EntityMap = std::unordered_map<std::string, uint16_t>;

struct EntityItem
{
    std::string name;
    uint32_t id;

    EntityItem(const std::string& name_, uint32_t id_)
        : name(name_), id(id_)
    {
    }
    bool operator<(const EntityItem& item) const
    {
        return id < item.id;
    }
};

struct EntityBucket
{
    void** begin;
    void** current; // Note, counts down from end to begin instead of up from begin to end :shrug:
    void** end;
};
struct EntityPool
{
    std::uint32_t slot_size;
    std::uint32_t initial_slots;
    std::uint32_t slots_growth;
    std::uint32_t current_slots;
    std::uint64_t _ulong_0;
    EntityBucket* _some_bucket;
    EntityBucket* bucket;
};
struct EntityFactory
{
    EntityDB types[0x395];
    bool type_set[0x395];
    std::unordered_map<std::uint32_t, OnHeapPointer<EntityPool>> entity_instance_map;
    EntityMap entity_map;
    void* _ptr_7;
};

EntityDB* get_type(uint32_t id);

ENT_TYPE to_id(std::string_view id);

std::string_view to_name(ENT_TYPE id);

std::vector<EntityItem> list_entities();

struct EntityFactory* entity_factory();
