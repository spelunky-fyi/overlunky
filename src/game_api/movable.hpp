#pragma once

#include "entity.hpp"
#include "state_structs.hpp"

#include <functional>

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
    float jump_height_multiplier;
    int32_t price;
    int32_t owner_uid;
    int32_t last_owner_uid;
    size_t animation_func;
    uint32_t idle_counter;
    int32_t standing_on_uid;
    float velocityx;
    float velocityy;
    int32_t holding_uid;
    uint8_t state;
    uint8_t last_state;
    uint8_t move_state;
    uint8_t health;
    uint16_t stun_timer;
    uint16_t stun_state;
    uint32_t some_state;
    int16_t poison_tick_timer;
    uint8_t dark_shadow_timer;
    uint8_t exit_invincibility_timer;
    uint8_t invincibility_frames_timer;
    uint8_t frozen_timer;
    uint8_t unknown_damage_counter_a;
    uint8_t unknown_damage_counter_b;
    uint8_t i120a;
    uint8_t i120b;
    uint8_t i120c;
    uint8_t i120d;
    uint8_t b124;
    uint8_t airtime;
    uint8_t b126;
    uint8_t b127;

    virtual bool can_jump() = 0;
    virtual void v37() = 0;
    virtual float sprint_factor() = 0;
    virtual void calculate_jump_height() = 0; // when disabled, jump height is very high
    virtual std::unordered_map<uint8_t, Animation>& get_animation_map() = 0;
    virtual void apply_velocity(float* velocities) = 0; // param is pointer to an array of two floats: velocity x and y
    virtual int8_t stomp_damage() = 0;                  // calculates the amount of stomp damage applied (checks spike shoes, movable.state and stand_counter resulting in different damage values)
    virtual int8_t stomp_damage_trampoline() = 0;       // simply jumps to the 43rd virtual function, aka stomp_damage...
    virtual void adjust_health(int8_t amount) = 0;      // called for dealing damage (neg amount) by the game, but not for gaining health (turkey, motherstatue), even though that works too (and there is even a max health check of 0x63)
    virtual void v45() = 0;
    virtual void v46() = 0;
    virtual void handle_flying_object_collision(Entity* victim) = 0;                                          // stuff like flying rocks, broken arrows hitting the player
    virtual void handle_regular_damage(Entity* damage_dealer, int8_t damage_amount, uint8_t unknown = 1) = 0; // disable for regular damage invincibility; does not handle crush deaths (boulder, quillback, ghost); unknown param = 2 when hired hand gets hit by player
    virtual void handle_stun_damage(Entity* damage_dealer) = 0;                                               // triggers for broken arrow hit, calls handle_regular_damage with 0 damage; unsure about functionality and name
    virtual void v50() = 0;
    virtual void stun(uint16_t framecount) = 0;
    virtual void freeze(uint8_t framecount) = 0;
    virtual void light_on_fire() = 0;
    virtual void set_cursed(bool b) = 0;
    virtual void handle_spiderweb_collision() = 0;
    virtual void set_last_owner_uid_b127(Entity* owner) = 0; // assigns player as last_owner_uid and also manipulates movable.b127
    virtual uint32_t get_last_owner_uid() = 0;               // for players, it checks !stunned && !frozen && !cursed && !has_overlay; for others: just returns last_owner_uid
    virtual void check_out_of_bounds() = 0;                  // kills with the 'still falling' death cause
    virtual Entity* standing_on() = 0;                       // looks up movable.standing_on_uid in state.instance_id_to_pointer
    virtual void stomped_on_by(Entity* stomper) = 0;
    virtual void thrown_by(Entity* thrower) = 0;             // implemented for special cases like hired hand (player with ai_func), horned lizard...
    virtual void handle_clonegunshot_hit(Entity* clone) = 0; // implemented for player/hired hand: copies health to clone etc
    virtual uint32_t get_type_id() = 0;
    virtual bool doesnt_have_spikeshoes() = 0;
    virtual bool is_player_mount_or_monster() = 0;
    virtual void pick_up(Entity* entity_to_pick_up) = 0;
    virtual void picked_up_by(Entity* entity_picking_up) = 0;
    virtual void drop(Entity* entity) = 0; // also used when throwing
    virtual void collect_treasure(uint32_t treasure_value) = 0;
    virtual void apply_movement() = 0;              // disable this function and things can't move, some spin in place
    virtual void damage_entity(Entity* victim) = 0; // can't trigger, maybe extra params are needed
    virtual bool is_monster_or_player() = 0;
    virtual void initialize() = 0;                       // e.g. cobra: set random spit_timer; bat: set random stand_counter; emerald: set price
    virtual void check_is_falling() = 0;                 // sets more_flags.falling by comparing velocityy to 0
    virtual void handle_stun_transition_animation() = 0; // e.g. the wiggle the dog does when waking up from being stunned
    virtual void process_input() = 0;                    // unsure of params
    virtual void post_collision_damage_related() = 0;
    virtual void picked_up() = 0;                    // gets called after picked_up_by
    virtual void hired_hand_related() = 0;           // checks ai_func, gets triggered just after throwing hired hand
    virtual void generate_fall_poof_particles() = 0; // entity.velocityy must be < -0.12 to generate a poof, might do other stuff regarding falling/landing
    virtual void handle_fall_logic() = 0;            // adjusts entity.velocityy when falling
    virtual void apply_friction() = 0;               // applies entity.type.friction to entity.velocityx
    virtual void boss_related() = 0;                 // when disabled, quillback keeps stomping through the level, including border tiles
    virtual void v84() = 0;                          // triggers when tusk is angered, calls get_last_owner_uid
    virtual void gravity_related() = 0;
    virtual void v86() = 0;

    void poison(int16_t frames); // 1 - 32767 frames ; -1 = no poison
    bool is_poisoned();

    bool is_button_pressed(uint32_t button);
    bool is_button_held(uint32_t button);
    bool is_button_released(uint32_t button);

    std::uint32_t set_pre_statemachine(std::function<bool(Movable*)> pre_state_machine);
    std::uint32_t set_post_statemachine(std::function<void(Movable*)> post_state_machine);
};

class Player : public Movable
{
  public:
    std::map<int64_t, int64_t> inside;
    Inventory* inventory_ptr;
    Illumination* emitted_light;
    int32_t linked_companion_child;  // entity uid
    int32_t linked_companion_parent; // entity uid
    size_t ai_func;
    size_t input_ptr;
    size_t p160;
    int32_t i168;
    int32_t i16c;
    uint32_t jump_flags;
    uint8_t some_timer;
    uint8_t can_use;
    uint8_t b176;
    uint8_t b177;

    void set_jetpack_fuel(uint8_t fuel);
    uint8_t kapala_blood_amount();

    /// Get the full name of the character, this will be the modded name not only the vanilla name.
    std::u16string get_name();
    /// Get the short name of the character, this will be the modded name not only the vanilla name.
    std::u16string get_short_name();
    /// Get the heart color of the character, this will be the modded heart color not only the vanilla heart color.
    Color get_heart_color();
    /// Check whether the character is female, will be `true` if the character was modded to be female as well.
    bool is_female();

    /// Set the heart color the character.
    void set_heart_color(Color color);
};

std::u16string get_character_name(int32_t type_id);
std::u16string get_character_short_name(int32_t type_id);
Color get_character_heart_color(int32_t type_id);
bool is_character_female(int32_t type_id);

void set_character_heart_color(int32_t type_id, Color color);

class PlayerTracker
{
  public:
    size_t __vftable;
    size_t unknown1; // points to a CodePointer
    Entity* parent_entity_pointer;
    uint64_t unknown3;
    uint64_t unknown4;
    uint64_t unknown5;
    uint32_t unknown6;
    uint32_t unknown7;
    int32_t affected_entity_uid;
    uint16_t animation_frame; // changes the color of the eyeball on Lahamu
    uint8_t unknown9c;
    uint8_t unknown9d;
    float offset_x;
    float offset_y;
    float scale_x;
    float scale_y;
    float unknown_dupe_offset_x;
    float unknown_dupe_offset_y;
    float unknown10;
    float unknown11;
    float unknown12;
    float unknown13;
    float unknown14;
    float unknown15;
    float unknown16;
    float unknown17;
    uint8_t unknown18; // related to unknown14,15,16,17
    uint8_t unknown19;
    uint8_t unknown20;
    uint8_t unknown21;
    uint32_t unknown22;
    RenderInfo* unknown23;
    Texture* texture;
    float texture_offset_x;
    float texture_offset_y;
};
