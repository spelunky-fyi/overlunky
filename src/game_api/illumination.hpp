#pragma once

#include <array>
#include <cstdint>

#include "aliases.hpp"

struct Color;
struct Vec2;

enum class LIGHT_TYPE : uint8_t
{
    NONE = 0x0,
    FOLLOW_CAMERA = 0x1,
    FOLLOW_ENTITY = 0x2,
    ROOM_LIGHT = 0x4,
};

struct LightParams // it's probably just Color
{
    float red; // default = 1.0 (can go over 1.0 for oversaturation)
    float green;
    float blue;
    float size;

    /// Returns LightParams as Color, note that size = alpha
    Color* as_color()
    {
        return reinterpret_cast<Color*>(this);
    };
};

struct Illumination
{
    union
    {
        /// Table of light1, light2, ... etc.
        std::array<LightParams, 4> lights;
        struct
        {
            LightParams light1;
            LightParams light2;
            LightParams light3;
            /// It's rendered on objects around, not as an actual bright spot
            LightParams light4;
        };
    };
    float brightness;
    float brightness_multiplier;
    float light_pos_x;
    float light_pos_y;
    float offset_x;
    float offset_y;
    float distortion;
    int32_t entity_uid;
    uint32_t timer;
    union
    {
        /// see [flags.hpp](https://github.com/spelunky-fyi/overlunky/blob/main/src/game_api/flags.hpp) illumination_flags
        uint32_t flags;
        struct
        {
            uint8_t light_flags; // not exposed since flags already is so no reason to essentially expose this again, even thou flags is technically not correct

            LIGHT_TYPE type_flags;
            uint8_t layer;
            bool enabled;
        };
    };
};

[[nodiscard]] Illumination* create_illumination(Vec2 pos, Color color, LIGHT_TYPE type, float size, uint8_t flags, int32_t uid, LAYER layer);
[[nodiscard]] Illumination* create_illumination(Color color, float size, float x, float y);
[[nodiscard]] Illumination* create_illumination(Color color, float size, int32_t uid);
void refresh_illumination(Illumination* illumination);
