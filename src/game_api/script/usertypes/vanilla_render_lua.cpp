#include "vanilla_render_lua.hpp"

#include <algorithm>   // for max
#include <cstdlib>     // for abs
#include <locale>      // for num_put
#include <new>         // for operator new
#include <optional>    // for nullopt
#include <sol/sol.hpp> // for global_table, proxy_key_t, optional, state
#include <tuple>       // for get
#include <type_traits> // for move, declval

#include "render_api.hpp" // for TextureRenderingInfo, WorldShader, TextRen...
#include "state.hpp"      // for enum_to_layer
#include "texture.hpp"    // for Texture, get_texture

void VanillaRenderContext::draw_text(const std::string& text, float x, float y, float scale_x, float scale_y, Color color, uint32_t alignment, uint32_t fontstyle)
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

// get intersection point of two lines
Vec2 intersection(const Vec2 A, const Vec2 B, const Vec2 C, const Vec2 D)
{
    float a = B.y - A.y;
    float b = A.x - B.x;
    float c = a * (A.x) + b * (A.y);
    // Line CD represented as a2x + b2y = c2
    float a1 = D.y - C.y;
    float b1 = C.x - D.x;
    float c1 = a1 * (C.x) + b1 * (C.y);

    float det = a * b1 - a1 * b;

    if (det == 0)
        return {INFINITY, INFINITY};

    return Vec2{(b1 * c - b * c1) / det, (a * c1 - a1 * c) / det};
}

enum class CORNER_FINISH
{
    CUT,
    ADAPTIVE,
    REAL,
};

auto g_angle_style = CORNER_FINISH::ADAPTIVE;

// get a Quad to fill out the corner between two lines and fix their overlap
Quad get_corner_quad(Quad& line1, Quad& line2)
{
    Vec2 A{line1.top_left_x, line1.top_left_y};
    Vec2 B{line1.top_right_x, line1.top_right_y};

    Vec2 C{line2.top_right_x, line2.top_right_y};
    Vec2 D{line2.top_left_x, line2.top_left_y};
    Vec2 corner1 = intersection(A, B, C, D);
    if (corner1.x == INFINITY || corner1.y == INFINITY)
        return {};

    Vec2 E{line1.bottom_left_x, line1.bottom_left_y};
    Vec2 F{line1.bottom_right_x, line1.bottom_right_y};

    Vec2 G{line2.bottom_right_x, line2.bottom_right_y};
    Vec2 H{line2.bottom_left_x, line2.bottom_left_y};
    Vec2 corner2 = intersection(E, F, G, H);
    if (corner2.x == INFINITY || corner2.y == INFINITY)
        return {};

    auto get_angle = [](const Vec2 a, Vec2& b, const Vec2 c) -> float
    {
        Vec2 ab = b - a;
        Vec2 bc = c - b;
        float angle = std::abs(std::atan((bc.y * ab.x - bc.x * ab.y) / (bc.x * ab.x + bc.y * ab.y)));
        return angle;
    };

    Vec2 ab = corner1 - B;
    Vec2 bc = D - corner1;
    float true_angle = std::atan2((bc.y * ab.x - bc.x * ab.y), (bc.x * ab.x + bc.y * ab.y));

    if (true_angle < 0) // check which one is the inner and which outer corner
    {
        if (true_angle < -1) // only do the test for one quadrant
        {
            // test angle for the outer corner
            float angle = get_angle(B, corner1, D);
            if (angle < 0.09f) // too small angle is problematic, and it get's worse when the lines almost overlap generating the corner in wrong spot
                return {};

            if (g_angle_style == CORNER_FINISH::ADAPTIVE && angle < 1)
            {
                auto offset = corner1 - corner2;
                offset = offset * angle;
                corner1 -= offset;
            }
        }
        if (g_angle_style == CORNER_FINISH::CUT)
            corner1 = D;

        line1.bottom_right_x = corner2.x;
        line1.bottom_right_y = corner2.y;
        line2.bottom_left_x = corner2.x;
        line2.bottom_left_y = corner2.y;
        return Quad{B, corner2, D, corner1};
    }
    else
    {
        // same as above
        if (true_angle > 1)
        {
            float angle = get_angle(F, corner2, H);
            if (angle < 0.09f)
                return {};

            if (g_angle_style == CORNER_FINISH::ADAPTIVE && angle < 1)
            {
                auto offset = corner2 - corner1;
                offset = offset * angle;
                corner2 -= offset;
            }
        }
        if (g_angle_style == CORNER_FINISH::CUT)
            corner2 = H;

        line1.top_right_x = corner1.x;
        line1.top_right_y = corner1.y;
        line2.top_left_x = corner1.x;
        line2.top_left_y = corner1.y;
        return Quad{F, corner1, H, corner2};
    }
};
// convert two points into Quad (rectangle) with given `thickness` (height)
Quad get_line_quad(const Vec2 A, const Vec2 B, float thickness)
{
    thickness *= .001f; // scale it to use more of a whole values
    float axis_AB_angle = std::atan2((B.y - A.y), (B.x) - (A.x));
    float hypotenuse = (float)std::sqrt(std::pow(B.x - A.x, 2) + std::pow(B.y - A.y, 2));

    // make it straight and then rotate because i'm stupid
    Quad dest{AABB{A.x, A.y + thickness / 2, (A.x + hypotenuse), A.y - thickness / 2}};
    dest.rotate(axis_AB_angle, A.x, A.y);
    return dest;
};

void VanillaRenderContext::draw_screen_rect(const AABB& rect, float thickness, Color color, sol::optional<float> angle, sol::optional<float> px, sol::optional<float> py)
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
void VanillaRenderContext::draw_screen_rect_filled(const AABB& rect, Color color, sol::optional<float> angle, sol::optional<float> px, sol::optional<float> py)
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
    for (auto line : draw_list)
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
    if (points.size() % 2 != 0) // not even number of points so the last pice is triangle
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

// For the custom constructor
void TextRenderingInfo_ctor(TextRenderingInfo& uninitialized_memory, const std::u16string text, float x, float y, float scale_x, float scale_y, uint32_t alignment, uint32_t fontstyle)
{
    new (&uninitialized_memory) TextRenderingInfo{};
    uninitialized_memory.set_text(text, x, y, scale_x, scale_y, alignment, fontstyle);
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
        static_cast<void (VanillaRenderContext::*)(TEXTURE, const Quad&, const Quad&, Color)>(&VanillaRenderContext::draw_screen_texture));
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

    /// Used in [set_callback](#set_callback) ON.RENDER_* callbacks, [set_post_render](#set_post_render), [set_post_render_screen](#set_post_render_screen), [set_pre_render](#set_pre_render), [set_pre_render_screen](#set_pre_render_screen)
    lua.new_usertype<VanillaRenderContext>(
        "VanillaRenderContext",
        "draw_text",
        draw_text,
        "draw_text_size",
        &VanillaRenderContext::draw_text_size,
        "draw_screen_texture",
        draw_screen_texture,
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
        draw_world_texture);

    auto texturerenderinginfo_type = lua.new_usertype<TextureRenderingInfo>("TextureRenderingInfo");
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
        sol::initializers(&TextRenderingInfo_ctor),
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
        "font",
        &TextRenderingInfo::font,
        "get_dest",
        &TextRenderingInfo::get_dest,
        "get_source",
        &TextRenderingInfo::get_source,
        "text_size",
        &TextRenderingInfo::text_size,
        "rotate",
        &TextRenderingInfo::rotate,
        "set_text",
        &TextRenderingInfo::set_textx);
    /* TextRenderingInfo
    // new
    // TextRenderingInfo:new(string text, float x, float y, float scale_x, float scale_y, int alignment, int fontstyle)
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
};
} // namespace NVanillaRender
