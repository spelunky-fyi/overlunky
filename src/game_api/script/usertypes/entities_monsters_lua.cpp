#include "entities_monsters_lua.hpp"

#include <algorithm>   // for max
#include <new>         // for operator new
#include <sol/sol.hpp> // for state, data_t, proxy_key_t, basic_t...
#include <string>      // for operator==, allocator
#include <tuple>       // for get
#include <type_traits> // for move, declval
#include <utility>     // for min, max, end, swap, find_if

#include "entities_monsters.hpp" // for CritterSlime, Hundun, Tiamat, Critt...
#include "entity.hpp"            // for Entity
#include "illumination.hpp"      // IWYU pragma: keep
#include "sound_manager.hpp"     // IWYU pragma: keep

class Movable;
class PowerupCapable;

namespace NEntitiesMonsters
{
void register_usertypes(sol::state& lua)
{
    lua["Entity"]["as_monster"] = &Entity::as<Monster>;

    lua["Entity"]["as_roomowner"] = &Entity::as<RoomOwner>;
    lua["Entity"]["as_walkingmonster"] = &Entity::as<WalkingMonster>;
    lua["Entity"]["as_npc"] = &Entity::as<NPC>;
    lua["Entity"]["as_ghost"] = &Entity::as<Ghost>;
    lua["Entity"]["as_bat"] = &Entity::as<Bat>;
    lua["Entity"]["as_jiangshi"] = &Entity::as<Jiangshi>;
    lua["Entity"]["as_monkey"] = &Entity::as<Monkey>;
    lua["Entity"]["as_goldmonkey"] = &Entity::as<GoldMonkey>;
    lua["Entity"]["as_mole"] = &Entity::as<Mole>;
    lua["Entity"]["as_spider"] = &Entity::as<Spider>;
    lua["Entity"]["as_hangspider"] = &Entity::as<HangSpider>;
    lua["Entity"]["as_shopkeeper"] = &Entity::as<Shopkeeper>;
    lua["Entity"]["as_yang"] = &Entity::as<Yang>;
    lua["Entity"]["as_tun"] = &Entity::as<Tun>;
    lua["Entity"]["as_pet"] = &Entity::as<Pet>;
    lua["Entity"]["as_caveman"] = &Entity::as<Caveman>;
    lua["Entity"]["as_cavemanshopkeeper"] = &Entity::as<CavemanShopkeeper>;
    lua["Entity"]["as_hornedlizard"] = &Entity::as<HornedLizard>;
    lua["Entity"]["as_mosquito"] = &Entity::as<Mosquito>;
    lua["Entity"]["as_mantrap"] = &Entity::as<Mantrap>;
    lua["Entity"]["as_skeleton"] = &Entity::as<Skeleton>;
    lua["Entity"]["as_scarab"] = &Entity::as<Scarab>;
    lua["Entity"]["as_imp"] = &Entity::as<Imp>;
    lua["Entity"]["as_lavamander"] = &Entity::as<Lavamander>;
    lua["Entity"]["as_firebug"] = &Entity::as<Firebug>;
    lua["Entity"]["as_firebugunchained"] = &Entity::as<FirebugUnchained>;
    lua["Entity"]["as_robot"] = &Entity::as<Robot>;
    lua["Entity"]["as_quillback"] = &Entity::as<Quillback>;
    lua["Entity"]["as_leprechaun"] = &Entity::as<Leprechaun>;
    lua["Entity"]["as_crocman"] = &Entity::as<Crocman>;
    lua["Entity"]["as_mummy"] = &Entity::as<Mummy>;
    lua["Entity"]["as_vanhorsing"] = &Entity::as<VanHorsing>;
    lua["Entity"]["as_witchdoctor"] = &Entity::as<WitchDoctor>;
    lua["Entity"]["as_witchdoctorskull"] = &Entity::as<WitchDoctorSkull>;
    lua["Entity"]["as_forestsister"] = &Entity::as<ForestSister>;
    lua["Entity"]["as_vampire"] = &Entity::as<Vampire>;
    lua["Entity"]["as_vlad"] = &Entity::as<Vlad>;
    lua["Entity"]["as_waddler"] = &Entity::as<Waddler>;
    lua["Entity"]["as_octopus"] = &Entity::as<Octopus>;
    lua["Entity"]["as_bodyguard"] = &Entity::as<Bodyguard>;
    lua["Entity"]["as_fish"] = &Entity::as<Fish>;
    lua["Entity"]["as_giantfish"] = &Entity::as<GiantFish>;
    lua["Entity"]["as_crabman"] = &Entity::as<Crabman>;
    lua["Entity"]["as_kingu"] = &Entity::as<Kingu>;
    lua["Entity"]["as_anubis"] = &Entity::as<Anubis>;
    lua["Entity"]["as_cobra"] = &Entity::as<Cobra>;
    lua["Entity"]["as_catmummy"] = &Entity::as<CatMummy>;
    lua["Entity"]["as_sorceress"] = &Entity::as<Sorceress>;
    lua["Entity"]["as_magmaman"] = &Entity::as<MagmaMan>;
    lua["Entity"]["as_bee"] = &Entity::as<Bee>;
    lua["Entity"]["as_ammit"] = &Entity::as<Ammit>;
    lua["Entity"]["as_apeppart"] = &Entity::as<ApepPart>;
    lua["Entity"]["as_apephead"] = &Entity::as<ApepHead>;
    lua["Entity"]["as_osirishead"] = &Entity::as<OsirisHead>;
    lua["Entity"]["as_osirishand"] = &Entity::as<OsirisHand>;
    lua["Entity"]["as_alien"] = &Entity::as<Alien>;
    lua["Entity"]["as_ufo"] = &Entity::as<UFO>;
    lua["Entity"]["as_lahamu"] = &Entity::as<Lahamu>;
    lua["Entity"]["as_yetiqueen"] = &Entity::as<YetiQueen>;
    lua["Entity"]["as_yetiking"] = &Entity::as<YetiKing>;
    lua["Entity"]["as_lamassu"] = &Entity::as<Lamassu>;
    lua["Entity"]["as_olmite"] = &Entity::as<Olmite>;
    lua["Entity"]["as_tiamat"] = &Entity::as<Tiamat>;
    lua["Entity"]["as_giantfrog"] = &Entity::as<GiantFrog>;
    lua["Entity"]["as_frog"] = &Entity::as<Frog>;
    lua["Entity"]["as_firefrog"] = &Entity::as<FireFrog>;
    lua["Entity"]["as_grub"] = &Entity::as<Grub>;
    lua["Entity"]["as_tadpole"] = &Entity::as<Tadpole>;
    lua["Entity"]["as_giantfly"] = &Entity::as<GiantFly>;
    lua["Entity"]["as_ghist"] = &Entity::as<Ghist>;
    lua["Entity"]["as_jumpdog"] = &Entity::as<JumpDog>;
    lua["Entity"]["as_eggplantminister"] = &Entity::as<EggplantMinister>;
    lua["Entity"]["as_yama"] = &Entity::as<Yama>;
    lua["Entity"]["as_hundun"] = &Entity::as<Hundun>;
    lua["Entity"]["as_hundunhead"] = &Entity::as<HundunHead>;
    lua["Entity"]["as_megajellyfish"] = &Entity::as<MegaJellyfish>;
    lua["Entity"]["as_scorpion"] = &Entity::as<Scorpion>;
    lua["Entity"]["as_hermitcrab"] = &Entity::as<Hermitcrab>;
    lua["Entity"]["as_necromancer"] = &Entity::as<Necromancer>;
    lua["Entity"]["as_protoshopkeeper"] = &Entity::as<ProtoShopkeeper>;
    lua["Entity"]["as_beg"] = &Entity::as<Beg>;
    lua["Entity"]["as_terra"] = &Entity::as<Terra>;
    lua["Entity"]["as_critter"] = &Entity::as<Critter>;
    lua["Entity"]["as_critterbeetle"] = &Entity::as<CritterBeetle>;
    lua["Entity"]["as_crittercrab"] = &Entity::as<CritterCrab>;
    lua["Entity"]["as_critterbutterfly"] = &Entity::as<CritterButterfly>;
    lua["Entity"]["as_critterlocust"] = &Entity::as<CritterLocust>;
    lua["Entity"]["as_crittersnail"] = &Entity::as<CritterSnail>;
    lua["Entity"]["as_critterfish"] = &Entity::as<CritterFish>;
    lua["Entity"]["as_critterpenguin"] = &Entity::as<CritterPenguin>;
    lua["Entity"]["as_critterfirefly"] = &Entity::as<CritterFirefly>;
    lua["Entity"]["as_critterdrone"] = &Entity::as<CritterDrone>;
    lua["Entity"]["as_critterslime"] = &Entity::as<CritterSlime>;

    lua.new_usertype<Monster>(
        "Monster",
        "chased_target_uid",
        &Monster::chased_target_uid,
        "target_selection_timer",
        &Monster::target_selection_timer,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable>());

    lua.new_usertype<RoomOwner>(
        "RoomOwner",
        "room_index",
        &RoomOwner::room_index,
        "climb_y_direction",
        &RoomOwner::climb_y_direction,
        "ai_state",
        &RoomOwner::ai_state,
        "patrol_timer",
        &RoomOwner::patrol_timer,
        "lose_interest_timer",
        &RoomOwner::lose_interest_timer,
        "countdown_timer",
        &RoomOwner::countdown_timer,
        "is_patrolling",
        &RoomOwner::is_patrolling,
        "aggro_trigger",
        &RoomOwner::aggro_trigger,
        "was_hurt",
        &RoomOwner::was_hurt,
        "should_attack_on_sight",
        &RoomOwner::should_attack_on_sight,
        "is_angry_flag_set",
        &RoomOwner::is_angry_flag_set,
        "weapon_type",
        &RoomOwner::weapon_type,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<WalkingMonster>(
        "WalkingMonster",
        "chatting_to_uid",
        &WalkingMonster::chatting_to_uid,
        "walk_pause_timer",
        &WalkingMonster::walk_pause_timer,
        "cooldown_timer",
        &WalkingMonster::cooldown_timer,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<NPC>(
        "NPC",
        "climb_direction",
        &NPC::climb_direction,
        "target_in_sight_timer",
        &NPC::target_in_sight_timer,
        "ai_state",
        &NPC::ai_state,
        "aggro",
        &NPC::aggro,
        "should_attack_on_sight",
        &NPC::should_attack_on_sight,
        "weapon_type",
        &NPC::weapon_type,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.create_named_table("GHOST_BEHAVIOR", "SAD", GHOST_BEHAVIOR::SAD, "MEDIUM_SAD", GHOST_BEHAVIOR::MEDIUM_SAD, "MEDIUM_HAPPY", GHOST_BEHAVIOR::MEDIUM_HAPPY, "SMALL_ANGRY", GHOST_BEHAVIOR::SMALL_ANGRY, "SMALL_SURPRISED", GHOST_BEHAVIOR::SMALL_SURPRISED, "SMALL_SAD", GHOST_BEHAVIOR::SMALL_SAD, "SMALL_HAPPY", GHOST_BEHAVIOR::SMALL_HAPPY);

    lua.new_usertype<Ghost>(
        "Ghost",
        "split_timer",
        &Ghost::split_timer,
        "wobble_timer",
        &Ghost::wobble_timer,
        "pace_timer",
        &Ghost::pace_timer,
        "velocity_multiplier",
        &Ghost::velocity_multiplier,
        "ghost_behaviour",
        &Ghost::ghost_behaviour,
        "emitted_light",
        &Ghost::emitted_light,
        "linked_ghost",
        &Ghost::linked_ghost,
        "sound",
        &Ghost::sound,
        "blown_by_player",
        &Ghost::blown_by_player,
        "happy_dancing_clockwise",
        &Ghost::happy_dancing_clockwise,
        "target_dist_visibility_factor",
        &Ghost::target_dist_visibility_factor,
        "target_layer_visibility_factor",
        &Ghost::target_layer_visibility_factor,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<Bat>(
        "Bat",
        "spawn_x",
        &Bat::spawn_x,
        "spawn_y",
        &Bat::spawn_y,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<Jiangshi>(
        "Jiangshi",
        "wait_timer",
        &Jiangshi::wait_timer,
        "jump_counter",
        &Jiangshi::jump_counter,
        "on_ceiling",
        &Jiangshi::on_ceiling,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<Monkey>(
        "Monkey",
        "sound",
        &Monkey::sound,
        "jump_timer",
        &Monkey::jump_timer,
        "on_vine",
        &Monkey::on_vine,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<GoldMonkey>(
        "GoldMonkey",
        "jump_timer",
        &GoldMonkey::jump_timer,
        "poop_timer",
        &GoldMonkey::poop_timer,
        "poop_count",
        &GoldMonkey::poop_count,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<Mole>(
        "Mole",
        "burrowing_sound",
        &Mole::burrowing_sound,
        "nonburrowing_sound",
        &Mole::nonburrowing_sound,
        "burrowing_particle",
        &Mole::burrowing_particle,
        "burrow_dir_x",
        &Mole::burrow_dir_x,
        "burrow_dir_y",
        &Mole::burrow_dir_y,
        "burrowing_in_uid",
        &Mole::burrowing_in_uid,
        "counter_burrowing",
        &Mole::counter_burrowing,
        "counter_nonburrowing",
        &Mole::counter_nonburrowing,
        "countdown_for_appearing",
        &Mole::countdown_for_appearing,
        "digging_state",
        &Mole::digging_state,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<Spider>(
        "Spider",
        "ceiling_pos_x",
        &Spider::ceiling_pos_x,
        "ceiling_pos_y",
        &Spider::ceiling_pos_y,
        "jump_timer",
        &Spider::jump_timer,
        "trigger_distance",
        &Spider::trigger_distance,
        "on_ceiling",
        &Spider::on_ceiling,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<HangSpider>(
        "HangSpider",
        "dangle_jump_timer",
        &HangSpider::dangle_jump_timer,
        "ceiling_pos_x",
        &HangSpider::ceiling_pos_x,
        "ceiling_pos_y",
        &HangSpider::ceiling_pos_y,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<Shopkeeper>(
        "Shopkeeper",
        "name",
        &Shopkeeper::name,
        "shotgun_attack_delay",
        &Shopkeeper::shotgun_attack_delay,
        "has_key",
        &Shopkeeper::has_key,
        "is_ear",
        &Shopkeeper::is_ear,
        "shop_owner",
        &Shopkeeper::shop_owner,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster, RoomOwner>());

    lua.new_usertype<Yang>(
        "Yang",
        "turkeys_in_den",
        &Yang::turkeys_in_den,
        "first_message_shown",
        &Yang::first_message_shown,
        "quest_incomplete",
        &Yang::quest_incomplete,
        "special_message_shown",
        &Yang::special_message_shown,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster, RoomOwner>());

    lua.new_usertype<Tun>(
        "Tun",
        "arrows_left",
        &Tun::arrows_left,
        "reload_timer",
        &Tun::reload_timer,
        "challenge_fee_paid",
        &Tun::challenge_fee_paid,
        "congrats_challenge",
        &Tun::congrats_challenge,
        "murdered",
        &Tun::murdered,
        "shop_entered",
        &Tun::shop_entered,
        "tiamat_encounter",
        &Tun::tiamat_encounter,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster, RoomOwner>());

    lua.new_usertype<Pet>(
        "Pet",
        "fx_button",
        &Pet::fx_button,
        "petting_by_uid",
        &Pet::petting_by_uid,
        "yell_counter",
        &Pet::yell_counter,
        "sit_timer",
        &Pet::sit_timer,
        "sit_cooldown_timer",
        &Pet::sit_cooldown_timer,
        "func_timer",
        &Pet::func_timer,
        "active_state",
        &Pet::active_state,
        "petted_counter",
        &Pet::petted_counter,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<Caveman>(
        "Caveman",
        "wake_up_timer",
        &Caveman::wake_up_timer,
        "can_pick_up_timer",
        &Caveman::can_pick_up_timer,
        "aggro_timer",
        &Caveman::aggro_timer,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster, WalkingMonster>());

    lua.new_usertype<CavemanShopkeeper>(
        "CavemanShopkeeper",
        "tripping",
        &CavemanShopkeeper::tripping,
        "shop_entered",
        &CavemanShopkeeper::shop_entered,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster, WalkingMonster>());

    lua.new_usertype<HornedLizard>(
        "HornedLizard",
        "eaten_uid",
        &HornedLizard::eaten_uid,
        "walk_pause_timer",
        &HornedLizard::walk_pause_timer,
        "attack_cooldown_timer",
        &HornedLizard::attack_cooldown_timer,
        "blood_squirt_timer",
        &HornedLizard::blood_squirt_timer,
        "sound",
        &HornedLizard::sound,
        "particle",
        &HornedLizard::particle,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<Mosquito>(
        "Mosquito",
        "direction_x",
        &Mosquito::direction_x,
        "direction_y",
        &Mosquito::direction_y,
        "stuck_rel_pos_x",
        &Mosquito::stuck_rel_pos_x,
        "stuck_rel_pos_y",
        &Mosquito::stuck_rel_pos_y,
        "sound",
        &Mosquito::sound,
        "timer",
        &Mosquito::timer,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<Mantrap>(
        "Mantrap",
        "walk_pause_timer",
        &Mantrap::walk_pause_timer,
        "eaten_uid",
        &Mantrap::eaten_uid,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<Skeleton>(
        "Skeleton",
        "explosion_timer",
        &Skeleton::explosion_timer,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<Scarab>(
        "Scarab",
        "sound",
        &Scarab::sound,
        "emitted_light",
        &Scarab::emitted_light,
        "timer",
        &Scarab::timer,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<Imp>(
        "Imp",
        "carrying_uid",
        &Imp::carrying_uid,
        "patrol_y_level",
        &Imp::patrol_y_level,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<Lavamander>(
        "Lavamander",
        "emitted_light",
        &Lavamander::emitted_light,
        "shoot_lava_timer",
        &Lavamander::shoot_lava_timer,
        "jump_pause_timer",
        &Lavamander::jump_pause_timer,
        "lava_detection_timer",
        &Lavamander::lava_detection_timer,
        "is_hot",
        &Lavamander::is_hot,
        "player_detect_state",
        &Lavamander::player_detect_state,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<Firebug>(
        "Firebug",
        "sound",
        &Firebug::sound,
        "fire_timer",
        &Firebug::fire_timer,
        "going_up",
        &Firebug::going_up,
        "detached_from_chain",
        &Firebug::detached_from_chain,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<FirebugUnchained>(
        "FirebugUnchained",
        "sound",
        &FirebugUnchained::sound,
        "max_flight_height",
        &FirebugUnchained::max_flight_height,
        "ai_timer",
        &FirebugUnchained::ai_timer,
        "walking_timer",
        &FirebugUnchained::walking_timer,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<Robot>(
        "Robot",
        "sound",
        &Robot::sound,
        "emitted_light_explosion",
        &Robot::emitted_light_explosion,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster, WalkingMonster>());

    lua.new_usertype<Quillback>(
        "Quillback",
        "sound",
        &Quillback::sound,
        "particle",
        &Quillback::particle,
        "broke_block",
        &Quillback::broke_block,
        "post_hit_wall_direction",
        &Quillback::post_hit_wall_direction,
        "seen_player",
        &Quillback::seen_player,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster, WalkingMonster>());

    lua.new_usertype<Leprechaun>(
        "Leprechaun",
        "sound",
        &Leprechaun::sound,
        "hump_timer",
        &Leprechaun::hump_timer,
        "target_in_sight_timer",
        &Leprechaun::target_in_sight_timer,
        "gold",
        &Leprechaun::gold,
        "timer_after_humping",
        &Leprechaun::timer_after_humping,
        "jump_trigger",
        &Leprechaun::jump_trigger,
        "collected_treasure",
        &Leprechaun::collected_treasure,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster, WalkingMonster>());

    lua.new_usertype<Crocman>(
        "Crocman",
        "teleport_cooldown",
        &Crocman::teleport_cooldown,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster, WalkingMonster>());

    lua.new_usertype<Mummy>(
        "Mummy",
        "walk_pause_timer",
        &Mummy::walk_pause_timer,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<VanHorsing>(
        "VanHorsing",
        "show_text",
        &VanHorsing::show_text,
        "special_message_shown",
        &VanHorsing::special_message_shown,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster, NPC>());

    lua.new_usertype<WitchDoctor>(
        "WitchDoctor",
        "sound",
        &WitchDoctor::sound,
        "skull_regen_timer",
        &WitchDoctor::skull_regen_timer,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster, WalkingMonster>());

    lua.new_usertype<WitchDoctorSkull>(
        "WitchDoctorSkull",
        "witch_doctor_uid",
        &WitchDoctorSkull::witch_doctor_uid,
        "emitted_light",
        &WitchDoctorSkull::emitted_light,
        "sound",
        &WitchDoctorSkull::sound,
        "rotation_angle",
        &WitchDoctorSkull::rotation_angle,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<ForestSister>(
        "ForestSister",
        "walk_pause_timer",
        &ForestSister::walk_pause_timer,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster, NPC>());

    lua.new_usertype<Vampire>(
        "Vampire",
        "jump_trigger_distance_x",
        &Vampire::jump_trigger_distance_x,
        "jump_trigger_distance_y",
        &Vampire::jump_trigger_distance_y,
        "sleep_pos_x",
        &Vampire::sleep_pos_x,
        "sleep_pos_y",
        &Vampire::sleep_pos_y,
        "walk_pause_timer",
        &Vampire::walk_pause_timer,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<Vlad>(
        "Vlad",
        "teleport_timer",
        &Vlad::teleport_timer,
        "aggro",
        &Vlad::aggro,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster, Vampire>());

    lua.new_usertype<Waddler>(
        "Waddler",
        "player_detected",
        &Waddler::player_detected,
        "on_the_ground",
        &Waddler::on_the_ground,
        "air_timer",
        &Waddler::air_timer,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster, RoomOwner>());

    lua.new_usertype<Octopus>(
        "Octopus",
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster, WalkingMonster>());

    lua.new_usertype<Bodyguard>(
        "Bodyguard",
        "position_state",
        &Bodyguard::position_state,
        "message_shown",
        &Bodyguard::message_shown,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster, NPC>());

    lua.new_usertype<Fish>(
        "Fish",
        "change_direction_timer",
        &Fish::change_direction_timer,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<GiantFish>(
        "GiantFish",
        "sound",
        &GiantFish::sound,
        "change_direction_timer",
        &GiantFish::change_direction_timer,
        "lose_interest_timer",
        &GiantFish::lose_interest_timer,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<Crabman>(
        "Crabman",
        "walk_pause_timer",
        &Crabman::walk_pause_timer,
        "invincibility_timer",
        &Crabman::invincibility_timer,
        "poison_attack_timer",
        &Crabman::poison_attack_timer,
        "attacking_claw_uid",
        &Crabman::attacking_claw_uid,
        "at_maximum_attack",
        &Crabman::at_maximum_attack,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<Kingu>(
        "Kingu",
        "sound1",
        &Kingu::sound1,
        "sound2",
        &Kingu::sound2,
        "climb_direction_x",
        &Kingu::climb_direction_x,
        "climb_direction_y",
        &Kingu::climb_direction_y,
        "climb_pause_timer",
        &Kingu::climb_pause_timer,
        "shell_invincibility_timer",
        &Kingu::shell_invincibility_timer,
        "monster_spawn_timer",
        &Kingu::monster_spawn_timer,
        "initial_shell_health",
        &Kingu::initial_shell_health,
        "player_seen_by_kingu",
        &Kingu::player_seen_by_kingu,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<Anubis>(
        "Anubis",
        "spawn_x",
        &Anubis::spawn_x,
        "spawn_y",
        &Anubis::spawn_y,
        "attack_proximity_y",
        &Anubis::attack_proximity_y,
        "attack_proximity_x",
        &Anubis::attack_proximity_x,
        "ai_timer",
        &Anubis::ai_timer,
        "next_attack_timer",
        &Anubis::next_attack_timer,
        "psychic_orbs_counter",
        &Anubis::psychic_orbs_counter,
        "awake",
        &Anubis::awake,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<Cobra>(
        "Cobra",
        "spit_timer",
        &Cobra::spit_timer,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<CatMummy>(
        "CatMummy",
        "ai_state",
        &CatMummy::jump_height_multiplier,
        "jump_height_multiplier",
        &CatMummy::jump_height_multiplier,
        "attack_timer",
        &CatMummy::attack_timer,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<Sorceress>(
        "Sorceress",
        "inbetween_attack_timer",
        &Sorceress::inbetween_attack_timer,
        "in_air_timer",
        &Sorceress::in_air_timer,
        "halo_emitted_light",
        &Sorceress::halo_emitted_light,
        "fx_entity",
        &Sorceress::fx_entity,
        "sound",
        &Sorceress::sound,
        "hover_timer",
        &Sorceress::hover_timer,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster, WalkingMonster>());

    lua.new_usertype<MagmaMan>(
        "MagmaMan",
        "emitted_light",
        &MagmaMan::emitted_light,
        "sound",
        &MagmaMan::sound,
        "particle",
        &MagmaMan::particle,
        "jump_timer",
        &MagmaMan::jump_timer,
        "alive_timer",
        &MagmaMan::alive_timer,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<Bee>(
        "Bee",
        "can_rest",
        &Bee::can_rest,
        "sound",
        &Bee::sound,
        "fly_hang_timer",
        &Bee::fly_hang_timer,
        "targeting_timer",
        &Bee::targeting_timer,
        "walk_start_time",
        &Bee::walk_start_time,
        "walk_end_time",
        &Bee::walk_end_time,
        "wobble_x",
        &Bee::wobble_x,
        "wobble_y",
        &Bee::wobble_y,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<Ammit>(
        "Ammit",
        "walk_pause_timer",
        &Ammit::walk_pause_timer,
        "particle",
        &Ammit::particle,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<ApepPart>(
        "ApepPart",
        "y_pos",
        &ApepPart::y_pos,
        "sine_angle",
        &ApepPart::sine_angle,
        "sync_timer",
        &ApepPart::sync_timer,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<ApepHead>(
        "ApepHead",
        "sound1",
        &ApepHead::sound1,
        "sound2",
        &ApepHead::sound2,
        "distance_traveled",
        &ApepHead::distance_traveled,
        "tail_uid",
        &ApepHead::tail_uid,
        "fx_mouthpiece1_uid",
        &ApepHead::fx_mouthpiece1_uid,
        "fx_mouthpiece2_uid",
        &ApepHead::fx_mouthpiece2_uid,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster, ApepPart>());

    lua.new_usertype<OsirisHead>(
        "OsirisHead",
        "right_hand_uid",
        &OsirisHead::right_hand_uid,
        "left_hand_uid",
        &OsirisHead::left_hand_uid,
        "moving_left",
        &OsirisHead::moving_left,
        "targeting_timer",
        &OsirisHead::oscillation_phase,
        "oscillation_phase",
        &OsirisHead::oscillation_phase,
        "invincibility_timer",
        &OsirisHead::invincibility_timer,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<OsirisHand>(
        "OsirisHand",
        "attack_cooldown_timer",
        &OsirisHand::attack_cooldown_timer,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<Alien>(
        "Alien",
        "jump_timer",
        &Alien::jump_timer,
        "shudder_timer",
        &Alien::shudder_timer,
        "leg_shake_timer",
        &Alien::leg_shake_timer,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<UFO>(
        "UFO",
        "sound",
        &UFO::sound,
        "patrol_distance",
        &UFO::patrol_distance,
        "attack_cooldown_timer",
        &UFO::attack_cooldown_timer,
        "is_falling",
        &UFO::is_rising,
        "is_rising",
        &UFO::is_rising,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<Lahamu>(
        "Lahamu",
        "sound",
        &Lahamu::sound,
        "eyeball",
        &Lahamu::eyeball,
        "attack_cooldown_timer",
        &Lahamu::attack_cooldown_timer,
        "has_logged_to_journal",
        &Lahamu::has_logged_to_journal,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<YetiQueen>(
        "YetiQueen",
        "walk_pause_timer",
        &YetiQueen::walk_pause_timer,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<YetiKing>(
        "YetiKing",
        "walk_pause_timer",
        &YetiKing::walk_pause_timer,
        "emitted_light",
        &YetiKing::emitted_light,
        "particle_fog",
        &YetiKing::particle_fog,
        "particle_dust",
        &YetiKing::particle_dust,
        "particle_sparkles",
        &YetiKing::particle_sparkles,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<Lamassu>(
        "Lamassu",
        "sound",
        &Lamassu::sound,
        "attack_effect_entity",
        &Lamassu::attack_effect_entity,
        "particle",
        &Lamassu::particle,
        "emitted_light",
        &Lamassu::emitted_light,
        "walk_pause_timer",
        &Lamassu::walk_pause_timer,
        "flight_timer",
        &Lamassu::flight_timer,
        "attack_timer",
        &Lamassu::attack_timer,
        "attack_angle",
        &Lamassu::attack_angle,
        "was_flying",
        &Lamassu::was_flying,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<Olmite>(
        "Olmite",
        "armor_on",
        &Olmite::armor_on,
        "in_stack",
        &Olmite::in_stack,
        "in_stack2",
        &Olmite::in_stack2,
        "on_top_uid",
        &Olmite::on_top_uid,
        "y_offset",
        &Olmite::y_offset,
        "attack_cooldown_timer",
        &Olmite::attack_cooldown_timer,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster, WalkingMonster>());

    lua.new_usertype<Tiamat>(
        "Tiamat",
        "sound",
        &Tiamat::sound,
        "fx_tiamat_head",
        &Tiamat::fx_tiamat_head_uid,
        "fx_tiamat_arm_right1",
        &Tiamat::fx_tiamat_arm_right1_uid,
        "fx_tiamat_arm_right2",
        &Tiamat::fx_tiamat_arm_right2_uid,
        "frown_timer",
        &Tiamat::invincibility_timer,
        "damage_timer",
        &Tiamat::damage_timer,
        "attack_timer",
        &Tiamat::attack_timer,
        "tail_angle",
        &Tiamat::tail_angle,
        "tail_radian",
        &Tiamat::tail_radian,
        "tail_move_speed",
        &Tiamat::tail_move_speed,
        "right_arm_angle",
        &Tiamat::right_arm_angle,
        "attack_x",
        &Tiamat::attack_x,
        "attack_y",
        &Tiamat::attack_y,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<GiantFrog>(
        "GiantFrog",
        "door_front_layer",
        &GiantFrog::door_front_layer,
        "door_back_layer",
        &GiantFrog::door_back_layer,
        "platform",
        &GiantFrog::platform,
        "attack_timer",
        &GiantFrog::attack_timer,
        "frogs_ejected_in_cycle",
        &GiantFrog::frogs_ejected_in_cycle,
        "invincibility_timer",
        &GiantFrog::invincibility_timer,
        "mouth_close_timer",
        &GiantFrog::mouth_close_timer,
        "mouth_open_trigger",
        &GiantFrog::mouth_open_trigger,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<Frog>(
        "Frog",
        "grub_being_eaten_uid",
        &Frog::grub_being_eaten_uid,
        "jump_timer",
        &Frog::jump_timer,
        "pause",
        &Frog::pause,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<FireFrog>(
        "FireFrog",
        "sound",
        &FireFrog::sound,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster, Frog>());

    lua.new_usertype<Grub>(
        "Grub",
        "rotation_delta",
        &Grub::rotation_delta,
        "drop",
        &Grub::drop,
        "rotation_direction",
        &Grub::rotation_direction,
        "looking_for_new_direction_timer",
        &Grub::wall_collision_cooldown,
        "wall_collision_cooldown",
        &Grub::wall_collision_cooldown,
        "rotation_timer",
        &Grub::rotation_timer,
        "walk_pause_timer",
        &Grub::walk_pause_timer,
        "turn_into_fly_timer",
        &Grub::turn_into_fly_timer,
        "particle",
        &Grub::particle,
        "sound",
        &Grub::sound,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<Tadpole>(
        "Tadpole",
        "acceleration_timer",
        &Tadpole::acceleration_timer,
        "player_spotted",
        &Tadpole::player_spotted,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<GiantFly>(
        "GiantFly",
        "head_entity",
        &GiantFly::head_entity,
        "sound",
        &GiantFly::sound,
        "particle",
        &GiantFly::particle,
        "sine_amplitude",
        &GiantFly::sine_amplitude,
        "sine_frequency",
        &GiantFly::sine_frequency,
        "delta_y_angle",
        &GiantFly::delta_y_angle,
        "sine_counter",
        &GiantFly::sine_counter,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<Ghist>(
        "Ghist",
        "body_uid",
        &Ghist::body_uid,
        "idle_timer",
        &Ghist::idle_timer,
        "sound",
        &Ghist::sound,
        "transparency",
        &Ghist::transparency,
        "fadeout",
        &Ghist::fadeout,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<JumpDog>(
        "JumpDog",
        "walk_pause_timer",
        &JumpDog::walk_pause_timer,
        "squish_timer",
        &JumpDog::squish_timer,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<EggplantMinister>(
        "EggplantMinister",
        "sound",
        &EggplantMinister::sound,
        "walk_pause_timer",
        &EggplantMinister::walk_pause_timer,
        "squish_timer",
        &EggplantMinister::squish_timer,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<Yama>(
        "Yama",
        "message_shown",
        &Yama::message_shown,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<Hundun>(
        "Hundun",
        "applied_hor_velocity",
        &Hundun::applied_hor_velocity,
        "applied_ver_velocity",
        &Hundun::applied_ver_velocity,
        "birdhead_entity_uid",
        &Hundun::birdhead_entity_uid,
        "snakehead_entity_uid",
        &Hundun::snakehead_entity_uid,
        "y_level",
        &Hundun::y_level,
        "bounce_timer",
        &Hundun::bounce_timer,
        "fireball_timer",
        &Hundun::fireball_timer,
        "birdhead_defeated",
        &Hundun::birdhead_defeated,
        "snakehead_defeated",
        &Hundun::snakehead_defeated,
        "hundun_flags",
        &Hundun::hundun_flags,
        "y_limit",
        &Hundun::y_limit,
        "rising_speed_x",
        &Hundun::rising_speed_x,
        "rising_speed_y",
        &Hundun::rising_speed_y,
        "bird_head_spawn_y",
        &Hundun::bird_head_spawn_y,
        "snake_head_spawn_y",
        &Hundun::snake_head_spawn_y,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.create_named_table("HUNDUNFLAGS", "WILLMOVELEFT", 1, "BIRDHEADEMERGED", 2, "SNAKEHEADEMERGED", 4, "TOPLEVELARENAREACHED", 8, "BIRDHEADSHOTLAST", 16);

    lua.new_usertype<HundunHead>(
        "HundunHead",
        "attack_position_x",
        &HundunHead::attack_position_x,
        "attack_position_y",
        &HundunHead::attack_position_y,
        "egg_crack_effect_uid",
        &HundunHead::egg_crack_effect_uid,
        "targeted_player_uid",
        &HundunHead::targeted_player_uid,
        "looking_for_target_timer",
        &HundunHead::looking_for_target_timer,
        "invincibility_timer",
        &HundunHead::invincibility_timer,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<MegaJellyfish>(
        "MegaJellyfish",
        "flipper1",
        &MegaJellyfish::flipper1,
        "flipper2",
        &MegaJellyfish::flipper2,
        "sound",
        &MegaJellyfish::sound,
        "orb_uid",
        &MegaJellyfish::orb_uid,
        "tail_bg_uid",
        &MegaJellyfish::tail_bg_uid,
        "applied_velocity",
        &MegaJellyfish::applied_velocity,
        "wagging_tail_counter",
        &MegaJellyfish::wagging_tail_counter,
        "flipper_distance",
        &MegaJellyfish::flipper_distance,
        "velocity_application_timer",
        &MegaJellyfish::velocity_application_timer,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<Scorpion>(
        "Scorpion",
        "walk_pause_timer",
        &Scorpion::walk_pause_timer,
        "jump_cooldown_timer",
        &Scorpion::jump_cooldown_timer,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<Hermitcrab>(
        "Hermitcrab",
        "carried_entity_type",
        &Hermitcrab::carried_entity_type,
        "carried_entity_uid",
        &Hermitcrab::carried_entity_uid,
        "walk_spit_timer",
        &Hermitcrab::walk_spit_timer,
        "is_active",
        &Hermitcrab::is_active,
        "is_inactive",
        &Hermitcrab::is_inactive,
        "spawn_new_carried_item",
        &Hermitcrab::spawn_new_carried_item,
        "going_up",
        &Hermitcrab::going_up,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<Necromancer>(
        "Necromancer",
        "sound",
        &Necromancer::sound,
        "red_skeleton_spawn_x",
        &Necromancer::red_skeleton_spawn_x,
        "red_skeleton_spawn_y",
        &Necromancer::red_skeleton_spawn_y,
        "resurrection_uid",
        &Necromancer::resurrection_uid,
        "target_layer",
        &Necromancer::target_layer,
        "resurrection_timer",
        &Necromancer::resurrection_timer,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster, WalkingMonster>());

    lua.new_usertype<ProtoShopkeeper>(
        "ProtoShopkeeper",
        "movement_state",
        &ProtoShopkeeper::movement_state,
        "walk_pause_explode_timer",
        &ProtoShopkeeper::walk_pause_explode_timer,
        "walking_speed",
        &ProtoShopkeeper::walking_speed,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<Beg>(
        "Beg",
        "walk_pause_timer",
        &Beg::walk_pause_timer,
        "disappear_timer",
        &Beg::disappear_timer,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster, NPC>());

    lua.new_usertype<Terra>(
        "Terra",
        "fx_button",
        &Terra::fx_button,
        "x_pos",
        &Terra::x_pos,
        "abuse_speechbubble_timer",
        &Terra::abuse_speechbubble_timer,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<Critter>(
        "Critter",
        "last_picked_up_by_uid",
        &Critter::last_picked_up_by_uid,
        "holding_state",
        &Critter::holding_state,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster>());

    lua.new_usertype<CritterBeetle>(
        "CritterBeetle",
        "pause",
        &CritterBeetle::pause,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster, Critter>());

    lua.new_usertype<CritterCrab>(
        "CritterCrab",
        "walk_pause_timer",
        &CritterCrab::walk_pause_timer,
        "walking_left",
        &CritterCrab::walking_left,
        "unfriendly",
        &CritterCrab::unfriendly,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster, Critter>());

    lua.new_usertype<CritterButterfly>(
        "CritterButterfly",
        "change_direction_timer",
        &CritterButterfly::change_direction_timer,
        "vertical_flight_direction",
        &CritterButterfly::vertical_flight_direction,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster, Critter>());

    lua.new_usertype<CritterLocust>(
        "CritterLocust",
        "sound",
        &CritterLocust::sound,
        "jump_timer",
        &CritterLocust::jump_timer,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster, Critter>());

    lua.new_usertype<CritterSnail>(
        "CritterSnail",
        "x_direction",
        &CritterSnail::x_direction,
        "y_direction",
        &CritterSnail::y_direction,
        "pos_x",
        &CritterSnail::pos_x,
        "pos_y",
        &CritterSnail::pos_y,
        "rotation_center_x",
        &CritterSnail::rotation_center_x,
        "rotation_center_y",
        &CritterSnail::rotation_center_y,
        "rotation_angle",
        &CritterSnail::rotation_angle,
        "rotation_speed",
        &CritterSnail::rotation_speed,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster, Critter>());

    lua.new_usertype<CritterFish>(
        "CritterFish",
        "swim_pause_timer",
        &CritterFish::swim_pause_timer,
        "player_in_proximity",
        &CritterFish::player_in_proximity,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster, Critter>());

    lua.new_usertype<CritterPenguin>(
        "CritterPenguin",
        "walk_pause_timer",
        &CritterPenguin::walk_pause_timer,
        "jump_timer",
        &CritterPenguin::jump_timer,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster, Critter>());

    lua.new_usertype<CritterFirefly>(
        "CritterFirefly",
        "sine_amplitude",
        &CritterFirefly::sine_amplitude,
        "sine_frequency",
        &CritterFirefly::sine_frequency,
        "sine_angle",
        &CritterFirefly::sine_angle,
        "change_direction_timer",
        &CritterFirefly::change_direction_timer,
        "sit_timer",
        &CritterFirefly::sit_timer,
        "sit_cooldown_timer",
        &CritterFirefly::sit_cooldown_timer,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster, Critter>());

    lua.new_usertype<CritterDrone>(
        "CritterDrone",
        "emitted_light",
        &CritterDrone::emitted_light,
        "sound",
        &CritterDrone::sound,
        "applied_hor_momentum",
        &CritterDrone::applied_hor_momentum,
        "applied_ver_momentum",
        &CritterDrone::applied_ver_momentum,
        "unfriendly",
        &CritterDrone::unfriendly,
        "move_timer",
        &CritterDrone::move_timer,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster, Critter>());

    lua.new_usertype<CritterSlime>(
        "CritterSlime",
        "x_direction",
        &CritterSlime::x_direction,
        "y_direction",
        &CritterSlime::y_direction,
        "pos_x",
        &CritterSlime::pos_x,
        "pos_y",
        &CritterSlime::pos_y,
        "rotation_center_x",
        &CritterSlime::rotation_center_x,
        "rotation_center_y",
        &CritterSlime::rotation_center_y,
        "rotation_angle",
        &CritterSlime::rotation_angle,
        "rotation_speed",
        &CritterSlime::rotation_speed,
        "walk_pause_timer",
        &CritterSlime::walk_pause_timer,
        sol::base_classes,
        sol::bases<Entity, Movable, PowerupCapable, Monster, Critter>());
}
} // namespace NEntitiesMonsters
