#pragma once

#include "entity.hpp"
#include <cstdint>

class Liquid : public Entity
{
  public:
    Entity* fx_surface;
    size_t unknown;
    uint16_t unknown_readonly1;
    uint16_t unknown_readonly2;
    uint16_t unknown_timer1;
    uint16_t unknown_timer2;
};

class Lava : public Liquid
{
  public:
    Illumination* emitted_light;
};
