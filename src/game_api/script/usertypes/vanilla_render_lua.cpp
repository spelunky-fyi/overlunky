#include "vanilla_render_lua.hpp"

#include "script/lua_backend.hpp"

void VanillaRenderContext::draw_text(const std::string& text, float x, float y, float scale_x, float scale_y, Color color, uint32_t alignment, uint32_t fontstyle)
{
    RenderAPI::get().draw_text(text, x, y, scale_x, scale_y, color, alignment, fontstyle);
}

std::pair<float, float> VanillaRenderContext::draw_text_size(const std::string& text, float scale_x, float scale_y, uint32_t fontstyle)
{
    return RenderAPI::get().draw_text_size(text, scale_x, scale_y, fontstyle);
}

void VanillaRenderContext::draw_screen_texture(TEXTURE texture_id, uint8_t row, uint8_t column, float left, float top, float right, float bottom, Color color)
{
    RenderAPI::get().draw_screen_texture(texture_id, row, column, left, top, right, bottom, color);
}

void VanillaRenderContext::draw_screen_texture(TEXTURE texture_id, uint8_t row, uint8_t column, const AABB& rect, Color color)
{
    RenderAPI::get().draw_screen_texture(texture_id, row, column, rect.left, rect.top, rect.right, rect.bottom, color);
}

void VanillaRenderContext::draw_world_texture(TEXTURE texture_id, uint8_t row, uint8_t column, float left, float top, float right, float bottom, Color color)
{
    draw_world_texture(texture_id, row, column, AABB{left, top, right, bottom}, color);
}

void VanillaRenderContext::draw_world_texture(TEXTURE texture_id, uint8_t row, uint8_t column, const AABB& rect, Color color)
{
    draw_world_texture(texture_id, row, column, rect, color, 0);
}

void VanillaRenderContext::draw_world_texture(TEXTURE texture_id, uint8_t row, uint8_t column, const AABB& rect, Color color, float angle)
{
    if (bounding_box.is_valid() && !rect.overlaps_with(bounding_box))
    {
        return;
    }
    QuadTree new_quad = {rect.left, rect.bottom, rect.right, rect.bottom, rect.right, rect.top, rect.left, rect.top};
    if (angle != 0)
    {
        const float sine = sin(angle);
        const float csine = cos(angle);
        const auto center = rect.center();
        float x = (new_quad.bottom_left_x - center.first);
        float y = (new_quad.bottom_left_y - center.second);
        new_quad.bottom_left_x = x * csine - y * sine + center.first;
        new_quad.bottom_left_y = y * csine + x * sine + center.second;

        x = (new_quad.bottom_right_x - center.first);
        y = (new_quad.bottom_right_y - center.second);
        new_quad.bottom_right_x = x * csine - y * sine + center.first;
        new_quad.bottom_right_y = y * csine + x * sine + center.second;

        x = (new_quad.top_left_x = new_quad.top_left_x - center.first);
        y = (new_quad.top_left_y = new_quad.top_left_y - center.second);
        new_quad.top_left_x = x * csine - y * sine + center.first;
        new_quad.top_left_y = y * csine + x * sine + center.second;

        x = (new_quad.top_right_x - center.first);
        y = (new_quad.top_right_y - center.second);
        new_quad.top_right_x = x * csine - y * sine + center.first;
        new_quad.top_right_y = y * csine + x * sine + center.second;
    }
    draw_world_texture(texture_id, row, column, new_quad, color);
}

void VanillaRenderContext::draw_world_texture(TEXTURE texture_id, uint8_t row, uint8_t column, const QuadTree& quad, Color color)
{
    RenderAPI::get().draw_world_texture(texture_id, row, column, quad, color);
}

namespace NVanillaRender
{
void register_usertypes(sol::state& lua)
{
    auto draw_screen_texture = sol::overload(
        static_cast<void (VanillaRenderContext::*)(TEXTURE, uint8_t, uint8_t, float, float, float, float, Color)>(&VanillaRenderContext::draw_screen_texture),
        static_cast<void (VanillaRenderContext::*)(TEXTURE, uint8_t, uint8_t, const AABB&, Color)>(&VanillaRenderContext::draw_screen_texture));
    auto draw_world_texture = sol::overload(
        static_cast<void (VanillaRenderContext::*)(TEXTURE, uint8_t, uint8_t, float, float, float, float, Color)>(&VanillaRenderContext::draw_world_texture),
        static_cast<void (VanillaRenderContext::*)(TEXTURE, uint8_t, uint8_t, const AABB&, Color)>(&VanillaRenderContext::draw_world_texture),
        static_cast<void (VanillaRenderContext::*)(TEXTURE, uint8_t, uint8_t, const AABB&, Color, float)>(&VanillaRenderContext::draw_world_texture),
        static_cast<void (VanillaRenderContext::*)(TEXTURE, uint8_t, uint8_t, const QuadTree&, Color)>(&VanillaRenderContext::draw_world_texture));
    lua.new_usertype<VanillaRenderContext>(
        "VanillaRenderContext",
        "draw_text",
        &VanillaRenderContext::draw_text,
        "draw_text_size",
        &VanillaRenderContext::draw_text_size,
        "draw_screen_texture",
        draw_screen_texture,
        "draw_world_texture",
        draw_world_texture);

    lua.new_usertype<TextureRenderingInfo>(
        "TextureRenderingInfo",
        "x",
        &TextureRenderingInfo::x,
        "y",
        &TextureRenderingInfo::y,

        "destination",
        &TextureRenderingInfo::destination,
        "source",
        &TextureRenderingInfo::source,
        "set_destination",
        &TextureRenderingInfo::set_destination,

        "destination_top_left_x",
        &TextureRenderingInfo::destination_top_left_x,
        "destination_top_left_y",
        &TextureRenderingInfo::destination_top_left_y,
        "destination_top_right_x",
        &TextureRenderingInfo::destination_top_right_x,
        "destination_top_right_y",
        &TextureRenderingInfo::destination_top_right_y,
        "destination_bottom_left_x",
        &TextureRenderingInfo::destination_bottom_left_x,
        "destination_bottom_left_y",
        &TextureRenderingInfo::destination_bottom_left_y,
        "destination_bottom_right_x",
        &TextureRenderingInfo::destination_bottom_right_x,
        "destination_bottom_right_y",
        &TextureRenderingInfo::destination_bottom_right_y,
        "source_top_left_x",
        &TextureRenderingInfo::source_top_left_x,
        "source_top_left_y",
        &TextureRenderingInfo::source_top_left_y,
        "source_top_right_x",
        &TextureRenderingInfo::source_top_right_x,
        "source_top_right_y",
        &TextureRenderingInfo::source_top_right_y,
        "source_bottom_left_x",
        &TextureRenderingInfo::source_bottom_left_x,
        "source_bottom_left_y",
        &TextureRenderingInfo::source_bottom_left_y,
        "source_bottom_right_x",
        &TextureRenderingInfo::source_bottom_right_x,
        "source_bottom_right_y",
        &TextureRenderingInfo::source_bottom_right_y);

    lua.new_usertype<TextRenderingInfo>(
        "TextRenderingInfo",
        "x",
        &TextRenderingInfo::x,
        "y",
        &TextRenderingInfo::y,
        "text_length",
        sol::readonly(&TextRenderingInfo::text_length),
        "width",
        &TextRenderingInfo::width,
        "height",
        &TextRenderingInfo::height,
        "font",
        &TextRenderingInfo::font);
};
} // namespace NVanillaRender
