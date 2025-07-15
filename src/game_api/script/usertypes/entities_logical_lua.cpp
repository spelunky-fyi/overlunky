#include "entities_logical_lua.hpp"

#include <algorithm>   // for max
#include <new>         // for operator new
#include <sol/sol.hpp> // for proxy_key_t, state, basic_table_core...
#include <string>      // for operator==, allocator
#include <tuple>       // for get
#include <type_traits> // for move, declval
#include <utility>     // for min, max

#include "entities_logical.hpp" // for Portal, DMSpawning, LogicalDoor, Our...
#include "entity.hpp"           // for Entity
#include "illumination.hpp"     // IWYU pragma: keep
#include "sound_manager.hpp"    // IWYU pragma: keep

namespace NEntitiesLogical
{
void register_usertypes(sol::state& lua)
{
    lua["Entity"]["as_portal"] = &Entity::as<Portal>;
    lua["Entity"]["as_shootingstarspawner"] = &Entity::as<ShootingStarSpawner>;
    lua["Entity"]["as_logicaldoor"] = &Entity::as<LogicalDoor>;
    lua["Entity"]["as_logicalsound"] = &Entity::as<LogicalSound>;
    lua["Entity"]["as_logicalstaticsound"] = &Entity::as<LogicalStaticSound>;
    lua["Entity"]["as_logicalliquidstreamsound"] = &Entity::as<LogicalLiquidStreamSound>;
    lua["Entity"]["as_logicaltraptrigger"] = &Entity::as<LogicalTrapTrigger>;
    lua["Entity"]["as_jungletraptrigger"] = &Entity::as<JungleTrapTrigger>;
    lua["Entity"]["as_weteffect"] = &Entity::as<WetEffect>;
    lua["Entity"]["as_onfireeffect"] = &Entity::as<OnFireEffect>;
    lua["Entity"]["as_poisonedeffect"] = &Entity::as<PoisonedEffect>;
    lua["Entity"]["as_cursedeffect"] = &Entity::as<CursedEffect>;
    lua["Entity"]["as_ouroborocameraanchor"] = &Entity::as<OuroboroCameraAnchor>;
    lua["Entity"]["as_ouroborocamerazoomin"] = &Entity::as<OuroboroCameraZoomin>;
    lua["Entity"]["as_cinematicanchor"] = &Entity::as<CinematicAnchor>;
    lua["Entity"]["as_burningropeeffect"] = &Entity::as<BurningRopeEffect>;
    lua["Entity"]["as_dustwallapep"] = &Entity::as<DustWallApep>;
    lua["Entity"]["as_cameraflash"] = &Entity::as<CameraFlash>;
    lua["Entity"]["as_roomlight"] = &Entity::as<RoomLight>;
    lua["Entity"]["as_limbanchor"] = &Entity::as<LimbAnchor>;
    lua["Entity"]["as_logicalconveyorbeltsound"] = &Entity::as<LogicalConveyorbeltSound>;
    lua["Entity"]["as_logicalanchovyflock"] = &Entity::as<LogicalAnchovyFlock>;
    lua["Entity"]["as_mummyfliessound"] = &Entity::as<MummyFliesSound>;
    lua["Entity"]["as_quicksandsound"] = &Entity::as<QuickSandSound>;
    lua["Entity"]["as_iceslidingsound"] = &Entity::as<IceSlidingSound>;
    lua["Entity"]["as_frostbreatheffect"] = &Entity::as<FrostBreathEffect>;
    lua["Entity"]["as_boulderspawner"] = &Entity::as<BoulderSpawner>;
    lua["Entity"]["as_pipetravelersound"] = &Entity::as<PipeTravelerSound>;
    lua["Entity"]["as_logicaldrain"] = &Entity::as<LogicalDrain>;
    lua["Entity"]["as_logicalregeneratingblock"] = &Entity::as<LogicalRegeneratingBlock>;
    lua["Entity"]["as_splashbubblegenerator"] = &Entity::as<SplashBubbleGenerator>;
    lua["Entity"]["as_eggplantthrower"] = &Entity::as<EggplantThrower>;
    lua["Entity"]["as_logicalminigame"] = &Entity::as<LogicalMiniGame>;
    lua["Entity"]["as_dmspawning"] = &Entity::as<DMSpawning>;
    lua["Entity"]["as_dmalienblast"] = &Entity::as<DMAlienBlast>;

    lua.new_usertype<Portal>(
        "Portal",
        "emitted_light",
        &Portal::emitted_light,
        "transition_timer",
        &Portal::transition_timer,
        "level",
        &Portal::level,
        "world",
        &Portal::world,
        "theme",
        &Portal::theme,
        "timer",
        &Portal::timer,
        sol::base_classes,
        sol::bases<Entity>());

    lua.new_usertype<ShootingStarSpawner>(
        "ShootingStarSpawner",
        "timer",
        &ShootingStarSpawner::timer,
        sol::base_classes,
        sol::bases<Entity>());

    lua.new_usertype<LogicalDoor>(
        "LogicalDoor",
        "door_type",
        &LogicalDoor::door_type,
        "platform_type",
        &LogicalDoor::platform_type,
        "visible",
        &LogicalDoor::not_hidden,
        "platform_spawned",
        &LogicalDoor::platform_spawned,
        sol::base_classes,
        sol::bases<Entity>());

    lua.new_usertype<LogicalSound>(
        "LogicalSound",
        "sound",
        &LogicalSound::sound,
        sol::base_classes,
        sol::bases<Entity>());

    lua.new_usertype<LogicalStaticSound>(
        "LogicalStaticSound",
        sol::base_classes,
        sol::bases<Entity, LogicalSound>());

    lua.new_usertype<LogicalLiquidStreamSound>(
        "LogicalLiquidStreamSound",
        sol::base_classes,
        sol::bases<Entity, LogicalSound, LogicalStaticSound>());

    lua.new_usertype<LogicalTrapTrigger>(
        "LogicalTrapTrigger",
        "min_empty_distance",
        &LogicalTrapTrigger::min_empty_distance,
        "trigger_distance",
        &LogicalTrapTrigger::trigger_distance,
        "vertical",
        &LogicalTrapTrigger::vertical,
        sol::base_classes,
        sol::bases<Entity>());

    lua.new_usertype<JungleTrapTrigger>(
        "JungleTrapTrigger",
        sol::base_classes,
        sol::bases<Entity, LogicalTrapTrigger>());

    lua.new_usertype<WetEffect>(
        "WetEffect",
        "particle",
        &WetEffect::particle,
        sol::base_classes,
        sol::bases<Entity>());

    lua.new_usertype<OnFireEffect>(
        "OnFireEffect",
        "particle_smoke",
        &OnFireEffect::particle_smoke,
        "particle_flame",
        &OnFireEffect::particle_flame,
        "illumination",
        &OnFireEffect::illumination,
        sol::base_classes,
        sol::bases<Entity>());

    lua.new_usertype<PoisonedEffect>(
        "PoisonedEffect",
        "particle_burst",
        &PoisonedEffect::particle_burst,
        "particle_base",
        &PoisonedEffect::particle_base,
        "burst_timer",
        &PoisonedEffect::burst_timer,
        "burst_active",
        &PoisonedEffect::burst_active,
        sol::base_classes,
        sol::bases<Entity>());

    lua.new_usertype<CursedEffect>(
        "CursedEffect",
        "particle",
        &CursedEffect::particle,
        "sound",
        &CursedEffect::sound,
        sol::base_classes,
        sol::bases<Entity>());

    lua.new_usertype<OuroboroCameraAnchor>(
        "OuroboroCameraAnchor",
        "target_x",
        &OuroboroCameraAnchor::target_x,
        "target_y",
        &OuroboroCameraAnchor::target_y,
        "velocity_x",
        &OuroboroCameraAnchor::velocity_x,
        "velocity_y",
        &OuroboroCameraAnchor::velocity_y,
        sol::base_classes,
        sol::bases<Entity>());

    lua.new_usertype<OuroboroCameraZoomin>(
        "OuroboroCameraZoomin",
        "zoomin_level",
        &OuroboroCameraZoomin::zoomin_level,
        sol::base_classes,
        sol::bases<Entity>());

    lua.new_usertype<CinematicAnchor>(
        "CinematicAnchor",
        "blackbar_top",
        &CinematicAnchor::blackbar_top,
        "blackbar_bottom",
        &CinematicAnchor::blackbar_bottom,
        "roll_in",
        &CinematicAnchor::roll_in,
        sol::base_classes,
        sol::bases<Entity>());

    lua.new_usertype<BurningRopeEffect>(
        "BurningRopeEffect",
        "illumination",
        &BurningRopeEffect::illumination,
        "sound",
        &BurningRopeEffect::sound,
        sol::base_classes,
        sol::bases<Entity>());

    lua.new_usertype<DustWallApep>(
        "DustWallApep",
        "particle",
        &DustWallApep::particle,
        sol::base_classes,
        sol::bases<Entity>());

    lua.new_usertype<CameraFlash>(
        "CameraFlash",
        "illumination1",
        &CameraFlash::illumination1,
        "illumination2",
        &CameraFlash::illumination2,
        "timer",
        &CameraFlash::timer,
        sol::base_classes,
        sol::bases<Entity>());

    lua.new_usertype<RoomLight>(
        "RoomLight",
        "illumination",
        &RoomLight::illumination,
        sol::base_classes,
        sol::bases<Entity>());

    auto limbanchor_type = lua.new_usertype<LimbAnchor>(
        "LimbAnchor",
        "anchor_x",
        &LimbAnchor::anchor_x,
        "anchor_y",
        &LimbAnchor::anchor_y,
        "move_origin_x",
        &LimbAnchor::move_origin_x,
        "move_origin_y",
        &LimbAnchor::move_origin_y,
        "move_destination_x",
        &LimbAnchor::move_destination_x,
        "move_destination_y",
        &LimbAnchor::move_destination_y,
        "elapsed_move_time",
        &LimbAnchor::elapsed_move_time,
        "move_duration",
        &LimbAnchor::move_duration,
        "flip_vertical",
        &LimbAnchor::flip_vertical,
        sol::base_classes,
        sol::bases<Entity>());

    /// NoDoc
    limbanchor_type["move_timer"] = &LimbAnchor::elapsed_move_time; // For backward compatibility.

    lua.new_usertype<LogicalConveyorbeltSound>(
        "LogicalConveyorbeltSound",
        sol::base_classes,
        sol::bases<Entity, LogicalSound>());

    lua.new_usertype<LogicalAnchovyFlock>(
        "LogicalAnchovyFlock",
        "current_speed",
        &LogicalAnchovyFlock::current_speed,
        "max_speed",
        &LogicalAnchovyFlock::max_speed,
        "timer",
        &LogicalAnchovyFlock::timer,
        sol::base_classes,
        sol::bases<Entity>());

    lua.new_usertype<MummyFliesSound>(
        "MummyFliesSound",
        "mummy_uid",
        &MummyFliesSound::mummy_uid,
        "flies",
        &MummyFliesSound::flies,
        sol::base_classes,
        sol::bases<Entity, LogicalSound>());

    lua.new_usertype<QuickSandSound>(
        "QuickSandSound",
        sol::base_classes,
        sol::bases<Entity, LogicalSound>());

    lua.new_usertype<IceSlidingSound>(
        "IceSlidingSound",
        sol::base_classes,
        sol::bases<Entity, LogicalSound>());

    lua.new_usertype<FrostBreathEffect>(
        "FrostBreathEffect",
        "timer",
        &FrostBreathEffect::timer,
        sol::base_classes,
        sol::bases<Entity>());

    lua.new_usertype<BoulderSpawner>(
        "BoulderSpawner",
        "timer",
        &BoulderSpawner::timer,
        "owner_uid",
        &BoulderSpawner::owner_uid,
        "sound",
        &BoulderSpawner::sound,
        sol::base_classes,
        sol::bases<Entity>());

    lua.new_usertype<PipeTravelerSound>(
        "PipeTravelerSound",
        "enter_exit",
        &PipeTravelerSound::enter_exit,
        sol::base_classes,
        sol::bases<Entity, LogicalSound>());

    lua.new_usertype<LogicalDrain>(
        "LogicalDrain",
        "timer",
        &LogicalDrain::timer,
        sol::base_classes,
        sol::bases<Entity>());

    lua.new_usertype<LogicalRegeneratingBlock>(
        "LogicalRegeneratingBlock",
        "timer",
        &LogicalRegeneratingBlock::timer,
        sol::base_classes,
        sol::bases<Entity>());

    lua.new_usertype<SplashBubbleGenerator>(
        "SplashBubbleGenerator",
        "timer",
        &SplashBubbleGenerator::timer,
        sol::base_classes,
        sol::bases<Entity>());

    lua.new_usertype<EggplantThrower>(
        "EggplantThrower",
        sol::base_classes,
        sol::bases<Entity>());

    lua.new_usertype<LogicalMiniGame>(
        "LogicalMiniGame",
        "timer",
        &LogicalMiniGame::timer,
        sol::base_classes,
        sol::bases<Entity>());

    lua.new_usertype<DMSpawning>(
        "DMSpawning",
        "spawn_x",
        &DMSpawning::spawn_x,
        "spawn_y",
        &DMSpawning::spawn_y,
        "sine_pos",
        &DMSpawning::sine_pos,
        "timer",
        &DMSpawning::timer,
        sol::base_classes,
        sol::bases<Entity>());

    lua.new_usertype<DMAlienBlast>(
        "DMAlienBlast",
        "owner_uid",
        &DMAlienBlast::owner_uid,
        "timer",
        &DMAlienBlast::timer,
        "sound",
        &DMAlienBlast::sound,
        "reticule_internal",
        &DMAlienBlast::fx_internal_reticule,
        "reticule_external",
        &DMAlienBlast::fx_external_reticule,
        sol::base_classes,
        sol::bases<Entity>());
}
}; // namespace NEntitiesLogical
