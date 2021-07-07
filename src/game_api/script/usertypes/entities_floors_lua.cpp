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
    lua["Entity"]["as_door"] = &Entity::as<Door>;
    lua["Entity"]["as_arrowtrap"] = &Entity::as<Arrowtrap>;

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
        sol::base_classes,
        sol::bases<Entity>());

    lua.new_usertype<Door>(
        "Door",
        "counter",
        &Door::counter,
        "fx_button",
        &Door::fx_button,
        sol::base_classes,
        sol::bases<Entity>());

    lua.new_usertype<Arrowtrap>(
        "Arrowtrap",
        "arrow_shot",
        &Arrowtrap::arrow_shot,
        "rearm",
        &Arrowtrap::rearm,
        sol::base_classes,
        sol::bases<Entity>());
}
} // namespace NEntitiesFloors
