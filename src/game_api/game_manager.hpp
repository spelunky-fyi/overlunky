#pragma once

#include <cstddef> // for size_t
#include <cstdint> // for uint32_t, uint8_t, int8_t

#include "aliases.hpp"    // for MAX_PLAYERS
#include "render_api.hpp" // for TextureRenderingInfo
#include "sound_manager.hpp"
#include "thread_utils.hpp" // for OnHeapPointer

struct SaveData;
class ScreenCamp;
class ScreenIntro;
class ScreenLeaderboards;
class ScreenLevel;
class ScreenLogo;
class ScreenMenu;
class ScreenOnlineLoading;
class ScreenOnlineLobby;
class ScreenOptions;
class ScreenPlayerProfile;
class ScreenPrologue;
class ScreenSeedInput;
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
};

struct SaveRelated
{
    OnHeapPointer<SaveData> savedata;
    JournalPopupUI journal_popup_ui;
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
    uint8_t unknown20;
    int8_t skip[2400]; // 600 floats, mostly seem to be 1.0
    float idle_counter;
    uint32_t unknown22;
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
    /// Zero-based indexing, i.e. use PlayerSlot.input_mapping_controller directly to index this.
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
    bool unknown1;
    bool unknown2;
    bool menu_input;
    bool lost_connection;
    int8_t input_index;
    uint8_t padding2[3];
    uint32_t buttons;
    // a lot more stuff
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

    int32_t unknown12; // -1
    int8_t unknown13;  // -1

    // uint8_t padding_probably2[3];

    size_t unknown14; // probably not one thig
    uint32_t unknown15;

    // uint32_t padding_probably3;
    size_t* unknown16;
    size_t unknwon17;
    double unknown18; // counts time or something? only active when the game window is active
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

    // screen pointers below are most likely in an array and indexed through the screen ID, hence the nullptrs for
    // screens that are available in State
    ScreenLogo* screen_logo;
    ScreenIntro* screen_intro;
    ScreenPrologue* screen_prologue;
    ScreenTitle* screen_title;
    ScreenMenu* screen_menu;
    ScreenOptions* screen_options;
    ScreenPlayerProfile* screen_player_profile;
    ScreenLeaderboards* screen_leaderboards;
    ScreenSeedInput* screen_seed_input;
    size_t unknown_screen_character_select; // available in State
    size_t unknown_screen_team_select;      // available in State
    ScreenCamp* screen_camp;
    ScreenLevel* screen_level;
    size_t* screen_transition;           // available in State, but it's a different object! this one only has a render_timer
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
    size_t* screen_arena_level;          // also available in State, but it's a different object! this one only has a render_timer, no UI parts
    size_t* unknown_screen_arena_score;  // available in State
    ScreenOnlineLoading* screen_online_loading;
    ScreenOnlineLobby* screen_online_lobby;
    PauseUI* pause_ui;
    JournalUI* journal_ui;
    BackgroundSound* main_menu_music;
};

GameManager* get_game_manager();
RawInput* get_raw_input();
