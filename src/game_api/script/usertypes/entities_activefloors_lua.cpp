#include "entities_activefloors_lua.hpp"

#include "entities_activefloors.hpp"
#include "script/script_impl.hpp"

#include <sol/sol.hpp>

namespace NEntitiesActiveFloors
{
void register_usertypes(sol::state& lua, [[maybe_unused]] ScriptImpl* script)
{
    lua["Entity"]["as_crushtrap"] = &Entity::as<Crushtrap>;
    lua["Entity"]["as_olmec"] = &Entity::as<Olmec>;
    lua["Entity"]["as_woodenlogtrap"] = &Entity::as<WoodenlogTrap>;
    lua["Entity"]["as_boulder"] = &Entity::as<Boulder>;
    lua["Entity"]["as_pushblock"] = &Entity::as<PushBlock>;
    lua["Entity"]["as_boneblock"] = &Entity::as<BoneBlock>;
    lua["Entity"]["as_chainedpushblock"] = &Entity::as<ChainedPushBlock>;
    lua["Entity"]["as_lightarrowplatform"] = &Entity::as<LightArrowPlatform>;
    lua["Entity"]["as_fallingplatform"] = &Entity::as<FallingPlatform>;
    lua["Entity"]["as_unchainedspikeball"] = &Entity::as<UnchainedSpikeBall>;
    lua["Entity"]["as_drill"] = &Entity::as<Drill>;
    lua["Entity"]["as_thinice"] = &Entity::as<ThinIce>;
    lua["Entity"]["as_elevator"] = &Entity::as<Elevator>;
    lua["Entity"]["as_clambase"] = &Entity::as<ClamBase>;
    lua["Entity"]["as_regenblock"] = &Entity::as<RegenBlock>;
    lua["Entity"]["as_timedpowderkeg"] = &Entity::as<TimedPowderkeg>;

    lua.new_usertype<Crushtrap>(
        "Crushtrap",
        "dirx",
        &Crushtrap::dirx,
        "diry",
        &Crushtrap::diry,
        "timer",
        &Crushtrap::timer,
        "bounce_back_timer",
        &Crushtrap::bounce_back_timer,
        sol::base_classes,
        sol::bases<Entity, Movable>());

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

    lua.new_usertype<WoodenlogTrap>(
        "WoodenlogTrap",
        "ceiling_1_uid",
        &WoodenlogTrap::ceiling_1_uid,
        "ceiling_2_uid",
        &WoodenlogTrap::ceiling_2_uid,
        "falling_speed",
        &WoodenlogTrap::falling_speed,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<Boulder>(
        "Boulder",
        "is_rolling",
        &Boulder::is_rolling,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<PushBlock>(
        "PushBlock",
        "dust_particle",
        &PushBlock::dust_particle,
        "dest_pos_x",
        &PushBlock::dest_pos_x,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<BoneBlock>(
        "BoneBlock",
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<ChainedPushBlock>(
        "ChainedPushBlock",
        "is_chained",
        &ChainedPushBlock::is_chained,
        sol::base_classes,
        sol::bases<Entity, Movable, PushBlock>());

    lua.new_usertype<LightArrowPlatform>(
        "LightArrowPlatform",
        "emitted_light",
        &LightArrowPlatform::emitted_light,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<FallingPlatform>(
        "FallingPlatform",
        "emitted_light",
        &FallingPlatform::timer,
        "shaking_factor",
        &FallingPlatform::shaking_factor,
        "y_pos",
        &FallingPlatform::y_pos,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<UnchainedSpikeBall>(
        "UnchainedSpikeBall",
        "bounce",
        &UnchainedSpikeBall::bounce,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<Drill>(
        "Drill",
        "top_chain_piece",
        &Drill::top_chain_piece,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<ThinIce>(
        "ThinIce",
        "strength",
        &ThinIce::strength,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<Elevator>(
        "Elevator",
        "emitted_light",
        &Elevator::emitted_light,
        "timer",
        &Elevator::timer,
        "moving_up",
        &Elevator::moving_up,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<ClamBase>(
        "ClamBase",
        "treasure_type",
        &ClamBase::treasure_type,
        "treasure_uid",
        &ClamBase::treasure_uid,
        "treasure_x_pos",
        &ClamBase::treasure_x_pos,
        "treasure_y_pos",
        &ClamBase::treasure_y_pos,
        "top_part_uid",
        &ClamBase::top_part_uid,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<RegenBlock>(
        "RegenBlock",
        "on_breaking",
        &RegenBlock::on_breaking,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<TimedPowderkeg>(
        "TimedPowderkeg",
        "timer",
        &TimedPowderkeg::timer,
        sol::base_classes,
        sol::bases<Entity, Movable, PushBlock>());
}
} // namespace NEntitiesActiveFloors
