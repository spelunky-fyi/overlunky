#pragma once

#include "movable.hpp"
#include "particles.hpp"
#include <cstdint>

class Monster : public Movable
{
  public:
    std::map<int64_t, int64_t> inside;
    int32_t chased_target_uid;
    uint32_t target_selection_timer;
};

class Ghost : public Monster
{
  public:
    uint16_t split_timer; // for SMALL_HAPPY this is also the sequence timer of its various states
    uint8_t unknown1;
    uint8_t unknown2;
    float velocity_multiplier;
    uint8_t unknown3;
    uint8_t unknown4;
    uint8_t ghost_behaviour; // 0 = SMALL_ANGRY; 1 = SMALL_SURPRISED; 2 = SMALL_SAD; 3 = SMALL_HAPPY
    uint8_t unknown6;
    bool unknown7;
    uint8_t unknown8;
    uint8_t unknown9;
    uint8_t unknown10;
    Illumination* emitted_light;
    Entity* linked_ghost;
    SoundPosition* sound_info;
};

class Jiangshi : public Monster
{
  public:
    uint8_t wait_timer; // wait time between jumps
};

class Monkey : public Monster
{
  public:
    SoundPosition* sound_pos;
    uint8_t jump_timer;
    bool on_vine;
};

class GoldMonkey : public Monster
{
  public:
    uint16_t jump_timer;
    uint16_t poop_timer;
    uint8_t poop_count;
};

class Mole : public Monster
{
  public:
    SoundPosition* burrowing_sound_pos;
    SoundPosition* nonburrowing_sound_pos;
    ParticleEmitterInfo* burrowing_particle;
    float burrow_dir_x;
    float burrow_dir_y;
    int32_t burrowing_in;
    uint8_t counter_burrowing;
    uint8_t counter_nonburrowing;
    uint8_t countdown_for_appearing;
    uint8_t unknown_two_while_burrowed;
};

class Spider : public Monster
{
  public:
    float ceiling_pos_x;
    float ceiling_pos_y;
    uint8_t jump_timer;
};

class HangSpider : public Monster
{
  public:
    int16_t dangle_jump_timer;
    uint16_t padding;
    float ceiling_pos_x;
    float ceiling_pos_y;
};

class Shopkeeper : public Monster
{
  public:
    int32_t related_entity_uid; // for ghist shopkeeper, the UID of the dead caveman
    float climb_y_direction;    // when on a rope or ladder
    uint8_t ai_state;
    uint8_t patrol_timer;
    uint16_t lose_interest_timer; // delay in-between attacks
    uint8_t countdown_timer;      // counts down to 0 when a value is forced
    uint8_t unknown5;
    uint8_t unknown6;
    bool is_patrolling; // to set off a shopkeeper, combine this with state.shoppie_aggro_levels > 0
};

class GhistShopkeeper : public Shopkeeper
{
  public:
    uint8_t movement_related_timer_1;
    uint8_t movement_related_timer_2;
    int16_t transparency;
    uint16_t fadeout; // when 0, ghist fades out/dies
};

class Yang : public Shopkeeper
{
  public:
    uint64_t unknown1;
    size_t unknown2;
    uint8_t turkeys_in_pen;
    uint8_t padding1;
    uint8_t padding2;
    uint8_t padding3;
    uint32_t unknown3;
    uint8_t unknown4;
    uint8_t unknown5;
    bool first_message_shown; // I'm looking for turkeys, wanna help?
    bool quest_incomplete;    // gets set to false when the quest is over (Yang dead or second turkey delivered)
    bool tidepool_message_shown;
    uint8_t padding4;
    uint8_t padding5;
    uint8_t padding6;
};

class Tun : public Shopkeeper
{
  public:
    uint32_t unknown1;
    uint32_t unknown2;
    uint8_t arrows_left;
    uint8_t reload_timer;    // when 0, a new arrow is loaded into the bow; resets when she finds an arrow on the ground
    bool challenge_fee_paid; // can't reset, you can only pay once
    bool congrats_challenge; // congrats message shown after exiting a challenge
    bool murdered;           // when she disappears in a cloud of smoke
    bool shop_entered;
    bool tiamat_encounter;
    uint8_t padding;
};

class Pet : public Monster
{
  public:
    uint64_t unknown1;
    int32_t unknown2;
    int16_t yell_counter; // counts up to 400 (6.6 sec); when 0 the pet yells out
    uint16_t padding1;
    uint32_t unknown3;
    int8_t active; // -1 = sitting and yelling; 0 = either running, dead or picked up
};

class Caveman : public Monster
{
  public:
    int32_t chatting_to_uid;
    int16_t walk_pause_timer; // alternates between walking and pausing every time it reaches zero
    int16_t chatting_timer;   // when greater than 0 it counts down during chatting; when smaller than 0 no chatting will occur until back to 0 ; 0 = ready to chat
    uint8_t wake_up_timer;
    uint8_t can_pick_up_timer;   // 0 = can pick something up; when holding forced to 179; after tripping and regaining consciousness counts down to 0
    int16_t unknown_aggro_timer; // 1 sec timer at the start of getting aggro
    int16_t aggro_timer;         // keeps resetting when angry and a player is nearby
};

class CavemanShopkeeper : public Monster
{
  public:
    int32_t unknown1;
    uint16_t timer1;
    uint8_t timer2;
    int8_t unknown2;
    uint8_t unknown3;
    uint8_t timer3;
    uint8_t timer4;
    bool tripping;
    int16_t timer5;
    int16_t padding;
    bool shop_entered;
};

class HornedLizard : public Monster
{
  public:
    int32_t dungbeetle_being_eaten;
    uint8_t walk_pause_timer;      // alternates between walking and pausing when timer reaches zero
    uint8_t attack_cooldown_timer; // won't attack until timer reaches zero
    uint8_t blood_squirt_timer;
    uint8_t unknown3;
    SoundPosition* sound_pos;
    ParticleEmitterInfo* particle;
};

class Mosquito : public Monster
{
  public:
    float direction_x;
    float direction_y;
    float stuck_rel_pos_x; // position relative to the block the mosquito is stuck in
    float stuck_rel_pos_y;
    SoundPosition* sound_pos;
    uint8_t timer;
};

class Mantrap : public Monster
{
  public:
    int16_t walk_pause_timer; // alternates between walking and pausing every time it reaches zero
    uint16_t padding;
    int32_t eaten_uid; // the uid of the entity the mantrap has eaten, in case it can break out, like a shopkeeper
};

class Skeleton : public Monster
{
  public:
    int16_t explosion_timer; // -1 never explodes
};

class Scarab : public Monster
{
  public:
    SoundPosition* sound_pos;
    Illumination* emitted_light; // set color in sat_vign_0, be sure to make the aperture large enough
    uint8_t unknown;             // changes when the scarab is pausing
};

class Imp : public Monster
{
  public:
    int32_t carrying;
    float patrol_y_level;
};

class Lavamander : public Monster
{
  public:
    Illumination* emitted_light;
    uint8_t shoot_lava_timer; // when this timer reaches zero, it appears on the surface/shoots lava; triggers on player proximity
    uint8_t padding1;
    uint8_t jump_pause_timer; // jump pause when cool; runs continuously when hot
    uint8_t padding2;
    uint8_t lava_detection_timer;
    bool is_hot;
    uint8_t unknown1;
    uint8_t padding3;
};

class Firebug : public Monster
{
  public:
    SoundPosition* sound_pos;
    uint16_t fire_timer; // when it reaches zero, it shoots fire
    bool going_up;
    bool detached_from_chain;
};

class FirebugUnchained : public Monster
{
  public:
    SoundPosition* sound_pos;
    float max_flight_height; // increments when flying up, until it reaches 1.0, then bug flies down
    uint8_t ai_timer;        // when reaches zero, decides whether to walk or fly or pause; doesn't reset when flying
    uint8_t padding1;
    uint8_t padding2;
    uint8_t padding3;
    uint16_t walking_timer; // counts down only when walking; can't start flying unless this is at zero
};

class Robot : public Monster
{
  public:
    int32_t unknown1;
    uint8_t walk_pause_timer; // alternates between walking and pausing when timer reaches zero
    uint8_t padding1;
    int16_t targeting_timer; // When a target is spotted, set to -600 (10 sec), then, when target can't be seen, counts down, but no behavioural change is seen after 10 sec?
    SoundPosition* sound_pos;
    Illumination* emitted_light_explosion;
};

class Quillback : public Monster
{
  public:
    int32_t unknown1;
    uint8_t walk_pause_timer; // alternates between walking and pausing when timer reaches zero
    uint8_t padding1;
    int16_t unknown2;
    SoundPosition* sound_pos;
    ParticleEmitterInfo* particle;
};

class Leprechaun : public Monster
{
  public:
    int32_t unknown1;
    uint8_t walk_pause_timer; // alternates between walking and pausing when timer reaches zero
    uint8_t unknown2;
    int16_t unknown3;
    SoundPosition* sound_pos;
    uint8_t hump_timer;
    uint8_t target_in_sight_timer;
    uint16_t teleport_related; // changes when teleporting
    uint8_t timer_after_humping;
};

class Crocman : public Monster
{
  public:
    int32_t unknown1;
    uint8_t walk_pause_timer; // alternates between walking and pausing when timer reaches zero
    uint8_t unknown2;
    int16_t unknown_timer; // counts for 10 sec, after stun/triggering?
};

class Mummy : public Monster
{
  public:
    uint8_t walk_pause_timer; // alternates between walking and pausing when timer reaches zero
};

class VanHorsing : public Monster
{
  public:
    float climb_direction;
    uint8_t unknown1;
    uint8_t unknown2;
    uint8_t unknown3;
    uint8_t target_spotted_timer;
    uint8_t attack_state;
    bool aggro;
};

class WitchDoctor : public Monster
{
  public:
    int32_t unknown1;
    uint8_t walk_pause_timer; // alternates between walking and pausing when timer reaches zero
    uint8_t padding1;
    int16_t attack_timer; // 10 sec timer starts counting after attack; won't start again until attack cycle is broken
    SoundPosition* sound_pos;
    uint16_t skull_regen_timer; // when zero, regenerates a new floating skull
};

class WitchDoctorSkull : public Monster
{
  public:
    int32_t witch_doctor_uid;
    uint32_t unknown1;
    Illumination* emitted_light;
    SoundPosition* sound_pos;
    float rotation_angle;
};

class ForestSister : public Monster
{
  public:
    uint32_t unknown1;
    uint8_t unknown2;
    uint8_t unknown3;
    uint8_t unknown_timer;
    uint8_t target_in_sight_timer;
    uint8_t ai_state;
    bool aggro;
    uint8_t padding2;
    uint8_t padding3;
    uint32_t unknown4;
    uint8_t walk_pause_timer; // alternates between walking and pausing when timer reaches zero
};

class Vampire : public Monster
{
  public:
    float jump_trigger_distance_x; // the distance from the player it starts jumping
    float jump_trigger_distance_y;
    float sleep_pos_x;
    float sleep_pos_y;
    uint8_t walk_pause_timer; // alternates between walking and pausing when timer reaches zero
    uint8_t padding1;
    uint8_t padding2;
    uint8_t padding3;
};

class Vlad : public Vampire
{
  public:
    uint32_t unknown1;
    uint8_t teleport_timer; // triggers when Vlad teleports, when timer running he can't teleport and will stun when hit
    bool aggro;
};

class Waddler : public Monster
{
  public:
    uint8_t unknown1;
    uint8_t padding1;
    uint8_t padding2;
    uint8_t padding3;
    uint32_t unknown2;
    uint8_t unknown3;
    uint8_t unknown4;
    uint8_t lose_interest_timer; // delay in-between attacks
    uint8_t unknown5;
    uint8_t unknown6;
    uint8_t unknown7;
    uint8_t unknown8;
    uint8_t unknown9;
    uint64_t unknown10;
    uint8_t unknown11;
    bool on_the_ground;
    uint16_t aggro_timer;
};

class Octopus : public Monster
{
  public:
    int32_t unknown1;
    uint8_t walk_pause_timer; // alternates between walking and pausing when timer reaches zero
};

class Bodyguard : public Monster
{
  public:
    float climb_y_direction; // when on a rope or ladder
    uint8_t unknown1;
    uint8_t unknown2;
    uint8_t unknown3;
    uint8_t target_detection_timer;
    uint8_t ai_state;
    bool aggro;
    uint8_t unknown7;
    uint8_t unknown8;
};

class MadameTusk : public Monster
{
  public:
    uint8_t unknown1;
    uint8_t padding1;
    uint8_t padding2;
    uint8_t padding3;
    uint32_t unknown2;
    uint8_t ai_state;
    uint8_t unknown3;
    uint8_t target_timer;
    uint8_t padding4;
    uint8_t unknown4;
    uint8_t unknown5;
    uint8_t unknown6;
    uint8_t unknown7;
};

class Fish : public Monster
{
  public:
    uint8_t change_direction_timer;
};

class GiantFish : public Monster
{
  public:
    SoundPosition* sound_pos;
    uint16_t change_direction_timer; // when bouncing into a wall, it changes direction too
    uint16_t lose_interest_timer;    // delay in-between attacks
    uint16_t unknown_timer;
    uint16_t padding1;
    float unknown1;
    float unknown2;
    float unknown3;
};

class Crabman : public Monster
{
  public:
    uint8_t walk_pause_timer; // alternates between walking and pausing when timer reaches zero
    uint8_t padding1;
    uint8_t padding2;
    uint8_t padding3;
    uint8_t invincibility_timer; // triggers after being attacked
    uint8_t poison_attack_timer;
    uint8_t padding4;
    uint8_t padding5;
    int32_t attacking_claw;
    bool at_maximum_attack;
    uint8_t unknown3;
};

class Kingu : public Monster
{
  public:
    uint64_t unknown1;
    uint64_t unknown2;
    float climb_direction_x; // distance too?
    float climb_direction_y;
    uint16_t climb_pause_timer;
    uint8_t shell_invincibility_timer;
    uint8_t monster_spawn_timer;
    uint8_t initial_shell_health; // excalibur wipes out immediately, bombs take off 11 points; when 0 vulnerable to whip
    bool seen_by_kingu;
};

class AnubisTwo : public Monster
{
  public:
    float spawn_x;
    float spawn_y;
    uint32_t unknown1;
    float attack_proximity_y; // how close you have to be to be attacked; set both to 0 for a pet anubis
    float attack_proximity_x;
    uint8_t ai_timer;
    uint8_t next_attack_timer;
};

class Anubis : public Monster
{
  public:
    float spawn_x;
    float spawn_y;
    float unknown1;
    float attack_proximity_y; // how close you have to be to be attacked; set both to 0 for a pet anubis
    float attack_proximity_x;
    uint8_t ai_timer;
    uint8_t next_attack_timer;
    uint8_t psychic_orbs_counter;
    bool awake;
};

class Cobra : public Monster
{
  public:
    uint8_t spit_timer; // alternates between counting to 100 and to 64
    uint8_t padding1;
    uint8_t padding2;
    uint8_t padding3;
    int32_t duplicate_move_state; // 6 is put in movable.move_state as well as in here while spitting poison
};

class CatMummy : public Monster
{
  public:
    uint8_t ai_state;
    uint8_t attack_timer;
};

class Sorceress : public Monster
{
  public:
    int32_t unknown1;
    int16_t walk_pause_timer; // alternates between walking and pausing every time it reaches zero
    uint16_t padding1;
    uint16_t inbetween_attack_timer;
    uint16_t padding2;
    float airtime;                    // why float?
    Illumination* halo_emitted_light; // not the shot but the halo, try putting a color at 10
    Entity* fx_entity;
    SoundPosition* sound_pos;
    uint8_t hover_timer;
};

class MagmaMan : public Monster
{
  public:
    Illumination* emitted_light;
    SoundPosition* sound_pos;
    ParticleEmitterInfo* particle;
    uint16_t jump_timer; // when 0 it jumps
    uint16_t padding1;
    uint16_t alive_timer; // when 0, the magmaman disappears
};

class Bee : public Monster
{
  public:
    uint8_t unknown1;
    uint8_t unknown2;
    uint8_t unknown3;
    uint8_t unknown4;
    uint32_t unknown5;
    SoundPosition* sound_pos;
    uint16_t fly_hang_timer; // alternates between hanging/sitting on the wall and flying every time it reaches zero
    uint8_t ai_state;
    uint8_t targeting_timer; // counts when bee takes off and hasn't spotted a target yet
    uint8_t unknown_rand1;   // looks to be a random number being put in here
    uint8_t unknown_rand2;   // looks to be a random number being put in here
    uint8_t unknown11;
    uint8_t unknown12;
    float wobble_x; // maybe the positional offset to make it look like it's buzzing
    float wobble_y;
};

class Ammit : public Monster
{
  public:
    uint8_t walk_pause_timer; // alternates between walking and pausing every time it reaches zero
};

class ApepPart : public Monster
{
  public:
    float y;
    float x;
};

class ApepHead : public ApepPart
{
  public:
    uint64_t unknown1;
    SoundPosition* sound_pos1;
    SoundPosition* sound_pos2;
};

class OsirisHead : public Monster
{
  public:
    int32_t right_hand; // right from his perspective
    int32_t left_hand;
    bool moving_left;
    uint8_t targeting_timer;
    uint8_t invincibility_timer;
};

class OsirisHand : public Monster
{
  public:
    uint8_t attack_cooldown_timer;
};

class Yeti : public Monster
{
  public:
    int32_t unknown1;
    uint16_t walk_pause_timer; // alternates between walking and pausing every time it reaches zero
};

class Alien : public Monster
{
  public:
    uint8_t jump_timer; // when 0 it jumps
};

class UFO : public Monster
{
  public:
    SoundPosition* sound_pos;
    int16_t patrol_distance; // pos = right, neg = left
    uint8_t attack_cooldown_timer;
};

class Lahamu : public Monster
{
  public:
    SoundPosition* sound_pos;
    PlayerTracker* eyeball_positioner;
    uint16_t attack_cooldown_timer;
    bool unknown_proximity_related; // gets set to true when you get close to Lahamu
};

class YetiQueen : public Monster
{
  public:
    uint16_t walk_pause_timer; // alternates between walking and pausing every time it reaches zero
};

class YetiKing : public Monster
{
  public:
    uint32_t walk_pause_timer; // alternates between walking and pausing every time it reaches zero
    uint32_t unknown1;
    Illumination* emitted_light;
};

class Lamassu : public Monster
{
  public:
    SoundPosition* sound_pos;
    Entity* attack_effect_entity;
    ParticleEmitterInfo* particle;
    Illumination* emitted_light;
    uint32_t walk_pause_timer; // alternates between walking and pausing every time it reaches zero
    uint32_t flight_timer;     // when 0 it starts flying; only counts down when it's walking
    int32_t attack_timer;      // greater than 0 = charging up; 0 = fire ; less than 0 = cooldown
    float attack_angle;
    uint8_t unknown_state;
};

class Olmite : public Monster
{
  public:
    int32_t unknown1;
    uint16_t walk_pause_timer; // alternates between walking and pausing every time it reaches zero
    int16_t unknown2;          // 10 sec timer starts ticking when pounce attack starts
    uint32_t unknown3;
    int32_t unknown4;
    float unknown5;
    uint8_t attack_cooldown_timer; // jump attacks can start when 0
    uint8_t unknown_countdown;
};

class Tiamat : public Monster
{
  public:
    uint64_t unknown1;
    int32_t fx_tiamat_head;
    int32_t fx_tiamat_arm_right1;
    int32_t fx_tiamat_arm_right2;
    uint8_t frown_timer; // makes her frown but doesn't seem to be used in cutscene or during attack
    uint8_t padding1;
    int16_t damage_timer;  // phase 1: 1 sec frowning (-60 to 0) ; phase 2: 1 sec yelling/repulsing (-60 to 0) ; phase 3: 5 secs of vulnerability (300 to 0)
    uint32_t attack_timer; // when 0, she attacks
    float unknown2;
    float unknown3;
    float unknown4;
    float unknown5;
    float unknown6;
};

class GiantFrog : public Monster
{
  public:
    Entity* door_front_layer;
    Entity* door_back_layer;
    Entity* platform;
    uint16_t attack_timer;
    uint16_t frogs_ejected_in_cycle; // ejects two frogs in quick succession, then waits a longer time
};

class Frog : public Monster
{
  public:
    int32_t unknown1;
    uint32_t jump_timer; // when 0 it jumps
};

class FireFrog : public Frog
{
  public:
    SoundPosition* sound_pos;
};

class Grub : public Monster
{
  public:
    float rotation_delta;
    bool drop;
    uint8_t unknown3;
    uint8_t unknown4;
    uint8_t unknown5;
    uint16_t unknown6;
    uint16_t walk_pause_timer; // alternates between walking and pausing every time it reaches zero
};

class Tadpole : public Monster
{
  public:
    uint8_t acceleration_timer; // when 0, the tadpole gets some horizontal velocity
    bool player_spotted;
};

class GiantFly : public Monster
{
  public:
    Entity* head_entity;
    SoundPosition* sound_pos;
    ParticleEmitterInfo* particle;
    float sine_amplitude;
    float sine_frequency;
    float delta_y_angle; // determines where in the sine wave we are (0 - 2 pi)
    float unknown7;
    float unknown8;
    uint8_t sine_counter;
};

class Ghist : public Monster
{
  public:
    int32_t linked_corpse_entity_uid;
    uint32_t unknown1;
    SoundPosition* sound_pos;
    uint8_t movement_related_timer_1;
    uint8_t movement_related_timer_2;
    uint8_t transparency;
    uint8_t padding1;
    uint16_t disappear_timer; // when 0, it disappears for a bit
};

class JumpDog : public Monster
{
  public:
    uint16_t walk_pause_timer; // alternates between walking and pausing every time it reaches zero
    uint16_t squish_timer;     // squish after jumped on; don't put it too high, the game puts it at 60 frames / 1 sec
};

class EggplantMinister : public Monster
{
  public:
    SoundPosition* sound_pos;
    uint8_t unknown1;         // checks flag 0x2F of 64-bit{flags, more_flags}, maybe prevent from squishing when stomped while falling?
    uint8_t walk_pause_timer; // alternates between walking and pausing every time it reaches zero
    uint16_t squish_timer;
};

class Yama : public Monster
{
  public:
    bool message_shown; // Every king and queen will someday lose their crown...
};

class Hundun : public Monster
{
  public:
    float applied_hor_velocity; // during flight
    float applied_ver_velocity;
    int32_t birdhead_entity_uid;
    int32_t snakehead_entity_uid;
    float y_level;
    uint16_t bounce_timer; // functionality depends on state, determines when it can bounce again, also how long velocity is applied for bouncing
    uint8_t fireball_timer;
    bool birdhead_defeated;
    bool snakehead_defeated;
    uint8_t hundun_flags;

    // hundun_flags:
    //   1: Will move to the left
    //   2: Birdhead emerged
    //   3: Snakehead emerged
    //   4: Top level arena reached
    //   5: Birdhead shot last (to alternate the heads shooting fireballs)
};

class HundunHead : public Monster
{
  public:
    uint64_t unknown1;
    int32_t egg_crack_effect_uid;
    int32_t targeted_player_uid;
    uint8_t unknown_timer;
};

class MegaJellyfish : public Monster
{
  public:
    Entity* flipper1;
    Entity* flipper2;
    SoundPosition* sound_pos;
    int32_t orb_uid;
    int32_t tail_bg_uid;
    float applied_velocity;
    float wagging_tail_counter; // float counter to determine position of wagging tail (0 = right ; pi = left)
    uint8_t flipper_distance;   // only applies to door-blocking one
    uint8_t velocity_application_timer;
};

class Scorpion : public Monster
{
  public:
    uint32_t walk_pause_timer;    // alternates between walking and pausing every time it reaches zero
    uint32_t jump_cooldown_timer; // only 10 frames
};

class Hermitcrab : public Monster
{
  public:
    uint32_t unknown1;
    int32_t carried_entity_uid;
    uint16_t walk_spit_timer;    // depending on movable.state either the countdown how many acid bubbles spat (3) or walking timer
    bool is_active;              // whether it is hidden behind the carried block or not
    bool is_inactive;            // must be something else, since we already have is_active (or vice versa)
    bool spawn_new_carried_item; // defaults to true, when toggled to false, a new carried item spawns
};

class Necromancer : public Monster
{
  public:
    int32_t unknown1;
    uint16_t walk_pause_timer; // alternates between walking and pausing every time it reaches zero
    int16_t unknown_timer_1;
    SoundPosition* sound_pos;
    float red_skeleton_spawn_x;
    float red_skeleton_spawn_y;
    int32_t resurrection_uid;
    uint8_t unknown2;
    uint8_t resurrection_timer;
};

class ProtoShopkeeper : public Monster
{
  public:
    uint64_t unknown1;
    uint8_t movement_state;
    uint8_t padding1;
    uint8_t padding2;
    uint8_t blowup_related;
    uint32_t animation_timer;
    uint16_t walk_pause_explode_timer; // alternates between walking and pausing; when stunned: crawl timer;  after crawling: head pulse timer (pulse frequency dependend on timer)
    uint8_t walking_speed;             // 0 = slow; 4 = fast
    uint8_t padding7;
    uint32_t unknown3; // counts down from 10 to 3 in the headpulse phase
};

class ShopkeeperClone : public Monster
{
  public:
    float climb_y_direction; // when on a rope or ladder
    uint8_t unknown_countdown_timer_1;
    bool unknown1;
    uint8_t unknown_countdown_timer_2;
    uint8_t target_spotted_timer;
    uint8_t ai_state;
    bool spawn_gun; // defaults to true, when toggled to false it will spawn a gun
};

class Sparrow : public Monster
{
  public:
    uint32_t unknown1;
    uint8_t unknown2;
    uint8_t unknown3;
    uint8_t unknown4;
    uint8_t target_spotted_timer;
    uint8_t ai_state;
    bool aggro;
};

class Beg : public Monster
{
  public:
    uint64_t unknown1;
    uint64_t unknown2;
    uint32_t walk_pause_timer; // alternates between walking and pausing every time it reaches zero
    uint16_t disappear_timer;
};

class Critter : public Monster
{
  public:
    int32_t unknown1;
    uint32_t unknown2;
};

class CritterCrab : public Critter
{
  public:
    uint8_t walk_pause_timer; // alternates between walking and pausing every time it reaches zero
    bool walking_left;
};

class CritterButterfly : public Critter
{
  public:
    uint8_t unknown1;
    uint8_t change_direction_timer;
    uint8_t unknown2;
    int8_t vertical_flight_direction;
};

class CritterLocust : public Critter
{
  public:
    SoundPosition* sound_pos;
    uint8_t jump_timer; // when 0 it jumps
    uint8_t unknown1;
};

class CritterSnail : public Critter
{
  public:
    float x_direction;
    float y_direction;
    float pos_x;
    float pos_y;
    float rotation_center_x;
    float rotation_center_y;
    float rotation_angle;
    float rotation_speed;
};

class CritterFish : public Critter
{
  public:
    uint8_t swim_pause_timer; // alternates between swimming and pausing every time it reaches zero
    bool player_in_proximity; // swims away fast when player detected
};

class CritterPenguin : public Critter
{
  public:
    uint8_t walk_pause_timer; // alternates between walking and pausing every time it reaches zero
    uint8_t padding;
    uint16_t jump_timer;
};

class CritterFirefly : public Critter
{
  public:
    float sine_amplitude;
    float sine_frequency;
    float sine_angle; // determines where in the sine wave we are (0 - 2 pi)
    float unknown1;
    float unknown2;
    uint8_t change_direction_timer;
    uint8_t sit_timer;
    uint8_t sit_cooldown_timer;
};

class CritterDrone : public Critter
{
  public:
    Illumination* emitted_light;
    SoundPosition* sound_pos;
    float applied_hor_momentum;
    float applied_ver_momentum;
    bool unknown1;
    uint8_t move_timer;
};

class CritterSlime : public Critter
{
  public:
    float x_direction;
    float y_direction;
    float pos_x;
    float pos_y;
    float rotation_center_x;
    float rotation_center_y;
    float rotation_angle;
    float rotation_speed;
    int16_t walk_pause_timer; // alternates between walking and pausing every time it reaches zero
};
