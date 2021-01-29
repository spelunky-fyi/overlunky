#pragma once

#include <unordered_map>
#include <map>
#include <string>
#include <vector>
#include "memory.h"

enum RepeatType : uint8_t {
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

struct Rect {
    int masks;
    float up_minus_down, side,
            up_plus_down;
    uint8_t field_10;
    uint8_t field_11;
    uint16_t field_12;
};

struct StateMemory {
    size_t p00;
    uint32_t screen_last;
    uint32_t screen;
    uint32_t screen_next;
    int32_t i14;
    size_t p18;
    int32_t i20;
    int32_t i24;
    int32_t i28;
    int32_t i2c;
    uint8_t ingame;
    uint8_t playing;
    uint8_t pause;
    uint8_t b33;
    int32_t i34;
    int32_t i38;
    int32_t i3c;
    int32_t i40;
    int32_t i44;
    uint32_t w;
    uint32_t h;
    int8_t kali_favor;
    int8_t kali_status;
    int8_t kali_altars_destroyed;
    uint8_t b4f;
    int32_t i50;
    int32_t i54;
    int32_t i58;
    uint32_t feedcode;
    uint32_t time_total;
    uint8_t world;
    uint8_t world_next;
    uint8_t level;
    uint8_t level_next;
    size_t leveldata;
    uint8_t theme;
    uint8_t theme_next;
    uint8_t b72;
    uint8_t b73;
    uint16_t shoppie_aggro;
    uint16_t merchant_aggro;
    uint8_t b7c;
    uint8_t b7d;
    uint8_t kills_npc;
    uint8_t level_count;
    uint8_t pad84[0x96c];
    uint32_t journal_flags;
    int32_t i9f0;
    int32_t i9f4;
    uint32_t time_last_level;
    uint32_t time_level;
    int32_t ia00;
    int32_t ia04;
    uint32_t hud_flags;
};

struct Color {
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

struct EntityDB {
    size_t create_func;
    size_t destroy_func;
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

struct EntityItem {
    std::string name;
    uint16_t id;

    EntityItem(const std::string& name_, uint64_t id_): name(name_), id(id_) {}
    bool operator<(const EntityItem &item) const
    {
        return id < item.id;
    }
};

std::vector<EntityItem> list_entities(size_t map_ptr);

size_t to_id(size_t map_ptr, std::string id);

Entity *state_find_item(size_t state_ptr, uint32_t unique_id);

template<typename T>
std::string to_le_bytes(T fmt) {
    return std::string((char *) &fmt, sizeof(T));
}

class Entity {
public:
    size_t __vftable;
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
    size_t pa0;
    int32_t ia8;
    int32_t iac;
    size_t pb0;
    int32_t ib8;
    int32_t ibc;
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
    int32_t i124;
    uint32_t inside;
    uint32_t i12c;
    uint32_t has_backpack;
    int32_t i134;
    size_t inventory_ptr;
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

    size_t pointer() {
        return (size_t)
                this;
    }

    std::pair<float, float> position();

    void teleport(float dx, float dy, bool s, float vx, float vy,
                  bool snap);

    Entity *topmost() {
        auto cur = this;
        while (cur->overlay) {
            cur = cur->overlay;
        }
        return cur;
    }

    Entity *topmost_mount() {
        auto topmost = this;
        while (auto cur = topmost->overlay) {
            if (cur->type->search_flags < 0x40) {
                topmost = cur;
            } else
                break;
        }
        return topmost;
    }

    uint8_t layer() {
        return read_u8(pointer() + 0x98);
    }

    std::pair<float, float> position_self() const;
};

class Mount : public Entity {
    void carry(Entity *rider);

    void tame(bool value);
};

using Carry = void (*)(size_t, size_t);

Carry get_carry();

class Player : public Entity {
    struct PlayerStatus {
        uint32_t what;
        uint8_t bomb;
        uint8_t rope;
    };

public:
    PlayerStatus *status() {
        return (PlayerStatus *) read_u64(pointer() + 0x138);
    }
};

class Door : public Entity {
public:
    void set_target(uint8_t w, uint8_t l, uint8_t f, uint8_t t);
};

std::vector<EntityItem> list_entities();
