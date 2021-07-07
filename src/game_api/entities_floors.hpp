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

class ExitDoor : public Door
{
  public:
    bool entered;      // if true entering it does not load the transition, probably for use in multiplayer
    bool special_door; // use provided world/level/theme
    uint8_t level;
    uint8_t timer; // counts from 30 to 0, dunno why
    uint8_t world;
    uint8_t theme;
    uint16_t padding;
};

class DecoratedDoor : public ExitDoor
{
  public:
    Entity* special_bg;
};

class LockedDoor : public Door
{
  public:
    bool unlocked;
};

class CityOfGoldDoor : public DecoratedDoor
{
  public:
    bool unlocked;
    uint8_t unknown1;
};

class MainExit : public ExitDoor
{
  public:
    SoundPosition* sound_pos;
    float top_value; // unsure
    float value;     // unsure
};

class EggShipDoor : public Door
{
  public:
    uint16_t unused1;
    uint8_t unused2;
    uint8_t timer; // counts from 30 to 0, dunno why
    uint16_t unknown3;
    uint16_t unknown4;
    bool entered; // works only for DOOR_EGGSHIP
};

class Arrowtrap : public Entity
{
  public:
    size_t unknown1;
    size_t unknown2;
    bool arrow_shot;

    void rearm();
};
