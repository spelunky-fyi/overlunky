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
    lua["Entity"]["as_vladscape"] = &Entity::as<VladsCape>;
    lua["Entity"]["as_kapalapowerup"] = &Entity::as<KapalaPowerup>;
    lua["Entity"]["as_mattock"] = &Entity::as<Mattock>;
    lua["Entity"]["as_gun"] = &Entity::as<Gun>;
    lua["Entity"]["as_webgun"] = &Entity::as<WebGun>;
    lua["Entity"]["as_flame"] = &Entity::as<Flame>;
    lua["Entity"]["as_flamesize"] = &Entity::as<FlameSize>;
    lua["Entity"]["as_climbablerope"] = &Entity::as<ClimbableRope>;
    lua["Entity"]["as_idol"] = &Entity::as<Idol>;
    lua["Entity"]["as_spear"] = &Entity::as<Spear>;
    lua["Entity"]["as_junglespearcosmetic"] = &Entity::as<JungleSpearCosmetic>;
    lua["Entity"]["as_webshot"] = &Entity::as<WebShot>;
    lua["Entity"]["as_hangstrand"] = &Entity::as<HangStrand>;
    lua["Entity"]["as_hanganchor"] = &Entity::as<HangAnchor>;
    lua["Entity"]["as_arrow"] = &Entity::as<Arrow>;
    lua["Entity"]["as_lightarrow"] = &Entity::as<LightArrow>;
    lua["Entity"]["as_lightshot"] = &Entity::as<LightShot>;
    lua["Entity"]["as_lightemitter"] = &Entity::as<LightEmitter>;
    lua["Entity"]["as_sceptershot"] = &Entity::as<ScepterShot>;
    lua["Entity"]["as_specialshot"] = &Entity::as<SpecialShot>;
    lua["Entity"]["as_soundshot"] = &Entity::as<SoundShot>;
    lua["Entity"]["as_spark"] = &Entity::as<Spark>;
    lua["Entity"]["as_tiamatshot"] = &Entity::as<TiamatShot>;
    lua["Entity"]["as_fireball"] = &Entity::as<Fireball>;
    lua["Entity"]["as_leaf"] = &Entity::as<Leaf>;
    lua["Entity"]["as_acidbubble"] = &Entity::as<AcidBubble>;
    lua["Entity"]["as_claw"] = &Entity::as<Claw>;
    lua["Entity"]["as_stretchchain"] = &Entity::as<StretchChain>;

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
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<WebGun>(
        "WebGun",
        "in_chamber",
        &WebGun::in_chamber,
        sol::base_classes,
        sol::bases<Entity, Movable, Gun>());

    lua.new_usertype<Flame>(
        "Flame",
        "emitted_light",
        &Flame::emitted_light,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<FlameSize>(
        "FlameSize",
        "flame_size",
        &FlameSize::flame_size,
        sol::base_classes,
        sol::bases<Entity, Movable, Flame>());

    lua.new_usertype<ClimbableRope>(
        "ClimbableRope",
        "segment_nr_inverse",
        &ClimbableRope::segment_nr_inverse,
        "burn_timer",
        &ClimbableRope::burn_timer,
        "above_part",
        &ClimbableRope::above_part,
        "below_part",
        &ClimbableRope::below_part,
        "segment_nr",
        &ClimbableRope::segment_nr,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<Idol>(
        "Idol",
        "trap_triggered",
        &Idol::trap_triggered,
        "touch",
        &Idol::touch,
        "spawn_x",
        &Idol::spawn_x,
        "spawn_y",
        &Idol::spawn_y,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<Spear>(
        "Spear",
        "sound_id",
        &Spear::sound_id,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<JungleSpearCosmetic>(
        "JungleSpearCosmetic",
        "move_x",
        &JungleSpearCosmetic::move_x,
        "move_y",
        &JungleSpearCosmetic::move_y,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<WebShot>(
        "WebShot",
        "shot",
        &WebShot::shot,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<HangStrand>(
        "HangStrand",
        "start_pos_y",
        &HangStrand::start_pos_y,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<HangAnchor>(
        "HangAnchor",
        "spider_uid",
        &HangAnchor::spider_uid,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<Arrow>(
        "Arrow",
        "flame_uid",
        &Arrow::flame_uid,
        "is_on_fire",
        &Arrow::is_on_fire,
        "is_poisoned",
        &Arrow::is_poisoned,
        "shot_from_trap",
        &Arrow::shot_from_trap,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<LightArrow>(
        "LightArrow",
        "emitted_light",
        &LightArrow::emitted_light,
        sol::base_classes,
        sol::bases<Entity, Movable, Arrow>());

    lua.new_usertype<LightShot>(
        "LightShot",
        "emitted_light",
        &LightShot::emitted_light,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<LightEmitter>(
        "LightEmitter",
        "emitted_light",
        &LightEmitter::emitted_light,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<ScepterShot>(
        "ScepterShot",
        "speed",
        &ScepterShot::speed,
        "idle_timer",
        &ScepterShot::idle_timer,
        sol::base_classes,
        sol::bases<Entity, Movable, LightEmitter>());

    lua.new_usertype<SpecialShot>(
        "SpecialShot",
        "target_x",
        &SpecialShot::target_x,
        "target_y",
        &SpecialShot::target_y,
        sol::base_classes,
        sol::bases<Entity, Movable, LightEmitter>());

    lua.new_usertype<SoundShot>(
        "SoundShot",
        sol::base_classes,
        sol::bases<Entity, Movable, LightShot>());

    lua.new_usertype<Spark>(
        "Spark",
        "particle",
        &Spark::particle,
        "rotation_center_x",
        &Spark::rotation_center_x,
        "rotation_center_y",
        &Spark::rotation_center_y,
        "angle",
        &Spark::angle,
        "size_multiply",
        &Spark::size_multiply,
        "next_size",
        &Spark::next_size,
        sol::base_classes,
        sol::bases<Entity, Movable, Flame>());

    lua.new_usertype<TiamatShot>(
        "TiamatShot",
        sol::base_classes,
        sol::bases<Entity, Movable, LightEmitter>());

    lua.new_usertype<Fireball>(
        "Fireball",
        "particle",
        &Fireball::particle,
        sol::base_classes,
        sol::bases<Entity, Movable, LightShot, SoundShot>());

    lua.new_usertype<Leaf>(
        "Leaf",
        "fade_away_counter",
        &Leaf::fade_away_counter,
        "swing_direction",
        &Leaf::swing_direction,
        "fade_away_trigger",
        &Leaf::fade_away_trigger,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<AcidBubble>(
        "AcidBubble",
        "speed_x",
        &AcidBubble::speed_x,
        "speed_y",
        &AcidBubble::speed_y,
        "float_counter",
        &AcidBubble::float_counter,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<Claw>(
        "Claw",
        "crabman_uid",
        &Claw::crabman_uid,
        "spawn_x",
        &Claw::spawn_x,
        "spawn_y",
        &Claw::spawn_y,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<StretchChain>(
        "StretchChain",
        "at_end_of_chain_uid",
        &StretchChain::at_end_of_chain_uid,
        "position_in_chain",
        &StretchChain::position_in_chain,
        "is_chain_overlapping_ball",
        &StretchChain::is_chain_overlapping_ball,
        sol::base_classes,
        sol::bases<Entity, Movable>());
}
} // namespace NEntitiesItems
