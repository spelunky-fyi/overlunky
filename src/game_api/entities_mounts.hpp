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
    // size_t unknown1;
    // uint64_t unknown2;
    int32_t rider_uid; // who rides it
    uint32_t unknown4;
    SoundMeta* sound;
    bool can_doublejump; // whether the doublejump has already occurred or not
    bool tamed;
    uint16_t walk_pause_timer; // alternates between walking and pausing every time it reaches zero
    uint8_t unknown9a;
    uint8_t unknown9b;
    uint8_t taming_timer; // when 0 it's tame
    uint8_t unknown9d;

    void carry(Movable* rider);

    void tame(bool value)
    {
        tamed = value;
        flags = flags | 0x20000;
    }

    virtual Vec2& get_special_offset(Vec2& offset) = 0; // gets special offset for the raider when jumping on mount
    virtual Vec2& v96(Vec2& value) = 0;                 // gets something for when crouching on mount
    virtual bool used_double_jump() = 0;                // checks can_doublejump and unknown9b
    virtual uint32_t v98(bool) = 0;                     // returns some constant value
    virtual uint32_t v99() = 0;                         // returns some constant value
    virtual void play_jump_on_sound() = 0;              // checks if it has rider
    virtual void remove_rider() = 0;
    virtual float v102() = 0;                // get offset? mech returns 0.9, the rest 0.5
    virtual uint32_t v103() = 0;             // returns some constant value
    virtual uint32_t v104() = 0;             // returns some constant value
    virtual uint32_t v105() = 0;             // returns some constant value
    virtual bool can_play_mount_sound() = 0; // called every frame, if returns true mount will make a sound
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
