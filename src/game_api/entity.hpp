#pragma once

#include <array>
#include <functional>
#include <map>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "aliases.hpp"
#include "color.hpp"
#include "math.hpp"
#include "memory.hpp"
#include "state_structs.hpp"

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
    float offsetx;
    float offsety;
    float hitboxx;
    float hitboxy;
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
    ENT_TYPE id;
    uint32_t search_flags;
    float width;
    float height;
    uint8_t draw_depth;
    uint8_t default_b3f; // value gets copied into entity.b3f along with draw_depth etc (RVA 0x21F30CC4)
    int16_t field_26;
    Rect rect_collision;
    uint32_t default_duckmask;
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
    STRINGID description;
    int32_t sound_killed_by_player;
    int32_t sound_killed_by_other;
    float field_a8;
    int32_t field_AC;
    AnimationMap animations;
    float default_special_offsetx;
    float default_special_offsety;
    uint8_t init;
};

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

EntityDB* get_type(uint32_t id);

ENT_TYPE to_id(std::string_view id);

class Vector
{
  public:
    uint32_t* heap;
    uint32_t* begin;
    uint32_t size, count;

    bool empty()
    {
        return !!count;
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
    /// Don't edit this dirrectly, use `set_draw_depth`
    uint8_t draw_depth;
    uint8_t b3f; //depth related, changed when going thru doors etc.
    float x;
    float y;
    float abs_x; // only for movable entities, or entities that can be spawned without overlay, for the rest it's FLOAT_MIN?
    float abs_y;
    float w;
    float h;
    float special_offsetx;
    float special_offsety;
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
    uint8_t b99; //this looks like FLOORSTYLED post-processing
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

    /// Moves the entity to specified layer, nothing else happens, so this does not emulate a door transition
    void set_layer(LAYER layer);
    /// Moves the entity to the limbo-layer where it can later be retrieved from again via `respawn`
    void remove();
    /// Moves the entity from the limbo-layer (where it was previously put by `remove`) to `layer`
    void respawn(LAYER layer);

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
            if (cur->type->search_flags <= 2)
            {
                topmost = cur;
            }
            else
                break;
        }
        return topmost;
    }

    bool overlaps_with(AABB hitbox)
    {
        return overlaps_with(hitbox.left, hitbox.bottom, hitbox.right, hitbox.top);
    }

    /// Deprecated
    /// Use `overlaps_with(AABB hitbox)` instead
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

    TEXTURE get_texture();
    bool set_texture(TEXTURE texture_id);

    void unhook(std::uint32_t id);
    struct EntityHooksInfo& get_hooks();

    bool is_movable();

    std::uint32_t set_on_dtor(std::function<void(Entity*)> cb);
    std::uint32_t reserve_callback_id();
    void set_on_destroy(std::uint32_t reserved_callback_id, std::function<void(Entity*)> on_destroy);
    void set_on_kill(std::uint32_t reserved_callback_id, std::function<void(Entity*, Entity*)> on_kill);
    void set_pre_collision1(std::uint32_t reserved_callback_id, std::function<bool(Entity*, Entity*)> pre_collision1);
    void set_pre_collision2(std::uint32_t reserved_callback_id, std::function<bool(Entity*, Entity*)> pre_collision2);

    template <typename T>
    T* as()
    {
        return static_cast<T*>(this);
    }

    virtual ~Entity() = 0;
    virtual void create_rendering_info() = 0;
    virtual void handle_state_machine() = 0;

    /// Kills the entity in the most violent way possible, for example cavemen turn into gibs
    virtual void kill(bool, Entity* frm) = 0;

    virtual void on_collision1(Entity* other_entity) = 0; // triggers on collision between whip and hit object

    /// Completely removes the entity from existence
    virtual void destroy() = 0;

    virtual void apply_texture(Texture*) = 0;
    virtual void hiredhand_description(char*) = 0;
    virtual void generate_stomp_damage_particles(Entity* victim) = 0; // particles when jumping on top of enemy
    virtual float get_type_field_a8() = 0;
    virtual bool block_pushing_related() = 0; // does a bittest for the 14 entities starting at pushblock, function hits when player pushes entity
    virtual void v11() = 0;
    virtual bool v12() = 0;                            // disabling this functions stops stomp damage, jump falling calculations, running
    virtual bool check_type_properties_flags_19() = 0; // checks (properties_flags >> 0x12) & 1; can't get it to trigger
    virtual uint32_t get_type_field_60() = 0;
    virtual void set_invisible(bool) = 0;
    virtual void handle_turning_left(bool apply) = 0; // if disabled, monsters don't turn left and keep walking in the wall (and other right-left issues)
    virtual void set_draw_depth(uint8_t draw_depth) = 0;
    virtual void resume_ai() = 0; // works on entities with ai_func != 0; runs when companions are let go from being held. AI resumes anyway in 1.23.3
    virtual float friction() = 0;
    virtual void v20() = 0;
    virtual void remove_item_ptr(Entity*) = 0;
    virtual Entity* get_held_entity() = 0;
    virtual void v23() = 0;
    virtual bool on_open(Entity* opener) = 0; // used for crates and presents

    /// Activates a button prompt (with the Use door/Buy button), e.g. buy shop item, activate drill, read sign, interact in camp, ... `get_entity(<udjat socket uid>):activate(players[1])` (make sure player 1 has the udjat eye though)
    virtual void activate(Entity* activator) = 0;

    virtual void on_collision2(Entity* other_entity) = 0; // needs investigating, difference between this and on_collision1
    virtual uint64_t on_save_level_transition_data() = 0; // e.g. for turkey: stores health, poison/curse state, for mattock: remaining swings (returned value is transferred)
    virtual void on_restore_level_transition_data(uint64_t data) = 0;
    virtual void on_walked_on_by(Entity* walker) = 0;  // hits when monster/player walks on a floor, does something when walker.velocityy<-0.21 (falling onto) and walker.hitboxy * hitboxx > 0.09
    virtual void on_walked_off_by(Entity* walker) = 0; // appears to be disabled in 1.23.3? hits when monster/player walks off a floor, it checks whether the walker has floor as overlay, and if so, removes walker from floor's items by calling virtual remove_item_ptr
    virtual void v31() = 0;
    virtual void on_stood_on_by(Entity* entity) = 0;  // e.g. pots, skulls, pushblocks, ... standing on floors
    virtual void toggle_backlayer_illumination() = 0; // for the player: when going to the backlayer, turns on player emitted light
    virtual void v34() = 0;
    virtual void liberate_from_shop() = 0; // can also be seen as event: when you anger the shopkeeper, this function gets called for each item; can be called on shopitems individually as well and they become 'purchased'

    /// Applies changes made in `entity.type`
    virtual void apply_db() = 0;

    //virtual void v38() = 0;
};

struct Inventory
{
    uint32_t money;
    uint8_t bombs;
    uint8_t ropes;
    /// Used in level transition to transfer to new player entity, is wrong during the level
    int16_t poison_tick_timer;
    /// Used in level transition to transfer to new player entity, is wrong during the level
    bool cursed;
    /// Used in level transition to transfer to new player entity, is wrong during the level
    bool elixir_buff;
    /// Used in level transition to transfer to new player entity, is wrong during the level
    uint8_t health;
    /// Used in level transition to transfer to new player entity, is wrong during the level
    uint8_t kapala_blood_amount;

    uint32_t unknown3;
    /// Used in level transition to transfer to new player entity, is wrong during the level
    ENT_TYPE held_item;
    /// Metadata of the held item (health, is cursed etc.) Used in level transition to transfer to new entity, is wrong during the level
    int16_t held_item_metadata;
    uint8_t unknown5c; //padding?

    int8_t player_slot;
    /// Used in level transition to transfer to new player entity, is wrong during the level
    ENT_TYPE mount_type;
    /// Metadata of the mount (health, is cursed etc.) Used in level transition to transfer to new player entity, is wrong during the level
    int16_t mount_metadata;
    int16_t unknown_mount_ralated;

    std::array<ENT_TYPE, 512> collected_money; // entity types
    std::array<uint32_t, 512> collected_money_values;
    uint32_t collected_money_count;
    std::array<ENT_TYPE, 256> killed_enemies; // entity types
    uint32_t kills_level;
    uint32_t kills_total;

    std::array<int16_t, 8> unknown_companions_realated;
    /// Companions poison tick timers, used in level transition to transfer to new player entity, is wrong during the level
    std::array<int16_t, 8> companion_poison_tick_timers;
    /// Companion ENT_TYPEs, used in level transition to transfer to new player entity, is wrong during the level
    std::array<ENT_TYPE, 8> companions;
    /// Items ENT_TYPE held by companions, used in level transition to transfer to new player entity, is wrong during the level
    std::array<ENT_TYPE, 8> companion_held_items;
    /// 0..3, used in level transition to transfer to new player entity, is wrong during the level
    std::array<uint8_t, 8> companion_trust;
    /// Number of companions, this is always up to date, can be edited
    uint8_t companion_count;
    /// Used in level transition to transfer to new player entity, is wrong during the level
    std::array<uint8_t, 8> companion_health;
    /// Used in level transition to transfer to new player entity, is wrong during the level
    std::array<bool, 8> is_companion_cursed;
    uint8_t padding1;
    uint8_t padding2;
    uint8_t padding3;

    /// Used in level transition to transfer to new player entity, is wrong during the level
    std::array<ENT_TYPE, 30> acquired_powerups;
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

std::vector<EntityItem> list_entities();

std::tuple<float, float, uint8_t> get_position(uint32_t uid);
std::tuple<float, float, uint8_t> get_render_position(uint32_t uid);

std::tuple<float, float> get_velocity(uint32_t uid);

AABB get_hitbox(uint32_t uid, bool use_render_pos);

struct EntityFactory* entity_factory();
