#pragma once

#include "containers/custom_unordered_map.hpp"
#include "screen.hpp"

struct ArenaRulesString
{
    STRINGID text_1; // StringsTableID
    STRINGID text_2; // StringsTableID
    STRINGID text_3; // StringsTableID
    STRINGID text_4; // StringsTableID
    uint8_t unknown1;
    uint8_t unknown2;
    uint8_t unknown3;
    uint8_t unknown4;
    uint32_t unknown5;
    size_t some_offset;
};

class ScreenArenaMenu : public Screen // ID: 21
{
  public:
    ScreenZoomAnimation brick_background_animation;
    ScreenZoomAnimation blurry_border_animation;
    MenuScreenPanels screen_panels;
    TextureRenderingInfo brick_background;
    TextureRenderingInfo blurry_border;
    TextureRenderingInfo blurry_border2;
    TextureRenderingInfo characters_drawing;
    TextureRenderingInfo info_black_background;
    TextureRenderingInfo main_panel_top_left_corner;
    TextureRenderingInfo main_panel_top;
    TextureRenderingInfo main_panel_top_right_corner;
    TextureRenderingInfo main_panel_left;
    TextureRenderingInfo main_panel_center;
    TextureRenderingInfo main_panel_right;
    TextureRenderingInfo main_panel_bottom_left_corner;
    TextureRenderingInfo main_panel_bottom;
    TextureRenderingInfo main_panel_bottom_right_corner;
    TextureRenderingInfo rules_scroll;
    TextureRenderingInfo black_option_boxes_left;
    TextureRenderingInfo black_option_boxes_center;
    TextureRenderingInfo black_option_boxes_right;
    TextureRenderingInfo gold_option_outline;
    TextureRenderingInfo option_icons;
    TextureRenderingInfo option_left_arrow;
    TextureRenderingInfo option_right_arrow;
    TextureRenderingInfo bottom_left_bricks;
    TextureRenderingInfo top_left_esc_panel;
    TextureRenderingInfo next_panel;
    custom_vector<ArenaRulesString> option_captions; // I assume it's custom, it's filled with the screen construction

    uint32_t unknown48; // load state?
    uint32_t unknown52;
    float center_panels_hor_slide_position;
    float esc_next_panels_slide_timer;
    float main_panel_vertical_scroll_position;
    uint32_t selected_option_index; // more like highlighted
    ScreenControls controls;
    uint8_t unknown57[3]; // some states, like screen depth
};

class ScreenArenaStagesSelect : public Screen // ID: 22 and 24
{
  public:
    MenuScreenPanels screen_panels;
    uint16_t buttons;
    uint16_t padding1;
    TextureRenderingInfo brick_background;
    TextureRenderingInfo info_black_background;
    TextureRenderingInfo woodenpanel_center;
    TextureRenderingInfo blocky_level_representation;
    TextureRenderingInfo theme_indicator;
    TextureRenderingInfo bricks_bottom_left;
    TextureRenderingInfo grid_background_row_0;
    TextureRenderingInfo grid_background_row_1;
    TextureRenderingInfo grid_background_row_2;
    TextureRenderingInfo grid_background_row_3;
    TextureRenderingInfo grid_background_row_4;
    TextureRenderingInfo grid_background_row_5;
    TextureRenderingInfo grid_background_row_6;
    TextureRenderingInfo grid_background_row_7;
    TextureRenderingInfo grid_background_disabled_cross;
    TextureRenderingInfo grid_background_manipulators;
    TextureRenderingInfo unknown21;
    TextureRenderingInfo grid_disabled_cross;
    TextureRenderingInfo grid_yellow_highlighter;
    TextureRenderingInfo woodpanel_esc;
    TextureRenderingInfo woodpanel_fight;
    TextureRenderingInfo big_player_drawing;
    TextureRenderingInfo players_turn_scroll;
    TextureRenderingInfo players_turn_scroll_handle;
    TextureRenderingInfo grid_player_icon;
    float unknown30;                                          // padding probably
    custom_unordered_map<uint32_t, float> stages_to_gray_out; // key is the stage, float is how gray out it is

    uint32_t unknown47; // load state?
    uint32_t unknown48;
    uint8_t unknown49;
    uint8_t padding_probably[3];
    float panels_slide_from_both_sides;
    float visibility_all_stages;
    uint32_t selected_stage_index; // more like highlighted
    ScreenControls controls;
    uint8_t unknown57[3]; // some states, like screen depth
};

class ScreenArenaItems : public Screen // ID: 23
{
  public:
    MenuScreenPanels screen_panels;
    TextureRenderingInfo brick_background;
    TextureRenderingInfo black_background_bottom_right;
    TextureRenderingInfo woodpanel_bottom;
    TextureRenderingInfo scroll_bottom;
    TextureRenderingInfo scroll_right_handle_bottom;
    TextureRenderingInfo held_item_crate_on_scroll;
    TextureRenderingInfo held_item_on_scroll;
    TextureRenderingInfo item_background;
    TextureRenderingInfo toggles_background;
    TextureRenderingInfo item_selection_gold_outline;
    TextureRenderingInfo item_icons;
    TextureRenderingInfo item_held_badge;
    TextureRenderingInfo item_equipped_badge;
    TextureRenderingInfo item_off_gray_overlay;
    TextureRenderingInfo esc_woodpanel;

    custom_unordered_map<uint32_t, float> items_to_gray_out;

    float unknown33;
    float center_panels_horizontal_slide_position;
    float esc_panel_slide_timer;
    uint32_t selected_item_index; // more like highlighted
    ScreenControls controls;
    uint8_t unknown57[3]; // some states, like screen depth
};

class ScreenArenaIntro : public Screen // ID: 25
{
  public:
    TextureRenderingInfo players;
    TextureRenderingInfo background_colors;
    TextureRenderingInfo vertical_lines;
    TextureRenderingInfo vertical_line_electricity_effect;
    TextureRenderingInfo unknown_all_forced; // probably scroll
    TextureRenderingInfo left_scroll;
    TextureRenderingInfo right_scroll;
    TextureRenderingInfo unknown1;
    float scroll_unfurl;
    bool waiting; // when false, the cutscene ends and gameplay starts
    uint8_t unknown10b;
    uint8_t unknown10c;
    uint8_t unknown10d;
    uint16_t unknown11a;
    uint16_t unknown11c;
    float names_opacity;
    float line_electricity_effect_timer;
    uint8_t state; // 0 = start, 1 = show names, ...
    uint8_t unknown14b;
    uint8_t unknown14c;
    uint8_t unknown14d;
    uint32_t countdown; // when 0, continues to gameplay
    // uint32_t padding_probably

    std::array<ParticleEmitterInfo*, 9> particles;
};

class ScreenArenaLevel : public Screen // ID: 26
{
  public:
    uint32_t unknown3;
    TextureRenderingInfo get_ready;
    TextureRenderingInfo get_ready_backdrop; /* unsure: flies by too fast to see */
    TextureRenderingInfo get_ready_gray_background;
    TextureRenderingInfo get_ready_outline;
    TextureRenderingInfo unknown7;
    TextureRenderingInfo unknown8;
    TextureRenderingInfo unknown9;
    TextureRenderingInfo unknown10;
    TextureRenderingInfo unknown11;
    TextureRenderingInfo unknown12;
    TextureRenderingInfo unknown13;
    TextureRenderingInfo unknown14;
    TextureRenderingInfo unknown15;
    TextureRenderingInfo unknown16;
    TextureRenderingInfo unknown17;
    TextureRenderingInfo unknown18;
    TextureRenderingInfo unknown19;
    TextureRenderingInfo unknown20;
    TextureRenderingInfo unknown21;
    TextureRenderingInfo unknown22;
    TextureRenderingInfo unknown23;
    TextureRenderingInfo unknown24;
    TextureRenderingInfo unknown25;
    TextureRenderingInfo unknown26;
    TextureRenderingInfo unknown27;
    TextureRenderingInfo unknown28;
    TextureRenderingInfo unknown29;
    TextureRenderingInfo unknown30;
    TextureRenderingInfo unknown31;
    TextureRenderingInfo unknown32;
    TextureRenderingInfo unknown33;
    TextureRenderingInfo unknown34;
    TextureRenderingInfo unknown35;
    TextureRenderingInfo unknown36;
    TextureRenderingInfo unknown37;
    TextureRenderingInfo unknown38;
    TextureRenderingInfo unknown39;
    TextureRenderingInfo unknown40;
    TextureRenderingInfo unknown41;
    TextureRenderingInfo unknown42;
    TextureRenderingInfo unknown43;
    TextureRenderingInfo unknown44;
    TextureRenderingInfo unknown45;
    TextureRenderingInfo unknown46;
    TextureRenderingInfo unknown47;
    TextureRenderingInfo unknown48;
    TextureRenderingInfo unknown49;
    TextureRenderingInfo unknown50;
    TextureRenderingInfo unknown51;
    TextureRenderingInfo unknown52;
    TextureRenderingInfo unknown53;
    std::array<ParticleEmitterInfo*, 11> particles;
};

struct ScreenArenaScoreLavaBubble
{
    float x;
    float y;
    int16_t timer1;
    uint8_t timer2; // counts down when the timer1 = -1
    bool visible;
};

struct ScreenArenaScoreUnknown
{
    // this is mostly a guess
    TextureRenderingInfo texture;
    float some_x; // when not in use set to 9999.0
    float some_y; // when not in use set to 9999.0
    float unknown4;
    float unknown5;
    float unknown6;
    float unknown7;
    float unknown8; // when not in use set to float max
    float unknown9; // when not in use set to float max
    float unknown10;
    float unknown11;
    float unknown12;
};

class ScreenArenaScore : public Screen // ID: 27
{
  public:
    uint16_t unknown3a; // probably float
    uint16_t unknown3b;
    float woodpanel_slide;
    float scroll_unfurl;
    uint32_t unknown6;
    uint32_t unknown7;
    uint32_t unknown8;
    float unknown9;

    // similar to the MenuScreenPanels ?
    TextureRenderingInfo unknown10;
    TextureRenderingInfo woodpanel;
    TextureRenderingInfo unknown_all_forced; // probably scroll
    TextureRenderingInfo woodpanel_left_scroll;
    TextureRenderingInfo woodpanel_right_scroll;

    STRINGID text_id_1; // Strings table ID
    STRINGID text_id_2; // Strings table ID
    bool woodpanel_visible;
    uint8_t unknown13b;
    bool woodpanel_slide_toggle;
    uint8_t unknown13d;
    uint32_t animation_sequence; // confetti, ...

    TextureRenderingInfo background;
    TextureRenderingInfo ok_panel;
    TextureRenderingInfo ready_panel;
    TextureRenderingInfo ready_speechbubble_indicator;
    TextureRenderingInfo pillars;
    TextureRenderingInfo bottom_lava;
    TextureRenderingInfo players;
    TextureRenderingInfo player_shadows;
    TextureRenderingInfo unknown24;
    TextureRenderingInfo unknown25;
    TextureRenderingInfo score_counter;
    TextureRenderingInfo unknown27;
    TextureRenderingInfo lava_bubbles;

    std::array<bool, MAX_PLAYERS> player_won;
    float victory_jump_y_pos;
    float victory_jump_velocity;
    float unknown35;
    uint32_t animation_frame; /* unsure */ // when victory jumping
    bool unknown37;
    bool squash_and_celebrate;
    std::array<bool, MAX_PLAYERS> player_ready;
    bool unknown43;
    bool unknown44;
    uint32_t next_transition_timer;
    std::array<float, MAX_PLAYERS> player_bottom_pillar_offset; // 0.0 = target, depending on score
    std::array<float, MAX_PLAYERS> player_crushing_pillar_height;
    std::array<bool, MAX_PLAYERS> player_create_giblets;
    float next_sidepanel_slidein_timer;
    // uint32_t padding_probably;
    std::array<ParticleEmitterInfo*, 13> particles;
    std::array<ScreenArenaScoreLavaBubble, 15> lava_bubbles_positions;
    std::array<ScreenArenaScoreUnknown, 160> unknown45; // only used if one of the players win, probably the flying down confetti/feathers
};
