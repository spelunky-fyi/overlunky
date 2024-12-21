#include "screen_lua.hpp"

#include <algorithm>   // for max
#include <array>       // for operator==
#include <new>         // for operator new
#include <sol/sol.hpp> // for proxy_key_t, state, data_t, table_proxy
#include <string>      // for operator==, allocator, u16string
#include <string_view> // for u16string_view
#include <tuple>       // for get
#include <type_traits> // for move, declval
#include <utility>     // for min, max

#include "aliases.hpp"       // for JournalPageType, JournalPageType::Bestiary
#include "entity.hpp"        // IWYU pragma: keep
#include "particles.hpp"     // IWYU pragma: keep
#include "screen.hpp"        // for ScreenCharacterSelect, ScreenOnlineLobby
#include "screen_arena.hpp"  // for ScreenArenaStagesSelect, ScreenArenaIntro
#include "sound_manager.hpp" //

namespace NScreen
{
void register_usertypes(sol::state& lua)
{

    /// Force the journal to open on a chapter and entry# when pressing the journal button. Only use even entry numbers. Set chapter to `JOURNALUI_PAGE_SHOWN.JOURNAL` to reset. (This forces the journal toggle to always read from `game_manager.save_related.journal_popup_ui.entry_to_show` etc.)
    lua["force_journal"] = force_journal;

    /// Open or close the journal as if pressing the journal button. Will respect visible journal popups and [force_journal](#force_journal).
    lua["toggle_journal"] = toggle_journal;

    /// Open the journal on a chapter and page. The main Journal spread is pages 0..1, so most chapters start at 2. Use even page numbers only.
    lua["show_journal"] = show_journal;

    auto screenpanels_type = lua.new_usertype<MenuScreenPanels>("MenuScreenPanels");
    screenpanels_type["woodpanels_velocity"] = &MenuScreenPanels::woodpanels_velocity;
    screenpanels_type["woodpanels_progress"] = &MenuScreenPanels::woodpanels_progress;
    screenpanels_type["scroll_unfurl_progress"] = &MenuScreenPanels::scroll_unfurl_progress;
    screenpanels_type["bottom_woodpanel_speed_multiplayer"] = &MenuScreenPanels::bottom_woodpanel_speed_multiplayer;
    screenpanels_type["bottom_woodpanel_y_offset"] = &MenuScreenPanels::bottom_woodpanel_y_offset;
    screenpanels_type["bottom_woodpanel"] = &MenuScreenPanels::bottom_woodpanel;
    screenpanels_type["top_woodpanel"] = &MenuScreenPanels::top_woodpanel;
    screenpanels_type["scroll"] = &MenuScreenPanels::scroll;
    screenpanels_type["top_woodpanel_left_scrollhandle"] = &MenuScreenPanels::top_woodpanel_left_scrollhandle;
    screenpanels_type["top_woodpanel_right_scrollhandle"] = &MenuScreenPanels::top_woodpanel_right_scrollhandle;
    screenpanels_type["scroll_text"] = &MenuScreenPanels::scroll_text;
    screenpanels_type["bottom_left_text"] = &MenuScreenPanels::bottom_left_text;
    screenpanels_type["bottom_right_text"] = &MenuScreenPanels::bottom_right_text;
    screenpanels_type["bottom_middle_text"] = &MenuScreenPanels::bottom_middle_text;
    screenpanels_type["top_woodpanel_visible"] = &MenuScreenPanels::top_woodpanel_visible;
    screenpanels_type["bottom_woodpanel_visible"] = &MenuScreenPanels::bottom_woodpanel_visible;
    screenpanels_type["toggle_woodpanel_slidein_animation"] = &MenuScreenPanels::toggle_woodpanel_slidein_animation;
    screenpanels_type["capitalize_scroll_text"] = &MenuScreenPanels::capitalize_scroll_text;

    lua.new_usertype<ScreenControls>(
        "ScreenControls",
        "up",
        &ScreenControls::up,
        "down",
        &ScreenControls::down,
        "left",
        &ScreenControls::left,
        "right",
        &ScreenControls::right,
        "direction_input",
        &ScreenControls::direction_input,
        "hold_down_timer",
        &ScreenControls::hold_down_timer,
        "fast_scroll_timer",
        &ScreenControls::fast_scroll_timer);

    lua.new_usertype<Screen>(
        "Screen",
        "render_timer",
        &Screen::render_timer,
        "init",
        &Screen::init);

    lua["Screen"]["as_screen_logo"] = &Screen::as<ScreenLogo>;
    lua["Screen"]["as_screen_intro"] = &Screen::as<ScreenIntro>;
    lua["Screen"]["as_screen_prologue"] = &Screen::as<ScreenPrologue>;
    lua["Screen"]["as_screen_title"] = &Screen::as<ScreenTitle>;
    lua["Screen"]["as_screen_menu"] = &Screen::as<ScreenMenu>;
    lua["Screen"]["as_screen_options"] = &Screen::as<ScreenOptions>;
    lua["Screen"]["as_screen_seed_input"] = &Screen::as<ScreenCodeInput>;
    lua["Screen"]["as_screen_character_select"] = &Screen::as<ScreenCharacterSelect>;
    lua["Screen"]["as_screen_team_select"] = &Screen::as<ScreenTeamSelect>;
    lua["Screen"]["as_screen_camp"] = &Screen::as<ScreenCamp>;
    lua["Screen"]["as_screen_state_camp"] = &Screen::as<ScreenStateCamp>; // probably not needed
    lua["Screen"]["as_screen_level"] = &Screen::as<ScreenLevel>;
    lua["Screen"]["as_screen_state_level"] = &Screen::as<ScreenStateLevel>; // probably not needed
    lua["Screen"]["as_screen_transition"] = &Screen::as<ScreenTransition>;
    lua["Screen"]["as_screen_death"] = &Screen::as<ScreenDeath>;
    lua["Screen"]["as_screen_win"] = &Screen::as<ScreenWin>;
    lua["Screen"]["as_screen_credits"] = &Screen::as<ScreenCredits>;
    lua["Screen"]["as_screen_scores"] = &Screen::as<ScreenScores>;
    lua["Screen"]["as_screen_constellation"] = &Screen::as<ScreenConstellation>;
    lua["Screen"]["as_screen_recap"] = &Screen::as<ScreenRecap>;
    lua["Screen"]["as_screen_arena_menu"] = &Screen::as<ScreenArenaMenu>;
    lua["Screen"]["as_screen_arena_stages"] = &Screen::as<ScreenArenaStagesSelect>;
    lua["Screen"]["as_screen_arena_items"] = &Screen::as<ScreenArenaItems>;
    lua["Screen"]["as_screen_arena_select"] = &Screen::as<ScreenArenaStagesSelect>;
    lua["Screen"]["as_screen_arena_intro"] = &Screen::as<ScreenArenaIntro>;
    lua["Screen"]["as_screen_arena_level"] = &Screen::as<ScreenArenaLevel>;
    lua["Screen"]["as_screen_arena_score"] = &Screen::as<ScreenArenaScore>;
    lua["Screen"]["as_screen_online_loading"] = &Screen::as<ScreenOnlineLoading>;
    lua["Screen"]["as_screen_online_lobby"] = &Screen::as<ScreenOnlineLobby>;

    lua.new_usertype<ScreenLogo>(
        "ScreenLogo",
        "logo_mossmouth",
        &ScreenLogo::logo_mossmouth,
        "logo_blitworks",
        &ScreenLogo::logo_blitworks,
        "logo_fmod",
        &ScreenLogo::logo_fmod,
        "state",
        &ScreenLogo::state,
        "timer",
        &ScreenLogo::timer,
        sol::base_classes,
        sol::bases<Screen>());

    lua.new_usertype<ScreenIntro>(
        "ScreenIntro",
        "blackout_background",
        &ScreenIntro::blackout_background,
        "blackout_alpha",
        &ScreenIntro::blackout_alpha,
        "active",
        &ScreenIntro::active,
        "skip_prologue",
        &ScreenIntro::skip_prologue,
        sol::base_classes,
        sol::bases<Screen>());

    lua.new_usertype<ScreenPrologue>(
        "ScreenPrologue",
        "line1",
        &ScreenPrologue::line1,
        "line2",
        &ScreenPrologue::line2,
        "line3",
        &ScreenPrologue::line3,
        "line1_alpha",
        &ScreenPrologue::line1_alpha,
        "line2_alpha",
        &ScreenPrologue::line2_alpha,
        "line3_alpha",
        &ScreenPrologue::line3_alpha,
        sol::base_classes,
        sol::bases<Screen>());

    lua.new_usertype<ScreenTitle>(
        "ScreenTitle",
        "logo_spelunky2",
        &ScreenTitle::logo_spelunky2,
        "ana",
        &ScreenTitle::ana,
        "ana_right_eyeball_torch_reflection",
        &ScreenTitle::ana_right_eyeball_torch_reflection,
        "ana_left_eyeball_torch_reflection",
        &ScreenTitle::ana_left_eyeball_torch_reflection,
        "particle_torchflame_smoke",
        &ScreenTitle::particle_torchflame_smoke,
        "particle_torchflame_backflames",
        &ScreenTitle::particle_torchflame_backflames,
        "particle_torchflame_flames",
        &ScreenTitle::particle_torchflame_flames,
        "particle_torchflame_backflames_animated",
        &ScreenTitle::particle_torchflame_backflames_animated,
        "particle_torchflame_flames_animated",
        &ScreenTitle::particle_torchflame_flames_animated,
        "particle_torchflame_ash",
        &ScreenTitle::particle_torchflame_ash,
        "brightness",
        &ScreenTitle::brightness,
        "music",
        &ScreenTitle::music,
        "torch_sound",
        &ScreenTitle::torch_sound,
        sol::base_classes,
        sol::bases<Screen>());

    lua.new_usertype<SpritePosition>(
        "SpritePosition",
        "column",
        &SpritePosition::column,
        "row",
        &SpritePosition::row);

    auto screenmenu_type = lua.new_usertype<ScreenMenu>("ScreenMenu", sol::base_classes, sol::bases<Screen>());
    screenmenu_type["state"] = &ScreenMenu::state;
    screenmenu_type["tunnel_background"] = &ScreenMenu::tunnel_background;
    screenmenu_type["cthulhu_disc"] = &ScreenMenu::cthulhu_disc;
    screenmenu_type["tunnel_ring_darkbrown"] = &ScreenMenu::tunnel_ring_darkbrown;
    screenmenu_type["cthulhu_body"] = &ScreenMenu::cthulhu_body;
    screenmenu_type["tunnel_ring_lightbrown"] = &ScreenMenu::tunnel_ring_lightbrown;
    screenmenu_type["vine_left"] = &ScreenMenu::vine_left;
    screenmenu_type["vine_right"] = &ScreenMenu::vine_right;
    screenmenu_type["skull_left"] = &ScreenMenu::skull_left;
    screenmenu_type["salamander_right"] = &ScreenMenu::salamander_right;
    screenmenu_type["left_spear"] = &ScreenMenu::left_spear;
    screenmenu_type["right_spear"] = &ScreenMenu::right_spear;
    screenmenu_type["spear_dangler_related"] = &ScreenMenu::spear_dangler_related;
    screenmenu_type["play_scroll"] = &ScreenMenu::play_scroll;
    screenmenu_type["info_toast"] = &ScreenMenu::info_toast;
    screenmenu_type["cthulhu_sound"] = &ScreenMenu::cthulhu_sound;
    screenmenu_type["particle_smoke"] = &ScreenMenu::particle_smoke;
    screenmenu_type["particle_rubble"] = &ScreenMenu::particle_rubble;
    screenmenu_type["cthulhu_disc_ring_angle"] = &ScreenMenu::cthulhu_disc_ring_angle;
    screenmenu_type["cthulhu_disc_split_progress"] = &ScreenMenu::cthulhu_disc_split_progress;
    screenmenu_type["cthulhu_disc_y"] = &ScreenMenu::cthulhu_disc_y;
    screenmenu_type["cthulhu_timer"] = &ScreenMenu::cthulhu_timer;
    screenmenu_type["controls"] = &ScreenMenu::controls;
    screenmenu_type["selected_menu_index"] = &ScreenMenu::selected_menu_index;
    screenmenu_type["sides_hold_down_timer"] = &ScreenMenu::sides_hold_down_timer;
    screenmenu_type["sides_fast_scroll_timer"] = &ScreenMenu::sides_fast_scroll_timer;
    screenmenu_type["loop"] = &ScreenMenu::loop;
    screenmenu_type["menu_id"] = &ScreenMenu::menu_id;
    screenmenu_type["transfer_to_menu_id"] = &ScreenMenu::transfer_to_menu_id;
    screenmenu_type["menu_text_opacity"] = &ScreenMenu::menu_text_opacity;
    screenmenu_type["spear_position"] = &ScreenMenu::spear_position;
    screenmenu_type["spear_dangler"] = &ScreenMenu::spear_dangler;
    screenmenu_type["spear_dangle_momentum"] = &ScreenMenu::spear_dangle_momentum;
    screenmenu_type["spear_dangle_angle"] = &ScreenMenu::spear_dangle_angle;
    screenmenu_type["play_scroll_descend_timer"] = &ScreenMenu::play_scroll_descend;
    screenmenu_type["scroll_text"] = &ScreenMenu::scroll_text;
    screenmenu_type["shake_offset_x"] = &ScreenMenu::shake_offset_x;
    screenmenu_type["shake_offset_y"] = &ScreenMenu::shake_offset_y;
    screenmenu_type["loaded_once"] = &ScreenMenu::loaded_once;

    auto screenoptions_type = lua.new_usertype<ScreenOptions>("ScreenOptions", sol::base_classes, sol::bases<Screen>());
    screenoptions_type["down"] = &ScreenOptions::down;
    screenoptions_type["up"] = &ScreenOptions::up;
    screenoptions_type["direction_input"] = &ScreenOptions::direction_input;
    screenoptions_type["hold_down_timer"] = &ScreenOptions::hold_down_timer;
    screenoptions_type["fast_scroll_timer"] = &ScreenOptions::fast_scroll_timer;
    screenoptions_type["selected_menu_index"] = &ScreenOptions::selected_menu_index;
    screenoptions_type["sides_hold_down_timer"] = &ScreenOptions::sides_hold_down_timer;
    screenoptions_type["sides_fast_scroll_timer"] = &ScreenOptions::sides_fast_scroll_timer;
    screenoptions_type["loop"] = &ScreenOptions::loop;
    screenoptions_type["screen_panels"] = &ScreenOptions::screen_panels;
    screenoptions_type["menu_id"] = &ScreenOptions::current_menu_id;
    screenoptions_type["transfer_to_menu_id"] = &ScreenOptions::transfer_to_menu_id;
    screenoptions_type["show_apply_button"] = &ScreenOptions::show_apply_button;
    screenoptions_type["topleft_woodpanel_esc"] = &ScreenOptions::topleft_woodpanel_esc;
    screenoptions_type["brick_background"] = &ScreenOptions::brick_background;
    screenoptions_type["brick_middlelayer"] = &ScreenOptions::brick_middlelayer;
    screenoptions_type["brick_foreground"] = &ScreenOptions::brick_foreground;
    screenoptions_type["selected_item_rounded_rect"] = &ScreenOptions::selected_item_rounded_rect;
    screenoptions_type["selected_item_scarab"] = &ScreenOptions::selected_item_scarab;
    screenoptions_type["item_option_arrow_left"] = &ScreenOptions::item_option_arrow_left;
    screenoptions_type["item_option_arrow_right"] = &ScreenOptions::item_option_arrow_right;
    screenoptions_type["tooltip_background"] = &ScreenOptions::tooltip_background;
    screenoptions_type["progressbar_background"] = &ScreenOptions::progressbar_background;
    screenoptions_type["volume_progressbar_foreground"] = &ScreenOptions::volume_progressbar_foreground;
    screenoptions_type["progressbar_foreground"] = &ScreenOptions::progressbar_foreground;
    screenoptions_type["volume_progressbar_position_indicator"] = &ScreenOptions::volume_progressbar_position_indicator;
    screenoptions_type["sectionheader_background"] = &ScreenOptions::sectionheader_background;
    screenoptions_type["pet_icons"] = &ScreenOptions::pet_icons;
    screenoptions_type["bottom_scroll"] = &ScreenOptions::bottom_scroll;
    screenoptions_type["bottom_left_scrollhandle"] = &ScreenOptions::bottom_left_scrollhandle;
    screenoptions_type["bottom_right_scrollhandle"] = &ScreenOptions::bottom_right_scrollhandle;
    screenoptions_type["topleft_woodpanel_esc_slidein"] = &ScreenOptions::topleft_woodpanel_esc_slidein;
    screenoptions_type["text_fadein"] = &ScreenOptions::text_fadein;
    screenoptions_type["vertical_scroll_effect"] = &ScreenOptions::vertical_scroll_effect;
    screenoptions_type["options_visiable"] = &ScreenOptions::options_visiable;
    screenoptions_type["show_highlight"] = &ScreenOptions::show_highlight;
    screenoptions_type["tooltip_text"] = &ScreenOptions::tooltip_text;
    screenoptions_type["disable_controls"] = &ScreenOptions::disable_controls;
    screenoptions_type["sync_progress_state"] = &ScreenOptions::sync_progress_state;
    screenoptions_type["credits_progression"] = &ScreenOptions::credits_progression;

    auto screenseedinput_type = lua.new_usertype<ScreenCodeInput>("ScreenCodeInput", sol::base_classes, sol::bases<Screen>());
    screenseedinput_type["screen_panels"] = &ScreenCodeInput::screen_panels;
    screenseedinput_type["allow_random"] = &ScreenCodeInput::allow_random;
    screenseedinput_type["selected_button_index"] = &ScreenCodeInput::selected_button_index;
    screenseedinput_type["pressed_select"] = &ScreenCodeInput::pressed_select;
    screenseedinput_type["topleft_woodpanel_esc_slidein"] = &ScreenCodeInput::topleft_woodpanel_esc_slidein;
    screenseedinput_type["scroll_text_id"] = &ScreenCodeInput::scroll_text_id;
    screenseedinput_type["start_text_id"] = &ScreenCodeInput::start_text_id;
    screenseedinput_type["main_woodpanel_left_border"] = &ScreenCodeInput::main_woodpanel_left_border;
    screenseedinput_type["main_woodpanel_center"] = &ScreenCodeInput::main_woodpanel_center;
    screenseedinput_type["main_woodpanel_right_border"] = &ScreenCodeInput::main_woodpanel_right_border;
    screenseedinput_type["top_scroll"] = &ScreenCodeInput::top_scroll;
    screenseedinput_type["letter_cutouts"] = &ScreenCodeInput::letter_cutouts;
    screenseedinput_type["hand_pointer"] = &ScreenCodeInput::hand_pointer;
    screenseedinput_type["key_background"] = &ScreenCodeInput::key_background;
    screenseedinput_type["topleft_woodpanel_esc"] = &ScreenCodeInput::topleft_woodpanel_esc;
    screenseedinput_type["start_sidepanel"] = &ScreenCodeInput::start_sidepanel;
    screenseedinput_type["start_sidepanel_slidein"] = &ScreenCodeInput::start_sidepanel_slidein;
    screenseedinput_type["seed_length"] = &ScreenCodeInput::code_length;
    screenseedinput_type["get_seed"] = &ScreenCodeInput::get_seed;
    screenseedinput_type["set_seed"] = &ScreenCodeInput::set_seed;

    lua.new_usertype<FlyingThing>(
        "FlyingThing",
        "texture_info",
        &FlyingThing::texture_info,
        "entity_type",
        &FlyingThing::entity_type,
        "spritesheet_column",
        &FlyingThing::spritesheet_column,
        "spritesheet_row",
        &FlyingThing::spritesheet_row,
        "spritesheet_animation_length",
        &FlyingThing::spritesheet_animation_length,
        "velocity_x",
        &FlyingThing::velocity_x,
        "amplitude",
        &FlyingThing::amplitude,
        "frequency",
        &FlyingThing::frequency,
        "sinewave_angle",
        &FlyingThing::sinewave_angle);

    auto screencharacterselect_type = lua.new_usertype<ScreenCharacterSelect>("ScreenCharacterSelect", sol::base_classes, sol::bases<Screen>());
    screencharacterselect_type["main_background_zoom_progress"] = &ScreenCharacterSelect::main_background_zoom_progress;
    screencharacterselect_type["main_background_zoom_target"] = &ScreenCharacterSelect::main_background_zoom_target;
    screencharacterselect_type["blurred_border_zoom_progress"] = &ScreenCharacterSelect::blurred_border_zoom_progress;
    screencharacterselect_type["blurred_border_zoom_target"] = &ScreenCharacterSelect::blurred_border_zoom_target;
    screencharacterselect_type["screen_panels"] = &ScreenCharacterSelect::screen_panels;
    screencharacterselect_type["mine_entrance_background"] = &ScreenCharacterSelect::mine_entrance_background;
    screencharacterselect_type["character"] = &ScreenCharacterSelect::character;
    screencharacterselect_type["character_shadow"] = &ScreenCharacterSelect::character_shadow;
    screencharacterselect_type["character_flag"] = &ScreenCharacterSelect::character_flag;
    screencharacterselect_type["character_left_arrow"] = &ScreenCharacterSelect::character_left_arrow;
    screencharacterselect_type["character_right_arrow"] = &ScreenCharacterSelect::character_right_arrow;
    screencharacterselect_type["mine_entrance_border"] = &ScreenCharacterSelect::mine_entrance_border;
    screencharacterselect_type["mine_entrance_shutter"] = &ScreenCharacterSelect::mine_entrance_shutter;
    screencharacterselect_type["background"] = &ScreenCharacterSelect::background;
    screencharacterselect_type["blurred_border"] = &ScreenCharacterSelect::blurred_border;
    screencharacterselect_type["blurred_border2"] = &ScreenCharacterSelect::blurred_border2;
    screencharacterselect_type["topleft_woodpanel_esc"] = &ScreenCharacterSelect::topleft_woodpanel_esc;
    screencharacterselect_type["start_sidepanel"] = &ScreenCharacterSelect::start_sidepanel;
    screencharacterselect_type["quick_select_panel"] = &ScreenCharacterSelect::quick_select_panel;
    screencharacterselect_type["quick_select_selected_char_background"] = &ScreenCharacterSelect::quick_select_selected_char_background;
    screencharacterselect_type["quick_select_panel_related"] = &ScreenCharacterSelect::quick_select_panel_related;
    screencharacterselect_type["player_shutter_timer"] = &ScreenCharacterSelect::player_shutter_timer;
    screencharacterselect_type["player_x"] = &ScreenCharacterSelect::player_x;
    screencharacterselect_type["player_y"] = &ScreenCharacterSelect::player_y;
    screencharacterselect_type["player_arrow_slidein_timer"] = &ScreenCharacterSelect::player_arrow_slidein_timer;
    screencharacterselect_type["player_facing_left"] = &ScreenCharacterSelect::player_facing_left;
    screencharacterselect_type["player_quickselect_shown"] = &ScreenCharacterSelect::player_quickselect_shown;
    screencharacterselect_type["player_quickselect_fadein_timer"] = &ScreenCharacterSelect::player_quickselect_fadein_timer;
    screencharacterselect_type["player_quickselect_coords"] = &ScreenCharacterSelect::player_quickselect_coords;
    screencharacterselect_type["player_quickselect_wiggle_angle"] = &ScreenCharacterSelect::player_quickselect_wiggle_angle;
    screencharacterselect_type["topleft_woodpanel_esc_slidein"] = &ScreenCharacterSelect::topleft_woodpanel_esc_slidein;
    screencharacterselect_type["start_panel_slidein"] = &ScreenCharacterSelect::start_panel_slidein;
    screencharacterselect_type["action_buttons_keycap_size"] = &ScreenCharacterSelect::action_buttons_keycap_size;
    screencharacterselect_type["screen_loading"] = &ScreenCharacterSelect::screen_loading;
    screencharacterselect_type["seeded_run"] = &ScreenCharacterSelect::seeded_run;
    screencharacterselect_type["daily_challenge"] = sol::readonly(&ScreenCharacterSelect::daily_challenge);
    /// Short for `screen->next_screen_to_load == SCREEN.TEAM_SELECT and not screen->seeded_run and not screen->daily_challenge`
    screencharacterselect_type["arena"] = sol::property([](ScreenCharacterSelect* screen) -> bool
                                                        { return screen->next_screen_to_load == 10 && !screen->seeded_run && !screen->daily_challenge; });
    screencharacterselect_type["next_screen_to_load"] = &ScreenCharacterSelect::next_screen_to_load;
    screencharacterselect_type["not_ready_to_start_yet"] = &ScreenCharacterSelect::not_ready_to_start_yet;
    screencharacterselect_type["available_mine_entrances"] = &ScreenCharacterSelect::available_mine_entrances;
    screencharacterselect_type["amount_of_mine_entrances_activated"] = &ScreenCharacterSelect::amount_of_mine_entrances_activated;
    screencharacterselect_type["screen_blackout"] = &ScreenCharacterSelect::screen_blackout;
    screencharacterselect_type["blackout_transparency"] = &ScreenCharacterSelect::blackout_transparency;
    screencharacterselect_type["start_pressed"] = &ScreenCharacterSelect::start_pressed;
    screencharacterselect_type["transition_to_game_started"] = &ScreenCharacterSelect::transition_to_game_started;
    screencharacterselect_type["disable_controls"] = &ScreenCharacterSelect::disable_controls;
    screencharacterselect_type["flying_things"] = &ScreenCharacterSelect::flying_things;
    screencharacterselect_type["flying_thing_countdown"] = &ScreenCharacterSelect::flying_thing_countdown;
    screencharacterselect_type["particle_ceilingdust_smoke"] = &ScreenCharacterSelect::particle_ceilingdust_smoke;
    screencharacterselect_type["particle_ceilingdust_rubble"] = &ScreenCharacterSelect::particle_ceilingdust_rubble;
    screencharacterselect_type["particle_mist"] = &ScreenCharacterSelect::particle_mist;
    screencharacterselect_type["particle_torchflame_smoke1"] = &ScreenCharacterSelect::particle_torchflame_smoke1;
    screencharacterselect_type["particle_torchflame_flames1"] = &ScreenCharacterSelect::particle_torchflame_flames1;
    screencharacterselect_type["particle_torchflame_smoke2"] = &ScreenCharacterSelect::particle_torchflame_smoke2;
    screencharacterselect_type["particle_torchflame_flames2"] = &ScreenCharacterSelect::particle_torchflame_flames2;
    screencharacterselect_type["particle_torchflame_smoke3"] = &ScreenCharacterSelect::particle_torchflame_smoke3;
    screencharacterselect_type["particle_torchflame_flames3"] = &ScreenCharacterSelect::particle_torchflame_flames3;
    screencharacterselect_type["particle_torchflame_smoke4"] = &ScreenCharacterSelect::particle_torchflame_smoke4;
    screencharacterselect_type["particle_torchflame_flames4"] = &ScreenCharacterSelect::particle_torchflame_flames4;
    screencharacterselect_type["torch_sound"] = &ScreenCharacterSelect::torch_sound;
    screencharacterselect_type["buttons"] = &ScreenCharacterSelect::buttons;

    auto screenteamselect_type = lua.new_usertype<ScreenTeamSelect>("ScreenTeamSelect", sol::base_classes, sol::bases<Screen>());
    screenteamselect_type["player_portrait"] = &ScreenTeamSelect::player_portrait;
    screenteamselect_type["scroll_bottom_left"] = &ScreenTeamSelect::scroll_bottom_left;
    screenteamselect_type["scrollend_bottom_left"] = &ScreenTeamSelect::scrollend_bottom_left;
    screenteamselect_type["four_ropes"] = &ScreenTeamSelect::four_ropes;
    screenteamselect_type["gems_above_the_ropes"] = &ScreenTeamSelect::gems_above_the_ropes;
    screenteamselect_type["four_characters"] = &ScreenTeamSelect::four_characters;
    screenteamselect_type["left_arrow"] = &ScreenTeamSelect::left_arrow;
    screenteamselect_type["right_arrow"] = &ScreenTeamSelect::right_arrow;
    screenteamselect_type["start_panel"] = &ScreenTeamSelect::start_panel;
    screenteamselect_type["go_back_wooden_panel"] = &ScreenTeamSelect::go_back_wooden_panel;
    screenteamselect_type["start_panel_slide"] = &ScreenTeamSelect::start_panel_slide;
    screenteamselect_type["go_back_wooden_panel_slide"] = &ScreenTeamSelect::go_back_wooden_panel_slide;
    screenteamselect_type["pulsating_arrows_timer"] = &ScreenTeamSelect::pulsating_arrows_timer;
    screenteamselect_type["selected_player"] = &ScreenTeamSelect::selected_player;
    screenteamselect_type["buttons"] = &ScreenTeamSelect::buttons;
    screenteamselect_type["ready"] = &ScreenTeamSelect::ready;

    lua.new_usertype<ScreenCamp>(
        "ScreenCamp",
        "buttons",
        &ScreenCamp::buttons,
        sol::base_classes,
        sol::bases<Screen>());

    lua.new_usertype<ScreenStateCamp>(
        "ScreenStateCamp",
        "time_till_reset",
        &ScreenStateCamp::time_till_reset,
        sol::base_classes,
        sol::bases<Screen>());

    lua.new_usertype<ScreenLevel>(
        "ScreenLevel",
        "buttons",
        &ScreenLevel::buttons,
        sol::base_classes,
        sol::bases<Screen>());

    lua.new_usertype<ScreenStateLevel>(
        "ScreenStateLevel",
        "buttons",
        &ScreenStateLevel::buttons,
        "time_till_death_screen",
        &ScreenStateLevel::time_till_death_screen,
        sol::base_classes,
        sol::bases<Screen>());

    auto screentransition_type = lua.new_usertype<ScreenTransition>("ScreenTransition", sol::base_classes, sol::bases<Screen>());
    screentransition_type["woodpanel_pos"] = &ScreenTransition::woodpanel_pos;
    screentransition_type["stats_scroll_horizontal_posaa"] = &ScreenTransition::stats_scroll_horizontal_pos;
    screentransition_type["stats_scroll_vertical_pos"] = &ScreenTransition::stats_scroll_vertical_pos;
    screentransition_type["level_completed_pos"] = &ScreenTransition::level_completed_pos;
    screentransition_type["stats_scroll_unfurl_actualvalue"] = &ScreenTransition::stats_scroll_unfurl_actualvalue;
    screentransition_type["stats_scroll_unfurl_targetvalue"] = &ScreenTransition::stats_scroll_unfurl_targetvalue;
    screentransition_type["woodpanel1"] = &ScreenTransition::woodpanel1;
    screentransition_type["woodpanel2"] = &ScreenTransition::woodpanel2;
    screentransition_type["woodpanel3"] = &ScreenTransition::woodpanel3;
    screentransition_type["woodpanel_cutout1"] = &ScreenTransition::woodpanel_cutout1;
    screentransition_type["woodpanel_cutout2"] = &ScreenTransition::woodpanel_cutout2;
    screentransition_type["woodpanel_cutout3"] = &ScreenTransition::woodpanel_cutout3;
    screentransition_type["woodplank"] = &ScreenTransition::woodplank;
    screentransition_type["woodpanel_bottomcutout1"] = &ScreenTransition::woodpanel_bottomcutout1;
    screentransition_type["woodpanel_bottomcutout2"] = &ScreenTransition::woodpanel_bottomcutout2;
    screentransition_type["woodpanel_bottomcutout3"] = &ScreenTransition::woodpanel_bottomcutout3;
    screentransition_type["scroll"] = &ScreenTransition::scroll;
    screentransition_type["stats_scroll_top_bottom"] = &ScreenTransition::stats_scroll_top_bottom;
    screentransition_type["killcount_rounded_rect"] = &ScreenTransition::killcount_rounded_rect;
    screentransition_type["level_completed_panel"] = &ScreenTransition::level_completed_panel;
    screentransition_type["stats_scroll_state_1"] = &ScreenTransition::stats_scroll_state_1;
    screentransition_type["stats_scroll_state_2"] = &ScreenTransition::stats_scroll_state_2;
    screentransition_type["hide_press_to_go_next_level"] = &ScreenTransition::hide_press_to_go_next_level;
    screentransition_type["mama_tunnel"] = &ScreenTransition::mama_tunnel;
    screentransition_type["speechbubble"] = &ScreenTransition::speechbubble;
    screentransition_type["speechbubble_arrow"] = &ScreenTransition::speechbubble_arrow;
    screentransition_type["mama_tunnel_fade_targetvalue"] = &ScreenTransition::mama_tunnel_fade_actualvalue;
    screentransition_type["mama_tunnel_fade_targetvalue"] = &ScreenTransition::mama_tunnel_fade_targetvalue;
    screentransition_type["mama_tunnel_text_id"] = &ScreenTransition::mama_tunnel_text_id;
    screentransition_type["mama_tunnel_choice_visible"] = &ScreenTransition::mama_tunnel_choice_visible;
    screentransition_type["mama_tunnel_agree_with_gift"] = &ScreenTransition::mama_tunnel_agree_with_gift;
    screentransition_type["mama_tunnel_face_invisible"] = &ScreenTransition::mama_tunnel_face_invisible;
    screentransition_type["mama_tunnel_face_transparency"] = &ScreenTransition::mama_tunnel_face_transparency;
    screentransition_type["mama_tunnel_agree_panel"] = &ScreenTransition::mama_tunnel_agree_panel;
    screentransition_type["mama_tunnel_agree_panel_indicator"] = &ScreenTransition::mama_tunnel_agree_panel_indicator;
    screentransition_type["woodpanel_cutout_big_money1"] = &ScreenTransition::woodpanel_cutout_big_money1;
    screentransition_type["woodpanel_cutout_big_money2"] = &ScreenTransition::woodpanel_cutout_big_money2;
    screentransition_type["woodpanel_cutout_big_money3"] = &ScreenTransition::woodpanel_cutout_big_money3;
    screentransition_type["big_dollar_sign"] = &ScreenTransition::big_dollar_sign;
    screentransition_type["stats_scroll_unfurl_sequence"] = &ScreenTransition::stats_scroll_unfurl_sequence;
    screentransition_type["player_stats_scroll_numeric_value"] = &ScreenTransition::player_stats_scroll_numeric_value;
    screentransition_type["player_secondary_icon"] = &ScreenTransition::player_secondary_icon;
    screentransition_type["player_icon"] = &ScreenTransition::player_icon;
    screentransition_type["player_secondary_icon_type"] = &ScreenTransition::player_secondary_icon_type;
    screentransition_type["player_icon_index"] = &ScreenTransition::player_icon_index;
    screentransition_type["hourglasses"] = &ScreenTransition::hourglasses;
    screentransition_type["small_dollar_signs"] = &ScreenTransition::small_dollar_signs;
    screentransition_type["this_level_money_color"] = &ScreenTransition::this_level_money_color;
    screentransition_type["buttons"] = &ScreenTransition::buttons;

    lua.new_usertype<ScreenDeath>(
        "ScreenDeath",
        sol::base_classes,
        sol::bases<Screen>());

    lua.new_usertype<ScreenWin>(
        "ScreenWin",
        "sequence_timer",
        &ScreenWin::sequence_timer,
        "frame_timer",
        &ScreenWin::frame_timer,
        "animation_state",
        &ScreenWin::animation_state,
        "rescuing_ship_entity",
        &ScreenWin::rescuing_ship_entity,
        sol::base_classes,
        sol::bases<Screen>());

    lua.new_usertype<ScreenCredits>(
        "ScreenCredits",
        "credits_progression",
        &ScreenCredits::credits_progression,
        "bg_music_info",
        &ScreenCredits::bg_music_info,
        sol::base_classes,
        sol::bases<Screen>());

    lua.new_usertype<ScreenScores>(
        "ScreenScores",
        "animation_state",
        &ScreenScores::animation_state,
        "woodpanel1",
        &ScreenScores::woodpanel1,
        "woodpanel2",
        &ScreenScores::woodpanel2,
        "woodpanel3",
        &ScreenScores::woodpanel3,
        "woodpanel_cutout",
        &ScreenScores::woodpanel_cutout,
        "dollarsign",
        &ScreenScores::dollarsign,
        "hourglass",
        &ScreenScores::hourglass,
        "animation_timer",
        &ScreenScores::animation_timer,
        "woodpanel_slidedown_timer",
        &ScreenScores::woodpanel_slidedown_timer,
        sol::base_classes,
        sol::bases<Screen>());

    lua.new_usertype<ScreenConstellation>(
        "ScreenConstellation",
        "sequence_state",
        &ScreenConstellation::sequence_state,
        "animation_timer",
        &ScreenConstellation::animation_timer,
        "constellation_text_opacity",
        &ScreenConstellation::constellation_text_opacity,
        "constellation_text",
        sol::property([](ScreenConstellation& s) -> std::u16string_view
                      {
                          std::u16string_view str(s.constellation_text) /**/;
                          return str; },
                      [](ScreenConstellation& s, std::u16string new_str)
                      {
                          const char16_t* src = new_str.c_str() /**/;
                          char16_t* temp = s.constellation_text;
                          unsigned int n = 0;
                          while ((*temp++ = *src++) != 0)
                              if (n++ >= 30)
                                  break;
                      }),
        "explosion_and_loop",
        &ScreenConstellation::explosion_and_loop,
        "music",
        &ScreenConstellation::music,
        sol::base_classes,
        sol::bases<Screen>());

    lua.new_usertype<ScreenRecap>(
        "ScreenRecap",
        sol::base_classes,
        sol::bases<Screen>());

    lua.new_usertype<ScreenOnlineLoading>(
        "ScreenOnlineLoading",
        "ouroboros",
        &ScreenOnlineLoading::ouroboros,
        "ouroboros_angle",
        &ScreenOnlineLoading::ouroboros_angle,
        sol::base_classes,
        sol::bases<Screen>());

    lua.new_usertype<OnlineLobbyScreenPlayer>(
        "OnlineLobbyScreenPlayer",
        "platform_icon",
        &OnlineLobbyScreenPlayer::platform_icon,
        "character",
        &OnlineLobbyScreenPlayer::character,
        "ready",
        &OnlineLobbyScreenPlayer::ready,
        "searching",
        &OnlineLobbyScreenPlayer::searching);

    auto screenonlinelobby_type = lua.new_usertype<ScreenOnlineLobby>("ScreenOnlineLobby", sol::base_classes, sol::bases<Screen>());
    screenonlinelobby_type["screen_panels"] = &ScreenOnlineLobby::screen_panels;
    screenonlinelobby_type["players"] = &ScreenOnlineLobby::players;
    screenonlinelobby_type["background_image"] = &ScreenOnlineLobby::background_image;
    screenonlinelobby_type["topleft_woodpanel_esc"] = &ScreenOnlineLobby::topleft_woodpanel_esc;
    screenonlinelobby_type["topleft_woodpanel_esc_slidein"] = &ScreenOnlineLobby::topleft_woodpanel_esc_slidein;
    screenonlinelobby_type["character_walk_offset"] = &ScreenOnlineLobby::character_walk_offset;
    screenonlinelobby_type["character_facing_left"] = &ScreenOnlineLobby::character_facing_left;
    screenonlinelobby_type["move_direction"] = &ScreenOnlineLobby::move_direction;
    screenonlinelobby_type["character"] = &ScreenOnlineLobby::character;
    screenonlinelobby_type["player_ready_icon"] = &ScreenOnlineLobby::player_ready_icon;
    screenonlinelobby_type["arrow_left"] = &ScreenOnlineLobby::arrow_left;
    screenonlinelobby_type["arrow_right"] = &ScreenOnlineLobby::arrow_right;
    screenonlinelobby_type["arrow_left_hor_offset"] = &ScreenOnlineLobby::arrow_left_hor_offset;
    screenonlinelobby_type["arrow_right_hor_offset"] = &ScreenOnlineLobby::arrow_right_hor_offset;
    screenonlinelobby_type["platform_icon"] = &ScreenOnlineLobby::platform_icon;
    screenonlinelobby_type["player_count"] = &ScreenOnlineLobby::player_count;
    screenonlinelobby_type["searching_for_players"] = &ScreenOnlineLobby::searching_for_players;
    screenonlinelobby_type["show_code_panel"] = &ScreenOnlineLobby::show_code_panel;
    screenonlinelobby_type["screen_code_input"] = sol::property([](ScreenOnlineLobby* screen) -> ScreenEnterOnlineCode*
                                                                { return reinterpret_cast<ScreenEnterOnlineCode*>(&screen->screen_code_input); });

    /// Available in ScreenOnlineLobby
    lua.new_usertype<ScreenEnterOnlineCode>(
        "ScreenEnterOnlineCode",
        "enter_code_your_code_scroll",
        &ScreenEnterOnlineCode::enter_code_your_code_scroll,
        "enter_code_your_code_scroll_left_handle",
        &ScreenEnterOnlineCode::enter_code_your_code_scroll_left_handle,
        "enter_code_your_code_scroll_right_handle",
        &ScreenEnterOnlineCode::enter_code_your_code_scroll_right_handle,
        sol::base_classes,
        sol::bases<Screen, ScreenCodeInput>());

    lua.new_usertype<PauseUI>(
        "PauseUI",
        "menu_slidein_progress",
        &PauseUI::menu_slidein_progress,
        "blurred_background",
        &PauseUI::blackout_background,
        "woodpanel_left",
        &PauseUI::woodpanel_left,
        "woodpanel_middle",
        &PauseUI::woodpanel_middle,
        "woodpanel_right",
        &PauseUI::woodpanel_right,
        "woodpanel_top",
        &PauseUI::woodpanel_top,
        "scroll",
        &PauseUI::scroll,
        "confirmation_panel",
        &PauseUI::confirmation_panel,
        "selected_option",
        &PauseUI::selected_option,
        "prompt_visible",
        &PauseUI::prompt_visible,
        "buttons_actions",
        &PauseUI::buttons_actions,
        "buttons_movement",
        &PauseUI::buttons_movement,
        "visibility",
        &PauseUI::visibility);

    lua.new_usertype<JournalUI>(
        "JournalUI",
        "state",
        &JournalUI::state,
        "chapter_shown",
        &JournalUI::chapter_shown,
        "current_page",
        &JournalUI::current_page,
        "flipping_to_page",
        &JournalUI::flipping_to_page,
        "max_page_count",
        &JournalUI::max_page_count,
        "book_background",
        &JournalUI::book_background,
        "arrow_left",
        &JournalUI::arrow_left,
        "arrow_right",
        &JournalUI::arrow_right,
        "entire_book",
        &JournalUI::entire_book,
        "fade_timer",
        &JournalUI::fade_timer,
        "page_timer",
        &JournalUI::page_timer,
        "opacity",
        sol::property([](JournalUI& ui) -> float
                      {
            switch (ui.state){
                case 0:
                    return 0.0f;
                case 1:
                    return 1.0f - (float)ui.fade_timer / 15.0f;
                case 5:
                    return (float)ui.fade_timer / 15.0f;
                default:
                    return 1.0f;
                } }),
        "pages",
        sol::readonly(&JournalUI::pages));

    /// Used in [set_callback](#set_callback) with ON.RENDER_POST_JOURNAL_PAGE
    lua.new_usertype<JournalPage>(
        "JournalPage",
        "background",
        &JournalPage::background,
        "page_number",
        &JournalPage::page_number,
        "is_right_side_page",
        &JournalPage::is_right_side_page,
        "get_type",
        &JournalPage::get_type);

    lua["JournalPage"]["as_journal_page_progress"] = &JournalPage::as<JournalPageProgress>;
    lua["JournalPage"]["as_journal_page_journalmenu"] = &JournalPage::as<JournalPageJournalMenu>;
    lua["JournalPage"]["as_journal_page_places"] = &JournalPage::as<JournalPagePlaces>;
    lua["JournalPage"]["as_journal_page_people"] = &JournalPage::as<JournalPagePeople>;
    lua["JournalPage"]["as_journal_page_bestiary"] = &JournalPage::as<JournalPageBestiary>;
    lua["JournalPage"]["as_journal_page_items"] = &JournalPage::as<JournalPageItems>;
    lua["JournalPage"]["as_journal_page_traps"] = &JournalPage::as<JournalPageTraps>;
    lua["JournalPage"]["as_journal_page_feats"] = &JournalPage::as<JournalPageFeats>;
    lua["JournalPage"]["as_journal_page_deathcause"] = &JournalPage::as<JournalPageDeathCause>;
    lua["JournalPage"]["as_journal_page_deathmenu"] = &JournalPage::as<JournalPageDeathMenu>;
    lua["JournalPage"]["as_journal_page_playerprofile"] = &JournalPage::as<JournalPagePlayerProfile>;
    lua["JournalPage"]["as_journal_page_lastgameplayed"] = &JournalPage::as<JournalPageLastGamePlayed>;

    lua.new_usertype<JournalPageProgress>(
        "JournalPageProgress",
        "coffeestain_top",
        &JournalPageProgress::coffeestain_top,
        sol::base_classes,
        sol::bases<JournalPage>());

    lua.new_usertype<JournalPageJournalMenu>(
        "JournalPageJournalMenu",
        "journal_text_info",
        &JournalPageJournalMenu::journal_text_info,
        "completion_badge",
        &JournalPageJournalMenu::completion_badge,
        sol::base_classes,
        sol::bases<JournalPage>());

    lua.new_usertype<JournalPageDiscoverable>(
        "JournalPageDiscoverable",
        "show_main_image",
        &JournalPageDiscoverable::show_main_image,
        "title_text_info",
        &JournalPageDiscoverable::title_text_info,
        "entry_text_info",
        &JournalPageDiscoverable::entry_text_info,
        "chapter_title_text_info",
        &JournalPageDiscoverable::chapter_title_text_info,
        sol::base_classes,
        sol::bases<JournalPage>());

    lua.new_usertype<JournalPagePlaces>(
        "JournalPagePlaces",
        "main_image",
        &JournalPagePlaces::main_image,
        sol::base_classes,
        sol::bases<JournalPage, JournalPageDiscoverable>());

    lua.new_usertype<JournalPagePeople>(
        "JournalPagePeople",
        "character_background",
        &JournalPagePeople::character_background,
        "character_icon",
        &JournalPagePeople::character_icon,
        "character_drawing",
        &JournalPagePeople::character_drawing,
        sol::base_classes,
        sol::bases<JournalPage, JournalPageDiscoverable>());

    lua.new_usertype<JournalPageBestiary>(
        "JournalPageBestiary",
        "monster_background",
        &JournalPageBestiary::monster_background,
        "monster_icon",
        &JournalPageBestiary::monster_icon,
        "defeated_killedby_black_bars",
        &JournalPageBestiary::defeated_killedby_black_bars,
        "defeated_text_info",
        &JournalPageBestiary::defeated_text_info,
        "defeated_value_text_info",
        &JournalPageBestiary::defeated_value_text_info,
        "killedby_text_info",
        &JournalPageBestiary::killedby_text_info,
        "killedby_value_text_info",
        &JournalPageBestiary::killedby_value_text_info,
        sol::base_classes,
        sol::bases<JournalPage, JournalPageDiscoverable>());

    lua.new_usertype<JournalPageItems>(
        "JournalPageItems",
        "item_icon",
        &JournalPageItems::item_icon,
        "item_background",
        &JournalPageItems::item_background,
        sol::base_classes,
        sol::bases<JournalPage, JournalPageDiscoverable>());

    lua.new_usertype<JournalPageTraps>(
        "JournalPageTraps",
        "trap_icon",
        &JournalPageTraps::trap_icon,
        "trap_background",
        &JournalPageTraps::trap_background,
        sol::base_classes,
        sol::bases<JournalPage, JournalPageDiscoverable>());

    lua.new_usertype<JournalPageFeats>(
        "JournalPageFeats",
        "chapter_title_text_info",
        &JournalPageFeats::chapter_title_text_info,
        "feat_icons",
        &JournalPageFeats::feat_icons,
        sol::base_classes,
        sol::bases<JournalPage>());

    lua.new_usertype<JournalPageDeathCause>(
        "JournalPageDeathCause",
        "death_cause_text_info",
        &JournalPageDeathCause::death_cause_text_info,
        sol::base_classes,
        sol::bases<JournalPage>());

    lua.new_usertype<JournalPageDeathMenu>(
        "JournalPageDeathMenu",
        "game_over_text_info",
        &JournalPageDeathMenu::game_over_text_info,
        "level_text_info",
        &JournalPageDeathMenu::level_text_info,
        "level_value_text_info",
        &JournalPageDeathMenu::level_value_text_info,
        "money_text_info",
        &JournalPageDeathMenu::money_text_info,
        "money_value_text_info",
        &JournalPageDeathMenu::money_value_text_info,
        "time_text_info",
        &JournalPageDeathMenu::time_text_info,
        "time_value_text_info",
        &JournalPageDeathMenu::time_value_text_info,
        sol::base_classes,
        sol::bases<JournalPage>());

    auto journalpageplayerprofile_type = lua.new_usertype<JournalPagePlayerProfile>("JournalPagePlayerProfile", sol::base_classes, sol::bases<JournalPage>());
    journalpageplayerprofile_type["player_icon"] = &JournalPagePlayerProfile::player_icon;
    journalpageplayerprofile_type["player_icon_id"] = &JournalPagePlayerProfile::player_icon_id;
    journalpageplayerprofile_type["player_profile_text_info"] = &JournalPagePlayerProfile::player_profile_text_info;
    journalpageplayerprofile_type["plays_text_info"] = &JournalPagePlayerProfile::plays_text_info;
    journalpageplayerprofile_type["plays_value_text_info"] = &JournalPagePlayerProfile::plays_value_text_info;
    journalpageplayerprofile_type["wins_text_info"] = &JournalPagePlayerProfile::wins_text_info;
    journalpageplayerprofile_type["wins_value_text_info"] = &JournalPagePlayerProfile::wins_value_text_info;
    journalpageplayerprofile_type["deaths_text_info"] = &JournalPagePlayerProfile::deaths_text_info;
    journalpageplayerprofile_type["deaths_value_text_info"] = &JournalPagePlayerProfile::deaths_value_text_info;
    journalpageplayerprofile_type["win_pct_text_info"] = &JournalPagePlayerProfile::win_pct_text_info;
    journalpageplayerprofile_type["win_pct_value_text_info"] = &JournalPagePlayerProfile::win_pct_value_text_info;
    journalpageplayerprofile_type["average_score_text_info"] = &JournalPagePlayerProfile::average_score_text_info;
    journalpageplayerprofile_type["average_score_value_text_info"] = &JournalPagePlayerProfile::average_score_value_text_info;
    journalpageplayerprofile_type["top_score_text_info"] = &JournalPagePlayerProfile::top_score_text_info;
    journalpageplayerprofile_type["top_score_value_text_info"] = &JournalPagePlayerProfile::top_score_value_text_info;
    journalpageplayerprofile_type["deepest_level_text_info"] = &JournalPagePlayerProfile::deepest_level_text_info;
    journalpageplayerprofile_type["deepest_level_value_text_info"] = &JournalPagePlayerProfile::deepest_level_value_text_info;
    journalpageplayerprofile_type["deadliest_level_text_info"] = &JournalPagePlayerProfile::deadliest_level_text_info;
    journalpageplayerprofile_type["deadliest_level_value_text_info"] = &JournalPagePlayerProfile::deadliest_level_value_text_info;
    journalpageplayerprofile_type["average_time_text_info"] = &JournalPagePlayerProfile::average_time_text_info;
    journalpageplayerprofile_type["average_time_value_text_info"] = &JournalPagePlayerProfile::average_time_value_text_info;
    journalpageplayerprofile_type["best_time_text_info"] = &JournalPagePlayerProfile::best_time_text_info;
    journalpageplayerprofile_type["best_time_value_text_info"] = &JournalPagePlayerProfile::best_time_value_text_info;

    lua.new_usertype<JournalPageLastGamePlayed>(
        "JournalPageLastGamePlayed",
        "main_image",
        &JournalPageLastGamePlayed::main_image,
        "last_game_played_text_info",
        &JournalPageLastGamePlayed::last_game_played_text_info,
        "level_text_info",
        &JournalPageLastGamePlayed::level_text_info,
        "level_value_text_info",
        &JournalPageLastGamePlayed::level_value_text_info,
        "money_text_info",
        &JournalPageLastGamePlayed::money_text_info,
        "money_value_text_info",
        &JournalPageLastGamePlayed::money_value_text_info,
        "time_text_info",
        &JournalPageLastGamePlayed::time_text_info,
        "time_value_text_info",
        &JournalPageLastGamePlayed::time_value_text_info,
        "sticker_count",
        &JournalPageLastGamePlayed::sticker_count,
        "stickers",
        &JournalPageLastGamePlayed::stickers,
        sol::base_classes,
        sol::bases<JournalPage>());

    lua.create_named_table(
        "SCREEN",
        "LOGO",
        0,
        "INTRO",
        1,
        "PROLOGUE",
        2,
        "TITLE",
        3,
        "MENU",
        4,
        "OPTIONS",
        5,
        "PLAYER_PROFILE",
        6,
        "LEADERBOARD",
        7,
        "SEED_INPUT",
        8,
        "CHARACTER_SELECT",
        9,
        "TEAM_SELECT",
        10,
        "CAMP",
        11,
        "LEVEL",
        12,
        "TRANSITION",
        13,
        "DEATH",
        14,
        "SPACESHIP",
        15,
        "WIN",
        16,
        "CREDITS",
        17,
        "SCORES",
        18,
        "CONSTELLATION",
        19,
        "RECAP",
        20,
        "ARENA_MENU",
        21,
        "ARENA_STAGES",
        22,
        "ARENA_ITEMS",
        23,
        "ARENA_SELECT",
        24,
        "ARENA_INTRO",
        25,
        "ARENA_LEVEL",
        26,
        "ARENA_SCORE",
        27,
        "ONLINE_LOADING",
        28,
        "ONLINE_LOBBY",
        29);

    lua.create_named_table(
        "PAUSEUI_VISIBILITY",
        "INVISIBLE",
        0,
        "SLIDING_DOWN",
        1,
        "VISIBLE",
        2,
        "SLIDING_UP",
        3);

    lua.create_named_table(
        "JOURNALUI_STATE",
        "INVISIBLE",
        0,
        "FADING_IN",
        1,
        "STABLE",
        2,
        "FLIPPING_LEFT",
        3,
        "FLIPPING_RIGHT",
        4,
        "FADING_OUT",
        5);

    lua.create_named_table(
        "JOURNALUI_PAGE_SHOWN",
        "PLAYER_PROFILE",
        1,
        "JOURNAL",
        2,
        "PLACES",
        3,
        "PEOPLE",
        4,
        "BESTIARY",
        5,
        "ITEMS",
        6,
        "TRAPS",
        7,
        "STORY",
        8,
        "FEATS",
        9,
        "RECAP",
        10,
        "DEATH",
        11);

    lua.create_named_table(
        "JOURNAL_PAGE_TYPE",
        "PROGRESS",
        JournalPageType::Progress,
        "JOURNAL_MENU",
        JournalPageType::JournalMenu,
        "PLACES",
        JournalPageType::Places,
        "PEOPLE",
        JournalPageType::People,
        "BESTIARY",
        JournalPageType::Bestiary,
        "ITEMS",
        JournalPageType::Items,
        "TRAPS",
        JournalPageType::Traps,
        "STORY",
        JournalPageType::Story,
        "FEATS",
        JournalPageType::Feats,
        "DEATH_CAUSE",
        JournalPageType::DeathCause,
        "DEATH_MENU",
        JournalPageType::DeathMenu,
        "RECAP",
        JournalPageType::Recap,
        "PLAYER_PROFILE",
        JournalPageType::PlayerProfile,
        "LAST_GAME_PLAYED",
        JournalPageType::LastGamePlayed);
}
} // namespace NScreen
