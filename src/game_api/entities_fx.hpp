#pragma once

#include "movable.hpp"
#include <cstdint>

class LavaGlow : public Movable
{
  public:
    float glow_radius;
    float unknown_float;
    float increment; // just an on frame increment for the value above
};

class OlmecFloater : public Movable
{
  public:
    bool both_floaters_intact; // strangely, this indicates whether both are intact, not just this specific one
};
