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

class RoomOwner : public Monster
{
  public:
    int32_t room_index;
    float climb_y_direction;    // when on a rope or ladder
    uint8_t ai_state;
    uint8_t patrol_timer;
    uint8_t lose_interest_timer; // delay in-between attacks
    uint8_t unknown_timer1;
    uint16_t countdown_timer;      // counts down to 0 when a value is forced, can't shot when the timer is running
    uint8_t unknown1;
    bool is_patrolling; // to set off a shopkeeper, combine this with state.shoppie_aggro_levels > 0
    bool aggro; // setting this makes him angry, if it's shopkeeper you get 2 agrro points
    bool was_hurt; // also is set true if you set aggro to true, get's trigger even when whiping
    uint16_t padding1;
    uint32_t padding2;
};

class WalkingMonster : public Monster
{
  public:
    int32_t chatting_to_uid;
    int16_t walk_pause_timer; // alternates between walking and pausing every time it reaches zero
    int16_t cooldown_timer; // used fo chatting with other monsters, attack cooldowns etc.
};

class NPC : public Monster
{
  public:
    float climb_direction;
    uint8_t unknown1;
    uint8_t unknown2;
    uint8_t unknown_timer;
    uint8_t target_in_sight_timer;
    uint8_t ai_state;
    bool aggro; // for bodyguard and shopkeeperclone it spawns a weapon as well
    uint8_t padding1;
    uint8_t padding2;
    uint32_t padding3;
};

class Ghost : public Monster
{
  public:
    uint16_t split_timer; // for SMALL_HAPPY this is also the sequence timer of its various states
    uint8_t unknown1_timer;
    uint8_t unknown2;
    float velocity_multiplier;
    uint16_t unknown3; // layer change related
    uint8_t ghost_behaviour; // 0 = SMALL_ANGRY aka standard chasing ; 1 = SMALL_SURPRISED; 2 = SMALL_SAD; 3 = SMALL_HAPPY
    // 4 and above = will move up and down, moving slightly more in one direction
    uint8_t unknown6;
    bool unknown7;
    uint8_t unknown8;
    uint8_t unknown9;
    uint8_t unknown10;
    Illumination* emitted_light;
    Entity* linked_ghost;
    SoundPosition* sound_info;
};

class Bat : public Monster
{
  public:
    float spawn_x;
    float spawn_y;
};

class Jiangshi : public Monster
{
  public:
    uint8_t wait_timer; // wait time between jumps
    uint8_t jump_counter; // only female aka assassin, when 0 will jump up into ceiling
    bool on_ceiling; // only female aka assassin
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
    uint8_t jump_timer;
    uint8_t padding; // ?
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
    int32_t burrowing_in_uid; // stores the last uid as well
    uint8_t counter_burrowing;
    uint8_t counter_nonburrowing;
    uint8_t countdown_for_appearing;
    uint8_t unknown_two_while_burrowed;
    uint8_t state; // 0 non_borrowed, 1 - unknown, 2 - borrowed, 3 - state_change
};

class Bat : public Monster
{
  public:
    float spawn_x;
    float spawn_y;
};

class Spider : public Monster
{
  public:
    float ceiling_pos_x;
    float ceiling_pos_y;
    uint8_t jump_timer;
    uint8_t padding1;
    uint16_t padding2;
    float trigger_distance; // only in the x coord
};

class HangSpider : public Monster
{
  public:
    int16_t dangle_jump_timer;
    uint16_t padding;
    float ceiling_pos_x;
    float ceiling_pos_y;
};

class Shopkeeper : public RoomOwner
{
  public:
    uint8_t name; // 0 - Ali, 1 - Bob, 2 - Comso ... and so one, anything above 28 is just random string, can crash the game
    uint8_t shotgun_attack_delay; // can't shot when the timer is running
    uint8_t unknown3; // accessed on stun/dmg? probably bool
    bool shop_owner;
    bool unknown5a; // sometimes set to true
    uint8_t padding1;
    uint8_t padding2;
    uint8_t padding3;
};

class Yang : public RoomOwner
{
  public:
    std::map<int64_t, int64_t> unknown2;
    uint8_t unknown4;
    uint8_t unknown5;
    bool first_message_shown; // I'm looking for turkeys, wanna help?
    bool quest_incomplete;    // gets set to false when the quest is over (Yang dead or second turkey delivered)
    bool special_message_shown; // tusk palace, black market or one way door message shown
    uint8_t padding4;
    uint8_t padding5;
    uint8_t padding6;
};

class Tun : public RoomOwner
{
  public:
    uint8_t arrows_left;
    uint8_t reload_timer;    // when 0, a new arrow is loaded into the bow; resets when she finds an arrow on the ground
    bool challenge_fee_paid; // affect only the speech bubble
    bool congrats_challenge; // congrats message shown after exiting a challenge
    bool murdered;           // when she disappears in a cloud of smoke
    bool shop_entered;
    bool tiamat_encounter; // if set to false, greets you with 'you've done well to reach this place'
    uint8_t padding;
};

class Pet : public Monster
{
  public:
    Entity* fx_button;
    int32_t petting_by_uid; // person whos petting it, only in the camp
    int16_t yell_counter; // counts up to 400 (6.6 sec); when 0 the pet yells out
    uint16_t unknown1;
    uint16_t unknown2;
    int16_t func_timer; // used when free running in the camp
    int8_t active_state; // -1 = sitting and yelling; 0 = either running, dead or picked up
};

class Caveman : public WalkingMonster
{
  public:
    uint8_t wake_up_timer;
    uint8_t can_pick_up_timer;   // 0 = can pick something up; when holding forced to 179; after tripping and regaining consciousness counts down to 0
    int16_t unknown_aggro_timer; // 1 sec timer at the start of getting aggro
    int16_t aggro_timer;         // keeps resetting when angry and a player is nearby
};

class CavemanShopkeeper : public WalkingMonster
{
  public:
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
    int32_t eaten_uid; // dungbeetle being eaten
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
    uint32_t timer;
};

class Mantrap : public Monster
{
  public:
    int32_t walk_pause_timer; // alternates between walking and pausing every time it reaches zero
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
    uint16_t timer;             // how long to stay in that position
};

class Imp : public Monster
{
  public:
    int32_t carrying_uid;
    float patrol_y_level;
};

class Lavamander : public Monster
{
  public:
    Illumination* emitted_light;
    uint8_t shoot_lava_timer; // when this timer reaches zero, it appears on the surface/shoots lava; triggers on player proximity
    uint8_t padding1;
    uint16_t jump_pause_timer; // jump pause when cool; runs continuously when hot
    uint8_t lava_detection_timer;
    bool is_hot;
    uint8_t player_detect_state; // 0 - didnt_saw_player, 1 - saw_player, 2 - spited_lava // used so he won't spit imminently after seeing the player
    uint8_t padding2;
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
    uint32_t ai_timer;        // when reaches zero, decides whether to walk or fly or pause; doesn't reset when flying
    uint32_t walking_timer; // counts down only when walking; can't start flying unless this is at zero
};

class Robot : public WalkingMonster
{
  public:
    SoundPosition* sound_pos;
    Illumination* emitted_light_explosion;
};

class Quillback : public WalkingMonster
{
  public:
    SoundPosition* sound_pos;
    ParticleEmitterInfo* particle;
    int16_t unknown_state; // accessed as byte and word, so, flags?, changed mostly when breaking floors
    uint8_t unknown_timer;
    uint8_t unknown3; // forced to 1 when rolling, otherwise compared with 0
};

class Leprechaun : public WalkingMonster
{
  public:
    SoundPosition* sound_pos;
    uint8_t hump_timer;
    uint8_t target_in_sight_timer;
    uint16_t gold; // ammoung of gold he picked up, will be droped on death
    uint8_t timer_after_humping;
    uint8_t unknown;
};

class Crocman : public WalkingMonster
{
  public:
    uint8_t teleport_cooldown;
};

class Mummy : public Monster
{
  public:
    uint32_t walk_pause_timer; // alternates between walking and pausing when timer reaches zero
};

class VanHorsing : public NPC
{
  public:
    bool show_text; // if set to true, he will say 'i've been hunting this fiend a long time!' when on screen
};

class WitchDoctor : public WalkingMonster
{
  public:
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

class ForestSister : public NPC
{
  public:
    uint32_t walk_pause_timer; // alternates between walking and pausing when timer reaches zero
};

class Vampire : public Monster
{
  public:
    float jump_trigger_distance_x; // the distance from the player it starts jumping
    float jump_trigger_distance_y;
    float sleep_pos_x;
    float sleep_pos_y;
    uint32_t walk_pause_timer; // alternates between walking and pausing when timer reaches zero
};

class Vlad : public Vampire
{
  public:
    uint32_t unknown1;
    uint8_t teleport_timer; // triggers when Vlad teleports, when timer running he can't teleport and will stun when hit
    bool aggro; // or awake
};

class Waddler : public RoomOwner
{
  public:
    bool player_detected; // to show the welcome message
    bool on_the_ground;
    uint8_t air_timer; // counts while in the air, jumping
};

class Octopus : public WalkingMonster
{
  public:
    int8_t unknown1; // something to do with state?
};

class Bodyguard : public NPC
{
  public:
    uint8_t position_state; // 0 - none, 1 - Tusk dice shop, 2 - Entrence to pleasure palace, 3 - Basement entrance to pleasure palace
    bool message_shown; // entrence and basement entrence
};

class Fish : public Monster
{
  public:
    uint32_t change_direction_timer;
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
    float unknown2; // increment value for unknown3?
    float unknown3;
};

class Crabman : public Monster
{
  public:
    uint32_t walk_pause_timer; // alternates between walking and pausing when timer reaches zero
    uint8_t invincibility_timer; // triggers after being attacked
    uint8_t poison_attack_timer;
    uint8_t padding4;
    uint8_t padding5;
    int32_t attacking_claw_uid;
    bool at_maximum_attack;
};

class Kingu : public Monster
{
  public:
    SoundPosition* sound_info1; // initialized when breaking the shell (sliding down sound maybe?)
    SoundPosition* sound_info2; // Turning into stone sound
    float climb_direction_x; // distance too?
    float climb_direction_y;
    uint16_t climb_pause_timer;
    uint8_t shell_invincibility_timer;
    uint8_t monster_spawn_timer;
    uint8_t initial_shell_health; // excalibur wipes out immediately, bombs take off 11 points; when 0 vulnerable to whip
    bool player_seen_by_kingu;    // unsure if correct
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
    uint32_t spit_timer; // alternates between counting to 100 and to 64
    int32_t duplicate_move_state; // 6 is put in movable.move_state as well as in here while spitting poison
};

class CatMummy : public Monster
{
  public:
    uint8_t ai_state;
    uint8_t attack_timer;
};

class Sorceress : public WalkingMonster
{
  public:
    uint32_t inbetween_attack_timer;
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
    uint32_t jump_timer; // when 0 it jumps
    uint32_t alive_timer; // when 0, the magmaman disappears
};

class Bee : public Monster
{
  public:
    bool can_rest;
    uint8_t padding1;
    uint16_t padding2;
    uint32_t padding3;
    SoundPosition* sound_pos;
    uint16_t fly_hang_timer; // alternates between hanging/sitting on the wall and flying every time it reaches zero
    uint8_t ai_state;
    uint8_t targeting_timer; // counts when bee takes off and hasn't spotted a target yet
    uint8_t unknown_rand1;   // looks to be a random number being put in here
    uint8_t unknown_rand2;   // looks to be a random number being put in here, something related to other bees in a level?
    uint8_t padding4;
    uint8_t padding5; // padding? quite a lot of unused memory in this entity, maybe this is more the one type?
    float wobble_x; // maybe the positional offset to make it look like it's buzzing
    float wobble_y;
};

class Ammit : public Monster
{
  public:
    uint32_t walk_pause_timer; // alternates between walking and pausing every time it reaches zero
    uint32_t padding; // not sure?
    ParticleEmitterInfo* particle;
};

class ApepPart : public Monster
{
  public:
    float y_pos;
    float sine_angle;
    uint8_t sync_timer; // or pause timer, used to sync the body parts moving up and down
    uint8_t padding1;
    uint16_t padding2;
    uint32_t padding3;
};

class ApepHead : public ApepPart
{
  public:
    SoundPosition* sound_pos1;
    SoundPosition* sound_pos2;
    float distance_traveled;
    uint32_t tail_uid;
    uint32_t fx_mouthpiece1_uid;
    uint32_t fx_mouthpiece2_uid;
};

class OsirisHead : public Monster
{
  public:
    int32_t right_hand_uid; // right from his perspective
    int32_t left_hand_uid;
    bool moving_left;
    uint8_t targeting_timer;
    uint8_t invincibility_timer;
};

class OsirisHand : public Monster
{
  public:
    uint8_t attack_cooldown_timer;
};

class Alien : public Monster
{
  public:
    uint8_t jump_timer; // when 0 it jumps
    uint8_t unknown1;
    uint8_t unknown2; // seen timer here once maybe? it's accessed right as he jumps from the ufo
};

class UFO : public Monster
{
  public:
    SoundPosition* sound_pos;
    int16_t patrol_distance; // pos = right, neg = left
    uint8_t attack_cooldown_timer;
    bool is_falling; // read only, manually setting this true has no effect
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
    uint32_t walk_pause_timer; // alternates between walking and pausing every time it reaches zero
    uint8_t unknown_timer;
};

class YetiKing : public Monster
{
  public:
    uint32_t walk_pause_timer; // alternates between walking and pausing every time it reaches zero
    uint8_t unknown_timer;
    uint8_t unknown1;
    uint16_t unknown2;
    Illumination* emitted_light;
    ParticleEmitterInfo* particle_fog;
    ParticleEmitterInfo* particle_dust;
    ParticleEmitterInfo* particle_sparkles;
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

class Olmite : public WalkingMonster
{
  public:
    bool armor_on; 
    bool in_stack; // disables the attack, stun, lock's looking left flag between stack
    bool in_stack2; // is set to false couple frame after being detached from stack
    int8_t padding1;
    int32_t on_top_uid; 
    float y_offset; // same as y position when stacked
    uint8_t attack_cooldown_timer; // jump attacks can start when 0
    uint8_t unknown_countdown;
    uint16_t padding2;
};

class Tiamat : public Monster
{
  public:
    SoundPosition* sound_pos; // Turning into stone sound
    int32_t fx_tiamat_head_uid;
    int32_t fx_tiamat_arm_right1_uid;
    int32_t fx_tiamat_arm_right2_uid;
    uint8_t invincibility_timer;
    uint8_t padding1;
    int16_t damage_timer;  // phase 1: 1 sec frowning (-60 to 0) ; phase 2: 1 sec yelling/repulsing (-60 to 0) ; phase 3: 5 secs of vulnerability (300 to 0)
    uint32_t attack_timer; // when 0, she attacks
    float unknown2;
    float tail_angle;
    float tail_radian; // Counts from 0 to 2*pi, Used to calculate tail angle
    float tail_move_speed;
    float right_arm_angle;
};

class GiantFrog : public Monster
{
  public:
    Entity* door_front_layer;
    Entity* door_back_layer;
    Entity* platform;
    uint16_t attack_timer;
    uint8_t frogs_ejected_in_cycle; // ejects thee frogs in quick succession, then waits a longer time
    uint16_t invincibility_timer;
    uint8_t mouth_close_timer;
    bool mouth_open_trigger; // opens the mouth and starts mouth_close_timer, used when detecting grub in the mouth area
    uint8_t padding1;
    uint8_t padding2;
};

class Frog : public Monster
{
  public:
    int32_t grub_being_eaten_uid;
    uint32_t jump_timer; // when 0 it jumps
    bool pause; // not actually used?
    int16_t padding;
};

class FireFrog : public Frog
{
  public:
    SoundPosition* sound_pos;
};

class Grub : public Monster
{
  public:
    float rotation_delta; // is it? i seen it in the code compared as a bit with 1, but not sure when it happens
    bool drop;
    bool unknown3;
    uint8_t looking_for_new_direction_timer; // used when he touches floor/wall/ceiling
    uint8_t padding1;
    uint16_t unknown6_timer; // paused when not moving
    uint16_t walk_pause_timer; // alternates between walking and pausing every time it reaches zero
    int16_t turn_into_fly_timer;
    uint16_t padding2;
    ParticleEmitterInfo* particle;
    SoundPosition* sound_pos;
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
    int32_t body_uid;
    uint8_t idle_timer; // will not chase after you while the timer is running
    uint8_t padding1;
    uint16_t padding2;
    SoundPosition* sound_pos;
    uint8_t movement_related_timer_1;
    uint8_t movement_related_timer_2;
    uint8_t transparency;
    uint8_t padding3;
    uint16_t fadeout; // when 0, ghist fades out/dies
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
    float attack_position_x; // Posiotion where the head will move
    float attack_position_y; // Posiotion where the head will move
    int32_t egg_crack_effect_uid;
    int32_t targeted_player_uid;
    uint16_t looking_for_target_timer; // also cooldown before attack
    uint8_t invincibility_timer;
};

class MegaJellyfish : public Monster
{
  public:
    Entity* flipper1;
    Entity* flipper2;
    SoundPosition* sound_pos;
    int32_t orb_uid; // the closest orb, does not gets updated
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
    uint32_t carried_entity_type;
    int32_t carried_entity_uid;
    uint16_t walk_spit_timer;    // depending on movable.state either the countdown how many acid bubbles spat (3) or walking timer
    bool is_active;              // whether it is hidden behind the carried block or not, if true you can damage him
    bool is_inactive;            // must be something else, since we already have is_active (or vice versa), gets triggered about have a second after is_active, maybe something like, can attack
    bool spawn_new_carried_item; // defaults to true, when toggled to false, a new carried item spawns
    uint8_t unknown; // sometimes i see 1 here, didn't get anything with breakpoint
};

class Necromancer : public WalkingMonster
{
  public:
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
    //"1": "Headpulse/explosion related",
    //"2": "Walking",
    //"3": "Headpulse/explosion related",
    //"4": "Crawling",
    //"5": "",
    //"6": "Headpulse/explosion related",
    uint8_t padding1;
    uint8_t padding2;
    uint8_t blowup_related;
    uint32_t animation_timer;
    uint16_t walk_pause_explode_timer; // alternates between walking and pausing; when stunned: crawl timer;  after crawling: head pulse timer (pulse frequency dependend on timer)
    uint8_t walking_speed;             // 0 = slow; 4 = fast
    uint8_t padding7;
    uint32_t unknown3; // counts down from 10 to 3 in the headpulse phase
};

class Beg : public NPC
{
  public:
    uint32_t walk_pause_timer; // alternates between walking and pausing every time it reaches zero
    uint16_t disappear_timer;
};

class Terra : public Monster
{
  public:
    Entity* fx_button; // used in the camp
    float x_pos;
    uint16_t unknown;
    uint16_t abuse_speechbubble_timer;
};

class Critter : public Monster
{
  public:
    int32_t last_picked_up_by_uid;
    uint8_t state;
    uint8_t padding1;
    uint8_t padding2;
    uint8_t padding3;
};

class CritterBeetle : public Critter
{
  public:
    bool pause; // used when he's getting eaten
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
    int64_t unknown1a;
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

class CritterBeetle : public Critter
{
  public:
    bool pause; // used when he's getting eaten
};
