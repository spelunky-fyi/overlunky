#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>

struct ShortTileCodeDef
{
    std::uint32_t id;
    std::uint8_t _uint_0;
    std::uint32_t _uint_1;
};
struct TileCodeDef
{
    std::uint32_t id;
};
struct TemplateDef
{
    std::uint32_t id;
};
struct ChanceDef
{
    std::uint32_t id;
};

struct LevelGenSystem
{
    void init();

    std::optional<std::uint32_t> get_tile_code(const std::string& tile_code);
    std::uint32_t define_tile_code(std::string tile_code);

    // TODO: Get offsets from binary instead of hardcoding them
    const std::unordered_map<std::uint8_t, ShortTileCodeDef>& short_tile_codes() const
    {
        return *(const std::unordered_map<std::uint8_t, ShortTileCodeDef>*)((size_t)this + 0x48);
    }
    const std::unordered_map<std::string, TileCodeDef>& tile_codes() const
    {
        return *(const std::unordered_map<std::string, TileCodeDef>*)((size_t)this + 0x88);
    }
    const std::unordered_map<std::string, TemplateDef>& templates() const
    {
        return *(const std::unordered_map<std::string, TemplateDef>*)((size_t)this + 0xC8);
    }
    const std::unordered_map<std::string, ChanceDef>& chances() const
    {
        return *(const std::unordered_map<std::string, ChanceDef>*)((size_t)this + 0x1330);
    }
};
