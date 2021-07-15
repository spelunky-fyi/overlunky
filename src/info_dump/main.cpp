#include <Windows.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <thread>

#include <nlohmann/json.hpp>

#include "entity.hpp"
#include "level_api.hpp"
#include "logger.h"
#include "memory.h"
#include "particles.hpp"
#include "script.hpp"
#include "sound_manager.hpp"
#include "state.hpp"
#include "texture.hpp"

using float_json = nlohmann::basic_json<std::map, std::vector, std::string, bool, std::int64_t, std::uint64_t, float>;
#define FLOAT_JSON_DEFINE_TYPE_NON_INTRUSIVE(Type, ...)                             \
    inline void to_json(float_json& nlohmann_json_j, const Type& nlohmann_json_t)   \
    {                                                                               \
        NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, __VA_ARGS__))    \
    }                                                                               \
    inline void from_json(const float_json& nlohmann_json_j, Type& nlohmann_json_t) \
    {                                                                               \
        NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_FROM, __VA_ARGS__))  \
    }

// Have to do this because otherwise it writes out animations like a mess
auto get_animations_as_string_map(const EntityDB& ent)
{
    std::map<std::string, Animation> animations;
    for (auto& [id, anim] : ent.animations)
    {
        animations[std::to_string(id)] = anim;
    }
    return animations;
}

FLOAT_JSON_DEFINE_TYPE_NON_INTRUSIVE(Animation, texture, count, interval, key, repeat);
FLOAT_JSON_DEFINE_TYPE_NON_INTRUSIVE(Rect, masks, up_minus_down, side, up_plus_down);
void to_json(float_json& j, const EntityDB& ent)
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
void to_json(float_json& j, const Texture& tex)
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

    auto items = list_entities();
    std::sort(items.begin(), items.end(), [](EntityItem& a, EntityItem& b) -> bool
              { return a.id < b.id; });

    Textures* textures_ptr = get_textures();
    std::sort(
        textures_ptr->textures.begin(), textures_ptr->textures.end(), [](Texture& a, Texture& b) -> bool
        { return a.id < b.id; });

    std::filesystem::create_directories("game_data");

    if (std::ofstream entities_file = std::ofstream("game_data/entities.json"))
    {
        float_json entities(float_json::object());
        for (auto& ent : items)
        {
            EntityDB* db = get_type(ent.id);
            if (!db)
                break;

#define HAX_FIX_FLOAT(x)                           \
    if (std::abs(x) < 1e-20 || std::abs(x) > 1e10) \
    x = 0.0f
            HAX_FIX_FLOAT(db->width);
            HAX_FIX_FLOAT(db->height);
            HAX_FIX_FLOAT(db->friction);
            HAX_FIX_FLOAT(db->elasticity);
            HAX_FIX_FLOAT(db->weight);
            HAX_FIX_FLOAT(db->acceleration);
            HAX_FIX_FLOAT(db->max_speed);
            HAX_FIX_FLOAT(db->sprint_factor);
            HAX_FIX_FLOAT(db->jump);
            HAX_FIX_FLOAT(db->glow_red);
            HAX_FIX_FLOAT(db->glow_green);
            HAX_FIX_FLOAT(db->glow_blue);
            HAX_FIX_FLOAT(db->glow_alpha);
            HAX_FIX_FLOAT(db->field_a8);
            HAX_FIX_FLOAT(db->attachOffsetX);
            HAX_FIX_FLOAT(db->attachOffsetY);
#undef HAX_FIX_FLOAT

            entities[ent.name] = *db;
        }

        std::string dump = entities.dump(2);
        entities_file.write(dump.data(), dump.size());
    }

    if (std::ofstream entities_file = std::ofstream("game_data/entities_texture_only.json"))
    {
        float_json entities(float_json::object());
        for (auto& ent : items)
        {
            EntityDB* db = get_type(ent.id);
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
        float_json textures(float_json::object());
        for (std::size_t i = 0; i < textures_ptr->num_textures; i++)
        {
            Texture& tex = textures_ptr->textures[i];
            if (tex.name != nullptr)
            {
                textures[std::to_string(tex.id)] = tex;
            }
        }

        std::string dump = textures.dump(2);
        textures_file.write(dump.data(), dump.size());
    }

    if (std::ofstream file = std::ofstream("game_data/textures.txt"))
    {
        std::unordered_map<std::string, uint32_t> counts;
        for (auto* tex : get_textures()->texture_map)
        {
            if (tex != nullptr && tex->name != nullptr)
            {
                std::string clean_tex_name = *tex->name;
                std::transform(
                    clean_tex_name.begin(), clean_tex_name.end(), clean_tex_name.begin(), [](unsigned char c)
                    { return std::toupper(c); });
                std::replace(clean_tex_name.begin(), clean_tex_name.end(), '/', '_');
                size_t index = clean_tex_name.find(".DDS", 0);
                if (index != std::string::npos)
                {
                    clean_tex_name.erase(index, 4);
                }
                clean_tex_name += '_' + std::to_string(counts[clean_tex_name]++);
                file << "TEXTURE." << clean_tex_name << ": " << tex->id << std::endl;
            }
        }
    }

    if (std::ofstream search_flags_file = std::ofstream("game_data/search_flags.json"))
    {
        float_json search_flags(float_json::object());
        for (size_t i = 31; i < 32; i--)
        {
            std::uint32_t search_flag = 1 << i;
            std::vector<std::string> entities;
            for (auto& ent : items)
            {
                EntityDB* db = get_type(ent.id);
                if (!db)
                    break;
                if ((db->search_flags & search_flag) != 0)
                {
                    entities.push_back(ent.name);
                }
            }
            search_flags[fmt::format("{}", search_flag)] = std::move(entities);
        }

        std::string dump = search_flags.dump(2);
        search_flags_file.write(dump.data(), dump.size());
    }

    if (auto file = std::ofstream("game_data/entities.txt"))
    {
        for (auto& ent : items)
        {
            EntityDB* db = get_type(ent.id);
            if (!db)
                break;
            file << ent.id << ": " << ent.name << std::endl;
        }
    }

    SoundManager sound_mgr(nullptr);

    if (auto file = std::ofstream("game_data/vanilla_sounds.txt"))
    {
        sound_mgr.for_each_event_name(
            [&file](std::string event_name)
            {
                std::string clean_event_name = event_name;
                std::transform(
                    clean_event_name.begin(), clean_event_name.end(), clean_event_name.begin(), [](unsigned char c)
                    { return std::toupper(c); });
                std::replace(clean_event_name.begin(), clean_event_name.end(), '/', '_');
                file << event_name << ": VANILLA_SOUND." << clean_event_name << std::endl;
            });
    }

    if (auto file = std::ofstream("game_data/vanilla_sound_params.txt"))
    {
        sound_mgr.for_each_parameter_name(
            [&file](std::string parameter_name, std::uint32_t id)
            {
                std::transform(parameter_name.begin(), parameter_name.end(), parameter_name.begin(), [](unsigned char c)
                               { return std::toupper(c); });
                file << id << ": VANILLA_SOUND_PARAM." << parameter_name << std::endl;
            });
    }

    if (auto file = std::ofstream("game_data/particle_emitters.txt"))
    {
        auto particles = list_particles();
        for (const auto& particle : particles)
        {
            file << particle.id << ": " << particle.name << "\n";
        }
    }

    if (auto file = std::ofstream("game_data/spel2.lua"))
    {
        SpelunkyScript api_gen_script(
            "meta.name = 'Script'\nmeta.version = '0.1'\nmeta.description = 'Shiny new script'\nmeta.author = 'You'",
            "api_gen",
            &sound_mgr,
            true);
        file << api_gen_script.dump_api() << std::endl;
    }

    auto* state = State::get().ptr();

    if (auto file = std::ofstream("game_data/tile_codes.txt"))
    {
        auto tile_codes = state->level_gen->data->tile_codes();
        for (const auto& tile_code : tile_codes)
        {
            file << tile_code.second.id << ": " << tile_code.first << "\n";
        }
    }

    if (auto file = std::ofstream("game_data/spawn_chances.txt"))
    {
        std::multimap<std::uint16_t, std::string> ordered_chances;
        for (auto* chances : {&state->level_gen->data->monster_chances(), &state->level_gen->data->trap_chances()})
        {
            for (const auto& spawn_chanc : *chances)
            {
                std::string clean_chance_name = spawn_chanc.first;
                std::transform(
                    clean_chance_name.begin(), clean_chance_name.end(), clean_chance_name.begin(), [](unsigned char c)
                    { return std::toupper(c); });
                std::replace(clean_chance_name.begin(), clean_chance_name.end(), '-', '_');
                ordered_chances.insert({spawn_chanc.second.id, std::move(clean_chance_name)});
            }
        }
        for (const auto& [id, name] : ordered_chances)
            file << name << ": " << id << "\n";
    }

    if (auto file = std::ofstream("game_data/room_templates.txt"))
    {
        auto templates = state->level_gen->data->room_templates();
        std::multimap<std::uint16_t, std::string> ordered_templates;
        for (const auto& room_template : templates)
        {
            std::string clean_room_name = room_template.first;
            std::transform(
                clean_room_name.begin(), clean_room_name.end(), clean_room_name.begin(), [](unsigned char c)
                { return std::toupper(c); });
            std::replace(clean_room_name.begin(), clean_room_name.end(), '-', '_');
            ordered_templates.insert({room_template.second.id, std::move(clean_room_name)});
        }
        for (const auto& [id, name] : ordered_templates)
            file << name << ": " << id << "\n";
    }

    std::exit(0);
}
