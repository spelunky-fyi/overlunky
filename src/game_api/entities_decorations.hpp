#pragma once

#include "entity.hpp"

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
    SoundPosition* sound_pos;
    Illumination* illumination; // illumination1/2 used only in dark level, no pointer in normal level
    Illumination* arrow_illumination;
    uint8_t arrow_change_timer;
};

class DecoRegeneratingBlock : public Entity
{
  public:
    int8_t unknown1;
    uint8_t unknown2;   // timer after self_destruct is triggered, modifies the size depending of the unknown1
    bool self_destruct; // UNSURE have reverse effect of spawning in after the broke is blocken, unsued in the game?
};

class DestructibleBG : public Entity
{
  public:
    int32_t attached_to_1; // wierd, unsure
    int32_t attached_to_2; // wierd, unsure
    bool unknown1;
    bool unknown2;
};
