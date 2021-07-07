#pragma once

#include "entity.hpp"
#include <cstdint>

enum FLOOR_SIDE
{
    TOP,
    BOTTOM,
    LEFT,
    RIGHT
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

    /// Used to add decoration to a floor entity after it was spawned outside of level gen, is not necessary when spawning during level gen.
    void add_decoration(FLOOR_SIDE side);
    void fix_decorations(bool fix_also_neighbors, bool fix_styled_floor);
    void remove_decoration(FLOOR_SIDE side);

    void add_decoration_opt(FLOOR_SIDE side, int32_t decoration_entity_type, struct Layer* layer_ptr);

    bool is_styled();
    int32_t get_decoration_entity_type();
    uint8_t get_decoration_animation_frame(FLOOR_SIDE side);
};
