#pragma once

#include <array>   // for array
#include <cstddef> // for size_t
#include <cstdint> // for uint8_t, uint32_t, int32_t, int8_t, uin...
#include <utility> // for pair
#include <vector>  // for vector

#include "aliases.hpp"       // for ENT_TYPE, LAYER
#include "state_structs.hpp" // for JournalProgressionSlot, Illumination (p...

class Entity;
class ScreenArenaIntro;
class ScreenArenaItems;
class ScreenArenaLevel;
class ScreenArenaMenu;
class ScreenArenaScore;
class ScreenArenaStagesSelect;
class ScreenCamp;
class ScreenCharacterSelect;
class ScreenConstellation;
class ScreenCredits;
class ScreenDeath;
class ScreenLevel;
class ScreenRecap;
class ScreenScores;
class ScreenTeamSelect;
class ScreenTransition;
class ScreenWin;
struct ParticleEmitterInfo;

const float ZF = 0.737f;

using PAUSE = uint8_t;
using THEME = uint8_t;
using QUEST_FLAG = uint32_t;
using PRESENCE_FLAG = uint32_t;
using JOURNAL_FLAG = uint32_t;
using CAUSE_OF_DEATH = uint8_t;

struct SaveData;
struct Layer;
struct LevelGenSystem;
class ThemeInfo;
struct Items;

void fix_liquid_out_of_bounds();

#pragma pack(push, 1) // disable struct padding
struct StateMemory
{
    size_t p00;
    /// Previous SCREEN, used to check where we're coming from when loading another SCREEN
    uint32_t screen_last;
    /// Current SCREEN, generally read-only or weird things will happen
    uint32_t screen;
    /// Next SCREEN, used to load the right screen when loading. Can be changed in PRE_LOAD_SCREEN to go somewhere else instead. Also see `state.loading`.
    uint32_t screen_next;
    /// Shows the current loading state (0=Not loading, 1=Fadeout, 2=Loading, 3=Fadein). Writing 1 or 2 will trigger a screen load to `screen_next`.
    uint32_t loading;
    /// The global level illumination, very big and bright.
    Illumination* illumination;
    /// Current fade-to-black amount (0.0 = all visible; 1.0 = all black). Manipulated by the loading routine when loading > 0.
    float fadevalue;
    /// Amount of frames the fadeout should last when loading
    uint32_t fadeout;
    /// Amount of frames the fadein should last when loading
    uint32_t fadein;
    /// if state.loading is 1, this timer counts down to 0 while the screen is black (used after Ouroboros, in credits etc.)
    uint32_t loading_black_screen_timer;
    /// Is 1 when you in a game, is set to 0 or 1 in main menu, can't be trusted there, normally in a level is 1 unless you go to the options
    uint8_t ingame;
    /// Is 1 when you are in a level, but going to options sets it to 0 and does not set it back to 1 after the way back, don't trust it
    uint8_t playing;
    /// 8bit flags, multiple might be active at the same time
    /// 1: Menu: Pauses the level timer and engine. Can't set, controller by the menu.
    /// 2: Fade/Loading: Pauses all timers and engine.
    /// 4: Cutscene: Pauses total/level time but not engine. Used by boss cutscenes.
    /// 8: Unknown: Pauses total/level time and engine. Does not pause the global counter so set_global_interval still runs.
    /// 16: Unknown: Pauses total/level time and engine. Does not pause the global counter so set_global_interval still runs.
    /// 32: Ankh: Pauses all timers, engine, but not camera. Used by the ankh cutscene.
    PAUSE pause;
    uint8_t pause_related1;
    uint8_t pause_related2;
    uint8_t padding1[3];
    /// 32bit flags, can be written to trigger a run reset on next level load etc.
    QUEST_FLAG quest_flags;
    /// See `get_correct_ushabti`. == anim_frame - (2 * floor(anim_frame/12))
    uint8_t correct_ushabti;
    uint8_t padding2[3];
    /// Who administers the tutorial speedrun in base camp
    ENT_TYPE speedrun_character;
    /// must transition from true to false to activate it
    bool speedrun_activation_trigger;
    uint8_t padding3[3];
    /// level width in rooms (number of rooms horizontally)
    uint32_t w;
    /// level height in rooms (number of rooms vertically)
    uint32_t h;
    int8_t kali_favor;
    int8_t kali_status;
    /// Also affects if the player has punish ball, if the punish ball is destroyed it is set to -1
    int8_t kali_altars_destroyed;
    /// 0 - none, 1 - item, 3 - kapala
    int8_t kali_gifts;
    int32_t outposts_spawned;
    /// Total negative amount spent in shops during the run<br>
    /// The total money currently available (in single player) is `players[1].inventory.money + players[1].inventory.collected_money_total + state.money_shop_total`
    int32_t money_shop_total;
    /// World number to start new runs in
    uint8_t world_start;
    /// Level number to start new runs in
    uint8_t level_start;
    /// THEME to start new runs in
    uint8_t theme_start;
    uint8_t b5f;
    /// Current seed in seeded mode, just set to a funny value and does nothing in adventure mode
    uint32_t seed;
    /// Total frames of current run, equal to the final game time on win
    uint32_t time_total;
    /// Current world number, shown in hud and used by some game logic like choosing the next level on transition
    uint8_t world;
    /// Next world number, used when loading a new level or transition
    uint8_t world_next;
    /// Current level number, shown in hud and used by some game logic like choosing the next level on transition
    uint8_t level;
    /// Next level number, used when loading a new level or transition
    uint8_t level_next;
    /// Points to the current ThemeInfo
    ThemeInfo* current_theme;
    /// Current THEME number, used to pick the music and by some game logic like choosing the next level on transition
    THEME theme;
    /// Next THEME number, used when loading a new level or transition
    THEME theme_next;
    /// 0 = no win 1 = tiamat win 2 = hundun win 3 = CO win; set this and next doorway leads to victory scene
    uint8_t win_state;
    uint8_t b73; // padding probably
    /// Who pops out the spaceship for a tiamat/hundun win, this is set upon the spaceship door open
    ENT_TYPE end_spaceship_character;
    /// Current shoppie aggro
    uint8_t shoppie_aggro;
    /// Shoppie aggro to use in the next level
    uint8_t shoppie_aggro_levels;
    /// Tun aggro
    uint8_t merchant_aggro;
    /// Run totals
    uint8_t saved_dogs;
    uint8_t saved_cats;
    uint8_t saved_hamsters;
    uint8_t kills_npc;
    /// Current zero-based level count, or number of levels completed
    uint8_t level_count;
    /// Total amount of damage taken, excluding cause of death
    uint16_t damage_taken;
    bool world2_coffin_spawned;
    bool world4_coffin_spawned;
    bool world6_coffin_spawned;
    uint8_t unknown2b;
    uint8_t unknown2c;
    uint8_t unknown2d;
    std::array<ENT_TYPE, 99> waddler_storage;
    std::array<int16_t, 99> waddler_storage_meta; // to store mattock durability for example
    uint16_t journal_progression_count;
    std::array<JournalProgressionSlot, 40> journal_progression_slots;
    uint8_t skip2[844]; // TODO
    ThemeProgression theme_progression;
    uint8_t unknown3;
    uint8_t unknown4;
    uint8_t unknown5a;
    uint8_t unknown5b;
    uint8_t unknown5c;
    uint8_t unknown5d;
    ArenaState arena;
    JOURNAL_FLAG journal_flags;
    ENT_TYPE first_damage_cause; // entity type that caused first damage, for the journal
    int8_t first_damage_world;
    int8_t first_damage_level;
    uint8_t i9f4c;
    uint8_t i9f4d;
    /// Level time of previous level in frames, used by game logic to decide dark levels etc
    uint32_t time_last_level;
    /// Level time of current level in frames, show on the hud
    uint32_t time_level;
    uint32_t time_speedrun;
    uint32_t money_last_levels;
    int32_t level_flags;
    PRESENCE_FLAG presence_flags;
    /// the contents of the special coffin that will be spawned during levelgen
    ENT_TYPE coffin_contents;
    CAUSE_OF_DEATH cause_of_death;
    uint8_t padding10;
    uint8_t padding11;
    uint8_t padding12;
    ENT_TYPE cause_of_death_entity_type;
    /// entity uid of the first floor_storage entity
    int32_t waddler_floor_storage;
    MultiLineTextRendering* toast;
    MultiLineTextRendering* speechbubble;
    uint32_t speechbubble_timer;
    uint32_t toast_timer;
    int32_t speechbubble_owner;
    Dialogue basecamp_dialogue;

    // screen pointers below are most likely in an array and indexed through the screen ID (-10), hence the nullptrs for
    // screens that are available in GameManager
    ScreenCharacterSelect* screen_character_select;
    ScreenTeamSelect* screen_team_select;
    ScreenCamp* screen_camp;
    ScreenLevel* screen_level;
    ScreenTransition* screen_transition;
    ScreenDeath* screen_death;
    size_t unknown_screen_spaceship; // potentially ScreenSpaceship, but is nullptr (there is no UI rendering on spaceship anyway)
    ScreenWin* screen_win;
    ScreenCredits* screen_credits;
    ScreenScores* screen_scores;
    ScreenConstellation* screen_constellation;
    ScreenRecap* screen_recap;
    ScreenArenaMenu* screen_arena_menu;
    ScreenArenaStagesSelect* screen_arena_stages_select1;
    ScreenArenaItems* screen_arena_items;
    ScreenArenaStagesSelect* screen_arena_stages_select2;
    ScreenArenaIntro* screen_arena_intro;
    ScreenArenaLevel* screen_arena_level;
    ScreenArenaScore* screen_arena_score;
    size_t unknown_screen_online_loading; // potentially ScreenOnlineLoading, available in GameManager
    size_t unknown_screen_online_lobby;   // potentially ScreenOnlineLobby, available in GameManager

    /// Next entity spawned will have this uid
    uint32_t next_entity_uid;
    uint16_t unknown20;
    uint16_t screen_change_counter; // increments every time screen changes; used in online sync together with next_entity_uid and unknown20 as a 64bit number
    /// Access the player inputs even when no player entities are available
    PlayerInputs* player_inputs;
    /// Has the current player count, player inventories and character selections
    Items* items;
    /// Entrance and exit coordinates, shop types and all themes
    LevelGenSystem* level_gen;
    Layer* layers[2];
    /// Level logic like dice game and cutscenes
    LogicList* logic;
    /// NPC quest states
    QuestsInfo* quests;
    AITarget* ai_targets; // e.g. hired hand uid -> snake uid
    LiquidPhysics* liquid_physics;
    std::vector<ParticleEmitterInfo*>* particle_emitters;
    std::vector<Illumination*>* lightsources;
    size_t unknown27; // lookup entity struct

    // This is a Robin Hood Table
    uint32_t uid_to_entity_mask;
    uint32_t padding13;
    RobinHoodTableEntry* uid_to_entity_data;

    custom_vector<std::pair<Entity*, uint8_t>> backlayer_player_related1; // inside vector: player and destination layer?
    uint32_t layer_transition_effect_timer;
    /// The currently drawn layer, can't be changed
    uint8_t camera_layer;
    uint8_t unknown31a; // padding probably
    uint8_t unknown31b;
    uint8_t unknown31c;
    ShopsInfo shops;
    /// Number of frames since the game was launched
    uint32_t time_startup;
    uint32_t special_visibility_flags;
    /// Camera bounds and position
    Camera* camera;
    uint8_t unknown40;
    int8_t unknown41; // other character related (hired hand, basecamp characters)
    uint8_t unknown42;
    uint8_t unknown43;
    uint32_t unknown44;
    uint64_t unknown45;

    /// This function should only be used in a very specific circumstance (forcing the exiting theme when manually transitioning). Will crash the game if used inappropriately!
    void force_current_theme(uint32_t t);

    /// Returns animation_frame of the correct ushabti
    uint16_t get_correct_ushabti();
    void set_correct_ushabti(uint16_t animation_frame);
};
#pragma pack(pop)

StateMemory* get_state_ptr();

struct State
{
    size_t location;

    static void set_do_hooks(bool do_hooks);

    static void set_write_load_opt(bool allow);

    static void init();
    static void post_init();

    static State& get();

    // Returns the main-thread version of StateMemory*
    StateMemory* ptr_main() const;
    // Returns the local-thread version of StateMemory*
    StateMemory* ptr() const;
    StateMemory* ptr_local() const;

    Layer* layer(uint8_t index) const
    {
        return ptr()->layers[index];
    }
    Layer* layer_local(uint8_t index) const
    {
        return ptr_local()->layers[index];
    }

    void godmode(bool g);
    void godmode_companions(bool g);
    void darkmode(bool g);

    static size_t get_zoom_level_address();
    static float get_zoom_level();
    void zoom(float level);

    static std::pair<float, float> click_position(float x, float y);
    static std::pair<float, float> screen_position(float x, float y);

    uint32_t flags() const
    {
        return ptr()->level_flags;
    }

    void set_flags(uint32_t f)
    {
        ptr()->level_flags = f;
    }

    void set_pause(uint8_t p)
    {
        ptr()->pause = p;
    }

    uint32_t get_frame_count_main() const;
    uint32_t get_frame_count() const;

    std::vector<int64_t> read_prng() const;

    Entity* find(uint32_t uid);
    Entity* find_local(uint32_t uid);

    static std::pair<float, float> get_camera_position();
    void set_camera_position(float cx, float cy);
    void warp(uint8_t w, uint8_t l, uint8_t t);
    void set_seed(uint32_t seed);
    SaveData* savedata();
    LiquidPhysicsEngine* get_correct_liquid_engine(ENT_TYPE liquid_type);
};
void init_state_update_hook();

uint8_t enum_to_layer(const LAYER layer, std::pair<float, float>& player_position);
uint8_t enum_to_layer(const LAYER layer);

uint32_t lowbias32(uint32_t x);
uint32_t lowbias32_r(uint32_t x);
