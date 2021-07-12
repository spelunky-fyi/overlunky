#pragma once

#include <functional>
#include <map>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "memory.hpp"

struct RenderInfo;
struct Texture;

enum REPEAT_TYPE : uint8_t
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
    REPEAT_TYPE repeat;
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
using EntityCreate = Entity* (*)();
using EntityDestroy = void (*)(Entity*);
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
    uint8_t draw_depth;
    uint8_t default_b3f; // value gets copied into entity.b3f along with draw_depth etc (RVA 0x21F30CC4)
    int16_t field_26;
    Rect rect_collision;
    int32_t field_3C;
    int32_t field_40;
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
    float glow_red;
    float glow_green;
    float glow_blue;
    float glow_alpha;
    int32_t texture;
    int32_t technique;
    int32_t tile_x;
    int32_t tile_y;
    uint8_t damage;
    uint8_t life;
    uint8_t field_96;
    uint8_t blood_content;
    bool leaves_corpse_behind;
    uint8_t field_99;
    uint8_t field_9A;
    uint8_t field_9B;
    int32_t description;
    int32_t sound_killed_by_player;
    int32_t sound_killed_by_other;
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

    EntityItem(const std::string& name_, uint64_t id_)
        : name(name_), id(id_)
    {
    }
    bool operator<(const EntityItem& item) const
    {
        return id < item.id;
    }
};

EntityDB* get_type(uint32_t id);

int32_t to_id(std::string_view id);

template <typename T>
std::string to_le_bytes(T fmt)
{
    return std::string((char*)&fmt, sizeof(T));
}

class Vector
{
  public:
    uint32_t* heap;
    uint32_t* begin;
    uint32_t size, count;

    bool empty()
    {
        return !!size;
    }
};

class Entity
{
  public:
    EntityDB* type;
    Entity* overlay;
    Vector items;
    uint32_t flags;
    uint32_t more_flags;
    int32_t uid;
    uint16_t animation_frame;
    uint8_t draw_depth;
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
    RenderInfo* rendering_info;
    Texture* texture;
    float tilew;
    float tileh;
    uint8_t layer;
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

    Entity* topmost()
    {
        auto cur = this;
        while (cur->overlay)
        {
            cur = cur->overlay;
        }
        return cur;
    }

    Entity* topmost_mount()
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

    bool overlaps_with(float rect_left, float rect_bottom, float rect_right, float rect_top)
    {
        const auto [posx, posy] = position();
        const float left = posx - hitboxx + offsetx;
        const float right = posx + hitboxx + offsetx;
        const float bottom = posy - hitboxy + offsety;
        const float top = posy + hitboxy + offsety;

        return left < rect_right && rect_left < right && bottom < rect_top && rect_bottom < top;
    }

    bool overlaps_with(Entity* other)
    {
        const auto [other_posx, other_posy] = other->position();
        const float other_left = other_posx - other->hitboxx + other->offsetx;
        const float other_right = other_posx + other->hitboxx + other->offsetx;
        const float other_top = other_posy + other->hitboxy + other->offsety;
        const float other_bottom = other_posy - other->hitboxy + other->offsety;

        return overlaps_with(other_left, other_bottom, other_right, other_top);
    }

    std::pair<float, float> position_self() const;
    std::pair<float, float> position_render() const;
    void remove_item(uint32_t id);
    void destroy();

    std::uint32_t get_texture();
    bool set_texture(std::uint32_t texture_id);

    void unhook(std::uint32_t id);
    struct EntityHooksInfo& get_hooks();

    std::uint32_t set_on_destroy(std::function<void(Entity*)> cb);

    template <typename T>
    T* as()
    {
        return static_cast<T*>(this);
    }

    virtual ~Entity() = 0;
    virtual void created() = 0;
    virtual void kill(bool, Entity* frm) = 0;
    virtual void v2(Entity*) = 0;
    virtual void v3() = 0;
    virtual void v4(void*) = 0;
    virtual void hiredhand_description(char*) = 0;
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
    virtual void remove_item_ptr(Entity*) = 0;
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
    int16_t poison_tick_timer; // Used in level transition to transfer to new player entity
    bool cursed;               // Used in level transition to transfer to new player entity
    uint8_t unknown_state;
    uint8_t kapala_blood_amount; // Used in level transition to transfer to new player entity

    uint8_t unknown2;
    uint32_t unknown3;
    uint32_t unknown4;
    uint8_t unknown5a;
    uint8_t unknown5b;
    uint8_t unknown5c;

    uint8_t player_slot;

    uint32_t unknown6;
    uint32_t unknown7;

    uint32_t collected_money[512]; // entity types
    uint32_t collected_money_values[512];
    uint32_t collected_money_count;
    uint32_t killed_enemies[256]; // entity types
    uint32_t kills_level;
    uint32_t kills_total;

    uint32_t unknown8;
    uint32_t unknown9;
    uint32_t unknown10;
    uint32_t unknown11;
    int32_t unknown12;
    int32_t unknown13;
    int32_t unknown14;
    int32_t unknown15;

    uint32_t companions[8]; // hired hands, unlocked chars

    uint32_t unknown24;
    uint32_t unknown25;
    uint32_t unknown26;
    uint32_t unknown27;
    uint32_t unknown28;
    uint32_t unknown29;
    uint32_t unknown30;
    uint32_t unknown31;

    uint8_t companion_trust[8];
    uint8_t companion_count;
    uint8_t companion_unknown_state[8];

    uint8_t unknown35;
    uint8_t unknown36;
    uint8_t unknown37;
    uint32_t unknown38;
    uint32_t unknown39;

    uint32_t acquired_powerups[30]; // Used in level transition to transfer to new player entity
    uint32_t collected_money_total;
};

class SoundPosition
{
  public:
    size_t __vftable;
    float x;
    float y;
    size_t sound_effect_pointer; // param to FMOD::Studio::EventInstance::SetParameterByID (this ptr + 0x30); soundeffect doesn't seem to change when you pick a similar object around it
    uint64_t fmod_param_id;      // param to FMOD::Studio::EventInstance::SetParameterByID
    float sound_x_coord_1;
    float sound_x_coord_2; // sometimes similar as coord_1, sometimes inverted, sometimes different
    float unknown5;
    float unknown6;
    float unknown7;
    float unknown8;
    float unknown9;
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

std::vector<EntityItem> list_entities();

std::tuple<float, float, int> get_position(uint32_t id);
std::tuple<float, float, int> get_render_position(uint32_t id);
