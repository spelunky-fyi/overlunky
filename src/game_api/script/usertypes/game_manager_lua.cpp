#include "game_manager_lua.hpp"

#include <algorithm>   // for max
#include <new>         // for operator new
#include <sol/sol.hpp> // for data_t, basic_table_core::new_usertype
#include <string>      // for operator==, allocator
#include <tuple>       // for get
#include <type_traits> // for move, declval
#include <utility>     // for min, max

#include "containers/game_allocator.hpp" // for game_allocator
#include "game_manager.hpp"              // for GameManager, JournalPopupUI, GameProps
#include "memory.hpp"                    // for memory_read TODO:temp
#include "savedata.hpp"                  // for SaveData
#include "screen.hpp"                    // IWYU pragma: keep
#include "script/sol_helper.hpp"         // for ZeroIndexArray

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
    gamemanager_type["screen_transition"] = &GameManager::screen_transition;
    gamemanager_type["screen_arena_level"] = &GameManager::screen_arena_level;
    gamemanager_type["screen_arena_score"] = &GameManager::screen_arena_score;
    gamemanager_type["screen_online_loading"] = &GameManager::screen_online_loading;
    gamemanager_type["screen_online_lobby"] = &GameManager::screen_online_lobby;
    gamemanager_type["pause_ui"] = &GameManager::pause_ui;
    gamemanager_type["journal_ui"] = &GameManager::journal_ui;
    gamemanager_type["save_related"] = &GameManager::save_related;
    gamemanager_type["main_menu_music"] = &GameManager::main_menu_music;
    gamemanager_type["buttons_controls"] = &GameManager::buttons_controls;
    gamemanager_type["buttons_movement"] = &GameManager::buttons_movement;

    /// Used in GameManager
    lua.new_usertype<SaveRelated>(
        "SaveRelated",
        "journal_popup_ui",
        &SaveRelated::journal_popup_ui,
        "places_data",
        &SaveRelated::places_data,
        "bestiary_data",
        &SaveRelated::bestiary_data,
        "monster_part_to_main",
        &SaveRelated::monster_part_to_main,
        "people_info",
        &SaveRelated::people_info,
        "people_part_to_main",
        &SaveRelated::people_part_to_main,
        "item_info",
        &SaveRelated::item_info,
        "trap_info",
        &SaveRelated::trap_info,
        "trap_part_to_main",
        &SaveRelated::trap_part_to_main,
        "stickers_data",
        &SaveRelated::stickers_data,
        "get_savegame",
        &SaveRelated::get_savegame);

    /// Used in SaveRelated
    lua.new_usertype<JournalPageData>(
        "JournalPageData",
        "page_nr",
        &JournalPageData::page_nr,
        "sprite_id",
        &JournalPageData::sprite_id,
        "name",
        &JournalPageData::name,
        "description",
        &JournalPageData::description,
        "scale",
        &JournalPageData::scale,
        "offset_x",
        &JournalPageData::offset_x,
        "offset_y",
        &JournalPageData::offset_y);

    /// Used in SaveRelated
    lua.new_usertype<JournalBestiaryData>(
        "JournalBestiaryData",
        "texture",
        &JournalBestiaryData::texture,
        "background_sprite_id",
        &JournalBestiaryData::background_sprite_id,
        "killed_by_NA",
        &JournalBestiaryData::killed_by_NA,
        "defeated_NA",
        &JournalBestiaryData::defeated_NA,
        sol::base_classes,
        sol::bases<JournalPageData>());

    /// Used in SaveRelated
    lua.new_usertype<JournalPeopleData>(
        "JournalPeopleData",
        "texture",
        &JournalPeopleData::texture,
        "background_sprite_id",
        &JournalPeopleData::background_sprite_id,
        "killed_by_NA",
        &JournalPeopleData::killed_by_NA,
        "defeated_NA",
        &JournalPeopleData::defeated_NA,
        "portrait_texture",
        &JournalPeopleData::portrait_texture,
        sol::base_classes,
        sol::bases<JournalPageData>());

    /// Used in SaveRelated
    lua.new_usertype<JournalTrapData>(
        "JournalTrapData",
        "texture",
        &JournalTrapData::texture,
        "background_sprite_id",
        &JournalTrapData::background_sprite_id,
        sol::base_classes,
        sol::bases<JournalPageData>());

    /// Used in SaveRelated
    lua.new_usertype<StickersData>(
        "StickersData",
        "sprite_id",
        &StickersData::sprite_id,
        "texture",
        &StickersData::texture);

    /// Used in SaveRelated
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

    auto gameprops_type = lua.new_usertype<GameProps>("GameProps");
    /// NoDoc
    gameprops_type["buttons"] = sol::property([](GameProps& gp) -> uint32_t
                                              { return gp.buttons[0]; });
    gameprops_type["input"] = &GameProps::buttons;
    gameprops_type["input_previous"] = &GameProps::buttons_previous;
    gameprops_type["input_menu"] = &GameProps::buttons_menu;
    gameprops_type["input_menu_previous"] = &GameProps::buttons_menu_previous;
    gameprops_type["game_has_focus"] = &GameProps::game_has_focus;
    gameprops_type["menu_open"] = sol::property([](GameProps& gp) -> bool
                                                { return gp.menu_icon_slot != -1; });
    gameprops_type["input_index"] = &GameProps::input_index;

    lua.new_usertype<RawInput>(
        "RawInput",
        "keyboard",
        //&RawInput::keyboard,
        sol::property([](RawInput& r)
                      { return ZeroIndexArray<KeyboardKey>(r.keyboard) /**/; }),
        "controller",
        //&RawInput::controller,
        sol::property([](RawInput& r)
                      { return ZeroIndexArray<ControllerInput>(r.controller) /**/; }));
    lua.new_usertype<KeyboardKey>(
        "KeyboardKey",
        "down",
        &KeyboardKey::down,
        "pressed",
        &KeyboardKey::pressed);
    lua.new_usertype<ControllerInput>(
        "ControllerInput",
        "buttons",
        //&ControllerInput::buttons,
        sol::property([](ControllerInput& r)
                      { return ZeroIndexArray<ControllerButton>(r.buttons) /**/; }));
    lua.new_usertype<ControllerButton>(
        "ControllerButton",
        "down",
        &ControllerButton::down,
        "pressed",
        &ControllerButton::pressed);

    /// Returns RawInput, a game structure for raw keyboard and controller state
    lua["get_raw_input"] = get_raw_input;

    /// Keycodes used specifically in RawInput.keyboard. There are more keys than this though, but I didn't want to figure out the OEM keys. Because these are physical keys, SHIFT, CTRL and ALT are only mapped to the left keys.
    lua.create_named_table("RAW_KEY"
                           //, "UP", 1
                           //, "", ...check__[lua_enums.txt]\[game_data/lua_enums.txt\]...
    );

    lua["RAW_KEY"]["BACKSPACE"] = 0x28;
    lua["RAW_KEY"]["TAB"] = 0x06;
    lua["RAW_KEY"]["RETURN"] = 0x05;
    lua["RAW_KEY"]["SHIFT"] = 0x23;
    lua["RAW_KEY"]["CTRL"] = 0x21;
    lua["RAW_KEY"]["ALT"] = 0x25;
    lua["RAW_KEY"]["ESCAPE"] = 0x04;
    lua["RAW_KEY"]["SPACE"] = 0x27;
    lua["RAW_KEY"]["PGUP"] = 0x6B;
    lua["RAW_KEY"]["PGDN"] = 0x69;
    lua["RAW_KEY"]["END"] = 0x6A;
    lua["RAW_KEY"]["HOME"] = 0x6C;
    lua["RAW_KEY"]["LEFT"] = 0x02;
    lua["RAW_KEY"]["UP"] = 0x00;
    lua["RAW_KEY"]["RIGHT"] = 0x03;
    lua["RAW_KEY"]["DOWN"] = 0x01;
    lua["RAW_KEY"]["INSERT"] = 0x67;
    lua["RAW_KEY"]["DELETE"] = 0x68;
    lua["RAW_KEY"]["NUMPAD0"] = 0x4B;
    lua["RAW_KEY"]["NUMPAD1"] = 0x4C;
    lua["RAW_KEY"]["NUMPAD2"] = 0x4D;
    lua["RAW_KEY"]["NUMPAD3"] = 0x4E;
    lua["RAW_KEY"]["NUMPAD4"] = 0x4F;
    lua["RAW_KEY"]["NUMPAD5"] = 0x50;
    lua["RAW_KEY"]["NUMPAD6"] = 0x51;
    lua["RAW_KEY"]["NUMPAD7"] = 0x52;
    lua["RAW_KEY"]["NUMPAD8"] = 0x53;
    lua["RAW_KEY"]["NUMPAD9"] = 0x54;
    lua["RAW_KEY"]["MULTIPLY"] = 0x57;
    lua["RAW_KEY"]["ADD"] = 0x56;
    lua["RAW_KEY"]["SUBTRACT"] = 0x55;
    lua["RAW_KEY"]["DECIMAL"] = 0x5A;
    lua["RAW_KEY"]["DIVIDE"] = 0x58;
    lua["RAW_KEY"]["NUMPADENTER"] = 0x6E;
    lua["RAW_KEY"]["F1"] = 0x29;
    lua["RAW_KEY"]["F2"] = 0x2A;
    lua["RAW_KEY"]["F3"] = 0x2B;
    lua["RAW_KEY"]["F4"] = 0x2C;
    lua["RAW_KEY"]["F5"] = 0x2D;
    lua["RAW_KEY"]["F6"] = 0x2E;
    lua["RAW_KEY"]["F7"] = 0x2F;
    lua["RAW_KEY"]["F8"] = 0x30;
    lua["RAW_KEY"]["F9"] = 0x31;
    lua["RAW_KEY"]["F10"] = 0x32;
    lua["RAW_KEY"]["F11"] = 0x33;
    lua["RAW_KEY"]["F12"] = 0x34;
    lua["RAW_KEY"]["LSHIFT"] = 0x23;
    lua["RAW_KEY"]["RSHIFT"] = 0x24;
    lua["RAW_KEY"]["LCONTROL"] = 0x21;
    lua["RAW_KEY"]["RCONTROL"] = 0x22;
    lua["RAW_KEY"]["LALT"] = 0x25;
    lua["RAW_KEY"]["RALT"] = 0x26;
    lua["RAW_KEY"]["PLUS"] = 0x62;
    lua["RAW_KEY"]["COMMA"] = 0x5D;
    lua["RAW_KEY"]["MINUS"] = 0x5C;
    lua["RAW_KEY"]["PERIOD"] = 0x5E;
    for (char c = '0'; c <= '9'; c++)
        lua["RAW_KEY"][std::string{c}] = (int)c + 17;
    for (char c = 'A'; c <= 'Z'; c++)
        lua["RAW_KEY"][std::string{c}] = (int)c - 58;

    lua.create_named_table("RAW_BUTTON", "UP", 0, "DOWN", 1, "LEFT", 2, "RIGHT", 3, "A", 4, "B", 5, "X", 6, "Y", 7, "LEFT_SHOULDER", 8, "RIGHT_SHOULDER", 9, "LEFT_TRIGGER", 10, "RIGHT_TRIGGER", 11, "LEFT_THUMB", 12, "RIGHT_THUMB", 13, "BACK", 14, "START", 15);

    lua.create_named_table("RAW_DUALSHOCK", "UP", 0, "DOWN", 1, "LEFT", 2, "RIGHT", 3, "CROSS", 4, "CIRCLE", 5, "SQUARE", 6, "TRIANGLE", 7, "L1", 8, "R1", 9, "L2", 10, "R2", 11, "L3", 12, "R3", 13, "SHARE", 14, "OPTIONS", 15);
}
}; // namespace NGM
