#pragma once

#include "containers/custom_map.hpp"
#include "containers/custom_set.hpp"
#include "entity.hpp"
#include "movable_behavior.hpp"

#include <functional>

struct MovableBehavior;
using DAMAGE_TYPE = uint16_t;

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
    uint8_t unknown_damage_counter_a;
    uint8_t unknown_damage_counter_b;
    uint8_t i120a; // timer, damage related
    uint8_t i120b; // timer
    uint8_t i120c; // timer
    uint8_t i120d;
    uint8_t b124;
    uint8_t falling_timer;
    uint8_t b126; // timer, after layer change?
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

    // don't use this, it's only to not break backwards compatibility
    void light_on_fire_broken()
    {
        this->light_on_fire(0x64); // kind of standard value that the game uses
    }
    /// Get all available behavior ids
    std::vector<uint32_t> get_all_behaviors();
    /// Set behavior, this is more than just state as it's an active function, for example climbing ladder is a behavior and it doesn't actually need ladder/rope entity
    /// Returns false if entity doesn't have this behavior id
    bool set_behavior(uint32_t behavior_id);
    /// Get the current behavior id
    uint32_t get_behavior();

    /// Set the absolute position of an entity and offset all rendering related things accordingly to teleport without any interpolation or graphical glitches. If the camera is focused on the entity, it is also moved.
    void set_position(float to_x, float to_y);

    // for backwards compatibility
    // adds a coin to the table cause the collected_money_count is expected to increase
    void add_money_broken(int amount)
    {
        static const auto coin = to_id("ENT_TYPE_ITEM_GOLDCOIN");
        this->collect_treasure(amount, coin);
    }

    /// effect = true - plays the sound and spawn particle above entity
    void set_cursed_fix(bool b, std::optional<bool> effect)
    {
        set_cursed(b, effect.value_or(true));
    }

    /// Return true if the entity is allowed to jump, even midair. Return false and can't jump, except from ladders apparently.
    virtual bool can_jump() = 0;                                             // 37
    virtual void get_collision_info(CollisionInfo* dest) = 0;                // 38
    virtual float sprint_factor() = 0;                                       // 39
    virtual void calculate_jump_height() = 0;                                // 40, when disabled, jump height is very high
    virtual std::unordered_map<uint8_t, Animation>& get_animation_map() = 0; // 41
    virtual void apply_velocity(Vec2* velocities, bool) = 0;                 // 42, param is pointer to an array of two floats: velocity x and y, could be returning something
    /// Returns stomp damage based on shoes and fall time
    virtual int8_t stomp_damage() = 0;            // 43, calculates the amount of stomp damage applied (checks spike shoes, movable.state and stand_counter resulting in different damage values)
    virtual int8_t stomp_damage_trampoline() = 0; // 44, simply jumps to the 43rd virtual function, aka stomp_damage...
    virtual bool is_on_fire() = 0;                // 45
    virtual void v46() = 0;                       // 46
    virtual void v47() = 0;                       // 47

    /// Damage the movable by the specified amount, stuns and gives it invincibility for the specified amount of frames and applies the velocities. `damage_dealer` can be set to nil.
    /// Returns: true if entity was affected, damage_dealer should break etc. false if the event should be ignored by damage_dealer?
    virtual bool damage(Entity* damage_dealer, int8_t damage_amount, DAMAGE_TYPE damage_flags, Vec2* velocity, uint8_t unknown_damage_phase, uint16_t stun_amount, uint8_t iframes, bool unknown_is_final) = 0; // 48

    /// Hit by broken arrows etc that don't deal damage, calls on_damage with 0 damage.
    virtual void on_hit(Entity* damage_dealer) = 0; // 49
    virtual void v50() = 0;                         // 50
    virtual void stun(uint16_t framecount) = 0;     // 51
    virtual void freeze(uint8_t framecount) = 0;    // 52

    /// Does not damage entity
    virtual void light_on_fire(uint8_t time) = 0;            // 53
    virtual void set_cursed(bool b, bool effect) = 0;        // 54
    virtual void on_spiderweb_collision() = 0;               // 55
    virtual void set_last_owner_uid_b127(Entity* owner) = 0; // 56, assigns player as last_owner_uid and also manipulates movable.b127
    virtual uint32_t get_last_owner_uid() = 0;               // 57, for players, it checks !stunned && !frozen && !cursed && !has_overlay; for others: just returns last_owner_uid
    /// Disable to not get killed outside level bounds.
    virtual void check_out_of_bounds() = 0;                      // 58, kills with the 'still falling' death cause, is called for any item/fx/mount/monster/player but not for liquid :(
    virtual void v59() = 0;                                      // 59
    virtual Entity* standing_on() = 0;                           // 60, looks up movable.standing_on_uid in state.instance_id_to_pointer
    virtual void on_stomped_on_by(Entity* stomper) = 0;          // 61
    virtual void on_thrown_by(Entity* thrower) = 0;              // 62, implemented for special cases like hired hand (player with ai_func), horned lizard...
    virtual void on_clonegunshot_hit(Entity* clone) = 0;         // 63, implemented for player/hired hand: copies health to clone etc
    virtual uint32_t get_type_id() = 0;                          // 64
    virtual bool doesnt_have_spikeshoes() = 0;                   // 65
    virtual bool is_player_mount_or_monster() = 0;               // 66
    virtual void pick_up(Entity* entity_to_pick_up) = 0;         // 67
    virtual void on_picked_up_by(Entity* entity_picking_up) = 0; // 68
    /// Called when dropping or throwing
    virtual void drop(Entity* entity_to_drop) = 0; // 69, also used when throwing

    /// Adds or subtracts the specified amount of money to the movable's (player's) inventory. Shows the calculation animation in the HUD. Adds treasure to the inventory list shown on transition. Use the global add_money to add money without adding specific treasure.
    virtual void collect_treasure(int32_t value, ENT_TYPE treasure) = 0; // 70
    virtual void apply_movement() = 0;                                   // 71, disable this function and things can't move, some spin in place
    virtual void damage_entity(Entity* victim) = 0;                      // 72, can't trigger, maybe extra params are needed
    virtual void v73() = 0;                                              // 73
    virtual bool is_monster_or_player() = 0;                             // 74
    virtual void initialize() = 0;                                       // 75, e.g. cobra: set random spit_timer; bat: set random stand_counter; emerald: set price
    virtual void check_is_falling() = 0;                                 // 76, sets more_flags.falling by comparing velocityy to 0
    virtual void handle_stun_transition_animation() = 0;                 // 77, e.g. the wiggle the dog does when waking up from being stunned
    virtual void process_input() = 0;                                    // 78, unsure of params
    virtual void post_collision_damage_related() = 0;                    // 79, used for enemies attacks as well?
    virtual void on_picked_up() = 0;                                     // 80, gets called after on_picked_up_by
    virtual void hired_hand_related() = 0;                               // 81, checks ai_func, gets triggered just after throwing hired hand
    virtual void generate_fall_poof_particles() = 0;                     // 82, entity.velocityy must be < -0.12 to generate a poof, might do other stuff regarding falling/landing
    /// Applies gravity to entity. Disable to float like on hoverpack.
    virtual void handle_fall_logic() = 0;                                    // 83, adjusts entity.velocityy when falling
    virtual void apply_friction() = 0;                                       // 84, applies entity.type.friction to entity.velocityx
    virtual void boss_related() = 0;                                         // 85, when disabled, quillback keeps stomping through the level, including border tiles
    virtual void v86() = 0;                                                  // 86, triggers when tusk is angered, calls get_last_owner_uid
    virtual void gravity_related() = 0;                                      // 87
    virtual void v88() = 0;                                                  // 88
    virtual void stack_plus_28_is_0() = 0;                                   // 89, unknown; triggers on item_rubble
    virtual void on_crushed_by(Entity*) = 0;                                 // 90, e.g. crushed by elevator, punishball, pushblock, crushtrap (not quillback or boulder)
    virtual void on_fall_onto(uint32_t unknown, Entity* fell_on_entity) = 0; // 91
    virtual void on_instakill_death() = 0;                                   // 92, seems to only trigger for enemies that die in one hit
};
