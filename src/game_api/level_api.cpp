#include "level_api.hpp"

#include "memory.hpp"

std::uint32_t g_last_tile_code_id;
std::uint32_t g_current_tile_code_id;

void LevelGenSystem::init()
{
    {
        auto& tile_codes_map = tile_codes();

        // Getting the last id like this in case the game decides to skip some ids so that last_id != tile_codes.size()
        auto max_id = 0u;
        for (auto& [name, def] : tile_codes_map)
        {
            max_id = std::max(def.id, max_id);
        }
        g_last_tile_code_id = max_id + 1;
    }

    g_current_tile_code_id = g_last_tile_code_id + 1;
}

std::optional<std::uint32_t> LevelGenSystem::get_tile_code(const std::string& tile_code)
{
    auto& tile_codes_map = tile_codes();
    auto it = tile_codes_map.find(tile_code);
    if (it != tile_codes_map.end())
    {
        return it->second.id;
    }
    return {};
}
std::uint32_t LevelGenSystem::define_tile_code(std::string tile_code)
{
    if (auto existing = get_tile_code(tile_code))
    {
        return existing.value();
    }

    auto& tile_code_map = *(std::unordered_map<std::string, TileCodeDef>*)((size_t)this + 0x88);
    auto& new_id = tile_code_map[tile_code].id;
    new_id = g_current_tile_code_id;
    g_current_tile_code_id++;
    return new_id;
}
