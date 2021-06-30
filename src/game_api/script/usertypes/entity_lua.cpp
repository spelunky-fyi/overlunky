#include "entity_lua.hpp"

#include "entities_items.hpp"
#include "entities_monsters.hpp"
#include "entities_mounts.hpp"
#include "entity.hpp"
#include "script/script_impl.hpp"

#include <sol/sol.hpp>

namespace NEntity
{
void register_usertypes(sol::state& lua, ScriptImpl* script)
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
        "width",
        &Entity::w,
        "height",
        &Entity::h,
        "angle",
        &Movable::angle,
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
        "as_door",
        &Entity::as<Door>,
        "as_container",
        &Entity::as<Container>,
        "as_mattock",
        &Entity::as<Mattock>,
        "as_mount",
        &Entity::as<Mount>,
        "as_player",
        &Entity::as<Player>,
        "as_monster",
        &Entity::as<Monster>,
        "as_gun",
        &Entity::as<Gun>,
        "as_bomb",
        &Entity::as<Bomb>,
        "as_crushtrap",
        &Entity::as<Crushtrap>,
        "as_arrowtrap",
        &Entity::as<Arrowtrap>,
        "as_olmec",
        &Entity::as<Olmec>,
        "as_olmec_floater",
        &Entity::as<OlmecFloater>,
        "as_cape",
        &Entity::as<Cape>,
        "as_vlads_cape",
        &Entity::as<VladsCape>,
        "as_ghost",
        &Entity::as<Ghost>,
        "as_jiangshi",
        &Entity::as<Jiangshi>,
        "as_kapala_powerup",
        &Entity::as<KapalaPowerup>,
        "as_caveman",
        &Entity::as<Caveman>,
        "as_backpack",
        &Entity::as<Backpack>,
        "as_jetpack",
        &Entity::as<Jetpack>,
        "as_hoverpack",
        &Entity::as<Hoverpack>,
        "as_walltorchflame",
        &Entity::as<WalltorchFlame>);
    /* Entity
            bool overlaps_with(Entity other)
            bool set_texture(uint32_t texture_id)
        */
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
        "color",
        &Movable::color,
        "hitboxx",
        &Movable::hitboxx,
        "hitboxy",
        &Movable::hitboxy,
        "offsetx",
        &Movable::offsetx,
        "offsety",
        &Movable::offsety,
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
    /* Movable
            bool is_poisoned()
            void poison(int16_t frames)
            bool is_button_pressed(uint32_t button)
            bool is_button_held(uint32_t button)
            bool is_button_released(uint32_t button)
        */
    lua.new_usertype<Monster>(
        "Monster",
        "chased_target_uid",
        &Monster::chased_target_uid,
        "target_selection_timer",
        &Monster::target_selection_timer,
        sol::base_classes,
        sol::bases<Entity, Movable>());
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
    lua.new_usertype<Mount>(
        "Mount",
        "carry",
        &Mount::carry,
        "tame",
        &Mount::tame,
        "rider_uid",
        &Mount::rider_uid,
        "can_doublejump",
        &Mount::can_doublejump,
        "tamed",
        &Mount::tamed,
        "walk_pause_timer",
        &Mount::walk_pause_timer,
        "taming_timer",
        &Mount::taming_timer,
        sol::base_classes,
        sol::bases<Entity, Movable>());
    lua.new_usertype<Bomb>("Bomb", "scale_hor", &Bomb::scale_hor, "scale_ver", &Bomb::scale_ver, sol::base_classes, sol::bases<Entity, Movable>());
    lua.new_usertype<Container>(
        "Container", "inside", &Container::inside, "timer", &Container::timer, sol::base_classes, sol::bases<Entity, Movable>());
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
    lua.new_usertype<Crushtrap>("Crushtrap", "dirx", &Crushtrap::dirx, "diry", &Crushtrap::diry, sol::base_classes, sol::bases<Entity, Movable>());
    lua.new_usertype<Arrowtrap>(
        "Arrowtrap", "arrow_shot", &Arrowtrap::arrow_shot, "rearm", &Arrowtrap::rearm, sol::base_classes, sol::bases<Entity>());
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
    lua.new_usertype<OlmecFloater>(
        "OlmecFloater", "both_floaters_intact", &OlmecFloater::both_floaters_intact, sol::base_classes, sol::bases<Entity, Movable>());
    lua.new_usertype<Cape>("Cape", "floating_down", &VladsCape::floating_down, sol::base_classes, sol::bases<Entity, Movable>());
    lua.new_usertype<VladsCape>("VladsCape", "can_double_jump", &VladsCape::can_double_jump, sol::base_classes, sol::bases<Entity, Movable, Cape>());
    lua.new_usertype<Ghost>(
        "Ghost",
        "split_timer",
        &Ghost::split_timer,
        "velocity_multiplier",
        &Ghost::velocity_multiplier,
        sol::base_classes,
        sol::bases<Entity, Movable, Monster>());
    lua.new_usertype<Jiangshi>(
        "Jiangshi", "wait_timer", &Jiangshi::wait_timer, sol::base_classes, sol::bases<Entity, Movable, Monster>());
    lua.new_usertype<KapalaPowerup>(
        "KapalaPowerup", "amount_of_blood", &KapalaPowerup::amount_of_blood, sol::base_classes, sol::bases<Entity, Movable>());
    lua.new_usertype<Caveman>(
        "Caveman",
        "chatting_to_uid",
        &Caveman::chatting_to_uid,
        "walk_pause_timer",
        &Caveman::walk_pause_timer,
        "chatting_timer",
        &Caveman::chatting_timer,
        "wake_up_timer",
        &Caveman::wake_up_timer,
        "can_pick_up_timer",
        &Caveman::can_pick_up_timer,
        "aggro_timer",
        &Caveman::aggro_timer,
        sol::base_classes,
        sol::bases<Entity, Movable, Monster>());
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
    lua.new_usertype<WalltorchFlame>(
        "WalltorchFlame",
        "emitted_light",
        &WalltorchFlame::emitted_light,
        "flare_up_size",
        &WalltorchFlame::flare_up_size,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.create_named_table("ENT_TYPE"
                           //, "FLOOR_BORDERTILE", 1
                           //, "", ...blah__blah__read__your__entities.txt...
                           //, "LIQUID_STAGNANT_LAVA", 898
    );
    for (auto& item : script->g_items)
    {
        auto name = item.name.substr(9, item.name.size());
        lua["ENT_TYPE"][name] = item.id;
    }
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
