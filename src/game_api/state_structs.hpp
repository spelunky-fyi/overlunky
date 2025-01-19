#pragma once

#include "aliases.hpp"
#include "containers/custom_map.hpp"
#include "containers/custom_vector.hpp"
#include "layer.hpp"
#include "math.h" // for AABB, Vec2
#include "render_api.hpp"
#include <array>
#include <cstdint>
#include <map>

class Entity;
struct SoundMeta;

struct RobinHoodTableEntry
{
    uint32_t uid_plus_one;
    uint32_t padding;
    Entity* entity;
};

struct InputMapping
{
    union
    {
        struct
        {
            int8_t jump;
            int8_t attack;
            int8_t bomb;
            int8_t rope;
            int8_t walk_run;
            int8_t use_door_buy;
            int8_t pause_menu;
            int8_t journal;
            int8_t left;
            int8_t right;
            int8_t up;
            int8_t down;
        };
        /// Can be indexed with INPUT_FLAG. Keyboard uses RAW_KEY values, controller uses RAW_BUTTON values.
        std::array<RAW_KEY, 12> mapping;
    };
};

struct PlayerSlot
{
    INPUTS buttons_gameplay;
    INPUTS buttons;
    uint32_t unknown1;
    InputMapping* input_mapping_keyboard;
    InputMapping* input_mapping_controller;
    int8_t player_slot;
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

struct QuestLogic
{
    // used to access flags in QuestsInfo
    uint32_t id;

    // called during layer loading by the state update, sets the level generation flags etc.
    virtual void pre_level_gen() = 0;

    // Gets called on every non-CO level by 18 virtual in current theme. Function checks whether the quest needs to be initialized.
    // Yang: checks whether his quest status = 1 -> makes his pen work
    // Sisters: make them appear on Olmec
    // Horsing: makes him appear in Vlad's castle
    // Sparrow: nop
    // Tusk: populates pleasure palace with other characters depending on their quest state
    // Beg: nop
    virtual void post_level_gen() = 0;

    // Only implemented for Beg quest, called by the 24 virtual in current theme
    virtual void post_level_gen2() = 0;

    virtual ~QuestLogic() = delete;
};

struct QuestsInfo
{
    QuestLogic* yang;
    QuestLogic* jungle_sisters;
    QuestLogic* van_horsing;
    QuestLogic* sparrow;
    QuestLogic* madame_tusk;
    QuestLogic* beg;
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
    float adjusted_focus_x; // focus adjusted so camera doesn't show beyond borders, can be updated manually by setting inertia to 5 and calling update_camera_position(), updated per frame
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
    uint8_t padding1[3];
    /// if set to -1, you have free control over camera focus through focus_x, focus_y
    int32_t focused_entity_uid;
    /// amount of frames to freeze camera in place and move to the peek_layer
    /// during the peek you can freely set camera position no matter if focused_entity_uid is set to -1 or not
    uint32_t peek_timer;
    uint8_t peek_layer;
    uint8_t padding2[3];

    /// This is a bad name, but it represents the camera tweening speed. [0..5] where 0=still, 1=default (move 20% of distance per frame), 5=max (move 5*20% or 100% aka instantly to destination per frame)
    float inertia;
    uint32_t unknown5;

    AABB get_bounds() const
    {
        return AABB(bounds_left, bounds_top, bounds_right, bounds_bottom);
    }
    void set_bounds(const AABB& bounds)
    {
        bounds_left = bounds.left;
        bounds_right = bounds.right;
        bounds_bottom = bounds.bottom;
        bounds_top = bounds.top;
    }
    static Vec2 get_position();
    void set_position(float cx, float cy);
    void update_position();
};

struct JournalProgressStickerSlot
{
    uint8_t theme;
    int8_t grid_position; // -1 = unassigned, will be assigned when opening the journal and gets the zoom effect
    uint8_t unknown3;
    uint8_t unknown4;
    ENT_TYPE entity_type;
    float x;
    float y;
    float angle;
};

struct JournalProgressStainSlot
{
    float x;
    float y;
    float angle;
    float scale;
    int32_t texture_column;
    int32_t texture_row;
    uint8_t texture_range; // when stain is first displayed, rolls [0, texture_range] and adds it to texture_column
    uint8_t padding[3];
};

struct ArenaConfigArenas // size: 40 bytes
{
    union
    {
        std::array<bool, 40> list;
        struct
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
    };
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
    uint8_t unknown14a; // player1_bot_ai
    uint8_t unknown14b; // player2_bot_ai
    uint8_t unknown14c; // player3_bot_ai
    uint8_t unknown14d; // player4_bot_ai
    uint8_t unknown14e;
    /// The menu selection for timer, default values 0..20 where 0 == 30 seconds, 19 == 10 minutes and 20 == infinite. Can go higher, although this will glitch the menu text. Actual time (seconds) = (state.arena.timer + 1) x 30
    uint8_t timer;
    uint8_t timer_ending;
    uint8_t wins;
    uint8_t lives;
    uint8_t time_to_win;
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

enum class LOGIC : uint32_t
{
    TUTORIAL = 0,
    OUROBOROS,
    SPEEDRUN,
    GHOST,
    GHOST_TOAST,
    TUN_AGGRO,
    DICESHOP,
    PRE_CHALLENGE,
    MOON_CHALLENGE,
    STAR_CHALLENGE,
    SUN_CHALLENGE,
    MAGMAMAN_SPAWN,
    WATER_BUBBLES,
    OLMEC_CUTSCENE,
    TIAMAT_CUTSCENE,
    APEP,
    COG_SACRIFICE,
    DUAT_BOSSES,
    BUBBLER,
    PLEASURE_PALACE,
    DISCOVERY_INFO,
    BLACK_MARKET,
    JELLYFISH,
    ARENA_1,
    ARENA_2,
    ARENA_3,
    ARENA_ALIEN_BLAST,
    ARENA_LOOSE_BOMBS,
};

class Logic
{
  public:
    LOGIC logic_index;
    uint32_t unused_padding;

    virtual ~Logic() = 0;

    // Continuously performs the main functionality of the logic instance
    // If it returns false, game will call deconstructor next in most cases
    virtual bool perform() = 0;
};

class LogicOuroboros : public Logic
{
  public:
    SoundMeta* sound;
    uint32_t timer;
};

class LogicBasecampSpeedrun : public Logic
{
  public:
    /// entity uid of the character that keeps the time
    uint32_t administrator;
    /// entity uid. you must break this crate for the run to be valid, otherwise you're cheating
    uint32_t crate;
};

class LogicGhostToast : public Logic
{
  public:
    ///  default 90
    uint32_t toast_timer;
};

class LogicDiceShop : public Logic
{
  public:
    uint32_t boss_uid;
    ENT_TYPE boss_type;
    /// entity uid
    uint32_t bet_machine;
    /// entity uid
    uint32_t die1;
    /// entity uid
    uint32_t die2;
    int8_t die_1_value;
    int8_t die_2_value;
    uint16_t unknown8;
    /// entity uid
    uint32_t prize_dispenser;
    /// entity uid
    uint32_t prize;
    /// entity uid
    uint32_t forcefield;
    bool bet_active;
    bool forcefield_deactivated;
    bool unknown;
    /// the time the boss waits after your second die throw to announce the results
    uint8_t result_announcement_timer;
    uint8_t won_prizes_count;
    uint8_t padding[3];
    /// cash balance of all the games
    int32_t balance;
};

class LogicChallenge : public Logic
{
  public:
    uint32_t unknown3;
    uint32_t unknown4;
    uint32_t floor_challenge_entrance_uid;
    uint32_t floor_challenge_waitroom_uid;
    bool challenge_active;
    uint8_t forcefield_countdown; // waiting area forcefield activation timer (the one that locks you in)
    uint16_t padding1;
    uint32_t padding2;
};

class LogicMoonChallenge : public LogicChallenge
{
  public:
    /// entity uid
    int32_t mattock_uid;
};

class LogicStarChallenge : public LogicChallenge
{
  public:
    std::vector<Entity*> torches; // TODO: check if custom vector (probably yes)
    uint8_t start_countdown;
    uint8_t padding[3];
    uint32_t unknown9;
    float unknown10; // position in front of tun and one tile higher, dunno what for?
    float unknown11; // kind of would make sense for the wanted poster, but you get this struct after you buy the challenge, not possible when tun is angry?
};

class LogicSunChallenge : public LogicChallenge
{
  public:
    uint8_t start_countdown;
    uint8_t padding[3];
    uint32_t unknown9;
    float unknown10; // same as for LogicStarChallenge
    float unknown11;
};

class MagmamanSpawnPosition
{
  public:
    uint32_t x;
    uint32_t y;
    uint32_t timer;

    MagmamanSpawnPosition(uint32_t x_, uint32_t y_);
};

class LogicMagmamanSpawn : public Logic
{
  public:
    custom_vector<MagmamanSpawnPosition> magmaman_positions;

    void add_spawn(uint32_t x, uint32_t y)
    {
        magmaman_positions.emplace_back(x, y);
    }
    void add_spawn(MagmamanSpawnPosition ms)
    {
        add_spawn(ms.x, ms.y);
    };
    void remove_spawn(uint32_t x, uint32_t y);
    void remove_spawn(MagmamanSpawnPosition ms)
    {
        remove_spawn(ms.x, ms.y);
    };
};

class LogicOlmecCutscene : public Logic
{
  public:
    /// Copied over [buttons_gameplay](#PlayerSlot) from the leader, used to skip the cutscene
    /// You can skip the cutscene if you set it to 1 or 4
    uint8_t leader_inputs;
    uint8_t padding[7];
    Entity* fx_olmecpart_large;
    Entity* olmec;
    Entity* player;
    Entity* cinematic_anchor;
    uint32_t timer;
};

class LogicTiamatCutscene : public Logic
{
  public:
    /// Copied over [buttons_gameplay](#PlayerSlot) from the leader, used to skip the cutscene
    /// You can skip the cutscene if you set it to 1 or 4
    uint8_t leader_inputs;
    uint8_t padding[7];
    Entity* tiamat;
    Entity* player;
    Entity* cinematic_anchor;
    uint32_t timer;
};

class LogicApepTrigger : public Logic
{
  public:
    uint32_t spawn_cooldown;
    bool cooling_down;
    bool apep_journal_entry_logged;
};

class LogicCOGAnkhSacrifice : public Logic
{
  public:
    uint8_t unknown3;
    uint8_t timer;
};

class LogicTiamatBubbles : public Logic
{
  public:
    uint8_t bubble_spawn_timer;
};

class LogicTuskPleasurePalace : public Logic
{
  public:
    int32_t locked_door; // entity uid
    uint32_t unknown4;   // default 1552
    uint32_t unknown5;   // dunno
    uint32_t unknown6;   // padding probably
};

class LogicArena1 : public Logic
{
  public:
    uint32_t crate_spawn_timer;
};

class LogicArenaAlienBlast : public Logic
{
  public:
    uint32_t timer;
};

class LogicArenaLooseBombs : public Logic
{
  public:
    uint32_t timer;
};

class LogicUnderwaterBubbles : public Logic
{
  public:
    /// 1.0 = normal, -1.0 = inversed, other values have undefined behavior
    /// this value basically have to be the same as return from `ThemeInfo:get_liquid_gravity()`
    float gravity_direction;

    /// It's inverse chance, so the lower the number the higher the chance, values below 10 may crash the game
    int16_t droplets_spawn_chance;
    /// Enable/disable spawn of ENT_TYPE.FX_WATER_DROP from ceiling (or ground if liquid gravity is inverse)
    bool droplets_enabled;
};

class LogicTunPreChallenge : public Logic
{
  public:
    // except for Tun the rest of the values do not make any sense (garbage)
    // the logic.perform does only ever touches the tun as well, first one always 0?
    size_t unknown1;
    size_t unknown2;
    size_t unknown3;
    int32_t tun_uid;
};

class LogicTutorial : public Logic
{
  public:
    Entity* pet_tutorial;
    uint32_t timer;
};

struct LogicList
{
    /// This only properly constructs the base class
    /// you may still need to initialise the parameters correctly
    Logic* start_logic(LOGIC idx);
    void stop_logic(LOGIC idx);
    void stop_logic(Logic* log);

    union
    {
        std::array<Logic*, 28> logic_indexed;
        struct
        {
            /// Handles dropping of the torch and rope in intro routine (first time play)
            LogicTutorial* tutorial;
            LogicOuroboros* ouroboros;
            /// Keep track of time, player position passing official
            LogicBasecampSpeedrun* basecamp_speedrun;
            /// It's absence is the only reason why ghost doesn't spawn at boss levels or CO
            Logic* ghost_trigger; // virtual does nothing, all the code elsewhere, the only purpose is to mark if ghost should spawn this level or not
            LogicGhostToast* ghost_toast_trigger;
            /// Spawns tun at the door at 30s mark
            Logic* tun_aggro;
            LogicDiceShop* diceshop;
            LogicTunPreChallenge* tun_pre_challenge;
            LogicMoonChallenge* tun_moon_challenge;
            LogicStarChallenge* tun_star_challenge;
            LogicSunChallenge* tun_sun_challenge;
            LogicMagmamanSpawn* magmaman_spawn;
            /// Only the bubbles that spawn from the floor (no border tiles, checks decoration flag), also spawn droplets falling from ceiling
            /// Even without it, entities moving in water still spawn bubbles
            LogicUnderwaterBubbles* water_bubbles;
            LogicOlmecCutscene* olmec_cutscene;
            LogicTiamatCutscene* tiamat_cutscene;
            /// Triggers and spawns Apep only in rooms set as ROOM_TEMPLATE.APEP
            LogicApepTrigger* apep_spawner;
            /// All it does is it runs transition to Duat after time delay (sets the state next theme etc. and state.items for proper player respawn)
            LogicCOGAnkhSacrifice* city_of_gold_ankh_sacrifice;
            Logic* duat_bosses_spawner;
            /// Spawn rising bubbles at Tiamat (position hardcoded)
            LogicTiamatBubbles* bubbler;
            /// Triggers aggro on everyone when non-high roller enters door
            LogicTuskPleasurePalace* tusk_pleasure_palace; //  TODO: van helsing?
            /// black market, vlad, wet fur discovery, logic shows the toast
            Logic* discovery_info;
            /// Changes the camera bounds when you reach black market
            Logic* black_market;
            Logic* jellyfish_trigger; // same as ghost_trigger
            /// Handles create spawns and more, is cleared as soon as the winner is decided (on last player alive)
            LogicArena1* arena_1;
            Logic* arena_2; // can't trigger
            /// Handles time end death
            Logic* arena_3;
            LogicArenaAlienBlast* arena_alien_blast;
            LogicArenaLooseBombs* arena_loose_bombs;
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

struct ItemOwnerDetails
{
    int32_t owner_uid;
    ENT_TYPE owner_type;
};

struct RoomOwnerDetails
{
    uint8_t layer;
    uint8_t padding1;
    uint8_t padding2;
    uint8_t padding3;
    uint32_t room_index;
    int32_t owner_uid;
};

struct RoomOwnersInfo
{
    /// key/index is the uid of an item
    custom_map<int32_t, ItemOwnerDetails> owned_items;
    std::vector<RoomOwnerDetails> owned_rooms;
};

struct MultiLineTextRendering
{
    size_t* timer;                         // some struct? game increments this value and one at +0x40, seam to be related to rendering, touching just the first one freezes the game
    std::vector<TextRenderingInfo*> lines; // each line is separate TextRenderingInfo
    float x;                               // center of the text box?
    float y;                               // center of the text box?
};

struct EntityLookup
{
    std::array<EntityList, 4> unknown1;

    // this is either very strange vector or something unrelated
    // if this is vector, then it's just holds list of pointers to the elements from array above
    // the result of the lookup would then be last element: (unknown3 - 1)
    EntityList** unknown2; // always points to the first one?
    EntityList** unknown3; // if lookup is not used, it's the same as unknown4
    EntityList** unknown4; // capacity? points to nullptr after all the other pointers
};
