#include "hitbox_lua.hpp"

#include <algorithm>   // for max
#include <cstdint>     // for uint32_t
#include <new>         // for operator new
#include <sol/sol.hpp> // for optional, global_table, proxy_key_t, data_t
#include <string>      // for allocator, operator==
#include <tuple>       // for get
#include <type_traits> // for move, declval
#include <utility>     // for min, max, get, pair

#include "entity.hpp" // for get_hitbox
#include "math.hpp"   // for AABB, Vec2, Quad, AABB::bottom, AABB::left
#include "rpc.hpp"    // for screen_aabb

namespace NHitbox
{
void register_usertypes(sol::state& lua)
{
    static auto fixup_hitbox = [](AABB hitbox, sol::optional<float> extrude, sol::optional<float> offsetx, sol::optional<float> offsety)
    {
        if (extrude)
        {
            hitbox.extrude(extrude.value());
        }
        hitbox.offset(offsetx.value_or(0), offsety.value_or(0));
        return hitbox;
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

    /// Simple object to hold pair of coordinates
    lua.new_usertype<Vec2>(
        "Vec2",
        sol::constructors<Vec2(), Vec2(const Vec2&), Vec2(float, float)>{},
        sol::meta_function::addition,
        &Vec2::operator+,
        sol::meta_function::subtraction,
        static_cast<Vec2 (Vec2::*)(const Vec2&) const>(&Vec2::operator-),
        sol::meta_function::equal_to,
        &Vec2::operator==,
        sol::meta_function::multiplication,
        static_cast<Vec2 (Vec2::*)(const Vec2&) const>(&Vec2::operator*),
        sol::meta_function::division,
        static_cast<Vec2 (Vec2::*)(const Vec2&) const>(&Vec2::operator/),
        "x",
        &Vec2::x,
        "y",
        &Vec2::y,
        "rotate",
        &Vec2::rotate,
        "split",
        // &Vec2::split); // for the autodoc
        &Vec2::operator std::pair<float, float>);

    const auto extrude = sol::overload(
        static_cast<AABB& (AABB::*)(float)>(&AABB::extrude),
        static_cast<AABB& (AABB::*)(float, float)>(&AABB::extrude));

    /// Axis-Aligned-Bounding-Box, represents for example a hitbox of an entity or the size of a gui element
    lua.new_usertype<AABB>(
        "AABB",
        sol::constructors<AABB(), AABB(const AABB&), AABB(const Vec2&, const Vec2&), AABB(float, float, float, float)>{},
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
        extrude,
        "offset",
        &AABB::offset,
        "area",
        &AABB::area,
        "center",
        &AABB::center,
        "width",
        &AABB::width,
        "height",
        &AABB::height,
        "split",
        // &Vec2::split); // for the autodoc
        &AABB::operator std::tuple<float, float, float, float>);

    auto offset = sol::overload(
        static_cast<Triangle& (Triangle::*)(const Vec2&)>(&Triangle::offset),
        static_cast<Triangle& (Triangle::*)(float, float)>(&Triangle::offset));

    lua.new_usertype<Triangle>(
        "Triangle",
        sol::constructors<Triangle(), Triangle(const Triangle&), Triangle(Vec2&, Vec2&, Vec2&), Triangle(float, float, float, float, float, float)>{},
        "A",
        &Triangle::A,
        "B",
        &Triangle::B,
        "C",
        &Triangle::C,
        "offset",
        offset,
        "rotate",
        &Triangle::rotate,
        "center",
        &Triangle::center,
        "get_angles",
        &Triangle::get_angles,
        "scale",
        &Triangle::scale,
        "split",
        // &Triangle::split); // for the autodoc
        &Triangle::operator std::tuple<Vec2, Vec2, Vec2>);

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
        "flip_horizontally",
        &Quad::flip_horizontally,
        "flip_vertically",
        &Quad::flip_vertically,
        "split",
        // &Quad::split); // for the autodoc
        &Quad::operator std::tuple<Vec2, Vec2, Vec2, Vec2>);
}
} // namespace NHitbox
