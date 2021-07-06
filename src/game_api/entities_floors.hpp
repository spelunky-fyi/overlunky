#pragma once

#include "Entity.hpp"
#include <cstdint>

class Floor : public Entity
{
  public:
    int32_t deco_top;
    int32_t deco_bottom;
    int32_t deco_left;
    int32_t deco_right;
};
