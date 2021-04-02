#include <Windows.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <thread>

#include <nlohmann/json.hpp>

#include "entity.hpp"
#include "logger.h"
#include "memory.h"
#include "texture.hpp"

using float_json = nlohmann::basic_json<std::map, std::vector, std::string, bool, std::int64_t, std::uint64_t, float>;
#define FLOAT_JSON_DEFINE_TYPE_NON_INTRUSIVE(Type, ...)                                                                                              \
    inline void to_json(float_json &nlohmann_json_j, const Type &nlohmann_json_t)                                                                    \
    {                                                                                                                                                \
        NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, __VA_ARGS__))                                                                     \
    }                                                                                                                                                \
    inline void from_json(const float_json &nlohmann_json_j, Type &nlohmann_json_t)                                                                  \
    {                                                                                                                                                \
        NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_FROM, __VA_ARGS__))                                                                   \
    }

// Have to do this because otherwise it writes out animations like a mess
auto get_animations_as_string_map(const EntityDB &ent)
{
    std::map<std::string, Animation> animations;
    for (auto &[id, anim] : ent.animations)
    {
        animations[std::to_string(id)] = anim;
    }
    return animations;
}

FLOAT_JSON_DEFINE_TYPE_NON_INTRUSIVE(Animation, texture, count, interval, key, repeat);
FLOAT_JSON_DEFINE_TYPE_NON_INTRUSIVE(Rect, masks, up_minus_down, side, up_plus_down);
void to_json(float_json &j, const EntityDB &ent)
{
    j = float_json{
        {"id", ent.id},
        {"search_flags", ent.search_flags},
        {"width", ent.width},
        {"height", ent.height},
        {"rect_collision", ent.rect_collision},
        {"friction", ent.friction},
        {"elasticity", ent.elasticity},
        {"weight", ent.weight},
        {"acceleration", ent.acceleration},
        {"max_speed", ent.max_speed},
        {"sprint_factor", ent.sprint_factor},
        {"jump", ent.jump},
        {"texture", ent.texture},
        {"technique", ent.technique},
        {"tile_x", ent.tile_x},
        {"tile_y", ent.tile_y},
        {"damage", ent.damage},
        {"life", ent.life},
        {"attachOffsetX", ent.attachOffsetX},
        {"attachOffsetY", ent.attachOffsetY},
        {"animations", get_animations_as_string_map(ent)},
    };
}
void to_json(float_json &j, const Texture &tex)
{
    j = float_json{
        {"path", *tex.name},
        {"width", tex.width},
        {"height", tex.height},
        {"num_tiles",
         float_json{
             {"width", tex.num_tiles_width},
             {"height", tex.num_tiles_height},
         }},
        {"tile_width", static_cast<std::uint32_t>(std::round(tex.tile_width_fraction * tex.width))},
        {"tile_height", static_cast<std::uint32_t>(std::round(tex.tile_height_fraction * tex.height))},
        {"offset",
         float_json{
             {"width", static_cast<std::uint32_t>(std::round(tex.offset_x_weird_math * tex.width - 0.5f))},
             {"height", static_cast<std::uint32_t>(std::round(tex.offset_y_weird_math * tex.height - 0.5f))},
         }},
    };
}

using namespace std::chrono_literals;

extern "C" __declspec(dllexport) void run(DWORD pid)
{
    DEBUG("Game injected! Press Ctrl+C to detach this window from the process.");

    while (true)
    {
        auto entities = list_entities();
        if (entities.size() >= 850)
        {
            DEBUG("Found {} entities, that's enough", entities.size());
            std::this_thread::sleep_for(100ms);
            DEBUG("Added {} entities", entities.size());
            break;
        }
        else if (entities.size() > 0)
        {
            DEBUG("Found {} entities", entities.size());
        }
        std::this_thread::sleep_for(100ms);
    }

    std::filesystem::create_directories("game_data");

    if (std::ofstream entities_file = std::ofstream("game_data/entities.json"))
    {
        auto items = list_entities();
        std::sort(items.begin(), items.end(), [](EntityItem &a, EntityItem &b) -> bool { return a.id < b.id; });

        float_json entities(float_json::object());
        for (auto &ent : items)
        {
            EntityDB *db = get_type(ent.id);
            if (!db)
                break;
            entities[ent.name] = *db;
        }

        std::string dump = entities.dump(2);
        entities_file.write(dump.data(), dump.size());
    }

    if (std::ofstream entities_file = std::ofstream("game_data/entities_texture_only.json"))
    {
        auto items = list_entities();
        std::sort(items.begin(), items.end(), [](EntityItem &a, EntityItem &b) -> bool { return a.id < b.id; });

        float_json entities(float_json::object());
        for (auto &ent : items)
        {
            EntityDB *db = get_type(ent.id);
            if (!db)
                break;
            entities[ent.name] = float_json{
                {"id", ent.id},
                {"texture", db->texture},
                {"animations", get_animations_as_string_map(*db)},
            };
        }

        std::string dump = entities.dump(2);
        entities_file.write(dump.data(), dump.size());
    }

    if (std::ofstream textures_file = std::ofstream("game_data/textures.json"))
    {
        Textures *textures_ptr = get_textures();
        std::sort(
            textures_ptr->textures, textures_ptr->textures + textures_ptr->num_textures, [](Texture &a, Texture &b) -> bool { return a.id < b.id; });

        float_json textures(float_json::object());
        for (std::size_t i = 0; i < textures_ptr->num_textures; i++)
        {
            Texture &tex = textures_ptr->textures[i];
            textures[std::to_string(tex.id)] = tex;
        }

        std::string dump = textures.dump(2);
        textures_file.write(dump.data(), dump.size());
    }

    std::exit(0);
}
