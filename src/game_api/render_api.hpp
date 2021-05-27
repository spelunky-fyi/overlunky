#pragma once

#include "memory.hpp"

struct Texture;

struct RenderAPI
{
    const size_t* api;
    size_t swap_chain_off;

    static RenderAPI get();

    size_t renderer() const
    {
        return read_u64(*api + 0x10);
    }

    size_t swap_chain() const
    {
        return read_u64(renderer() + swap_chain_off);
    }
};

// straight out of the x64dbg plugin
struct RenderInfo
{
    size_t __vftable;
    uint32_t unknown1;
    float x;
    float y;
    uint32_t unknown4;
    uint32_t unknown5;
    uint32_t unknown6;
    uint32_t unknown7;
    uint32_t unknown8;
    uint32_t unknown9;
    float x_dupe_1;
    float y_dupe_1;
    uint32_t unknown10;
    float x_dupe2;
    float y_dupe2;
    uint32_t unknown11;
    uint32_t unknown12;
    uint32_t unknown13;
    uint32_t unknown14;
    uint8_t unknown15_counter;
    uint8_t unknown16_counter;
    bool unknown17;
    bool unknown18;
    uint32_t unknown19;
    uint32_t unknown20;
    float bottom_left_x; // entity.x - (entity.w/2)
    float bottom_left_y; // entity.y - (entity.h/2)
    float bottom_left_unknown;
    float bottom_right_x;
    float bottom_right_y;
    float bottom_right_unknown;
    float top_right_x;
    float top_right_y;
    float top_right_unknown;
    float top_left_x;
    float top_left_y;
    float top_left_unknown;
    float unknown21; // unknown 21-28 are related to the texture pointer info (see below in entity.p80.texture)
    float unknown22;
    float unknown23;
    float unknown24;
    float unknown25;
    float unknown26;
    float unknown27;
    float unknown28;
    float unknown29;
    float unknown30;
    float unknown31;
    float unknown32;
    float unknown33;
    float unknown34;
    uint32_t unknown35; // changes when climbing
    uint32_t unknown36; // changes when climbing
    uint32_t unknown37; // changes when climbing
    uint8_t animation_frame;
    uint8_t padding1;
    uint8_t padding2;
    uint8_t padding3;
    uint32_t unknown38;
    Texture* texture;
    const char** texture_name;
};
