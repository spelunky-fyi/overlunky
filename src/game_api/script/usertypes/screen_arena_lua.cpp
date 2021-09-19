#include "screen_arena_lua.hpp"

#include "screen.hpp"
#include "screen_arena.hpp"

#include <sol/sol.hpp>

namespace NScreenArena
{
void register_usertypes(sol::state& lua)
{
    lua.new_usertype<ScreenArenaMenu>(
        "ScreenArenaMenu",
        "brick_background_animation",
        &ScreenArenaMenu::brick_background_animation,
        "blurry_border_animation",
        &ScreenArenaMenu::blurry_border_animation,
        "top_woodpanel_slidein_timer",
        &ScreenArenaMenu::top_woodpanel_slidein_timer,
        "top_scroll_unfurl_timer",
        &ScreenArenaMenu::top_scroll_unfurl_timer,
        "unknown13",
        &ScreenArenaMenu::unknown13,
        "woodpanel_top",
        &ScreenArenaMenu::woodpanel_top,
        "unknown15",
        &ScreenArenaMenu::unknown15,
        "left_scroll_handle",
        &ScreenArenaMenu::left_scroll_handle,
        "right_scroll_handle",
        &ScreenArenaMenu::right_scroll_handle,
        "scroll_text_id",
        &ScreenArenaMenu::scroll_text_id,
        "unknown17_text_id",
        &ScreenArenaMenu::unknown17_text_id,
        "unknown18_text_id",
        &ScreenArenaMenu::unknown18_text_id,
        "unknown19_text_id",
        &ScreenArenaMenu::unknown19_text_id,
        "top_woodpanel_visible",
        &ScreenArenaMenu::top_woodpanel_visible,
        "bottom_woodpanel_visible",
        &ScreenArenaMenu::bottom_woodpanel_visible,
        "woodpanels_toggle",
        &ScreenArenaMenu::woodpanels_toggle,
        "brick_background",
        &ScreenArenaMenu::brick_background,
        "blurry_border",
        &ScreenArenaMenu::blurry_border,
        "blurry_border2",
        &ScreenArenaMenu::blurry_border2,
        "characters_drawing",
        &ScreenArenaMenu::characters_drawing,
        "info_black_background",
        &ScreenArenaMenu::info_black_background,
        "main_panel_top_left_corner",
        &ScreenArenaMenu::main_panel_top_left_corner,
        "main_panel_top",
        &ScreenArenaMenu::main_panel_top,
        "main_panel_top_right_corner",
        &ScreenArenaMenu::main_panel_top_right_corner,
        "main_panel_left",
        &ScreenArenaMenu::main_panel_left,
        "main_panel_center",
        &ScreenArenaMenu::main_panel_center,
        "main_panel_right",
        &ScreenArenaMenu::main_panel_right,
        "main_panel_bottom_left_corner",
        &ScreenArenaMenu::main_panel_bottom_left_corner,
        "main_panel_bottom",
        &ScreenArenaMenu::main_panel_bottom,
        "main_panel_bottom_right_corner",
        &ScreenArenaMenu::main_panel_bottom_right_corner,
        "rules_scroll",
        &ScreenArenaMenu::rules_scroll,
        "black_option_boxes_left",
        &ScreenArenaMenu::black_option_boxes_left,
        "black_option_boxes_center",
        &ScreenArenaMenu::black_option_boxes_center,
        "black_option_boxes_right",
        &ScreenArenaMenu::black_option_boxes_right,
        "gold_option_outline",
        &ScreenArenaMenu::gold_option_outline,
        "option_icons",
        &ScreenArenaMenu::option_icons,
        "option_left_arrow",
        &ScreenArenaMenu::option_left_arrow,
        "option_right_arrow",
        &ScreenArenaMenu::option_right_arrow,
        "bottom_left_bricks",
        &ScreenArenaMenu::bottom_left_bricks,
        "top_left_esc_panel",
        &ScreenArenaMenu::top_left_esc_panel,
        "next_panel",
        &ScreenArenaMenu::next_panel,
        "center_panels_hor_slide_position",
        &ScreenArenaMenu::center_panels_hor_slide_position,
        "esc_next_panels_slide_timer",
        &ScreenArenaMenu::esc_next_panels_slide_timer,
        "main_panel_vertical_scroll_position",
        &ScreenArenaMenu::main_panel_vertical_scroll_position,
        "selected_option_index",
        &ScreenArenaMenu::selected_option_index,
        sol::base_classes,
        sol::bases<Screen>());

    lua.new_usertype<ScreenZoomAnimation>(
        "ScreenZoomAnimation",
        "zoom_target",
        &ScreenZoomAnimation::zoom_target);

    lua.new_usertype<ScreenArenaStagesSelect>(
        "ScreenArenaStagesSelect",
        "woodenpanel_top_slidein_timer",
        &ScreenArenaStagesSelect::woodenpanel_top_slidein_timer,
        "woodenpanel_top_scroll_unfurl_timer",
        &ScreenArenaStagesSelect::woodenpanel_top_scroll_unfurl_timer,
        "woodenpanel_top",
        &ScreenArenaStagesSelect::woodenpanel_top,
        "woodenpanel_top_left_scroll",
        &ScreenArenaStagesSelect::woodenpanel_top_left_scroll,
        "woodenpanel_top_right_scroll",
        &ScreenArenaStagesSelect::woodenpanel_top_right_scroll,
        "text_id_1",
        &ScreenArenaStagesSelect::text_id_1,
        "text_id_2",
        &ScreenArenaStagesSelect::text_id_2,
        "text_id_3",
        &ScreenArenaStagesSelect::text_id_3,
        "text_id_4",
        &ScreenArenaStagesSelect::text_id_4,
        "woodenpanel_top_visible",
        &ScreenArenaStagesSelect::woodenpanel_top_visible,
        "woodenpanel_bottom_visible",
        &ScreenArenaStagesSelect::woodenpanel_bottom_visible,
        "woodenpanels_toggle",
        &ScreenArenaStagesSelect::woodenpanels_toggle,
        "buttons",
        &ScreenArenaStagesSelect::buttons,
        "brick_background",
        &ScreenArenaStagesSelect::brick_background,
        "info_black_background",
        &ScreenArenaStagesSelect::info_black_background,
        "woodenpanel_center",
        &ScreenArenaStagesSelect::woodenpanel_center,
        "blocky_level_representation",
        &ScreenArenaStagesSelect::blocky_level_representation,
        "theme_indicator",
        &ScreenArenaStagesSelect::theme_indicator,
        "bricks_bottom_left",
        &ScreenArenaStagesSelect::bricks_bottom_left,
        "grid_background_row_0",
        &ScreenArenaStagesSelect::grid_background_row_0,
        "grid_background_row_1",
        &ScreenArenaStagesSelect::grid_background_row_1,
        "grid_background_row_2",
        &ScreenArenaStagesSelect::grid_background_row_2,
        "grid_background_row_3",
        &ScreenArenaStagesSelect::grid_background_row_3,
        "grid_background_row_4",
        &ScreenArenaStagesSelect::grid_background_row_4,
        "grid_background_row_5",
        &ScreenArenaStagesSelect::grid_background_row_5,
        "grid_background_row_6",
        &ScreenArenaStagesSelect::grid_background_row_6,
        "grid_background_row_7",
        &ScreenArenaStagesSelect::grid_background_row_7,
        "grid_background_disabled_cross",
        &ScreenArenaStagesSelect::grid_background_disabled_cross,
        "grid_background_manipulators",
        &ScreenArenaStagesSelect::grid_background_manipulators,
        "unknown21",
        &ScreenArenaStagesSelect::unknown21,
        "grid_disabled_cross",
        &ScreenArenaStagesSelect::grid_disabled_cross,
        "grid_yellow_highlighter",
        &ScreenArenaStagesSelect::grid_yellow_highlighter,
        "woodpanel_esc",
        &ScreenArenaStagesSelect::woodpanel_esc,
        "woodpanel_fight",
        &ScreenArenaStagesSelect::woodpanel_fight,
        "big_player_drawing",
        &ScreenArenaStagesSelect::big_player_drawing,
        "players_turn_scroll",
        &ScreenArenaStagesSelect::players_turn_scroll,
        "players_turn_scroll_handle",
        &ScreenArenaStagesSelect::players_turn_scroll_handle,
        "grid_player_icon",
        &ScreenArenaStagesSelect::grid_player_icon,
        sol::base_classes,
        sol::bases<Screen>());

    lua.new_usertype<ScreenArenaItems>(
        "ScreenArenaItems",
        "woodpanel_top_slidein_timer",
        &ScreenArenaItems::woodpanel_top_slidein_timer,
        "woodpanel_top_scroll_unfurl_timer",
        &ScreenArenaItems::woodpanel_top_scroll_unfurl_timer,
        "unknown9",
        &ScreenArenaItems::unknown9,
        "woodpanel_top",
        &ScreenArenaItems::woodpanel_top,
        "unknown11",
        &ScreenArenaItems::unknown11,
        "top_scroll_left_handle",
        &ScreenArenaItems::top_scroll_left_handle,
        "top_scroll_right_handle",
        &ScreenArenaItems::top_scroll_right_handle,
        "scroll_text_id",
        &ScreenArenaItems::scroll_text_id,
        "text_id_2",
        &ScreenArenaItems::text_id_2,
        "text_id_3",
        &ScreenArenaItems::text_id_3,
        "text_id_4",
        &ScreenArenaItems::text_id_4,
        "woodpanel_top_visible",
        &ScreenArenaItems::woodpanel_top_visible,
        "woodpanel_bottom_visible",
        &ScreenArenaItems::woodpanel_bottom_visible,
        "woodpanels_toggle",
        &ScreenArenaItems::woodpanels_toggle,
        "brick_background",
        &ScreenArenaItems::brick_background,
        "black_background_bottom_right",
        &ScreenArenaItems::black_background_bottom_right,
        "woodpanel_bottom",
        &ScreenArenaItems::woodpanel_bottom,
        "scroll_bottom",
        &ScreenArenaItems::scroll_bottom,
        "scroll_right_handle_bottom",
        &ScreenArenaItems::scroll_right_handle_bottom,
        "held_item_crate_on_scroll",
        &ScreenArenaItems::held_item_crate_on_scroll,
        "held_item_on_scroll",
        &ScreenArenaItems::held_item_on_scroll,
        "item_background",
        &ScreenArenaItems::item_background,
        "toggles_background",
        &ScreenArenaItems::toggles_background,
        "item_selection_gold_outline",
        &ScreenArenaItems::item_selection_gold_outline,
        "item_icons",
        &ScreenArenaItems::item_icons,
        "item_held_badge",
        &ScreenArenaItems::item_held_badge,
        "item_equipped_badge",
        &ScreenArenaItems::item_equipped_badge,
        "item_off_gray_overlay",
        &ScreenArenaItems::item_off_gray_overlay,
        "esc_woodpanel",
        &ScreenArenaItems::esc_woodpanel,
        "center_panels_horizontal_slide_position",
        &ScreenArenaItems::center_panels_horizontal_slide_position,
        "esc_panel_slide_timer",
        &ScreenArenaItems::esc_panel_slide_timer,
        "selected_item_index",
        &ScreenArenaItems::selected_item_index,
        sol::base_classes,
        sol::bases<Screen>());

    lua.new_usertype<ScreenArenaIntro>(
        "ScreenArenaIntro",
        "players",
        &ScreenArenaIntro::players,
        "background_colors",
        &ScreenArenaIntro::background_colors,
        "vertical_lines",
        &ScreenArenaIntro::vertical_lines,
        "vertical_line_electricity_effect",
        &ScreenArenaIntro::vertical_line_electricity_effect,
        "unknown_all_forced",
        &ScreenArenaIntro::unknown_all_forced,
        "left_scroll",
        &ScreenArenaIntro::left_scroll,
        "right_scroll",
        &ScreenArenaIntro::right_scroll,
        "scroll_unfurl_timer",
        &ScreenArenaIntro::scroll_unfurl_timer,
        "waiting",
        &ScreenArenaIntro::waiting,
        "names_opacity",
        &ScreenArenaIntro::names_opacity,
        "line_electricity_effect_timer",
        &ScreenArenaIntro::line_electricity_effect_timer,
        "state",
        &ScreenArenaIntro::state,
        "countdown",
        &ScreenArenaIntro::countdown,
        sol::base_classes,
        sol::bases<Screen>());

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

    lua.new_usertype<ScreenArenaScore>(
        "ScreenArenaScore",
        "woodpanel_slide_timer",
        &ScreenArenaScore::woodpanel_slide_timer,
        "scroll_unfurl_timer",
        &ScreenArenaScore::scroll_unfurl_timer,
        "unknown10",
        &ScreenArenaScore::unknown10,
        "woodpanel",
        &ScreenArenaScore::woodpanel,
        "unknown_all_forced",
        &ScreenArenaScore::unknown_all_forced,
        "woodpanel_left_scroll",
        &ScreenArenaScore::woodpanel_left_scroll,
        "woodpanel_right_scroll",
        &ScreenArenaScore::woodpanel_right_scroll,
        "text_id_1",
        &ScreenArenaScore::text_id_1,
        "text_id_2",
        &ScreenArenaScore::text_id_2,
        "woodpanel_visible",
        &ScreenArenaScore::woodpanel_visible,
        "woodpanel_slide_toggle",
        &ScreenArenaScore::woodpanel_slide_toggle,
        "animation_sequence",
        &ScreenArenaScore::animation_sequence,
        "background",
        &ScreenArenaScore::background,
        "ok_panel",
        &ScreenArenaScore::ok_panel,
        "ready_panel",
        &ScreenArenaScore::ready_panel,
        "ready_speechbubble_indicator",
        &ScreenArenaScore::ready_speechbubble_indicator,
        "pillars",
        &ScreenArenaScore::pillars,
        "bottom_lava",
        &ScreenArenaScore::bottom_lava,
        "players",
        &ScreenArenaScore::players,
        "player_shadows",
        &ScreenArenaScore::player_shadows,
        "unknown24",
        &ScreenArenaScore::unknown24,
        "unknown25",
        &ScreenArenaScore::unknown25,
        "score_counter",
        &ScreenArenaScore::score_counter,
        "unknown27",
        &ScreenArenaScore::unknown27,
        "lava_bubbles",
        &ScreenArenaScore::lava_bubbles,
        "player_won",
        &ScreenArenaScore::player_won,
        "victory_jump_y_pos",
        &ScreenArenaScore::victory_jump_y_pos,
        "victory_jump_velocity",
        &ScreenArenaScore::victory_jump_velocity,
        "animation_frame",
        &ScreenArenaScore::animation_frame,
        "squash_and_celebrate",
        &ScreenArenaScore::squash_and_celebrate,
        "player_ready",
        &ScreenArenaScore::player_ready,
        "next_transition_timer",
        &ScreenArenaScore::next_transition_timer,
        "player_bottom_pillar_offset",
        &ScreenArenaScore::player_bottom_pillar_offset,
        "player_crushing_pillar_height",
        &ScreenArenaScore::player_crushing_pillar_height,
        "player_create_giblets",
        &ScreenArenaScore::player_create_giblets,
        "next_sidepanel_slidein_timer",
        &ScreenArenaScore::next_sidepanel_slidein_timer,
        sol::base_classes,
        sol::bases<Screen>());
}
} // namespace NScreenArena
