#include "entities_items_lua.hpp"

#include "entities_items.hpp"
#include "entity.hpp"
#include "script/script_impl.hpp"

#include <sol/sol.hpp>

namespace NEntitiesItems
{
void register_usertypes(sol::state& lua, ScriptImpl* script)
{
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
    lua["Entity"]["as_chest"] = &Entity::as<Chest>;
    lua["Entity"]["as_treasure"] = &Entity::as<Treasure>;
    lua["Entity"]["as_hundunchest"] = &Entity::as<HundunChest>;
    lua["Entity"]["as_boombox"] = &Entity::as<Boombox>;
    lua["Entity"]["as_tv"] = &Entity::as<TV>;
    lua["Entity"]["as_telescope"] = &Entity::as<Telescope>;
    lua["Entity"]["as_torch"] = &Entity::as<Torch>;
    lua["Entity"]["as_walltorch"] = &Entity::as<WallTorch>;
    lua["Entity"]["as_torchflame"] = &Entity::as<TorchFlame>;
    lua["Entity"]["as_lampflame"] = &Entity::as<LampFlame>;
    lua["Entity"]["as_bullet"] = &Entity::as<Bullet>;
    lua["Entity"]["as_timedshot"] = &Entity::as<TimedShot>;
    lua["Entity"]["as_clonegunshot"] = &Entity::as<CloneGunShot>;
    lua["Entity"]["as_punishball"] = &Entity::as<PunishBall>;
    lua["Entity"]["as_chain"] = &Entity::as<Chain>;
    lua["Entity"]["as_container"] = &Entity::as<Container>;
    lua["Entity"]["as_coffin"] = &Entity::as<Coffin>;
    lua["Entity"]["as_fly"] = &Entity::as<Fly>;
    lua["Entity"]["as_olmeccannon"] = &Entity::as<OlmecCannon>;
    lua["Entity"]["as_landmine"] = &Entity::as<Landmine>;
    lua["Entity"]["as_udjatsocket"] = &Entity::as<UdjatSocket>;
    lua["Entity"]["as_ushabti"] = &Entity::as<Ushabti>;
    lua["Entity"]["as_honey"] = &Entity::as<Honey>;
    lua["Entity"]["as_giantclamtop"] = &Entity::as<GiantClamTop>;
    lua["Entity"]["as_playerghost"] = &Entity::as<PlayerGhost>;
    lua["Entity"]["as_ghostbreath"] = &Entity::as<GhostBreath>;
    lua["Entity"]["as_laserbeam"] = &Entity::as<LaserBeam>;
    lua["Entity"]["as_treasurehook"] = &Entity::as<TreasureHook>;
    lua["Entity"]["as_axolotlshot"] = &Entity::as<AxolotlShot>;
    lua["Entity"]["as_trappart"] = &Entity::as<TrapPart>;
    lua["Entity"]["as_skulldroptrap"] = &Entity::as<SkullDropTrap>;
    lua["Entity"]["as_frozenliquid"] = &Entity::as<FrozenLiquid>;
    lua["Entity"]["as_switch"] = &Entity::as<Switch>;
    lua["Entity"]["as_flyhead"] = &Entity::as<FlyHead>;
    lua["Entity"]["as_snaptrap"] = &Entity::as<SnapTrap>;
    lua["Entity"]["as_empressgrave"] = &Entity::as<EmpressGrave>;
    lua["Entity"]["as_tentacle"] = &Entity::as<Tentacle>;
    lua["Entity"]["as_minigameship"] = &Entity::as<MiniGameShip>;
    lua["Entity"]["as_minigameasteroid"] = &Entity::as<MiniGameAsteroid>;
    lua["Entity"]["as_pot"] = &Entity::as<Pot>;
    lua["Entity"]["as_cursedpot"] = &Entity::as<CursedPot>;
    lua["Entity"]["as_cookfire"] = &Entity::as<CookFire>;
    lua["Entity"]["as_orb"] = &Entity::as<Orb>;
    lua["Entity"]["as_eggsac"] = &Entity::as<EggSac>;
    lua["Entity"]["as_goldbar"] = &Entity::as<Goldbar>;
    lua["Entity"]["as_coin"] = &Entity::as<Coin>;
    lua["Entity"]["as_rollingitem"] = &Entity::as<RollingItem>;
    lua["Entity"]["as_playerbag"] = &Entity::as<PlayerBag>;
    lua["Entity"]["as_parachutepowerup"] = &Entity::as<ParachutePowerup>;
    lua["Entity"]["as_truecrownpowerup"] = &Entity::as<TrueCrownPowerup>;
    lua["Entity"]["as_ankhpowerup"] = &Entity::as<AnkhPowerup>;
    lua["Entity"]["as_yellowcape"] = &Entity::as<YellowCape>;
    lua["Entity"]["as_teleporter"] = &Entity::as<Teleporter>;
    lua["Entity"]["as_boomerang"] = &Entity::as<Boomerang>;
    lua["Entity"]["as_excalibur"] = &Entity::as<Excalibur>;
    lua["Entity"]["as_shield"] = &Entity::as<Shield>;

    lua.new_usertype<Bomb>(
        "Bomb", "scale_hor",
        &Bomb::scale_hor,
        "scale_ver",
        &Bomb::scale_ver,
        "is_big_bomb",
        &Bomb::is_big_bomb,
        sol::base_classes,
        sol::bases<Entity, Movable>());

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
        "fx_entity",
        &Spark::fx_entity,
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

    lua.new_usertype<Chest>(
        "Chest",
        "leprechaun",
        &Chest::leprechaun,
        "bomb",
        &Chest::bomb,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<Treasure>(
        "Treasure",
        "cashed",
        &Treasure::cashed,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<HundunChest>(
        "HundunChest",
        "timer",
        &HundunChest::timer,
        sol::base_classes,
        sol::bases<Entity, Movable, Treasure>());

    lua.new_usertype<Boombox>(
        "Boombox",
        "fx_button",
        &Boombox::fx_button,
        "music_note1",
        &Boombox::music_note1,
        "music_note2",
        &Boombox::music_note2,
        "spawn_y",
        &Boombox::spawn_y,
        "station",
        &Boombox::station,
        "station_change_delay",
        &Boombox::station_change_delay,
        "jump_timer",
        &Boombox::jump_timer,
        "jump_state",
        &Boombox::jump_state,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<TV>(
        "TV",
        "fx_button",
        &TV::fx_button,
        "emitted_light",
        &TV::emitted_light,
        "station",
        &TV::station,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<Telescope>(
        "Telescope",
        "fx_button",
        &Telescope::fx_button,
        "camera_anchor",
        &Telescope::camera_anchor,
        "looked_through_by_uid",
        &Telescope::looked_through_by_uid,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<Torch>(
        "Torch",
        "flame_uid",
        &Torch::flame_uid,
        "is_lit",
        &Torch::is_lit,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<WallTorch>(
        "WallTorch",
        "dropped_gold",
        &WallTorch::dropped_gold,
        sol::base_classes,
        sol::bases<Entity, Movable, Torch>());

    lua.new_usertype<TorchFlame>(
        "TorchFlame",
        "smoke_particle",
        &TorchFlame::smoke_particle,
        "flame_particle",
        &TorchFlame::flame_particle,
        "warp_particle",
        &TorchFlame::warp_particle,
        "flame_size",
        &TorchFlame::flame_size,
        sol::base_classes,
        sol::bases<Entity, Movable, Flame>());

    lua.new_usertype<LampFlame>(
        "LampFlame",
        "flame_particle",
        &LampFlame::flame_particle,
        sol::base_classes,
        sol::bases<Entity, Movable, Flame>());

    lua.new_usertype<Bullet>(
        "Bullet",
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<TimedShot>(
        "TimedShot",
        "timer",
        &TimedShot::timer,
        sol::base_classes,
        sol::bases<Entity, Movable, LightShot>());

    lua.new_usertype<CloneGunShot>(
        "CloneGunShot",
        "spawn_y",
        &CloneGunShot::spawn_y,
        sol::base_classes,
        sol::bases<Entity, Movable, LightShot, TimedShot>());

    lua.new_usertype<PunishBall>(
        "PunishBall",
        "attached_to_uid",
        &PunishBall::attached_to_uid,
        "x_pos",
        &PunishBall::x_pos,
        "y_pos",
        &PunishBall::y_pos,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<Chain>(
        "Chain",
        "attached_to_uid",
        &Chain::attached_to_uid,
        "timer",
        &Chain::timer,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<Container>(
        "Container",
        "inside",
        &Container::inside,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<Coffin>(
        "Coffin",
        "timer",
        &Coffin::timer,
        sol::base_classes,
        sol::bases<Entity, Movable, Container>());

    lua.new_usertype<Fly>(
        "Fly",
        "timer",
        &Fly::timer,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<OlmecCannon>(
        "OlmecCannon",
        "timer",
        &OlmecCannon::timer,
        "bombs_left",
        &OlmecCannon::bombs_left,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<Landmine>(
        "Landmine",
        "timer",
        &Landmine::timer,
        sol::base_classes,
        sol::bases<Entity, Movable, LightEmitter>());

    lua.new_usertype<UdjatSocket>(
        "UdjatSocket",
        "fx_button",
        &UdjatSocket::fx_button,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<Ushabti>(
        "Ushabti",
        "wiggle_timer",
        &Ushabti::wiggle_timer,
        "shine_timer",
        &Ushabti::shine_timer,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<Honey>(
        "Honey",
        "wiggle_timer",
        &Honey::wiggle_timer,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<GiantClamTop>(
        "GiantClamTop",
        "close_timer",
        &GiantClamTop::close_timer,
        "open_timer",
        &GiantClamTop::open_timer,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<PlayerGhost>(
        "PlayerGhost",
        "sparkles_particle",
        &PlayerGhost::sparkles_particle,
        "player_inputs",
        &PlayerGhost::player_inputs,
        "inventory",
        &PlayerGhost::inventory,
        "shake_timer",
        &PlayerGhost::shake_timer,
        "boost_timer",
        &PlayerGhost::boost_timer,
        sol::base_classes,
        sol::bases<Entity, Movable, LightEmitter>());

    lua.new_usertype<GhostBreath>(
        "GhostBreath",
        "timer",
        &GhostBreath::timer,
        "big_cloud",
        &GhostBreath::big_cloud,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<LaserBeam>(
        "LaserBeam",
        "sparks",
        &LaserBeam::sparks,
        "emitted_light",
        &LaserBeam::emitted_light,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<TreasureHook>(
        "TreasureHook",
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<AxolotlShot>(
        "AxolotlShot",
        "trapped_uid",
        &AxolotlShot::trapped_uid,
        "size",
        &AxolotlShot::size,
        "swing",
        &AxolotlShot::swing,
        "swing_periodicity",
        &AxolotlShot::swing_periodicity,
        "distance_after_capture",
        &AxolotlShot::distance_after_capture,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<TrapPart>(
        "TrapPart",
        "ceiling",
        &TrapPart::ceiling,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<SkullDropTrap>(
        "SkullDropTrap",
        "left_skull_uid",
        &SkullDropTrap::left_skull_uid,
        "middle_skull_uid",
        &SkullDropTrap::middle_skull_uid,
        "right_skull_uid",
        &SkullDropTrap::right_skull_uid,
        "left_skull_drop_time",
        &SkullDropTrap::left_skull_drop_time,
        "middle_skull_drop_time",
        &SkullDropTrap::middle_skull_drop_time,
        "right_skull_drop_time",
        &SkullDropTrap::right_skull_drop_time,
        "timer",
        &SkullDropTrap::timer,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<FrozenLiquid>(
        "FrozenLiquid",
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<Switch>(
        "Switch",
        "timer",
        &Switch::timer,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<FlyHead>(
        "FlyHead",
        "vored_entity_uid",
        &FlyHead::vored_entity_uid,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<SnapTrap>(
        "SnapTrap",
        "bait_uid",
        &SnapTrap::bait_uid,
        "reload_timer",
        &SnapTrap::reload_timer,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<EmpressGrave>(
        "EmpressGrave",
        "fx_button",
        &EmpressGrave::fx_button,
        "ghost",
        &EmpressGrave::ghost,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<Tentacle>(
        "Tentacle",
        "bottom",
        &Tentacle::bottom,
        sol::base_classes,
        sol::bases<Entity, Movable, Chain>());

    lua.new_usertype<MiniGameShip>(
        "MiniGameShip",
        "velocity_x",
        &MiniGameShip::velocity_x,
        "velocity_y",
        &MiniGameShip::velocity_y,
        "swing",
        &MiniGameShip::swing,
        "up_down_normal",
        &MiniGameShip::up_down_normal,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<MiniGameAsteroid>(
        "MiniGameAsteroid",
        "spin_speed",
        &MiniGameAsteroid::spin_speed,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<Pot>(
        "Pot",
        "dont_transfer_dmg",
        &Pot::dont_transfer_dmg,
        sol::base_classes,
        sol::bases<Entity, Movable, Container>());

    lua.new_usertype<CursedPot>(
        "CursedPot",
        "smoke",
        &CursedPot::smoke,
        "smoke2",
        &CursedPot::smoke2,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<CookFire>(
        "CookFire",
        "lit",
        &CookFire::lit,
        "emitted_light",
        &CookFire::emitted_light,
        "particles_smoke",
        &CookFire::particles_smoke,
        "particles_flames",
        &CookFire::particles_flames,
        "particles_warp",
        &CookFire::particles_warp,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<Orb>(
        "Orb",
        "timer",
        &Orb::timer,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<EggSac>(
        "EggSac",
        "timer",
        &EggSac::timer,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<Goldbar>(
        "Goldbar",
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<Coin>(
        "Coin",
        "nominal_price",
        &Coin::nominal_price,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<RollingItem>(
        "RollingItem",
        "roll_speed",
        &RollingItem::roll_speed,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<PlayerBag>(
        "PlayerBag",
        "bombs",
        &PlayerBag::bombs,
        "ropes",
        &PlayerBag::ropes,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<ParachutePowerup>(
        "ParachutePowerup",
        "falltime_deploy",
        &ParachutePowerup::falltime_deploy,
        "deployed",
        sol::readonly(&ParachutePowerup::deployed),
        "after_deploy",
        &ParachutePowerup::after_deploy,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<TrueCrownPowerup>(
        "TrueCrownPowerup",
        "timer",
        &TrueCrownPowerup::timer,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<AnkhPowerup>(
        "AnkhPowerup",
        "player",
        &AnkhPowerup::player,
        "fx_glow",
        &AnkhPowerup::fx_glow,
        "timer1",
        &AnkhPowerup::timer1,
        "timer2",
        &AnkhPowerup::timer2,
        "timer3",
        &AnkhPowerup::timer3,
        "music_on_off",
        &AnkhPowerup::music_on_off,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<YellowCape>(
        "YellowCape",
        sol::base_classes,
        sol::bases<Entity, Movable, Cape>());

    lua.new_usertype<Teleporter>(
        "Teleporter",
        "teleport_number",
        &Teleporter::teleport_number,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<Boomerang>(
        "Boomerang",
        "trail",
        &Boomerang::trail,
        "distance",
        &Boomerang::distance,
        "rotation",
        &Boomerang::rotation,
        "returns_to_uid",
        &Boomerang::returns_to_uid,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<Excalibur>(
        "Excalibur",
        "in_stone",
        &Excalibur::in_stone,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<Shield>(
        "Shield",
        "shake",
        &Shield::shake,
        sol::base_classes,
        sol::bases<Entity, Movable>());
}
} // namespace NEntitiesItems
