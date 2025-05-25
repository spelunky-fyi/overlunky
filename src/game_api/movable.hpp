#pragma once

#include "containers/custom_map.hpp"
#include "containers/custom_set.hpp"
#include "entity.hpp"
#include "movable_behavior.hpp"

#include <functional>

struct MovableBehavior;

class CutsceneBehavior
{
  public:
    virtual ~CutsceneBehavior(){};
    virtual void update(Movable* e) = 0;
    // no more virtuals, it's possible that different sub classes have some extra variables as well
};

class Movable : public Entity
{
  public:
    custom_map<uint32_t, MovableBehavior*> behaviors_map;
    custom_set<MovableBehavior*, SortMovableBehavior> behaviors;
    MovableBehavior* current_behavior;
    CutsceneBehavior* cutscene_behavior;
    union
    {
        /// {movex, movey}
        Vec2 move;
        struct
        {
            /// Move directions (-1.0 to 1.0) that represent in whit direction the entity want's to move
            float movex;
            /// Move directions (-1.0 to 1.0) that represent in whit direction the entity want's to move
            float movey;
        };
    };
    BUTTON buttons;
    BUTTON buttons_previous;
    int16_t unknown_padding; // garbage?
    uint32_t stand_counter;
    /// EntityDB.jump gets multiplied by this to get the jump
    float jump_height_multiplier;
    int32_t price;
    int32_t owner_uid;
    int32_t last_owner_uid;
    Animation* current_animation;
    uint32_t idle_counter;
    int32_t standing_on_uid;
    /// speed, can be relative to the platform you standing on (pushblocks, elevators), use [get_velocity](#get_velocity) to get accurate speed in the game world
    float velocityx;
    /// speed, can be relative to the platform you standing on (pushblocks, elevators), use [get_velocity](#get_velocity) to get accurate speed in the game world
    float velocityy;
    int32_t holding_uid;
    uint8_t state;
    uint8_t last_state;
    uint8_t move_state;
    uint8_t health;
    uint16_t stun_timer;
    uint16_t stun_state;
    union
    { // weird fix to not break compatibility with already exposed some_state
        /// NoDoc
        uint32_t some_state;
        struct
        {
            /// Related to taking damage, also drops you from ladder/rope, can't be set while on the ground unless you're on a mount
            uint16_t lock_input_timer;
            uint16_t wet_effect_timer; // fading the entity to black, similar to dark_shadow_timer
        };
    };
    /// Used to apply damage from poison, can be set to -1 to cure poison, to set poison use [poison_entity](#poison_entity)
    int16_t poison_tick_timer;
    uint8_t onfire_effect_timer;
    uint8_t exit_invincibility_timer;   // when exiting a door or a pipe, ...
    uint8_t invincibility_frames_timer; // blinks the entity
    uint8_t frozen_timer;
    uint8_t dont_damage_owner_timer;       // When > 0, will not deal damage to the owner_uid, so that throwing an object does not harm yourself.
    uint8_t knockback_invincibility_timer; // Can be overridden in damage flags.
    uint8_t reset_owner_timer;             // Timer for resetting owner_uid.
    uint8_t i120b;                         // timer
    /// Makes you immune to the item you just thrown for short time
    uint8_t throw_damage_immunity_timer;
    uint8_t i120d;
    uint8_t b124;
    uint8_t falling_timer;
    uint8_t exit_gold_invincibility_timer; // When > 0, money intersecting a door or at a pipe entrance will not be collected.
    uint8_t b127;

    /// NoDoc
    void poison(int16_t frames); // 1 - 32767 frames ; -1 = no poison // Changes default poison_tick_timer
    bool is_poisoned() const
    {
        return (poison_tick_timer != -1);
    }

    /// NoDoc
    bool broken_damage(uint32_t damage_dealer_uid, int8_t damage_amount, uint16_t stun_time, float velocity_x, float velocity_y, std::optional<uint8_t> iframes)
    {
        auto dealer = get_entity_ptr(damage_dealer_uid);

        Vec2 velocity{velocity_x, velocity_y};
        uint8_t unknown1{0};
        bool unknown2{true};
        return damage(dealer, damage_amount, 0x1, &velocity, unknown1, stun_time, iframes.value_or(80), unknown2);
    }

    bool is_button_pressed(BUTTON button) const
    {
        return (buttons & button) == button && (buttons_previous & button) == 0;
    }
    bool is_button_held(BUTTON button) const
    {
        return (buttons & button) == button && (buttons_previous & button) == button;
    }
    bool is_button_released(BUTTON button) const
    {
        return (buttons & button) == 0 && (buttons_previous & button) == button;
    }
    void set_pre_statemachine(std::uint32_t reserved_callback_id, std::function<bool(Movable*)> pre_state_machine);
    void set_post_statemachine(std::uint32_t reserved_callback_id, std::function<void(Movable*)> post_state_machine);

    /// Force the gravity for this entity. Will override anything set by special states like swimming too, unless you reset it. Default 1.0
    void set_gravity(float gravity);

    /// Remove the gravity hook and reset to defaults
    void reset_gravity();

    /// Get all available behavior ids
    std::vector<uint32_t> get_all_behaviors();
    /// Set behavior, this is more than just state as it's an active function, for example climbing ladder is a behavior and it doesn't actually need ladder/rope entity
    /// Returns false if entity doesn't have this behavior id
    bool set_behavior(uint32_t behavior_id);
    /// Get the current behavior id
    uint32_t get_behavior();

    /// Set the absolute position of an entity and offset all rendering related things accordingly to teleport without any interpolation or graphical glitches. If the camera is focused on the entity, it is also moved.
    void set_position(float to_x, float to_y);

    /// Return true if the entity is allowed to jump, even midair. Return false and can't jump, except from ladders apparently.
    virtual bool can_jump() = 0;                                             // 37
    virtual void get_collision_info(CollisionInfo& dest) = 0;                // 38, from entityDB
    virtual float sprint_factor() = 0;                                       // 39, from entityDB
    virtual float calculate_jump_velocity(bool dont_ignore_liquid) = 0;      // 40
    virtual std::unordered_map<uint8_t, Animation>& get_animation_map() = 0; // 41
    /// Mostly used for ragdoll by the game
    virtual void apply_velocity(Vec2* velocities, bool ignore_weight) = 0; // 42
    /// Returns the damage that the entity deals
    virtual int8_t get_damage() = 0;       //                                // 43, for player it calculates stomp damages as that's the only damage that the player entity can deal, the "normal" damage is done by the whip
    virtual int8_t get_stomp_damage() = 0; //                                // 44, calls get_damage except for mech which always returns 3, dunno what's the difference between this and get_damage
    virtual bool is_on_fire() = 0;         //                                // 45
    /// Runs on contact damage, returns false if there wasn't any interaction (called from on_collision2, will be called as long as the hitboxes overlap)
    virtual bool attack(Entity* victim) = 0; //                              // 46
    /// Same as above, but for being thrown into something and potentially dealing damage that way
    virtual bool thrown_into(Entity* victim) = 0; //                         // 47
    /// Damage the movable by the specified amount, stuns and gives it invincibility for the specified amount of frames and applies the velocities. `damage_dealer` can be set to nil.
    /// Returns: true if entity was affected (for stuff like: if pot was thrown into entity, should that pot break after hit), false if the event should be ignored by damage_dealer
    virtual bool damage(Entity* damage_dealer, int8_t damage_amount, DAMAGE_TYPE damage_flags, Vec2* velocity, uint8_t unknown_damage_phase, uint16_t stun_amount, uint8_t iframes, bool unknown_is_final) = 0; // 48
    /// Hit by broken arrows etc that don't deal damage, calls damage with 0 damage.
    virtual void on_hit(Entity* damage_dealer) = 0; // 49
    /// returns sound id for the damage taken, return 0 to make it silence
    virtual SOUNDID get_damage_sound(DAMAGE_TYPE damage) = 0; // 50
    virtual void stun(uint16_t framecount) = 0;               // 51
    /// Sets the `frozen_timer`, the param `ignore_lava` doesn't do much, just skips the liquid check,
    /// if in lava the game will set `frozen_timer` to 0 immediately most of the time
    virtual void freeze(uint8_t framecount, bool ignore_lava) = 0; // 52
    /// Does not damage entity
    virtual void light_on_fire(uint8_t time) = 0;       // 53
    virtual void set_cursed(bool b, bool effect) = 0;   // 54
    virtual void on_spiderweb_collision(bool) = 0;      // 55, the bool sets pause statemachine flag? needs testing
    virtual void set_last_owner_uid(Entity* owner) = 0; // 56, assigns entity as last_owner_uid and also manipulates movable.b127
    virtual uint32_t get_last_owner_uid() = 0;          // 57, for players, it checks !stunned && !frozen && !cursed && !has_overlay; for others: just returns last_owner_uid
    /// Disable to not get killed outside level bounds.
    virtual void check_out_of_bounds() = 0;               // 58, kills with the 'still falling' death cause, is called for any item/fx/mount/monster/player
    virtual void set_standing_on(int32_t entity_uid) = 0; // 59
    virtual Entity* standing_on() = 0;                    // 60
    virtual bool on_stomped_on_by(Entity* stomper) = 0;   // 61
    virtual void on_thrown_by(Entity* thrower) = 0;       // 62, implemented for special cases like hired hand (player with ai_func), horned lizard...
    /// Entities must be of the same type!
    virtual void copy_extra_info(Entity* clone, int32_t some_entity_uid) = 0; // 63, some_entity_uid - only used for CHAR_ entities, related to hired hand chain
    virtual uint32_t get_type_id() = 0;                                       // 64, dunno what for, implemented solely that ITEM_EXCALIBUR can return ITEM_BROKENEXCALIBUR instead
    virtual bool doesnt_have_spikeshoes() = 0;                                // 65, potentially wrong name. For most entities checks if they are dead, frozen or stun (and apparently returns false if they are), for Yeti queen checks something in the animation_func, returns true for all the items etc. only for CHAR_ entities checks the spike shoes
    virtual bool is_player_mount_or_monster() = 0;                            // 66, returns false for MONS_ALIENQUEEN, MONS_FIREFROG and MOUNT_MECH, for the rest checks EntityDB mask with value 7
    virtual void pick_up(Entity* entity_to_pick_up) = 0;                      // 67
    virtual bool can_be_picked_up_by(Entity* entity_picking_up, bool) = 0;    // 68, the bool has something to do with the entity being attached to some entity already
    /// Called when dropping or throwing
    virtual void drop() = 0; //                                               // 69
    /// Adds or subtracts the specified amount of money to the movable's (player's) inventory. Shows the calculation animation in the HUD. Adds treasure to the inventory list shown on transition. Use the global add_money to add money without adding specific treasure.
    virtual bool collect_treasure(int32_t value, ENT_TYPE treasure) = 0; // 70
    virtual bool apply_movement(uint8_t, uint8_t, uint8_t) = 0;          // 71, disable this function and things can't move, some spin in place
    virtual void damage_entity(Entity* victim) = 0;                      // 72, implemented for responsibility and journal update
    virtual bool v73() = 0;                                              // 73, checks some flags, held entity, is in liquid, floor entities around?, standing_on, does the current theme has the loop
    virtual bool is_powerup_capable() = 0;                               // 74
    virtual void initialize() = 0;                                       // 75, e.g. cobra: set random spit_timer; bat: set random stand_counter; emerald: set price
    virtual void check_is_falling() = 0;                                 // 76, sets more_flags.falling by comparing velocityy to 0, sets i120a to FF, clears owner_uid, can call remove_rider on mounts, for player updates the extra y_pos, for bosses clears lock input timer
    virtual void v77() = 0;                                              // 77
    virtual void process_input() = 0;                                    // 78, more like: handle_movement
    virtual void post_collision_damage_related() = 0;                    // 79, used for enemies attacks as well? 3 versions for: eggplant minister, players and the rest
    /// Called for entity that just has been picked up
    virtual void on_picked_up() = 0; //                                                 // 80, plays pickup sound depending on the entity mask/type etc. set stun for pets and mounts etc.
    /// Called for entity that just has been thrown/dropped
    virtual void on_release() = 0; //                                                   // 81, only for hired hands and lava pots, the rest just returns
    /// Only for landing on the floor or activefloor, generates "poof" particle and plays sfx (note: when stunned, sfx is played by the damage function)
    virtual void generate_landing_effects() = 0; //                                     // 82, entity.velocityy must be < -0.12 to generate a poof, might do other stuff regarding falling/landing
    /// Applies gravity to entity. Disable to float like on hoverpack.
    virtual void handle_fall_logic(float) = 0;                                          // 83, adjusts entity.velocityy when falling
    virtual void apply_friction(float, bool vertical, float) = 0;                       // 84, applies entity.type.friction to entity.velocityx, the two floats for characters just multiply the friction, could also be returning the value
    virtual bool can_break_block(bool horizontal, Entity* block) = 0;                   // 85, check on collision if the entity should break the block, used for stuff like drill, hundun etc. surprisingly no mattoc
    virtual void break_block(bool camera_shake, Entity* block) = 0;                     // 86
    virtual void v87(Entity* entity, float, Entity* floor, float, bool) = 0;            // 87, on_contact_with_ground ? calls on_stood_on_by, on_fall_onto
    virtual void v88(Entity* entity, float vecloty) = 0;                                // 88, on_ragdoll? - for player, triggers only when you throw him into wall/ground/celling
    virtual bool v89(void*, void*, bool, bool default_return_flipped) = 0;              // 89, triggers on item_rubble?, first parameter only tested if it's 0 for punishball, ignored in the rest, second parameter never used (leftover?)
    virtual void on_crushed_by(Entity*) = 0;                                            // 90, e.g. crushed by elevator, punishball, pushblock, crushtrap (not quillback or boulder)
    virtual SoundMeta* on_fall_onto(SOUNDID play_sound_id, Entity* fell_on_entity) = 0; // 91, plays the sfx at the entity and sets sound parameters
    virtual void on_body_destruction() = 0;                                             // 92, creates some big struct on stack, feeds it to some unknown function
};
