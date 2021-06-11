#include "char_state.hpp"

#include <sol/sol.hpp>

namespace NCharacterState
{
void register_usertypes(sol::state& lua)
{
    lua.create_named_table(
        "CHAR_STATE",
        "FLAILING",
        0,
        "STANDING",
        1,
        "SITTING",
        2,
        "HANGING",
        4,
        "DUCKING",
        5,
        "CLIMBING",
        6,
        "PUSHING",
        7,
        "JUMPING",
        8,
        "FALLING",
        9,
        "DROPPING",
        10,
        "ATTACKING",
        12,
        "THROWING",
        17,
        "STUNNED",
        18,
        "ENTERING",
        19,
        "LOADING",
        20,
        "EXITING",
        21,
        "DYING",
        22);
}
} // namespace NCharacterState