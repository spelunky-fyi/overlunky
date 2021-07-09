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
