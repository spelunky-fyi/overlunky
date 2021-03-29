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

int32_t to_id(std::string id);

Entity *state_find_item(size_t state_ptr, uint32_t unique_id);

template <typename T> std::string to_le_bytes(T fmt)
{
    return std::string((char *)&fmt, sizeof(T));
}

class Vector
{
  public:
    uint32_t *heap;
    uint32_t *begin;
    uint32_t size, count;

    bool empty()
    {
        return !!size;
    }
};

class Entity
{
  public:
    EntityDB *type;
    Entity *overlay;
    Vector items;
    uint32_t flags;
    uint32_t more_flags;
    int32_t uid;
    uint8_t animation_frame;
    uint8_t b3d;
    uint8_t b3e;
    uint8_t b3f;
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
    float angle;
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

    template <typename T> T *as()
    {
        return static_cast<T *>(this);
    }

    virtual ~Entity() = 0;
    virtual void created() = 0;
    virtual void kill(bool, Entity *frm) = 0;
    virtual void v2(Entity *) = 0;
    virtual void v3() = 0;
    virtual void v4(void *) = 0;
    virtual void hiredhand_description(char *) = 0;
    virtual void v6() = 0;
    virtual void v7() = 0;
    virtual void v8() = 0;
    virtual void v9() = 0;
    virtual bool v10() = 0;
    virtual void v11() = 0;
    virtual void v12() = 0;
    virtual void set_invisible(bool) = 0;
    virtual void v14(bool) = 0;
    virtual void v15(uint8_t a, uint8_t b) = 0;
    virtual void v16() = 0;
    virtual float friction() = 0;
    virtual void v18() = 0;
    virtual void remove_item_ptr(Entity *) = 0;
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
    virtual void apply_db() = 0;
    virtual void v35() = 0;
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

#include "movable.hpp"

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
    std::tuple<uint8_t, uint8_t, uint8_t> get_target();
};

class Arrowtrap : public Entity
{
  public:
    size_t unknown1;
    size_t unknown2;
    bool arrow_shot;
    
    void rearm();
};

std::vector<EntityItem> list_entities();
