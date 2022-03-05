#include "entities_chars_lua.hpp"

#include "entities_chars.hpp"
#include "state_structs.hpp"

#include <sol/sol.hpp>

namespace NEntitiesChars
{
void register_usertypes(sol::state& lua)
{
    lua["Entity"]["as_powerupcapable"] = &Entity::as<PowerupCapable>;
    lua["Entity"]["as_player"] = &Entity::as<Player>;

    lua.new_usertype<PowerupCapable>(
        "PowerupCapable",
        "remove_powerup",
        &PowerupCapable::remove_powerup,
        "give_powerup",
        &PowerupCapable::give_powerup,
        "has_powerup",
        &PowerupCapable::has_powerup,
        "get_powerups",
        &PowerupCapable::get_powerups,
        "unequip_backitem",
        &PowerupCapable::unequip_backitem,
        "worn_backitem",
        &PowerupCapable::worn_backitem,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<Inventory>(
        "Inventory",
        "money",
        &Inventory::money,
        "bombs",
        &Inventory::bombs,
        "ropes",
        &Inventory::ropes,
        "player_slot", /*
        &Inventory::player_slot,
                       */
        sol::property([](Inventory& i) -> int8_t
                      {
                          if (i.player_slot >= 0)
                              return i.player_slot + 1;
                          else
                              return i.player_slot; },
                      [](Inventory& i, int8_t s)
                      {
                          if (s > 0)
                              i.player_slot = s - 1;
                          else
                              i.player_slot = -1;
                      }),
        "poison_tick_timer",
        &Inventory::poison_tick_timer,
        "cursed",
        &Inventory::cursed,
        "elixir_buff",
        &Inventory::elixir_buff,
        "health",
        &Inventory::health,
        "kapala_blood_amount",
        &Inventory::kapala_blood_amount,
        "time_of_death",
        &Inventory::time_of_death,
        "held_item",
        &Inventory::held_item,
        "held_item_metadata",
        &Inventory::held_item_metadata,
        "mount_type",
        &Inventory::mount_type,
        "mount_metadata",
        &Inventory::mount_metadata,
        "kills_level",
        &Inventory::kills_level,
        "kills_total",
        &Inventory::kills_total,
        "collected_money_total",
        &Inventory::collected_money_total,
        "collected_money",
        &Inventory::collected_money,
        "collected_money_values",
        &Inventory::collected_money_values,
        "killed_enemies",
        &Inventory::killed_enemies,
        "companion_count",
        &Inventory::companion_count,
        "companions",
        &Inventory::companions,
        "companion_held_items",
        &Inventory::companion_held_items,
        "companion_held_item_metadatas",
        &Inventory::companion_held_item_metadatas,
        "companion_trust",
        &Inventory::companion_trust,
        "companion_health",
        &Inventory::companion_health,
        "companion_poison_tick_timers",
        &Inventory::companion_poison_tick_timers,
        "is_companion_cursed",
        &Inventory::is_companion_cursed);

    lua.new_usertype<Ai>(
        "Ai",
        "target",
        &Ai::target,
        "target_uid",
        &Ai::target_uid,
        "timer",
        &Ai::timer,
        "state",
        &Ai::state,
        "trust",
        &Ai::trust,
        "whipped",
        &Ai::whipped);

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
        "ai",
        &Player::ai,
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
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable>());

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
