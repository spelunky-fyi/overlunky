#include "screen_arena_lua.hpp"

#include <algorithm>   // for max
#include <new>         // for operator new
#include <sol/sol.hpp> // for data_t, basic_table_core::new_usertype
#include <string>      // for operator==, allocator
#include <tuple>       // for get
#include <type_traits> // for move, declval
#include <utility>     // for min, max

#include "screen.hpp"       // for ScreenZoomAnimation, Screen (ptr only)
#include "screen_arena.hpp" // for ScreenArenaMenu, ScreenArenaStagesSelect

namespace NScreenArena
{
void register_usertypes(sol::state& lua)
{
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

    auto screenarenamenu_type = lua.new_usertype<ScreenArenaMenu>("ScreenArenaMenu", sol::base_classes, sol::bases<Screen>());
    screenarenamenu_type["brick_background_animation"] = &ScreenArenaMenu::brick_background_animation;
    screenarenamenu_type["blurry_border_animation"] = &ScreenArenaMenu::blurry_border_animation;
    screenarenamenu_type["brick_background"] = &ScreenArenaMenu::brick_background;
    screenarenamenu_type["blurry_border"] = &ScreenArenaMenu::blurry_border;
    screenarenamenu_type["blurry_border2"] = &ScreenArenaMenu::blurry_border2;
    screenarenamenu_type["characters_drawing"] = &ScreenArenaMenu::characters_drawing;
    screenarenamenu_type["info_black_background"] = &ScreenArenaMenu::info_black_background;
    screenarenamenu_type["main_panel_top_left_corner"] = &ScreenArenaMenu::main_panel_top_left_corner;
    screenarenamenu_type["main_panel_top"] = &ScreenArenaMenu::main_panel_top;
    screenarenamenu_type["main_panel_top_right_corner"] = &ScreenArenaMenu::main_panel_top_right_corner;
    screenarenamenu_type["main_panel_left"] = &ScreenArenaMenu::main_panel_left;
    screenarenamenu_type["main_panel_center"] = &ScreenArenaMenu::main_panel_center;
    screenarenamenu_type["main_panel_right"] = &ScreenArenaMenu::main_panel_right;
    screenarenamenu_type["main_panel_bottom_left_corner"] = &ScreenArenaMenu::main_panel_bottom_left_corner;
    screenarenamenu_type["main_panel_bottom"] = &ScreenArenaMenu::main_panel_bottom;
    screenarenamenu_type["main_panel_bottom_right_corner"] = &ScreenArenaMenu::main_panel_bottom_right_corner;
    screenarenamenu_type["rules_scroll"] = &ScreenArenaMenu::rules_scroll;
    screenarenamenu_type["black_option_boxes_left"] = &ScreenArenaMenu::black_option_boxes_left;
    screenarenamenu_type["black_option_boxes_center"] = &ScreenArenaMenu::black_option_boxes_center;
    screenarenamenu_type["black_option_boxes_right"] = &ScreenArenaMenu::black_option_boxes_right;
    screenarenamenu_type["gold_option_outline"] = &ScreenArenaMenu::gold_option_outline;
    screenarenamenu_type["option_icons"] = &ScreenArenaMenu::option_icons;
    screenarenamenu_type["option_left_arrow"] = &ScreenArenaMenu::option_left_arrow;
    screenarenamenu_type["option_right_arrow"] = &ScreenArenaMenu::option_right_arrow;
    screenarenamenu_type["bottom_left_bricks"] = &ScreenArenaMenu::bottom_left_bricks;
    screenarenamenu_type["top_left_esc_panel"] = &ScreenArenaMenu::top_left_esc_panel;
    screenarenamenu_type["next_panel"] = &ScreenArenaMenu::next_panel;
    screenarenamenu_type["center_panels_hor_slide_position"] = &ScreenArenaMenu::center_panels_hor_slide_position;
    screenarenamenu_type["esc_next_panels_slide_timer"] = &ScreenArenaMenu::esc_next_panels_slide_timer;
    screenarenamenu_type["main_panel_vertical_scroll_position"] = &ScreenArenaMenu::main_panel_vertical_scroll_position;
    screenarenamenu_type["selected_option_index"] = &ScreenArenaMenu::selected_option_index;
    screenarenamenu_type["contols"] = &ScreenArenaMenu::contols;

    lua.new_usertype<ScreenZoomAnimation>(
        "ScreenZoomAnimation",
        "zoom_target",
        &ScreenZoomAnimation::zoom_target);

    auto screenarenastagesselect_type = lua.new_usertype<ScreenArenaStagesSelect>("ScreenArenaStagesSelect", sol::base_classes, sol::bases<Screen>());
    screenarenastagesselect_type["buttons"] = &ScreenArenaStagesSelect::buttons;
    screenarenastagesselect_type["brick_background"] = &ScreenArenaStagesSelect::brick_background;
    screenarenastagesselect_type["info_black_background"] = &ScreenArenaStagesSelect::info_black_background;
    screenarenastagesselect_type["woodenpanel_center"] = &ScreenArenaStagesSelect::woodenpanel_center;
    screenarenastagesselect_type["blocky_level_representation"] = &ScreenArenaStagesSelect::blocky_level_representation;
    screenarenastagesselect_type["theme_indicator"] = &ScreenArenaStagesSelect::theme_indicator;
    screenarenastagesselect_type["bricks_bottom_left"] = &ScreenArenaStagesSelect::bricks_bottom_left;
    screenarenastagesselect_type["grid_background_row_0"] = &ScreenArenaStagesSelect::grid_background_row_0;
    screenarenastagesselect_type["grid_background_row_1"] = &ScreenArenaStagesSelect::grid_background_row_1;
    screenarenastagesselect_type["grid_background_row_2"] = &ScreenArenaStagesSelect::grid_background_row_2;
    screenarenastagesselect_type["grid_background_row_3"] = &ScreenArenaStagesSelect::grid_background_row_3;
    screenarenastagesselect_type["grid_background_row_4"] = &ScreenArenaStagesSelect::grid_background_row_4;
    screenarenastagesselect_type["grid_background_row_5"] = &ScreenArenaStagesSelect::grid_background_row_5;
    screenarenastagesselect_type["grid_background_row_6"] = &ScreenArenaStagesSelect::grid_background_row_6;
    screenarenastagesselect_type["grid_background_row_7"] = &ScreenArenaStagesSelect::grid_background_row_7;
    screenarenastagesselect_type["grid_background_disabled_cross"] = &ScreenArenaStagesSelect::grid_background_disabled_cross;
    screenarenastagesselect_type["grid_background_manipulators"] = &ScreenArenaStagesSelect::grid_background_manipulators;
    screenarenastagesselect_type["unknown21"] = &ScreenArenaStagesSelect::unknown21;
    screenarenastagesselect_type["grid_disabled_cross"] = &ScreenArenaStagesSelect::grid_disabled_cross;
    screenarenastagesselect_type["grid_yellow_highlighter"] = &ScreenArenaStagesSelect::grid_yellow_highlighter;
    screenarenastagesselect_type["woodpanel_esc"] = &ScreenArenaStagesSelect::woodpanel_esc;
    screenarenastagesselect_type["woodpanel_fight"] = &ScreenArenaStagesSelect::woodpanel_fight;
    screenarenastagesselect_type["big_player_drawing"] = &ScreenArenaStagesSelect::big_player_drawing;
    screenarenastagesselect_type["players_turn_scroll"] = &ScreenArenaStagesSelect::players_turn_scroll;
    screenarenastagesselect_type["players_turn_scroll_handle"] = &ScreenArenaStagesSelect::players_turn_scroll_handle;
    screenarenastagesselect_type["grid_player_icon"] = &ScreenArenaStagesSelect::grid_player_icon;
    screenarenastagesselect_type["selected_stage_index"] = &ScreenArenaStagesSelect::selected_stage_index;
    screenarenastagesselect_type["contols"] = &ScreenArenaStagesSelect::contols;

    auto screenarenaitems_type = lua.new_usertype<ScreenArenaItems>("ScreenArenaItems", sol::base_classes, sol::bases<Screen>());
    screenarenaitems_type["brick_background"] = &ScreenArenaItems::brick_background;
    screenarenaitems_type["black_background_bottom_right"] = &ScreenArenaItems::black_background_bottom_right;
    screenarenaitems_type["woodpanel_bottom"] = &ScreenArenaItems::woodpanel_bottom;
    screenarenaitems_type["scroll_bottom"] = &ScreenArenaItems::scroll_bottom;
    screenarenaitems_type["scroll_right_handle_bottom"] = &ScreenArenaItems::scroll_right_handle_bottom;
    screenarenaitems_type["held_item_crate_on_scroll"] = &ScreenArenaItems::held_item_crate_on_scroll;
    screenarenaitems_type["held_item_on_scroll"] = &ScreenArenaItems::held_item_on_scroll;
    screenarenaitems_type["item_background"] = &ScreenArenaItems::item_background;
    screenarenaitems_type["toggles_background"] = &ScreenArenaItems::toggles_background;
    screenarenaitems_type["item_selection_gold_outline"] = &ScreenArenaItems::item_selection_gold_outline;
    screenarenaitems_type["item_icons"] = &ScreenArenaItems::item_icons;
    screenarenaitems_type["item_held_badge"] = &ScreenArenaItems::item_held_badge;
    screenarenaitems_type["item_equipped_badge"] = &ScreenArenaItems::item_equipped_badge;
    screenarenaitems_type["item_off_gray_overlay"] = &ScreenArenaItems::item_off_gray_overlay;
    screenarenaitems_type["esc_woodpanel"] = &ScreenArenaItems::esc_woodpanel;
    screenarenaitems_type["center_panels_horizontal_slide_position"] = &ScreenArenaItems::center_panels_horizontal_slide_position;
    screenarenaitems_type["esc_panel_slide_timer"] = &ScreenArenaItems::esc_panel_slide_timer;
    screenarenaitems_type["selected_item_index"] = &ScreenArenaItems::selected_item_index;
    screenarenaitems_type["controls"] = &ScreenArenaItems::controls;

    auto screenarenaintro_type = lua.new_usertype<ScreenArenaIntro>("ScreenArenaIntro", sol::base_classes, sol::bases<Screen>());
    screenarenaintro_type["players"] = &ScreenArenaIntro::players;
    screenarenaintro_type["background_colors"] = &ScreenArenaIntro::background_colors;
    screenarenaintro_type["vertical_lines"] = &ScreenArenaIntro::vertical_lines;
    screenarenaintro_type["vertical_line_electricity_effect"] = &ScreenArenaIntro::vertical_line_electricity_effect;
    screenarenaintro_type["unknown_all_forced"] = &ScreenArenaIntro::unknown_all_forced;
    screenarenaintro_type["left_scroll"] = &ScreenArenaIntro::left_scroll;
    screenarenaintro_type["right_scroll"] = &ScreenArenaIntro::right_scroll;
    screenarenaintro_type["scroll_unfurl_timer"] = &ScreenArenaIntro::scroll_unfurl_timer;
    screenarenaintro_type["waiting"] = &ScreenArenaIntro::waiting;
    screenarenaintro_type["names_opacity"] = &ScreenArenaIntro::names_opacity;
    screenarenaintro_type["line_electricity_effect_timer"] = &ScreenArenaIntro::line_electricity_effect_timer;
    screenarenaintro_type["state"] = &ScreenArenaIntro::state;
    screenarenaintro_type["countdown"] = &ScreenArenaIntro::countdown;

    lua.new_usertype<ScreenArenaLevel>(
        "ScreenArenaLevel",
        "get_ready",
        &ScreenArenaLevel::get_ready,
        "get_ready_gray_background",
        &ScreenArenaLevel::get_ready_gray_background,
        "get_ready_outline",
        &ScreenArenaLevel::get_ready_outline,
        sol::base_classes,
        sol::bases<Screen>());

    auto screenarenascore_type = lua.new_usertype<ScreenArenaScore>("ScreenArenaScore", sol::base_classes, sol::bases<Screen>());
    screenarenascore_type["woodpanel_slide_timer"] = &ScreenArenaScore::woodpanel_slide_timer;
    screenarenascore_type["scroll_unfurl_timer"] = &ScreenArenaScore::scroll_unfurl_timer;
    screenarenascore_type["unknown10"] = &ScreenArenaScore::unknown10;
    screenarenascore_type["woodpanel"] = &ScreenArenaScore::woodpanel;
    screenarenascore_type["unknown_all_forced"] = &ScreenArenaScore::unknown_all_forced;
    screenarenascore_type["woodpanel_left_scroll"] = &ScreenArenaScore::woodpanel_left_scroll;
    screenarenascore_type["woodpanel_right_scroll"] = &ScreenArenaScore::woodpanel_right_scroll;
    screenarenascore_type["text_id_1"] = &ScreenArenaScore::text_id_1;
    screenarenascore_type["text_id_2"] = &ScreenArenaScore::text_id_2;
    screenarenascore_type["woodpanel_visible"] = &ScreenArenaScore::woodpanel_visible;
    screenarenascore_type["woodpanel_slide_toggle"] = &ScreenArenaScore::woodpanel_slide_toggle;
    screenarenascore_type["animation_sequence"] = &ScreenArenaScore::animation_sequence;
    screenarenascore_type["background"] = &ScreenArenaScore::background;
    screenarenascore_type["ok_panel"] = &ScreenArenaScore::ok_panel;
    screenarenascore_type["ready_panel"] = &ScreenArenaScore::ready_panel;
    screenarenascore_type["ready_speechbubble_indicator"] = &ScreenArenaScore::ready_speechbubble_indicator;
    screenarenascore_type["pillars"] = &ScreenArenaScore::pillars;
    screenarenascore_type["bottom_lava"] = &ScreenArenaScore::bottom_lava;
    screenarenascore_type["players"] = &ScreenArenaScore::players;
    screenarenascore_type["player_shadows"] = &ScreenArenaScore::player_shadows;
    screenarenascore_type["unknown24"] = &ScreenArenaScore::unknown24;
    screenarenascore_type["unknown25"] = &ScreenArenaScore::unknown25;
    screenarenascore_type["score_counter"] = &ScreenArenaScore::score_counter;
    screenarenascore_type["unknown27"] = &ScreenArenaScore::unknown27;
    screenarenascore_type["lava_bubbles"] = &ScreenArenaScore::lava_bubbles;
    screenarenascore_type["player_won"] = &ScreenArenaScore::player_won;
    screenarenascore_type["victory_jump_y_pos"] = &ScreenArenaScore::victory_jump_y_pos;
    screenarenascore_type["victory_jump_velocity"] = &ScreenArenaScore::victory_jump_velocity;
    screenarenascore_type["animation_frame"] = &ScreenArenaScore::animation_frame;
    screenarenascore_type["squash_and_celebrate"] = &ScreenArenaScore::squash_and_celebrate;
    screenarenascore_type["player_ready"] = &ScreenArenaScore::player_ready;
    screenarenascore_type["next_transition_timer"] = &ScreenArenaScore::next_transition_timer;
    screenarenascore_type["player_bottom_pillar_offset"] = &ScreenArenaScore::player_bottom_pillar_offset;
    screenarenascore_type["player_crushing_pillar_height"] = &ScreenArenaScore::player_crushing_pillar_height;
    screenarenascore_type["player_create_giblets"] = &ScreenArenaScore::player_create_giblets;
    screenarenascore_type["next_sidepanel_slidein_timer"] = &ScreenArenaScore::next_sidepanel_slidein_timer;
}
} // namespace NScreenArena
