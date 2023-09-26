#include "entities_activefloors_lua.hpp"

#include <algorithm> // for max
#include <new>       // for operator new
#include <sol/sol.hpp>
#include <string>      // for operator==, allocator
#include <tuple>       // for get
#include <type_traits> // for move, declval
#include <utility>     // for min, max

#include "entities_activefloors.hpp" // for Olmec, ClamBase, Crushtrap, Ele...
#include "entity.hpp"                // for Entity
#include "particles.hpp"             // IWYU pragma: keep
#include "sound_manager.hpp"         //
#include "state_structs.hpp"         // IWYU pragma: keep

class Movable;

namespace NEntitiesActiveFloors
{
void register_usertypes(sol::state& lua)
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
    lua["Entity"]["as_crushelevator"] = &Entity::as<CrushElevator>;

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
        "sound",
        &Olmec::sound,
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
        "sound",
        &PushBlock::sound,
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
        "timer",
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
        "sound1",
        &Drill::sound1,
        "sound2",
        &Drill::sound2,
        "top_chain_piece",
        &Drill::top_chain_piece,
        "trigger",
        &Drill::trigger,
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

    lua.new_usertype<CrushElevator>(
        "CrushElevator",
        "y_limit",
        &CrushElevator::y_limit,
        "speed",
        &CrushElevator::speed,
        sol::base_classes,
        sol::bases<Entity, Movable>());
}
} // namespace NEntitiesActiveFloors
