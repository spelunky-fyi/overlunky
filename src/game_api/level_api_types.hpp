#pragma once

#include <array>
#include <optional>

#include "aliases.hpp"

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
