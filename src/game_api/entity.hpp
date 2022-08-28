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

#include "aliases.hpp" // for ENT_TYPE, LAYER, TEXTURE, STRINGID
#include "color.hpp"   // for Color
#include "layer.hpp"   // for EntityList
#include "math.hpp"    // for AABB

struct RenderInfo;
struct Texture;

enum class REPEAT_TYPE : uint8_t
{
    NoRepeat,
    Linear,
    BackAndForth,
};

enum class SHAPE : uint8_t
{
    RECTANGLE = 1,
    CIRCLE = 2,
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
class Movable;
class Container;

template <class FunT>
struct HookWithId
{
    std::uint32_t id;
    std::function<FunT> fun;
};
struct EntityHooksInfo
{
    std::int32_t entity;
    std::uint32_t cbcount;
    std::vector<HookWithId<void(Entity*)>> on_dtor;
    std::vector<HookWithId<void(Entity*)>> on_destroy;
    std::vector<HookWithId<void(Entity*, Entity*)>> on_kill;
    std::vector<HookWithId<bool(Entity*)>> pre_floor_update;
    std::vector<HookWithId<void(Entity*)>> post_floor_update;
    std::vector<HookWithId<bool(Entity*)>> on_player_instagib;
    std::vector<HookWithId<bool(Entity*, Entity*, int8_t, float, float, uint16_t, uint8_t)>> on_damage;
    std::vector<HookWithId<bool(Movable*)>> pre_statemachine;
    std::vector<HookWithId<void(Movable*)>> post_statemachine;
    std::vector<HookWithId<void(Container*, Movable*)>> on_open;
    std::vector<HookWithId<bool(Entity*, Entity*)>> pre_collision1;
    std::vector<HookWithId<bool(Entity*, Entity*)>> pre_collision2;
    std::vector<HookWithId<bool(Entity*)>> pre_render;
    std::vector<HookWithId<void(Entity*)>> post_render;
};

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
    Rect rect_collision;
    uint8_t default_shape;
    bool default_hitbox_enabled;
    uint8_t field_3A;
    uint8_t field_3B;
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
    std::unordered_map<uint8_t, Animation> animations;
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

std::string_view to_name(ENT_TYPE id);

class Entity
{
  public:
    EntityDB* type;
    Entity* overlay;
    EntityList items;
    uint32_t flags;
    uint32_t more_flags;
    int32_t uid;
    uint16_t animation_frame;
    /// Don't edit this directly, use `set_draw_depth`
    uint8_t draw_depth;
    uint8_t b3f; // depth related, changed when going thru doors etc.
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
    SHAPE shape;         // 1 = rectangle, 2 = circle
    bool hitbox_enabled; // probably, off for bg, deco, logical etc
    uint8_t b82;
    uint8_t b83;
    float angle;
    RenderInfo* rendering_info;
    Texture* texture;
    float tilew;
    float tileh;
    uint8_t layer;
    uint8_t b99; // this looks like FLOORSTYLED post-processing
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
    /// Performs a teleport as if the entity had a teleporter and used it. The delta coordinates are where you want the entity to teleport to relative to its current position, in tiles (so integers, not floats). Positive numbers = to the right and up, negative left and down.
    void perform_teleport(uint8_t delta_x, uint8_t delta_y);

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
    void remove_item(uint32_t item_uid);

    TEXTURE get_texture();
    /// Changes the entity texture, check the [textures.txt](game_data/textures.txt) for available vanilla textures or use [define_texture](#define_texture) to make custom one
    bool set_texture(TEXTURE texture_id);

    void unhook(std::uint32_t id);
    struct EntityHooksInfo& get_hooks();

    bool is_player();
    bool is_movable();
    bool is_liquid();

    std::uint32_t set_on_dtor(std::function<void(Entity*)> cb);
    std::uint32_t reserve_callback_id();
    void set_on_destroy(std::uint32_t reserved_callback_id, std::function<void(Entity*)> on_destroy);
    void set_on_kill(std::uint32_t reserved_callback_id, std::function<void(Entity*, Entity*)> on_kill);
    void set_on_player_instagib(std::uint32_t reserved_callback_id, std::function<bool(Entity*)> on_instagib);
    void set_on_damage(std::uint32_t reserved_callback_id, std::function<bool(Entity*, Entity*, int8_t, float, float, uint16_t, uint8_t)> on_damage);
    void set_pre_floor_update(std::uint32_t reserved_callback_id, std::function<bool(Entity*)> pre_update);
    void set_post_floor_update(std::uint32_t reserved_callback_id, std::function<void(Entity*)> post_update);
    void set_pre_collision1(std::uint32_t reserved_callback_id, std::function<bool(Entity*, Entity*)> pre_collision1);
    void set_pre_collision2(std::uint32_t reserved_callback_id, std::function<bool(Entity*, Entity*)> pre_collision2);
    void set_pre_render(std::uint32_t reserved_callback_id, std::function<bool(Entity* self)> pre_render);
    void set_post_render(std::uint32_t reserved_callback_id, std::function<void(Entity* self)> post_render);

    std::span<uint32_t> get_items();

    template <typename T>
    T* as()
    {
        return static_cast<T*>(this);
    }

    virtual ~Entity() = 0; // vritual 0
    virtual void create_rendering_info() = 0;
    virtual void handle_state_machine() = 0;

    /// Kills the entity, you can set responsible to `nil` to ignore it
    virtual void kill(bool destroy_corpse, Entity* responsible) = 0;

    virtual void on_collision1(Entity* other_entity) = 0; // triggers on collision between whip and hit object

    /// Completely removes the entity from existence
    virtual void destroy() = 0;

    virtual void apply_texture(Texture*) = 0;
    virtual void format_shopitem_name(char16_t*) = 0;
    virtual void generate_stomp_damage_particles(Entity* victim) = 0; // particles when jumping on top of enemy
    virtual float get_type_field_a8() = 0;
    virtual bool can_be_pushed() = 0; // (runs only for activefloors?) checks if entity type is pushblock, for chained push block checks ChainedPushBlock.is_chained, is only a check that allows for the pushing animation
    virtual bool v11() = 0;           // for arrows: returns true if it's moving (for y possibily checks for some speed as well?)
    /// Returns true if entity is in water/lava
    virtual bool is_in_liquid() = 0;
    virtual bool check_type_properties_flags_19() = 0; // checks (properties_flags >> 0x12) & 1; for hermitcrab checks if he's invisible; can't get it to trigger
    virtual uint32_t get_type_field_60() = 0;
    virtual void set_invisible(bool value) = 0;
    virtual void handle_turning_left(bool apply) = 0; // if disabled, monsters don't turn left and keep walking in the wall (and other right-left issues)
    virtual void set_draw_depth(uint8_t draw_depth) = 0;
    virtual void resume_ai() = 0; // works on entities with ai_func != 0; runs when companions are let go from being held. AI resumes anyway in 1.23.3
    virtual float friction() = 0;
    virtual void v20() = 0;
    virtual void remove_item_ptr(Entity*) = 0;
    virtual Entity* get_held_entity() = 0;
    virtual void v23(Entity* logical_trigger, Entity* who_triggered_it) = 0; // spawns LASERTRAP_SHOT from LASERTRAP, also some trigger entities use this, seam to be called right after "on_collision2", tiggers use self as the first parameter
    /// Triggers weapons and other held items like teleportter, mattock etc. You can check the [virtual-availability.md](https://github.com/spelunky-fyi/overlunky/blob/main/docs/virtual-availability.md), if entity has `open` in the `on_open` you can use this function, otherwise it does nothing. Returns false if action could not be performed (cooldown is not 0, no arrow loaded in etc. the animation could still be played thou)
    virtual bool trigger_action(Entity* user) = 0;
    /// Activates a button prompt (with the Use door/Buy button), e.g. buy shop item, activate drill, read sign, interact in camp, ... `get_entity(<udjat socket uid>):activate(players[1])` (make sure player 1 has the udjat eye though)
    virtual void activate(Entity* activator) = 0;

    virtual void on_collision2(Entity* other_entity) = 0; // needs investigating, difference between this and on_collision1, maybe this is on_hitbox_overlap as it works for logical tiggers
    virtual uint16_t get_metadata() = 0;                  // e.g. for turkey: stores health, poison/curse state, for mattock: remaining swings (returned value is transferred)
    virtual void apply_metadata(uint16_t metadata) = 0;
    virtual void on_walked_on_by(Entity* walker) = 0;  // hits when monster/player walks on a floor, does something when walker.velocityy<-0.21 (falling onto) and walker.hitboxy * hitboxx > 0.09
    virtual void on_walked_off_by(Entity* walker) = 0; // appears to be disabled in 1.23.3? hits when monster/player walks off a floor, it checks whether the walker has floor as overlay, and if so, removes walker from floor's items by calling virtual remove_item_ptr
    virtual void on_ledge_grab(Entity* who) = 0;       // only ACTIVEFLOOR_FALLING_PLATFORM, does something with game menager
    virtual void on_stood_on_by(Entity* entity) = 0;   // e.g. pots, skulls, pushblocks, ... standing on floors
    virtual void toggle_backlayer_illumination() = 0;  // only for CHAR_*: when going to the backlayer, turns on player emitted light
    virtual void v34() = 0;                            // only ITEM_TORCH, calls Torch.light_up(false), can't get it to trigger
    virtual void liberate_from_shop() = 0;             // can also be seen as event: when you anger the shopkeeper, this function gets called for each item; can be called on shopitems individually as well and they become 'purchased'

    /// Applies changes made in `entity.type`
    virtual void apply_db() = 0; // This is actually just an initialize call that is happening once after  the entity is created
};

std::vector<EntityItem> list_entities();

std::tuple<float, float, uint8_t> get_position(uint32_t uid);
std::tuple<float, float, uint8_t> get_render_position(uint32_t uid);

std::tuple<float, float> get_velocity(uint32_t uid);

AABB get_hitbox(uint32_t uid, bool use_render_pos);

struct EntityFactory* entity_factory();
Entity* get_entity_ptr(uint32_t uid);
Entity* get_entity_ptr_local(uint32_t uid);
