#pragma once

#include <cstddef>       // for size_t
#include <cstdint>       // for uint8_t, uint32_t, int32_t, uint16_t, int64_t
#include <functional>    // for function, equal_to
#include <span>          // for span
#include <string>        // for allocator, string
#include <string_view>   // for string_view
#include <tuple>         // for tuple
#include <type_traits>   // for move
#include <unordered_map> // for _Umap_traits<>::allocator_type, unordered_map
#include <utility>       // for pair
#include <vector>        // for vector

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
    int32_t count; // num_tiles
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
