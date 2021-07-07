#include "entities_activefloors_lua.hpp"

#include "entities_activefloors.hpp"
#include "script/script_impl.hpp"

#include <sol/sol.hpp>

namespace NEntitiesActiveFloors
{
void register_usertypes(sol::state& lua, ScriptImpl* script)
{
    lua["Entity"]["as_crushtrap"] = &Entity::as<Crushtrap>;
    lua["Entity"]["as_olmec"] = &Entity::as<Olmec>;

    lua.new_usertype<Crushtrap>("Crushtrap", "dirx", &Crushtrap::dirx, "diry", &Crushtrap::diry, sol::base_classes, sol::bases<Entity, Movable>());

    lua.new_usertype<Olmec>(
        "Olmec",
        "target_uid",
        &Olmec::target_uid,
        "attack_phase",
        &Olmec::attack_phase,
        "attack_timer",
        &Olmec::attack_timer,
        "ai_timer",
        &Olmec::ai_timer,
        "move_direction",
        &Olmec::move_direction,
        "jump_timer",
        &Olmec::jump_timer,
        "phase1_amount_of_bomb_salvos",
        &Olmec::phase1_amount_of_bomb_salvos,
        "unknown_attack_state",
        &Olmec::unknown_attack_state,
        "broken_floaters",
        &Olmec::broken_floaters,
        sol::base_classes,
        sol::bases<Entity, Movable>());
    /* Olmec
            int broken_floaters()
        */
}
} // namespace NEntitiesActiveFloors
