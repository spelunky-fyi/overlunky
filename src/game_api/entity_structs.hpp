#pragma once

#include <cstdint> // for uint8_t, int32_t

enum class REPEAT_TYPE : uint8_t
{
    NoRepeat,
    Linear,
    BackAndForth,
};

enum class SHAPE : uint8_t
{
    RECTANGLE = 1,
    CIRCLE = 2,
};

struct Animation
{
    int32_t first_tile;
    // num_tiles
    int32_t count;
    int32_t interval;
    uint8_t id;
    REPEAT_TYPE repeat;
};

struct Rect
{
    float offsetx;
    float offsety;
    float hitboxx;
    float hitboxy;
};

struct CollisionInfo
{
    Rect rect;
    SHAPE shape;
    bool hitbox_enabled;
    uint8_t field_3A;
    uint8_t field_3B;
};
