#include "entity_lua.hpp"

#include "entities_items.hpp"
#include "entities_monsters.hpp"
#include "entities_mounts.hpp"
#include "entity.hpp"
#include "script/script_impl.hpp"

#include <sol/sol.hpp>

namespace NEntity
{
void register_usertypes(sol::state& lua, LuaBackend* script)
{
    lua.new_usertype<Color>("Color", "r", &Color::r, "g", &Color::g, "b", &Color::b, "a", &Color::a);
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
    lua.new_usertype<Animation>(
        "Animation",
        "first_tile",
        &Animation::texture,
        "num_tiles",
        &Animation::count,
        "interval",
        &Animation::interval,
        "repeat_mode",
        &Animation::repeat);
    lua.new_usertype<EntityDB>(
        "EntityDB",
        "id",
        &EntityDB::id,
        "search_flags",
        &EntityDB::search_flags,
        "width",
        &EntityDB::width,
        "height",
        &EntityDB::height,
        "draw_depth",
        &EntityDB::draw_depth,
        "friction",
        &EntityDB::friction,
        "elasticity",
        &EntityDB::elasticity,
        "weight",
        &EntityDB::weight,
        "acceleration",
        &EntityDB::acceleration,
        "max_speed",
        &EntityDB::max_speed,
        "sprint_factor",
        &EntityDB::sprint_factor,
        "jump",
        &EntityDB::jump,
        "glow_red",
        &EntityDB::glow_red,
        "glow_green",
        &EntityDB::glow_green,
        "glow_blue",
        &EntityDB::glow_blue,
        "glow_alpha",
        &EntityDB::glow_alpha,
        "damage",
        &EntityDB::damage,
        "life",
        &EntityDB::life,
        "blood_content",
        &EntityDB::blood_content,
        "texture",
        &EntityDB::texture,
        "animations",
        &EntityDB::animations,
        "properties_flags",
        &EntityDB::properties_flags,
        "default_flags",
        &EntityDB::default_flags,
        "default_more_flags",
        &EntityDB::default_more_flags,
        "leaves_corpse_behind",
        &EntityDB::leaves_corpse_behind,
        "sound_killed_by_player",
        &EntityDB::sound_killed_by_player,
        "sound_killed_by_other",
        &EntityDB::sound_killed_by_other);

    auto overlaps_with = sol::overload(
        static_cast<bool (Entity::*)(Entity*)>(&Entity::overlaps_with),
        static_cast<bool (Entity::*)(AABB)>(&Entity::overlaps_with),
        static_cast<bool (Entity::*)(float, float, float, float)>(&Entity::overlaps_with));
    lua.new_usertype<Entity>(
        "Entity",
        "type",
        &Entity::type,
        "overlay",
        &Entity::overlay,
        "flags",
        &Entity::flags,
        "more_flags",
        &Entity::more_flags,
        "uid",
        &Entity::uid,
        "animation_frame",
        &Entity::animation_frame,
        "x",
        &Entity::x,
        "y",
        &Entity::y,
        "layer",
        &Entity::layer,
        "width",
        &Entity::w,
        "height",
        &Entity::h,
        "angle",
        &Entity::angle,
        "color",
        &Entity::color,
        "hitboxx",
        &Entity::hitboxx,
        "hitboxy",
        &Entity::hitboxy,
        "offsetx",
        &Entity::offsetx,
        "offsety",
        &Entity::offsety,
        "topmost",
        &Entity::topmost,
        "topmost_mount",
        &Entity::topmost_mount,
        "overlaps_with",
        overlaps_with,
        "get_texture",
        &Entity::get_texture,
        "set_texture",
        &Entity::set_texture,
        "as_movable",
        &Entity::as<Movable>,
        "as_player",
        &Entity::as<Player>);
    lua.new_usertype<Movable>(
        "Movable",
        "movex",
        &Movable::movex,
        "movey",
        &Movable::movey,
        "buttons",
        &Movable::buttons,
        "stand_counter",
        &Movable::stand_counter,
        "jump_height_multiplier",
        &Movable::jump_height_multiplier,
        "owner_uid",
        &Movable::owner_uid,
        "last_owner_uid",
        &Movable::last_owner_uid,
        "idle_counter",
        &Movable::idle_counter,
        "standing_on_uid",
        &Movable::standing_on_uid,
        "velocityx",
        &Movable::velocityx,
        "velocityy",
        &Movable::velocityy,
        "holding_uid",
        &Movable::holding_uid,
        "state",
        &Movable::state,
        "last_state",
        &Movable::last_state,
        "move_state",
        &Movable::move_state,
        "health",
        &Movable::health,
        "stun_timer",
        &Movable::stun_timer,
        "stun_state",
        &Movable::stun_state,
        "some_state",
        &Movable::some_state,
        "airtime",
        &Movable::airtime,
        "is_poisoned",
        &Movable::is_poisoned,
        "poison",
        &Movable::poison,
        "dark_shadow_timer",
        &Movable::dark_shadow_timer,
        "exit_invincibility_timer",
        &Movable::exit_invincibility_timer,
        "invincibility_frames_timer",
        &Movable::invincibility_frames_timer,
        "frozen_timer",
        &Movable::frozen_timer,
        "is_button_pressed",
        &Movable::is_button_pressed,
        "is_button_held",
        &Movable::is_button_held,
        "is_button_released",
        &Movable::is_button_released,
        "price",
        &Movable::price,
        sol::base_classes,
        sol::bases<Entity>());
    lua.new_usertype<Player>(
        "Player",
        "inventory",
        &Player::inventory_ptr,
        "emitted_light",
        &Player::emitted_light,
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

    lua.create_named_table("ENT_TYPE"
                           //, "FLOOR_BORDERTILE", 1
                           //, "", ...check__[entities.txt]\[https://github.com/spelunky-fyi/overlunky/tree/main/docs/game_data/entities.txt\]...
                           //, "LIQUID_STAGNANT_LAVA", 898
    );
    for (auto& item : script->g_items)
    {
        auto name = item.name.substr(9, item.name.size());
        lua["ENT_TYPE"][name] = item.id;
    }
    lua.create_named_table("REPEAT_TYPE", "NO_REPEAT", REPEAT_TYPE::NoRepeat, "LINEAR", REPEAT_TYPE::Linear, "BACK_AND_FORTH", REPEAT_TYPE::BackAndForth);
    lua.create_named_table("BUTTON", "JUMP", 1, "WHIP", 2, "BOMB", 4, "ROPE", 8, "RUN", 16, "DOOR", 32);
    lua.create_named_table(
        "MASK",
        "PLAYER",
        0x1,
        "MOUNT",
        0x2,
        "MONSTER",
        0x4,
        "ITEM",
        0x8,
        "EXPLOSION",
        0x10,
        "ROPE",
        0x20,
        "FX",
        0x40,
        "ACTIVEFLOOR",
        0x80,
        "FLOOR",
        0x100,
        "DECORATION",
        0x200,
        "BG",
        0x400,
        "SHADOW",
        0x800,
        "LOGICAL",
        0x1000,
        "WATER",
        0x2000,
        "LAVA",
        0x4000);
}
}; // namespace NEntity
