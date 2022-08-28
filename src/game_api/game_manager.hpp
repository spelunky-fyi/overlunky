#pragma once

#include <cstddef> // for size_t
#include <cstdint> // for uint32_t, uint8_t, int8_t

#include "aliases.hpp"      // for MAX_PLAYERS
#include "render_api.hpp"   // for TextureRenderingInfo
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

struct AudioChannelParams
{
    uint64_t unknown1;
    uint64_t unknown2;
    uint64_t unknown3;
    uint64_t unknown4;
    float unknown5;
    float unknown6; // triggers on ghost (music distortion)
    float unknown7a;
    float unknown7b;
    uint64_t unknown8;
    uint64_t unknown9;
    uint64_t unknown10;
    uint64_t unknown11;
    float unknown12;
    float unknown13;
    float unknown14;
    float unknown15;
    float unknown16;
    uint32_t unknown17;
    uint64_t unknown18;
    uint64_t unknown19;
    uint64_t unknown20;
    uint64_t unknown21;
    float unknown22;
    float unknown23;
    float unknown24;
    uint32_t unknown25;
};

struct MusicTrack
{
    uint32_t unknown1;
    uint32_t unknown2;
    size_t music_track;  // unsure?
    uint64_t fmod_param; // param to FMOD::Studio::EventInstance::setParameterByID
    AudioChannelParams left_channel;
    AudioChannelParams right_channel;
    /// when false, current track starts from the beginning, is immediately set back to true
    bool start_over;
    uint8_t unknown49;
    /// set to false to turn off
    bool music_on;
    uint8_t unknown51;
    uint32_t garbage1;
    bool fadeout_music_and_crash; // probably need to call destroy after or something

    virtual void start() = 0;                                // just sets music_on to true
    virtual void fade_out(uint8_t) = 0;                      // unsure
    virtual void get_name(size_t buttor, uint32_t size) = 0; // unsure?
    virtual ~MusicTrack() = 0;                               //
    virtual void update() = 0;                               // disabling this function does not progresses the track, does not stop it at the end level etc.
                                                             // like if you start a level you have one loop and the after you move it porgresses to another one
    virtual bool unknown() = 0;
};

struct BackgroundMusic
{
    MusicTrack* game_startup;
    MusicTrack* main_backgroundtrack;
    MusicTrack* basecamp;
    MusicTrack* win_scene;
    MusicTrack* arena;
    MusicTrack* arena_intro_and_win;
    MusicTrack* level_gameplay;
    MusicTrack* dark_level;
    MusicTrack* level_transition;
    MusicTrack* backlayer;
    MusicTrack* shop;
    MusicTrack* angered_shopkeeper;
    MusicTrack* inside_sunken_city_pipe;
    MusicTrack* pause_menu;
    size_t unknown15;
    MusicTrack* sunken_city_duat_transition;
    uint8_t unknown17;
    uint8_t unknown18;
    uint8_t unknown19;
    uint8_t unknown20;
    int8_t skip[2400]; // 600 floats, mostly seem to be 1.0
    float idle_counter;
    uint32_t unknown22;
};

struct GameProps
{
    uint32_t buttons;
    uint32_t unknown1;
    uint32_t unknown2;
    uint32_t unknown3;
    uint32_t buttons_dupe;
    uint32_t unknown4;
    uint32_t unknown5;
    uint32_t unknown6;
    uint32_t buttons_dupe_but_different;
    int8_t unknown8;
    bool game_has_focus;
    bool unknown9;
    bool unknown10;
    // there's more stuff here
};

struct GameManager
{
    BackgroundMusic* backgroundmusic;
    SaveRelated* save_related;
    uint8_t buttons_controls[MAX_PLAYERS];
    uint8_t buttons_movement[MAX_PLAYERS];
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
    size_t screen_transition;            // available in State, but it's a different object! this one only has a render_timer
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
    size_t screen_arena_level;           // also available in State, but it's a different object! this one only has a render_timer, no UI parts
    size_t unknown_screen_arena_score;   // available in State
    ScreenOnlineLoading* screen_online_loading;
    ScreenOnlineLobby* screen_online_lobby;
    PauseUI* pause_ui;
    JournalUI* journal_ui;
};

GameManager* get_game_manager();
