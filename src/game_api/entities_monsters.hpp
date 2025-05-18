#pragma once

#include <cstdint>

#include "containers/custom_set.hpp"
#include "containers/custom_vector.hpp"
#include "entities_chars.hpp"
#include "movable.hpp"
#include "particles.hpp"
#include "sound_manager.hpp"

class Monster : public PowerupCapable
{
  public:
    int32_t chased_target_uid;
    uint32_t target_selection_timer;

    virtual void increase_killcount() = 0;    // 95, increases state.kills_npc, is not called for normal monsters but they all have the same function
    virtual void on_aggro(uint8_t, bool) = 0; // 96, updates state.quests in case of npc
    virtual void unknown_v97() = 0;           // 97, can't trigger it
    virtual void on_shop_entered() = 0;       // 98
    // shopkeeper will walk towards you (doesn't work for Yang, even though he has the same virtual)
    // if disabled some monster will stop moving (like bats, jiangshi) some wont attack (crabman), shopkeeper can still kick you but won't fire his weapon
    virtual void attack_logic_related(uint8_t, float) = 0; // 99
    virtual bool update_target(Entity* ent, float&) = 0;   // 100, float from the function above, also works as an output?
};

// RoomOwner and NPC may have common subclass for the first two virtuals, but they later diverge, weapon type is the same spot, but they probably just made one first then copied over the virtuals
class RoomOwner : public Monster
{
  public:
    int32_t room_index;
    float climb_y_direction; // when on a rope or ladder
    uint8_t ai_state;
    uint8_t patrol_timer;
    uint8_t lose_interest_timer; // delay in-between attacks
    uint8_t unknown_timer1;
    /// can't shot when the timer is running
    uint16_t countdown_timer; // counts down to 0 when a value is forced
    uint8_t unknown1;
    bool is_patrolling; // to set off a shopkeeper, combine this with state.shoppie_aggro_levels > 0
    /// setting this makes him angry, if it's shopkeeper you get 2 aggro points
    bool aggro_trigger;
    /// also is set true if you set aggro to true, get's trigger even when whipping
    bool was_hurt;
    uint16_t padding1;
    uint32_t padding2;

    virtual void on_criminal_act_committed(uint8_t reason) = 0; // shows the appropriate message (vandal, cheater, ...)
    // for shopkeepers: checks state.shoppie_aggro_levels, for waddler checks the state.quest_flags
    // if you return false, but you have attacked them before, they will be patrolling but won't attack you on sight
    virtual bool should_attack_on_sight() = 0; // Tusk and Yang always return false (Yang won't show anymore if attacked before, Tusk is aggroed from the start on 6-3 when you attacked her in tidepool)
    virtual bool is_angry_flag_set() = 0;      // checks state.level_flags 10-16 depending on the monster

    // only for shopkeeper: sets shopkeeper.shotgun_attack_delay to 6
    // triggers only at the start when aggroed
    virtual void set_initial_attack_delay() = 0;

    virtual Entity* spawn_weapon() = 0; // return the weapon entity that will be used to attack the player
    virtual ENT_TYPE weapon_type() = 0; // the entity type of the weapon that will be spawned to attack the player
    /// For weapons, checks if the entity should hit the trigger or not
    virtual bool should_attack(std::tuple<Entity*, float, float, float> target) = 0; // parameter is some struct that contains the target

    virtual void unknown_v108() = 0;           // for shopkeepers, it loops over (some of) the items for sale
    virtual void on_death_treasure_drop() = 0; // coins and if you're lucky, gold bar from shopkeeper
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
    /// for bodyguard and shopkeeperclone it spawns a weapon as well
    bool aggro;
    uint8_t padding1;
    uint8_t padding2;
    uint32_t padding3;

    virtual void on_criminal_act_committed() = 0;
    virtual bool should_attack_on_sight() = 0;
    virtual Vec2& v_103(Vec2&) = 0;              // only accessed when not angered
    virtual void on_interaction(bool, bool) = 0; // does the quests stuff etc.
    /// Do not use this function for Tusk or Waddler, small oversight by the devs, they never call this function for them, which would try to spawn entity type 0
    virtual Entity* spawn_weapon() = 0;
    virtual ENT_TYPE weapon_type() = 0;
    /// For weapons, checks if the entity should hit the trigger or not
    virtual bool should_attack(std::tuple<Entity*, float, float, float> target) = 0;
    virtual void on_criminal_act_committed2(void*) = 0; // calls the on_criminal_act_committed except for bodyguard which calls the should_attack_on_sight and turns off any speech-bubble
};

class WalkingMonster : public Monster
{
  public:
    int32_t chatting_to_uid;
    /// alternates between walking and pausing every time it reaches zero
    int16_t walk_pause_timer;
    /// used for chatting with other monsters, attack cooldowns etc.
    int16_t cooldown_timer;
    // Aggro or calm, if forced to return 0 it will not aggro unless you overlap his hitbox. For caveman this is called when he wakes up (from sleep or stun)
    virtual bool can_aggro() = 0;
    virtual Entity* v_102(Entity*) = 0; // returns the same entity as provided in the parameter
};

enum class GHOST_BEHAVIOR : uint8_t
{
    SAD = 0,
    MEDIUM_SAD = 0,
    MEDIUM_HAPPY = 1,
    SMALL_ANGRY = 0,
    SMALL_SURPRISED = 1,
    SMALL_SAD = 2,
    SMALL_HAPPY = 3
};

class Ghost : public Monster
{
  public:
    /// for SMALL_HAPPY this is also the sequence timer of its various states
    uint16_t split_timer;
    uint8_t wobble_timer;
    uint8_t unknown2;
    float velocity_multiplier;
    /// Controls ghost pacing when all players are dead.
    uint16_t pace_timer;
    GHOST_BEHAVIOR ghost_behaviour;
    bool blown_by_player;
    bool happy_dancing_clockwise; // Randomly set at the start of happy's dance phase to determine the dance rotation direction.
    uint8_t unknown8;
    uint8_t unknown9;
    uint8_t unknown10;
    Illumination* emitted_light;
    Entity* linked_ghost;
    float target_dist_visibility_factor;  // Value from 0.5 to 1, based on the distance to the ghost's target, multiplied by the target_layer_visibility_factor to set the transparency and illumination of the ghost.
    float target_layer_visibility_factor; // Value from 0 to 1, based on how long the ghost has been in the same layer as its target, multiplied by the target_dist_visibility_factor to set the transparency and illumination of the ghost.
    SoundMeta* sound;
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
    /// wait time between jumps
    uint8_t wait_timer;
    /// only female aka assassin: when 0 will jump up into ceiling
    uint8_t jump_counter;
    /// only female aka assassin
    bool on_ceiling;
};

class Monkey : public Monster
{
  public:
    SoundMeta* sound;
    uint8_t jump_timer;
    bool on_vine;
};

class GoldMonkey : public Monster
{
  public:
    uint8_t jump_timer;
    uint8_t padding;
    uint16_t poop_timer;
    uint8_t poop_count;
};

class Mole : public Monster
{
  public:
    SoundMeta* burrowing_sound;
    SoundMeta* nonburrowing_sound;
    ParticleEmitterInfo* burrowing_particle;
    float burrow_dir_x;
    float burrow_dir_y;
    /// stores the last uid as well
    int32_t burrowing_in_uid;
    uint8_t counter_burrowing;
    uint8_t counter_nonburrowing;
    uint8_t countdown_for_appearing;
    uint8_t unknown_two_while_burrowed;
    /// 0 - non_burrowed, 1 - unknown, 2 - burrowed, 3 - state_change
    uint8_t digging_state;
};

class Spider : public Monster
{
  public:
    float ceiling_pos_x;
    float ceiling_pos_y;
    /// For the giant spider, some times he shot web instead of jumping
    uint8_t jump_timer;
    uint8_t padding1;
    uint16_t padding2;
    /// only in the x coordinate
    float trigger_distance;

    virtual void spawn_offset_related() = 0; // disabling this function makes the spider spawn in weird position
    virtual float v_102() = 0;               // for spider returns 0.02, for giant spider 0.025, game does some calculations with this when triggered by player
    virtual bool on_ceiling() = 0;
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
    /// 0 - Ali, 1 - Bob, 2 - Comso ... and so one, anything above 28 is just random string, can crash the game
    uint8_t name;
    /// can't shot when the timer is running
    uint8_t shotgun_attack_delay;
    /// will drop key after stun/kill
    bool has_key;
    bool shop_owner;
    bool is_ear;
    uint8_t padding11;
    uint8_t padding21;
    uint8_t padding31;
};

class Yang : public RoomOwner
{
  public:
    /// Table of uids of the turkeys, goes only up to 3, is nil when yang is angry
    custom_set<int32_t> turkeys_in_den;
    uint8_t unknown4;
    uint8_t unknown5;
    /// I'm looking for turkeys, wanna help?
    bool first_message_shown;
    /// Is set to false when the quest is over (Yang dead or second turkey delivered)
    bool quest_incomplete;
    /// Tusk palace/black market/one way door - message shown
    bool special_message_shown;
    uint8_t padding4;
    uint8_t padding5;
    uint8_t padding6;
};

class Tun : public RoomOwner
{
  public:
    uint8_t arrows_left;
    /// when 0, a new arrow is loaded into the bow; resets when she finds an arrow on the ground
    uint8_t reload_timer;
    /// affect only the speech bubble
    bool challenge_fee_paid;
    /// congrats message shown after exiting a challenge
    bool congrats_challenge;
    bool murdered; // when she disappears in a cloud of smoke
    bool shop_entered;
    /// if set to false, greets you with 'you've done well to reach this place'
    bool tiamat_encounter;
    uint8_t padding;
};

class Pet : public Monster
{
  public:
    Entity* fx_button;
    /// person whos petting it, only in the camp
    int32_t petting_by_uid;
    /// counts up to 400 (6.6 sec), when 0 the pet yells out
    int16_t yell_counter;
    /// When sitting after colliding with a player in the camp.
    uint16_t sit_timer;
    /// Cooldown before sitting again when colliding with a player in the camp.
    uint16_t sit_cooldown_timer;
    /// used when free running in the camp
    int16_t func_timer;
    /// -1 = sitting and yelling, 0 = either running, dead or picked up
    int8_t active_state;
    /// number of times petted in the camp
    uint8_t petted_counter;
};

class Caveman : public WalkingMonster
{
  public:
    uint8_t wake_up_timer;
    /// 0 = can pick something up, when holding forced to 179, after tripping and regaining consciousness counts down to 0
    uint8_t can_pick_up_timer;
    int16_t unknown_aggro_timer; // 1 sec timer at the start of getting aggro
    /// keeps resetting when angry and a player is nearby
    int16_t aggro_timer;
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
    /// dungbeetle being eaten
    int32_t eaten_uid;
    /// alternates between walking and pausing when timer reaches zero
    uint8_t walk_pause_timer;
    /// won't attack until timer reaches zero
    uint8_t attack_cooldown_timer;
    uint8_t blood_squirt_timer;
    uint8_t unknown3;
    SoundMeta* sound;
    ParticleEmitterInfo* particle;
};

class Mosquito : public Monster
{
  public:
    float direction_x;
    float direction_y;
    float stuck_rel_pos_x; // position relative to the block the mosquito is stuck in
    float stuck_rel_pos_y;
    SoundMeta* sound;
    uint32_t timer;
};

class Mantrap : public Monster
{
  public:
    /// alternates between walking and pausing every time it reaches zero
    int32_t walk_pause_timer;
    /// the uid of the entity the mantrap has eaten, in case it can break out, like a shopkeeper
    int32_t eaten_uid;
};

class Skeleton : public Monster
{
  public:
    /// -1 = never explodes
    int16_t explosion_timer;
};

class Scarab : public Monster
{
  public:
    SoundMeta* sound;
    Illumination* emitted_light; // set color in sat_vign_0, be sure to make the aperture large enough
    /// how long to stay in current position
    uint16_t timer;
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
    /// when this timer reaches zero, it appears on the surface/shoots lava, triggers on player proximity
    uint8_t shoot_lava_timer;
    uint8_t padding1;
    uint16_t jump_pause_timer; // jump pause when cool; runs continuously when hot
    uint8_t lava_detection_timer;
    bool is_hot;
    /// 0 - didn't see player, 1 - saw player, 2 - spitted lava; probably used so he won't spit imminently after seeing the player
    uint8_t player_detect_state;
    uint8_t padding2;
};

class Firebug : public Monster
{
  public:
    SoundMeta* sound;
    uint16_t fire_timer; // when it reaches zero, it shoots fire
    bool going_up;
    bool detached_from_chain;
};

class FirebugUnchained : public Monster
{
  public:
    SoundMeta* sound;
    float max_flight_height; // increments when flying up, until it reaches 1.0, then bug flies down
    uint32_t ai_timer;       // when reaches zero, decides whether to walk or fly or pause; doesn't reset when flying
    uint32_t walking_timer;  // counts down only when walking; can't start flying unless this is at zero
};

class Robot : public WalkingMonster
{
  public:
    SoundMeta* sound;
    Illumination* emitted_light_explosion;
};

class Quillback : public WalkingMonster
{
  public:
    SoundMeta* sound;
    ParticleEmitterInfo* particle;
    int8_t unknown_state; // accessed as byte and word, so, flags?, changed mostly when breaking floors
    int8_t hit_wall_direction; // Set when in apply_movement to 1 when colliding with a hard wall to the left, -1 if to the right, 0 if no collision.
    uint8_t unknown_timer;
    bool seen_player;
};

class Leprechaun : public WalkingMonster
{
  public:
    SoundMeta* sound;
    uint8_t hump_timer;
    uint8_t target_in_sight_timer;
    /// amount of gold he picked up, will be drooped on death
    uint16_t gold;
    uint8_t timer_after_humping;
    bool jump_trigger; // Triggers a jump on the next frame.
    custom_vector<ENT_TYPE> collected_treasure;
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
    /// if set to true, he will say "I've been hunting this fiend a long time!" when on screen
    bool show_text;
    /// one way door message has been shown
    bool special_message_shown;
};

class WitchDoctor : public WalkingMonster
{
  public:
    SoundMeta* sound;
    uint16_t skull_regen_timer; // when zero, regenerates a new floating skull
};

class WitchDoctorSkull : public Monster
{
  public:
    int32_t witch_doctor_uid;
    uint32_t unknown1;
    Illumination* emitted_light;
    SoundMeta* sound;
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
    int32_t _padding;
};

class Vlad : public Vampire
{
  public:
    /// triggers when Vlad teleports, when timer running he can't teleport and will stun when hit
    uint8_t teleport_timer;
    /// or is awake
    bool aggro;
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
    /// 0 - none, 1 - Tusk dice shop, 2 - Entrance to pleasure palace, 3 - Basement entrance to pleasure palace
    uint8_t position_state;
    bool message_shown; // entrance and basement entrance
};

class Fish : public Monster
{
  public:
    uint32_t change_direction_timer;
};

class GiantFish : public Monster
{
  public:
    SoundMeta* sound;
    /// when bouncing into a wall
    uint16_t change_direction_timer;
    /// delay in-between attacks
    uint16_t lose_interest_timer;
    uint16_t unknown_timer;
    uint16_t padding1;
    float unknown1;
    float unknown2; // increment value for unknown3?
    float unknown3;
};

class Crabman : public Monster
{
  public:
    uint32_t walk_pause_timer;   // alternates between walking and pausing when timer reaches zero
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
    /// initialized when breaking the shell (sliding down sound maybe?)
    SoundMeta* sound1;
    /// Turning into stone sound
    SoundMeta* sound2;
    float climb_direction_x; // distance too?
    float climb_direction_y;
    uint16_t climb_pause_timer;
    uint8_t shell_invincibility_timer;
    uint8_t monster_spawn_timer;
    /// Excalibur wipes out immediately, bombs take off 11 points, when 0 vulnerable to whip
    uint8_t initial_shell_health;
    bool player_seen_by_kingu; // unsure if correct
};

class Anubis : public Monster
{
  public:
    float spawn_x;
    float spawn_y;
    float unknown1;
    float attack_proximity_y; // how close you have to be to be attacked; set both to 0 for a pet Anubis
    float attack_proximity_x;
    uint8_t ai_timer;
    uint8_t next_attack_timer;
    uint8_t psychic_orbs_counter;
    bool awake;

    virtual void set_next_attack_timer() = 0; // sets next_attack_timer based on the psychic_orbs_counter
    virtual void normal_attack() = 0;
    virtual void play_attack_sound() = 0;
};

class Cobra : public Monster
{
  public:
    uint32_t spit_timer;          // alternates between counting to 100 and to 64
    int32_t duplicate_move_state; // 6 is put in movable.move_state as well as in here while spitting poison
};

class CatMummy : public Monster
{
  public:
    uint8_t jump_height_multiplier; // Set in process_input when jump is triggered to be used when applying velocity for the jump.
    uint8_t attack_timer;
};

class Sorceress : public WalkingMonster
{
  public:
    uint32_t inbetween_attack_timer;
    float in_air_timer;               // why float?
    Illumination* halo_emitted_light; // not the shot but the halo, try putting a color at 10
    Entity* fx_entity;
    SoundMeta* sound;
    uint8_t hover_timer;
};

class MagmaMan : public Monster
{
  public:
    Illumination* emitted_light;
    SoundMeta* sound;
    ParticleEmitterInfo* particle;
    uint32_t jump_timer;  // when 0 it jumps
    uint32_t alive_timer; // when 0, the magmaman disappears
};

class Bee : public Monster
{
  public:
    bool can_rest;
    uint8_t padding1;
    uint16_t padding2;
    uint32_t padding3;
    SoundMeta* sound;
    uint16_t fly_hang_timer; // When standing or clinging to a wall, controls the time before flying off. When flying, controls the time before changing direction.
    uint8_t wobble_timer;    // 4-frame timer to choose another random wobble.
    uint8_t targeting_timer; // Timer while flying before the bee can land.
    uint8_t walk_start_time; // While in the standing state, will start walking when the fly_hang_timer gets down to this value.
    uint8_t walk_end_time;   // While in the standing state, will stop walking when the fly_hang_timer gets down to this value.
    uint8_t padding4;
    uint8_t padding5; // padding? quite a lot of unused memory in this entity, maybe this is more the one type?
    float wobble_x;   // maybe the positional offset to make it look like it's buzzing
    float wobble_y;
};

class Ammit : public Monster
{
  public:
    uint32_t walk_pause_timer; // alternates between walking and pausing every time it reaches zero
    uint32_t padding;          // not sure?
    ParticleEmitterInfo* particle;
};

class ApepPart : public Monster
{
  public:
    float y_pos;
    float sine_angle;
    /// or pause timer, used to sync the body parts moving up and down
    uint8_t sync_timer;
    uint8_t padding1;
    uint16_t padding2;
    uint32_t padding3;
};

class ApepHead : public ApepPart
{
  public:
    SoundMeta* sound1;
    SoundMeta* sound2;
    float distance_traveled;
    uint32_t tail_uid;
    uint32_t fx_mouthpiece1_uid;
    uint32_t fx_mouthpiece2_uid;
};

class OsirisHead : public Monster
{
  public:
    /// right from his perspective
    int32_t right_hand_uid;
    int32_t left_hand_uid;
    bool moving_left;
    uint8_t oscillation_phase;
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
    uint16_t jump_timer; // when 0 it jumps
    uint8_t shudder_timer; // Will make sound and shudder during the duration of this timer while falling.
    uint8_t leg_shake_timer; // Will make a leg shake animation for the duration of this timer while falling.
};

class UFO : public Monster
{
  public:
    SoundMeta* sound;
    int16_t patrol_distance; // pos = right, neg = left
    uint8_t attack_cooldown_timer;
    bool is_rising;
};

class Lahamu : public Monster
{
  public:
    SoundMeta* sound;
    Entity* eyeball;
    uint16_t attack_cooldown_timer;
    bool has_logged_to_journal;
};

class YetiQueen : public Monster
{
  public:
    uint32_t walk_pause_timer; // alternates between walking and pausing every time it reaches zero
    uint8_t unknown_timer;

    virtual void v_101() = 0;                  // can't trigger
    virtual void attack_related(float[4]) = 0; // parameter is some struct, if disabled it doesn't do jump attack, but still does the protect head thing
    virtual void v_103() = 0;                  // return
    virtual void jump_related() = 0;           // if disabled she squads but never makes the jump
    virtual void on_death() = 0;               // spawns the drops
};

// TODO: YetiKing should inherence from YetiQueen, should probably change the name of YetiQueen, since we don't know much about the functions and all, i leave it for someone in the future to deal with this xd
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

    virtual void v_101() = 0;                  // can't trigger
    virtual void attack_related(float[4]) = 0; // parameter is some struct, if disabled it doesn't do jump attack, but still does the protect head thing
    virtual void on_attack() = 0;              // freezes stuff when attacks and spawns particles
    virtual void screem_related() = 0;         // if disabled he opens the mount but never screams
    virtual void on_death() = 0;               // spawns the drops
};

class Lamassu : public Monster
{
  public:
    SoundMeta* sound;
    Entity* attack_effect_entity;
    ParticleEmitterInfo* particle;
    Illumination* emitted_light;
    uint32_t walk_pause_timer; // alternates between walking and pausing every time it reaches zero
    uint32_t flight_timer;     // when 0 it starts flying; only counts down when it's walking
    int32_t attack_timer;      // greater than 0 = charging up; 0 = fire ; less than 0 = cooldown
    float attack_angle;
    bool was_flying;        // Used to return to the correct state after an attack.
};

class Olmite : public WalkingMonster
{
  public:
    bool armor_on;
    /// disables the attack, stun, lock's looking left flag between stack
    bool in_stack;
    /// is set to false couple frame after being detached from stack
    bool in_stack2;
    int8_t padding1;
    int32_t on_top_uid;
    float y_offset;                // same as y position when stacked
    uint8_t attack_cooldown_timer; // jump attacks can start when 0
    uint8_t unknown_countdown;
    uint16_t padding2;
};

class Tiamat : public Monster
{
  public:
    /// Turning into stone sound
    SoundMeta* sound;
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
    /// This is custom variable, you need [activate_tiamat_position_hack](#activate_tiamat_position_hack) to use it
    float attack_x;
    /// This is custom variable, you need [activate_tiamat_position_hack](#activate_tiamat_position_hack) to use it
    float attack_y;
};

class GiantFrog : public Monster
{
  public:
    Entity* door_front_layer;
    Entity* door_back_layer;
    Entity* platform;
    uint16_t attack_timer;
    uint8_t frogs_ejected_in_cycle; // ejects thee frogs in quick succession, then waits a longer time
    uint8_t invincibility_timer;
    uint8_t mouth_close_timer;
    /// opens the mouth and starts mouth_close_timer, used when detecting grub in the mouth area
    bool mouth_open_trigger;
};

class Frog : public Monster
{
  public:
    int32_t grub_being_eaten_uid;
    uint8_t jump_timer; // when 0 it jumps
    bool pause;         // not actually used?
    int16_t padding;
};

class FireFrog : public Frog
{
  public:
    SoundMeta* sound;
};

class Grub : public Monster
{
  public:
    float rotation_delta; // is it? i seen it in the code compared as a bit with 1, but not sure when it happens
    bool drop;
    bool rotation_direction; // Counter-clockwise if true.
    /// Delay after colliding into a wall before it will change its movement direction again.
    uint8_t wall_collision_cooldown;
    uint8_t padding1;
    uint16_t rotation_timer;   // Will randomly pick a new direction and angle when this timer elapses.
    uint16_t walk_pause_timer; // alternates between walking and pausing every time it reaches zero
    int16_t turn_into_fly_timer;
    uint16_t padding2;
    ParticleEmitterInfo* particle;
    SoundMeta* sound;
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
    SoundMeta* sound;
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
    SoundMeta* sound;
    uint8_t movement_related_timer_1;
    uint8_t movement_related_timer_2;
    uint8_t transparency;
    uint8_t padding3;
    uint16_t fadeout; // when 0, ghist fades out/dies

    virtual void on_body_destroyed() = 0; // clears level_flags "Angry ghist shopkeeper" and other stuff, then calls ->destroy()
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
    SoundMeta* sound;
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
    /// current floor level
    float y_level;
    uint16_t bounce_timer; // functionality depends on state, determines when it can bounce again, also how long velocity is applied for bouncing
    uint8_t fireball_timer;
    bool birdhead_defeated;
    bool snakehead_defeated;
    /// 1:  Will move to the left, 2: Birdhead emerged, 3: Snakehead emerged, 4: Top level arena reached, 5: Birdhead shot last - to alternate the heads shooting fireballs
    uint8_t hundun_flags;
    uint16_t padding;
    /// This is custom variable, you need [activate_hundun_hack](#activate_hundun_hack) to use it
    float y_limit;
    /// This is custom variable, you need [activate_hundun_hack](#activate_hundun_hack) to use it
    float rising_speed_x;
    /// This is custom variable, you need [activate_hundun_hack](#activate_hundun_hack) to use it
    float rising_speed_y;
    /// This is custom variable, you need [activate_hundun_hack](#activate_hundun_hack) to use it
    float bird_head_spawn_y;
    /// This is custom variable, you need [activate_hundun_hack](#activate_hundun_hack) to use it
    float snake_head_spawn_y;
};

class HundunHead : public Monster
{
  public:
    /// Position where the head will move on attack
    float attack_position_x;
    float attack_position_y;
    int32_t egg_crack_effect_uid;
    int32_t targeted_player_uid;
    /// also cooldown before attack
    uint16_t looking_for_target_timer;
    uint8_t invincibility_timer;
};

class MegaJellyfish : public Monster
{
  public:
    Entity* flipper1;
    Entity* flipper2;
    SoundMeta* sound;
    /// game checks if this uid, and two following exist, if not, the Jellyfish starts chasing player
    int32_t orb_uid;
    int32_t tail_bg_uid;
    float applied_velocity;
    float wagging_tail_counter; // float counter to determine position of wagging tail (0 = right ; pi = left)
    /// only applies to door-blocking one
    uint8_t flipper_distance;
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
    ENT_TYPE carried_entity_type;
    int32_t carried_entity_uid;
    uint16_t walk_spit_timer; // depending on movable.state either the countdown how many acid bubbles spat (3) or walking timer
    /// whether it is hidden behind the carried block or not, if true you can damage him
    bool is_active;
    bool is_inactive; // must be something else, since we already have is_active (or vice versa), gets triggered about half a second after is_active, maybe something like, can attack
    /// defaults to true, when toggled to false, a new carried item spawns
    bool spawn_new_carried_item;
    bool going_up; // Whether the hermit crab is moving up when climbing a pole.
};

class Necromancer : public WalkingMonster
{
  public:
    SoundMeta* sound;
    float red_skeleton_spawn_x;
    float red_skeleton_spawn_y;
    int32_t resurrection_uid;
    uint8_t target_layer;
    uint8_t resurrection_timer;
};

class ProtoShopkeeper : public Monster
{
  public:
    uint64_t unknown1;
    /// 1: "Headpulse/explosion related, 2: Walking, 3: Headpulse/explosion related, 4: Crawling, 6: Headpulse/explosion related
    uint8_t movement_state;
    uint8_t padding1;
    uint8_t padding2;
    uint8_t blowup_related;
    uint32_t animation_timer;
    uint16_t walk_pause_explode_timer; // alternates between walking and pausing; when stunned: crawl timer;  after crawling: head pulse timer (pulse frequency depended on timer)
    /// 0 = slow, 4 = fast
    uint8_t walking_speed;
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
    uint8_t holding_state;
    uint8_t padding1;
    uint8_t padding2;
    uint8_t padding3;
};

class CritterBeetle : public Critter
{
  public:
    /// used when he's getting eaten
    bool pause;
};

class CritterCrab : public Critter
{
  public:
    uint8_t walk_pause_timer; // alternates between walking and pausing every time it reaches zero
    bool walking_left;
    /// moves away from its target instead of towards it
    bool unfriendly;
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
    SoundMeta* sound;
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

    virtual float get_speed() = 0;
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
    SoundMeta* sound;
    float applied_hor_momentum;
    float applied_ver_momentum;
    /// moves away from its target instead of towards it
    bool unfriendly;
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

    virtual float get_speed() = 0;
};
