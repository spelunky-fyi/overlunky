#pragma once

#include "aliases.hpp"
#include <array>
#include <cstdint>

class Entity;

struct RobinHoodTableEntry
{
    uint32_t uid_plus_one;
    uint32_t padding;
    Entity* entity;
};

struct LightParams
{
    float red; // default = 1.0 (can go over 1.0 for oversaturation)
    float green;
    float blue;
    float size;
};

struct Illumination
{
    union
    {
        /// Table of light1, light2, ... etc.
        std::array<LightParams, 4> lights;
        struct
        {
            LightParams light1;
            LightParams light2;
            LightParams light3;
            ///It's rendered on objects around, not as an actual bright spot
            LightParams light4;
        };
    };
    float brightness;
    float brightness_multiplier;
    float light_pos_x;
    float light_pos_y;
    float offset_x;
    float offset_y;
    float distortion;
    int32_t entity_uid;
    uint32_t timer;
    union
    {
        /// see [flags.hpp](../src/game_api/flags.hpp) illumination_flags
        uint32_t flags;
        struct
        {
            uint8_t light_flags; // no reason to expose this

            /// Only one can be set: 1 - Follow camera, 2 - Follow Entity, 3 - Rectangle, full brightness
            /// Rectangle always uses light1, even when it's disabled in flags
            uint8_t type_flags;
            uint8_t layer;
            bool enabled;
        };
    };
};

struct InputMapping
{
    uint8_t jump;
    uint8_t attack;
    uint8_t bomb;
    uint8_t rope;
    uint8_t walk_run;
    uint8_t use_door_buy;
    uint8_t pause_menu;
    uint8_t journal;
    uint8_t left;
    uint8_t right;
    uint8_t up;
    uint8_t down;
};

struct PlayerSlot
{
    INPUTS buttons_gameplay;
    INPUTS buttons;
    uint32_t unknown1;
    InputMapping* input_mapping_keyboard;
    InputMapping* input_mapping_controller;
    uint8_t player_slot;
    bool is_participating;
    uint8_t unknown4; // padding most likely
    uint8_t unknown5; // padding most likely
    uint32_t unknown6;
};

struct PlayerSlotSettings
{
    bool controller_vibration;
    bool auto_run_enabled;
    bool controller_right_stick;
};

struct PlayerInputs
{
    union
    {
        std::array<PlayerSlot, MAX_PLAYERS> player_slots;
        struct
        {
            PlayerSlot player_slot_1;
            PlayerSlot player_slot_2;
            PlayerSlot player_slot_3;
            PlayerSlot player_slot_4;
        };
    };
    union
    {
        std::array<PlayerSlotSettings, MAX_PLAYERS> player_settings;
        struct
        {
            PlayerSlotSettings player_slot_1_settings;
            PlayerSlotSettings player_slot_2_settings;
            PlayerSlotSettings player_slot_3_settings;
            PlayerSlotSettings player_slot_4_settings;
        };
    };
};

struct QuestsInfo
{
    size_t unknown1; // the first six are pointers to small similar objects
    size_t unknown2; // that don't appear to change at all
    size_t unknown3;
    size_t unknown4;
    size_t unknown5;
    size_t unknown6;
    int8_t yang_state;
    int8_t jungle_sisters_flags; // flags! not state ; -1 = sisters angry
    int8_t van_horsing_state;
    int8_t sparrow_state;
    int8_t madame_tusk_state;
    int8_t beg_state;
    uint8_t unknown13;
    uint8_t unknown14;
    uint32_t unknown15;
    uint32_t unknown16;
};

struct Camera
{
    float bounds_left;
    float bounds_right;
    float bounds_bottom;
    float bounds_top;
    float adjusted_focus_x; // focus adjusted so camera doesn't show beyond borders
    float adjusted_focus_y;
    float calculated_focus_x; // forced values
    float calculated_focus_y;
    float focus_offset_x; // added to position of focused entity, if any
    float focus_offset_y;
    float unknown1; // does not get reset on level reload
    float focus_x;  // the unadjusted center point to focus the camera on; set this for full camera control when the focused entity = -1
    float focus_y;
    float unknown2;
    float vertical_pan; // set to a high number, like 5000 and the camera pans from top to bottom

    // to shake the camera, set shake_countdown_start and shake_countdown to the number of frames you want to shake for
    // set the shake amplitude, and control the direction with the multipliers, optionally add randomness by toggling uniform_shake

    uint32_t shake_countdown_start; // probably used to calculate the percentage of the amplitude when progressing through the shake
    uint32_t shake_countdown;
    float shake_amplitude;    // the amount of camera shake
    float shake_multiplier_x; // set to 0 to eliminate horizontal shake; negative inverts direction
    float shake_multiplier_y; // set to 0 to eliminate vertical shake; negative inverts direction
    bool uniform_shake;       // if false, the shake gets randomized a bit
    uint8_t padding1;
    uint8_t padding2;
    uint8_t padding3;
    int32_t focused_entity_uid; // if set to -1, you have free control over camera focus through focus_x, focus_y
    uint32_t unknown3;
    uint32_t unknown4;
    float inertia; // 0 = still; 1 = follow immediately
    uint32_t unknown5;
    uint32_t unknown6;
    uint32_t unknown7;
};

struct JournalProgressionSlot
{
    uint8_t unknown1;
    int8_t grid_position; // -1 = unassigned, will be assigned when opening the journal and gets the zoom effect
    uint8_t unknown3;
    uint8_t unknown4;
    ENT_TYPE entity;
    float x;
    float y;
    float angle;
};

struct ThemeProgression
{
    uint8_t count;
    uint8_t visited_themes[9];
};

struct ArenaConfigArenas // size: 40 bytes
{
    bool dwelling_1;
    bool dwelling_2;
    bool dwelling_3;
    bool dwelling_4;
    bool dwelling_5;
    bool jungle_1;
    bool jungle_2;
    bool jungle_3;
    bool jungle_4;
    bool jungle_5;
    bool volcana_1;
    bool volcana_2;
    bool volcana_3;
    bool volcana_4;
    bool volcana_5;
    bool tidepool_1;
    bool tidepool_2;
    bool tidepool_3;
    bool tidepool_4;
    bool tidepool_5;
    bool temple_1;
    bool temple_2;
    bool temple_3;
    bool temple_4;
    bool temple_5;
    bool icecaves_1;
    bool icecaves_2;
    bool icecaves_3;
    bool icecaves_4;
    bool icecaves_5;
    bool neobabylon_1;
    bool neobabylon_2;
    bool neobabylon_3;
    bool neobabylon_4;
    bool neobabylon_5;
    bool sunkencity_1;
    bool sunkencity_2;
    bool sunkencity_3;
    bool sunkencity_4;
    bool sunkencity_5;
};

struct ArenaConfigItems // size: 40 bytes
{
    bool rock;
    bool pot;
    bool bombbag;
    bool bombbox;
    bool ropepile;
    bool pickup_12bag;
    bool pickup_24bag;
    bool cooked_turkey;
    bool royal_jelly;
    bool torch;
    bool boomerang;
    bool machete;
    bool mattock;
    bool crossbow;
    bool webgun;
    bool freezeray;
    bool shotgun;
    bool camera;
    bool plasma_cannon;
    bool wooden_shield;
    bool metal_shield;
    bool teleporter;
    bool mine;
    bool snaptrap;
    bool paste;
    bool climbing_gloves;
    bool pitchers_mitt;
    bool spike_shoes;
    bool spring_shoes;
    bool parachute;
    bool cape;
    bool vlads_cape;
    bool jetpack;
    bool hoverpack;
    bool telepack;
    bool powerpack;
    bool excalibur;
    bool scepter;
    bool kapala;
    bool true_crown;
};

struct ArenaConfigEquippedItems // size: 40 bytes
{
    bool dummy1; // the other items have spaces here but are unused
    bool dummy2;
    bool dummy3;
    bool dummy4;
    bool dummy5;
    bool dummy6;
    bool dummy7;
    bool dummy8;
    bool dummy9;
    bool dummy10;
    bool dummy11;
    bool dummy12;
    bool dummy13;
    bool dummy14;
    bool dummy15;
    bool dummy16;
    bool dummy17;
    bool dummy18;
    bool dummy19;
    bool dummy20;
    bool dummy21;
    bool dummy22;
    bool dummy23;
    bool dummy24;
    bool paste;
    bool climbing_gloves;
    bool pitchers_mitt;
    bool spike_shoes;
    bool spring_shoes;
    bool parachute;
    bool dummy25;
    bool dummy26;
    bool dummy27;
    bool dummy28;
    bool dummy29;
    bool dummy30;
    bool dummy31;
    bool dummy32;
    bool kapala;
    bool scepter;
};

struct ArenaState
{
    uint32_t current_arena;
    std::array<uint8_t, 4> player_teams;
    uint8_t format;
    uint8_t ruleset;
    std::array<uint8_t, 4> player_lives;
    std::array<uint8_t, 4> player_totalwins;
    int8_t unknown9;
    std::array<bool, 4> player_won;
    uint8_t unknown14a;
    uint8_t unknown14b;
    uint8_t unknown14c;
    uint8_t unknown14d;
    uint8_t unknown14e;
    /// The menu selection for timer, default values 0..20 where 0 == 30 seconds, 19 == 10 minutes and 20 == infinite. Can go higher, although this will glitch the menu text. Actual time (seconds) = (state.arena.timer + 1) x 30
    uint8_t timer;
    uint8_t timer_ending;
    uint8_t wins;
    uint8_t lives;
    uint8_t unknown15;
    uint8_t unknown16;
    std::array<uint16_t, 4> player_idolheld_countdown;
    uint8_t health;
    uint8_t bombs;
    uint8_t ropes;
    uint8_t stun_time;
    uint8_t mount;
    uint8_t arena_select;
    ArenaConfigArenas arenas;
    uint8_t dark_level_chance;
    uint8_t crate_frequency;
    ArenaConfigItems items_enabled;
    ArenaConfigItems items_in_crate;
    int8_t held_item;
    int8_t equipped_backitem;
    ArenaConfigEquippedItems equipped_items;
    uint8_t whip_damage;
    bool final_ghost;
    uint8_t breath_cooldown;
    bool punish_ball;
};

class Logic
{
  public:
    uint32_t logic_index; // array index into state.logic, where this instance resides
    uint32_t padding;

    virtual ~Logic() = 0;

    // Continuously performs the main functionality of the logic instance
    // Tutorial: handles dropping of the torch and rope in intro routine
    // Ouroboros: transitions to level when music finished
    // Basecamp speedrun: keep track of time, player position passing official
    // Ghost trigger: spawns ghost when time is up (checks cursed for earlier ghost)
    // Ghost toast trigger: shows the 'A terrible chill...' toast after 90 frames
    // Tun aggro: spawns Tun after 30 seconds
    // Dice shop: runs the logic of the dice shop
    // Tun pre challenge: unknown
    // Moon challenge: handles waitroom forcefields + tracks mattock breakage
    // Star challenge: handles waitroom forcefields + tracks torches/timer
    // Sun challenge: handles waitroom forcefields + tracks timer
    // Volcana/Lava: spawns magmamen at random (and does other things)
    // Water: unknown
    // Olmec cutscene: runs the cutscene
    // Tiamat cutscene: runs the cutscene
    // Apep trigger: tracks player position, spawns APEP_HEAD
    // COG Ankh sacrifice: countdown timer (100 frames) from the moment you die, triggers transitioning to duat
    // Duat bosses trigger: tracks player position, spawns ANUBIS2 and OSIRIS_HEAD
    // Tiamat: spawns bubbles
    // Tusk pleasure palace: triggers aggro on everyone when non-high roller enters door
    // Discovery: shows the toast
    // Black market: lifts camera bounds restrictions
    // Cosmic ocean: spawns jelly when time is up
    // Arena 1: handles crate spawning
    // Arena 3: handles death mist
    virtual void perform() = 0;
};

class LogicOuroboros : public Logic
{
  public:
    size_t unknown3; // sound related?
    uint16_t timer;

    virtual ~LogicOuroboros() = 0;
};

class LogicBasecampSpeedrun : public Logic
{
  public:
    uint32_t official; // entity uid of the character that keeps the time
    uint32_t crate;    // entity uid; you must break this crate for the run to be valid, otherwise you're cheating
    uint32_t unknown3;
    uint32_t unknown4;

    virtual ~LogicBasecampSpeedrun() = 0;
};

class LogicGhostToast : public Logic
{
  public:
    uint32_t toast_timer;

    virtual ~LogicGhostToast() = 0;
};

class LogicDiceShop : public Logic
{
  public:
    uint32_t boss; // entity uid; either tusk or the shopkeeper
    uint32_t unknown4;
    uint32_t bet_machine; // entity uid
    uint32_t die1;        // entity uid
    uint32_t die2;        // entity uid
    uint8_t die_1_value;
    uint8_t die_2_value;
    uint16_t unknown8;
    uint32_t prize_dispenser; // entity uid
    uint32_t prize;           // entity uid
    uint32_t forcefield;      // entity uid
    bool bet_active;
    bool forcefield_deactivated;
    bool boss_angry;
    uint8_t result_announcement_timer; // the time the boss waits after your second die throw to announce the results
    uint8_t won_prizes_count;          // to see whether you achieved high roller status
    uint8_t unknown14;
    uint8_t unknown15;
    uint8_t unknown16;
    uint32_t balance; // cash balance of all the games

    virtual ~LogicDiceShop() = 0;
};

class LogicMoonChallenge : public Logic
{
  public:
    uint32_t unknown3;
    uint32_t unknown4;
    uint32_t floor_challenge_entrance_uid;
    uint32_t floor_challenge_waitroom_uid;
    bool challenge_active;
    uint8_t forcefield_countdown; // waiting area forcefield activation timer (the one that locks you in)
    uint16_t unknown7;
    uint16_t unknown8a;
    uint16_t unknown8b;
    uint32_t mattock; // entity uid

    virtual ~LogicMoonChallenge() = 0;
};

class LogicStarChallenge : public Logic
{
  public:
    uint32_t unknown3;
    uint32_t unknown4;
    uint32_t floor_challenge_entrance_uid;
    uint32_t floor_challenge_waitroom_uid;
    bool challenge_active;
    uint8_t forcefield_countdown; // waiting area forcefield activation timer (the one that locks you in)
    uint16_t unknown7;
    uint32_t unknown8;
    std::vector<Entity*> torches;
    uint32_t start_countdown;

    virtual ~LogicStarChallenge() = 0;
};

class LogicSunChallenge : public Logic
{
  public:
    uint32_t unknown3;
    uint32_t unknown4;
    uint32_t floor_challenge_entrance_uid;
    uint32_t floor_challenge_waitroom_uid;
    bool challenge_active;
    uint8_t forcefield_countdown; // waiting area forcefield activation timer (the one that locks you in)
    uint16_t unknown7;
    uint32_t unknown8;
    uint8_t start_countdown;

    virtual ~LogicSunChallenge() = 0;
};

class LogicOlmecCutscene : public Logic
{
  public:
    uint8_t unknown6a;
    uint8_t unknown6b;
    uint8_t unknown6c;
    uint8_t unknown6d;
    uint8_t unknown7a;
    uint8_t unknown7b;
    uint8_t unknown7c;
    uint8_t unknown7d;
    Entity* fx_olmecpart_large;
    Entity* olmec;
    Entity* player;
    Entity* cinematic_anchor;
    uint32_t timer;

    virtual ~LogicOlmecCutscene() = 0;
};

class LogicTiamatCutscene : public Logic
{
  public:
    uint32_t unknown3;
    uint32_t unknown4;
    Entity* tiamat;
    Entity* player;
    Entity* cinematic_anchor;
    uint32_t timer;
    int32_t unknown5;
    uint32_t unknown6;
    uint32_t unknown7;

    virtual ~LogicTiamatCutscene() = 0;
};

class LogicApepTrigger : public Logic
{
  public:
    uint32_t spawn_cooldown;
    bool cooling_down;
    bool unknown4b;
    uint32_t unknown4c;
    uint32_t unknown4d;
    uint32_t unknown5;
    uint32_t unknown6;

    virtual ~LogicApepTrigger() = 0;
};

class LogicCOGAnkhSacrifice : public Logic
{
    uint8_t unknown3;
    uint8_t timer;

    virtual ~LogicCOGAnkhSacrifice() = 0;
};

class LogicDuatBossesTrigger : public Logic
{
  public:
    virtual ~LogicDuatBossesTrigger() = 0;
};

class LogicTuskPleasurePalace : public Logic
{
  public:
    uint32_t locked_door; // entity uid
    uint32_t unknown4;

    virtual ~LogicTuskPleasurePalace() = 0;
};

class LogicArena1 : public Logic
{
  public:
    uint32_t crate_spawn_timer;
    uint32_t unknown4;
    uint32_t unknown5;
    uint32_t unknown6;

    virtual ~LogicArena1() = 0;
};

class LogicArenaAlienBlast : public Logic
{
  public:
    uint32_t timer;

    virtual ~LogicArenaAlienBlast() = 0;
};

class LogicArenaLooseBombs : public Logic
{
  public:
    uint32_t timer;

    virtual ~LogicArenaLooseBombs() = 0;
};

struct LogicList
{
    Logic* tutorial;
    LogicOuroboros* ouroboros;
    LogicBasecampSpeedrun* basecamp_speedrun;
    Logic* ghost_trigger;
    LogicGhostToast* ghost_toast_trigger;
    Logic* tun_aggro;
    LogicDiceShop* diceshop;
    Logic* tun_pre_challenge;
    LogicMoonChallenge* tun_moon_challenge;
    LogicStarChallenge* tun_star_challenge;
    LogicSunChallenge* tun_sun_challenge;
    Logic* volcana_related;
    Logic* water_related;
    LogicOlmecCutscene* olmec_cutscene;
    LogicTiamatCutscene* tiamat_cutscene;
    LogicApepTrigger* apep_trigger;
    LogicCOGAnkhSacrifice* city_of_gold_ankh_sacrifice;
    LogicDuatBossesTrigger* duat_bosses_trigger;
    Logic* tiamat;
    LogicTuskPleasurePalace* tusk_pleasure_palace;
    Logic* discovery_info; // black market, vlad, wet fur discovery; shows the toast
    Logic* black_market;
    Logic* cosmic_ocean;
    LogicArena1* arena_1;
    Logic* arena_2;
    Logic* arena_3;
    LogicArenaAlienBlast* arena_alien_blast;
    LogicArenaLooseBombs* arena_loose_bombs;
};

struct LiquidPhysicsEngine
{
    bool pause_physics;
    uint8_t padding[3];
    int32_t physics_tick_timer; /* unsure */
    int32_t unknown1;
    int32_t unknown2;
    int8_t unknown3;
    int8_t unknown4;
    int8_t unknown5;
    int8_t unknown6;
    int8_t unknown_7;
    int8_t unknown8;
    int8_t unknown9;
    int8_t unknown10;
    uint32_t unknown11;
    float unknown12;
    float blob_size;
    float weight;
    float unknown15;
    uint32_t entity_count;
    uint32_t allocated_size;
    uint32_t unk23;         // padding probably
    std::list<size_t> unk1; //seams to be empty, or have one element 0?
    uint32_t resize_value;  // used to resive the arrays?
    uint32_t unk3b;         // padding probably
    std::list<int32_t> liquid_ids;
    std::list<int32_t> unknown44;                        // all of them are -1
    std::list<int32_t>::const_iterator* list_liquid_ids; // list of all iterators of liquid_ids?
    int32_t unknown45a;                                  // size related for the array above
    int32_t unknown45b;                                  // padding
    uint32_t* liquid_flags;                              // array
    int32_t unknown47a;                                  // size related for the array above
    int32_t unknown47b;                                  // padding
    std::pair<float, float>* entity_coordinates;         // array
    int32_t unknown49a;                                  // size related for the array above
    int32_t unknown49b;                                  //padding
    std::pair<float, float>* entity_velocities;          // array
    int32_t unknown51a;                                  // size related for the array above
    int32_t unknown51b;                                  //padding
    std::pair<float, float>* unknown52;                  // not sure about the type, it's defenetly a 64bit
    std::pair<float, float>* unknown53;
    size_t unknown54;
    std::pair<float, float>* unknown55;
    // 3 more size_t here
};

struct LiquidPhysicsParams
{
    int32_t shader_type; //can also be flags, as for water, any value with bit one is fine
    uint8_t unknown2;    //shader related, shader id maybe?
    uint8_t padding1;
    uint8_t padding2;
    uint8_t padding3;
    float unknown3;
    float cohesion; // negative number makes the liquid balls come apart more easily?
    float gravity;  // negative number to invert gravity
    float unknown6;
    float unknown7;
    float agitation; // is agitation the right word? for me is just how bouncy the liquid is
    float unknown9;  // starts going nuts at around 2.70, pressure force? it seam to only matter at spawn, when there is a lot of liquid in one place
    float unknown10;
    float unknown11;
    float unknown12;
    float unknown13;
    float unknown14;
    float unknown15;
    float unknown16;
    float unknown17;
    float unknown18;
    float unknown19;
    uint32_t unknown20;
    float unknown21;
    uint32_t unknown22;
    float unknown23;
    uint32_t unknown24;
};

struct LiquidTileSpawnData
{
    uint32_t liquid_flags; // 2 - lava_interaction? crashes the game if no lava is present, 3 - pause_physics, 6 - low_agitation?, 7 - high_agitation?, 8 - high_surface_tension?, 9 - low_surface_tension?, 11 - high_bounce?, 12 - low_bounce?
    float last_spawn_x;
    float last_spawn_y;
    float spawn_velocity_x;
    float spawn_velocity_y;
    uint32_t unknown31;
    uint32_t unknown32;
    uint32_t unknown33;
    size_t unknown34;                  // MysteryLiquidPointer2 in plugin, contains last spawn entity
    size_t unknown35;                  // DataPointer? seam to get access validation if you change to something
    uint32_t liquidtile_liquid_amount; // how much liquid will be spawned from tilecode, 1=1x2, 2=2x3, 3=3x4 etc.
    float blobs_separation;
    int32_t unknown39; //is the last 4 garbage? seams not accessed
    float unknown40;
    float unknown41;
    uint32_t unknown42;
};

struct LiquidPhysics
{
    size_t unknown1; // MysteryLiquidPointer1 in plugin
    union
    {
        struct
        {
            LiquidPhysicsParams physics_defaults;
            LiquidPhysicsEngine* physics_engine;
            LiquidTileSpawnData tile_spawn_data;
        } pools[5];
        struct
        {
            LiquidPhysicsParams water_physics_defaults;
            LiquidPhysicsEngine* water_physics_engine;
            LiquidTileSpawnData water_tile_spawn_data;
            LiquidPhysicsParams coarse_water_physics_defaults;
            LiquidPhysicsEngine* coarse_water_physics_engine;
            LiquidTileSpawnData coarse_water_tile_spawn_data;
            LiquidPhysicsParams lava_physics_defaults;
            LiquidPhysicsEngine* lava_physics_engine;
            LiquidTileSpawnData lava_tile_spawn_data;
            LiquidPhysicsParams coarse_lava_physics_defaults;
            LiquidPhysicsEngine* coarse_lava_physics_engine;
            LiquidTileSpawnData coarse_lava_tile_spawn_data;
            LiquidPhysicsParams stagnant_lava_physics_defaults;
            LiquidPhysicsEngine* stagnant_lava_physics_engine;
            LiquidTileSpawnData stagnant_lava_tile_spawn_data;
        };
    };
};

struct AITarget
{
    uint32_t ai_uid;
    uint32_t target_uid;
};

struct DialogueDetails
{
    STRINGID line;          // strings table ID
    uint8_t param_player1;  // character db ID (although they don't always match up with the in game dialogue)
    uint8_t param_player2;  // for %-replacement in line
    uint8_t unknown3;       // most likely padding
    uint8_t unknown4;       // most likely padding
    STRINGID param_string1; // strings table ID for %-replacement in line
    STRINGID param_string2;
};

struct DialogueDetailsGroup
{
    DialogueDetails ana_spelunky;
    DialogueDetails margaret_tunnel;
    DialogueDetails colin_northward;
    DialogueDetails roffy_d_sloth;
    DialogueDetails alto_singh;
    DialogueDetails liz_mutton;
    DialogueDetails nekka_the_eagle;
    DialogueDetails lise_project;
    DialogueDetails coco_von_diamonds;
    DialogueDetails manfred_tunnel;
    DialogueDetails little_jay;
    DialogueDetails tina_flan;
    DialogueDetails valerie_crump;
    DialogueDetails au;
    DialogueDetails demi_von_diamonds;
    DialogueDetails pilot;
    DialogueDetails princess_airyn;
    DialogueDetails dirk_yamaoka;
    DialogueDetails guy_spelunky;
    DialogueDetails classic_guy;
    DialogueDetails mama_tunnel;
};

#pragma pack(push, 1) // disable struct padding because of that straggling uint8_t at the end
struct DialogueInteraction
{
    bool interacted;
    uint8_t unknown1;
    uint8_t unknown2;
    uint8_t unknown3;
    uint32_t unknown4;
    uint32_t unknown5;
    uint32_t unknown6;
    uint32_t unknown7;
    uint8_t unknown8;
};
#pragma pack(pop)

struct DialogueInteractionGroup
{
    DialogueInteraction ana_spelunky;
    DialogueInteraction margaret_tunnel;
    DialogueInteraction colin_northward;
    DialogueInteraction roffy_d_sloth;
    DialogueInteraction alto_singh;
    DialogueInteraction liz_mutton;
    DialogueInteraction nekka_the_eagle;
    DialogueInteraction lise_project;
    DialogueInteraction coco_von_diamonds;
    DialogueInteraction manfred_tunnel;
    DialogueInteraction little_jay;
    DialogueInteraction tina_flan;
    DialogueInteraction valerie_crump;
    DialogueInteraction au;
    DialogueInteraction demi_von_diamonds;
    DialogueInteraction pilot;
    DialogueInteraction princess_airyn;
    DialogueInteraction dirk_yamaoka;
    DialogueInteraction guy_spelunky;
    DialogueInteraction classic_guy;
    DialogueInteraction mama_tunnel;
};

struct Dialogue
{
    DialogueDetailsGroup base_dialogue;
    DialogueDetailsGroup extra_dialogue;
    DialogueDetailsGroup win_dialogue;
    DialogueInteractionGroup interaction1;
    DialogueInteractionGroup interaction2;
    uint8_t lise_calculated_victory_chance; // random number 1-100 used in the 'Calculating the change of victory on your next run' line
    uint8_t padding1;
    uint32_t win_flags;             // see flags.hpp ; character saved -> triggers the 'Thanks for getting me out of there' line
    bool trigger_win_dialogue;      // set before entering basecamp
    bool trigger_progress_dialogue; // set before entering basecamp
    uint16_t win_related;
    uint32_t unknown1;
    uint32_t unknown2;
    uint32_t unknown3;
    uint32_t unknown4;
    uint32_t unknown5;
    uint32_t unknown6;
    uint32_t unknown7;
    uint32_t unknown8;
    uint32_t unknown9;
    uint32_t unknown10;
    uint32_t unknown11;
    uint32_t unknown12;
    uint32_t unknown13;
    uint32_t unknown14;
    uint32_t unknown15;
    uint32_t unknown16;
    uint32_t unknown17;
    uint32_t unknown18;
};

struct SelectPlayerSlot
{
    bool activated;
    uint8_t padding1;
    uint8_t padding2;
    uint8_t padding3;
    ENT_TYPE character;  // Entity DB ID
    uint32_t texture_id; // Texture DB ID
};
