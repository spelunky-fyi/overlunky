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

class Door : public Entity
{
  public:
    uint8_t counter; // counts down as you go thru it
    int8_t unused1;
    int16_t unused2;
    int32_t unused3;
    Entity* fx_button;

    void set_target(uint8_t w, uint8_t l, uint8_t t);
    std::tuple<uint8_t, uint8_t, uint8_t> get_target();
};

class Arrowtrap : public Entity
{
  public:
    size_t unknown1;
    size_t unknown2;
    bool arrow_shot;

    void rearm();
};
