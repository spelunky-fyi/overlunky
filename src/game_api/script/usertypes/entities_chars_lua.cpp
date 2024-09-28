#include "entities_chars_lua.hpp"

#include <algorithm>   // for max
#include <cstdint>     // for int8_t
#include <new>         // for operator new
#include <sol/sol.hpp> // for global_table, proxy_key_t, data_t, state
#include <string>      // for operator==, allocator
#include <tuple>       // for get
#include <type_traits> // for move, declval
#include <utility>     // for min, max

#include "entities_chars.hpp" // for Ai, Player, PowerupCapable, get_charac...
#include "entity.hpp"         // for Entity
#include "illumination.hpp"   // IWYU pragma: keep
#include "items.hpp"          // for Inventory, Inventory::acquired_powerups
#include "sound_manager.hpp"  // IWYU pragma: keep
#include "state_structs.hpp"  // IWYU pragma: keep

class Movable;

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

    /// Used in Player, PlayerGhost and Items
    auto inventory_type = lua.new_usertype<Inventory>("Inventory");
    inventory_type["money"] = &Inventory::money;
    inventory_type["bombs"] = &Inventory::bombs;
    inventory_type["ropes"] = &Inventory::ropes;
    inventory_type["player_slot"] = sol::property([](Inventory& i) -> int8_t
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
                                                  });
    inventory_type["poison_tick_timer"] = &Inventory::poison_tick_timer;
    inventory_type["cursed"] = &Inventory::cursed;
    inventory_type["elixir_buff"] = &Inventory::elixir_buff;
    inventory_type["health"] = &Inventory::health;
    inventory_type["kapala_blood_amount"] = &Inventory::kapala_blood_amount;
    inventory_type["time_of_death"] = &Inventory::time_of_death;
    inventory_type["held_item"] = &Inventory::held_item;
    inventory_type["held_item_metadata"] = &Inventory::held_item_metadata;
    inventory_type["saved_pets_count"] = &Inventory::saved_pets_count;
    inventory_type["mount_type"] = &Inventory::mount_type;
    inventory_type["mount_metadata"] = &Inventory::mount_metadata;
    inventory_type["kills_level"] = &Inventory::kills_level;
    inventory_type["kills_total"] = &Inventory::kills_total;
    inventory_type["collected_money_total"] = &Inventory::collected_money_total;
    inventory_type["collected_money_count"] = &Inventory::collected_money_count;
    inventory_type["collected_money"] = &Inventory::collected_money;
    inventory_type["collected_money_values"] = &Inventory::collected_money_values;
    inventory_type["killed_enemies"] = &Inventory::killed_enemies;
    inventory_type["companion_count"] = &Inventory::companion_count;
    inventory_type["companions"] = &Inventory::companions;
    inventory_type["companion_held_items"] = &Inventory::companion_held_items;
    inventory_type["companion_held_item_metadatas"] = &Inventory::companion_held_item_metadatas;
    inventory_type["companion_trust"] = &Inventory::companion_trust;
    inventory_type["companion_health"] = &Inventory::companion_health;
    inventory_type["companion_poison_tick_timers"] = &Inventory::companion_poison_tick_timers;
    inventory_type["is_companion_cursed"] = &Inventory::is_companion_cursed;
    inventory_type["acquired_powerups"] = &Inventory::acquired_powerups;

    /// Used in Player
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
        "last_state",
        &Ai::last_state,
        "trust",
        &Ai::trust,
        "whipped",
        &Ai::whipped,
        "walk_pause_timer",
        &Ai::walk_pause_timer);

    auto player_type = lua.new_usertype<Player>("Player", sol::base_classes, sol::bases<Entity, Movable, PowerupCapable>());
    player_type["inventory"] = &Player::inventory_ptr;
    player_type["emitted_light"] = &Player::emitted_light;
    player_type["linked_companion_parent"] = &Player::linked_companion_parent;
    player_type["linked_companion_child"] = &Player::linked_companion_child;
    player_type["ai"] = &Player::ai;
    player_type["input"] = &Player::input_ptr;
    player_type["basecamp_button_entity"] = &Player::basecamp_button_entity;
    player_type["special_sound"] = &Player::special_sound;
    player_type["jump_lock_timer"] = &Player::jump_lock_timer;
    player_type["coyote_timer"] = &Player::coyote_timer;
    player_type["swim_timer"] = &Player::swim_timer;
    player_type["hired_hand_name"] = &Player::hired_hand_name;
    player_type["set_jetpack_fuel"] = &Player::set_jetpack_fuel;
    player_type["kapala_blood_amount"] = &Player::kapala_blood_amount;
    player_type["get_name"] = &Player::get_name;
    player_type["get_short_name"] = &Player::get_short_name;
    player_type["get_heart_color"] = &Player::get_heart_color;
    player_type["is_female"] = &Player::is_female;
    player_type["set_heart_color"] = &Player::set_heart_color;
    player_type["let_go"] = &Player::let_go;

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
