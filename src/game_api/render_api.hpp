#pragma once

#include "entity.hpp"
#include "memory.hpp"
#include "texture.hpp"

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

struct RenderAPI
{
    const size_t* api;
    size_t swap_chain_off;

    mutable std::mutex custom_textures_lock;
    std::unordered_map<std::uint64_t, Texture> custom_textures;

    static RenderAPI& get();

    size_t renderer() const
    {
        return read_u64(*api + 0x10);
    }

    size_t swap_chain() const
    {
        return read_u64(renderer() + swap_chain_off);
    }

    TextureDefinition get_texture_definition(std::uint32_t texture_id);
    Texture* get_texture(std::uint64_t texture_id);
    std::uint64_t define_texture(TextureDefinition data);
    const char** load_texture(std::string file_name);

    void draw_text(const std::string& text, float x, float y, float scale_x, float scale_y, Color color, uint32_t alignment, uint32_t fontstyle);
    std::pair<float, float> draw_text_size(const std::string& text, float scale_x, float scale_y, uint32_t fontstyle);
    void draw_texture(uint32_t texture_id, uint8_t row, uint8_t column, float x1, float y1, float x2, float y2, Color color);
};

// straight out of the x64dbg plugin
struct RenderInfo
{
    size_t __vftable;
    uint32_t unknown1;
    float x;
    float y;
    uint32_t unknown4;
    uint32_t unknown5;
    uint32_t unknown6;
    uint32_t unknown7;
    uint32_t unknown8;
    uint32_t unknown9;
    float x_dupe_1;
    float y_dupe_1;
    uint32_t unknown10;
    float x_dupe2;
    float y_dupe2;
    uint32_t unknown11;
    uint32_t unknown12;
    uint32_t unknown13;
    uint32_t unknown14;
    uint8_t unknown15_counter;
    uint8_t unknown16_counter;
    bool unknown17;
    bool unknown18;
    uint32_t unknown19;
    uint32_t unknown20;

    // destination in world coords
    float destination_bottom_left_x; // entity.x - (entity.w/2)
    float destination_bottom_left_y; // entity.y - (entity.h/2)
    float destination_bottom_left_unknown;
    float destination_bottom_right_x;
    float destination_bottom_right_y;
    float destination_bottom_right_unknown;
    float destination_top_right_x;
    float destination_top_right_y;
    float destination_top_right_unknown;
    float destination_top_left_x;
    float destination_top_left_y;
    float destination_top_left_unknown;

    // source in sprite sheet coords (multiply the floats by TextureDB.width and height)
    float source_bottom_left_x;
    float source_bottom_left_y;
    float source_bottom_right_x;
    float source_bottom_right_y;
    float source_top_right_x;
    float source_top_right_y;
    float source_top_left_x;
    float source_top_left_y;

    float tilew;
    float tileh;
    float x_dupe3;
    float y_dupe3;
    float x_dupe4;
    float y_dupe4;
    float angle1;
    float angle2;
    float angle_related;
    uint32_t animation_frame;
    uint32_t unknown38;
    Texture* texture;
    const char** texture_name;
};

struct TextRenderingInfo
{
    float x;
    float y;
    uint32_t text_length;
    float width;
    float height;
    uint32_t unknown3;
    size_t unknown4;
    size_t unknown5;
    size_t unknown6;
    uint16_t unknown7;
    uint16_t unknown8;
    int32_t unknown9;
    size_t unknown10;
    size_t unknown11;
};

struct TextureRenderingInfo
{
    // where to draw on the screen:
    float x;
    float y;
    // destination is relative to the x,y centerpoint
    float destination_top_left_x;
    float destination_top_left_y;
    float destination_top_right_x;
    float destination_top_right_y;
    float destination_bottom_left_x;
    float destination_bottom_left_y;
    float destination_bottom_right_x;
    float destination_bottom_right_y;
    // source rectangle in the texture to render
    float source_top_left_x;
    float source_top_left_y;
    float source_top_right_x;
    float source_top_right_y;
    float source_bottom_left_x;
    float source_bottom_left_y;
    float source_bottom_right_x;
    float source_bottom_right_y;
};

void init_render_api_hooks();
