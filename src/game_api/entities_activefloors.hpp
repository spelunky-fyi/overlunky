#pragma once

#include <cstddef> // for size_t
#include <cstdint> // for uint8_t, int32_t, uint32_t, int8_t

#include "aliases.hpp"        // for ENT_TYPE
#include "entities_items.hpp" // for UnknownPointerGroup
#include "movable.hpp"        // for Movable

class Entity;
struct Illumination;
struct ParticleEmitterInfo;
struct SoundMeta;

// Seams like all activefloors have one virtual function, but it just `return` so no reason to add it, maybe "purchasable" ?

class Crushtrap : public Movable
{
  public:
    float dirx;
    float diry;
    /// counts from 30 to 0 before moving, after it stops, counts from 60 to 0 before it can be triggered again
    uint8_t timer;
    /// counts from 7 to 0 after it hits the wall and moves away until the timer hits 0, then moves back and counts from 255 until it hits the wall again, if needed it will start the counter again for another bounce
    uint8_t bounce_back_timer;
};

class Olmec : public Movable
{
  public:
    SoundMeta* sound;
    uint32_t target_uid;
    /// 0 = stomp, 1 = bombs, 2 = stomp+ufos, 3 = in lava
    uint8_t attack_phase;
    /// in phase 0/2: time spent looking for player, in phase 1: time between bomb salvo
    uint8_t attack_timer;
    /// general timer that counts down whenever olmec is active
    uint8_t ai_timer;
    /// -1 = left, 0 = down, 1 = right, phase 0/2: depends on target, phase 1: travel direction
    int8_t move_direction;
    uint8_t jump_timer;
    uint8_t phase1_amount_of_bomb_salvos;
    uint8_t unknown_attack_state; // some attack state in phase 1/2 (changes to 2 when stomping and 3 during ufos)

    uint8_t broken_floaters();
};

class WoodenlogTrap : public Movable
{
  public:
    int32_t ceiling_1_uid;
    int32_t ceiling_2_uid;
    float falling_speed;
    uint8_t unknown1; // it's forced to 0, for whatever reason
};

class Boulder : public Movable
{
  public:
    size_t unknown1;
    /// is set to 1 when the boulder first hits the ground
    uint8_t is_rolling;
};

class PushBlock : public Movable
{
  public:
    UnknownPointerGroup unknown1;
    SoundMeta* sound;
    ParticleEmitterInfo* dust_particle;
    float dest_pos_x;
    uint32_t unused;
};

class BoneBlock : public Movable
{
  public:
    float unknown1; // 1.0 when whipped from left and -1.0 when whipped from right
};

class ChainedPushBlock : public PushBlock
{
  public:
    bool is_chained; // changing this when chained does nothing, when unchained it's forced to 0
};

class LightArrowPlatform : public Movable
{
  public:
    uint64_t unused;
    Illumination* emitted_light;
};

class FallingPlatform : public Movable
{
  public:
    int32_t timer;
    float shaking_factor;
    float y_pos;
};

class UnchainedSpikeBall : public Movable
{
  public:
    bool bounce; // if false it bounces when hitting the ground, it's also forced to true when moving sideways or rolling, can be changed to true in the air for weird effect
};

class Drill : public Movable
{
  public:
    SoundMeta* sound1;
    SoundMeta* sound2;
    Entity* top_chain_piece;
    uint8_t unknown1; // it's forced to 0, for whatever reason

    void trigger(std::optional<bool> play_sound_effect);
};

class ThinIce : public Movable
{
  public:
    UnknownPointerGroup unknown1;
    /// counts down when standing on, maximum is 134 as based of this value it changes animation_frame, and above that value it changes to wrong sprite
    uint16_t strength;
};

class Elevator : public Movable
{
  public:
    Illumination* emitted_light;
    /// pause timer, counts down 60 to 0
    uint32_t timer;
    bool moving_up;
};

class ClamBase : public Movable
{
  public:
    ENT_TYPE treasure_type;
    int32_t treasure_uid; // set to nothing when spawning in overlunky
    float treasure_x_pos;
    float treasure_y_pos;
    int32_t top_part_uid;
};

class RegenBlock : public Movable
{
  public:
    bool on_breaking;
};

class TimedPowderkeg : public PushBlock
{
  public:
    /// timer till explosion, -1 = pause, counts down
    int32_t timer;
};

class CrushElevator : public Movable
{
  public:
    /// This is custom variable, you need [activate_crush_elevator_hack](#activate_crush_elevator_hack) to use it
    float y_limit;
    /// This is custom variable, you need [activate_crush_elevator_hack](#activate_crush_elevator_hack) to use it
    float speed;
};
