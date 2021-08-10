#include "hitbox_lua.hpp"

#include "entity.hpp"
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
    lua["get_hitbox"] = [](uint32_t uid, sol::optional<float> extrude, sol::optional<float> offsetx, sol::optional<float> offsety)
    {
        auto [sx1, sy1, sx2, sy2] = get_hitbox(uid, false);
        return fixup_hitbox(AABB{sx1, sy1, sx2, sy2}, extrude, offsetx, offsety);
    };
    /// Same as `get_hitbox` but based on `get_render_position`
    lua["get_render_hitbox"] = [](uint32_t uid, sol::optional<float> extrude, sol::optional<float> offsetx, sol::optional<float> offsety)
    {
        auto [sx1, sy1, sx2, sy2] = get_hitbox(uid, true);
        return fixup_hitbox(AABB{sx1, sy1, sx2, sy2}, extrude, offsetx, offsety);
    };
    /// Convert an `AABB` to a screen `AABB` that can be directly passed to draw functions
    lua["screen_aabb"] = [](AABB box) -> AABB
    {
        auto [sx1, sy1, sx2, sy2] = screen_aabb(box.left, box.bottom, box.right, box.top);
        return AABB{sx1, sy2, sx2, sy1};
    };

    /// Axis-Aligned-Bounding-Box, represents for example a hitbox of an entity or the size of a gui element
    lua.new_usertype<AABB>(
        "AABB",
        sol::constructors<AABB(), AABB(float, float, float, float)>{},
        "left",
        &AABB::left,
        "bottom",
        &AABB::bottom,
        "right",
        &AABB::right,
        "top",
        &AABB::top);
}
} // namespace NHitbox
