#include "entities_floors_lua.hpp"

#include "entities_floors.hpp"
#include "entity.hpp"
#include "script/script_impl.hpp"

#include <sol/sol.hpp>

namespace NEntitiesFloors
{
void register_usertypes(sol::state& lua, ScriptImpl* script)
{
    lua["Entity"]["as_floor"] = &Entity::as<Floor>;

    lua.new_usertype<Floor>(
        "Floor",
        "deco_top",
        &Floor::deco_top,
        "deco_bottom",
        &Floor::deco_bottom,
        "deco_left",
        &Floor::deco_left,
        "deco_right",
        &Floor::deco_right,
        "fix_decorations",
        &Floor::fix_decorations,
        "add_decoration",
        &Floor::add_decoration,
        "remove_decoration",
        &Floor::remove_decoration,
        sol::base_classes,
        sol::bases<Entity>());

    lua.create_named_table(
        "FLOOR_SIDE",
        "TOP",
        FLOOR_SIDE::TOP,
        "BOTTOM",
        FLOOR_SIDE::BOTTOM,
        "LEFT",
        FLOOR_SIDE::LEFT,
        "RIGHT",
        FLOOR_SIDE::RIGHT);
}
} // namespace NEntitiesFloors
