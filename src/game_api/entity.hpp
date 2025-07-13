#pragma once

#include <cstddef>       // for size_t
#include <cstdint>       // for uint8_t, uint32_t, int32_t, uint16_t, int64_t
#include <functional>    // for function, equal_to
#include <optional>      //
#include <span>          // for span
#include <string>        // for allocator, string
#include <string_view>   // for string_view
#include <tuple>         // for tuple
#include <type_traits>   // for move
#include <unordered_map> // for _Umap_traits<>::allocator_type, unordered_map
#include <utility>       // for pair
#include <vector>        // for vector

#include "aliases.hpp"        // for ENT_TYPE, LAYER, TEXTURE, STRINGID
#include "color.hpp"          // for Color
#include "entity_db.hpp"      // for EntityDB
#include "entity_structs.hpp" // for CollisionInfo
#include "layer.hpp"          // for EntityList
#include "math.hpp"           // for AABB, Vec2

struct RenderInfo;
struct Texture;
struct SoundMeta;

class Entity;
class Movable;

struct EntityHooksInfo;
using ENT_FLAG = uint32_t;
using ENT_MORE_FLAG = uint32_t;
using DAMAGE_TYPE = uint16_t;

enum class RECURSIVE_MODE
{
    EXCLUSIVE,
    INCLUSIVE,
    NONE,
};

class Entity
{
  public:
    /// Type of the entity, contains special properties etc. If you want to edit them just for this entity look at the EntityDB
    EntityDB* type;
    Entity* overlay;
    EntityList items;
    /// see [flags.hpp](https://github.com/spelunky-fyi/overlunky/blob/main/src/game_api/flags.hpp) entity_flags
    ENT_FLAG flags;
    /// see [flags.hpp](https://github.com/spelunky-fyi/overlunky/blob/main/src/game_api/flags.hpp) more_flags
    ENT_MORE_FLAG more_flags;
    /// Unique id of the entity, save it to variable to check this entity later (don't use the whole Entity type as it will be replaced with a different one when this is destroyed)
    int32_t uid;
    /// Number (id) of the sprite in the texture
    uint16_t animation_frame;
    /// Depth level that this entity is drawn on.
    /// Don't edit this directly, use `set_draw_depth` function
    uint8_t draw_depth;
    uint8_t b3f; // depth related, changed when going thru doors etc.
    /// Position of the entity in the world, or relative to overlay if attached to something. Use `get_absolute_position` to get real position of anything in the game world.
    float x;
    /// Position of the entity in the world, or relative to overlay if attached to something. Use `get_absolute_position` to get real position of anything in the game world.
    float y;
    /// Absolute position in the world, even if overlaid. Might be a frame off since it's updated with `apply_movement` function and so it does not update if game moves the entity in different way after movement is processed.
    /// Use `get_absolute_position` for precise. Read only.
    float abs_x;
    /// Absolute position in the world, even if overlaid. Might be a frame off since it's updated with `apply_movement` function and so it does not update if game moves the entity in different way after movement is processed.
    /// Use `get_absolute_position` for precise. Read only.
    float abs_y;
    /// Width of the sprite
    float w;
    /// Height of the sprite
    float h;
    /// Special offset used for entities attached to others (or picked by others) that need to flip to the other side when the parent flips sides
    float special_offsetx;
    /// Special offset used for entities attached to others (or picked by others) that need to flip to the other side when the parent flips sides
    float special_offsety;
    Color color;
    union
    {
        struct
        {
            /// Offset of the hitbox in relation to the entity position
            float offsetx;
            /// Offset of the hitbox in relation to the entity position
            float offsety;
            /// Half of the width of the hitbox
            float hitboxx;
            /// Half of the height of the hitbox
            float hitboxy;
            SHAPE shape;         // 1 = rectangle, 2 = circle
            bool hitbox_enabled; // probably, off for bg, deco, logical etc
            uint8_t b82;
            uint8_t b83;
        };
        CollisionInfo collision_info;
    };
    float angle;
    RenderInfo* rendering_info;
    Texture* texture;
    /// Size of the sprite in the texture
    float tilew;
    /// Size of the sprite in the texture
    float tileh;
    /// Use `set_layer` to change
    uint8_t layer;
    uint8_t b99; // this looks like FLOORSTYLED post-processing
    uint8_t b9a;
    uint8_t b9b;
    uint32_t i9c;
    /* for the autodoc
    any user_data;
    */
    // {x, y}
    Vec2 position_self() const
    {
        return Vec2{x, y};
    }
    /// Get the absolute position of an entity in the game world
    Vec2 abs_position() const;
    /// Get's the velocity relative to the game world, only for movable or liquid entities
    Vec2 get_absolute_velocity() const;
    /// `use_render_pos` default is `false`
    AABB get_hitbox(std::optional<bool> use_render_pos) const;
    /// Moves the entity to specified layer with all it's items, nothing else happens, so this does not emulate a door transition
    void set_layer(LAYER layer);
    /// Adds the entity to its own layer, to add it to entity lookup tables without waiting for a state update
    void apply_layer();
    /// Moves the entity to the limbo-layer where it can later be retrieved from again via `respawn`
    void remove();
    /// Moves the entity from the limbo-layer (where it was previously put by `remove`) to `layer`
    void respawn(LAYER layer_to)
    {
        set_layer(layer_to);
    }
    /// Performs a teleport as if the entity had a teleporter and used it. The delta coordinates are where you want the entity to teleport to relative to its current position, in tiles (so integers, not floats). Positive numbers = to the right and up, negative left and down.
    void perform_teleport(uint8_t delta_x, uint8_t delta_y);
    /// Returns the top entity in a chain (overlay)
    Entity* topmost()
    {
        auto cur = this;
        while (cur->overlay)
        {
            cur = cur->overlay;
        }
        return cur;
    }
    /// NoDoc
    Entity* topmost_mount()
    {
        auto topmost = this;
        while (auto cur = topmost->overlay)
        {
            if (!(cur->type->search_flags & (ENTITY_MASK::MOUNT | ENTITY_MASK::PLAYER)))
                break;

            topmost = cur;
        }
        return topmost;
    }

    bool overlaps_with(AABB hitbox) const
    {
        return overlaps_with(hitbox.left, hitbox.bottom, hitbox.right, hitbox.top);
    }

    /// Deprecated
    /// Use `overlaps_with(AABB hitbox)` instead
    bool overlaps_with(float rect_left, float rect_bottom, float rect_right, float rect_top) const
    {
        const auto [posx, posy] = abs_position();
        const float left = posx - hitboxx + offsetx;
        const float right = posx + hitboxx + offsetx;
        const float bottom = posy - hitboxy + offsety;
        const float top = posy + hitboxy + offsety;

        return left < rect_right && rect_left < right && bottom < rect_top && rect_bottom < top;
    }

    bool overlaps_with(Entity* other) const
    {
        const auto [other_posx, other_posy] = other->abs_position();
        const float other_left = other_posx - other->hitboxx + other->offsetx;
        const float other_right = other_posx + other->hitboxx + other->offsetx;
        const float other_top = other_posy + other->hitboxy + other->offsety;
        const float other_bottom = other_posy - other->hitboxy + other->offsety;

        return overlaps_with(other_left, other_bottom, other_right, other_top);
    }

    TEXTURE get_texture() const;
    /// Changes the entity texture, check the [textures.txt](game_data/textures.txt) for available vanilla textures or use [define_texture](#define_texture) to make custom one
    bool set_texture(TEXTURE texture_id);

    bool is_player() const;
    bool is_movable() const;
    bool is_liquid() const;
    bool is_cursed() const
    {
        return more_flags & 0x4000;
    };

    std::vector<uint32_t> get_items() const
    {
        return std::vector<uint32_t>(items.uids().begin(), items.uids().end());
    }

    /// Kill entity along with all entities attached to it. Be aware that for example killing push block with this function will also kill anything on top of it, any items, players, monsters etc.
    /// To avoid that, you can inclusively or exclusively limit certain MASK and ENT_TYPE. Note: the function will first check mask, if the entity doesn't match, it will look in the provided ENT_TYPE's
    /// destroy_corpse and responsible are the standard parameters for the kill function
    void kill_recursive(bool destroy_corpse, Entity* responsible, std::optional<ENTITY_MASK> mask, const std::vector<ENT_TYPE> ent_types, RECURSIVE_MODE rec_mode);
    /// Short for using RECURSIVE_MODE.NONE
    void kill_recursive(bool destroy_corpse, Entity* responsible)
    {
        kill_recursive(destroy_corpse, responsible, std::nullopt, {}, RECURSIVE_MODE::NONE);
    };
    /// Destroy entity along with all entities attached to it. Be aware that for example destroying push block with this function will also destroy anything on top of it, any items, players, monsters etc.
    /// To avoid that, you can inclusively or exclusively limit certain MASK and ENT_TYPE. Note: the function will first check the mask, if the entity doesn't match, it will look in the provided ENT_TYPE's
    void destroy_recursive(std::optional<ENTITY_MASK> mask, const std::vector<ENT_TYPE> ent_types, RECURSIVE_MODE rec_mode);
    /// Short for using RECURSIVE_MODE.NONE
    void destroy_recursive()
    {
        destroy_recursive(std::nullopt, {}, RECURSIVE_MODE::NONE);
    }
    /// Detach from overlay
    void detach(std::optional<bool> check_autokill)
    {
        if (overlay)
            overlay->remove_item(this, check_autokill.value_or(true));
    }
    /// Attach to other entity (at the current relative position to it)
    void attach(Entity* new_overlay)
    {
        if (new_overlay == overlay)
            return;

        detach(false);
        auto const pos = new_overlay->abs_position();
        x -= pos.x;
        y -= pos.y;
        overlay = new_overlay;
        overlay->items.insert(this, false);
    }

    // for supporting HookableVTable
    uint32_t get_aux_id() const
    {
        return uid;
    }
    // Needed despite HookableVTable for cleanup of arbitrary entity related data
    std::uint32_t set_on_dtor(std::function<void(Entity*)> cb)
    {
        return hook_dtor_impl(this, std::move(cb));
    }
    void clean_on_dtor(std::uint32_t dtor_cb_id)
    {
        clear_dtor_impl(this, dtor_cb_id);
    }
    /// NoDoc
    void set_enable_turning(bool enabled);
    template <typename T>
    T* as()
    {
        return static_cast<T*>(this);
    }
    static void set_hook_dtor_impl(
        std::function<std::uint32_t(Entity*, std::function<void(Entity*)>)> hook_fun,
        std::function<void(Entity*, std::uint32_t)> clear_fun)
    {
        hook_dtor_impl = std::move(hook_fun);
        clear_dtor_impl = std::move(clear_fun);
    }
    inline static std::function<std::uint32_t(Entity*, std::function<void(Entity*)>)> hook_dtor_impl{};
    inline static std::function<void(Entity*, std::uint32_t)> clear_dtor_impl{};

    virtual ~Entity() = 0;                    // virtual 0
    virtual void create_rendering_info() = 0; // 1
    virtual void update_state_machine() = 0;  // 2
    /// Kills the entity, you can set responsible to `nil` to ignore it
    virtual void kill(bool destroy_corpse, Entity* responsible) = 0; // 3
    /// Collisions with stuff that blocks you, like walls, floors, etc. Triggers for entities in it's EntityDB.collision_mask
    virtual void on_collision1(Entity* other_entity) = 0; // 4
    /// Completely removes the entity from existence
    virtual void destroy() = 0;                                                                   // 5
    virtual void apply_texture(Texture*) = 0;                                                     // 6
    virtual void format_shopitem_name(char16_t* output) = 0;                                      // 7
    virtual void generate_damage_particles(Entity* victim, DAMAGE_TYPE damage, bool killing) = 0; // 8, contact dmg
    virtual float get_type_field_a8() = 0;                                                        // 9
    virtual bool can_be_pushed() = 0;                                                             // 10, (runs only for activefloors?) checks if entity type is pushblock, for chained push block checks ChainedPushBlock.is_chained, is only a check that allows for the pushing animation
    virtual bool v11() = 0;                                                                       // 11, is in motion? (only projectiles and some weapons), theme procedural spawn uses this
    /// Returns true if entity is in water/lava
    virtual bool is_in_liquid() = 0;                                  // 12, drill always returns false
    virtual bool check_type_properties_flags_19() = 0;                // 13, checks (properties_flags >> 0x12) & 1; for hermitcrab checks if he's invisible; can't get it to trigger
    virtual uint8_t get_type_field_60() = 0;                          // 14, the value is compared to entity state and used in some behavior function
    virtual void set_invisible(bool value) = 0;                       // 15
    virtual void flip(bool left) = 0;                                 // 16
    virtual void set_draw_depth(uint8_t draw_depth, uint8_t b3f) = 0; // 17
    virtual void reset_draw_depth() = 0;                              // 18
    /// Friction of this entity, affects it's contact with other entities (how fast it slows down on the floor, how fast it can move but also the other way around for floors/activefloors: how other entities can move on it)
    virtual float friction() = 0;                     //              // 19
    virtual void set_as_sound_source(SoundMeta*) = 0; //              // 20, update sound position to entity position?
    /// Can be called multiple times for the same entity (for example when play throws/drops entity from it's hands)
    virtual void remove_item(Entity* entity, bool autokill_check) = 0;       // 21, if autokill_check is true, it will check if the entity has the "kill if overlay lost" flag and kill it if it's set
    virtual Entity* get_held_entity() = 0;                                   // 22
    virtual void v23(Entity* logical_trigger, Entity* who_triggered_it) = 0; // 23, spawns LASERTRAP_SHOT from LASERTRAP, also some trigger entities use this, seam to be called right after "on_collision2", triggers use self as the first parameter. Called when there is entity overlapping trigger entity, even if they don't move
    /// Triggers weapons and other held items like teleporter, mattock etc. You can check the [virtual-availability.md](https://github.com/spelunky-fyi/overlunky/blob/main/docs/virtual-availability.md), if entity has `open` in the `on_open` you can use this function, otherwise it does nothing. Returns false if action could not be performed (cooldown is not 0, no arrow loaded in etc. the animation could still be played thou)
    virtual bool trigger_action(Entity* user) = 0; //                        // 24, also used for throwables, disabling this for bomb make it always spawn an the ground, but you can still pick it up and throw it
    /// Activates a button prompt (with the Use door/Buy button), e.g. buy shop item, activate drill, read sign, interact in camp, ... `get_entity(<udjat socket uid>):activate(players[1])` (make sure player 1 has the udjat eye though)
    virtual void activate(Entity* activator) = 0; //                         // 25
    /// More like on_overlap, triggers when entities touch/overlap each other. Triggers for entities in it's EntityDB.collision2_mask
    virtual void on_collision2(Entity* other_entity) = 0; // 26
    /// e.g. for turkey: stores health, poison/curse state, for mattock: remaining swings (returned value is transferred)
    virtual uint16_t get_metadata() = 0;                    // 27
    virtual void apply_metadata(uint16_t metadata) = 0;     // 28
    virtual void on_walked_on_by(Entity* walker) = 0;       // 29, hits when monster/player walks on a floor, does something when walker.velocityy<-0.21 (falling onto) and walker.hitboxy * hitboxx > 0.09
    virtual void on_walked_off_by(Entity* walker) = 0;      // 30, appears to be disabled in 1.23.3? hits when monster/player walks off a floor, it checks whether the walker has floor as overlay, and if so, removes walker from floor's items by calling virtual remove_item
    virtual void on_ledge_grab(Entity* who) = 0;            // 31, only ACTIVEFLOOR_FALLING_PLATFORM, does something with game menager
    virtual void on_stood_on_by(Entity* entity, Vec2*) = 0; // 32, e.g. pots, skulls, pushblocks, ... standing on floors. The Vec2 is just a guess, it only compares Y with 0.1f
    /// only for CHAR_*: when going to the backlayer, turns on/off player emitted light
    virtual void toggle_backlayer_illumination() = 0; //    // 33
    virtual void v34() = 0;                           //    // 34, only ITEM_TORCH, calls Torch.light_up(false), can't get it to trigger
    /// `clear_parent` used only for CHAR_* entities, sets the `linked_companion_parent` to -1. It's not called when item is bought
    virtual void liberate_from_shop(bool clear_parent) = 0; // 35, can also be seen as event: when you anger the shopkeeper, this function gets called for each item; can be called on shopitems individually as well and they become 'purchased'
    /// Applies changes made in `entity.type`
    virtual void apply_db() = 0; // 36, This is actually just an initialize call that is happening once after  the entity is created
};

Entity* get_entity_ptr(uint32_t uid);
