#pragma once

#include "entity.hpp"
#include <cstdint>

enum FLOOR_SIDE
{
    TOP,
    BOTTOM,
    LEFT,
    RIGHT,
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT,
};

class Floor : public Entity
{
  public:
    union
    {
        int32_t decos[4];
        struct
        {
            int32_t deco_top;
            int32_t deco_bottom;
            int32_t deco_left;
            int32_t deco_right;
        };
    };

    void fix_border_tile_animation();

    /// Used to add decoration to a floor entity after it was spawned outside of level gen, is not necessary when spawning during level gen.
    void fix_decorations(bool fix_also_neighbors, bool fix_styled_floor);
    void add_decoration(FLOOR_SIDE side);
    void remove_decoration(FLOOR_SIDE side);

    Entity* find_corner_decoration(FLOOR_SIDE side);
    void add_decoration_opt(FLOOR_SIDE side, int32_t decoration_entity_type, struct Layer* layer_ptr);

    bool has_corners() const;
    bool is_styled() const;
    int32_t get_decoration_entity_type() const;
    uint8_t get_decoration_animation_frame(FLOOR_SIDE side) const;

    static FLOOR_SIDE get_opposite_side(FLOOR_SIDE side);
    static bool get_perpendicular_sides(FLOOR_SIDE side, FLOOR_SIDE (&perp_sides)[2]);
    static bool get_corner_sides(FLOOR_SIDE side, FLOOR_SIDE (&corner_sides)[2]);
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
