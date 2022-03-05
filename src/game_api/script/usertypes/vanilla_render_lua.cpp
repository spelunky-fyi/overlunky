#include "vanilla_render_lua.hpp"

#include "render_api.hpp"
#include "script/lua_backend.hpp"
#include "state.hpp" // for enum_to_layer

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
    draw_screen_texture(texture_id, row, column, Quad(AABB(left, top, right, bottom)), color);
}

void VanillaRenderContext::draw_screen_texture(TEXTURE texture_id, uint8_t row, uint8_t column, const AABB& rect, Color color)
{
    draw_screen_texture(texture_id, row, column, Quad(rect), color);
}

void VanillaRenderContext::draw_screen_texture(TEXTURE texture_id, uint8_t row, uint8_t column, const AABB& rect, Color color, float angle, float px, float py)
{
    Quad dest{rect};
    if (angle != 0)
    {
        constexpr float ratio = 16.0f / 9.0f;
        constexpr float inverse_ratio = 9.0f / 16.0f;

        // fix ratio to 1/1 to properly rotate the coordinates
        const AABB new_rect{rect.left * ratio, rect.top, rect.right * ratio, rect.bottom};
        dest = Quad{new_rect};
        auto pivot = new_rect.center();
        if (px != 0 || py != 0)
        {
            pivot.first += abs(pivot.first - new_rect.left) * px;
            pivot.second += abs(pivot.second - new_rect.bottom) * py;
        }

        dest.rotate(angle, pivot.first, pivot.second);
        // bring back to the 16/9
        dest.bottom_left_x *= inverse_ratio;
        dest.bottom_right_x *= inverse_ratio;
        dest.top_left_x *= inverse_ratio;
        dest.top_right_x *= inverse_ratio;
    }
    draw_screen_texture(texture_id, row, column, dest, color);
}

void VanillaRenderContext::draw_screen_texture(TEXTURE texture_id, uint8_t row, uint8_t column, const Quad& dest, Color color)
{
    auto texture = get_texture(texture_id);
    if (texture == nullptr)
    {
        return;
    }
    float uv_left = (texture->tile_width_fraction * column) + texture->offset_x_weird_math;
    float uv_right = uv_left + texture->tile_width_fraction - texture->one_over_width;
    float uv_top = (texture->tile_height_fraction * row) + texture->offset_y_weird_math;
    float uv_bottom = uv_top + texture->tile_height_fraction - texture->one_over_height;

    const Quad source(
        // bottom left:
        uv_left,
        uv_bottom,
        // bottom right:
        uv_right,
        uv_bottom,
        // top right:
        uv_right,
        uv_top,
        // top left:
        uv_left,
        uv_top);

    RenderAPI::get().draw_screen_texture(texture, source, dest, color);
}

void VanillaRenderContext::draw_screen_texture(TEXTURE texture_id, const Quad& source, const Quad& quad, Color color)
{
    auto texture = get_texture(texture_id);
    if (texture == nullptr)
    {
        return;
    }
    RenderAPI::get().draw_screen_texture(texture, source, quad, color);
}

void VanillaRenderContext::draw_world_texture(TEXTURE texture_id, uint8_t row, uint8_t column, float left, float top, float right, float bottom, Color color)
{
    draw_world_texture(texture_id, row, column, Quad{AABB{left, top, right, bottom}}, color);
}

void VanillaRenderContext::draw_world_texture(TEXTURE texture_id, uint8_t row, uint8_t column, const AABB& rect, Color color)
{
    draw_world_texture(texture_id, row, column, Quad{rect}, color);
}

void VanillaRenderContext::draw_world_texture(TEXTURE texture_id, uint8_t row, uint8_t column, const AABB& rect, Color color, float angle, float px, float py)
{
    if (bounding_box.is_valid() && !rect.overlaps_with(bounding_box))
    {
        return;
    }
    Quad new_quad(rect);
    if (angle != 0)
    {
        auto center = rect.center();
        if (px != 0 || py != 0)
        {
            center.first += abs(center.first - rect.left) * px;
            center.second += abs(center.second - rect.bottom) * py;
        }
        new_quad.rotate(angle, center.first, center.second);
    }
    draw_world_texture(texture_id, row, column, new_quad, color);
}

void VanillaRenderContext::draw_world_texture(TEXTURE texture_id, uint8_t row, uint8_t column, const Quad& quad, Color color)
{
    auto texture = get_texture(texture_id);
    if (texture == nullptr)
    {
        return;
    }

    float uv_left = (texture->tile_width_fraction * column) + texture->offset_x_weird_math;
    float uv_right = uv_left + texture->tile_width_fraction - texture->one_over_width;
    float uv_top = (texture->tile_height_fraction * row) + texture->offset_y_weird_math;
    float uv_bottom = uv_top + texture->tile_height_fraction - texture->one_over_height;

    Quad source = {
        // bottom left:
        uv_left,
        uv_bottom,
        // bottom right:
        uv_right,
        uv_bottom,
        // top right:
        uv_right,
        uv_top,
        // top left:
        uv_left,
        uv_top,
    };
    RenderAPI::get().draw_world_texture(texture, source, quad, color);
}

void VanillaRenderContext::draw_world_texture(TEXTURE texture_id, const Quad& source, const Quad& dest, Color color)
{
    auto texture = get_texture(texture_id);
    if (texture == nullptr)
    {
        return;
    }
    RenderAPI::get().draw_world_texture(texture, source, dest, color);
}

namespace NVanillaRender
{
void register_usertypes(sol::state& lua)
{
    static const auto set_lut = [](sol::optional<TEXTURE> texture_id, LAYER layer)
    {
        RenderAPI& render_api = RenderAPI::get();
        if (layer != LAYER::BOTH)
        {
            const uint8_t real_layer = enum_to_layer(layer);
            if (texture_id)
            {
                render_api.set_lut(texture_id.value(), real_layer);
            }
            else
            {
                render_api.reset_lut(real_layer);
            }
        }
        else
        {
            if (texture_id)
            {
                render_api.set_lut(texture_id.value(), 0);
                render_api.set_lut(texture_id.value(), 1);
            }
            else
            {
                render_api.reset_lut(0);
                render_api.reset_lut(1);
            }
        }
    };

    // Force the LUT texture for the given layer (or both) until it is reset
    // Pass `nil` in the first parameter to reset
    lua["set_lut"] = [](sol::optional<TEXTURE> texture_id, LAYER layer)
    {
        set_lut(texture_id, layer);
    };
    // Same as `set_lut(nil, layer)`
    lua["reset_lut"] = [](LAYER layer)
    {
        set_lut(sol::nullopt, layer);
    };

    auto draw_screen_texture = sol::overload(
        static_cast<void (VanillaRenderContext::*)(TEXTURE, uint8_t, uint8_t, float, float, float, float, Color)>(&VanillaRenderContext::draw_screen_texture),
        static_cast<void (VanillaRenderContext::*)(TEXTURE, uint8_t, uint8_t, const AABB&, Color)>(&VanillaRenderContext::draw_screen_texture),
        static_cast<void (VanillaRenderContext::*)(TEXTURE, uint8_t, uint8_t, const AABB&, Color, float, float, float)>(&VanillaRenderContext::draw_screen_texture),
        static_cast<void (VanillaRenderContext::*)(TEXTURE, uint8_t, uint8_t, const Quad&, Color)>(&VanillaRenderContext::draw_screen_texture),
        static_cast<void (VanillaRenderContext::*)(TEXTURE, const Quad&, const Quad&, Color)>(&VanillaRenderContext::draw_screen_texture));
    auto draw_world_texture = sol::overload(
        static_cast<void (VanillaRenderContext::*)(TEXTURE, uint8_t, uint8_t, float, float, float, float, Color)>(&VanillaRenderContext::draw_world_texture),
        static_cast<void (VanillaRenderContext::*)(TEXTURE, uint8_t, uint8_t, const AABB&, Color)>(&VanillaRenderContext::draw_world_texture),
        static_cast<void (VanillaRenderContext::*)(TEXTURE, uint8_t, uint8_t, const AABB&, Color, float, float, float)>(&VanillaRenderContext::draw_world_texture),
        static_cast<void (VanillaRenderContext::*)(TEXTURE, uint8_t, uint8_t, const Quad&, Color)>(&VanillaRenderContext::draw_world_texture),
        static_cast<void (VanillaRenderContext::*)(TEXTURE, const Quad&, const Quad&, Color)>(&VanillaRenderContext::draw_world_texture));
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

        "destination_bottom_left_x",
        &TextureRenderingInfo::destination_bottom_left_x,
        "destination_bottom_left_y",
        &TextureRenderingInfo::destination_bottom_left_y,
        "destination_bottom_right_x",
        &TextureRenderingInfo::destination_bottom_right_x,
        "destination_bottom_right_y",
        &TextureRenderingInfo::destination_bottom_right_y,
        "destination_top_left_x",
        &TextureRenderingInfo::destination_top_left_x,
        "destination_top_left_y",
        &TextureRenderingInfo::destination_top_left_y,
        "destination_top_right_x",
        &TextureRenderingInfo::destination_top_right_x,
        "destination_top_right_y",
        &TextureRenderingInfo::destination_top_right_y,
        "set_destination",
        &TextureRenderingInfo::set_destination,
        "dest_get_quad",
        &TextureRenderingInfo::dest_get_quad,
        "dest_set_quad",
        &TextureRenderingInfo::dest_set_quad,
        "source_bottom_left_x",
        &TextureRenderingInfo::source_bottom_left_x,
        "source_bottom_left_y",
        &TextureRenderingInfo::source_bottom_left_y,
        "source_bottom_right_x",
        &TextureRenderingInfo::source_bottom_right_x,
        "source_bottom_right_y",
        &TextureRenderingInfo::source_bottom_right_y,
        "source_top_left_x",
        &TextureRenderingInfo::source_top_left_x,
        "source_top_left_y",
        &TextureRenderingInfo::source_top_left_y,
        "source_top_right_x",
        &TextureRenderingInfo::source_top_right_x,
        "source_top_right_y",
        &TextureRenderingInfo::source_top_right_y,
        "source_get_quad",
        &TextureRenderingInfo::source_get_quad,
        "source_set_quad",
        &TextureRenderingInfo::source_set_quad);

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
