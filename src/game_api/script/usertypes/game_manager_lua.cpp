#include "game_manager_lua.hpp"

#include <algorithm>   // for max
#include <new>         // for operator new
#include <sol/sol.hpp> // for data_t, basic_table_core::new_usertype
#include <string>      // for operator==, allocator
#include <tuple>       // for get
#include <type_traits> // for move, declval
#include <utility>     // for min, max

#include "game_manager.hpp" // for GameManager, JournalPopupUI, GameProps
#include "screen.hpp"       // IWYU pragma: keep

namespace NGM
{
void register_usertypes(sol::state& lua)
{
    lua.new_usertype<GameManager>(
        "GameManager",
        "game_props",
        &GameManager::game_props,
        "screen_logo",
        &GameManager::screen_logo,
        "screen_intro",
        &GameManager::screen_intro,
        "screen_prologue",
        &GameManager::screen_prologue,
        "screen_title",
        &GameManager::screen_title,
        "screen_menu",
        &GameManager::screen_menu,
        "screen_options",
        &GameManager::screen_options,
        "screen_player_profile",
        &GameManager::screen_player_profile,
        "screen_leaderboards",
        &GameManager::screen_leaderboards,
        "screen_seed_input",
        &GameManager::screen_seed_input,
        "screen_camp",
        &GameManager::screen_camp,
        "screen_level",
        &GameManager::screen_level,
        "screen_online_loading",
        &GameManager::screen_online_loading,
        "screen_online_lobby",
        &GameManager::screen_online_lobby,
        "pause_ui",
        &GameManager::pause_ui,
        "journal_ui",
        &GameManager::journal_ui,
        "save_related",
        &GameManager::save_related);
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
    lua.new_usertype<GameProps>(
        "GameProps",
        "buttons",
        &GameProps::buttons,
        "game_has_focus",
        &GameProps::game_has_focus);
}
}; // namespace NGM
