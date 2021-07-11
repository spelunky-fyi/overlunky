#include "entities_fx_lua.hpp"

#include "entities_fx.hpp"
#include "entity.hpp"
#include "script/script_impl.hpp"

#include <sol/sol.hpp>

namespace NEntitiesFX
{
void register_usertypes(sol::state& lua, ScriptImpl* script)
{
    lua["Entity"]["as_lavaglow"] = &Entity::as<LavaGlow>;
    lua["Entity"]["as_olmecfloater"] = &Entity::as<OlmecFloater>;

    lua.new_usertype<LavaGlow>(
        "LavaGlow",
        "glow_radius",
        &LavaGlow::glow_radius,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<OlmecFloater>(
        "OlmecFloater",
        "both_floaters_intact",
        &OlmecFloater::both_floaters_intact,
        sol::base_classes,
        sol::bases<Entity, Movable>());
}
} // namespace NEntitiesFX
