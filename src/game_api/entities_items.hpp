#pragma once

#include "entities_chars.hpp"
#include "movable.hpp"
#include "particles.hpp"
#include <cstdint>

class Backpack : public Movable
{
  public:
    /// More like on fire trigger, the explosion happens when the timer reaches > 29
    bool explosion_trigger;
    uint8_t explosion_timer; // counts from 0 to 30
    uint16_t unknown1;
    uint32_t unknown2;
};

class Jetpack : public Backpack
{
  public:
    bool flame_on; // Can be used as is_on
    uint8_t unknown11;
    uint16_t unknown21;
    uint32_t fly_time; // it's per level, not even per jetpack lol, it also adds at when it explodes
    uint16_t fuel;     // only set the fuel for an equipped jetpack (player->items)!
};

class TeleporterBackpack : public Backpack
{
  public:
    uint16_t teleport_number;
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
    SoundPosition* sound_pos;
    /// 1.25 = default regular bomb, 1.875 = default giant bomb, > 1.25 generates ENT_TYPE_FX_POWEREDEXPLOSION
    float scale_hor;
    float scale_ver;
    /// is bomb from powerpack
    bool is_big_bomb;
};

class Cape : public Backpack
{
  public:
    bool floating_down;
    uint8_t padding1;
    uint8_t padding2;
    uint8_t padding3;
    uint32_t floating_count; // it's per level, not per cape
};

class VladsCape : public Cape
{
  public:
    bool can_double_jump;
};

class Mattock : public Movable
{
  public:
    uint8_t remaining;
};

class Gun : public Movable
{
  public:
    uint8_t cooldown;
    /// used only for webgun
    uint8_t shots;
    /// used only for clonegun
    uint8_t shots2;
    uint8_t b12b;
    /// Only for webgun, uid of the webshot entity
    int32_t in_chamber;
};

class WebGun : public Gun
{
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

class Flame : public Movable
{
  public:
    SoundPosition* sound_pos;
    Illumination* emitted_light;
};

class FlameSize : public Flame
{
  public:
    /// if changed, gradually goes down |0.03 per frame| to the default size
    float flame_size;
};

class ClimbableRope : public Movable
{
  public:
    uint32_t segment_nr_inverse;
    /// entity is killed after 20
    int32_t burn_timer;
    Entity* above_part;
    Entity* below_part;
    uint32_t segment_nr;
    uint32_t unused;
    uint64_t unknown;
};

class Idol : public Movable
{
  public:
    /// if you set it to true for the ice caves or volcano idol, the trap won't trigger
    bool trap_triggered;
    /// changes to 0 when first picked up by player and back to -1 if HH picks it up
    int8_t touch;
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
    size_t unknown_uid1; // it's pointer to the uid
    size_t unknown_uid2;
    size_t unknown_uid3;
};

class WebShot : public Movable
{
  public:
    UnknownPointerGroup unknown1;
    float unknown2;
    /// if false, it's attached to the gun
    bool shot;
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
    /// short timer before it goes after target
    uint16_t idle_timer;
    int16_t unknown; // setting it low or high can make it last forever, won't matter if you change it back afterwards, anubis is 300 and player 400 by default
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
    Entity* fx_entity;
    float rotation_center_x;
    float rotation_center_y;
    float rotation_angle;
    /// slowly goes down to default 1.0, is 0.0 when not on screen
    float size;
    /// 0.0 when not on screen
    float size_multiply;
    /// width and height will be set to this value * size_multiply next frame
    float next_size;
    /// very short timer before next size change, giving a pulsing effect
    uint8_t size_change_timer;
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
    /// counts to 100.0 then the leaf fades away
    float fade_away_counter;
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
    float dot_offset;
    uint8_t position_in_chain;                 // 0 closest to the end, so if the chain has 15 segments the last one will be 14
    uint8_t inverse_doubled_position_in_chain; // only punish chain
    uint8_t unknown_timer;                     // only punish chain
    bool is_dot_hidden;                        // only punish chain
};

class Chest : public Movable
{
  public:
    bool leprechaun;
    /// size of the bomb is random, if set both true only leprechaun spawns
    bool bomb;
};

class Treasure : public Movable
{
  public:
    uint32_t unknown_state; /* unsure */
    /// spawns a dust effect and adds money for the total
    bool cashed;
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
    float unknown; // seams to be distance between camera_anchor and telescope, but it's off by a little
};

class Torch : public Movable
{
  public:
    int32_t flame_uid;
    /// It's used just to check, to light/extinguish use `light_up` function
    bool is_lit;
    int8_t unknown1;
    int16_t unknown2;

    virtual void light_up(bool lit) = 0;
    virtual std::pair<float, float>& v_94(std::pair<float, float>& value) = 0; // sets the value to some constant, runs on spawn
    virtual ENT_TYPE get_flame_type() = 0;
};

class CookFire : public Torch
{
  public:
    Illumination* emitted_light;
    ParticleEmitterInfo* particles_smoke;
    ParticleEmitterInfo* particles_flames;
    ParticleEmitterInfo* particles_warp;
    SoundPosition* sound_pos;
};

class WallTorch : public Torch
{
  public:
    /// if false, it will drop gold when light up
    bool dropped_gold;
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

class CloneGunShot : public LightShot
{
  public:
    uint8_t timer;
    int8_t unused1;
    int16_t unused2;
    float spawn_y;
    float float_timer; /* unsure */
};

class PunishBall : public Movable
{
  public:
    int32_t attached_to_uid;
    uint8_t unknown_state; /* unsure */
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
    ENT_TYPE inside;

    void set_on_open(std::uint32_t reserved_callback_id, std::function<void(Container*, Movable*)> on_open);
};

class Coffin : public Movable
{
  public:
    ENT_TYPE inside;
    uint8_t timer;
    int8_t shake_state; /* unsure */
    bool player_respawn;
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
    /// explodes at 57, if you set it to 58 will count to overflow
    int32_t timer;
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
    /// Is not set to -1 when crushed
    int32_t body_uid;
    uint16_t unknown_timer;
    uint8_t shake_timer;
    uint8_t boost_timer;
    uint8_t unknown_timer2;
    uint8_t padding1;
    uint8_t padding2;
    uint8_t padding3;
    int32_t padding4;
    size_t unknown;
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
    /// counts from 60 3 times, the last time dropping the skulls, then random longer timer for reset
    uint8_t timer;
};

class FrozenLiquid : public Movable
{
  public:
    int32_t unknown1; // have problem cheeking those, looks like unused memory, but then, it's always zeros on spawn, changing it can crash the game
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
    float swing; // angle change?
    /// 0.0 - down, 1.0 - up, 0.5 - idle
    float up_down_normal;
};

class MiniGameAsteroid : public Movable
{
  public:
    float spin_speed;
};

class Pot : public Movable
{
  public:
    ENT_TYPE inside;
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
    uint8_t unknown_shine; // get's updated every time animation_frame is changes by the game, setting it to 64 or less disables the effect
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

class Powerup : public Movable
{
  public:
    // thoes could be wrong becouse of the update
    virtual void apply_effect_to_player(Player* player) = 0;
    virtual void remove_effect_from_player(Player* player) = 0;
};

class KapalaPowerup : public Powerup
{
  public:
    uint8_t amount_of_blood;
};

class ParachutePowerup : public Powerup
{
  public:
    /// this gets compared with entity's falling_timer
    uint8_t falltime_deploy;
    uint8_t unknown1;
    uint16_t unknown2;
    uint16_t unknown3;
    bool deployed;                  // if you set only this one it will crash the game after a while, also won't add new parachute to inventory
    bool deployed2;                 /* unsure */
    bool after_deploy; /* unsure */ // if set to true before deploying, parachute will drop imminently after deploy, not sure if it's even a bool
    void deploy();
};

class TrueCrownPowerup : public Powerup
{
  public:
    uint16_t timer;
};

class AnkhPowerup : public Powerup
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
    float wall_collision;  /* unsure */
    uint8_t unknown_state; /* unsure */
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

class PrizeDispenser : public Movable
{
  public:
    /// Id's of the items (not types), by default 0-24, look at [change_diceshop_prizes](#change_diceshop_prizes) for the list of default prizes
    /// so for example: id 0 equals ITEM_PICKUP_BOMBBAG, id 1 equals ITEM_PICKUP_BOMBBOX etc. Game generates 6 but uses max 5 for Tusk dice shop
    std::array<uint8_t, 6> item_ids;
    uint8_t prizes_spawned;
    int8_t padding;
};
