#pragma once

#include "entity.hpp"
#include "sound_manager.hpp"

struct Illumination;

class CrossBeam : public Entity
{
  public:
    int32_t attached_to_side_uid;
    int32_t attached_to_top_uid;
};

class PalaceSign : public Entity
{
  public:
    /// The neon buzz sound
    SoundMeta* sound;
    Illumination* illumination; // illumination1/2 used only in dark level, no pointer in normal level
    Illumination* arrow_illumination;
    uint8_t arrow_change_timer;
};

class DecoRegeneratingBlock : public Entity
{
  public:
    int8_t unknown1;
    uint8_t unknown2;   // timer after self_destruct is triggered, modifies the size depending of the unknown1
    bool self_destruct; // UNSURE have reverse effect of spawning in after the block is broken, unused in the game?
};

class DestructibleBG : public Entity
{
  public:
    int32_t attached_to_1; // weird, unsure
    int32_t attached_to_2; // weird, unsure
    bool unknown1;
    bool unknown2;
};
