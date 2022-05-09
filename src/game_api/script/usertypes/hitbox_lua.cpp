#include "hitbox_lua.hpp"

#include "rpc.hpp"

#include <sol/sol.hpp>

namespace NHitbox
{
void register_usertypes(sol::state& lua)
{
    static auto fixup_hitbox = [](AABB hitbox, sol::optional<float> extrude, sol::optional<float> offsetx, sol::optional<float> offsety)
    {
        auto [sx1, sy1, sx2, sy2] = hitbox;
        if (extrude)
        {
            sx1 -= extrude.value();
            sy1 -= extrude.value();
            sx2 += extrude.value();
            sy2 += extrude.value();
        }
        if (offsetx)
        {
            sx1 += offsetx.value();
            sx2 += offsetx.value();
        }
        if (offsety)
        {
            sy1 += offsety.value();
            sy2 += offsety.value();
        }
        return AABB{sx1, sy1, sx2, sy2};
    };
    /// Gets the hitbox of an entity, use `extrude` to make the hitbox bigger/smaller in all directions and `offset` to offset the hitbox in a given direction
    lua["get_hitbox"] = [](uint32_t uid, sol::optional<float> extrude, sol::optional<float> offsetx, sol::optional<float> offsety) -> AABB
    {
        auto [sx1, sy1, sx2, sy2] = get_hitbox(uid, false);
        return fixup_hitbox(AABB{sx1, sy1, sx2, sy2}, extrude, offsetx, offsety);
    };
    /// Same as `get_hitbox` but based on `get_render_position`
    lua["get_render_hitbox"] = [](uint32_t uid, sol::optional<float> extrude, sol::optional<float> offsetx, sol::optional<float> offsety) -> AABB
    {
        auto [sx1, sy1, sx2, sy2] = get_hitbox(uid, true);
        return fixup_hitbox(AABB{sx1, sy1, sx2, sy2}, extrude, offsetx, offsety);
    };
    /// Convert an `AABB` to a screen `AABB` that can be directly passed to draw functions
    lua["screen_aabb"] = [](AABB box) -> AABB
    {
        auto [sx1, sy1, sx2, sy2] = screen_aabb(box.left, box.top, box.right, box.bottom);
        return AABB{sx1, sy1, sx2, sy2};
    };

    lua.new_usertype<Vec2>(
        "Vec2",
        sol::constructors<Vec2(), Vec2(const Vec2&), Vec2(float, float)>{},
        sol::meta_function::addition,
        &Vec2::operator+,
        sol::meta_function::subtraction,
        &Vec2::operator-,
        sol::meta_function::equal_to,
        &Vec2::operator==,
        sol::meta_function::multiplication,
        &Vec2::operator*,
        sol::meta_function::division,
        &Vec2::operator/,
        "x",
        &Vec2::x,
        "y",
        &Vec2::y,
        "rotate",
        &Vec2::rotate,
        "split",
        &Vec2::operator std::pair<float, float>);

    /// Axis-Aligned-Bounding-Box, represents for example a hitbox of an entity or the size of a gui element
    lua.new_usertype<AABB>(
        "AABB",
        sol::constructors<AABB(), AABB(const AABB&), AABB(float, float, float, float)>{},
        "left",
        &AABB::left,
        "bottom",
        &AABB::bottom,
        "right",
        &AABB::right,
        "top",
        &AABB::top,
        "overlaps_with",
        &AABB::overlaps_with,
        "abs",
        &AABB::abs,
        "extrude",
        &AABB::extrude,
        "offset",
        &AABB::offset,
        "area",
        &AABB::area,
        "center",
        &AABB::center,
        "width",
        &AABB::width,
        "height",
        &AABB::height);

    lua.new_usertype<Quad>(
        "Quad",
        sol::constructors<Quad(), Quad(const Quad&), Quad(const AABB&), Quad(float, float, float, float, float, float, float, float), Quad(Vec2&, Vec2&, Vec2&, Vec2&)>{},
        "bottom_left_x",
        &Quad::bottom_left_x,
        "bottom_left_y",
        &Quad::bottom_left_y,
        "bottom_right_x",
        &Quad::bottom_right_x,
        "bottom_right_y",
        &Quad::bottom_right_y,
        "top_right_x",
        &Quad::top_right_x,
        "top_right_y",
        &Quad::top_right_y,
        "top_left_x",
        &Quad::top_left_x,
        "top_left_y",
        &Quad::top_left_y,
        "get_AABB",
        &Quad::get_AABB,
        "offset",
        &Quad::offset,
        "rotate",
        &Quad::rotate,
        "flip_horizontal",
        &Quad::flip_horizontal,
        "flip_vertically",
        &Quad::flip_vertically,
        "split",
        &Quad::split);
}
} // namespace NHitbox
