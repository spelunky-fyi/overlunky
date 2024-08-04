#pragma once

#include "entities_items.hpp"
#include "entity.hpp"
#include "particles.hpp"

class Portal : public Entity
{
  public:
    UnknownPointerGroup UnknownPointerGroup;
    size_t unknown4;
    size_t unknown5;
    size_t unknown6;
    Illumination* emitted_light;
    SoundMeta* sound;
    float unknown7;
    uint8_t transition_timer;
    uint8_t level;
    uint8_t world;
    uint8_t theme;
    int32_t timer;
};

class ShootingStarSpawner : public Entity
{
  public:
    uint16_t timer;
};

class LogicalDoor : public Entity
{
  public:
    /// Spawns this entity when not covered by floor. Must be initialized to valid ENT_TYPE before revealed, or crashes the game.
    ENT_TYPE door_type;
    /// Spawns this entity below when tile below is uncovered. Doesn't spawn anything if it was never covered by floor, unless platform_spawned is set to false. Must be initialized to valid ENT_TYPE before revealed, or crashes the game.
    ENT_TYPE platform_type;
    /// Set automatically when not covered by floor.
    bool not_hidden;
    /// Set automatically when tile below is not covered by floor. Unset to force the platform to spawn if it was never covered in the first place.
    bool platform_spawned;
    bool unk5;
    bool unk6;
};

class LogicalSound : public Entity
{
  public:
    SoundMeta* sound;
};

class LogicalStaticSound : public LogicalSound
{
  public:
    float x_pos; /* unsure */
    float y_pos; /* unsure */
};

class LogicalLiquidStreamSound : public LogicalStaticSound
{
  public:
    float unknown; // soundpositionpointer unknown10
};

class LogicalTrapTrigger : public Entity
{
  public:
    /// Used in BigSpearTrap when it has to have minimum 2 free spaces to be able to trigger, value in tiles
    int32_t min_empty_distance;
    /// Value in tiles
    int32_t trigger_distance;
    bool vertical;
    int8_t padding1;
    int16_t padding2;
    int32_t padding3;
};

class JungleTrapTrigger : public LogicalTrapTrigger
{
  public:
    int32_t counter1; // one of those weird counters that counts all the traps, not only this single one
    int32_t counter2; // also there are 3 of them?
    int32_t counter3;
};

class WetEffect : public Entity
{
  public:
    ParticleEmitterInfo* particle;
};

class OnFireEffect : public Entity
{
  public:
    ParticleEmitterInfo* particle_smoke;
    ParticleEmitterInfo* particle_flame;
    Illumination* illumination;
    bool unknown;
};

class PoisonedEffect : public Entity
{
  public:
    ParticleEmitterInfo* particle_burst;
    ParticleEmitterInfo* particle_base;
    uint16_t burst_timer;
    /// If forced to false, it will not play the sound or spawn burst particles
    bool burst_active;
};

class CursedEffect : public Entity
{
  public:
    ParticleEmitterInfo* particle;
    SoundMeta* sound;
};

class OuroboroCameraAnchor : public Entity
{
  public:
    float target_x;
    float target_y;
    float velocity_x;
    float velocity_y;
};

class OuroboroCameraZoomin : public Entity
{
  public:
    /// Can be set to negative, seams to trigger the warp at some value
    float zoomin_level;
};

class CinematicAnchor : public Entity
{
  public:
    Entity* blackbar_top;
    Entity* blackbar_bottom;
    /// 0.0 to 1.0
    float roll_in;
};

class BurningRopeEffect : public Entity
{
  public:
    Illumination* illumination;
    SoundMeta* sound;
};

class DustWallApep : public Entity
{
  public:
    ParticleEmitterInfo* particle;
};

class CameraFlash : public Entity
{
  public:
    Illumination* illumination1;
    Illumination* illumination2;
    int64_t unknown;
    uint8_t timer;
};

class RoomLight : public Entity
{
  public:
    Illumination* illumination;
};

class LimbAnchor : public Entity
{
  public:
    float x_pos;
    float y_pos;
    float hand_feet_x_offset;
    float hand_feet_y_offset;
    float x_unk5; // all of those are offsets/positions of different joints, i'm too lazy to document them all
    float y_unk6; // if anyone wants to, go ahead
    float x_unk7;
    float y_unk8;
    float x_unk9;
    float y_unk10;
    float unk11;
    float unk12;
    float unk13;
    uint16_t move_timer;
    bool flip_vertical;
};

class LogicalConveyorbeltSound : public LogicalSound
{
  public:
    UnknownPointerGroup unknown; // why?
};

class LogicalAnchovyFlock : public Entity
{
  public:
    /// Increases until max_speed reached
    float current_speed;
    float max_speed;
    uint16_t timer;
};

class MummyFliesSound : public LogicalSound
{
  public:
    int32_t mummy_uid;
    /// Numbers of flies spawned
    uint8_t flies;
    int8_t padding1;
    int16_t padding2;
    UnknownPointerGroup unknown; // why? because of this this entity falls into the larger bucket
};

class QuickSandSound : public LogicalSound
{
  public:
    int32_t counter; // probably unsigned?
};

class IceSlidingSound : public LogicalSound
{
  public:
    int32_t counter; // probably unsigned?
};

class FrostBreathEffect : public Entity
{
  public:
    uint16_t timer;
};

class BoulderSpawner : public Entity
{
  public:
    /// Can be set negative for longer time period, spawns boulder at 150, setting it higher with count to overflow
    int32_t timer;
    int32_t padding; // unsure
    SoundMeta* sound;
};

class PipeTravelerSound : public LogicalSound
{
  public:
    float sound_speed; // unsure, soundpositionpointer -> VALUE
    int32_t counter;   // unsure, probably unsigned?
    int8_t unknown1;   // enter/exit sound time duration?
    int8_t unknown2;   // enter/exit sound time duration?
    bool enter_exit;
};

class LogicalDrain : public Entity
{
  public:
    /// Little delay between pulling blob of liquid thru
    uint8_t timer;
};

class LogicalRegeneratingBlock : public Entity
{
  public:
    uint8_t timer;
};

class SplashBubbleGenerator : public Entity
{
  public:
    uint8_t timer;
};

class EggplantThrower : public Entity
{
  public:
    bool unknown; // Allow throw again when it's false? hard to test
};

class LogicalMiniGame : public Entity
{
  public:
    /// Delay between spawning ufo
    uint16_t timer;
};

class DMSpawning : public Entity
{
  public:
    float spawn_x;
    float spawn_y;
    float sine_pos;
    uint8_t timer;
};

class DMAlienBlast : public Entity
{
  public:
    int32_t owner_uid;
    int32_t unknown1;
    Entity* fx_internal_reticule;
    Entity* fx_external_reticule;
    SoundMeta* sound;
    uint8_t timer;
};
