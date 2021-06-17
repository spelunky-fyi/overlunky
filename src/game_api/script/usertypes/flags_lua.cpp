#include "flags_lua.hpp"

#include <sol/sol.hpp>

namespace NEntityFlags
{
void register_usertypes(sol::state& lua)
{
    lua.create_named_table(
        "ENT_FLAG",
        "INVISIBLE",
        1,
        "INDESTRUCTIBLE_OR_SPECIAL_FLOOR",
        2,
        "SOLID",
        3,
        "PASSES_THROUGH_OBJECTS",
        4,
        "PASSES_THROUGH_EVERYTHING",
        5,
        "TAKE_NO_DAMAGE",
        6,
        "THROWABLE_OR_KNOCKBACKABLE",
        7,
        "IS_PLATFORM",
        8,
        "CLIMBABLE",
        9,
        "NO_GRAVITY",
        10,
        "INTERACT_WITH_WATER",
        11,
        "STUNNABLE",
        12,
        "COLLIDES_WALLS",
        13,
        "INTERACT_WITH_SEMISOLIDS",
        14,
        "CAN_BE_STOMPED",
        15,
        "POWER_STOMPS",
        16,
        "FACING_LEFT",
        17,
        "PICKUPABLE",
        18,
        "USABLE_ITEM",
        19,
        "ENABLE_BUTTON_PROMPT",
        20,
        "INTERACT_WITH_WEBS",
        21,
        "LOCKED",
        22,
        "SHOP_ITEM",
        23,
        "SHOP_FLOOR",
        24,
        "PASSES_THROUGH_PLAYER",
        25,
        "PAUSE_AI_AND_PHYSICS",
        28,
        "DEAD",
        29,
        "HAS_BACKITEM",
        32

    );
}
}; // namespace NEntityFlags
