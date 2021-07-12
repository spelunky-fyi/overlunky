#pragma once

#include "entities_items.hpp"
#include "entity.hpp"
#include "movable.hpp"
#include <cstdint>

class Crushtrap : public Movable
{
  public:
    float dirx;
    float diry;
};

class Olmec : public Movable
{
  public:
    size_t unknown_pointer;
    uint32_t target_uid;
    uint8_t attack_phase;  // 0 = stomp ; 1 = bombs ; 2 = stomp+ufos ; 3 = in lava
    uint8_t attack_timer;  // in phase 0/2: time spent looking for player ; in phase 1: time between bomb salvo
    uint8_t ai_timer;      // general timer that counts down whenever olmec is active
    int8_t move_direction; // -1 = left ; 0 = down ; 1 = right (phase 0/2: depends on target, phase 1: travel direction)
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
    uint8_t is_rolling; // is set to 1 when the boulder first hits the ground
};

class PushBlock : public Movable
{
  public:
    UnknownPointerGroup unknown1;
    SoundPosition* sound_pos;
    ParticleEmitterInfo* dust_particle;
    float dest_pos_x;
};

class BoneBlock : public Movable
{
  public:
    float unknown1; // 1.0 when hited from left and -1.0 when hited from right
};

class ChainedPushBlock : public PushBlock
{
  public:
    uint32_t unused;
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
    SoundPosition* sound_pos1;
    SoundPosition* sound_pos2;
    Entity* top_chain_piece;
    uint8_t unknown1; // it's forced to 0, for whatever reason
};

class ThinIce : public Movable
{
  public:
    UnknownPointerGroup unknown1;
    uint16_t strength; // counts frames when standing on, maximum is 134 as based of this value it changes animation_frame, and above that value it changes to wrong sprite
};

class Elevator : public Movable
{
  public:
    Illumination* emitted_light;
    uint32_t timer; // counts from 60 to 0
    bool moving_up;
};

class ClamBase : public Movable
{
  public:
    uint32_t treasure_type;
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
    uint32_t unused;
    int32_t timer; // timer till explosion, it's paused when negative, can be stooped
};
