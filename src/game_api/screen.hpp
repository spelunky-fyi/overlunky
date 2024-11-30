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

struct MenuScreenPanels
{
    float woodpanels_velocity;
    float woodpanels_progress;
    float scroll_unfurl_progress;
    float bottom_woodpanel_speed_multiplayer;
    float bottom_woodpanel_y_offset; // maybe a resolution thing?
    TextureRenderingInfo bottom_woodpanel;
    TextureRenderingInfo top_woodpanel;
    TextureRenderingInfo scroll;
    TextureRenderingInfo top_woodpanel_left_scrollhandle;
    TextureRenderingInfo top_woodpanel_right_scrollhandle;

    STRINGID scroll_text;
    STRINGID bottom_left_text;
    STRINGID bottom_right_text;
    STRINGID bottom_middle_text;
    bool top_woodpanel_visible;
    bool bottom_woodpanel_visible;
    bool toggle_woodpanel_slidein_animation;
    bool capitalize_scroll_text;
};

struct ScreenControls
{
    bool up;
    bool down;
    bool left;
    bool right;
    /// -1 - none, 0 - UP, 1 - DOWN, 2 - LEFT, 3 - RIGHT
    uint32_t direction_input;
    /// Delay after which fast scroll activates (can stop at different value, only matters when you hold down the direction button)
    uint32_t hold_down_timer;
    uint32_t fast_scroll_timer;
};

// probably common thing, right now only used in the arena screen
struct ScreenZoomAnimation
{
    float unknown1;
    float unknown2;
    float unknown3;
    float unknown4;
    float unknown5;
    uint32_t image; /* unsure*/ // probably wrong
    uint32_t unknown6;
    float zoom_timer;
    float zoom_target;
};

class Screen
{
  public:
    float render_timer;
    uint32_t unknown_zero;

    /// Initializes the screen.
    virtual void init() = 0;
    virtual void update() = 0; // runs each frame, for level screens: death, camera zoom (level/shop), camera bounds, some save data stuff
    virtual ~Screen() = 0;
    virtual void render() = 0; // mostly used by the non gameplay screens to draw textures, text, UI

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
    /// 0 - mossmouth, 1 - blitworks, 2 - fmod, 3 - end (next screen)
    uint32_t state;
    uint32_t timer;
};

class ScreenIntro : public Screen // ID: 1
{
  public:
    TextureRenderingInfo blackout_background;
    float blackout_alpha;
    /// ends the intro immediately if set to false
    bool active;
    /// skips prologue and goes straight to the title screen after the intro
    bool skip_prologue;
};

class ScreenPrologue : public Screen // ID: 2
{
  public:
    STRINGID line1; // Strings table ID
    STRINGID line2; // Strings table ID
    STRINGID line3; // Strings table ID
    float line1_alpha;
    float line2_alpha;
    float line3_alpha;
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
    uint8_t unknown7a; // random very short timer that doesn't seam to do anything
    float brightness;
    SoundMeta* music;
    SoundMeta* torch_sound;
};

struct MenuOption
{
    // return and first param are the same, pointer on stack, it really seam to be just two 32bit fields
    // int is normally set to 0x2B and float to 0, if the third parameter is not 2, then the function sets first to 0, and the float to float max,
    using OptionSelect = std::pair<uint32_t, float>&(std::pair<uint32_t, float>&, Screen* src, uint8_t);

    STRINGID option_name;
    float y_position;
    float brigthness; // 1.0 for selected item, 0 for non selected
    bool enabled;     // only visual thing, locks brightness at 0
    // uint8_t padding_probably[3]

    OptionSelect* select; // called as soon as you hit enter on the selected option
    // function is expected to play the select sound
    // for main menu it also stops the cthulhu_sound (call kill(true) and set the pointer to null) when leaving the menu to different screen
};

class ScreenMenu : public Screen // ID: 4
{
  public:
    using ScreenTransition = void();

    float backlayer_transition_speed;
    float frontlayer_transition_speed;
    float unknown6;
    float unknown7;
    float unknown8;              // middle layer related?
    uint32_t some_unknown_state; // 5 in main menu, 1 when going to different screen
    float unknown10;
    float zoom_in_progress;
    float zoom_limit;
    /// 0: "cthulhu_pre_movement",
    /// 1: "cthulhu_rotating",
    /// 2: "cthulhu_separating",
    /// 3: "cthulhu_lowering",
    /// 4: "cthulhu_transition_to_menu",
    /// 5: "return_from_backlayer",
    /// 6: "highlight_selection",
    /// 7: "idle",
    /// 8: "to_submenu",
    /// 9: "to_backlayer",
    /// 10: "backlayer_idle"
    uint32_t state;
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
    TextureRenderingInfo unknown15; // probably xbox username scroll

    uint32_t unknown16a;
    uint32_t padding_probably1;
    SoundMeta* cthulhu_sound;
    ParticleEmitterInfo* particle_smoke;
    ParticleEmitterInfo* particle_rubble;
    float cthulhu_disc_ring_angle;
    float cthulhu_disc_split_progress;
    float cthulhu_disc_y;
    float cthulhu_timer;
    ScreenTransition* screen_transition; // called when you leave the menu, just sets the state.screen_next and stuff

    custom_vector<custom_vector<MenuOption>> menu_tree; // always have the vector of the main menu, and then any deeper level menu, like Play or Online
    custom_vector<uint32_t> menu_index_order;           // to go back to, probably needs a better name
    ScreenControls controls;
    uint32_t selected_menu_index; // more like highlighted
    uint8_t sides_hold_down_timer;
    uint8_t sides_fast_scroll_timer;

    // uint8_t padding_probably[2];

    uint32_t unknown27; // pressed direction? 1 = left, 0 = right, no neutral, stays at the last state
    /// Allow going up from first to last option
    bool loop;

    // uint8_t padding_probably[3];

    /// 0 = main menu, 1 = play, 2 = online
    uint32_t menu_id;
    uint32_t transfer_to_menu_id;
    float menu_text_opacity;
    std::array<float, 6> spear_position;
    std::array<SpritePosition, 6> spear_dangler;
    std::array<uint32_t, 6> spear_dangle_momentum;
    std::array<uint32_t, 6> spear_dangle_angle;

    float play_scroll_descend;
    STRINGID scroll_text;
    float shake_offset_x;
    float shake_offset_y;
    /// Set to true when going from title to menu screen for the first time, makes sure the animation play once
    bool loaded_once;
    // maybe two more 32bit values? hard to tell
};

struct GraphicandAudioSettings
{
    uint32_t fullscreen_resolution_id; // depends on the GetMonitorInfo etc.
    uint32_t windowed_resolution_id;   // depends on the GetMonitorInfo etc.
    /// 100 = 1.0
    uint32_t resolution_scale;
    /// 0 = Fullscreen, 1 = Borderless Windowed, 2 = Windowed
    uint8_t display_mode;
    uint8_t unknown33;
    bool unknown34; // if true, first input just sets it to false and does nothing else
    uint8_t unknown35;
    bool unknown36; // if it's not false, it's set to false and nothing else touches it ???
    // uint8_t padding_probably6[3];
    uint32_t unknown37;
    uint32_t unknown38;
};

class ScreenOptions : public Screen // ID: 5
{
  public:
    custom_vector<custom_vector<MenuOption>> menu_tree;
    custom_vector<uint32_t> menu_index_order; // to go back to, probably needs a better name

    // yes, this is just ScreenControls but up/down are reversed and not left/right for some reason
    // also ScreenControls could include the selected_index, but in arena screen it's above and here it's below :|
    bool down;
    bool up;
    // bool unused[2]
    /// -1 = none, 0 = down, 1 = up
    int32_t direction_input;
    uint32_t hold_down_timer;
    uint32_t fast_scroll_timer;

    uint32_t selected_menu_index; // more like highlighted
    uint8_t sides_hold_down_timer;
    uint8_t sides_fast_scroll_timer;
    // uint16_t probably_padding1;
    uint32_t unknown0; // pressed direction? 1 = left, 0 = right, no neutral, stays at the last state
    /// Allow going up from first to last option
    bool loop;

    // uint8_t probably_padding2[3];

    // this is probably similar stuff that is at the beginning of ScreenMenu, transitions from and to menus
    float unknown1;
    float unknown2;
    float unknown3;
    float unknown4;
    float unknown5;
    uint32_t unknown6;
    uint32_t unknown7; // speed related?
    float unknown8;
    float unknown9;
    float unknown10;
    float unknown11;
    float unknown12;
    float unknown13;
    float unknown14;
    uint32_t unknown15;
    int32_t unknown16;
    float unknown17;
    float unknown18;

    MenuScreenPanels screen_panels;

    uint32_t unknown_state; // 0 = none, 2 = moving between inner menus, 3 = exiting menu options
    uint32_t current_menu_id;
    uint32_t transfer_to_menu_id;
    bool show_apply_button;
    // uint8_t padding_probably4[3];
    GraphicandAudioSettings graphic_and_audio;

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
    TextureRenderingInfo volume_progressbar_foreground;
    TextureRenderingInfo progressbar_foreground; // the border
    TextureRenderingInfo volume_progressbar_position_indicator;
    TextureRenderingInfo sectionheader_background; // behind 'GRAPHICS' and 'AUDIO' the black bars

    TextureRenderingInfo unknown44; // could be something else, or just not used in PC version or something
    /// In "Gameplay" menu
    TextureRenderingInfo pet_icons;
    /// For the code in the sync menu
    TextureRenderingInfo bottom_scroll;
    TextureRenderingInfo bottom_left_scrollhandle;
    TextureRenderingInfo bottom_right_scrollhandle;
    float topleft_woodpanel_esc_slidein;
    float text_fadein;
    float vertical_scroll_effect;
    uint8_t unknown49; // small random timer
    bool options_visiable;
    /// Shows the red background behind the option, the scarab on the left and left/right arrows
    bool show_highlight;
    // uint8_t padding_probably8[5];
    custom_vector<size_t> unknown50; // holds one 8 byte value, related to choosen option menu
    uint8_t unknown51;               // probably bool
    // padding_probably10[7];
    custom_vector<STRINGID> tooltip_text;
    /// Used for sync progress
    bool disable_controls;
    // uint8_t padding_probably9[3];
    /// 0 - none, 1 - waiting for the code, 2 - code acquired, 3 - sync in progress, 4 - sync completed
    uint32_t sync_progress_state;
    uint32_t unknown54; // some timer
    // uint32_t padding_probably10;
    float* credits_progression;
};

class ScreenCodeInput : public Screen // ID: 8
{
  public:
    MenuScreenPanels screen_panels;
    /// needs to be set before opening the screen to show the correct text at the bottom
    bool allow_random;
    // uint8_t probably_padding1[3];
    uint32_t selected_button_index;
    bool pressed_select;
    // uint8_t probably_padding2;
    char16_t code_chars[9]; // utf16 chars

    /// Current input length (0-8). You probably shouldn't write to this, except to set it to 0.
    uint8_t code_length;
    // uint8_t probably_padding3[3];

    float topleft_woodpanel_esc_slidein;
    STRINGID scroll_text_id;
    STRINGID start_text_id;
    TextureRenderingInfo main_woodpanel_left_border;
    TextureRenderingInfo main_woodpanel_center;
    TextureRenderingInfo main_woodpanel_right_border;
    TextureRenderingInfo top_scroll;
    TextureRenderingInfo letter_cutouts;
    TextureRenderingInfo hand_pointer;
    TextureRenderingInfo key_background;
    TextureRenderingInfo topleft_woodpanel_esc;
    TextureRenderingInfo start_sidepanel;
    float start_sidepanel_slidein;

    /// Set the seed entered in the seed dialog. Call without arguments to clear entered seed. Optionally enter a length to set partial seed.
    void set_seed(std::optional<uint32_t> seed, std::optional<uint8_t> length);
    /// Get the seed currently entered in the seed dialog or nil if nothing is entered. Will also return incomplete seeds, check seed_length to verify it's ready.
    std::optional<uint32_t> get_seed();

    virtual void unknown() = 0; // set seed? sets the game variables in state, for ScreenEnterOnlineCode it just sets the unknown10
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

struct InputsDevice
{
    uint16_t buttons;
    uint16_t buttons_previous;
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
    float main_background_zoom_progress;
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
    float blurred_border_zoom_progress;
    float blurred_border_zoom_target;

    MenuScreenPanels screen_panels;
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
    TextureRenderingInfo unknown56;

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
    float topleft_woodpanel_esc_slidein;
    float start_panel_slidein;
    float action_buttons_keycap_size;
    bool screen_loading; // locks the inputs?
    bool unknown64b;     // sets the state.ingame
    bool seeded_run;
    bool daily_challenge;
    uint32_t next_screen_to_load;
    bool not_ready_to_start_yet;
    uint8_t available_mine_entrances; // The rest are boarded off
    uint8_t amount_of_mine_entrances_activated;
    // uint8_t probably_padding1;
    size_t* reset_func; /* unsure*/
    uint32_t unknown69; // it's some states, or flags
    TextureRenderingInfo screen_blackout;
    float blackout_transparency;
    bool start_pressed;
    bool transition_to_game_started;
    bool disable_buttons; /* unsure */ // hides the buttons on the entrances and disables control (without setting the bool below)
    bool disable_controls;
    uint32_t unknown72;

    std::array<FlyingThing, 6> flying_things;
    uint16_t flying_thing_countdown; // when 0, flying things appear
    int16_t unknown79;               // negative, counts up to 0

    // uint32_t probably_padding2;

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
    std::array<SoundMeta*, 4> torch_sound;
    std::array<InputsDevice, 12> inputs;
    std::array<uint8_t, MAX_PLAYERS> buttons;
};

class ScreenTeamSelect : public Screen // ID: 10
{
  public:
    TextureRenderingInfo player_portrait;
    TextureRenderingInfo scroll_bottom_left;
    TextureRenderingInfo scrollend_bottom_left;
    TextureRenderingInfo four_ropes;
    TextureRenderingInfo gems_above_the_ropes;
    TextureRenderingInfo four_characters;
    TextureRenderingInfo left_arrow;
    TextureRenderingInfo right_arrow;
    TextureRenderingInfo start_panel;
    TextureRenderingInfo go_back_wooden_panel;
    float start_panel_slide;
    float go_back_wooden_panel_slide;
    float pulsating_arrows_timer;
    uint8_t selected_player;
    uint8_t buttons;
    bool ready;
};

class ScreenCamp : public Screen // ID: 11 GameManager, same as ScreenLevel
{
  public:
    uint8_t buttons;
};

class ScreenStateCamp : public Screen // ID: 11 StateMemory
{
  public:
    /// Delay after player death to reset camp
    uint8_t time_till_reset;
};

class ScreenStateLevel : public Screen // ID: 12 StateMemory
{
  public:
    uint8_t buttons;
    /// Delay after player death to open the death screen
    uint8_t time_till_death_screen;
};

class ScreenLevel : public Screen // ID: 12 GameManager
{
  public:
    uint8_t buttons;
    // garbage here ?
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
    TextureRenderingInfo scroll;
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
    char16_t mama_tunnel_text_buffer[256]; // UTF16 string
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

    char16_t string_buffer[130]; // UTF16 string
    uint32_t stats_scroll_unfurl_sequence;
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

    char16_t string_buffer_this_level_time[10];        // UTF16 string
    char16_t string_buffer_total_time[10];             // UTF16 string
    char16_t string_buffer_this_level_milliseconds[5]; // UTF16 string
    char16_t string_buffer_total_milliseconds[5];      // UTF16 string
    char16_t string_buffer_this_level_money[10];       // UTF16 string
    char16_t string_buffer_total_money[10];            // UTF16 string

    Color this_level_money_color;

    std::array<uint8_t, MAX_PLAYERS> buttons;
};

/// The POST render call will only be visible in the polaroid area on the left of the book. The book is apparently drawn on top of that.
class ScreenDeath : public Screen // ID: 14
{
  public:
    size_t unknown;
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
};

class ScreenCredits : public Screen // ID: 17
{
  public:
    float* credits_progression;
    SoundMeta* bg_music_info;
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
    size_t unknown; // not sure if it's something actually there, or it's some allocator aliment to memory page or something, seam to happen a lot with the screens
};

/// The recap book is drawn on top of the POST render event
class ScreenRecap : public Screen // ID: 20
{
  public:
    uint32_t unknown1;
    uint32_t unknown2;
};

class ScreenOnlineLoading : public Screen // ID: 28
{
  public:
    TextureRenderingInfo ouroboros;
    float ouroboros_angle;
};

struct OnlineLobbyScreenPlayer
{
    /// 16 = PC, 17 = Discord, 18 = Steam, 19 = Xbox, 32 = switch, 48 = PS, 49 = PS again?
    uint8_t platform_icon; // weird numbers, anything else results in the standard PC icon, maybe it's some actual id
    /// 0 - Ana Spelunky, 1 - Margaret Tunnel, 2 - Colin Northward, 3 - Roffy D. Sloth.. and so on. Same order as in ENT_TYPE
    uint8_t character;
    bool ready;
    bool searching;
};

class ScreenEnterOnlineCode : public ScreenCodeInput // no ID, very special screen
{
  public:
    int32_t unknown10; // -1
    TextureRenderingInfo enter_code_your_code_scroll;
    TextureRenderingInfo enter_code_your_code_scroll_left_handle;
    TextureRenderingInfo enter_code_your_code_scroll_right_handle;
    uint32_t unknown11;
    TextureRenderingInfo unknown12;
    uint32_t unknown13;
    TextureRenderingInfo unknown14;
    TextureRenderingInfo unknown15;
};

class ScreenOnlineLobby : public Screen // ID: 29
{
  public:
    MenuScreenPanels screen_panels;
    std::array<OnlineLobbyScreenPlayer, 4> players;
    TextureRenderingInfo background_image;
    TextureRenderingInfo unknown35;
    TextureRenderingInfo unknown36;
    TextureRenderingInfo unknown37;
    float unknown38;
    TextureRenderingInfo topleft_woodpanel_esc;
    float topleft_woodpanel_esc_slidein;
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

    // can't put ScreenEnterOnlineCode here as it's abstract class
    // no idea how the game code apparently allows this, unless this is some compiler optimization bullshit

    size_t screen_code_input;
};

struct MenuInsert
{
    float x;
    float y;
    float unknown1; // text_disappearance_speed?
    float text_spacing;
    float unknown5;
    uint32_t unknown6;
    float unknown7;
    float unknown8;
    uint32_t unknown9;
    float unknown10;
    std::vector<size_t*> unknown11; // menu options, probably just a bunch of floats, surprisingly it's not TextRenderingInfo
    size_t* unknown12;
    size_t* unknown13; // function
    uint32_t selected_menu_index;
    bool loop;
    bool unknown16;
    bool disable_controls;
};

struct PauseUI
{
    float menu_slidein_progress;
    TextureRenderingInfo blackout_background;
    TextureRenderingInfo woodpanel_left;
    TextureRenderingInfo woodpanel_middle;
    TextureRenderingInfo woodpanel_right;
    TextureRenderingInfo woodpanel_top;
    TextureRenderingInfo scroll;

    // uint32_t probably_padding1;
    MenuInsert* menu;
    /// Prompt background
    TextureRenderingInfo confirmation_panel;
    MultiLineTextRendering* prompt_question;
    MenuInsert* prompt_menu;
    bool unknown8;
    // uint8_t probably_padding2[3];
    /// It's set wh game displays the prompt
    uint32_t selected_option;
    bool prompt_visible;
    std::array<uint8_t, MAX_PLAYERS> buttons_actions;  // per player, so no default menu input
    std::array<uint8_t, MAX_PLAYERS> buttons_movement; // per player, so no default menu input
    int8_t unknown11;
    // uint16_t probably_padding3;
    /// 0 - Invisible, 1 - Sliding down, 2 - Visible, 3 - Sliding up
    uint32_t visibility;
};

using JOURNAL_PAGE_TYPE = JournalPageType;

class JournalPage
{
  public:
    TextureRenderingInfo background;
    uint32_t page_number;
    uint32_t unknown2; // probably padding

    template <typename T>
    T* as()
    {
        return static_cast<T*>(this);
    }

    /// background.x < 0
    bool is_right_side_page() const
    {
        return (this->background.x < 0);
    }
    void set_page_background_side(bool right);
    JOURNAL_PAGE_TYPE get_type();

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
};

class JournalPageJournalMenu : public JournalPage
{
  public:
    MenuInsert menu;
    TextRenderingInfo* journal_text_info;
    TextureRenderingInfo completion_badge;
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
};

class JournalPagePlaces : public JournalPageDiscoverable
{
  public:
    TextureRenderingInfo main_image;
};

class JournalPagePeople : public JournalPageDiscoverable
{
  public:
    TextureRenderingInfo character_background;
    TextureRenderingInfo character_icon;
    TextureRenderingInfo character_drawing;
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
};

class JournalPageItems : public JournalPageDiscoverable
{
  public:
    TextureRenderingInfo item_icon;
    TextureRenderingInfo item_background;
};

class JournalPageTraps : public JournalPageDiscoverable
{
  public:
    TextureRenderingInfo trap_icon;
    TextureRenderingInfo trap_background;
};

class JournalPageStory : public JournalPage
{
  public:
    static JournalPageStory* construct(bool right_side, uint32_t page_number);
};

class JournalPageFeats : public JournalPage
{
  public:
    TextRenderingInfo* chapter_title_text_info;
    TextureRenderingInfo feat_icons;
};

class JournalPageDeathCause : public JournalPage
{
  public:
    TextRenderingInfo* death_cause_text_info;
};

class JournalPageDeathMenu : public JournalPage
{
  public:
    MenuInsert menu;
    TextRenderingInfo* game_over_text_info;
    TextRenderingInfo* level_text_info;
    TextRenderingInfo* level_value_text_info;
    TextRenderingInfo* money_text_info;
    TextRenderingInfo* money_value_text_info;
    TextRenderingInfo* time_text_info;
    TextRenderingInfo* time_value_text_info;
};

class JournalPageRecap : public JournalPage
{
  public:
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
};

using JOURNALUI_PAGE_SHOWN = uint8_t; // NoAlias

struct JournalUI
{
    uint32_t state;
    JOURNALUI_PAGE_SHOWN chapter_shown;

    // uint8_t padding_probably1[3];

    /// Stores pages loaded into memory. It's not cleared after the journal is closed or when you go back to the main (menu) page.
    /// Use `:get_type()` to check page type and cast it correctly (see ON.[RENDER_PRE_JOURNAL_PAGE](#ON-RENDER_PRE_JOURNAL_PAGE))
    custom_vector<JournalPage*> pages;
    custom_vector<JournalPage*> pages_tmp; // pages are constructed in the show_journal function and put here, later transferred to the pages vector
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

    uint8_t fade_timer;
    float unknown26;

    uint32_t page_timer;

    uint32_t unknown28;
    size_t unknown29;
};

Screen* get_screen_ptr(uint32_t screen_id);
void force_journal(uint32_t chapter, uint32_t entry);
void toggle_journal();
void show_journal(JOURNALUI_PAGE_SHOWN chapter, uint32_t page);
