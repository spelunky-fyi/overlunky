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
    bool down;
    size_t unknown;
};

struct ControllerButton
{
    bool down;
    bool pressed;
};

struct ControllerInput
{
    std::array<ControllerButton, 16> buttons;
};

struct RawInput
{
    std::array<KeyboardKey, 112> keyboard;
    std::array<ControllerInput, 12> controller;
};

struct SomeInput
{
    bool enabled;
    uint8_t padding1[3];
    int8_t input_index;
    uint8_t padding2[3];
    uint32_t buttons;
};

struct GameProps
{
    /// Might be used for some menu inputs not found in buttons_menu
    uint32_t buttons;
    uint32_t unknown1;
    uint32_t unknown2;
    uint32_t unknown3;
    /// Might be used for some menu inputs not found in buttons_menu
    uint32_t buttons_extra;
    uint32_t unknown4;
    uint32_t unknown5;
    uint32_t unknown6;
    /// Previous state of buttons_menu
    MENU_INPUT buttons_menu_previous;
    /// Inputs used to control all the menus, separate from player inputs. You can probably capture and edit this in ON.PRE_UPDATE.
    MENU_INPUT buttons_menu;
    int8_t modal_open;
    bool game_has_focus;
    bool unknown9;
    bool unknown10;
    /// Yet another place for some buttons in some random order, too tired to make another enum for them
    std::array<SomeInput*, 12> some_input;

    std::array<int8_t, 5> input_index; // 0-3 keyboards, 4->controllers, if not used it's -1
    // for example if you just run the game and use OL to warp somewhere immediately there will be no controller setup, so all of those will be -1

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
