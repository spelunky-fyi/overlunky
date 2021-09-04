#pragma once
using CallbackId = uint32_t;
using Flags = std::uint32_t;
using uColor = uint32_t;
using SPAWN_TYPE = int;                       // NoAlias
using VANILLA_SOUND = std::string;            // NoAlias
using VANILLA_SOUND_CALLBACK_TYPE = uint32_t; // NoAlias
using TEXTURE = std::int64_t;                 // NoAlias
using INPUTS = std::uint16_t;                 // NoAlias
using BUTTON = std::uint8_t;                  // NoAlias
using ENT_TYPE = uint32_t;                    // NoAlias
using ROOM_TEMPLATE = uint16_t;               // NoAlias
using PROCEDURAL_CHANCE = uint32_t;           // NoAlias
using ROOM_TEMPLATE_TYPE = int32_t;           // NoAlias
using VANILLA_SOUND_PARAM = std::uint32_t;    // NoAlias
using SpawnTypeFlags = int;                   // NoAlias

constexpr uint8_t MAX_PLAYERS = 4;

enum class LAYER
{
    FRONT = 0,
    BACK = 1,
    PLAYER = -1,
    BOTH = -128
};
