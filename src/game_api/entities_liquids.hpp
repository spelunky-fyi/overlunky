#pragma once

#include "entity.hpp"

class MysteryLiquid1
{
  public:
    size_t prev; // points to the previos element in this ... array?
    /* MysteryLiquid2* */ size_t unknown;
    int32_t liquid_id;
    int32_t unknown4; // garbage?
};

class Liquid : public Entity
{
  public:
    Entity* fx_surface;
    float x_pos;
    float y_pos;
    MysteryLiquid1* unknown1;
    uint16_t unknown_readonly1;
    uint16_t unknown_readonly2;
    uint8_t unknown_timer1;
    uint8_t unknown_timer2;
    uint8_t unknown_timer3;
    uint8_t unk21; //probably padding
};

class Lava : public Liquid
{
  public:
    Illumination* emitted_light;
};
