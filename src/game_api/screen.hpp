#pragma once

#include <array>      // for array
#include <cstddef>    // for size_t
#include <cstdint>    // for uint32_t, uint8_t, uint16_t, int32_t, int8_t
#include <functional> // for function
#include <string>     // for allocator, string
#include <vector>     // for vector

#include "aliases.hpp"                  // for STRINGID, MAX_PLAYERS
#include "color.hpp"                    // for Color
#include "containers/custom_vector.hpp" //
#include "render_api.hpp"               // for TextureRenderingInfo, TextRenderingInfo (p...

struct ParticleEmitterInfo;
struct SoundMeta;
struct MultiLineTextRendering;
class Entity;

class Screen
{
  public:
    float render_timer;
    uint32_t unknown_zero;

    // this first virtual does not appear to be the destructor in the game
    // but is made one here to appease Clang
    virtual ~Screen() = 0;
    virtual void v1() = 0;
    virtual void v2() = 0;
    virtual void render() = 0;

    std::uint32_t reserve_callback_id();
    void unhook(std::uint32_t id);
    struct ScreenHooksInfo& get_hooks();

    void set_pre_render(std::uint32_t reserved_callback_id, std::function<bool(Screen*)> pre_render);
    void set_post_render(std::uint32_t reserved_callback_id, std::function<void(Screen*)> post_render);

    template <typename T>
    T* as()
    {
        return static_cast<T*>(this);
    }
};

class ScreenLogo : public Screen // ID: 0
{
  public:
    TextureRenderingInfo logo_mossmouth;
    TextureRenderingInfo logo_blitworks;
    TextureRenderingInfo logo_fmod;
};

class ScreenIntro : public Screen // ID: 1
{
  public:
    TextureRenderingInfo unknown4;
};

class ScreenPrologue : public Screen // ID: 2
{
  public:
    STRINGID line1; // Strings table ID
    STRINGID line2; // Strings table ID
    STRINGID line3; // Strings table ID
};

class ScreenTitle : public Screen // ID: 3
{
  public:
    TextureRenderingInfo logo_spelunky2;
    TextureRenderingInfo ana;
    TextureRenderingInfo ana_right_eyeball_torch_reflection; // right from her perspective
    TextureRenderingInfo ana_left_eyeball_torch_reflection;  // left from her perspective
    ParticleEmitterInfo* particle_torchflame_smoke;
    ParticleEmitterInfo* particle_torchflame_backflames;
    ParticleEmitterInfo* particle_torchflame_flames;
    ParticleEmitterInfo* particle_torchflame_backflames_animated;
    ParticleEmitterInfo* particle_torchflame_flames_animated;
    ParticleEmitterInfo* particle_torchflame_ash;
    uint32_t unknown7;
    float unknown8;
    SoundMeta* music;
    SoundMeta* torch_sound;
};

struct SpearDanglerAnimFrames
{
    uint32_t column;
    uint32_t row;
};

class ScreenMenu : public Screen // ID: 4
{
  public:
    float unknown4;
    float unknown5;
    float unknown6;
    float unknown7;
    float unknown8;
    float unknown9;
    float unknown10;
    float unknown11;
    float unknown12;
    uint32_t unknown13;
    TextureRenderingInfo tunnel_background;
    TextureRenderingInfo cthulhu_disc;
    TextureRenderingInfo tunnel_ring_darkbrown;
    TextureRenderingInfo cthulhu_body;
    TextureRenderingInfo tunnel_ring_lightbrown;
    TextureRenderingInfo vine_left;
    TextureRenderingInfo vine_right;
    TextureRenderingInfo skull_left;
    TextureRenderingInfo salamander_right;
    TextureRenderingInfo left_spear;
    TextureRenderingInfo right_spear;
    TextureRenderingInfo spear_dangler_related;
    TextureRenderingInfo play_scroll;
    TextureRenderingInfo info_toast;
    TextureRenderingInfo unknown15;

    uint32_t unknown16a;
    uint32_t unknown16b;
    SoundMeta* cthulhu_sound;
    size_t unknown16e;
    size_t unknown16f;
    float unknown16g;
    float unknown16h;
    float unknown16i;
    float unknown16j;
    size_t unknown16k;
    std::vector<size_t> unknown17;   // pointers
    std::vector<uint32_t> unknown20; // unsure what's inside
    size_t buttons;
    uint32_t unknown23;
    uint32_t unknown24;
    uint32_t selected_menu_index;
    uint32_t unknown26;
    uint32_t unknown27;
    uint32_t unknown28;
    uint32_t unknown29a;
    uint32_t unknown29b;
    float menu_text_opacity;
    std::array<float, 6> spear_position;
    std::array<SpearDanglerAnimFrames, 6> spear_dangler;
    std::array<uint32_t, 6> spear_dangle_momentum;
    std::array<uint32_t, 6> spear_dangle_angle;

    float play_scroll_descend_timer;
    STRINGID scroll_text;

    float cthulhu_disc_ring_angle;
    float cthulhu_disc_split_progress;
    float cthulhu_disc_y;
    float cthulhu_timer;
};

class ScreenOptions : public Screen // ID: 5
{
  public:
    size_t unknown4;
    size_t unknown5;
    size_t unknown6;
    size_t unknown7;
    size_t unknown8;
    size_t unknown9;

    uint32_t selected_menu_index;
    uint16_t key_press_timer; // might be two separate values

    uint16_t unknown12;

    bool moved_left;

    uint8_t padding1;
    uint8_t padding2;
    uint8_t padding3;
    uint8_t unknown14;
    uint8_t unknown15;
    uint8_t unknown16;
    uint8_t unknown17;

    TextureRenderingInfo brick_border;
    float top_bottom_woodpanels_velocity;
    float top_bottom_woodpanels_progress; // set to 0 to start sliding in
    float scroll_unfurl_progress;         // set to 0 to start unfurl
    float unknown21;
    float bottom_woodpanel_y;
    float unknown23;
    float top_bottom_woodpanels_slide_in_related;
    TextureRenderingInfo bottom_woodpanel;
    TextureRenderingInfo top_woodpanel;
    TextureRenderingInfo unknown27;
    TextureRenderingInfo top_woodpanel_left_scrollhandle;
    TextureRenderingInfo top_woodpanel_right_scrollhandle;

    STRINGID button_right_caption;
    STRINGID button_middle_caption;
    bool top_woodpanel_visible;
    bool bottom_woodpanel_visible;
    bool toggle_woodpanel_slidein_animation;
    bool capitalize_top_woodpanel;
    uint32_t unknown28;
    uint32_t current_menu_1;
    uint32_t current_menu_2;
    uint32_t unknown31;
    uint32_t unknown32;
    uint32_t unknown33;
    uint32_t unknown34;
    uint32_t unknown35;
    uint32_t unknown36;
    uint32_t unknown37;
    uint32_t unknown38;

    TextureRenderingInfo topleft_woodpanel_esc;
    TextureRenderingInfo brick_background;
    TextureRenderingInfo brick_middlelayer;
    TextureRenderingInfo brick_foreground;
    TextureRenderingInfo selected_item_rounded_rect;
    TextureRenderingInfo selected_item_scarab;
    TextureRenderingInfo item_option_arrow_left;
    TextureRenderingInfo item_option_arrow_right;
    TextureRenderingInfo tooltip_background;
    TextureRenderingInfo progressbar_background; // brightness 'progressbar' background texture
    TextureRenderingInfo unknown40;
    TextureRenderingInfo progressbar_foreground;
    TextureRenderingInfo progressbar_position_indicator;
    TextureRenderingInfo sectionheader_background; // behind 'GRAPHICS' and 'AUDIO'

    TextureRenderingInfo unknown44;
    TextureRenderingInfo unknown45;
    float topleft_woodpanel_esc_slidein_timer;
    float text_fadein_timer;
    float vertical_scroll_effect_timer;
    uint8_t unknown49;
    uint8_t unknown50;
    uint8_t unknown51;
};

class ScreenPlayerProfile : public Screen // ID: 6
{
  public: // not reverse engineered
};

class ScreenLeaderboards : public Screen // ID: 7
{
  public: // not reverse engineered
};

class ScreenSeedInput : public Screen // ID: 8
{
  public:
    uint16_t unknown4;
    uint16_t unknown5;
    float bottom_woodpanel_slideup_timer;
    float unknown_timer;
    float unknown6;
    float bottom_woodpanel_y;
    TextureRenderingInfo bottom_woodpanel;
    TextureRenderingInfo unknown8;
    TextureRenderingInfo unknown9;
    TextureRenderingInfo unknown10;
    TextureRenderingInfo unknown11;
    STRINGID unknown12;
    STRINGID unknown13;
    STRINGID buttons_text_id;
    STRINGID unknown15;
    uint8_t unknown16;
    uint8_t unknown17;
    uint8_t unknown18;
    uint8_t unknown19;
    uint8_t unknown20;
    uint8_t unknown21;
    uint8_t unknown22;
    uint8_t unknown23;
    uint32_t unknown24;
    uint16_t unknown25;

    uint16_t seed_chars[9]; // utf16 chars

    uint16_t unknown38;
    uint16_t unknown39;

    float topleft_woodpanel_esc_slidein_timer;
    STRINGID scroll_text_id;
    STRINGID start_text_id;
    TextureRenderingInfo main_woodpanel_left_border;
    TextureRenderingInfo main_woodpanel_center;
    TextureRenderingInfo main_woodpanel_right_border;
    TextureRenderingInfo top_scroll;
    TextureRenderingInfo seed_letter_cutouts;
    TextureRenderingInfo unknown40;
    TextureRenderingInfo unknown41;
    TextureRenderingInfo topleft_woodpanel_esc;
    TextureRenderingInfo start_sidepanel;
    float start_sidepanel_slidein_timer;
};

struct FlyingThing
{
    TextureRenderingInfo texture_info;
    ENT_TYPE entity_type;
    bool unknown1;
    uint8_t padding1;
    uint8_t padding2;
    uint8_t padding3;
    float unknown2;
    float unknown3;
    float spritesheet_column;
    float spritesheet_row;
    float spritesheet_animation_length; // how many sprites are in the cycle
    float unknown6;
    uint8_t unknown7;
    uint8_t unknown8;
    uint8_t unknown9;
    uint8_t unknown10;
    float velocity_x;
    float amplitude;
    float frequency;
    float sinewave_angle;
    float unknown15;
    float unknown16;
    uint8_t unknown17;
    uint8_t unknown18;
    uint8_t unknown19;
    uint8_t unknown20;
    uint32_t unknown21;
    uint32_t unknown22;
};

class ScreenCharacterSelect : public Screen // ID: 9
{
  public:
    float unknown4;
    float unknown5;
    float unknown6;
    float unknown7;
    float main_background_zoom_related1;
    uint32_t main_background_zoom_related2;
    uint32_t unknown10;
    float main_background_zoom_timer;
    float main_background_zoom_target;
    float blurred_border_zoom;
    float unknown14;
    float unknown15;
    float unknown16;
    float unknown17;
    uint32_t blurred_border_animation_frame; /* unsure */
    uint8_t unknown19;
    uint8_t unknown20;
    uint8_t unknown21;
    uint8_t unknown22;
    float blurred_border_zoom_timer;
    float blurred_border_zoom_target;
    float unknown25;
    float top_bottom_woodpanel_slidein_timer;
    float top_scroll_unfurl_timer;
    float unknown28;
    uint32_t unknown29;
    uint32_t unknown30;
    TextureRenderingInfo bottom_woodpanel;
    TextureRenderingInfo top_woodpanel;
    TextureRenderingInfo unknown32;
    TextureRenderingInfo left_scroll_handle;
    TextureRenderingInfo right_scroll_handle;
    STRINGID left_button_text_id;
    STRINGID right_button_text_id;
    STRINGID middle_button_text_id;
    bool top_woodpanel_visible;
    bool bottom_woodpanel_visible;
    bool toggle_woodpanel_slidein_animation;
    uint8_t padding1;
    TextureRenderingInfo mine_entrance_background;
    TextureRenderingInfo character;
    TextureRenderingInfo character_shadow;
    TextureRenderingInfo character_flag;
    TextureRenderingInfo character_left_arrow;
    TextureRenderingInfo character_right_arrow;
    TextureRenderingInfo unknown45;
    TextureRenderingInfo mine_entrance_border;
    TextureRenderingInfo mine_entrance_shutter;
    TextureRenderingInfo unknown47;
    TextureRenderingInfo background;
    TextureRenderingInfo blurred_border;
    TextureRenderingInfo blurred_border2;
    TextureRenderingInfo unknown51;
    TextureRenderingInfo topleft_woodpanel_esc;
    TextureRenderingInfo start_sidepanel;
    TextureRenderingInfo quick_select_panel;
    TextureRenderingInfo quick_select_selected_char_background;
    TextureRenderingInfo quick_select_panel_related;

    std::array<float, MAX_PLAYERS> player_shutter_timer;
    std::array<float, MAX_PLAYERS> player_x;
    std::array<float, MAX_PLAYERS> player_y;
    std::array<float, MAX_PLAYERS> player_unknown;

    float player_walk_timer; /* unsure */ // faster timer to make them walk fast when selecting a different character?

    std::array<std::array<float, 2>, MAX_PLAYERS> player_arrow_slidein_timer; // 4 sets of (left, right) floats
    std::array<bool, MAX_PLAYERS> player_facing_left;
    std::array<bool, MAX_PLAYERS> player_quickselect_shown;
    std::array<float, MAX_PLAYERS> player_quickselect_fadein_timer;
    std::array<std::array<float, 2>, MAX_PLAYERS> player_quickselect_coords; // 4 sets of (column, row) floats
    std::array<float, MAX_PLAYERS> player_quickselect_wiggle_angle;

    float another_timer;
    float topleft_woodpanel_esc_slidein_timer;
    float start_panel_slidein_timer;
    float action_buttons_keycap_size;
    bool unknown64a;
    bool unknown64b;
    bool unknown64c;
    bool unknown64d;
    uint32_t unknown65;
    bool not_ready_to_start_yet;
    uint8_t available_mine_entrances; // The rest are boarded off
    uint8_t amount_of_mine_entrances_activated;
    uint8_t unknown66;
    size_t reset_func; /* unsure*/
    uint32_t buttons;
    TextureRenderingInfo unknown69;

    float opacity;
    bool start_pressed;
    bool transition_to_game_started;
    uint8_t unknown71c;
    uint8_t unknown71d;
    uint32_t unknown72;

    std::array<FlyingThing, 6> flying_things;
    uint16_t flying_thing_countdown; // when 0, flying things appear
    int16_t unknown79;
    uint32_t unknown80;

    ParticleEmitterInfo* particle_ceilingdust_smoke;
    ParticleEmitterInfo* particle_ceilingdust_rubble;
    ParticleEmitterInfo* particle_mist;
    ParticleEmitterInfo* particle_torchflame_smoke1;
    ParticleEmitterInfo* particle_torchflame_flames1;
    ParticleEmitterInfo* particle_torchflame_smoke2;
    ParticleEmitterInfo* particle_torchflame_flames2;
    ParticleEmitterInfo* particle_torchflame_smoke3;
    ParticleEmitterInfo* particle_torchflame_flames3;
    ParticleEmitterInfo* particle_torchflame_smoke4;
    ParticleEmitterInfo* particle_torchflame_flames4;
    SoundMeta* sound;
};

class ScreenTeamSelect : public Screen // ID: 10
{
  public:
    TextureRenderingInfo ana_carrying_torch;
    TextureRenderingInfo scroll_bottom_left;
    TextureRenderingInfo scrollend_bottom_left;
    TextureRenderingInfo four_ropes;
    TextureRenderingInfo unknown4;
    TextureRenderingInfo four_characters;
    TextureRenderingInfo left_arrow;
    TextureRenderingInfo right_arrow;
    TextureRenderingInfo start_panel;
    float start_panel_slide_timer;
    float pulsating_arrows_timer;
    uint8_t selected_player;
    uint8_t buttons;
    bool ready;
    uint8_t unknown4d;
    uint32_t unknown5;
    uint32_t unknown6;
    uint32_t unknown7;
};

class ScreenCamp : public Screen // ID: 11
{
  public:
    uint8_t buttons;
};

class ScreenLevel : public Screen // ID: 12
{
  public:
    uint8_t buttons;
};

class ScreenTransition : public Screen // ID: 13
{
  public:
    float woodpanel_pos;               // from 1.3 (invisible) to 0 (in its place)
    float stats_scroll_horizontal_pos; // from 1 (invisible) to 0 (in its place)
    float stats_scroll_vertical_pos;   // from about 1.30 (invisible) to 0 (in its place)
    float level_completed_pos;         // top left thing on the chains that slides in, showing e.g. 1-2 COMPLETED! from 0.5 to 0
    float stats_scroll_unfurl_actualvalue;
    float stats_scroll_unfurl_targetvalue;

    TextureRenderingInfo woodpanel1;
    TextureRenderingInfo woodpanel2;
    TextureRenderingInfo woodpanel3;
    TextureRenderingInfo woodpanel_cutout1;
    TextureRenderingInfo woodpanel_cutout2;
    TextureRenderingInfo woodpanel_cutout3;
    TextureRenderingInfo woodplank;
    TextureRenderingInfo woodpanel_bottomcutout1;
    TextureRenderingInfo woodpanel_bottomcutout2;
    TextureRenderingInfo woodpanel_bottomcutout3;
    TextureRenderingInfo unknown_all_forced;
    TextureRenderingInfo stats_scroll_top_bottom;
    TextureRenderingInfo killcount_rounded_rect;
    TextureRenderingInfo level_completed_panel;

    uint32_t stats_scroll_state_1;
    uint32_t stats_scroll_state_2;
    bool hide_press_to_go_next_level;
    bool unknown2b;
    bool unknown2c;
    bool unknown2d;

    TextureRenderingInfo mama_tunnel;
    TextureRenderingInfo speechbubble;
    TextureRenderingInfo speechbubble_arrow;

    float unknown5;
    float unknown6;
    float unknown7;
    float mama_tunnel_fade_actualvalue;
    float mama_tunnel_fade_targetvalue;
    STRINGID mama_tunnel_text_id;
    uint16_t mama_tunnel_text_buffer[256]; // UTF16 string
    bool mama_tunnel_choice_visible;
    bool mama_tunnel_agree_with_gift;
    bool mama_tunnel_face_invisible;
    uint8_t unknown18; // probably padding
    float mama_tunnel_face_transparency;
    TextureRenderingInfo mama_tunnel_agree_panel;
    TextureRenderingInfo mama_tunnel_agree_panel_indicator;
    TextureRenderingInfo woodpanel_cutout_big_money1;
    TextureRenderingInfo woodpanel_cutout_big_money2;
    TextureRenderingInfo woodpanel_cutout_big_money3;
    TextureRenderingInfo big_dollar_sign;
    TextureRenderingInfo unknown26;

    uint16_t string_buffer[130]; // UTF16 string
    uint32_t stats_scroll_unfurl_sequence_timer;
    uint32_t unknown30;
    uint32_t unknown31;

    std::array<int32_t, MAX_PLAYERS> player_stats_scroll_numeric_value;
    std::array<TextureRenderingInfo, MAX_PLAYERS> player_secondary_icon;
    std::array<TextureRenderingInfo, MAX_PLAYERS> player_icon;
    std::array<int32_t, MAX_PLAYERS> player_secondary_icon_type; // something strange going on here: same nr for diff players yields diff sprite
    std::array<int32_t, MAX_PLAYERS> player_icon_index;

    TextureRenderingInfo unknown32; // probably also something per player
    TextureRenderingInfo unknown33;
    TextureRenderingInfo unknown34;
    TextureRenderingInfo unknown35;

    TextureRenderingInfo hourglasses;
    TextureRenderingInfo small_dollar_signs;

    uint16_t string_buffer_this_level_time[10];        // UTF16 string
    uint16_t string_buffer_total_time[10];             // UTF16 string
    uint16_t string_buffer_this_level_milliseconds[5]; // UTF16 string
    uint16_t string_buffer_total_milliseconds[5];      // UTF16 string
    uint16_t string_buffer_this_level_money[10];       // UTF16 string
    uint16_t string_buffer_total_money[10];            // UTF16 string

    Color this_level_money_color;

    uint8_t unknown41;
    uint8_t unknown42;
    uint8_t unknown43;
    uint8_t unknown44;
    uint32_t unknown45;
    uint32_t unknown46;
    uint32_t unknown47;
    uint32_t unknown48;
    uint32_t unknown49;
};

/// The POST render call will only be visible in the polaroid area on the left of the book. The book is apparently drawn on top of that.
class ScreenDeath : public Screen // ID: 14
{
};

class ScreenWin : public Screen // ID: 16
{
  public:
    size_t unknown2;
    size_t unknown3;
    size_t unknown4;
    size_t unknown5;

    uint16_t sequence_timer; // counts descent until door opens, closes, ...
    uint16_t frame_timer;
    uint32_t animation_state;
    Entity* rescuing_ship_entity;

    size_t unknown8;
    uint32_t unknown9;
    uint32_t unknown10;
};

class ScreenCredits : public Screen // ID: 17
{
  public:
    float* credits_progression;
    size_t bg_music_info; /* unsure */
};

class ScreenScores : public Screen // ID: 18
{
  public:
    uint32_t animation_state; // states indicating whether money, time is shown, and to drop the treasure
    TextureRenderingInfo woodpanel1;
    TextureRenderingInfo woodpanel2;
    TextureRenderingInfo woodpanel3;
    TextureRenderingInfo woodpanel_cutout;
    TextureRenderingInfo dollarsign;
    TextureRenderingInfo hourglass;
    uint32_t animation_timer; // wait time to show the wood panel, score and timer text, ...
    float woodpanel_slidedown_timer;
};

class ScreenConstellation : public Screen // ID: 19
{
  public:
    uint16_t sequence_state;
    uint16_t animation_timer;
    float constellation_text_opacity;
    char16_t constellation_text[32]; // UTF16 string
    float* credits_progression;
    SoundMeta* explosion_and_loop;
    SoundMeta* music;
};

/// The recap book is drawn on top of the POST render event
class ScreenRecap : public Screen // ID: 20
{
};

struct ScreenZoomAnimation
{
    float unknown1;
    float unknown2;
    float unknown3;
    float unknown4;
    float unknown5;
    uint32_t image; /* unsure*/
    uint32_t unknown6;
    float zoom_timer;
    float zoom_target;
};

// For the ARENA screens, see screen_arena.hpp
// Putting them here makes the compiler run out of heap space

class ScreenOnlineLoading : public Screen // ID: 28
{
  public:
    TextureRenderingInfo ouroboros;
    float ouroboros_angle;
};

struct OnlineLobbyScreenPlayer
{
    uint8_t unknown1;
    uint8_t character;
    bool ready;
    uint8_t unknown2;
};

class ScreenOnlineLobby : public Screen // ID: 29
{
  public:
    uint8_t unknown2;
    uint8_t unknown3;
    uint8_t unknown4;
    uint8_t unknown5;
    float woodpanels_slidein_timer;
    float scroll_unfurl_timer;
    uint32_t unknown8;
    uint32_t unknown9;
    TextureRenderingInfo woodpanel_bottom;
    TextureRenderingInfo woodpanel_top;
    TextureRenderingInfo unknown13;
    TextureRenderingInfo left_scroll_handle;
    TextureRenderingInfo right_scroll_handle;
    STRINGID scroll_text_id;
    STRINGID btn_left_text_id;
    STRINGID btn_right_text_id;
    STRINGID btn_center_text_id;
    bool woodpanel_top_visible;
    bool woodpanel_bottom_visible;
    bool toggle_panels_slidein;
    bool unknown21;
    std::array<OnlineLobbyScreenPlayer, 4> players;
    TextureRenderingInfo background_image;
    size_t unknown22;
    size_t unknown23;
    size_t unknown24;
    size_t unknown25;
    size_t unknown26;
    size_t unknown27;
    size_t unknown28;
    size_t unknown29;
    size_t unknown30;
    TextureRenderingInfo unknown36;
    TextureRenderingInfo unknown37;
    float unknown38;
    TextureRenderingInfo topleft_woodpanel_esc;
    float topleft_woodpanel_esc_slidein_timer;
    float character_walk_offset;
    bool character_facing_left;
    int8_t move_direction;
    uint8_t unknown41;
    uint8_t unknown42;
    TextureRenderingInfo character;
    TextureRenderingInfo player_ready_icon;
    float render_timer_dupe;
    TextureRenderingInfo arrow_left;
    TextureRenderingInfo arrow_right;
    float another_timer;
    float arrow_left_hor_offset;
    float arrow_right_hor_offset;
    TextureRenderingInfo platform_icon;
    uint8_t player_count;
    bool searching_for_players;
    uint8_t unknown47;
    uint8_t unknown48;
    float another_timer_2;
    bool show_code_panel;
    uint8_t unknown49;
    uint8_t unknown50;
    uint8_t unknown51;
    uint32_t unknown53;

    // The following is actually class ScreenEnterOnlineCode but it has no direct pointer in GameManager
    // or State. In assembly this pointer is accessed by &ScreenOnlineLobby + sizeof(ScreenOnlineLobby)
    size_t enter_code_screen_vftable;
    float enter_code_render_timer;
    uint32_t unknown54;
    float unknown56;
    float enter_code_woodpanel_bottom_slidein_pos;
    float unknown58;
    float unknown59;
    float unknown60;
    TextureRenderingInfo enter_code_woodpanel_bottom;
    TextureRenderingInfo unknown61;
    TextureRenderingInfo unknown62;
    TextureRenderingInfo unknown63;
    TextureRenderingInfo unknown64;
    STRINGID text_id_1;
    STRINGID text_id_2;
    STRINGID enter_code_btn_right_text_id;
    STRINGID text_id_4;
    bool enter_code_woodpanel_top_visible;
    bool enter_code_woodpanel_bottom_visible;
    bool enter_code_toggle_panels_slidein;
    bool unknown68;
    uint32_t unknown69;
    uint32_t selected_character;
    bool unknown71a;
    uint8_t unknown71b;
    uint16_t code_chars[8];
    uint16_t code_char_terminator;
    uint32_t characters_entered_count;
    float enter_code_topleft_woodpanel_esc_slidein_timer;
    STRINGID enter_code_banner_text_id;
    STRINGID enter_code_OK_text_id;
    TextureRenderingInfo enter_code_main_woodpanel_left;
    TextureRenderingInfo enter_code_main_woodpanel_center;
    TextureRenderingInfo enter_code_main_woodpanel_right;
    TextureRenderingInfo enter_code_banner;
    TextureRenderingInfo enter_code_char_cutouts;
    TextureRenderingInfo enter_code_pointing_hand;
    TextureRenderingInfo enter_code_buttons;
    TextureRenderingInfo unknown85;
    TextureRenderingInfo enter_code_OK_panel;
    float enter_code_OK_panel_slidein_timer;
    int32_t unknown87;
    TextureRenderingInfo enter_code_your_code_scroll;
    TextureRenderingInfo enter_code_your_code_scroll_left_handle;
    TextureRenderingInfo enter_code_your_code_scroll_right_handle;

    void set_code(const std::string& code);
};

struct PauseUI
{
    float menu_slidein_progress;
    TextureRenderingInfo blurred_background;
    TextureRenderingInfo woodpanel_left;
    TextureRenderingInfo woodpanel_middle;
    TextureRenderingInfo woodpanel_right;
    TextureRenderingInfo woodpanel_top;
    TextureRenderingInfo scroll;

    uint32_t unknown2;
    size_t unknown3;

    TextureRenderingInfo confirmation_panel;

    uint32_t unknown5;
    uint32_t unknown6;
    size_t unknown7;
    uint32_t unknown8;

    uint32_t previously_selected_menu_index;
    uint32_t buttons_actions;
    uint32_t buttons_movement;
    uint8_t unknown11a;
    int8_t unknown11b;
    uint16_t unknown12;
    uint32_t visibility;
};

class JournalPage
{
  public:
    TextureRenderingInfo background;
    uint32_t page_number;
    uint32_t unknown2;

    template <typename T>
    T* as()
    {
        return static_cast<T*>(this);
    }

    /// background.x < 0
    bool is_right_side_page();
    void set_page_background_side(bool right);

    virtual ~JournalPage() = 0;
    virtual void v1() = 0;
    virtual void v2() = 0;
    virtual void v3() = 0;
    virtual void render() = 0;
};

class JournalPageProgress : public JournalPage
{
  public:
    TextureRenderingInfo coffeestain_top;

    virtual ~JournalPageProgress() = 0;
};

class JournalPageJournalMenu : public JournalPage
{
  public:
    float unknown3;
    float unknown4;
    float unknown5;
    float unknown6;
    float unknown7;
    float unknown8;
    float unknown9;
    float unknown10;
    uint32_t unknown11;
    float unknown12;
    size_t unknown13;
    size_t unknown15;
    size_t unknown17;
    size_t unknown19;
    size_t unknown21;
    uint32_t selected_menu_index;
    uint32_t unknown23;
    TextRenderingInfo* journal_text_info;
    TextureRenderingInfo completion_badge;

    virtual ~JournalPageJournalMenu() = 0;
};

class JournalPageDiscoverable : public JournalPage
{
  public:
    bool show_main_image;
    uint8_t unknown3b;
    uint8_t unknown3c;
    uint8_t unknown3d;
    uint32_t unknown4;
    uint32_t unknown5;
    uint32_t unknown6;
    uint32_t unknown7;
    float unknown8;
    uint32_t unknown9;
    float unknown10;
    TextRenderingInfo* title_text_info;
    MultiLineTextRendering* text_lines;
    TextRenderingInfo* entry_text_info;
    TextRenderingInfo* chapter_title_text_info;

    virtual ~JournalPageDiscoverable() = 0;
};

class JournalPagePlaces : public JournalPageDiscoverable
{
  public:
    TextureRenderingInfo main_image;

    virtual ~JournalPagePlaces() = 0;
};

class JournalPagePeople : public JournalPageDiscoverable
{
  public:
    TextureRenderingInfo character_background;
    TextureRenderingInfo character_icon;
    TextureRenderingInfo character_drawing;

    virtual ~JournalPagePeople() = 0;
};

class JournalPageBestiary : public JournalPageDiscoverable
{
  public:
    TextureRenderingInfo monster_background;
    TextureRenderingInfo monster_icon;
    TextureRenderingInfo defeated_killedby_black_bars;
    TextRenderingInfo* defeated_text_info;
    TextRenderingInfo* defeated_value_text_info;
    TextRenderingInfo* killedby_text_info;
    TextRenderingInfo* killedby_value_text_info;

    virtual ~JournalPageBestiary() = 0;
};

class JournalPageItems : public JournalPageDiscoverable
{
  public:
    TextureRenderingInfo item_icon;
    TextureRenderingInfo item_background;

    virtual ~JournalPageItems() = 0;
};

class JournalPageTraps : public JournalPageDiscoverable
{
  public:
    TextureRenderingInfo trap_icon;
    TextureRenderingInfo trap_background;

    virtual ~JournalPageTraps() = 0;
};

class JournalPageStory : public JournalPage
{
  public:
    virtual ~JournalPageStory() = 0;

    static JournalPageStory* construct(bool right_side, uint32_t page_number);
};

class JournalPageFeats : public JournalPage
{
  public:
    TextRenderingInfo* chapter_title_text_info;
    TextureRenderingInfo feat_icons;

    virtual ~JournalPageFeats() = 0;
};

class JournalPageDeathCause : public JournalPage
{
  public:
    TextRenderingInfo* death_cause_text_info;

    virtual ~JournalPageDeathCause() = 0;
};

class JournalPageDeathMenu : public JournalPage
{
  public:
    float unknown3;
    float unknown4;
    float unknown5;
    float unknown6;
    float unknown7;
    float unknown8;
    float unknown9;
    float unknown10;
    uint32_t unknown11;
    float unknown12;
    size_t unknown13;
    size_t unknown14;
    size_t unknown15;
    size_t unknown16;
    size_t unknown17;
    uint32_t selected_menu_index;
    uint32_t unknown18;
    TextRenderingInfo* game_over_text_info;
    TextRenderingInfo* level_text_info;
    TextRenderingInfo* level_value_text_info;
    TextRenderingInfo* money_text_info;
    TextRenderingInfo* money_value_text_info;
    TextRenderingInfo* time_text_info;
    TextRenderingInfo* time_value_text_info;

    virtual ~JournalPageDeathMenu() = 0;
};

class JournalPageRecap : public JournalPage
{
  public:
    virtual ~JournalPageRecap() = 0;
};

class JournalPagePlayerProfile : public JournalPage
{
  public:
    TextureRenderingInfo player_icon;
    uint32_t player_icon_id;
    float unknown4;
    TextRenderingInfo* player_profile_text_info;
    TextRenderingInfo* plays_text_info;
    TextRenderingInfo* plays_value_text_info;
    TextRenderingInfo* wins_text_info;
    TextRenderingInfo* wins_value_text_info;
    TextRenderingInfo* deaths_text_info;
    TextRenderingInfo* deaths_value_text_info;
    TextRenderingInfo* win_pct_text_info;
    TextRenderingInfo* win_pct_value_text_info;
    TextRenderingInfo* average_score_text_info;
    TextRenderingInfo* average_score_value_text_info;
    TextRenderingInfo* top_score_text_info;
    TextRenderingInfo* top_score_value_text_info;
    TextRenderingInfo* deepest_level_text_info;
    TextRenderingInfo* deepest_level_value_text_info;
    TextRenderingInfo* deadliest_level_text_info;
    TextRenderingInfo* deadliest_level_value_text_info;
    TextRenderingInfo* average_time_text_info;
    TextRenderingInfo* average_time_value_text_info;
    TextRenderingInfo* best_time_text_info;
    TextRenderingInfo* best_time_value_text_info;

    virtual ~JournalPagePlayerProfile() = 0;
};

class JournalPageLastGamePlayed : public JournalPage
{
  public:
    TextureRenderingInfo main_image;
    TextRenderingInfo* last_game_played_text_info;
    TextRenderingInfo* level_text_info;
    TextRenderingInfo* level_value_text_info;
    TextRenderingInfo* money_text_info;
    TextRenderingInfo* money_value_text_info;
    TextRenderingInfo* time_text_info;
    TextRenderingInfo* time_value_text_info;
    uint32_t sticker_count;
    std::array<TextureRenderingInfo, 20> stickers;

    virtual ~JournalPageLastGamePlayed() = 0;
};

struct JournalUI
{
    uint32_t state;
    uint8_t chapter_shown;

    uint8_t unknown1;
    uint16_t unknown2;
    custom_vector<JournalPage*> pages;     // adding pages directly to it crash the game (on vector resize)
    custom_vector<JournalPage*> pages_tmp; // pages are constructed in the show_journal function and put here, later transfered to the pages vector
    uint32_t current_page;
    uint32_t flipping_to_page;
    uint32_t unknown10;
    int32_t max_page_count; // influences the right arrow shown and being able to move to the next page

    size_t unknown12;
    size_t unknown13;
    size_t unknown14;
    size_t unknown15; // ptr
    size_t unknown16;
    size_t unknown17;
    size_t unknown18;
    size_t unknown19;
    size_t unknown20; // ptr
    size_t unknown21;

    TextureRenderingInfo book_background;
    TextureRenderingInfo arrow_left;
    TextureRenderingInfo arrow_right;
    TextureRenderingInfo unknown23;
    TextureRenderingInfo entire_book;

    float unknown25;
    float unknown26;

    uint32_t page_timer;

    uint32_t unknown28;
    size_t unknown29;
    float unknown31;
    uint32_t unknown30;
};

Screen* get_screen_ptr(uint32_t screen_id);
