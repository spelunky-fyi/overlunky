#include "entities_mounts_lua.hpp"

#include <algorithm>   // for max
#include <new>         // for operator new
#include <sol/sol.hpp> // for proxy_key_t, state, basic_table_core:...
#include <string>      // for operator==, allocator
#include <tuple>       // for get
#include <type_traits> // for move, declval
#include <utility>     // for min, max

#include "entities_mounts.hpp" // for Mount, Mech, Axolotl, Qilin, Rockdog
#include "entity.hpp"          // for Entity
#include "sound_manager.hpp"   //

class Movable;
class PowerupCapable;

namespace NEntitiesMounts
{
void register_usertypes(sol::state& lua)
{
    lua["Entity"]["as_mount"] = &Entity::as<Mount>;

    lua["Entity"]["as_rockdog"] = &Entity::as<Rockdog>;
    lua["Entity"]["as_axolotl"] = &Entity::as<Axolotl>;
    lua["Entity"]["as_mech"] = &Entity::as<Mech>;
    lua["Entity"]["as_qilin"] = &Entity::as<Qilin>;

    lua.new_usertype<Mount>(
        "Mount",
        "rider_uid",
        &Mount::rider_uid,
        "sound",
        &Mount::sound,
        "can_doublejump",
        &Mount::can_doublejump,
        "tamed",
        &Mount::tamed,
        "walk_pause_timer",
        &Mount::walk_pause_timer,
        "taming_timer",
        &Mount::taming_timer,
        "used_double_jump",
        &Mount::used_double_jump,
        "remove_rider",
        &Mount::remove_rider,
        "carry",
        &Mount::carry,
        "tame",
        &Mount::tame,
        "get_rider_offset",
        &Mount::get_rider_offset,
        "get_rider_offset_crouching",
        &Mount::get_rider_offset_crouching,
        "get_jump_sound",
        &Mount::get_jump_sound,
        "get_attack_sound",
        &Mount::get_attack_sound,
        "get_mounting_sound",
        &Mount::get_mounting_sound,
        "get_walking_sound",
        &Mount::get_walking_sound,
        "get_untamed_loop_sound",
        &Mount::get_untamed_loop_sound,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable>());

    lua.new_usertype<Rockdog>(
        "Rockdog",
        "attack_cooldown",
        &Rockdog::attack_cooldown,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Mount>());

    lua.new_usertype<Axolotl>(
        "Axolotl",
        "attack_cooldown",
        &Axolotl::attack_cooldown,
        "can_teleport",
        &Axolotl::can_teleport,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Mount>());

    lua.new_usertype<Mech>(
        "Mech",
        "crouch_walk_sound",
        &Mech::crouch_walk_sound,
        "explosion_sound",
        &Mech::explosion_sound,
        "gun_cooldown",
        &Mech::gun_cooldown,
        "walking",
        &Mech::walking,
        "breaking_wall",
        &Mech::breaking_wall,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Mount>());

    lua.new_usertype<Qilin>(
        "Qilin",
        "fly_gallop_sound",
        &Qilin::fly_gallop_sound,
        "attack_cooldown",
        &Qilin::attack_cooldown,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Mount>());

    /// Make `mount_uid` carry `rider_uid` on their back. Only use this with actual mounts and living things.
    lua["carry"] = [](uint32_t mount_uid, uint32_t rider_uid)
    {
        auto mount = get_entity_ptr(mount_uid)->as<Mount>();
        auto rider = get_entity_ptr(rider_uid)->as<Movable>();
        if (mount == nullptr || rider == nullptr)
            return;
        mount->carry(rider);
    };
}
} // namespace NEntitiesMounts
