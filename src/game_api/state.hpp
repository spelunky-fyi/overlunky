#pragma once

#include "items.hpp"
#include "screen.hpp"
#include "screen_arena.hpp"
#include "state_structs.hpp"

const float ZF = 0.737f;

struct SaveData;
struct Layer;
struct LevelGenSystem;
class ThemeInfo;
void fix_liquid_out_of_bounds();

#pragma pack(push, 1) // disable struct padding
struct StateMemory
{
    size_t p00;
    uint32_t screen_last;
    uint32_t screen;
    uint32_t screen_next;
    uint32_t loading;
    Illumination* illumination;
    float fadevalue; // 0.0 = all visible; 1.0 = all black
    uint32_t fadeout;
    uint32_t fadein;
    uint32_t loading_black_screen_timer; // if state.loading is 1, this timer counts down to 0 while the screen is black (used after Ouroboros, in
                                         // credits, ...)
    uint8_t ingame;
    uint8_t playing;
    /// `state.pause == 2` will pause the game but that won't run any callback, `state.pause == 16` will do the same but `set_global_interval` will still work
    uint8_t pause;
    uint8_t b33;
    int32_t i34;
    uint32_t quest_flags;
    uint8_t correct_ushabti; // correct_ushabti = anim_frame - (2 * floor(anim_frame/12))
    uint8_t i3cb;
    uint8_t i3cc;
    uint8_t i3cd;
    ENT_TYPE speedrun_character;         // who administers the speedrun in base camp
    uint8_t speedrun_activation_trigger; // must transition from true to false to activate it
    uint8_t padding4;
    uint8_t padding5;
    uint8_t padding6;
    uint32_t w;
    uint32_t h;
    int8_t kali_favor;
    int8_t kali_status;
    /// Also affects if the player has punish ball, if the punish ball is destroyed it is set to -1
    int8_t kali_altars_destroyed;
    /// 0 - none, 1 - item, 3 - kapala
    int8_t kali_gifts;
    int32_t i50;              // kali related?
    int32_t money_shop_total; // total $ spent at shops, persists between levels, number will be negative
    uint8_t world_start;
    uint8_t level_start;
    uint8_t theme_start;
    uint8_t b5f;
    uint32_t seed;
    uint32_t time_total;
    uint8_t world;
    uint8_t world_next;
    uint8_t level;
    uint8_t level_next;
    ThemeInfo* current_theme;
    uint8_t theme;
    uint8_t theme_next;
    /// 0 = no win 1 = tiamat win 2 = hundun win 3 = CO win; set this and next doorway leads to victory scene
    uint8_t win_state;
    uint8_t b73;
    /// Who pops out the spaceship for a tiamat/hundun win, this is set upon the spaceship door open
    ENT_TYPE end_spaceship_character;
    uint8_t shoppie_aggro;
    uint8_t shoppie_aggro_levels;
    uint8_t merchant_aggro;
    /// Run totals
    uint8_t saved_dogs;
    uint8_t saved_cats;
    uint8_t saved_hamsters;
    uint8_t kills_npc;
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
    uint8_t skip2[844];
    ThemeProgression theme_progression;
    uint8_t unknown3;
    uint8_t unknown4;
    uint8_t unknown5a;
    uint8_t unknown5b;
    uint8_t unknown5c;
    uint8_t unknown5d;
    ArenaState arena;
    uint32_t journal_flags;
    ENT_TYPE first_damage_cause; // entity type that caused first damage, for the journal
    int8_t first_damage_world;
    int8_t first_damage_level;
    uint8_t i9f4c;
    uint8_t i9f4d;
    uint32_t time_last_level;
    uint32_t time_level;
    uint32_t time_speedrun;
    uint32_t money_last_levels;
    int32_t level_flags;
    uint32_t presence_flags;
    ENT_TYPE coffin_contents; // entity type - the contents of the coffin that will be spawned (during levelgen)
    uint8_t cause_of_death;
    uint8_t padding10;
    uint8_t padding11;
    uint8_t padding12;
    ENT_TYPE cause_of_death_entity_type;
    int32_t waddler_floor_storage; // entity uid of the first floor_storage entity
    size_t toast;
    size_t speechbubble;
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

    uint32_t next_entity_uid;
    uint16_t unknown20;
    uint16_t screen_change_counter; // increments every time screen changes; used in online sync together with next_entity_uid and unknown20 as a 64bit number
    PlayerInputs* player_inputs;

    Items* items;
    LevelGenSystem* level_gen;
    Layer* layers[2];
    LogicList* logic;
    QuestsInfo* quests;
    AITarget* ai_targets; // e.g. hired hand uid -> snake uid
    LiquidPhysics* liquid_physics;
    std::vector<ParticleEmitterInfo*>* particle_emitters;
    std::vector<Illumination*>* lightsources;
    size_t unknown27;

    // This is a Robin Hood Table
    uint32_t uid_to_entity_mask;
    uint32_t padding13;
    RobinHoodTableEntry* uid_to_entity_data;

    size_t backlayer_player_related1;
    size_t backlayer_player_related2;

    size_t unknown30;
    uint32_t layer_transition_effect_timer;
    uint8_t camera_layer;
    uint8_t unknown31a;
    uint8_t unknown31b;
    uint8_t unknown31c;
    ShopsInfo shops;
    uint32_t time_startup;
    uint32_t special_visibility_flags;
    Camera* camera;

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

    static void set_write_load_opt(bool allow);

    static State& get();

    // Returns the main-thread version of StateMemory*
    StateMemory* ptr() const;
    // Returns the local-thread version of StateMemory*
    StateMemory* ptr_local() const;

    Layer* layer(uint8_t index) const
    {
        return ptr()->layers[index];
    }
    Layer* layer_local(uint8_t index) const
    {
        return ptr_local()->layers[index];
    }

    Items* items() const
    {
        auto pointer = ptr()->items;
        return pointer;
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

    uint32_t get_frame_count() const;

    std::vector<int64_t> read_prng() const;

    Entity* find(uint32_t uid);

    static std::pair<float, float> get_camera_position();
    void set_camera_position(float cx, float cy);
    void warp(uint8_t w, uint8_t l, uint8_t t);
    void set_seed(uint32_t seed);
    SaveData* savedata();
    LiquidPhysicsEngine* get_correct_liquid_engine(ENT_TYPE liquid_type);
};

uint8_t enum_to_layer(const LAYER layer, std::pair<float, float>& player_position);
uint8_t enum_to_layer(const LAYER layer);
