#pragma once

#include <array>
#include <optional>

using SHORT_TILE_CODE = uint8_t;
using TILE_CODE = uint32_t;         // NoAlias
using ROOM_TEMPLATE = uint16_t;     // NoAlias
using PROCEDURAL_CHANCE = uint32_t; // NoAlias
using ROOM_TEMPLATE_TYPE = int32_t; // NoAlias

struct ShortTileCodeDef
{
    /// Tile code that is used by default when this short tile code is encountered. Defaults to 0.
    TILE_CODE tile_code{0};
    /// Chance in percent to pick `tile_code` over `alt_tile_code`, ignored if `chance == 0`. Defaults to 100.
    uint8_t chance{100};
    /// Alternative tile code, ignored if `chance == 100`. Defaults to 0.
    TILE_CODE alt_tile_code{0};

    bool operator==(const ShortTileCodeDef&) const = default;
};

using SingleRoomData = std::array<std::array<char, 10>, 8>;
struct LevelGenRoomData
{
    SingleRoomData front_layer;
    std::optional<SingleRoomData> back_layer;
};
