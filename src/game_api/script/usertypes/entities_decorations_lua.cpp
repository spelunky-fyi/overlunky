#include "entities_decorations_lua.hpp"

#include "entities_decorations.hpp"
#include "state_structs.hpp"

#include <sol/sol.hpp>

namespace NEntitiesDecorations
{
void register_usertypes(sol::state& lua)
{
    lua["Entity"]["as_crossbeam"] = &Entity::as<CrossBeam>;
    lua["Entity"]["as_destructiblebg"] = &Entity::as<DestructibleBG>;
    lua["Entity"]["as_palacesign"] = &Entity::as<PalaceSign>;
    lua["Entity"]["as_decoregeneratingblock"] = &Entity::as<DecoRegeneratingBlock>;

    lua.new_usertype<CrossBeam>(
        "CrossBeam",
        "attached_to_side_uid",
        &CrossBeam::attached_to_side_uid,
        "attached_to_top_uid",
        &CrossBeam::attached_to_top_uid,
        sol::base_classes,
        sol::bases<Entity>());

    lua.new_usertype<DestructibleBG>(
        "DestructibleBG",
        sol::base_classes,
        sol::bases<Entity>());

    lua.new_usertype<PalaceSign>(
        "PalaceSign",
        "illumination",
        &PalaceSign::illumination,
        "arrow_illumination",
        &PalaceSign::arrow_illumination,
        "arrow_change_timer",
        &PalaceSign::arrow_change_timer,
        sol::base_classes,
        sol::bases<Entity>());

    lua.new_usertype<DecoRegeneratingBlock>(
        "DecoRegeneratingBlock",
        sol::base_classes,
        sol::bases<Entity>());
}
}; // namespace NEntitiesDecorations
