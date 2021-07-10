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

class Flame : public Movable
{
  public:
    SoundPosition* sound_pos;
    Illumination* emitted_light;
};

class FlameSize : public Flame
{
  public:
    float flame_size; // if changed, gradually goes down (0.03 per frame) to the default size
};

class ClimbableRope : public Movable
{
  public:
    uint32_t segment_nr_inverse;
    int32_t burn_timer; // entity is killed after 20
    Entity* above_part;
    Entity* below_part;
    uint32_t segment_nr;
    uint32_t unused;
    uint64_t unknown;
};

class Idol : public Movable
{
  public:
    bool trap_triggered; // if you set it to true for the ice caves or volcano idol, the trap won't trigger
    int8_t touch;        // changes to 0 when first picked up by player and back to -1 if HH picks it up
    uint16_t unused;
    float spawn_x;
    float spawn_y;
};

class Spear : public Movable
{
  public:
    uint32_t sound_id;
};

class JungleSpearCosmetic : public Movable
{
  public:
    float move_x;
    float move_y;
};

struct UnknownPointerGroup
{
    uint32_t unknown_uid1;
    uint32_t unknown_uid2;
    uint32_t unknown_uid3;
};

class WebShot : public Movable
{
  public:
    UnknownPointerGroup unknown1;
    float unknown2;
    bool shot; // if false, it's attached to the gun
};

class HangStrand : public Movable
{
  public:
    float start_pos_y;
};

class HangAnchor : public Movable
{
  public:
    int32_t spider_uid;
};

class Arrow : public Movable
{
  public:
    int32_t flame_uid;
    bool is_on_fire;
    bool is_poisoned;
    bool shot_from_trap;
    int8_t unused;
};

class LightArrow : public Arrow
{
  public:
    Illumination* emitted_light;
};

class LightShot : public Movable
{
  public:
    UnknownPointerGroup unknown1;
    Illumination* emitted_light;
};

class LightEmitter : public Movable
{
  public:
    Illumination* emitted_light;
};

class ScepterShot : public LightEmitter
{
  public:
    SoundPosition* sound_pos;
    float speed;
    uint16_t idle_timer; // short timer before it goes after target
    int16_t unknown;     // setting it low or high can make it last forever, won't matter if you change it back afterwards, anubis is 300 and player 400 by default
};

class SpecialShot : public LightEmitter
{
  public:
    float target_x;
    float target_y;
};

class SoundShot : public LightShot
{
  public:
    SoundPosition* sound_info;
};

class Spark : public Flame
{
  public:
    ParticleEmitterInfo* particle;
    Entity* fx;
    float rotation_center_x;
    float rotation_center_y;
    float angle;
    float particle_size; /* unsure */ // slowly goes down to default 1.0, is 0.0 when not on screen
    float size_multiply;              // 0.0 when not on screen
    float next_size;                  // width and height will be set to this value * size_multiply next frame
    uint8_t state;                    /* unsure */
};

class TiamatShot : public LightEmitter
{
  public:
    SoundPosition* sound_info;
};

class Fireball : public SoundShot
{
  public:
    ParticleEmitterInfo* particle;
};

class Leaf : public Movable
{
  public:
    int64_t unknown1;
    float fade_away_counter; // counts to 100.0 then the leaf fades away
    int32_t swing_direction;
    bool fade_away_trigger; // if set true, it will fade away after a while
};

class AcidBubble : public Movable
{
  public:
    float speed_x;
    float speed_y;
    float float_counter;
};

class Claw : public Movable
{
  public:
    int32_t crabman_uid;
    float spawn_x;
    float spawn_y;
};

class StretchChain : public Movable
{
  public:
    int32_t at_end_of_chain_uid;
    float stretch;                  /* unsure */
    uint8_t position_in_chain;      // 0 closest to the end, so if the chain has 15 segments the last one will be 14
    uint8_t state; /* unsure*/      // only punish chain
    uint8_t unknown_timer;          // only punish chain
    bool is_chain_overlapping_ball; // only punish chain
};
