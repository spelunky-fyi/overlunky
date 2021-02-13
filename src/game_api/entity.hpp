#pragma once

#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include "memory.hpp"

enum RepeatType : uint8_t
{
    NoRepeat,
    Linear,
    BackAndForth,
};

struct Animation
{
    int32_t texture;
    int32_t count;
    int32_t interval;
    uint8_t key;
    RepeatType repeat;
};

struct Rect
{
    int masks;
    float up_minus_down, side, up_plus_down;
    uint8_t field_10;
    uint8_t field_11;
    uint16_t field_12;
};

struct Color
{
    float r;
    float g;
    float b;
    float a;
};

class Entity;
// Creates an instance of this entity
using EntityCreate = Entity *(*)();
using EntityDestroy = void (*)(Entity *);
using AnimationMap = std::unordered_map<uint8_t, Animation>;

struct EntityDB
{
    EntityCreate create_func;
    EntityDestroy destroy_func;
    int32_t field_10;
    /* Entity id (ENT_...) */
    int32_t id;
    uint32_t search_flags;
    float width;
    float height;
    uint8_t field_24;
    uint8_t field_25;
    int16_t field_26;
    Rect rect_collision;
    int32_t field_3C;
    int32_t field_40;
    int32_t field_44;
    int64_t field_48;
    int32_t field_50;
    float friction;
    float elasticity;
    float weight;
    uint8_t field_60;
    float acceleration;
    float max_speed;
    float sprint_factor;
    float jump;

    /* ??? */
    float _a;
    float _b;
    float _c;
    float _d;

    int32_t texture;
    int32_t technique;
    int32_t tile_x;
    int32_t tile_y;
    uint8_t damage;
    uint8_t life;
    uint8_t field_96;
    uint8_t field_97;
    uint8_t field_98;
    int32_t description;
    int32_t field_a0;
    int32_t field_a4;
    float field_a8;
    int32_t field_AC;
    AnimationMap animations;
    float attachOffsetX;
    float attachOffsetY;
    uint8_t init;
};

struct EntityItem
{
    std::string name;
    uint16_t id;

    EntityItem(const std::string &name_, uint64_t id_) : name(name_), id(id_)
    {
    }
    bool operator<(const EntityItem &item) const
    {
        return id < item.id;
    }
};

std::vector<EntityItem> list_entities(size_t map_ptr);

EntityDB *get_type(uint32_t id);

size_t to_id(size_t map_ptr, std::string id);

Entity *state_find_item(size_t state_ptr, uint32_t unique_id);

template <typename T> std::string to_le_bytes(T fmt)
{
    return std::string((char *)&fmt, sizeof(T));
}

class Entity
{
  public:
    EntityDB *type;
    Entity *overlay;
    size_t some_items_func;
    size_t items_ptr;
    uint32_t items_size;
    uint32_t items_count;
    uint32_t flags;
    uint32_t more_flags;
    uint32_t uid;
    uint32_t animation;
    float x;
    float y;
    float w;
    float h;
    float f50;
    float f54;
    Color color;
    float offsetx;
    float offsety;
    float hitboxx;
    float hitboxy;
    uint32_t duckmask;
    int32_t i7c;
    size_t p80;
    size_t texture;
    float tilew;
    float tileh;
    uint8_t camera_layer;
    uint8_t b99;
    uint8_t b9a;
    uint8_t b9b;
    uint32_t i9c;

    size_t pointer()
    {
        return (size_t)this;
    }

    std::pair<float, float> position();

    void teleport(float dx, float dy, bool s, float vx, float vy, bool snap);
    void teleport_abs(float dx, float dy, float vx, float vy);

    Entity *topmost()
    {
        auto cur = this;
        while (cur->overlay)
        {
            cur = cur->overlay;
        }
        return cur;
    }

    Entity *topmost_mount()
    {
        auto topmost = this;
        while (auto cur = topmost->overlay)
        {
            if (cur->type->search_flags < 0x40)
            {
                topmost = cur;
            }
            else
                break;
        }
        return topmost;
    }

    uint8_t layer()
    {
        return read_u8(pointer() + 0x98);
    }
    std::pair<float, float> position_self() const;
    void remove_item(uint32_t id);
    void destroy();

    virtual ~Entity() = default;
    virtual void v0() = 0;
    virtual void v1() = 0;
    virtual void v2(Entity *) = 0;
    virtual void v3() = 0;
    virtual void v4(void *) = 0;
    virtual void hiredhand_description(char *) = 0;
    virtual void v6() = 0;
    virtual void v7() = 0;
    virtual void v8() = 0;
    virtual void v9() = 0;
    virtual void v10() = 0;
    virtual void v11() = 0;
    virtual void v12() = 0;
    virtual void v13() = 0;
    virtual void v14() = 0;
    virtual void v15() = 0;
    virtual void v16() = 0;
    virtual void v17() = 0;
    virtual void v18() = 0;
    virtual void v19() = 0;
    virtual void v20() = 0;
    virtual void v21() = 0;
    virtual void v22() = 0;
    virtual void v23() = 0;
    virtual void v24() = 0;
    virtual void v25() = 0;
    virtual void v26() = 0;
    virtual void v27() = 0;
    virtual void v28() = 0;
    virtual void v29() = 0;
    virtual void v30() = 0;
    virtual void v31() = 0;
    virtual void v32() = 0;
    virtual void v33() = 0;
    virtual void v34() = 0;
};

struct Inventory
{
    uint32_t money;
    uint8_t bombs;
    uint8_t ropes;
    uint8_t b06;
    uint8_t b07;
    uint8_t pad08[0x141c]; // specific treasure and killed monsters here, boring
    uint32_t kills_level;
    uint32_t kills_total;
};

class Movable : public Entity
{
  public:
    std::map<int64_t, int64_t> pa0;
    std::map<int, int> pb0;
    size_t anim_func;
    int32_t ic8;
    int32_t icc;
    float movex;
    float movey;
    uint32_t buttons;
    uint32_t stand_counter;
    float fe0;
    int32_t ie4;
    uint32_t owner_uid;
    uint32_t last_owner_uid;
    size_t animation_func;
    uint32_t idle_counter;
    uint32_t standing_on_uid;
    float velocityx;
    float velocityy;
    uint32_t holding_uid;
    uint8_t state;
    uint8_t last_state;
    uint8_t move_state;
    uint8_t health;
    int32_t i110;
    uint32_t some_state;
    int32_t i118;
    int32_t i11c;
    int32_t i120;
    uint8_t b124;
    uint8_t airtime;
    uint8_t b126;
    uint8_t b127;
};

class Monster : public Movable
{
  public:
    std::map<int64_t, int64_t> inside;
};

class Player : public Monster
{
  public:
    Inventory *inventory_ptr;
    size_t p140;
    int32_t i148;
    int32_t i14c;
    size_t i150;
    size_t p158;
    size_t p160;
    int32_t i168;
    int32_t i16c;
    uint32_t jump_flags;
    uint8_t some_timer;
    uint8_t can_use;
    uint8_t b176;
    uint8_t b177;
};

class Container : public Movable
{
  public:
    int32_t inside;
    int32_t timer;
};

class Mattock : public Movable
{
  public:
    int32_t remaining;
};

class Mount : public Monster
{
  public:
    void carry(Movable *rider);

    void tame(bool value);
};

using Carry = void (*)(Entity *, Entity *);

Carry get_carry();

struct Target
{
    uint8_t b00;
    uint8_t enabled;
    uint8_t level;
    uint8_t from;
    uint8_t world;
    uint8_t theme;
};

class Door : public Entity
{
  public:
    void set_target(uint8_t w, uint8_t l, uint8_t t);
};

std::vector<EntityItem> list_entities();
