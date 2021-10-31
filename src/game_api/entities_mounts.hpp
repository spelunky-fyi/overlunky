#pragma once

#include "movable.hpp"

class Mount : public Movable
{
  public:
    size_t unknown1;
    uint64_t unknown2;
    int32_t rider_uid; // who rides it
    uint32_t unknown4;
    SoundPosition* sound_pos;
    bool can_doublejump; // whether the doublejump has already occurred or not
    bool tamed;
    uint16_t walk_pause_timer; // alternates between walking and pausing every time it reaches zero
    uint8_t unknown9a;
    uint8_t unknown9b;
    uint8_t taming_timer; // when 0 it's tame
    uint8_t unknown9d;

    void carry(Movable* rider);

    void tame(bool value);
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
    SoundPosition* sound_position;
    SoundPosition* explosion_sound_pos;
    uint64_t unknown11;
    uint8_t gun_cooldown;
    uint8_t unknown21;
    bool walking;
    bool breaking_wall;
};

class Qilin : public Mount
{
  public:
    SoundPosition* sound_position;
    uint8_t attack_cooldown; // not actually used? you can fire again before it's 0
};
