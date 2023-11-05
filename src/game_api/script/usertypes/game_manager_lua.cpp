#include "game_manager_lua.hpp"

#include <algorithm>   // for max
#include <new>         // for operator new
#include <sol/sol.hpp> // for data_t, basic_table_core::new_usertype
#include <string>      // for operator==, allocator
#include <tuple>       // for get
#include <type_traits> // for move, declval
#include <utility>     // for min, max

#include "game_manager.hpp"      // for GameManager, JournalPopupUI, GameProps
#include "memory.hpp"            // for memory_read TODO:temp
#include "screen.hpp"            // IWYU pragma: keep
#include "script/sol_helper.hpp" //

namespace NGM
{
void register_usertypes(sol::state& lua)
{
    /// Used in GameManager
    lua.new_usertype<BackgroundMusic>(
        "BackgroundMusic",
        "game_startup",
        &BackgroundMusic::game_startup,
        "main_backgroundtrack",
        &BackgroundMusic::main_backgroundtrack,
        "basecamp",
        &BackgroundMusic::basecamp,
        "win_scene",
        &BackgroundMusic::win_scene,
        "arena",
        &BackgroundMusic::arena,
        "arena_intro_and_win",
        &BackgroundMusic::arena_intro_and_win,
        "level_gameplay",
        &BackgroundMusic::level_gameplay,
        "dark_level",
        &BackgroundMusic::dark_level,
        "level_transition",
        &BackgroundMusic::level_transition,
        "backlayer",
        &BackgroundMusic::backlayer,
        "shop",
        &BackgroundMusic::shop,
        "angered_shopkeeper",
        &BackgroundMusic::angered_shopkeeper,
        "inside_sunken_city_pipe",
        &BackgroundMusic::inside_sunken_city_pipe,
        "pause_menu",
        &BackgroundMusic::pause_menu,
        "death_transition",
        &BackgroundMusic::death_transition);

    /// Can be accessed via global [game_manager](#game_manager)
    auto gamemanager_type = lua.new_usertype<GameManager>("GameManager");
    gamemanager_type["music"] = &GameManager::music;
    gamemanager_type["game_props"] = &GameManager::game_props;
    gamemanager_type["screen_logo"] = &GameManager::screen_logo;
    gamemanager_type["screen_intro"] = &GameManager::screen_intro;
    gamemanager_type["screen_prologue"] = &GameManager::screen_prologue;
    gamemanager_type["screen_title"] = &GameManager::screen_title;
    gamemanager_type["screen_menu"] = &GameManager::screen_menu;
    gamemanager_type["screen_options"] = &GameManager::screen_options;
    gamemanager_type["screen_player_profile"] = &GameManager::screen_player_profile;
    gamemanager_type["screen_leaderboards"] = &GameManager::screen_leaderboards;
    gamemanager_type["screen_seed_input"] = &GameManager::screen_seed_input;
    gamemanager_type["screen_camp"] = &GameManager::screen_camp;
    gamemanager_type["screen_level"] = &GameManager::screen_level;
    gamemanager_type["screen_online_loading"] = &GameManager::screen_online_loading;
    gamemanager_type["screen_online_lobby"] = &GameManager::screen_online_lobby;
    gamemanager_type["pause_ui"] = &GameManager::pause_ui;
    gamemanager_type["journal_ui"] = &GameManager::journal_ui;
    gamemanager_type["save_related"] = &GameManager::save_related;
    gamemanager_type["main_menu_music"] = &GameManager::main_menu_music;
    gamemanager_type["buttons_controls"] = &GameManager::buttons_controls;
    gamemanager_type["buttons_movement"] = &GameManager::buttons_movement;

    lua.new_usertype<SaveRelated>(
        "SaveRelated",
        "journal_popup_ui",
        &SaveRelated::journal_popup_ui);
    lua.new_usertype<JournalPopupUI>(
        "JournalPopupUI",
        "wiggling_page_icon",
        &JournalPopupUI::wiggling_page_icon,
        "black_background",
        &JournalPopupUI::black_background,
        "button_icon",
        &JournalPopupUI::button_icon,
        "wiggling_page_angle",
        &JournalPopupUI::wiggling_page_angle,
        "chapter_to_show",
        &JournalPopupUI::chapter_to_show,
        "entry_to_show",
        &JournalPopupUI::entry_to_show,
        "timer",
        &JournalPopupUI::timer,
        "slide_position",
        &JournalPopupUI::slide_position);
    lua.new_usertype<InputDevice>(
        "InputDevice",
        "disabled",
        &InputDevice::disabled,
        "enabled",
        &InputDevice::enabled,
        "menu_input",
        &InputDevice::menu_input,
        "connected",
        &InputDevice::connected,
        "input_index",
        &InputDevice::input_index,
        "buttons",
        &InputDevice::buttons);
    lua.new_usertype<GameProps>(
        "GameProps",
        "buttons",
        [](GameProps& gp) -> uint32_t
        {
            return gp.buttons[1];
        },
        "buttons_previous",
        [](GameProps& gp) -> uint32_t
        {
            return gp.buttons_previous[1];
        },
        "buttons_menu",
        &GameProps::buttons_menu,
        "buttons_menu_previous",
        &GameProps::buttons_menu_previous,
        "input",
        &GameProps::buttons,
        "input_previous",
        &GameProps::buttons_previous,
        "input_menu",
        &GameProps::buttons_menu,
        "input_menu_previous",
        &GameProps::buttons_menu_previous,
        "game_has_focus",
        &GameProps::game_has_focus,
        "modal_open",
        sol::property([](GameProps& gp)
                      { return gp.modal_open == 0; }),
        "input_device",
        &GameProps::input_device,
        "input_index",
        &GameProps::input_index);
    lua.new_usertype<RawInput>(
        "RawInput",
        "keyboard",
        &RawInput::keyboard,
        "controller",
        //&RawInput::controller,
        sol::property([](RawInput& r)
                      { return ZeroIndexArray<ControllerInput>(r.controller) /**/; }));
    lua.new_usertype<KeyboardKey>(
        "KeyboardKey",
        "down",
        &KeyboardKey::down);
    lua.new_usertype<ControllerInput>(
        "ControllerInput",
        "buttons",
        &ControllerInput::buttons);
    lua.new_usertype<ControllerButton>(
        "ControllerButton",
        "down",
        &ControllerButton::down,
        "pressed",
        &ControllerButton::pressed);

    /// Returns RawInput, a game structure for raw keyboard and controller state
    lua["get_raw_input"] = get_raw_input;
}
}; // namespace NGM
