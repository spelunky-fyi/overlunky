#pragma once

#include <cstddef> // for size_t
#include <cstdint> // for uint32_t, uint8_t, int8_t

#include "aliases.hpp"                       // for MAX_PLAYERS
#include "containers/game_unordered_map.hpp" // for game_unordered_map
#include "containers/identity_hasher.hpp"    // for identity_hasher
#include "render_api.hpp"                    // for TextureRenderingInfo
#include "sound_manager.hpp"                 // for BackgroundSound
#include "thread_utils.hpp"                  // for OnHeapPointer

struct SaveData;
class ScreenCamp;
class ScreenIntro;
class Screen;
class ScreenLevel;
class ScreenLogo;
class ScreenMenu;
class ScreenOnlineLoading;
class ScreenOnlineLobby;
class ScreenOptions;
class ScreenPrologue;
class ScreenCodeInput;
class ScreenTitle;
struct JournalUI;
struct PauseUI;

struct JournalPopupUI
{
    TextureRenderingInfo wiggling_page_icon;
    TextureRenderingInfo black_background;
    TextureRenderingInfo button_icon;
    float wiggling_page_angle;
    uint32_t chapter_to_show;
    uint32_t entry_to_show; // use the odd entry of the left hand page
    uint32_t timer;
    float slide_position;
    uint8_t unknown;
    // uint8_t padding[3]; // probably?
};

class JournalPageData
{
  public:
    uint32_t page_nr;
    uint32_t sprite_id;
    STRINGID name;
    STRINGID description;
    float scale;
    float offset_x;
    float offset_y;
};

class JournalPeopleData : public JournalPageData
{
  public:
    TEXTURE texture;
    uint32_t background_sprite_id;
    bool killed_by_NA;
    bool defeated_NA;
    // uint16_t padding;
    TEXTURE portrait_texture;
};

// probably same as JournalPeopleData, just not using portrait_texture
class JournalBestiaryData : public JournalPageData
{
  public:
    TEXTURE texture;
    uint32_t background_sprite_id;
    bool killed_by_NA;
    bool defeated_NA;
};

// this is probably part of JournalPageData, just only used for bestiary, people and trap
class JournalTrapData : public JournalPageData
{
  public:
    TEXTURE texture;
    uint32_t background_sprite_id;
};

struct StickersData
{
    uint32_t sprite_id;
    TEXTURE texture;
};

struct SaveRelated
{
    OnHeapPointer<SaveData> savedata;
    JournalPopupUI journal_popup_ui;

    ENT_TYPE player_entity;                  // for the journal stuff, probably the leader?
    ENT_TYPE progress_stickers_powerups[29]; // pre-journal progress setup, maybe gathering from all players or something?

    /// Scale and offset not used in those pages. Can't add more
    game_unordered_map<uint8_t, JournalPageData, identity_hasher<>> places_data;
    game_unordered_map<ENT_TYPE, JournalBestiaryData, identity_hasher<>> bestiary_data;
    /// used to map stuff like Osiris_Hand -> Osiris_Head, Hundun limbs -> Hundun etc.
    game_unordered_map<ENT_TYPE, ENT_TYPE, identity_hasher<>> monster_part_to_main;
    game_unordered_map<ENT_TYPE, JournalPeopleData, identity_hasher<>> people_info;
    /// used to map shopkeeper clone to shopkeeper only
    game_unordered_map<ENT_TYPE, ENT_TYPE, identity_hasher<>> people_part_to_main;
    game_unordered_map<ENT_TYPE, JournalPageData, identity_hasher<>> item_info;
    game_unordered_map<ENT_TYPE, JournalPageData, identity_hasher<>> trap_info;
    /// used for stuff like upsidedown_spikes -> spikes, skulldrop skulls -> skulldrop trap etc.
    game_unordered_map<ENT_TYPE, ENT_TYPE, identity_hasher<>> trap_part_to_main;
    game_unordered_map<ENT_TYPE, StickersData, identity_hasher<>> stickers_data;

    /// Gets local version of the SaveData
    SaveData* get_savegame()
    {
        return savedata.decode_local();
    }
};

struct BGMUnknown
{
    std::array<float, 40> unknown; // probably wrong size
};

struct BackgroundMusic
{
    BackgroundSound* game_startup;
    BackgroundSound* main_backgroundtrack;
    BackgroundSound* basecamp;
    BackgroundSound* win_scene;
    BackgroundSound* arena;
    BackgroundSound* arena_intro_and_win;
    BackgroundSound* level_gameplay;
    BackgroundSound* dark_level;
    BackgroundSound* level_transition;
    BackgroundSound* backlayer;
    BackgroundSound* shop;
    BackgroundSound* angered_shopkeeper;
    BackgroundSound* inside_sunken_city_pipe;
    BackgroundSound* pause_menu;
    BackgroundSound* unknown15;
    BackgroundSound* death_transition;
    uint8_t unknown17;
    uint8_t unknown18;
    uint8_t unknown19;
    uint8_t padding_probably;
    std::array<BGMUnknown, 15> unknown21; // They continuously go from 1 to 0 and back as a one big table, from first one to the last one, each change is one tick counted by the unknown22
    float idle_counter;                   // counts down at the start of a level, then reacts to the player movement controls
    uint32_t unknown22;                   // some timer (counts continuously)
};

struct KeyboardKey
{
    /// Key is being held
    bool down;
    /// Key was just pressed down this frame
    bool pressed;
    size_t unknown;
};

struct ControllerButton
{
    /// Button is being held
    bool down;
    /// Button was just pressed down this frame
    bool pressed;
};

struct ControllerInput
{
    /// Zero-based indexing. Use PlayerSlot.input_mapping_controller or RAW_BUTTON (or RAW_DUALSHOCK) to index this.
    std::array<ControllerButton, 16> buttons;
};

struct RawInput
{
    /// State of all keyboard buttons in a random game order as usual, most key indexes can be found in RAW_KEY. Zero-based indexing, i.e. use PlayerSlot.input_mapping_keyboard directly to index this.
    std::array<KeyboardKey, 112> keyboard;
    /// State of controller buttons per controller. Zero-based indexing, i.e. use GameProps.input_index directly to index this.
    std::array<ControllerInput, 12> controller;
};

struct InputDevice
{
    // No idea what these actually do, better not to expose this anyway
    bool unknown1;   // is_keyboard ?
    bool unknown2;   // is_controller ?
    bool menu_input; // keyboard doesn't seam to care about this
    bool lost_connection;
    int8_t input_index;
    uint8_t padding2[3];
    uint32_t buttons;
    uint8_t controller_index; // for XInput used in XInputSetState
};

struct GameProps
{
    /// Used for player input and might be used for some menu inputs not found in buttons_menu. You can probably capture and edit this in ON.POST_PROCESS_INPUT. These are raw inputs, without things like autorun applied.
    std::array<uint32_t, MAX_PLAYERS> buttons;
    std::array<uint32_t, MAX_PLAYERS> buttons_previous;
    /// Previous state of buttons_menu
    MENU_INPUT buttons_menu_previous;
    /// Inputs used to control all the menus, separate from player inputs. You can probably capture and edit this in ON.POST_PROCESS_INPUT
    MENU_INPUT buttons_menu;
    int8_t menu_icon_slot;
    bool game_has_focus;
    bool unknown9;
    bool unknown10;
    /// Yet another place for some buttons in some random order, too tired to make another enum for them
    std::array<InputDevice*, 12> input_device;

    /// Input index for players 1-4 and maybe for the menu controls. -1: disabled, 0..3: keyboards, 4..7: Xinput, 8..11: other controllers
    std::array<int8_t, 5> input_index;

    // uint8_t padding_probably1[3];

    int32_t next_player_entrence; // unsure?
    int8_t unknown13a;            // -1
    int8_t unknown13b;            // bool?

    // uint8_t padding_probably2[2];

    uint32_t unknown14a;
    int8_t unknown14b[3]; // -1

    // uint8_t padding_probably3;

    uint32_t unknown15;

    // uint32_t padding_probably4;
    size_t* unknown16;
    size_t unknown17;
    std::array<double, MAX_PLAYERS> unknown18; // counts time or something? only active when the game window is active when there is a player choosen?
    int32_t unknown19;
    int32_t unknown20; // -1
    size_t unknown21;
    size_t unknown22;
    bool unknown23;

    // seam like only garbage below
};

struct GameManager
{
    BackgroundMusic* music;
    SaveRelated* save_related;
    /// Yet another place to get player inputs, in some format
    std::array<uint8_t, MAX_PLAYERS> buttons_controls;
    /// Yet another place to get player inputs, in some format
    std::array<uint8_t, MAX_PLAYERS> buttons_movement;
    GameProps* game_props;

    // screen pointers below are most likely in an array and indexed through the screen ID, hence the nullptr for
    // screens that are available in State
    ScreenLogo* screen_logo;
    ScreenIntro* screen_intro;
    ScreenPrologue* screen_prologue;
    ScreenTitle* screen_title;
    ScreenMenu* screen_menu;
    ScreenOptions* screen_options;
    /// It just opens journal
    Screen* screen_player_profile;
    /// All handled by the Online
    Screen* screen_leaderboards;
    ScreenCodeInput* screen_seed_input;
    size_t unknown_screen_character_select; // available in State
    size_t unknown_screen_team_select;      // available in State
    ScreenCamp* screen_camp;
    ScreenLevel* screen_level;
    Screen* screen_transition;           // available in State, but it's a different object! this one only has a render_timer
    size_t unknown_screen_death;         // available in State
    size_t unknown_screen_spaceship;     // (also not) available in State
    size_t unknown_screen_win;           // available in State
    size_t unknown_screen_credits;       // available in State
    size_t unknown_screen_scores;        // available in State
    size_t unknown_screen_constellation; // available in State
    size_t unknown_screen_recap;         // available in State
    size_t unknown_screen_arena_menu;    // available in State
    size_t unknown_screen_arena_stages;  // available in State
    size_t unknown_screen_arena_items;   // available in State
    size_t unknown_screen_arena_select;  // available in State
    size_t unknown_screen_arena_intro;   // available in State
    Screen* screen_arena_level;          // also available in State, but it's a different object! Seams to only call pause function at the start, probably for the intro stuff
    Screen* screen_arena_score;          // available in State
    ScreenOnlineLoading* screen_online_loading;
    ScreenOnlineLobby* screen_online_lobby;
    PauseUI* pause_ui;
    JournalUI* journal_ui;
    BackgroundSound* main_menu_music;
    float layer_transition; // menus, going inwards/backwards
    float layer;
    int8_t transition_timer;
    bool unknown31;
    // uint8_t padding_probably1[2];
    uint32_t unknown33;
    bool unknown34;
    bool unknown35;
    // uint8_t padding_probably2[2];
    // 3 * 4 bytes more maybe?
};

GameManager* get_game_manager();
RawInput* get_raw_input();
