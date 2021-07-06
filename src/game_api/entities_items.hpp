#pragma once

#include "movable.hpp"
#include "particles.hpp"
#include <cstdint>

class Backpack : public Movable
{
  public:
    bool explosion_trigger;
    uint8_t explosion_timer; // counts from 0 to 30
    uint16_t unknown1;
    uint32_t unknown2;
};

class Jetpack : public Backpack
{
  public:
    bool flame_on; // Can be used as is_on
    uint8_t unknown1;
    uint16_t unknown2;
    uint32_t fly_time; // it's per level, not even per jetpack lol, it also adds at when it explodes
    uint16_t fuel;     // only set the fuel for an equipped jetpack (player->items)!
};

class Hoverpack : public Backpack
{
  public:
    SoundPosition* sound_pos;
    bool is_on;
};

class Bomb : public Movable
{
  public:
    size_t unknown1;
    float scale_hor; // 1.25 = default regular bomb ; 1.875 = default giant bomb ; > 1.25 generates ENT_TYPE_FX_POWEREDEXPLOSION
    float scale_ver;
    bool unknown2;
};

class Cape : public Movable
{
  public:
    size_t unknown;
    bool floating_down;
    uint8_t padding1;
    uint8_t padding2;
    uint8_t padding3;
    uint32_t floating_count; // increments whenever floating
};

class VladsCape : public Cape
{
  public:
    bool can_double_jump;
};

class KapalaPowerup : public Movable
{
  public:
    uint8_t amount_of_blood;
};

class Mattock : public Movable
{
  public:
    int32_t remaining;
};

class Gun : public Movable
{
  public:
    uint8_t cooldown;
    uint8_t shots;  // used only for webgun
    uint8_t shots2; // used only for clonegun
    uint8_t b12b;
};

class WebGun : public Gun
{
  public:
    int32_t in_chamber;
};

class UdjatSocket : public Movable
{
  public:
    Entity* fx_button;
};

class Ushabti : public Movable
{
  public:
    uint16_t wiggle_timer;
};

class GiantClamTop : public Movable
{
  public:
    int8_t close_timer;
    uint8_t open_timer;
};

class CookFire : public Movable
{
  public:
    int32_t unknown1;
    bool lit;
    uint8_t unknown3;
    uint8_t unknown4;
    uint8_t unknown5;
    Illumination* emitted_light;
    ParticleEmitterInfo* particles_smoke;
    ParticleEmitterInfo* particles_flames;
    ParticleEmitterInfo* particles_warp;
    size_t unknown10;
};
