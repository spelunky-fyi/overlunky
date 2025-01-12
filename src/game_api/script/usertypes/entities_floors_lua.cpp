#include "entities_floors_lua.hpp"

#include <algorithm>   // for max
#include <locale>      // for num_put
#include <new>         // for operator new
#include <sol/sol.hpp> // for proxy_key_t, state, table_proxy, data_t
#include <string>      // for operator==, allocator
#include <tuple>       // for get
#include <type_traits> // for move, declval
#include <utility>     // for min, max, swap

#include "entities_floors.hpp" // for MotherStatue, Floor, ExitDoor, Door
#include "entity.hpp"          // for Entity
#include "illumination.hpp"    // IWYU pragma: keep
#include "rpc.hpp"             // for set_door_target, get_door_target
#include "sound_manager.hpp"   // IWYU pragma: keep

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
    lua["Entity"]["as_eggshipdoors"] = &Entity::as<EggShipDoorS>;
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
    lua["Entity"]["as_junglespeartrap"] = &Entity::as<JungleSpearTrap>;

    auto floor_type = lua.new_usertype<Floor>("Floor", sol::base_classes, sol::bases<Entity>());
    floor_type["deco_top"] = &Floor::deco_top;
    floor_type["deco_bottom"] = &Floor::deco_bottom;
    floor_type["deco_left"] = &Floor::deco_left;
    floor_type["deco_right"] = &Floor::deco_right;
    floor_type["fix_border_tile_animation"] = &Floor::fix_border_tile_animation;
    floor_type["fix_decorations"] = &Floor::fix_decorations;
    floor_type["add_decoration"] = &Floor::add_decoration;
    floor_type["remove_decoration"] = &Floor::remove_decoration;
    floor_type["decorate_internal"] = &Floor::decorate_internal;
    floor_type["get_floor_type"] = &Floor::get_floor_type;

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
        "enter",
        &Door::enter,
        "light_level",
        &Door::light_level,
        "is_unlocked",
        &Door::is_unlocked,
        "can_enter",
        &Door::can_enter,
        "unlock",
        &Door::unlock,
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
        "set_target",
        &ExitDoor::set_target,
        "get_target",
        &ExitDoor::get_target,
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
        "sound",
        &MainExit::sound,
        "door_blocker",
        &MainExit::door_blocker,
        sol::base_classes,
        sol::bases<Entity, Floor, Door, ExitDoor>());

    lua.new_usertype<EggShipDoor>(
        "EggShipDoor",
        "timer",
        &EggShipDoor::timer,
        sol::base_classes,
        sol::bases<Entity, Floor, Door>());

    lua.new_usertype<EggShipDoorS>(
        "EggShipDoorS",
        "entered",
        &EggShipDoorS::entered,
        sol::base_classes,
        sol::bases<Entity, Floor, Door, EggShipDoor>());

    lua.new_usertype<Arrowtrap>(
        "Arrowtrap",
        "arrow_shot",
        &Arrowtrap::arrow_shot,
        "rearm",
        &Arrowtrap::rearm,
        "trigger",
        &Arrowtrap::trigger,
        sol::base_classes,
        sol::bases<Entity, Floor>());

    lua.new_usertype<TotemTrap>(
        "TotemTrap",
        "spawn_entity_type",
        &TotemTrap::spawn_entity_type,
        "first_sound_id",
        &TotemTrap::first_sound_id,
        "trigger",
        &TotemTrap::trigger,
        sol::base_classes,
        sol::bases<Entity, Floor>());

    lua.new_usertype<LaserTrap>(
        "LaserTrap",
        "emitted_light",
        &LaserTrap::emitted_light,
        "timer",
        &LaserTrap::timer,
        "reset_timer", // Deprecated
        &LaserTrap::reset_timer,
        "phase_2", // Deprecated
        &LaserTrap::phase_2,
        "trigger",
        &LaserTrap::trigger,
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
        "sound",
        &SpikeballTrap::sound,
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
        "transferred_entities",
        &TransferFloor::transferred_entities,
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
        sol::bases<Entity, Floor>());

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
        "ball_rise",
        &SlidingWallCeiling::ball_rise,
        "ball_drop",
        &SlidingWallCeiling::ball_drop,
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
        "trigger",
        &BigSpearTrap::trigger,
        sol::base_classes,
        sol::bases<Entity, Floor>());

    lua.new_usertype<StickyTrap>(
        "StickyTrap",
        "sound",
        &StickyTrap::sound,
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

    auto motherstatue_type = lua.new_usertype<MotherStatue>("MotherStatue", sol::base_classes, sol::bases<Entity, Floor>());
    motherstatue_type["players_standing"] = &MotherStatue::players_standing;
    motherstatue_type["player1_standing"] = &MotherStatue::player1_standing;
    motherstatue_type["player2_standing"] = &MotherStatue::player2_standing;
    motherstatue_type["player3_standing"] = &MotherStatue::player3_standing;
    motherstatue_type["player4_standing"] = &MotherStatue::player4_standing;
    motherstatue_type["players_health_received"] = &MotherStatue::players_health_received;
    motherstatue_type["player1_health_received"] = &MotherStatue::player1_health_received;
    motherstatue_type["player2_health_received"] = &MotherStatue::player2_health_received;
    motherstatue_type["player3_health_received"] = &MotherStatue::player3_health_received;
    motherstatue_type["player4_health_received"] = &MotherStatue::player4_health_received;
    motherstatue_type["players_health_timer"] = &MotherStatue::players_health_timer;
    motherstatue_type["player1_health_timer"] = &MotherStatue::player1_health_timer;
    motherstatue_type["player2_health_timer"] = &MotherStatue::player2_health_timer;
    motherstatue_type["player3_health_timer"] = &MotherStatue::player3_health_timer;
    motherstatue_type["player4_health_timer"] = &MotherStatue::player4_health_timer;
    motherstatue_type["eggplantchild_timer"] = &MotherStatue::eggplantchild_timer;
    motherstatue_type["eggplantchild_detected"] = &MotherStatue::eggplantchild_detected;

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
        "sound",
        &ForceField::sound,
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
        "sound",
        &HorizontalForceField::sound,
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
        "state",
        &TentacleBottom::state,
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

    lua.new_usertype<JungleSpearTrap>(
        "JungleSpearTrap",
        "trigger",
        &JungleSpearTrap::trigger,
        sol::base_classes,
        sol::bases<Entity, Floor>());

    /// Make an ENT_TYPE.FLOOR_DOOR_EXIT go to world `w`, level `l`, theme `t`
    lua["set_door_target"] = [](uint32_t uid, uint8_t w, uint8_t l, uint8_t t)
    {
        if (auto door = get_entity_ptr(uid)->as<ExitDoor>())
            door->set_target(w, l, t);
    };
    /// Short for [set_door_target](#set_door_target).
    lua["set_door"] = [](uint32_t uid, uint8_t w, uint8_t l, uint8_t t)
    {
        if (auto door = get_entity_ptr(uid)->as<ExitDoor>())
            door->set_target(w, l, t);
    };
    /// Get door target `world`, `level`, `theme`
    lua["get_door_target"] = [](uint32_t uid) -> std::tuple<uint8_t, uint8_t, uint8_t>
    {
        auto door = get_entity_ptr(uid)->as<ExitDoor>();
        if (door == nullptr || !door->special_door)
            return {};

        return std::make_tuple(door->world, door->level, door->theme);
    };
    /// Calls the enter door function, position doesn't matter, can also enter closed doors (like COG, EW) without unlocking them
    lua["enter_door"] = enter_door;
}
} // namespace NEntitiesFloors
