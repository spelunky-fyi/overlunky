#pragma once

#include "entity.hpp"

class Liquid : public Entity
{
  public:
    Entity* fx_surface;
    float x_pos;
    float y_pos;
    std::list<int32_t>::const_iterator liquid_id; // the id's change all the time, but the iterator is static
    uint16_t unknown_readonly1;
    uint16_t unknown_readonly2;
    uint8_t unknown_timer1;
    uint8_t pos_update_timer; // when 0, updates x_pos and y_pos
    uint8_t unknown_timer3;
    uint8_t unk21; // probably padding

    uint32_t get_liquid_flags();
    void set_liquid_flags(uint32_t flags);
};

class Lava : public Liquid
{
  public:
    Illumination* emitted_light;
};
