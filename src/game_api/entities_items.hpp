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
    uint8_t shine_timer;
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
    uint8_t unused1;
    uint16_t unused2;
    Illumination* emitted_light;
    ParticleEmitterInfo* particles_smoke;
    ParticleEmitterInfo* particles_flames;
    ParticleEmitterInfo* particles_warp;
    SoundPosition* sound_pos;
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

class Chest : public Movable
{
  public:
    bool leprechaun;
    bool bomb; // size of the bomb is random, if set both true only leprechaun spawns
};

class Treasure : public Movable
{
  public:
    uint32_t state; /* unsure */
    bool cashed;    // spawns a dust effect
    int8_t unknown1;
    int16_t unknown2;
};

class HundunChest : public Treasure
{
  public:
    uint16_t unknown3;
    uint8_t unknown4;
    uint8_t timer;
};

class Boombox : public Movable
{
  public:
    Entity* fx_button;
    ParticleEmitterInfo* music_note1;
    ParticleEmitterInfo* music_note2;
    float spawn_y;
    int8_t station; // -1 to turn off and bring back normal camp music, anything above 10 is mute
    uint8_t station_change_delay;
    uint8_t jump_timer;
    uint8_t jump_state;
};

class TV : public Movable
{
  public:
    SoundPosition* sound_pos;
    Entity* fx_button;
    Illumination* emitted_light;
    uint8_t station; // 0 to turn off, anything above 3 has the music from station 3 and the screen doesn't change
};

class Telescope : public Movable
{
  public:
    Entity* fx_button;
    Entity* camera_anchor;
    int32_t looked_through_by_uid;
    float unknown;
};

class Torch : public Movable
{
  public:
    int32_t flame_uid;
    bool is_lit;
    int8_t unknown1;
    int16_t unknown2;
};

class WallTorch : public Torch
{
  public:
    bool no_reward; /* unsure */ // if false, it will drop gold when lit up
};

class TorchFlame : public Flame
{
  public:
    ParticleEmitterInfo* smoke_particle;
    ParticleEmitterInfo* flame_particle;
    ParticleEmitterInfo* warp_particle;
    float flame_size;
};

class LampFlame : public Flame
{
  public:
    ParticleEmitterInfo* flame_particle;
};

class Bullet : public Movable
{
  public:
    UnknownPointerGroup unknown1;
};

class TimedShot : public LightShot
{
  public:
    uint8_t timer;
};

class CloneGunShot : public TimedShot
{
  public:
    int8_t unused1;
    int16_t unused2;
    float spawn_y;
    float float_timer; /* unsure */
};

class PunishBall : public Movable
{
  public:
    int32_t attached_to_uid;
    uint8_t state; /* unsure */
    uint8_t unused1;
    uint16_t unused2;
    float x_pos;
    float y_pos;
};

class Chain : public Movable
{
  public:
    int32_t attached_to_uid;
    int32_t timer;
};

class Container : public Movable
{
  public:
    int32_t inside;
};

class Coffin : public Container
{
  public:
    int8_t timer;
    int8_t shake_state; /* unsure */
};

class Fly : public Movable
{
  public:
    uint8_t timer;
};

class OlmecCannon : public Movable
{
  public:
    uint16_t timer;
    uint8_t bombs_left;
};

class Landmine : public LightEmitter
{
  public:
    int32_t timer; // explodes at 57, if you set it to 58 will count to overflow
};

class Honey : public Movable
{
  public:
    uint8_t wiggle_timer;
};

class PlayerGhost : public LightEmitter
{
  public:
    ParticleEmitterInfo* sparkles_particle;
    PlayerInputs* player_inputs;
    Inventory* inventory;
    SoundPosition* sound_pos;
    uint16_t unknown_timer;
    uint8_t shake_timer;
    uint8_t boost_timer;
    int32_t unused; /* unsure */
    int64_t unknown;
};

class GhostBreath : public Movable
{
  public:
    UnknownPointerGroup unknown1;
    uint16_t timer;
    bool big_cloud;
    bool expand; /* unsure*/ // when set true on start makes it go further
};

class LaserBeam : public Movable
{
  public:
    ParticleEmitterInfo* sparks;
    Illumination* emitted_light;
};

class TreasureHook : public Movable
{
  public:
    SoundPosition* sound_pos;
};

class AxolotlShot : public Movable
{
  public:
    UnknownPointerGroup unknown1;
    int32_t trapped_uid;
    float size;
    float swing;             // always swings to the right first
    float swing_periodicity; // how often switch from right to left swing
    float distance_after_capture;
    uint8_t unknown2;
    uint8_t unknown3;
    uint16_t unknown4;
};

class TrapPart : public Movable
{
  public:
    Entity* ceiling;
};

class SkullDropTrap : public Movable
{
  public:
    SoundPosition* sound_pos;
    int32_t left_skull_uid;
    int32_t middle_skull_uid;
    int32_t right_skull_uid;
    uint8_t left_skull_drop_time;
    uint8_t middle_skull_drop_time;
    uint8_t right_skull_drop_time;
    uint8_t timer; // counts from 60 3 times, the last time dropping the skulls, then random longer timer for reset
};

class FrozenLiquid : public Movable
{
  public:
    int32_t unknown1;
    int32_t unknown2;
};

class Switch : public Movable
{
  public:
    uint8_t timer; // has to be 0 before next use
};

class FlyHead : public Movable
{
  public:
    int32_t vored_entity_uid;
};

class SnapTrap : public Movable
{
  public:
    int32_t bait_uid;
    uint8_t reload_timer;
};

class EmpressGrave : public Movable
{
  public:
    Entity* fx_button;
    Entity* ghost;
};

class Tentacle : public Chain
{
  public:
    Entity* bottom;
};

class MiniGameShip : public Movable
{
  public:
    SoundPosition* sound_pos;
    float velocity_x;
    float velocity_y;
    float swing;
    float up_down_normal; // 0.0 means down, 1.0 means up, 0.5 idle
};

class MiniGameAsteroid : public Movable
{
  public:
    float spin_speed;
};

class Pot : public Container
{
  public:
    bool dont_transfer_dmg; // if false, spawned entity will receive dmg that the pot received
};

class CursedPot : public Movable
{
  public:
    ParticleEmitterInfo* smoke;
    ParticleEmitterInfo* smoke2;
};

class Orb : public Movable
{
  public:
    SoundPosition* sound_pos;
    uint8_t timer;
};

class EggSac : public Movable
{
  public:
    uint8_t timer;
};

class Goldbar : public Movable
{
  public:
    uint8_t unknown; // get's updated every time animation_frame is changes by the game, setting it to 64 or less disables the effect
};

class Coin : public Movable
{
  public:
    uint32_t nominal_price;
};

class RollingItem : public Movable
{
  public:
    float roll_speed; // only positive numbers
};

class PlayerBag : public Movable
{
  public:
    uint8_t bombs;
    uint8_t ropes;
};

class ParachutePowerup : public Movable
{
  public:
    uint8_t falltime_deploy; // this gets compared with users falling_timer
    uint8_t unknown1;
    uint16_t unknown2;
    uint16_t unknown3;
    bool deploy;       // if you set only this one it will crash the game after a while, also won't add new parachute to inventory
    bool deploy2;      /* unsure */
    bool after_deploy; // if set to true before deploying, parachute will drop imminently after deploy
};

class TrueCrownPowerup : public Movable
{
  public:
    uint16_t timer;
};

class AnkhPowerup : public Movable
{
  public:
    SoundPosition* sound_pos;
    Entity* player;
    Entity* fx_glow;
    float unknown1;
    uint16_t timer1;
    uint16_t timer2;
    uint16_t timer3;
    bool music_on_off;
};

class YellowCape : public Cape
{
  public:
    SoundPosition* sound_pos;
};

class Teleporter : public Movable
{
  public:
    uint16_t teleport_number; // max 3, need to stand on the ground to reset
};

class Boomerang : public Movable
{
  public:
    SoundPosition* sound_pos;
    ParticleEmitterInfo* trail;
    float distance;
    float rotation;
    float wall_collision; /* unsure */
    uint8_t state;        /* unsure */
    uint8_t unused1;
    uint16_t unused2;
    int32_t returns_to_uid; // entity to return to
    uint16_t unknown1;      // something when the boomerang can't get to you
    uint8_t unknown2;
};

class Excalibur : public Movable
{
  public:
    bool in_stone;
};

class Shield : public Movable
{
  public:
    float shake;
};
