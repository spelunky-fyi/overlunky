#include "items.hpp"
#include "savedata.hpp"
#include "screen.hpp"
#include "screen_arena.hpp"
#include "thread_utils.hpp"

struct TmpStruct
{
    OnHeapPointer<SaveData> savedata;
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
    void* backgroundmusic;
    TmpStruct* tmp;
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
    ScreenCharacterSelect* screen_character_select;
    size_t unknown_screen_team_select; // available in State
    ScreenCamp* screen_camp;
    ScreenLevel* screen_level;
    size_t unknown_screen_transition;    // available in State
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
