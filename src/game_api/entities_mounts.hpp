#pragma once

#include <cstdint> // for uint8_t, uint32_t, int32_t, uint16_t
#include <utility> // for pair

#include "entities_chars.hpp" // for PowerupCapable
#include "math.h"             // for Vec2

class Movable;
struct SoundMeta;

class Mount : public PowerupCapable
{
  public:
    int32_t rider_uid; // who rides it
    uint32_t padding1;
    SoundMeta* sound;
    bool can_doublejump; // whether the doublejump has already occurred or not
    bool tamed;
    uint16_t walk_pause_timer; // alternates between walking and pausing every time it reaches zero
    uint8_t unknown9a;
    bool double_jumping;  // used to play different animation for the double jump then the standard jump, is true for less than a frame
    uint8_t taming_timer; // when 0 it's tame
    uint8_t padding2;

    void carry(Movable* rider);

    void tame(bool value)
    {
        tamed = value;
        flags = flags | 0x20000;
    }

    virtual Vec2& get_rider_offset(Vec2& offset) = 0;          // 95
    virtual Vec2& get_rider_offset_crouching(Vec2& value) = 0; // 96
    virtual bool used_double_jump() = 0;                       // 97,  (can_doublejump | double_jumping)
    virtual SOUNDID get_jump_sound(bool double_jump) = 0;      // 98
    virtual SOUNDID get_attack_sound() = 0;                    // 99
    virtual void play_jump_on_sound() = 0;                     // 100, checks if it has rider etc. get's sound from 103 virtual
    virtual void remove_rider() = 0;                           // 101
    virtual float v102() = 0;                                  // 102, mech returns 0.9, the rest 0.5, related to distance at which the player can mount
    virtual SOUNDID get_mounting_sound() = 0;                  // 103
    virtual SOUNDID get_walking_sound() = 0;                   // 104
    virtual SOUNDID get_untamed_loop_sound() = 0;              // 105
    /// called every frame, if returns true mount will make a sound
    virtual bool can_play_mount_sound() = 0; // 106
};

class Rockdog : public Mount
{
  public:
    uint8_t attack_cooldown;
};

class Axolotl : public Mount
{
  public:
    uint8_t attack_cooldown;
    bool can_teleport;
    uint8_t unknown;
};

class Mech : public Mount
{
  public:
    SoundMeta* crouch_walk_sound;
    SoundMeta* explosion_sound;
    uint64_t unknown11;
    uint8_t gun_cooldown;
    uint8_t unknown21;
    bool walking;
    bool breaking_wall;
};

class Qilin : public Mount
{
  public:
    SoundMeta* fly_gallop_sound;
    uint8_t attack_cooldown; // not actually used? you can fire again before it's 0
};
