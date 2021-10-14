#pragma once

#include "entity.hpp"
#include "state_structs.hpp"
#include <cstdint>

class Liquid : public Entity
{
  public:
    Entity* fx_surface;
    float x_pos;
    float y_pos;
    size_t unknown; // MysteryLiquidPointer1 in plugin
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
