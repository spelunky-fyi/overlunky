#include "entities_chars_lua.hpp"

#include "entities_chars.hpp"
#include "script/script_impl.hpp"

#include <sol/sol.hpp>

namespace NEntitiesChars
{
void register_usertypes(sol::state& lua, ScriptImpl* script)
{
    lua.new_usertype<Inventory>(
        "Inventory",
        "money",
        &Inventory::money,
        "bombs",
        &Inventory::bombs,
        "ropes",
        &Inventory::ropes,
        "kills_level",
        &Inventory::kills_level,
        "kills_total",
        &Inventory::kills_total,
        "collected_money_total",
        &Inventory::collected_money_total);

    lua.new_usertype<Player>(
        "Player",
        "inventory",
        &Player::inventory_ptr,
        "emitted_light",
        &Player::emitted_light,
        "linked_companion_parent",
        &Player::linked_companion_parent,
        "linked_companion_child",
        &Player::linked_companion_child,
        "set_jetpack_fuel",
        &Player::set_jetpack_fuel,
        "kapala_blood_amount",
        &Player::kapala_blood_amount,
        "get_name",
        &Player::get_name,
        "get_short_name",
        &Player::get_short_name,
        "get_heart_color",
        &Player::get_heart_color,
        "is_female",
        &Player::is_female,
        "set_heart_color",
        &Player::set_heart_color,
        "remove_powerup",
        &Player::remove_powerup,
        "give_powerup",
        &Player::give_powerup,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    /// Same as `Player.get_name`
    lua["get_character_name"] = get_character_name;
    /// Same as `Player.get_short_name`
    lua["get_character_short_name"] = get_character_short_name;
    /// Same as `Player.get_heart_color`
    lua["get_character_heart_color"] = get_character_heart_color;
    /// Same as `Player.is_female`
    lua["is_character_female"] = is_character_female;
    /// Same as `Player.set_heart_color`
    lua["set_character_heart_color"] = set_character_heart_color;
}
} // namespace NEntitiesChars
