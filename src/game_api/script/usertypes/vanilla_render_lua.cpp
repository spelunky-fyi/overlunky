#include "vanilla_render_lua.hpp"

#include <algorithm>   // for max
#include <cstdlib>     // for abs
#include <locale>      // for num_put
#include <new>         // for operator new
#include <optional>    // for nullopt
#include <sol/sol.hpp> // for global_table, proxy_key_t, optional, state
#include <tuple>       // for get
#include <type_traits> // for move, declval

#include "particles.hpp"          // for ParticleEmitterInfo
#include "render_api.hpp"         // for TextureRenderingInfo, WorldShader, TextRen...
#include "script/lua_backend.hpp" // for get_calling_backend
#include "state.hpp"              // for enum_to_layer
#include "texture.hpp"            // for Texture, get_texture

void VanillaRenderContext::draw_text(const std::string& text, float x, float y, float scale_x, float scale_y, Color color, VANILLA_TEXT_ALIGNMENT alignment, VANILLA_FONT_STYLE fontstyle)
{
    TextRenderingInfo tri{};
    tri.set_text(text, x, y, scale_x, scale_y, alignment, fontstyle);
    RenderAPI::get().draw_text(&tri, std::move(color));
}

void VanillaRenderContext::draw_text(const TextRenderingInfo* tri, Color color)
{
    RenderAPI::get().draw_text(tri, std::move(color));
}

std::pair<float, float> VanillaRenderContext::draw_text_size(const std::string& text, float scale_x, float scale_y, uint32_t fontstyle)
{
    return RenderAPI::get().draw_text_size(text, scale_x, scale_y, fontstyle);
}

void VanillaRenderContext::draw_screen_texture(TEXTURE texture_id, uint8_t row, uint8_t column, float left, float top, float right, float bottom, Color color)
{
    draw_screen_texture(texture_id, row, column, Quad(AABB(left, top, right, bottom)), std::move(color));
}

void VanillaRenderContext::draw_screen_texture(TEXTURE texture_id, uint8_t row, uint8_t column, const AABB& rect, Color color)
{
    draw_screen_texture(texture_id, row, column, Quad(rect), std::move(color));
}

Quad& convert_ratio(Quad& quad, bool to_screen)
{
    constexpr float ratio = 16.0f / 9.0f;
    constexpr float inverse_ratio = 9.0f / 16.0f;
    if (to_screen)
    {
        quad.bottom_left_x *= inverse_ratio;
        quad.bottom_right_x *= inverse_ratio;
        quad.top_left_x *= inverse_ratio;
        quad.top_right_x *= inverse_ratio;
    }
    else
    {
        quad.bottom_left_x *= ratio;
        quad.bottom_right_x *= ratio;
        quad.top_left_x *= ratio;
        quad.top_right_x *= ratio;
    }
    return quad;
}

void VanillaRenderContext::draw_screen_texture(TEXTURE texture_id, uint8_t row, uint8_t column, const AABB& rect, Color color, float angle, float px, float py)
{
    Quad dest{rect};
    if (angle != 0)
    {
        constexpr float ratio = 16.0f / 9.0f;

        // fix ratio to 1/1 to properly rotate the coordinates
        const AABB new_rect{rect.left * ratio, rect.top, rect.right * ratio, rect.bottom};
        dest = Quad{new_rect};
        auto pivot = new_rect.center();
        if (px != 0 || py != 0)
        {
            pivot.first += std::abs(pivot.first - new_rect.left) * px;
            pivot.second += std::abs(pivot.second - new_rect.bottom) * py;
        }

        dest.rotate(angle, pivot.first, pivot.second);
        convert_ratio(dest, true);
    }
    draw_screen_texture(texture_id, row, column, dest, std::move(color));
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

    RenderAPI::get().draw_screen_texture(texture, source, dest, std::move(color), 0x29);
}

void VanillaRenderContext::draw_screen_texture(TEXTURE texture_id, const Quad& source, const Quad& quad, Color color)
{
    auto texture = get_texture(texture_id);
    if (texture == nullptr)
    {
        return;
    }
    RenderAPI::get().draw_screen_texture(texture, source, quad, std::move(color), 0x29);
}

void VanillaRenderContext::draw_screen_texture(TEXTURE texture_id, TextureRenderingInfo tri, Color color)
{
    auto texture = get_texture(texture_id);
    if (texture == nullptr)
    {
        return;
    }
    RenderAPI::get().draw_screen_texture(texture, std::move(tri), std::move(color), 0x29);
}

void VanillaRenderContext::set_corner_finish(CORNER_FINISH c)
{
    auto backend = LuaBackend::get_calling_backend();
    backend->vanilla_render_corner_finish = c;
}

// get a Quad to fill out the corner between two lines and fix their overlap
Quad get_corner_quad(Quad& line1, Quad& line2)
{
    auto backend = LuaBackend::get_calling_backend();
    if (backend->vanilla_render_corner_finish == CORNER_FINISH::NONE)
        return {};

    // save the corners as Vec2 for easier calculations
    // and calculate inner and outher corner, we don't know which is which at this point
    Vec2 A{line1.top_left_x, line1.top_left_y};
    Vec2 B{line1.top_right_x, line1.top_right_y};

    Vec2 C{line2.top_right_x, line2.top_right_y};
    Vec2 D{line2.top_left_x, line2.top_left_y};
    Vec2 corner1 = intersection(A, B, C, D);
    if (isinf(corner1.x) || isinf(corner1.y))
        return {};

    Vec2 E{line1.bottom_left_x, line1.bottom_left_y};
    Vec2 F{line1.bottom_right_x, line1.bottom_right_y};

    Vec2 G{line2.bottom_right_x, line2.bottom_right_y};
    Vec2 H{line2.bottom_left_x, line2.bottom_left_y};
    Vec2 corner2 = intersection(E, F, G, H);
    if (isinf(corner2.x) || isinf(corner2.y))
        return {};

    // calculate true angle between the two lines that we want to fill (doesn't matter if we use (B, corner1, D) vs (F, corner2, H) as the lines are parallel to each other
    Vec2 ab = corner1 - B;
    Vec2 bc = D - corner1;
    float true_angle = std::atan2((bc.y * ab.x - bc.x * ab.y), (bc.x * ab.x + bc.y * ab.y));

    // use references to minimize code duplication
    Vec2 *first, *second, *outer_corner, *inner_corner;

    if (true_angle < 0) // check which one is the inner and which outer corner
    {
        first = &B;
        second = &D;
        outer_corner = &corner1;
        inner_corner = &corner2;
    }
    else
    {
        first = &F;
        second = &H;
        outer_corner = &corner2;
        inner_corner = &corner1;
    }

    Vec2 middle_point = *first - (*first - *second) / 2;

    if (backend->vanilla_render_corner_finish == CORNER_FINISH::ADAPTIVE && std::abs(true_angle) > 1.6f)
    {
        Vec2 offset = *outer_corner - middle_point;
        offset = offset * (float)std::pow((std::abs(true_angle) - 1.6f) / 1.54f, 2);
        *outer_corner -= offset;
    }
    else if (backend->vanilla_render_corner_finish == CORNER_FINISH::CUT)
    {
        *outer_corner = *second;
    }
    // else CORNER_FINISH::REAL

    if (std::abs(true_angle) > 3) // for small corners it's hard to do this right, so we give up on this
    {
        *inner_corner = middle_point;
    }
    else
    {
        if (true_angle < 0) // check inner vs outer again, fix the two lines from the drawn boxes that overlap
        {
            line1.bottom_right_x = inner_corner->x;
            line1.bottom_right_y = inner_corner->y;
            line2.bottom_left_x = inner_corner->x;
            line2.bottom_left_y = inner_corner->y;
        }
        else
        {
            line1.top_right_x = inner_corner->x;
            line1.top_right_y = inner_corner->y;
            line2.top_left_x = inner_corner->x;
            line2.top_left_y = inner_corner->y;
        }
    }

    return Quad{*first, *inner_corner, *second, *outer_corner};
};
// convert two points into Quad (rectangle) with given `thickness` (height)
Quad get_line_quad(const Vec2 A, const Vec2 B, float thickness, bool world = false)
{
    // scale it to use more of a whole values, and make world vs screen the same with default zoom level
    if (world)
        thickness *= .00559659576f;
    else
        thickness *= .001f;

    float axis_AB_angle = std::atan2((B.y - A.y), (B.x) - (A.x));
    float hypotenuse = (float)std::sqrt(std::pow(B.x - A.x, 2) + std::pow(B.y - A.y, 2));

    // make ractangle and then rotate it because i'm stupid
    Quad dest{AABB{A.x, A.y + thickness / 2, (A.x + hypotenuse), A.y - thickness / 2}};
    dest.rotate(axis_AB_angle, A.x, A.y);
    return dest;
};

void VanillaRenderContext::draw_screen_rect(const AABB& rect, float thickness, Color color, std::optional<float> angle, std::optional<float> px, std::optional<float> py)
{
    Quad dest{rect};
    if (angle.has_value())
    {
        constexpr float ratio = 16.0f / 9.0f;

        // fix ratio to 1/1 to properly rotate the coordinates
        const AABB new_rect{rect.left * ratio, rect.top, rect.right * ratio, rect.bottom};
        dest = Quad{new_rect};
        auto pivot = new_rect.center();
        if (px.has_value() || py.has_value())
        {
            pivot.first += std::abs(pivot.first - new_rect.left) * px.value_or(0);
            pivot.second += std::abs(pivot.second - new_rect.bottom) * py.value_or(0);
        }
        dest.rotate(angle.value(), pivot.first, pivot.second);
        convert_ratio(dest, true);
    }
    draw_screen_poly(dest, thickness, std::move(color), true);
}
void VanillaRenderContext::draw_screen_rect_filled(const AABB& rect, Color color, std::optional<float> angle, std::optional<float> px, std::optional<float> py)
{
    Quad dest{rect};
    if (angle.has_value())
    {
        constexpr float ratio = 16.0f / 9.0f;

        // fix ratio to 1/1 to properly rotate the coordinates
        const AABB new_rect{rect.left * ratio, rect.top, rect.right * ratio, rect.bottom};
        dest = Quad{new_rect};
        auto pivot = new_rect.center();
        if (px.has_value() || py.has_value())
        {
            pivot.first += std::abs(pivot.first - new_rect.left) * px.value_or(0);
            pivot.second += std::abs(pivot.second - new_rect.bottom) * py.value_or(0);
        }
        dest.rotate(angle.value(), pivot.first, pivot.second);
        convert_ratio(dest, true);
    }
    draw_screen_poly_filled(dest, std::move(color));
}

void VanillaRenderContext::draw_screen_triangle(const Triangle& triangle, float thickness, Color color)
{
    draw_screen_poly({triangle.A, triangle.B, triangle.C}, thickness, std::move(color), true);
}
void VanillaRenderContext::draw_screen_triangle_filled(const Triangle& triangle, Color color)
{
    draw_screen_poly_filled({triangle.A, triangle.B, triangle.C}, std::move(color));
}

void VanillaRenderContext::draw_screen_line(const Vec2& A, const Vec2& B, float thickness, Color color)
{
    constexpr float ratio = 16.0f / 9.0f;

    Vec2 new_A{A.x * ratio, A.y};
    Vec2 new_B{B.x * ratio, B.y};
    Quad line = get_line_quad(new_A, new_B, thickness);
    draw_screen_poly_filled(convert_ratio(line, true), std::move(color));
}

void VanillaRenderContext::draw_screen_poly(const Quad& points, float thickness, Color color, bool closed)
{
    auto [A, B, C, D] = points.operator std::tuple<Vec2, Vec2, Vec2, Vec2>();
    draw_screen_poly(std::vector<Vec2>{A, B, C, D}, thickness, color, closed);
}
void VanillaRenderContext::draw_screen_poly(std::vector<Vec2> points, float thickness, Color color, bool closed)
{
    constexpr float ratio = 16.0f / 9.0f;

    if (points.size() < 2)
        return;

    std::vector<Quad> draw_list;
    draw_list.reserve(points.size());
    Vec2 last_point{points[0].x * ratio, points[0].y};

    if (closed)
    {
        Vec2 end = points.back();
        end.x *= ratio;
        draw_list.push_back(get_line_quad(end, last_point, thickness));
    }

    for (int i = 1; i < points.size(); ++i)
    {
        Vec2 new_B{points[i].x * ratio, points[i].y};
        Quad line = get_line_quad(last_point, new_B, thickness);
        if (!draw_list.empty())
        {
            Quad corner = get_corner_quad(draw_list.back(), line);
            if (!corner.is_null())
                draw_screen_poly_filled(convert_ratio(corner, true), std::move(color));
        }
        draw_list.push_back(line);
        last_point = new_B;
    }
    if (closed)
    {
        Quad corner = get_corner_quad(draw_list.back(), draw_list.front());
        if (!corner.is_null())
            draw_screen_poly_filled(convert_ratio(corner, true), std::move(color));
    }
    for (auto& line : draw_list)
    {
        draw_screen_poly_filled(convert_ratio(line, true), std::move(color));
    }
}
void VanillaRenderContext::draw_screen_poly_filled(const Quad& dest, Color color)
{
    auto texture = get_texture(0);                                                       // any texture works
    RenderAPI::get().draw_screen_texture(texture, Quad{}, dest, std::move(color), 0x27); // 0x27 funky shader, 2C also works
}
void VanillaRenderContext::draw_screen_poly_filled(std::vector<Vec2> points, Color color)
{
    if (points.size() < 3)
        return;

    Vec2 temp = points[1];
    unsigned int i = 3;
    for (; i < points.size(); i += 2)
    {

        draw_screen_poly_filled(Quad{points[0], temp, points[i - 1], points[i]}, std::move(color));
        temp = points[i]; // always repeat the last "line" drawn
    }
    if (points.size() % 2 != 0) // not even number of points so the last piece is triangle
    {
        draw_screen_poly_filled(Quad{points[0], temp, points.back(), points.back()}, std::move(color));
    }
}

void VanillaRenderContext::draw_world_texture(TEXTURE texture_id, uint8_t row, uint8_t column, float left, float top, float right, float bottom, Color color)
{
    draw_world_texture(texture_id, row, column, Quad{AABB{left, top, right, bottom}}, std::move(color));
}

void VanillaRenderContext::draw_world_texture(TEXTURE texture_id, uint8_t row, uint8_t column, const AABB& rect, Color color)
{
    draw_world_texture(texture_id, row, column, Quad{rect}, std::move(color));
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
    draw_world_texture(texture_id, row, column, new_quad, std::move(color));
}

void VanillaRenderContext::draw_world_texture(TEXTURE texture_id, uint8_t row, uint8_t column, const Quad& dest, Color color, WORLD_SHADER shader)
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
    RenderAPI::get().draw_world_texture(texture, source, dest, std::move(color), (WorldShader)shader);
}

void VanillaRenderContext::draw_world_texture(TEXTURE texture_id, uint8_t row, uint8_t column, const Quad& dest, Color color)
{
    draw_world_texture(texture_id, row, column, dest, std::move(color), (WORLD_SHADER)WorldShader::TextureColor);
}

void VanillaRenderContext::draw_world_texture(TEXTURE texture_id, const Quad& source, const Quad& dest, Color color, WORLD_SHADER shader)
{
    auto texture = get_texture(texture_id);
    if (texture == nullptr)
    {
        return;
    }
    RenderAPI::get().draw_world_texture(texture, source, dest, std::move(color), (WorldShader)shader);
}

void VanillaRenderContext::draw_world_texture(TEXTURE texture_id, const Quad& source, const Quad& dest, Color color)
{
    draw_world_texture(texture_id, source, dest, std::move(color), (WORLD_SHADER)WorldShader::TextureColor);
}

void VanillaRenderContext::draw_world_rect(const AABB& rect, float thickness, Color color, std::optional<float> angle, std::optional<float> px, std::optional<float> py)
{
    Quad dest{rect};
    if (angle.has_value())
    {
        auto pivot = rect.center();
        if (px.has_value() || py.has_value())
        {
            pivot.first += std::abs(pivot.first - rect.left) * px.value_or(0);
            pivot.second += std::abs(pivot.second - rect.bottom) * py.value_or(0);
        }
        dest.rotate(angle.value(), pivot.first, pivot.second);
    }
    draw_world_poly(dest, thickness, std::move(color), true);
}

void VanillaRenderContext::draw_world_rect_filled(const AABB& rect, Color color, std::optional<float> angle, std::optional<float> px, std::optional<float> py)
{
    Quad dest{rect};
    if (angle.has_value())
    {
        auto pivot = rect.center();
        if (px.has_value() || py.has_value())
        {
            pivot.first += std::abs(pivot.first - rect.left) * px.value_or(0);
            pivot.second += std::abs(pivot.second - rect.bottom) * py.value_or(0);
        }
        dest.rotate(angle.value(), pivot.first, pivot.second);
    }
    draw_world_poly_filled(dest, std::move(color));
}

void VanillaRenderContext::draw_world_triangle(const Triangle& triangle, float thickness, Color color)
{
    draw_world_poly({triangle.A, triangle.B, triangle.C}, thickness, std::move(color), true);
}

void VanillaRenderContext::draw_world_triangle_filled(const Triangle& triangle, Color color)
{
    draw_world_poly_filled({triangle.A, triangle.B, triangle.C}, std::move(color));
}

void VanillaRenderContext::draw_world_line(const Vec2& A, const Vec2& B, float thickness, Color color)
{
    Quad line = get_line_quad(A, B, thickness, true);
    draw_world_poly_filled(line, std::move(color));
}

void VanillaRenderContext::draw_world_poly(const Quad& points, float thickness, Color color, bool closed)
{
    auto [A, B, C, D] = points.operator std::tuple<Vec2, Vec2, Vec2, Vec2>();
    draw_world_poly(std::vector<Vec2>{A, B, C, D}, thickness, color, closed);
}

void VanillaRenderContext::draw_world_poly(std::vector<Vec2> points, float thickness, Color color, bool closed)
{
    if (points.size() < 2)
        return;

    std::vector<Quad> draw_list;
    draw_list.reserve(points.size());
    Vec2 last_point{points[0]};

    if (closed)
    {
        Vec2 end = points.back();
        draw_list.push_back(get_line_quad(end, last_point, thickness, true));
    }

    for (int i = 1; i < points.size(); ++i)
    {
        Quad line = get_line_quad(last_point, points[i], thickness, true);
        if (!draw_list.empty())
        {
            Quad corner = get_corner_quad(draw_list.back(), line);
            if (!corner.is_null())
                draw_world_poly_filled(corner, std::move(color));
        }
        draw_list.push_back(line);
        last_point = points[i];
    }
    if (closed)
    {
        Quad corner = get_corner_quad(draw_list.back(), draw_list.front());
        if (!corner.is_null())
            draw_world_poly_filled(corner, std::move(color));
    }
    for (auto& line : draw_list)
    {
        draw_world_poly_filled(line, std::move(color));
    }
}

void VanillaRenderContext::draw_world_poly_filled(const Quad& dest, Color color)
{
    auto texture = get_texture(0); // any texture works
    RenderAPI::get().draw_world_texture(texture, Quad{}, dest, std::move(color), WorldShader::DeferredColorTransparent);
}

void VanillaRenderContext::draw_world_poly_filled(std::vector<Vec2> points, Color color)
{
    if (points.size() < 3)
        return;

    Vec2 temp = points[1];
    unsigned int i = 3;
    for (; i < points.size(); i += 2)
    {

        draw_world_poly_filled(Quad{points[0], temp, points[i - 1], points[i]}, std::move(color));
        temp = points[i]; // always repeat the last "line" drawn
    }
    if (points.size() % 2 != 0) // not even number of points so the last piece is triangle
    {
        draw_world_poly_filled(Quad{points[0], temp, points.back(), points.back()}, std::move(color));
    }
}

// For the custom constructor
void TextRenderingInfo_ctor(TextRenderingInfo& uninitialized_memory, const std::u16string text, float x, float y, float scale_x, float scale_y, uint32_t alignment, uint32_t fontstyle)
{
    new (&uninitialized_memory) TextRenderingInfo{};
    uninitialized_memory.set_text(text, x, y, scale_x, scale_y, alignment, fontstyle);
}
void TextRenderingInfo_ctor2(TextRenderingInfo& uninitialized_memory, const std::u16string text, float scale_x, float scale_y, uint32_t alignment, uint32_t fontstyle)
{
    new (&uninitialized_memory) TextRenderingInfo{};
    uninitialized_memory.set_text(text, 0, 0, scale_x, scale_y, alignment, fontstyle);
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

    /// Force the LUT texture for the given layer (or both) until it is reset.
    /// Pass `nil` in the first parameter to reset
    lua["set_lut"] = [](sol::optional<TEXTURE> texture_id, LAYER layer)
    {
        set_lut(texture_id, layer);
    };
    /// Same as `set_lut(nil, layer)`
    lua["reset_lut"] = [](LAYER layer)
    {
        set_lut(sol::nullopt, layer);
    };

    /// NoDoc: Dev-tool only
    lua["reload_shaders"] = []()
    {
        RenderAPI::get().reload_shaders();
    };

    auto draw_screen_texture = sol::overload(
        static_cast<void (VanillaRenderContext::*)(TEXTURE, uint8_t, uint8_t, float, float, float, float, Color)>(&VanillaRenderContext::draw_screen_texture),
        static_cast<void (VanillaRenderContext::*)(TEXTURE, uint8_t, uint8_t, const AABB&, Color)>(&VanillaRenderContext::draw_screen_texture),
        static_cast<void (VanillaRenderContext::*)(TEXTURE, uint8_t, uint8_t, const AABB&, Color, float, float, float)>(&VanillaRenderContext::draw_screen_texture),
        static_cast<void (VanillaRenderContext::*)(TEXTURE, uint8_t, uint8_t, const Quad&, Color)>(&VanillaRenderContext::draw_screen_texture),
        static_cast<void (VanillaRenderContext::*)(TEXTURE, const Quad&, const Quad&, Color)>(&VanillaRenderContext::draw_screen_texture),
        static_cast<void (VanillaRenderContext::*)(TEXTURE, TextureRenderingInfo, Color)>(&VanillaRenderContext::draw_screen_texture));
    auto draw_world_texture = sol::overload(
        static_cast<void (VanillaRenderContext::*)(TEXTURE, uint8_t, uint8_t, float, float, float, float, Color)>(&VanillaRenderContext::draw_world_texture),
        static_cast<void (VanillaRenderContext::*)(TEXTURE, uint8_t, uint8_t, const AABB&, Color)>(&VanillaRenderContext::draw_world_texture),
        static_cast<void (VanillaRenderContext::*)(TEXTURE, uint8_t, uint8_t, const AABB&, Color, float, float, float)>(&VanillaRenderContext::draw_world_texture),
        static_cast<void (VanillaRenderContext::*)(TEXTURE, uint8_t, uint8_t, const Quad&, Color)>(&VanillaRenderContext::draw_world_texture),
        static_cast<void (VanillaRenderContext::*)(TEXTURE, uint8_t, uint8_t, const Quad&, Color, WORLD_SHADER)>(&VanillaRenderContext::draw_world_texture),
        static_cast<void (VanillaRenderContext::*)(TEXTURE, const Quad&, const Quad&, Color)>(&VanillaRenderContext::draw_world_texture),
        static_cast<void (VanillaRenderContext::*)(TEXTURE, const Quad&, const Quad&, Color, WORLD_SHADER)>(&VanillaRenderContext::draw_world_texture));
    auto draw_text = sol::overload(
        static_cast<void (VanillaRenderContext::*)(const std::string&, float, float, float, float, Color, uint32_t, uint32_t)>(&VanillaRenderContext::draw_text),
        static_cast<void (VanillaRenderContext::*)(const TextRenderingInfo*, Color)>(&VanillaRenderContext::draw_text));
    auto draw_screen_poly = sol::overload(
        static_cast<void (VanillaRenderContext::*)(const Quad&, float, Color, bool)>(&VanillaRenderContext::draw_screen_poly),
        static_cast<void (VanillaRenderContext::*)(const std::vector<Vec2>, float, Color, bool)>(&VanillaRenderContext::draw_screen_poly));
    auto draw_screen_poly_filled = sol::overload(
        static_cast<void (VanillaRenderContext::*)(const Quad&, Color)>(&VanillaRenderContext::draw_screen_poly_filled),
        static_cast<void (VanillaRenderContext::*)(const std::vector<Vec2>, Color)>(&VanillaRenderContext::draw_screen_poly_filled));
    auto draw_world_poly = sol::overload(
        static_cast<void (VanillaRenderContext::*)(const Quad&, float, Color, bool)>(&VanillaRenderContext::draw_world_poly),
        static_cast<void (VanillaRenderContext::*)(const std::vector<Vec2>, float, Color, bool)>(&VanillaRenderContext::draw_world_poly));
    auto draw_world_poly_filled = sol::overload(
        static_cast<void (VanillaRenderContext::*)(const Quad&, Color)>(&VanillaRenderContext::draw_world_poly_filled),
        static_cast<void (VanillaRenderContext::*)(const std::vector<Vec2>, Color)>(&VanillaRenderContext::draw_world_poly_filled));

    auto render_draw_depth_lua = [](VanillaRenderContext&, LAYER layer, uint8_t draw_depth, AABB bbox)
    {
        const uint8_t real_layer = enum_to_layer(layer);
        auto layer_ptr = State::get().layer(real_layer);
        render_draw_depth(layer_ptr, draw_depth, bbox.left, bbox.bottom, bbox.right, bbox.top);
    };

    /// Used in [set_callback](#set_callback) ON.RENDER_* callbacks, [set_post_render](#set_post_render), [set_post_render_screen](#set_post_render_screen), [set_pre_render](#set_pre_render), [set_pre_render_screen](#set_pre_render_screen)
    lua.new_usertype<VanillaRenderContext>(
        "VanillaRenderContext",
        "draw_text",
        draw_text,
        "draw_text_size",
        &VanillaRenderContext::draw_text_size,
        "draw_screen_texture",
        draw_screen_texture,
        "set_corner_finish",
        &VanillaRenderContext::set_corner_finish,
        "draw_screen_line",
        &VanillaRenderContext::draw_screen_line,
        "draw_screen_rect",
        &VanillaRenderContext::draw_screen_rect,
        "draw_screen_rect_filled",
        &VanillaRenderContext::draw_screen_rect_filled,
        "draw_screen_triangle",
        &VanillaRenderContext::draw_screen_triangle,
        "draw_screen_triangle_filled",
        &VanillaRenderContext::draw_screen_triangle_filled,
        "draw_screen_poly",
        draw_screen_poly,
        "draw_screen_poly_filled",
        draw_screen_poly_filled,
        "draw_world_texture",
        draw_world_texture,
        "draw_world_line",
        &VanillaRenderContext::draw_world_line,
        "draw_world_rect",
        &VanillaRenderContext::draw_world_rect,
        "draw_world_rect_filled",
        &VanillaRenderContext::draw_world_rect_filled,
        "draw_world_triangle",
        &VanillaRenderContext::draw_world_triangle,
        "draw_world_triangle_filled",
        &VanillaRenderContext::draw_world_triangle_filled,
        "draw_world_poly",
        draw_world_poly,
        "draw_world_poly_filled",
        draw_world_poly_filled,
        "bounding_box",
        &VanillaRenderContext::bounding_box,
        "render_draw_depth",
        render_draw_depth_lua);

    lua.create_named_table("CORNER_FINISH", "NONE", CORNER_FINISH::NONE, "REAL", CORNER_FINISH::REAL, "CUT", CORNER_FINISH::CUT, "ADAPTIVE", CORNER_FINISH::ADAPTIVE);
    /* CORNER_FINISH
    // NONE
    // Don't draw corner at all, will draw lines as separate pieces, overlaping etc.
    // REAL
    // Draws a real corner, no matter how far away the "peak" of the corner may end up being
    // CUT
    // Instead of drawing a sharp point at the end of the corner, it just cuts it flat
    // ADAPTIVE
    // Default
    // similar to REAL but for low angles reduces the size of the "peak" of the corner
    */

    auto texturerenderinginfo_type = lua.new_usertype<TextureRenderingInfo>("TextureRenderingInfo");
    texturerenderinginfo_type["new"] = sol::constructors<TextureRenderingInfo(), TextureRenderingInfo(const TextureRenderingInfo&)>{};
    texturerenderinginfo_type["x"] = &TextureRenderingInfo::x;
    texturerenderinginfo_type["y"] = &TextureRenderingInfo::y;
    texturerenderinginfo_type["destination_bottom_left_x"] = &TextureRenderingInfo::destination_bottom_left_x;
    texturerenderinginfo_type["destination_bottom_left_y"] = &TextureRenderingInfo::destination_bottom_left_y;
    texturerenderinginfo_type["destination_bottom_right_x"] = &TextureRenderingInfo::destination_bottom_right_x;
    texturerenderinginfo_type["destination_bottom_right_y"] = &TextureRenderingInfo::destination_bottom_right_y;
    texturerenderinginfo_type["destination_top_left_x"] = &TextureRenderingInfo::destination_top_left_x;
    texturerenderinginfo_type["destination_top_left_y"] = &TextureRenderingInfo::destination_top_left_y;
    texturerenderinginfo_type["destination_top_right_x"] = &TextureRenderingInfo::destination_top_right_x;
    texturerenderinginfo_type["destination_top_right_y"] = &TextureRenderingInfo::destination_top_right_y;
    texturerenderinginfo_type["set_destination"] = &TextureRenderingInfo::set_destination;
    texturerenderinginfo_type["dest_get_quad"] = &TextureRenderingInfo::dest_get_quad;
    texturerenderinginfo_type["dest_set_quad"] = &TextureRenderingInfo::dest_set_quad;
    texturerenderinginfo_type["source_bottom_left_x"] = &TextureRenderingInfo::source_bottom_left_x;
    texturerenderinginfo_type["source_bottom_left_y"] = &TextureRenderingInfo::source_bottom_left_y;
    texturerenderinginfo_type["source_bottom_right_x"] = &TextureRenderingInfo::source_bottom_right_x;
    texturerenderinginfo_type["source_bottom_right_y"] = &TextureRenderingInfo::source_bottom_right_y;
    texturerenderinginfo_type["source_top_left_x"] = &TextureRenderingInfo::source_top_left_x;
    texturerenderinginfo_type["source_top_left_y"] = &TextureRenderingInfo::source_top_left_y;
    texturerenderinginfo_type["source_top_right_x"] = &TextureRenderingInfo::source_top_right_x;
    texturerenderinginfo_type["source_top_right_y"] = &TextureRenderingInfo::source_top_right_y;
    texturerenderinginfo_type["source_get_quad"] = &TextureRenderingInfo::source_get_quad;
    texturerenderinginfo_type["source_set_quad"] = &TextureRenderingInfo::source_set_quad;

    lua.new_usertype<Letter>(
        "Letter",
        "bottom",
        &Letter::bottom,
        "top",
        &Letter::top,
        "get_quad",
        &Letter::get_quad,
        "set_quad",
        &Letter::set_quad,
        "center",
        &Letter::center);

    lua.new_usertype<TextRenderingInfo>(
        "TextRenderingInfo",
        "new",
        sol::initializers(&TextRenderingInfo_ctor, &TextRenderingInfo_ctor2),
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
        "special_texture_id",
        &TextRenderingInfo::special_texture_id,
        "get_dest",
        &TextRenderingInfo::get_dest,
        "get_source",
        &TextRenderingInfo::get_source,
        "text_size",
        &TextRenderingInfo::text_size,
        "rotate",
        &TextRenderingInfo::rotate,
        "set_text",
        &TextRenderingInfo::set_textx,
        "get_font",
        &TextRenderingInfo::get_font,
        "set_font",
        &TextRenderingInfo::set_font);
    /* TextRenderingInfo
    // new
    // TextRenderingInfo:new(string text, float scale_x, float scale_y, VANILLA_TEXT_ALIGNMENT alignment, VANILLA_FONT_STYLE fontstyle)
    // TextRenderingInfo:new(string text, float x, float y, float scale_x, float scale_y, VANILLA_TEXT_ALIGNMENT alignment, VANILLA_FONT_STYLE fontstyle)
    // Creates new TextRenderingInfo that can be used in VanillaRenderContext draw_text
    // For static text, it is better to use one object and call draw_text with it, instead of relaying on draw_text creating this object for you
    */

    lua.create_named_table(
        "WORLD_SHADER",
        "COLOR",
        WorldShader::Colors,
        "TEXTURE",
        WorldShader::Texture,
        "TEXTURE_ALPHA_COLOR",
        WorldShader::TextureAlphaColor,
        "TEXTURE_COLOR",
        WorldShader::TextureColor,
        "TEXTURE_COLORS_WARP",
        WorldShader::TextureColorsWarp,
        "DEFERRED_COLOR_TRANSPARENT",
        WorldShader::DeferredColorTransparent,
        "DEFERRED_TEXTURE_COLOR",
        WorldShader::DeferredTextureColor,
        "DEFERRED_TEXTURE_COLOR_POISONED",
        WorldShader::DeferredTextureColor_Poisoned,
        "DEFERRED_TEXTURE_COLOR_CURSED",
        WorldShader::DeferredTextureColor_Cursed,
        "DEFERRED_TEXTURE_COLOR_POISONED_CURSED",
        WorldShader::DeferredTextureColor_PoisonedCursed,
        "DEFERRED_TEXTURE_COLOR_TRANSPARENT",
        WorldShader::DeferredTextureColor_Transparent,
        "DEFERRED_TEXTURE_COLOR_TRANSPARENT_CORRECTED",
        WorldShader::DeferredTextureColor_TransparentCorrected,
        "DEFERRED_TEXTURE_COLOR_EMISSIVE",
        WorldShader::DeferredTextureColor_Emissive,
        "DEFERRED_TEXTURE_COLOR_EMISSIVE_GLOW",
        WorldShader::DeferredTextureColor_EmissiveGlow,
        "DEFERRED_TEXTURE_COLOR_EMISSIVE_GLOW_HEAVY",
        WorldShader::DeferredTextureColor_EmissiveGlowHeavy,
        "DEFERRED_TEXTURE_COLOR_EMISSIVE_GLOW_BRIGHTNESS",
        WorldShader::DeferredTextureColor_EmissiveGlowBrightness,
        "DEFERRED_TEXTURE_COLOR_EMISSIVE_COLORIZED_GLOW",
        WorldShader::DeferredTextureColor_EmissiveColorizedGlow,
        "DEFERRED_TEXTURE_COLOR_EMISSIVE_COLORIZED_GLOW_DYNAMIC_GLOW",
        WorldShader::DeferredTextureColor_EmissiveColorizedGlow_DynamicGlow,
        "DEFERRED_TEXTURE_COLOR_EMISSIVE_COLORIZED_GLOW_SATURATION",
        WorldShader::DeferredTextureColor_EmissiveColorizedGlow_Saturation);
    /* WORLD_SHADER
    // COLOR
    // Renders a solid color
    // TEXTURE
    // Renders a texture without applying the given color
    // TEXTURE_ALPHA_COLOR
    // Renders a texture by interpreting its red channel as alpha and applying the given color
    // TEXTURE_COLOR
    // The default shader to be used, just renders a texture with transparancy and the given color
    // TEXTURE_COLORS_WARP
    // Renders the texture, with "gamma correction" of the color channels and multiplying everything by the input color alpha only
    // DEFERRED_COLOR_TRANSPARENT
    // Basically same as COLOR but goes through the deferred pipeline
    // DEFERRED_TEXTURE_COLOR
    // Basically same as TEXTURE_COLOR but goes through the deferred pipeline
    // DEFERRED_TEXTURE_COLOR
    // Basically same as TEXTURE_COLOR but goes through the deferred pipeline
    // DEFERRED_TEXTURE_COLOR_POISONED
    // Same as DEFERRED_TEXTURE_COLOR but applies poison color effect
    // DEFERRED_TEXTURE_COLOR_CURSED
    // Same as DEFERRED_TEXTURE_COLOR but applies cursed color effect
    // DEFERRED_TEXTURE_COLOR_POISONED_CURSED
    // Same as DEFERRED_TEXTURE_COLOR but applies poisoned and cursed color effect
    // DEFERRED_TEXTURE_COLOR_TRANSPARENT
    // Basically same as DEFERRED_TEXTURE_COLOR
    // DEFERRED_TEXTURE_COLOR_TRANSPARENT_CORRECTED
    // Same as DEFERRED_TEXTURE_COLOR_TRANSPARENT but applies gamma correction to alpha channel
    // DEFERRED_TEXTURE_COLOR_EMISSIVE
    // Same as DEFERRED_TEXTURE_COLOR but renders to the emissive channel
    // DEFERRED_TEXTURE_COLOR_EMISSIVE_GLOW
    // Same as DEFERRED_TEXTURE_COLOR but renders to the emissive channel with glow
    // DEFERRED_TEXTURE_COLOR_EMISSIVE_GLOW_HEAVY
    // Same as DEFERRED_TEXTURE_COLOR_EMISSIVE_GLOW but renders to the emissive channel with heavy glow
    // DEFERRED_TEXTURE_COLOR_EMISSIVE_GLOW_BRIGHTNESS
    // Same as DEFERRED_TEXTURE_COLOR_EMISSIVE_GLOW_HEAVY but renders glow on top of the texture
    // DEFERRED_TEXTURE_COLOR_EMISSIVE_COLORIZED_GLOW
    // Same as DEFERRED_TEXTURE_COLOR but renders heavy glow behind the texture
    // DEFERRED_TEXTURE_COLOR_EMISSIVE_COLORIZED_GLOW_DYNAMIC_GLOW
    // Basically same as DEFERRED_TEXTURE_COLOR_EMISSIVE_COLORIZED_GLOW
    // DEFERRED_TEXTURE_COLOR_EMISSIVE_COLORIZED_GLOW_SATURATION
    // Same as DEFERRED_TEXTURE_COLOR_EMISSIVE_COLORIZED_GLOW but renders texture as solid color
    */

    auto hudinventory_type = lua.new_usertype<HudInventory>("HudInventory");
    hudinventory_type["enabled"] = &HudInventory::enabled;
    hudinventory_type["health"] = &HudInventory::health;
    hudinventory_type["bombs"] = &HudInventory::bombs;
    hudinventory_type["ropes"] = &HudInventory::ropes;
    hudinventory_type["ankh"] = &HudInventory::ankh;
    hudinventory_type["kapala"] = &HudInventory::kapala;
    hudinventory_type["kapala_blood"] = &HudInventory::kapala_blood;
    hudinventory_type["kapala_sprite"] = &HudInventory::kapala_sprite;
    hudinventory_type["poison"] = &HudInventory::poison;
    hudinventory_type["curse"] = &HudInventory::curse;
    hudinventory_type["elixir"] = &HudInventory::elixir;
    hudinventory_type["crown"] = &HudInventory::crown;
    hudinventory_type["powerup_sprites"] = &HudInventory::powerup_sprites;
    hudinventory_type["item_count"] = &HudInventory::powerup_count;

    auto hudelement_type = lua.new_usertype<HudElement>("HudElement");
    hudelement_type["dim"] = &HudElement::dim;
    hudelement_type["opacity"] = &HudElement::opacity;
    hudelement_type["time_dim"] = &HudElement::time_dim;

    auto hudplayer_type = lua.new_usertype<HudPlayer>("HudPlayer", sol::base_classes, sol::bases<HudElement>());
    hudplayer_type["health"] = &HudPlayer::health;
    hudplayer_type["bombs"] = &HudPlayer::bombs;
    hudplayer_type["ropes"] = &HudPlayer::ropes;

    auto hudmoney_type = lua.new_usertype<HudMoney>("HudMoney", sol::base_classes, sol::bases<HudElement>());
    hudmoney_type["total"] = &HudMoney::total;
    hudmoney_type["counter"] = &HudMoney::counter;
    hudmoney_type["timer"] = &HudMoney::timer;

    auto huddata_type = lua.new_usertype<HudData>("HudData");
    huddata_type["inventory"] = &HudData::inventory;
    huddata_type["udjat"] = &HudData::udjat;
    huddata_type["money_total"] = &HudData::money_total;
    huddata_type["money_counter"] = &HudData::money_counter;
    huddata_type["time_total"] = &HudData::time_total;
    huddata_type["time_level"] = &HudData::time_level;
    huddata_type["world_num"] = &HudData::world_num;
    huddata_type["level_num"] = &HudData::level_num;
    huddata_type["angry_shopkeeper"] = &HudData::angry_shopkeeper;
    huddata_type["seed_shown"] = &HudData::seed_shown;
    huddata_type["seed"] = &HudData::seed;
    huddata_type["opacity"] = &HudData::opacity;
    huddata_type["roll_in"] = &HudData::roll_in;
    huddata_type["players"] = &HudData::players;
    huddata_type["money"] = &HudData::money;
    huddata_type["money_increase_sparkles"] = &HudData::money_increase_sparkles;
    huddata_type["timer"] = &HudData::timer;
    huddata_type["level"] = &HudData::level;
    huddata_type["clover_falling_apart_timer"] = &HudData::clover_falling_apart_timer;
    huddata_type["player_cursed_particles"] = &HudData::player_cursed_particles;
    huddata_type["player_poisoned_particles"] = &HudData::player_poisoned_particles;

    huddata_type["player_highlight"] = &HudData::player_highlight;
    huddata_type["player_heart"] = &HudData::player_heart;
    huddata_type["player_ankh"] = &HudData::player_ankh;
    huddata_type["kapala_icon"] = &HudData::kapala_icon;
    huddata_type["player_crown"] = &HudData::player_crown;
    huddata_type["player_bomb"] = &HudData::player_bomb;
    huddata_type["player_rope"] = &HudData::player_rope;
    huddata_type["udjat_icon"] = &HudData::udjat_icon;
    huddata_type["money_and_time_highlight"] = &HudData::money_and_time_highlight;
    huddata_type["dollar_icon"] = &HudData::dollar_icon;
    huddata_type["hourglass_icon"] = &HudData::hourglass_icon;
    huddata_type["clover_icon"] = &HudData::clover_icon;
    huddata_type["level_highlight"] = &HudData::level_highlight;
    huddata_type["level_icon"] = &HudData::level_icon;
    huddata_type["seed_background"] = &HudData::seed_background;

    auto hud_type = lua.new_usertype<Hud>("Hud");
    hud_type["y"] = &Hud::y;
    hud_type["opacity"] = &Hud::opacity;
    hud_type["data"] = &Hud::data;

    lua["get_hud"] = get_hud;
};
} // namespace NVanillaRender
