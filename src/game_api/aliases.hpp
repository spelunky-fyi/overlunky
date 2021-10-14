#pragma once

#include <cstdint>

using CallbackId = uint32_t;
using Flags = uint32_t;
using uColor = uint32_t;
using SPAWN_TYPE = uint32_t;                  // NoAlias
using VANILLA_SOUND = std::string;            // NoAlias
using VANILLA_SOUND_PARAM = uint32_t;         // NoAlias
using VANILLA_SOUND_CALLBACK_TYPE = uint32_t; // NoAlias
using TEXTURE = int64_t;                      // NoAlias
using INPUTS = uint16_t;                      // NoAlias
using BUTTON = uint8_t;                       // NoAlias
using ENT_TYPE = uint32_t;                    // NoAlias
using ROOM_TEMPLATE = uint16_t;               // NoAlias
using ROOM_TEMPLATE_TYPE = int32_t;           // NoAlias
using PROCEDURAL_CHANCE = uint32_t;           // NoAlias
using LEVEL_CONFIG = uint32_t;                // NoAlias
using TILE_CODE = uint32_t;                   // NoAlias
using SHORT_TILE_CODE = uint8_t;

inline constexpr uint8_t MAX_PLAYERS = 4;

enum class LAYER : int32_t
{
    FRONT = 0,
    BACK = 1,
    PLAYER = -1,
    PLAYER1 = -1,
    PLAYER2 = -2,
    PLAYER3 = -3,
    PLAYER4 = -4,
    BOTH = -128
};

enum class IMAGE : int64_t
{
};
