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

    virtual void v36() = 0;
    virtual void v37() = 0;
    virtual float sprint_factor() = 0;
    virtual void v39() = 0;
    virtual void v40() = 0;
    virtual void v41() = 0;
    virtual void v42() = 0;
    virtual void v43() = 0;
    virtual void v44() = 0;
    virtual void v45() = 0;
    virtual void v46() = 0;
    virtual void v47() = 0;
    virtual void v48() = 0;
    virtual void v49() = 0;
    virtual void v50() = 0;
    virtual void v51() = 0;
    virtual void v52() = 0;
    virtual void v53() = 0;
    virtual void v54() = 0;
    virtual void v55() = 0;
    virtual void v56() = 0;
    virtual void v57() = 0;
    virtual void v58() = 0;
    virtual void v59() = 0;
    virtual void v60() = 0;
    virtual void v61() = 0;
    virtual void v62() = 0;
    virtual void v63() = 0;
    virtual void v64() = 0;
    virtual void v65() = 0;
    virtual void v66() = 0;
    virtual void v67() = 0;
    virtual void v68() = 0;
    virtual void v69() = 0;
    virtual void v70() = 0;
    virtual void v71() = 0;
    virtual void v72() = 0;
    virtual void v73() = 0;
    virtual void v74() = 0;
    virtual void v75() = 0;
    virtual void v76() = 0;
    virtual void v77() = 0;
    virtual void v78() = 0;
    virtual void v79() = 0;
    virtual void v80() = 0;
    virtual void v81() = 0;
    virtual void v82() = 0;
    virtual void v83() = 0;
    virtual void v84() = 0;
    virtual void v85() = 0;
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

    std::u16string get_name();
    std::u16string get_short_name();
    Color get_heart_color();
    bool is_female();

    void set_heart_color(Color color);
};

class OlmecFloater : public Movable
{
  public:
    bool both_floaters_intact; // strangely, this indicates whether both are intact, not just this specific one
};

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
    uint8_t animation_frame; // changes the color of the eyeball on Lahamu
    uint8_t unknown9b;
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
