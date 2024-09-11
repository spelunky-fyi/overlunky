#include "entities_backgrounds_lua.hpp"

#include <algorithm>   // for max
#include <new>         // for operator new
#include <sol/sol.hpp> // for proxy_key_t, state, basic_table_...
#include <string>      // for operator==, allocator
#include <tuple>       // for get
#include <type_traits> // for move, declval
#include <utility>     // for min, max

#include "entities_backgrounds.hpp" // for BGEggshipRoom, BGShootingStar
#include "entity.hpp"               // for Entity
#include "illumination.hpp"         // IWYU pragma: keep

namespace NEntitiesBG
{
void register_usertypes(sol::state& lua)
{
    lua["Entity"]["as_bgsurfacestar"] = &Entity::as<BGSurfaceStar>;
    lua["Entity"]["as_bgshootingstar"] = &Entity::as<BGShootingStar>;
    lua["Entity"]["as_bgrelativeelement"] = &Entity::as<BGRelativeElement>;
    lua["Entity"]["as_bgmovingstar"] = &Entity::as<BGMovingStar>;
    lua["Entity"]["as_bgsurfacelayer"] = &Entity::as<BGSurfaceLayer>;
    lua["Entity"]["as_bgeggshiproom"] = &Entity::as<BGEggshipRoom>;
    lua["Entity"]["as_bgbacklayerdoor"] = &Entity::as<BGBackLayerDoor>;
    lua["Entity"]["as_bgtutorialsign"] = &Entity::as<BGTutorialSign>;
    lua["Entity"]["as_bgshopentrance"] = &Entity::as<BGShopEntrance>;
    lua["Entity"]["as_bgfloatingdebris"] = &Entity::as<BGFloatingDebris>;
    lua["Entity"]["as_bgshopkeeperprime"] = &Entity::as<BGShopKeeperPrime>;

    lua.new_usertype<BGBackLayerDoor>(
        "BGBackLayerDoor",
        "illumination1",
        &BGBackLayerDoor::illumination1,
        "illumination2",
        &BGBackLayerDoor::illumination2,
        sol::base_classes,
        sol::bases<Entity>());

    lua.new_usertype<BGSurfaceStar>(
        "BGSurfaceStar",
        "blink_timer",
        &BGSurfaceStar::blink_timer,
        "relative_x",
        &BGSurfaceStar::relative_x,
        "relative_y",
        &BGSurfaceStar::relative_y,
        sol::base_classes,
        sol::bases<Entity>());

    lua.new_usertype<BGRelativeElement>(
        "BGRelativeElement",
        "relative_x",
        &BGRelativeElement::relative_x,
        "relative_y",
        &BGRelativeElement::relative_y,
        sol::base_classes,
        sol::bases<Entity>());

    lua.new_usertype<BGSurfaceLayer>(
        "BGSurfaceLayer",
        "relative_offset_x",
        &BGSurfaceLayer::relative_offset_x,
        "relative_offset_y",
        &BGSurfaceLayer::relative_offset_y,
        sol::base_classes,
        sol::bases<Entity, BGRelativeElement>());

    lua.new_usertype<BGEggshipRoom>(
        "BGEggshipRoom",
        "sound",
        &BGEggshipRoom::sound,
        "fx_shell",
        &BGEggshipRoom::fx_shell,
        "fx_door",
        &BGEggshipRoom::fx_door,
        "platform_left",
        &BGEggshipRoom::platform_left,
        "platform_middle",
        &BGEggshipRoom::platform_middle,
        "platform_right",
        &BGEggshipRoom::platform_right,
        "player_in",
        &BGEggshipRoom::player_in,
        sol::base_classes,
        sol::bases<Entity>());

    lua.new_usertype<BGMovingStar>(
        "BGMovingStar",
        "falling_speed",
        &BGMovingStar::falling_speed,
        sol::base_classes,
        sol::bases<Entity, BGSurfaceStar>());

    lua.new_usertype<BGTutorialSign>(
        "BGTutorialSign",
        "is_shown",
        &BGTutorialSign::is_shown,
        sol::base_classes,
        sol::bases<Entity>());

    lua.new_usertype<BGShootingStar>(
        "BGShootingStar",
        "x_increment",
        &BGShootingStar::x_increment,
        "y_increment",
        &BGShootingStar::y_increment,
        "timer",
        &BGShootingStar::timer,
        "max_timer",
        &BGShootingStar::max_timer,
        "size",
        &BGShootingStar::size,
        sol::base_classes,
        sol::bases<Entity, BGRelativeElement>());

    lua.new_usertype<BGShopEntrance>(
        "BGShopEntrance",
        "on_entering",
        &BGShopEntrance::on_entering,
        sol::base_classes,
        sol::bases<Entity>());

    lua.new_usertype<BGFloatingDebris>(
        "BGFloatingDebris",
        "distance",
        &BGFloatingDebris::distance,
        "speed",
        &BGFloatingDebris::speed,
        "sine_angle",
        &BGFloatingDebris::sine_angle,
        sol::base_classes,
        sol::bases<Entity, BGSurfaceLayer>());

    lua.new_usertype<BGShopKeeperPrime>(
        "BGShopKeeperPrime",
        "normal_y",
        &BGShopKeeperPrime::normal_y,
        "sine_pos",
        &BGShopKeeperPrime::sine_pos,
        "bubbles_timer",
        &BGShopKeeperPrime::bubbles_timer,
        "bubble_spawn_trigger",
        &BGShopKeeperPrime::bubble_spawn_trigger,
        "bubble_spawn_delay",
        &BGShopKeeperPrime::bubble_spawn_delay,
        sol::base_classes,
        sol::bases<Entity>());
}
}; // namespace NEntitiesBG
