#include "hitbox.hpp"

#include "entity.hpp"
#include "rpc.hpp"

#include <sol/sol.hpp>

namespace NHitbox
{
void register_usertypes(sol::state& lua)
{
    /// Gets the hitbox of an entity, use `extrude` to make the hitbox bigger/smaller in all directions and `offset` to offset the hitbox in a given direction
    lua["get_hitbox"] = [](uint32_t uid, float extrude, float offsetx, float offsety)
    {
        auto [sx1, sy1, sx2, sy2] = get_hitbox(uid);
        sx1 -= extrude - offsetx;
        sy1 -= extrude - offsety;
        sx2 += extrude + offsetx;
        sy2 += extrude + offsety;
        return AABB{ sx1, sy1, sx2, sy2 };
    };
    /// Convert an `AABB` to a screen `AABB` that can be directly passed to draw functions
    lua["screen_aabb"] = [](AABB box) -> AABB
    {
        auto [sx1, sy1, sx2, sy2] = screen_aabb(box.left, box.bottom, box.right, box.top);
        return AABB{ sx1, sy2, sx2, sy1 };
    };

    /// Axis-Aligned-Bounding-Box, represents for example a hitbox of an entity or the size of a gui element
    lua.new_usertype<AABB>(
        "AABB",
        "left", &AABB::left,
        "bottom", &AABB::bottom,
        "right", &AABB::right,
        "top", &AABB::top);
}
} // namespace NHitbox
