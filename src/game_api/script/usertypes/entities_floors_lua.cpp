#include "entities_floors_lua.hpp"

#include "entities_floors.hpp"
#include "entity.hpp"

#include <sol/sol.hpp>

namespace NEntitiesFloors
{
void register_usertypes(sol::state& lua)
{
    lua["Entity"]["as_floor"] = &Entity::as<Floor>;
    lua["Entity"]["as_door"] = &Entity::as<Door>;
    lua["Entity"]["as_exitdoor"] = &Entity::as<ExitDoor>;
    lua["Entity"]["as_decorateddoor"] = &Entity::as<DecoratedDoor>;
    lua["Entity"]["as_lockeddoor"] = &Entity::as<LockedDoor>;
    lua["Entity"]["as_cityofgolddoor"] = &Entity::as<CityOfGoldDoor>;
    lua["Entity"]["as_mainexit"] = &Entity::as<MainExit>;
    lua["Entity"]["as_eggshipdoor"] = &Entity::as<EggShipDoor>;
    lua["Entity"]["as_arrowtrap"] = &Entity::as<Arrowtrap>;
    lua["Entity"]["as_totemtrap"] = &Entity::as<TotemTrap>;
    lua["Entity"]["as_lasertrap"] = &Entity::as<LaserTrap>;
    lua["Entity"]["as_sparktrap"] = &Entity::as<SparkTrap>;
    lua["Entity"]["as_altar"] = &Entity::as<Altar>;
    lua["Entity"]["as_spikeballtrap"] = &Entity::as<SpikeballTrap>;
    lua["Entity"]["as_transferfloor"] = &Entity::as<TransferFloor>;
    lua["Entity"]["as_conveyorbelt"] = &Entity::as<ConveyorBelt>;
    lua["Entity"]["as_pipe"] = &Entity::as<Pipe>;
    lua["Entity"]["as_generator"] = &Entity::as<Generator>;
    lua["Entity"]["as_slidingwallceiling"] = &Entity::as<SlidingWallCeiling>;
    lua["Entity"]["as_quicksand"] = &Entity::as<QuickSand>;
    lua["Entity"]["as_bigspeartrap"] = &Entity::as<BigSpearTrap>;
    lua["Entity"]["as_stickytrap"] = &Entity::as<StickyTrap>;
    lua["Entity"]["as_motherstatue"] = &Entity::as<MotherStatue>;
    lua["Entity"]["as_teleportingborder"] = &Entity::as<TeleportingBorder>;
    lua["Entity"]["as_forcefield"] = &Entity::as<ForceField>;
    lua["Entity"]["as_timedforcefield"] = &Entity::as<TimedForceField>;
    lua["Entity"]["as_horizontalforcefield"] = &Entity::as<HorizontalForceField>;
    lua["Entity"]["as_tentaclebottom"] = &Entity::as<TentacleBottom>;
    lua["Entity"]["as_poledeco"] = &Entity::as<PoleDeco>;

    lua.new_usertype<Floor>(
        "Floor",
        "deco_top",
        &Floor::deco_top,
        "deco_bottom",
        &Floor::deco_bottom,
        "deco_left",
        &Floor::deco_left,
        "deco_right",
        &Floor::deco_right,
        "fix_border_tile_animation",
        &Floor::fix_border_tile_animation,
        "fix_decorations",
        &Floor::fix_decorations,
        "add_decoration",
        &Floor::add_decoration,
        "remove_decoration",
        &Floor::remove_decoration,
        sol::base_classes,
        sol::bases<Entity>());

    // The corner options only work for FLOOR_BORDERTILE and FLOOR_BORDERTILE_OCTOPUS
    lua.create_named_table(
        "FLOOR_SIDE",
        "TOP",
        FLOOR_SIDE::TOP,
        "BOTTOM",
        FLOOR_SIDE::BOTTOM,
        "LEFT",
        FLOOR_SIDE::LEFT,
        "RIGHT",
        FLOOR_SIDE::RIGHT,
        "TOP_LEFT",
        FLOOR_SIDE::TOP_LEFT,
        "TOP_RIGHT",
        FLOOR_SIDE::TOP_RIGHT,
        "BOTTOM_LEFT",
        FLOOR_SIDE::BOTTOM_LEFT,
        "BOTTOM_RIGHT",
        FLOOR_SIDE::BOTTOM_RIGHT);

    lua.new_usertype<Door>(
        "Door",
        "counter",
        &Door::counter,
        "fx_button",
        &Door::fx_button,
        sol::base_classes,
        sol::bases<Entity, Floor>());

    lua.new_usertype<ExitDoor>(
        "ExitDoor",
        "entered",
        &ExitDoor::entered,
        "special_door",
        &ExitDoor::special_door,
        "level",
        &ExitDoor::level,
        "timer",
        &ExitDoor::timer,
        "world",
        &ExitDoor::world,
        "theme",
        &ExitDoor::theme,
        sol::base_classes,
        sol::bases<Entity, Floor, Door>());

    lua.new_usertype<DecoratedDoor>(
        "DecoratedDoor",
        "special_bg",
        &DecoratedDoor::special_bg,
        sol::base_classes,
        sol::bases<Entity, Floor, Door, ExitDoor>());

    lua.new_usertype<LockedDoor>(
        "LockedDoor",
        "unlocked",
        &LockedDoor::unlocked,
        sol::base_classes,
        sol::bases<Entity, Floor, Door>());

    lua.new_usertype<CityOfGoldDoor>(
        "CityOfGoldDoor",
        "unlocked",
        &CityOfGoldDoor::unlocked,
        sol::base_classes,
        sol::bases<Entity, Floor, Door, ExitDoor, DecoratedDoor>());

    lua.new_usertype<MainExit>(
        "MainExit",
        sol::base_classes,
        sol::bases<Entity, Floor, Door, ExitDoor>());

    lua.new_usertype<EggShipDoor>(
        "EggShipDoor",
        "timer",
        &EggShipDoor::timer,
        "entered",
        &EggShipDoor::entered,
        sol::base_classes,
        sol::bases<Entity, Floor, Door>());

    lua.new_usertype<Arrowtrap>(
        "Arrowtrap",
        "arrow_shot",
        &Arrowtrap::arrow_shot,
        "rearm",
        &Arrowtrap::rearm,
        sol::base_classes,
        sol::bases<Entity, Floor>());

    lua.new_usertype<TotemTrap>(
        "TotemTrap",
        "spawn_entity_type",
        &TotemTrap::spawn_entity_type,
        "first_sound_id",
        &TotemTrap::first_sound_id,
        sol::base_classes,
        sol::bases<Entity, Floor>());

    lua.new_usertype<LaserTrap>(
        "LaserTrap",
        "emitted_light",
        &LaserTrap::emitted_light,
        "reset_timer",
        &LaserTrap::reset_timer,
        "phase_2",
        &LaserTrap::phase_2,
        sol::base_classes,
        sol::bases<Entity, Floor>());

    lua.new_usertype<SparkTrap>(
        "SparkTrap",
        "emitted_light",
        &SparkTrap::emitted_light,
        "spark_uid",
        &SparkTrap::spark_uid,
        sol::base_classes,
        sol::bases<Entity, Floor>());

    lua.new_usertype<Altar>(
        "Altar",
        "timer",
        &Altar::timer,
        sol::base_classes,
        sol::bases<Entity, Floor>());

    lua.new_usertype<SpikeballTrap>(
        "SpikeballTrap",
        "chain",
        &SpikeballTrap::chain,
        "end_piece",
        &SpikeballTrap::end_piece,
        "state",
        &SpikeballTrap::state,
        "timer",
        &SpikeballTrap::timer,
        sol::base_classes,
        sol::bases<Entity, Floor>());

    lua.new_usertype<TransferFloor>(
        "TransferFloor",
        sol::base_classes,
        sol::bases<Entity, Floor>());

    lua.new_usertype<ConveyorBelt>(
        "ConveyorBelt",
        "timer",
        &ConveyorBelt::timer,
        sol::base_classes,
        sol::bases<Entity, Floor, TransferFloor>());

    lua.new_usertype<Pipe>(
        "Pipe",
        "direction_type",
        &Pipe::direction_type,
        "end_pipe",
        &Pipe::end_pipe,
        sol::base_classes,
        sol::bases<Entity, Floor, TransferFloor>());

    lua.new_usertype<Generator>(
        "Generator",
        "spawned_uid",
        &Generator::spawned_uid,
        "set_timer",
        &Generator::set_timer,
        "timer",
        &Generator::timer,
        "start_counter",
        &Generator::start_counter,
        "on_off",
        &Generator::on_off,
        sol::base_classes,
        sol::bases<Entity, Floor>());

    lua.new_usertype<SlidingWallCeiling>(
        "SlidingWallCeiling",
        "attached_piece",
        &SlidingWallCeiling::attached_piece,
        "active_floor_part_uid",
        &SlidingWallCeiling::active_floor_part_uid,
        "state",
        &SlidingWallCeiling::state,
        sol::base_classes,
        sol::bases<Entity, Floor>());

    lua.new_usertype<QuickSand>(
        "QuickSand",
        sol::base_classes,
        sol::bases<Entity, Floor>());

    lua.new_usertype<BigSpearTrap>(
        "BigSpearTrap",
        "spear_uid",
        &BigSpearTrap::spear_uid,
        "left_part",
        &BigSpearTrap::left_part,
        sol::base_classes,
        sol::bases<Entity, Floor>());

    lua.new_usertype<StickyTrap>(
        "StickyTrap",
        "attached_piece_uid",
        &StickyTrap::attached_piece_uid,
        "ball_uid",
        &StickyTrap::ball_uid,
        "state",
        &StickyTrap::state,
        "timer",
        &StickyTrap::timer,
        sol::base_classes,
        sol::bases<Entity, Floor>());

    lua.new_usertype<MotherStatue>(
        "MotherStatue",
        "player1_standing",
        &MotherStatue::player1_standing,
        "player2_standing",
        &MotherStatue::player2_standing,
        "player3_standing",
        &MotherStatue::player3_standing,
        "player4_standing",
        &MotherStatue::player4_standing,
        "player1_health_received",
        &MotherStatue::player1_health_received,
        "player2_health_received",
        &MotherStatue::player2_health_received,
        "player3_health_received",
        &MotherStatue::player3_health_received,
        "player4_health_received",
        &MotherStatue::player4_health_received,
        "player1_health_timer",
        &MotherStatue::player1_health_timer,
        "player2_health_timer",
        &MotherStatue::player2_health_timer,
        "player3_health_timer",
        &MotherStatue::player3_health_timer,
        "player4_health_timer",
        &MotherStatue::player4_health_timer,
        "eggplantchild_timer",
        &MotherStatue::eggplantchild_timer,
        "eggplantchild_detected",
        &MotherStatue::eggplantchild_detected,
        sol::base_classes,
        sol::bases<Entity, Floor>());

    lua.new_usertype<TeleportingBorder>(
        "TeleportingBorder",
        "direction",
        &TeleportingBorder::direction,
        sol::base_classes,
        sol::bases<Entity, Floor>());

    lua.new_usertype<ForceField>(
        "ForceField",
        "first_item_beam",
        &ForceField::first_item_beam,
        "fx",
        &ForceField::fx,
        "emitted_light",
        &ForceField::emitted_light,
        "is_on",
        sol::readonly(&ForceField::is_on),
        "activate_laserbeam",
        &ForceField::activate_laserbeam,
        sol::base_classes,
        sol::bases<Entity, Floor>());

    lua.new_usertype<TimedForceField>(
        "TimedForceField",
        "timer",
        &TimedForceField::timer,
        "pause",
        &TimedForceField::pause,
        sol::base_classes,
        sol::bases<Entity, Floor, ForceField>());

    lua.new_usertype<HorizontalForceField>(
        "HorizontalForceField",
        "first_item_beam",
        &HorizontalForceField::first_item_beam,
        "fx",
        &HorizontalForceField::fx,
        "timer",
        &HorizontalForceField::timer,
        "is_on",
        sol::readonly(&HorizontalForceField::is_on),
        sol::base_classes,
        sol::bases<Entity, Floor>());

    lua.new_usertype<TentacleBottom>(
        "TentacleBottom",
        "attached_piece_uid",
        &TentacleBottom::attached_piece_uid,
        "tentacle_uid",
        &TentacleBottom::tentacle_uid,
        sol::base_classes,
        sol::bases<Entity, Floor>());

    lua.new_usertype<PoleDeco>(
        "PoleDeco",
        "deco_up",
        &PoleDeco::deco_up,
        "deco_down",
        &PoleDeco::deco_down,
        sol::base_classes,
        sol::bases<Entity, Floor>());
}
} // namespace NEntitiesFloors
