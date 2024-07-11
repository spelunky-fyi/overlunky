#pragma once

#include <array>       // for array
#include <cstdint>     // for uint32_t
#include <optional>    // for optional
#include <string>      // for string
#include <string_view> // for string_view

#include "aliases.hpp" // for TEXTURE

struct Texture
{
    TEXTURE id;
    uint32_t padding{0};
    const char** name;
    std::uint32_t width;
    std::uint32_t height;
    std::uint32_t num_tiles_width;
    std::uint32_t num_tiles_height;
    float offset_x_weird_math;
    float offset_y_weird_math;
    float tile_width_fraction;
    float tile_height_fraction;
    float tile_width_minus_one_fraction;
    float tile_height_minus_one_fraction;
    float one_over_width;
    float one_over_height;
};

struct Textures
{
    std::uint32_t num_textures;
    std::array<Texture, 0x192> textures;
    std::array<Texture*, 0x192> texture_map;
};

struct TextureDefinition
{
    std::string texture_path;
    uint32_t width;
    uint32_t height;
    uint32_t tile_width;
    uint32_t tile_height;
    uint32_t sub_image_offset_x{0};
    uint32_t sub_image_offset_y{0};
    uint32_t sub_image_width{0};
    uint32_t sub_image_height{0};
};

Textures* get_textures();
TextureDefinition get_texture_definition(TEXTURE texture_id);
Texture* get_texture(TEXTURE texture_id);
TEXTURE define_texture(TextureDefinition data);
std::optional<TEXTURE> get_texture(TextureDefinition data);
std::optional<TEXTURE> get_texture(std::string_view texture_name);
void reload_texture(const char* texture_name);  // Does a lookup for the right texture to reload
void reload_texture(const char** texture_name); // Reloads the texture directly
bool replace_texture(TEXTURE vanilla_id, TEXTURE custom_id);
void reset_texture(TEXTURE vanilla_id);
bool replace_texture_and_heart_color(TEXTURE vanilla_id, TEXTURE custom_id);
