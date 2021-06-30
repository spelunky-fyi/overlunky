#include "entities_items_lua.hpp"

#include "entities_items.hpp"
#include "entity.hpp"
#include "script/script_impl.hpp"

#include <sol/sol.hpp>

namespace NEntitiesItems
{
void register_usertypes(sol::state& lua, ScriptImpl* script)
{
    lua["Entity"]["as_container"] = &Entity::as<Container>;

    lua["Entity"]["as_bomb"] = &Entity::as<Bomb>;
    lua["Entity"]["as_backpack"] = &Entity::as<Backpack>;
    lua["Entity"]["as_jetpack"] = &Entity::as<Jetpack>;
    lua["Entity"]["as_hoverpack"] = &Entity::as<Hoverpack>;
    lua["Entity"]["as_cape"] = &Entity::as<Cape>;
    lua["Entity"]["as_vlads_cape"] = &Entity::as<VladsCape>;
    lua["Entity"]["as_kapala_powerup"] = &Entity::as<KapalaPowerup>;
    lua["Entity"]["as_mattock"] = &Entity::as<Mattock>;
    lua["Entity"]["as_gun"] = &Entity::as<Gun>;
    lua["Entity"]["as_walltorchflame"] = &Entity::as<WalltorchFlame>;

    lua.new_usertype<Container>(
        "Container",
        "inside",
        &Container::inside,
        "timer",
        &Container::timer,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<Bomb>("Bomb", "scale_hor", &Bomb::scale_hor, "scale_ver", &Bomb::scale_ver, sol::base_classes, sol::bases<Entity, Movable>());

    lua.new_usertype<Backpack>(
        "Backpack",
        "explosion_trigger",
        &Backpack::explosion_trigger,
        "explosion_timer",
        &Backpack::explosion_timer,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<Jetpack>(
        "Jetpack",
        "flame_on",
        &Jetpack::flame_on,
        "fuel",
        &Jetpack::fuel,
        sol::base_classes,
        sol::bases<Entity, Movable, Backpack>());

    lua.new_usertype<Hoverpack>(
        "Hoverpack",
        "is_on",
        &Hoverpack::is_on,
        sol::base_classes,
        sol::bases<Entity, Movable, Backpack>());

    lua.new_usertype<Cape>("Cape", "floating_down", &VladsCape::floating_down, sol::base_classes, sol::bases<Entity, Movable>());

    lua.new_usertype<VladsCape>("VladsCape", "can_double_jump", &VladsCape::can_double_jump, sol::base_classes, sol::bases<Entity, Movable, Cape>());

    lua.new_usertype<KapalaPowerup>(
        "KapalaPowerup",
        "amount_of_blood",
        &KapalaPowerup::amount_of_blood,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<Mattock>(
        "Mattock",
        "remaining",
        &Mattock::remaining,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<Gun>(
        "Gun",
        "cooldown",
        &Gun::cooldown,
        "shots",
        &Gun::shots,
        "shots2",
        &Gun::shots2,
        "in_chamber",
        &Gun::in_chamber,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<WalltorchFlame>(
        "WalltorchFlame",
        "emitted_light",
        &WalltorchFlame::emitted_light,
        "flare_up_size",
        &WalltorchFlame::flare_up_size,
        sol::base_classes,
        sol::bases<Entity, Movable>());
}
} // namespace NEntitiesItems
