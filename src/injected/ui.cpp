#include "ui.hpp"
#include "ui_util.hpp"

#include <ShlObj.h>
#include <Shlwapi.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>
#include <shellapi.h>
#include <windows.h>

#include <algorithm>
#include <array>
#include <charconv>
#include <chrono>
#include <codecvt>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <locale>
#include <map>
#include <random>
#include <string>

#pragma warning(push, 0)
#include <toml.hpp>
#pragma warning(pop)
#include <fmt/chrono.h>
#include <fmt/core.h>

#include "olfont.h"

#include "bucket.hpp"
#include "console.hpp"
#include "custom_types.hpp"
#include "entities_chars.hpp"
#include "entities_floors.hpp"
#include "entities_items.hpp"
#include "entities_logical.hpp"
#include "entities_mounts.hpp"
#include "file_api.hpp"
#include "flags.hpp"
#include "game_api.hpp"
#include "game_manager.hpp"
#include "illumination.hpp"
#include "items.hpp"
#include "level_api.hpp"
#include "liquid_engine.hpp"
#include "logger.h"
#include "math.hpp"
#include "savedata.hpp"
#include "screen.hpp"
#include "script.hpp"
#include "settings_api.hpp"
#include "socket.hpp"
#include "sound_manager.hpp" // TODO: remove from here?
#include "state.hpp"
#include "state_structs.hpp"
#include "steam_api.hpp"
#include "version.hpp"
#include "window_api.hpp"

#include "decode_audio_file.hpp"

#include "render_api.hpp"
#include "script/usertypes/vanilla_render_lua.hpp"

#pragma warning(disable : 4366)

using cvt_type = std::codecvt_utf8<wchar_t>;
std::wstring_convert<cvt_type, wchar_t> cvt;

template <class T>
concept Script = std::is_same_v<T, SpelunkyConsole> || std::is_same_v<T, SpelunkyScript>;

std::unique_ptr<SoundManager> g_SoundManager;

std::unique_ptr<SpelunkyConsole> g_Console;
std::deque<ScriptMessage> g_ConsoleMessages;

std::map<std::string, std::unique_ptr<SpelunkyScript>> g_scripts;
std::map<std::string, std::unique_ptr<SpelunkyScript>> g_ui_scripts;
std::vector<std::filesystem::path> g_script_files;
std::vector<std::string> g_script_autorun;
ScriptImage g_cursor;

std::map<std::string, int64_t> default_keys{
    {"enter", VK_RETURN},
    {"escape", VK_ESCAPE},
    {"move_left", VK_LEFT},
    {"move_up", VK_UP},
    {"move_right", VK_RIGHT},
    {"move_down", VK_DOWN},
    {"move_pageup", VK_PRIOR},
    {"move_pagedown", VK_NEXT},
    {"toggle_mouse", OL_KEY_CTRL | 'M'},
    {"toggle_godmode", OL_KEY_CTRL | 'G'},
    {"toggle_noclip", OL_KEY_CTRL | 'F'},
    {"toggle_flymode", OL_KEY_CTRL | OL_KEY_SHIFT | 'F'},
    {"toggle_snap", OL_KEY_CTRL | 'S'},
    {"toggle_pause", OL_KEY_CTRL | VK_SPACE},
    {"toggle_disable_pause", OL_KEY_CTRL | OL_KEY_SHIFT | 'P'},
    {"toggle_grid", OL_KEY_CTRL | OL_KEY_SHIFT | 'G'},
    {"toggle_hitboxes", OL_KEY_CTRL | OL_KEY_SHIFT | 'H'},
    {"toggle_entity_info", OL_KEY_CTRL | OL_KEY_SHIFT | 'E'},
    {"toggle_entity_tooltip", OL_KEY_CTRL | OL_KEY_SHIFT | 'Y'},
    {"toggle_hud", OL_KEY_CTRL | 'H'},
    {"toggle_lights", OL_KEY_CTRL | 'L'},
    {"toggle_ghost", OL_KEY_CTRL | 'O'},
    {"toggle_void", OL_KEY_ALT | 'V'},
    {"import_void", OL_KEY_CTRL | 'V'},
    {"export_void", OL_KEY_CTRL | 'E'},
    {"toggle_speedhack_auto", OL_KEY_CTRL | OL_KEY_SHIFT | 'T'},
    {"frame_advance", VK_SPACE},
    {"frame_advance_alt", OL_KEY_SHIFT | VK_SPACE},
    {"tool_entity", VK_F1},
    {"tool_door", VK_F2},
    {"tool_camera", VK_F3},
    {"tool_entity_properties", VK_F4},
    {"tool_game_properties", VK_F5},
    {"tool_options", VK_F9},
    {"tool_debug", OL_KEY_CTRL | OL_KEY_SHIFT | VK_F12},
    {"tool_metrics", OL_KEY_CTRL | OL_KEY_SHIFT | 'I'},
    {"tool_style", OL_KEY_CTRL | OL_KEY_SHIFT | 'U'},
    {"tool_keys", OL_KEY_CTRL | OL_KEY_SHIFT | 'K'},
    {"tool_script", VK_F8},
    {"tool_save", VK_F6},
    {"tool_finder", VK_F7},
    {"reset_windows", OL_KEY_CTRL | OL_KEY_SHIFT | 'R'},
    {"detach_tab", OL_KEY_CTRL | OL_KEY_SHIFT | 'D'},
    {"save_settings", OL_KEY_CTRL | OL_KEY_SHIFT | 'S'},
    {"load_settings", OL_KEY_CTRL | OL_KEY_SHIFT | 'L'},
    {"spawn_entity", OL_KEY_CTRL | VK_RETURN},
    {"spawn_kit_1", OL_KEY_SHIFT | '1'},
    {"spawn_kit_2", OL_KEY_SHIFT | '2'},
    {"spawn_kit_3", OL_KEY_SHIFT | '3'},
    {"spawn_kit_4", OL_KEY_SHIFT | '4'},
    {"spawn_kit_5", OL_KEY_SHIFT | '5'},
    {"spawn_kit_6", OL_KEY_SHIFT | '6'},
    {"spawn_kit_7", OL_KEY_SHIFT | '7'},
    {"spawn_kit_8", OL_KEY_SHIFT | '8'},
    {"spawn_kit_9", OL_KEY_SHIFT | '9'},
    {"hotbar_1", '1'},
    {"hotbar_2", '2'},
    {"hotbar_3", '3'},
    {"hotbar_4", '4'},
    {"hotbar_5", '5'},
    {"hotbar_6", '6'},
    {"hotbar_7", '7'},
    {"hotbar_8", '8'},
    {"hotbar_9", '9'},
    {"hotbar_0", '0'},
    {"load_state_1", OL_KEY_SHIFT | VK_F1},
    {"load_state_2", OL_KEY_SHIFT | VK_F2},
    {"load_state_3", OL_KEY_SHIFT | VK_F3},
    {"load_state_4", OL_KEY_SHIFT | VK_F4},
    {"save_state_1", OL_KEY_SHIFT | VK_F5},
    {"save_state_2", OL_KEY_SHIFT | VK_F6},
    {"save_state_3", OL_KEY_SHIFT | VK_F7},
    {"save_state_4", OL_KEY_SHIFT | VK_F8},
    {"toggle_hotbar", OL_KEY_CTRL | OL_KEY_SHIFT | 'B'},
    {"spawn_layer_door", OL_KEY_SHIFT | VK_RETURN},
    {"spawn_warp_door", OL_KEY_CTRL | OL_KEY_SHIFT | VK_RETURN},
    {"destroy_grabbed", VK_DELETE},
    {"destroy_selected", OL_KEY_ALT | VK_DELETE},
    {"warp", OL_KEY_CTRL | 'W'},
    {"warp_next_level_a", OL_KEY_CTRL | 'A'},
    {"warp_next_level_b", OL_KEY_CTRL | 'B'},
    {"hide_ui", VK_F11},
    {"switch_ui", VK_F12},
    {"zoom_in", OL_KEY_CTRL | VK_OEM_COMMA},
    {"zoom_out", OL_KEY_CTRL | VK_OEM_PERIOD},
    {"zoom_reset", OL_KEY_CTRL | '1'},
    {"zoom_2x", OL_KEY_CTRL | '2'},
    {"zoom_3x", OL_KEY_CTRL | '3'},
    {"zoom_4x", OL_KEY_CTRL | '4'},
    {"zoom_5x", OL_KEY_CTRL | '5'},
    {"zoom_auto", OL_KEY_CTRL | '0'},
    {"camera_left", OL_KEY_SHIFT | 'J'},
    {"camera_up", OL_KEY_SHIFT | 'I'},
    {"camera_right", OL_KEY_SHIFT | 'L'},
    {"camera_down", OL_KEY_SHIFT | 'K'},
    {"camera_reset", OL_KEY_SHIFT | 'U'},
    {"mouse_spawn", OL_BUTTON_MOUSE | 0x01},
    {"mouse_spawn_throw", OL_BUTTON_MOUSE | 0x01},
    {"mouse_spawn_over", OL_BUTTON_MOUSE | OL_KEY_CTRL | 0x01},
    {"mouse_draw", OL_BUTTON_MOUSE | OL_KEY_ALT | 0x01},
    {"mouse_erase", OL_BUTTON_MOUSE | OL_KEY_ALT | 0x05},
    {"mouse_decorate", OL_BUTTON_MOUSE | OL_KEY_ALT | 0x04},
    {"mouse_teleport", OL_BUTTON_MOUSE | 0x02},
    {"mouse_teleport_throw", OL_BUTTON_MOUSE | 0x02},
    {"mouse_grab", OL_BUTTON_MOUSE | 0x03},
    {"mouse_grab_unsafe", OL_BUTTON_MOUSE | OL_KEY_SHIFT | 0x03},
    {"mouse_grab_throw", OL_BUTTON_MOUSE | OL_KEY_CTRL | 0x03},
    {"mouse_grab_throw_unsafe", OL_BUTTON_MOUSE | OL_KEY_CTRL | OL_KEY_SHIFT | 0x03},
    {"mouse_select", OL_BUTTON_MOUSE | OL_KEY_ALT | 0x03},
    {"mouse_select_unsafe", OL_BUTTON_MOUSE | OL_KEY_ALT | OL_KEY_SHIFT | 0x03},
    {"mouse_camera_drag", OL_BUTTON_MOUSE | 0x04},
    {"mouse_blast", 0x0},
    {"mouse_boom", 0x0},
    {"mouse_zap", 0x0},
    {"mouse_big_boom", 0x0},
    {"mouse_nuke", 0x0},
    {"mouse_clone", 0x0},
    {"mouse_destroy", OL_BUTTON_MOUSE | 0x05},
    {"mouse_destroy_unsafe", OL_BUTTON_MOUSE | OL_KEY_SHIFT | 0x05},
    {"mouse_zoom_out", OL_BUTTON_MOUSE | OL_KEY_CTRL | OL_WHEEL_DOWN},
    {"mouse_zoom_in", OL_BUTTON_MOUSE | OL_KEY_CTRL | OL_WHEEL_UP},
    {"reload_enabled_scripts", OL_KEY_CTRL | VK_F5}, // ctrl + f5 same as playlunky
    {"console", VK_OEM_3},                           // ~ for US
    {"console_alt", VK_OEM_5},                       // \ for US
    {"close_console", VK_ESCAPE},                    // alternative to close it
    {"change_layer", OL_KEY_SHIFT | VK_TAB},
    {"quick_start", 'Q'},
    {"quick_restart", OL_KEY_CTRL | 'Q'},
    {"quick_restart_seed", OL_KEY_ALT | 'Q'},
    {"quick_camp", OL_KEY_CTRL | 'C'},
    {"peek_layer", 0x8}, // backspace
    {"speedhack_increase", OL_KEY_CTRL | OL_KEY_SHIFT | VK_PRIOR},
    {"speedhack_decrease", OL_KEY_CTRL | OL_KEY_SHIFT | VK_NEXT},
    {"speedhack_10pct", OL_KEY_CTRL | OL_KEY_SHIFT | '1'},
    {"speedhack_20pct", OL_KEY_CTRL | OL_KEY_SHIFT | '2'},
    {"speedhack_30pct", OL_KEY_CTRL | OL_KEY_SHIFT | '3'},
    {"speedhack_40pct", OL_KEY_CTRL | OL_KEY_SHIFT | '4'},
    {"speedhack_50pct", OL_KEY_CTRL | OL_KEY_SHIFT | '5'},
    {"speedhack_60pct", OL_KEY_CTRL | OL_KEY_SHIFT | '6'},
    {"speedhack_70pct", OL_KEY_CTRL | OL_KEY_SHIFT | '7'},
    {"speedhack_80pct", OL_KEY_CTRL | OL_KEY_SHIFT | '8'},
    {"speedhack_90pct", OL_KEY_CTRL | OL_KEY_SHIFT | '9'},
    {"speedhack_normal", OL_KEY_CTRL | OL_KEY_SHIFT | '0'},
    {"speedhack_turbo", VK_PRIOR},
    {"speedhack_slow", VK_NEXT},
    {"toggle_uncapped_fps", OL_KEY_CTRL | OL_KEY_SHIFT | 'U'},
    {"respawn", OL_KEY_CTRL | 'R'},
    {"clear_messages", OL_KEY_CTRL | VK_BACK},
    //{ "", 0x },
};

auto keys = default_keys;

struct Window
{
    std::string name;
    bool detached;
    bool open;
    bool popup;
};
std::map<std::string, Window*> windows;

struct EntityWindow
{
    std::string name;
    int uid;
    bool open;
};
std::map<int, EntityWindow*> entity_windows;

struct Callback
{
    uint32_t frame;
    std::function<void()> func;
};
std::vector<Callback> callbacks;

static ImFont *font, *bigfont, *hugefont;

float g_x = 0, g_y = 0, g_vx = 0, g_vy = 0, g_dx = 0, g_dy = 0, g_zoom = 13.5f, g_hue = 0.63f, g_sat = 0.66f, g_val = 0.66f, g_camera_speed = 1.0f, g_camera_inertia = -FLT_MAX;
ImVec2 startpos;
int g_held_id = -1, g_last_id = -1, g_over_id = -1, g_current_item = 0, g_filtered_count = 0, g_last_frame = 0,
    g_last_gun = 0, g_last_time = -1, g_level_time = -1, g_total_time = -1,
    g_force_width = 0, g_force_height = 0, g_pause_at = -1, g_hitbox_mask = 0x80BF, g_last_type = -1, g_force_level_width = 4, g_force_level_height = 4;
unsigned int g_level_width = 0, grid_x = 0, grid_y = 0;
uint8_t g_level = 1, g_world = 1, g_to = 0;
uint32_t g_held_flags = 0, g_dark_mode = 0, g_last_kit_spawn = 0;
std::vector<EntityItem> g_items;
std::vector<int> g_filtered_items;
struct Kit
{
    std::string items;
    bool automatic;
};
std::vector<std::string> saved_entities;
std::vector<uint32_t> saved_hotbar;
std::vector<Kit*> kits;
std::vector<Player*> g_players;
std::vector<uint32_t> g_selected_ids;
bool set_focus_entity = false, set_focus_world = false, set_focus_finder = false, set_focus_uid = false, scroll_to_entity = false, scroll_top = false, click_teleport = false,
     throw_held = false, show_app_metrics = false, lock_entity = false, lock_player = false,
     freeze_last = false, freeze_level = false, freeze_total = false, hide_ui = false,
     enable_noclip = false, enable_camp_camera = true, enable_camera_bounds = true, freeze_quest_yang = false, freeze_quest_sisters = false, freeze_quest_horsing = false, freeze_quest_sparrow = false, freeze_quest_tusk = false, freeze_quest_beg = false, run_finder = false, in_menu = false, zooming = false, g_inv = false, edit_last_id = false, edit_achievements = false, peek_layer = false, death_disable = false;
std::optional<int8_t> quest_yang_state, quest_sisters_state, quest_horsing_state, quest_sparrow_state, quest_tusk_state, quest_beg_state;
Entity* g_entity = 0;
Entity* g_held_entity = 0;
StateMemory* g_state = 0;
SaveData* g_save = 0;
GameManager* g_game_manager = 0;
Bucket* g_bucket = 0;
std::map<int, std::string> entity_names;
std::map<int, std::string> entity_full_names;
std::string active_tab = "", activate_tab = "", detach_tab = "", focused_tool = "", g_load_void = "";
std::vector<std::string> tab_order = {"tool_entity", "tool_door", "tool_camera", "tool_entity_properties", "tool_game_properties", "tool_save", "tool_finder", "tool_script", "tool_texture", "tool_options", "tool_style", "tool_keys", "tool_debug"};
std::vector<std::string> tab_order_main = {"tool_entity", "tool_door", "tool_camera", "tool_entity_properties", "tool_game_properties", "tool_save", "tool_finder", "tool_script", "tool_options"};
std::vector<std::string> tab_order_extra = {"tool_texture"};
std::vector<std::string> tabs_open = {"tool_entity", "tool_door", "tool_camera", "tool_entity_properties", "tool_game_properties", "tool_finder"};
std::vector<std::string> tabs_detached = {};

std::map<uint32_t, uint32_t> hotbar = {};

std::string text;
std::string g_change_key = "";

const char* inifile = "imgui.ini";
const std::string cfgfile = "overlunky.ini";
std::string scriptpath = "Overlunky/Scripts";
const std::string version_check_url = "https://api.github.com/repos/spelunky-fyi/overlunky/git/ref/tags/whip";

std::string fontfile = "";
std::vector<float> fontsize = {14.0f, 32.0f, 72.0f};

[[maybe_unused]] const char s8_zero = 0, s8_one = 1, s8_min = -128, s8_max = 127;
[[maybe_unused]] const ImU8 u8_zero = 0, u8_one = 1, u8_min = 0, u8_max = 255, u8_four = 4, u8_seven = 7, u8_seventeen = 17, u8_draw_depth_max = 52, u8_shader_max = 36;
[[maybe_unused]] const short s16_zero = 0, s16_one = 1, s16_min = -32768, s16_max = 32767;
[[maybe_unused]] const ImU16 u16_zero = 0, u16_one = 1, u16_min = 0, u16_max = 65535;
[[maybe_unused]] const ImS32 s32_zero = 0, s32_one = 1, s32_min = INT_MIN / 2, s32_max = INT_MAX / 2, s32_hi_a = INT_MAX / 2 - 100, s32_hi_b = INT_MAX / 2;
[[maybe_unused]] const ImU32 u32_zero = 0, u32_one = 1, u32_min = 0, u32_max = UINT_MAX / 2, u32_hi_a = UINT_MAX / 2 - 100, u32_hi_b = UINT_MAX / 2;
[[maybe_unused]] const ImS64 s64_zero = 0, s64_one = 1, s64_min = LLONG_MIN / 2, s64_max = LLONG_MAX / 2, s64_hi_a = LLONG_MAX / 2 - 100, s64_hi_b = LLONG_MAX / 2;
[[maybe_unused]] const ImU64 u64_zero = 0, u64_one = 1, u64_thousand = 1000, u64_charmin = 194, u64_charmax = 216, u64_min = 0, u64_max = ULLONG_MAX / 2,
                             u64_hi_a = ULLONG_MAX / 2 - 100, u64_hi_b = ULLONG_MAX / 2;
[[maybe_unused]] const float f32_zero = 0.f, f32_one = 1.f, f32_lo_a = -10000000000.0f, f32_hi_a = +10000000000.0f;
[[maybe_unused]] const double f64_zero = 0., f64_one = 1., f64_lo_a = -1000000000000000.0, f64_hi_a = +1000000000000000.0;

int safe_entity_mask = 0x18f;
inline constexpr unsigned int unsafe_entity_mask = 0;

inline constexpr int default_entity_mask = 0x18f;
inline constexpr int default_hitbox_mask = 0x80bf;

std::chrono::time_point<std::chrono::system_clock> last_focus_time;
bool last_focus;

std::map<std::string, bool> options = {
    {"mouse_control", true},
    {"keyboard_control", true},
    {"god_mode", false},
    {"god_mode_companions", false},
    {"noclip", false},
    {"fly_mode", false},
    {"speedhack", false},
    {"skip_fades", false},
    {"camera_hack", false},
    {"snap_to_grid", false},
    {"spawn_floor_decorated", true},
    {"disable_pause", false},
    {"draw_grid", false},
    {"draw_hitboxes", false},
    {"draw_entity_info", false},
    {"draw_entity_tooltip", false},
    {"enable_unsafe_scripts", false},
    {"warp_increments_level_count", true},
    {"warp_transition", true},
    {"lights", false},
    {"disable_achievements", true},
    {"disable_savegame", true},
    {"disable_ghost_timer", false},
    {"draw_hud", true},
    {"draw_hotbar", true},
    {"draw_script_messages", true},
    {"fade_script_messages", true},
    {"console_script_messages", false},
    {"draw_hitboxes_interpolated", true},
    {"show_tooltips", true},
    {"smooth_camera", true},
    {"multi_viewports", true},
    {"docking_with_shift", true},
    {"menu_ui", true},
    {"hd_cursor", true},
    {"inverted", false},
    {"borders", false},
    {"console_alt_keys", false},
    {"vsync", true},
    {"uncap_unfocused_fps", true},
    {"pause_update_camera", true},
    {"pause_last_instance", true},
    {"update_check", true},
    {"modifiers_clear_input", true},
    {"load_scripts", true},
    {"load_packs", false},
};

double g_engine_fps = 60.0, g_unfocused_fps = 33.0;
double fps_min = 0, fps_max = 600.0;
float g_speedhack_ui_multiplier = 1.0;
float g_speedhack_old_multiplier = 1.0;

enum class VERSION_CHECK
{
    HIDDEN,
    DISABLED,
    CHECKING,
    FAILED,
    OLD,
    LATEST,
    STABLE,
};
struct VersionCheck
{
    std::string message;
    float color[4];
    float fade;
};
struct VersionCheckStatus
{
    VERSION_CHECK state;
    float opacity;
    int64_t start;
    float color[4];
};
VersionCheckStatus version_check_status{VERSION_CHECK::HIDDEN, 1.0f, 0};
std::array<VersionCheck, 7> version_check_messages{
    VersionCheck{"", {0.0f, 0.0f, 0.0f, 0.0f}, 0},
    VersionCheck{"Automatic update check is disabled...", {0.5f, 0.5f, 0.5f, 0.8f}, 600.0f},
    VersionCheck{"Checking for updates on GitHub...", {0.3f, 0.6f, 1.0f, 0.9f}, 0},
    VersionCheck{"Automatic update check failed. Please retry, check GitHub or use Modlunky to update!", {0.8f, 0.0f, 0.0f, 1.0f}, 900.0f},
    VersionCheck{"This is not the latest build of Overlunky WHIP! Please run the Overlunky launcher or use Modlunky to get the latest build!", {0.8f, 0.0f, 0.0f, 1.0f}, 3600.0f},
    VersionCheck{"This is the latest build of Overlunky WHIP! Yippee!", {0.0f, 0.8f, 0.2f, 0.8f}, 900.0f},
    VersionCheck{"This is a stable build of Overlunky. Get the WHIP build for automatic updates!", {0.9f, 0.6f, 0.0f, 0.8f}, 600.0f},
};
void render_version_warning()
{
    if (version_check_status.state == VERSION_CHECK::HIDDEN)
        return;

    version_check_status.color[0] = version_check_messages[(int)version_check_status.state].color[0];
    version_check_status.color[1] = version_check_messages[(int)version_check_status.state].color[1];
    version_check_status.color[2] = version_check_messages[(int)version_check_status.state].color[2];
    version_check_status.color[3] = version_check_messages[(int)version_check_status.state].color[3];

    if (version_check_messages[(int)version_check_status.state].fade > 0)
    {
        if (version_check_status.start == 0)
            version_check_status.start = API::get_global_frame_count();

        auto duration = API::get_global_frame_count() - version_check_status.start;
        version_check_status.opacity = 1.0f - duration / version_check_messages[(int)version_check_status.state].fade;
        if (version_check_status.opacity <= 0.0f)
        {
            version_check_status.state = VERSION_CHECK::HIDDEN;
            return;
        }
        version_check_status.color[3] = version_check_status.opacity;
    }

    auto& render_api = RenderAPI::get();
    const float scale{0.0004f};
    static TextRenderingInfo tri{};
    tri.set_text(version_check_messages[(int)version_check_status.state].message, 0, 0, scale, scale, 1, 0);
    const auto [w, h] = tri.text_size();
    tri.y = -1.0f + std::abs(h) / 2.0f + std::abs(h) + 0.005f;
    render_api.draw_text(&tri, version_check_status.color);
}

void get_version_info(std::optional<std::string> res, std::optional<std::string> err)
{
    // http error
    if (!res.has_value())
    {
        version_check_status.state = VERSION_CHECK::FAILED;
        if (err.has_value())
            DEBUG("UpdateCheck: Error: {}", err.value());
        return;
    }
    std::string data = res.value();

    // some github error
    if (data.find("overlunky") == std::string::npos)
    {
        version_check_status.state = VERSION_CHECK::FAILED;
        DEBUG("UpdateCheck: {}", version_check_messages[(int)version_check_status.state].message);
        return;
    }

    std::string version = fmt::format("\"sha\": \"{}", get_version());

    // old version
    if (data.find(version) == std::string::npos)
    {
        version_check_status.state = VERSION_CHECK::OLD;
        DEBUG("UpdateCheck: {}", version_check_messages[(int)version_check_status.state].message);
        return;
    }
    // latest version
    else
    {
        version_check_status.state = VERSION_CHECK::LATEST;
        DEBUG("UpdateCheck: {}", version_check_messages[(int)version_check_status.state].message);
        return;
    }
}

void version_check(bool force = false)
{
    version_check_status = VersionCheckStatus{VERSION_CHECK::HIDDEN, 1.0f, 0};

    if (!options["update_check"] && !force)
    {
        version_check_status.state = VERSION_CHECK::DISABLED;
        DEBUG("UpdateCheck: {}", version_check_messages[(int)version_check_status.state].message);
        return;
    }

    auto version = std::string(get_version());

    // not a whip build
    if (version.find(".") != std::string::npos)
    {
        version_check_status.state = VERSION_CHECK::STABLE;
        DEBUG("UpdateCheck: {}", version_check_messages[(int)version_check_status.state].message);
        return;
    }

    version_check_status.state = VERSION_CHECK::CHECKING;
    DEBUG("UpdateCheck: {}", version_check_messages[(int)version_check_status.state].message);
    new HttpRequest(version_check_url, get_version_info);
}

void hook_savegame()
{
    static bool savegame_hooked = false;
    if (!savegame_hooked)
    {
        register_on_write_to_file([](const char* backup_file, const char* file, void* data, size_t data_size, WriteToFileOrig* original)
                                  {
                                      if (strcmp(file, "savegame.sav") == 0 and options["disable_savegame"])
                                          return;
                                      original(backup_file, file, data, data_size); });
        savegame_hooked = true;
    }
}

static inline void ltrim(std::string& s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch)
                                    { return !std::isspace(ch); }));
}

static inline void rtrim(std::string& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch)
                         { return !std::isspace(ch); })
                .base(),
            s.end());
}

static inline void trim(std::string& s)
{
    ltrim(s);
    rtrim(s);
}

ImVec4 hue_shift(ImVec4 in, float hue) // unused
{
    float U = std::cos(hue * 3.14159265f / 180);
    float W = std::sin(hue * 3.14159265f / 180);
    ImVec4 out = ImVec4(
        (.299f + .701f * U + .168f * W) * in.x + (.587f - .587f * U + .330f * W) * in.y + (.114f - .114f * U - .497f * W) * in.z,
        (.299f - .299f * U - .328f * W) * in.x + (.587f + .413f * U + .035f * W) * in.y + (.114f - .114f * U + .292f * W) * in.z,
        (.299f - .3f * U + 1.25f * W) * in.x + (.587f - .588f * U - 1.05f * W) * in.y + (.114f + .886f * U - .203f * W) * in.z,
        in.w); //((float)rand() / RAND_MAX) * 0.5 + 0.5);
    return out;
}

void set_colors()
{
    if (options["inverted"])
        ImGui::StyleColorsLight();
    else
        ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = ImGui::GetStyle().Colors;

    if (style.Alpha < 0.2f)
        style.Alpha = 0.2f;

    float col_main_sat = g_sat;
    float col_main_val = options["inverted"] ? 1.0f - g_val : g_val;
    float col_area_sat = g_sat * 0.60f;
    float col_area_val = options["inverted"] ? 1.0f - g_val * 0.55f : g_val * 0.55f;
    float col_back_sat = g_sat * 0.33f;
    float col_back_val = options["inverted"] ? 1.0f - g_val * 0.20f : g_val * 0.20f;

    ImVec4 col_text = ImColor::HSV(g_hue, options["inverted"] ? 1.0f - 15.f / 255.f : 15.f / 255.f, options["inverted"] ? 1.0f - 245.f / 255.f : 245.f / 255.f);
    ImVec4 col_main = ImColor::HSV(g_hue, col_main_sat, col_main_val);
    ImVec4 col_back = ImColor::HSV(g_hue, col_back_sat, col_back_val);
    ImVec4 col_area = ImColor::HSV(g_hue, col_area_sat, col_area_val);

    colors[ImGuiCol_Text] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(col_text.x, col_text.y, col_text.z, 0.58f);
    colors[ImGuiCol_WindowBg] = ImVec4(col_back.x, col_back.y, col_back.z, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(col_back.x, col_back.x, col_back.x, 0.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(col_back.x, col_back.y, col_back.z, 0.98f);
    colors[ImGuiCol_Border] = ImVec4(col_text.x, col_text.y, col_text.z, 0.40f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.68f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(col_back.x, col_back.y, col_back.z, 0.80f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(col_back.x, col_back.y, col_back.z, 0.80f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(col_area.x, col_area.y, col_area.z, 0.80f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(col_area.x, col_area.y, col_area.z, 0.57f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(col_main.x, col_main.y, col_main.z, 0.31f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(col_text.x, col_text.y, col_text.z, 0.80f);
    colors[ImGuiCol_SliderGrab] = ImVec4(col_main.x, col_main.y, col_main.z, 0.24f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(col_main.x, col_main.y, col_main.z, 0.44f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
    colors[ImGuiCol_ButtonActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(col_main.x, col_main.y, col_main.z, 0.50f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.30f);
    colors[ImGuiCol_HeaderActive] = ImVec4(col_main.x, col_main.y, col_main.z, 0.40f);
    colors[ImGuiCol_Separator] = ImVec4(col_main.x, col_main.y, col_main.z, 0.50f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(col_main.x, col_main.y, col_main.z, 0.20f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.43f);
    colors[ImGuiCol_Tab] = ImVec4(col_back.x, col_back.y, col_back.z, 0.86f);
    colors[ImGuiCol_TabHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(col_main.x, col_main.y, col_main.z, 0.80f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(col_area.x, col_area.y, col_area.z, 0.60f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(col_area.x, col_area.y, col_area.z, 0.80f);
    colors[ImGuiCol_DockingPreview] = ImVec4(col_area.x, col_area.y, col_area.z, 0.6f);
    colors[ImGuiCol_DockingEmptyBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(col_main.x, col_main.y, col_main.z, 0.63f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(col_main.x, col_main.y, col_main.z, 0.63f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    colors[ImGuiCol_TableHeaderBg] = ImVec4(col_back.x, col_back.x, col_back.x, 0.40f);
    colors[ImGuiCol_TableBorderStrong] = ImVec4(col_main.x, col_main.y, col_main.z, 0.40f);
    colors[ImGuiCol_TableBorderLight] = ImVec4(col_main.x, col_main.y, col_main.z, 0.40f);
    colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.30f, 0.30f, 0.30f, 0.09f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.43f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(col_main.x, col_main.y, col_main.z, 0.95f);
    colors[ImGuiCol_NavHighlight] = ImVec4(col_main.x, col_main.y, col_main.z, 0.80f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(col_main.x, col_main.y, col_main.z, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(col_back.x, col_back.y, col_back.z, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(col_back.x, col_back.y, col_back.z, 0.35f);

    style.DisplaySafeAreaPadding = {0, 0};
    style.WindowPadding = {4, 4};

    style.WindowRounding = 0;
    style.FrameRounding = 0;
    style.PopupRounding = 0;
    style.GrabRounding = 0;
    style.TabRounding = 0;
    style.ScrollbarRounding = 0;

    float bordersize = options["borders"] ? 1.0f : 0;
    style.WindowBorderSize = 0; // meh
    style.FrameBorderSize = bordersize;
    style.PopupBorderSize = bordersize;
    style.ChildBorderSize = bordersize;
    style.TabBorderSize = bordersize;
}

void load_cursor()
{
    if (g_cursor.texture == nullptr)
        create_d3d11_texture_from_memory(cursor_png, cursor_size, &g_cursor.texture, &g_cursor.width, &g_cursor.height);
}

void render_cursor()
{
    ImGuiContext& g = *GImGui;
    g.Style.MouseCursorScale = 1.5f;
    if (g_cursor.texture == nullptr || !g.IO.MouseDrawCursor || g.MouseCursor != ImGuiMouseCursor_Arrow)
        return;
    g.MouseCursor = ImGuiMouseCursor_None;
    const float scale = 0.8f;
    for (int n = 0; n < g.Viewports.Size; n++)
    {
        ImGuiViewportP* viewport = g.Viewports[n];
        const ImVec2 pos = g.IO.MousePos;
        ImDrawList* draw_list = ImGui::GetForegroundDrawList(viewport);
        ImTextureID tex_id = g_cursor.texture;
        draw_list->PushTextureID(tex_id);
        draw_list->AddImage(tex_id, pos, {pos.x + g_cursor.width * scale, pos.y + g_cursor.height * scale}, {0.0f, 0.0f}, {1.0f, 1.0f}, 0xffffffff);
        draw_list->PopTextureID();
    }
}

void load_script(std::string file, bool enable = true)
{
    std::replace(file.begin(), file.end(), '\\', '/');
    std::ifstream data(file);
    std::ostringstream buf;
    if (!data.fail())
    {
        buf << data.rdbuf();
        SpelunkyScript* script = new SpelunkyScript(buf.str(), file, g_SoundManager.get(), g_Console.get(), enable);
        g_scripts[script->get_file()] = std::unique_ptr<SpelunkyScript>{script};
        data.close();
    }
}

void load_script(std::wstring wfile, bool enable = true)
{
    std::replace(wfile.begin(), wfile.end(), '\\', '/');
    std::string file(cvt.to_bytes(wfile));
    std::ifstream data(wfile.c_str(), std::ios::in | std::ios::binary);
    std::ostringstream buf;
    if (!data.fail())
    {
        buf << data.rdbuf();
        SpelunkyScript* script = new SpelunkyScript(buf.str(), file, g_SoundManager.get(), g_Console.get(), enable);
        g_scripts[script->get_file()] = std::unique_ptr<SpelunkyScript>{script};
        data.close();
    }
}

std::string key_string(int64_t keycode)
{
    UCHAR virtualKey = keycode & 0xff;
    CHAR szName[128];
    int result = 0;
    std::string name;
    if ((keycode & 0xff) == 0)
    {
        name = "Disabled";
    }
    else if (!(keycode & OL_BUTTON_MOUSE)) // keyboard
    {
        UINT scanCode = MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC);
        switch (virtualKey)
        {
        case VK_LEFT:
        case VK_UP:
        case VK_RIGHT:
        case VK_DOWN:
        case VK_RCONTROL:
        case VK_RMENU:
        case VK_LWIN:
        case VK_RWIN:
        case VK_APPS:
        case VK_PRIOR:
        case VK_NEXT:
        case VK_END:
        case VK_HOME:
        case VK_INSERT:
        case VK_DELETE:
        case VK_DIVIDE:
        case VK_NUMLOCK:
            scanCode |= KF_EXTENDED;
            [[fallthrough]];
        default:
            result = GetKeyNameTextA(scanCode << 16, szName, 128);
        }
        if (result == 0)
        {
            name = "Unknown";
        }
        std::string keyname(szName);
        name = keyname;
    }
    else // mouse
    {
        std::stringstream buttonss;
        if (!(keycode & OL_MOUSE_WHEEL))
            buttonss << "Mouse" << (keycode & 0xff);
        else if ((keycode & 0xff) == OL_WHEEL_DOWN)
            buttonss << "WheelDown";
        else if ((keycode & 0xff) == OL_WHEEL_UP)
            buttonss << "WheelUp";
        name = buttonss.str();
    }

    if (keycode & OL_KEY_SHIFT)
    {
        name = "Shift+" + name;
    }
    if (keycode & OL_KEY_ALT)
    {
        name = "Alt+" + name;
    }
    if (keycode & OL_KEY_CTRL)
    {
        name = "Ctrl+" + name;
    }
    return name;
}

bool SliderByte(const char* label, char* value, char min = 0, char max = 0, const char* format = "%lld")
{
    return ImGui::SliderScalar(label, ImGuiDataType_U8, value, &min, &max, format);
}

void refresh_script_files()
{
    g_script_files.clear();
    if (options["load_scripts"] && std::filesystem::exists(scriptpath) && std::filesystem::is_directory(scriptpath))
    {
        for (const auto& file : std::filesystem::directory_iterator(scriptpath))
        {
            if (file.path().extension().wstring() == L".lua")
            {
                g_script_files.push_back(file.path());
            }
        }
    }
    else if (!options["load_scripts"] && std::filesystem::exists(scriptpath) && std::filesystem::is_directory(scriptpath))
    {
        std::vector<std::string> unload_scripts;
        for (const auto& script : g_scripts)
        {
            if (!script.second->is_enabled() && std::filesystem::equivalent(std::filesystem::path(script.second->get_path()), std::filesystem::path(scriptpath)))
            {
                unload_scripts.push_back(script.second->get_file());
            }
        }
        for (auto& id : unload_scripts)
        {
            auto it = g_scripts.find(id);
            if (it != g_scripts.end())
                g_scripts.erase(id);
        }
    }

    if (options["load_packs"] && std::filesystem::exists("Mods/Packs") && std::filesystem::is_directory("Mods/Packs"))
    {
        for (const auto& file : std::filesystem::recursive_directory_iterator("Mods/Packs"))
        {
            if (file.path().filename().wstring() == L"main.lua")
            {
                g_script_files.push_back(file.path());
            }
        }
    }
    else if (!options["load_packs"] && std::filesystem::exists("Mods/Packs") && std::filesystem::is_directory("Mods/Packs"))
    {
        std::vector<std::string> unload_scripts;
        for (const auto& script : g_scripts)
        {
            if (!script.second->is_enabled() && std::filesystem::equivalent(std::filesystem::path(script.second->get_path()).parent_path(), std::filesystem::path("Mods/Packs")))
            {
                unload_scripts.push_back(script.second->get_file());
            }
        }
        for (auto& id : unload_scripts)
        {
            auto it = g_scripts.find(id);
            if (it != g_scripts.end())
                g_scripts.erase(id);
        }
    }

    for (auto& file : g_script_files)
    {
        load_script(file.wstring(), false);
    }
}

void autorun_scripts()
{
    for (auto& file : g_script_autorun)
    {
        std::string script = scriptpath + "/" + file;
        if (std::filesystem::exists(script) && std::filesystem::is_regular_file(script))
        {
            load_script(script, true);
        }
    }
}

void update_frametimes()
{
    g_Console.get()->execute(fmt::format("set_frametime({})", g_engine_fps == 0 ? 0 : 1.0 / g_engine_fps), true);
    g_Console.get()->execute(fmt::format("set_frametime_unfocused({})", g_unfocused_fps == 0 ? 0 : 1.0 / g_unfocused_fps), true);
}

void save_config(std::string file)
{
    std::ofstream writeData(file);
    writeData << "# Overlunky hotkeys" << std::endl
              << "# Syntax:" << std::endl
              << "# function = keycode_in_hex" << std::endl
              << "# For modifiers, add 0x100 for Ctrl, 0x200 for Shift and 0x800 for Alt" << std::endl
              << "# For mouse buttons, add 0x400" << std::endl
              << "# For Mouse wheel, 0x11 = down, 0x12 = up" << std::endl
              << "# Set to 0x0 to disable key" << std::endl
              << "# Example: G is 0x47, so Ctrl+G is 0x147, 0x402 is Mouse2 etc" << std::endl
              << "# Get more hex keycodes from https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes" << std::endl
              << "# If you mess this file up, you can just delete it and run overlunky to get the defaults back" << std::endl
              << "[hotkeys]" << std::endl;
    for (const auto& kv : keys)
    {
        if (kv.first != "")
            writeData << std::left << std::setw(24) << kv.first << " = " << std::hex << "0x" << std::setw(8) << kv.second << "# "
                      << key_string(keys[kv.first]) << std::endl;
    }

    writeData << "\n[options] # 0 or 1 unless stated otherwise (default state "
                 "of options)\n";
    for (const auto& kv : options)
    {
        writeData << kv.first << " = " << std::dec << kv.second << std::endl;
    }

    ImGuiStyle& style = ImGui::GetStyle();
    writeData << "hue = " << std::fixed << std::setprecision(2) << g_hue << " # float, 0.0 - 1.0" << std::endl;
    writeData << "saturation = " << std::fixed << std::setprecision(2) << g_sat << " # float, 0.0 - 1.0" << std::endl;
    writeData << "lightness = " << std::fixed << std::setprecision(2) << g_val << " # float, 0.0 - 1.0" << std::endl;
    writeData << "alpha = " << std::fixed << std::setprecision(2) << style.Alpha << " # float, 0.0 - 1.0" << std::endl;
    writeData << "scale = " << std::fixed << std::setprecision(2) << ImGui::GetIO().FontGlobalScale << " # float, 0.3 - 2.0" << std::endl;
    writeData << "camera_speed = " << std::fixed << std::setprecision(2) << g_camera_speed << " # float" << std::endl;
    writeData << "pause_type = 0x" << std::hex << (uint64_t)g_bucket->pause_api->pause_type << " # 64bit flags" << std::endl;
    writeData << "pause_ignore_screen = 0x" << std::hex << (uint64_t)g_bucket->pause_api->ignore_screen << " # 64bit flags" << std::endl;
    writeData << "pause_ignore_screen_trigger = 0x" << std::hex << (uint64_t)g_bucket->pause_api->ignore_screen_trigger << " # 64bit flags" << std::endl;
    writeData << "pause_trigger = 0x" << std::hex << (uint64_t)g_bucket->pause_api->pause_trigger << " # 64bit flags" << std::endl;
    writeData << "pause_screen = 0x" << std::hex << (uint64_t)g_bucket->pause_api->pause_screen << " # 64bit flags" << std::endl;
    writeData << "unpause_trigger = 0x" << std::hex << (uint64_t)g_bucket->pause_api->unpause_trigger << " # 64bit flags" << std::endl;
    writeData << "unpause_screen = 0x" << std::hex << (uint64_t)g_bucket->pause_api->unpause_screen << " # 64bit flags" << std::endl;
    writeData << "modifiers_block = 0x" << std::hex << g_bucket->pause_api->modifiers_block << " # 32bit flags" << std::endl;

    writeData << "kits = [";
    for (unsigned int i = 0; i < kits.size(); i++)
    {
        writeData << std::endl
                  << "  \"" << kits[i]->items << "\"";
        if (i < kits.size() - 1)
            writeData << ",";
    }
    if (!kits.empty())
        writeData << std::endl;
    writeData << "]" << std::endl;

    uint32_t max = 0;
    for (auto [i, id] : hotbar)
    {
        max = i;
    }
    writeData << "hotbar = [";
    for (unsigned int i = 0; i <= max; i++)
    {
        uint32_t id = 0;
        if (hotbar.contains(i))
            id = hotbar[i];
        writeData << std::dec << id;
        if (i < max)
            writeData << ", ";
    }
    writeData << "] # array of numeric ent types" << std::endl;

    writeData << "font_file = \"" << fontfile << "\" # string, \"file.ttf\" or empty to use the embedded font \"Hack\"" << std::endl;
    writeData << "font_size = [";
    for (unsigned int i = 0; i < fontsize.size(); i++)
    {
        writeData << std::endl
                  << "  " << fontsize[i];
        if (i < fontsize.size() - 1)
            writeData << ",";
    }
    if (!fontsize.empty())
        writeData << std::endl;
    writeData << "] # [ small/ui, medium/messages, big ]" << std::endl;

    writeData << "# Script filenames to load automatically on start. Example: autorun_scripts = [\"foo.lua\", \"bar.lua\"]" << std::endl;
    writeData << "autorun_scripts = [";
    for (unsigned int i = 0; i < g_script_autorun.size(); i++)
    {
        writeData << std::endl
                  << "  \"" << g_script_autorun[i] << "\"";
        if (i < g_script_autorun.size() - 1)
            writeData << ",";
    }
    if (!g_script_autorun.empty())
        writeData << std::endl;
    writeData << "]" << std::endl;

    writeData << "# Directory where the scripts are loaded from. You can use relative or absolute path." << std::endl
              << "# A relative path is relative to the game directory. Use forward / slashes." << std::endl;
    writeData << "script_dir = \"" << scriptpath << "\"" << std::endl;

    std::vector<std::string> ini_tabs_open;
    std::vector<std::string> ini_tabs_detached;
    for (auto& [name, window] : windows)
    {
        if (window->open)
            ini_tabs_open.push_back(name);
        if (window->detached)
            ini_tabs_detached.push_back(name);
    }
    if (windows.size() == 0)
    {
        ini_tabs_open = tabs_open;
        ini_tabs_detached = tabs_detached;
    }

    writeData << "tabs_open = [";
    for (unsigned int i = 0; i < ini_tabs_open.size(); i++)
    {
        writeData << std::endl
                  << "  \"" << ini_tabs_open[i] << "\"";
        if (i < ini_tabs_open.size() - 1)
            writeData << ",";
    }
    if (!ini_tabs_open.empty())
        writeData << std::endl;
    writeData << "]" << std::endl;

    writeData << "tabs_detached = [";
    for (unsigned int i = 0; i < ini_tabs_detached.size(); i++)
    {
        writeData << std::endl
                  << "  \"" << ini_tabs_detached[i] << "\"";
        if (i < ini_tabs_detached.size() - 1)
            writeData << ",";
    }
    if (!ini_tabs_detached.empty())
        writeData << std::endl;
    writeData << "]" << std::endl;

    writeData << "tab_active = \"" << active_tab << "\"" << std::endl;

    writeData.close();
}

void load_config(std::string file)
{
    toml::value data;
    try
    {
        data = toml::parse(cfgfile);
    }
    catch (std::exception&)
    {
        save_config(file);
        return;
    }

    toml::value hotkeys;
    try
    {
        hotkeys = toml::find(data, "hotkeys");
    }
    catch (std::exception&)
    {
        save_config(file);
        return;
    }
    for (const auto& kv : keys)
    {
        keys[kv.first] = toml::find_or<toml::integer>(hotkeys, kv.first, kv.second);
    }

    toml::value opts;
    try
    {
        opts = toml::find(data, "options");
    }
    catch (std::exception&)
    {
        save_config(file);
        return;
    }
    for (const auto& kv : options)
    {
        options[kv.first] = (bool)toml::find_or<int>(opts, kv.first, (int)kv.second);
    }
    ImGuiStyle& style = ImGui::GetStyle();
    g_hue = toml::find_or<float>(opts, "hue", 0.63f);
    g_sat = toml::find_or<float>(opts, "saturation", 0.66f);
    g_val = toml::find_or<float>(opts, "lightness", 0.66f);
    style.Alpha = toml::find_or<float>(opts, "alpha", 0.66f);
    ImGui::GetIO().FontGlobalScale = toml::find_or<float>(opts, "scale", 1.0f);
    g_camera_speed = toml::find_or<float>(opts, "camera_speed", 1.0f);
    kits.clear();
    saved_entities.clear();
    saved_entities = toml::find_or<std::vector<std::string>>(opts, "kits", {});
    for (auto& saved : saved_entities)
    {
        kits.push_back(new Kit({saved, false}));
    }
    saved_entities.clear();
    hotbar.clear();
    saved_hotbar.clear();
    saved_hotbar = toml::find_or<std::vector<uint32_t>>(opts, "hotbar", {});
    for (uint32_t i = 0; i < saved_hotbar.size(); ++i)
    {
        if (saved_hotbar[i] != 0)
            hotbar[i] = saved_hotbar[i];
    }
    saved_hotbar.clear();
    g_script_autorun = toml::find_or<std::vector<std::string>>(opts, "autorun_scripts", {});
    scriptpath = toml::find_or<std::string>(opts, "script_dir", "Overlunky/Scripts");
    fontfile = toml::find_or<std::string>(opts, "font_file", "");
    auto ini_fontsize = toml::find_or<std::vector<float>>(opts, "font_size", {14.0f, 32.0f, 72.0f});
    if (ini_fontsize.size() >= 3)
        fontsize = ini_fontsize;
    auto ini_tabs_open = toml::find_or<std::vector<std::string>>(opts, "tabs_open", {"tool_entity", "tool_door", "tool_camera", "tool_entity_properties", "tool_game_properties", "tool_finder"});
    tabs_open = ini_tabs_open;
    auto ini_tabs_detached = toml::find_or<std::vector<std::string>>(opts, "tabs_detached", {});
    tabs_detached = ini_tabs_detached;
    active_tab = toml::find_or<std::string>(opts, "tab_active", "tool_entity");
    activate_tab = active_tab;
    UI::godmode(options["god_mode"]);
    if (options["disable_achievements"])
        UI::steam_achievements(false);
    if (options["disable_savegame"])
        hook_savegame();
    UI::set_time_ghost_enabled(!options["disable_ghost_timer"]);
    UI::set_time_jelly_enabled(!options["disable_ghost_timer"]);
    UI::set_cursepot_ghost_enabled(!options["disable_ghost_timer"]);
    if (options["multi_viewports"] && !detect_wine())
    {
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    }
    else
    {
        options["multi_viewports"] = false;
        ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable;
    }
    ImGui::GetIO().ConfigDockingWithShift = options["docking_with_shift"];
    g_Console->set_alt_keys(options["console_alt_keys"]);
    imgui_vsync(options["vsync"]);
    if (options["uncap_unfocused_fps"])
    {
        g_unfocused_fps = 0;
        update_frametimes();
    }
    if (g_bucket)
    {
        g_bucket->pause_api->update_camera = options["pause_update_camera"];
        g_bucket->pause_api->last_instance = options["pause_last_instance"];
        g_bucket->pause_api->pause_type = (PAUSE_TYPE)toml::find_or<int64_t>(opts, "pause_type", 0x140);
        g_bucket->pause_api->ignore_screen = (PAUSE_SCREEN)toml::find_or<int64_t>(opts, "pause_ignore_screen", 0);
        g_bucket->pause_api->ignore_screen_trigger = (PAUSE_SCREEN)toml::find_or<int64_t>(opts, "pause_ignore_screen_trigger", 0);
        g_bucket->pause_api->pause_trigger = (PAUSE_TRIGGER)toml::find_or<int64_t>(opts, "pause_trigger", 0);
        g_bucket->pause_api->pause_screen = (PAUSE_SCREEN)toml::find_or<int64_t>(opts, "pause_screen", 0);
        g_bucket->pause_api->unpause_trigger = (PAUSE_TRIGGER)toml::find_or<int64_t>(opts, "unpause_trigger", 0);
        g_bucket->pause_api->unpause_screen = (PAUSE_SCREEN)toml::find_or<int64_t>(opts, "unpause_screen", 0);
        g_bucket->pause_api->modifiers_block = toml::find_or<uint32_t>(opts, "modifiers_block", 0);
        g_bucket->pause_api->modifiers_clear_input = options["modifiers_clear_input"];
    }
    save_config(file);
}

bool detached(std::string window)
{
    if (windows.find(window) == windows.end())
        return false;
    return windows[window]->detached;
}

bool toggle(std::string tool)
{
    if (detached(tool))
    {
        windows[tool]->open = true;
        const char* name = windows[tool]->name.c_str();
        ImGuiContext& g = *GImGui;
        ImGuiWindow* current = g.NavWindow;
        ImGuiWindow* win = ImGui::FindWindowByName(name);
        if (win != NULL)
        {
            if (win->Collapsed || win != current)
            {
                win->Collapsed = false;
                ImGui::FocusWindow(win);
                return true;
            }
            else
            {
                win->Collapsed = true;
                ImGui::FocusWindow(NULL);
            }
        }
        return false;
    }
    else if (options["menu_ui"])
    {
        for (auto& [name, window] : windows)
            window->popup = false;
        windows[tool]->popup = true;
        return true;
    }
    else
    {
        ImGuiWindow* win = ImGui::FindWindowByName("Overlunky");
        if (win)
            win->Collapsed = false;
        for (auto& window : windows)
        {
            if (window.first == tool)
            {
                window.second->open = true;
                activate_tab = tool;
            }
        }
        return true;
    }
}

bool focused(std::string window)
{
    return focused_tool == window;
}

bool active(std::string window)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* current = g.NavWindow;
    if (current == NULL)
        return false;
    if (detached(window))
    {
        if (windows.find(window) == windows.end())
            return false;
        return current == ImGui::FindWindowByName(windows[window]->name.c_str()) || window == active_tab;
    }
    else if (options["menu_ui"])
    {
        return window == active_tab;
    }
    else
    {
        const char* test = strstr(current->Name, "Overlunky");
        return current->Name == test && active_tab == window;
    }
}

void detach(std::string window)
{
    if (windows.find(window) == windows.end())
        return;
    windows[window]->detached = true;
}

void attach(std::string window) // unused
{
    if (windows.find(window) == windows.end())
        return;
    windows[window]->detached = false;
}

bool visible(std::string window)
{
    if (windows.find(window) == windows.end())
        return false;
    if (detached(window))
    {
        ImGuiWindow* win = ImGui::FindWindowByName(windows[window]->name.c_str());
        if (win != NULL)
            return !win->Collapsed;
        return false;
    }
    else if (options["menu_ui"])
    {
        return window == active_tab;
    }
    else
    {
        return active_tab == window;
    }
}

void escape()
{
    ImGui::SetWindowFocus(nullptr);
}

void save_search()
{
    std::string items = text;
    trim(items);
    std::stringstream sss(items);
    uint32_t id = 0;
    sss >> id;
    if (id != 0)
    {
        kits.push_back(new Kit({items, false}));
    }
    else if (g_current_item > 0 || (unsigned)g_filtered_count < g_items.size())
    {
        EntityItem to_add = g_items[g_filtered_items[g_current_item]];
        items = fmt::format("{}", to_add.id);
        kits.push_back(new Kit({items, false}));
    }
    save_config(cfgfile);
}

bool update_players()
{
    g_players = UI::get_players();
    return true;
}

void fix_decorations_at(float x, float y, LAYER layer)
{
    int gx = static_cast<uint32_t>(std::round(x));
    int gy = static_cast<uint32_t>(std::round(y));
    for (int dx = gx - 1; dx <= gx + 1; ++dx)
    {
        for (int dy = gy - 1; dy <= gy + 1; ++dy)
        {
            auto fx = static_cast<float>(dx);
            auto fy = static_cast<float>(dy);
            UI::update_floor_at(fx, fy, layer);
        }
    }
}

void smart_delete(Entity* ent, bool unsafe = false)
{
    static auto first_door = to_id("ENT_TYPE_FLOOR_DOOR_ENTRANCE");
    static auto logical_door = to_id("ENT_TYPE_LOGICAL_DOOR");
    if (!ent->is_player())
        ent->flags = set_flag(ent->flags, 1);
    if (!(ent->type->search_flags & ENTITY_MASK::ACTIVEFLOOR))
    {
        for (auto item : ent->items.entities())
        {
            if (!item->is_player())
                item->flags = set_flag(item->flags, 1);
        }
    }
    UI::safe_destroy(ent, unsafe);
    if ((ent->type->id >= first_door && ent->type->id <= first_door + 15) || ent->type->id == logical_door)
    {
        auto pos = ent->abs_position();
        auto layer = (LAYER)ent->layer;
        UI::cleanup_at(pos.x, pos.y, layer, ent->type->id);
    }
    if (!!(ent->type->search_flags & (ENTITY_MASK::ACTIVEFLOOR | ENTITY_MASK::FLOOR)))
    {
        auto pos = ent->abs_position();
        auto layer = (LAYER)ent->layer;
        ENT_TYPE type = ent->type->id;
        fix_decorations_at(std::round(pos.x), std::round(pos.y), layer);
        UI::cleanup_at(std::round(pos.x), std::round(pos.y), layer, type);
    }
}

void reset_windows()
{
    for (auto& [name, window] : windows)
    {
        window->detached = false;
        window->open = true;
    }
}

static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)
{
    return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y);
}

static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs)
{
    return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y);
}

ImVec2 fix_pos(ImVec2 orig)
{
    return ImGui::GetMainViewport()->Pos + orig;
}

ImVec2 mouse_pos()
{
    auto base = ImGui::GetMainViewport();
    return ImGui::GetMousePos() - base->Pos;
}

std::optional<EntityItem> get_spawn_item()
{
    auto to_spawn = g_items[g_filtered_items[g_current_item]];
    if (g_current_item == 0 && (unsigned)g_filtered_count == g_items.size())
    {
        if (g_entity && g_entity->type->id <= to_id("ENT_TYPE_LIQUID_COARSE_LAVA"))
        {
            to_spawn = EntityItem{entity_full_names[g_entity->type->id], g_entity->type->id};
        }
        else if (g_last_type >= 0 && g_last_type <= (int)to_id("ENT_TYPE_LIQUID_COARSE_LAVA"))
        {
            to_spawn = EntityItem{entity_full_names[g_last_type], (uint32_t)g_last_type};
        }
        else
        {
            return std::nullopt;
        }
    }
    return to_spawn;
}

std::string spawned_type()
{
    const auto pos = text.find_first_of(" ");
    if (pos == std::string::npos && g_filtered_count > 0)
    {
        auto to_spawn = get_spawn_item();
        if (to_spawn.has_value())
            return to_spawn.value().name;
    }
    else
    {
        return text;
    }
    return "";
}

int32_t spawn_entityitem(EntityItem to_spawn, bool s, bool set_last = true)
{
    static const ENT_TYPE also_snap[] = {
        to_id("ENT_TYPE_LOGICAL_DOOR"),
        to_id("ENT_TYPE_LOGICAL_BLACKMARKET_DOOR"),
        to_id("ENT_TYPE_LOGICAL_PLATFORM_SPAWNER"),
    };
    bool flip = g_vx < -0.04f;
    std::pair<float, float> cpos = UI::click_position(g_x, g_y);
    if (to_spawn.name.find("ENT_TYPE_CHAR") != std::string::npos)
    {
        int spawned = UI::spawn_companion(to_spawn.id, cpos.first, cpos.second, LAYER::PLAYER, g_vx, g_vy);
        get_entity_ptr(spawned)->apply_layer();
        if (!lock_entity && set_last && options["draw_hitboxes"])
            g_last_id = spawned;
        return spawned;
    }
    else if (to_spawn.name.find("ENT_TYPE_ITEM_POWERUP") != std::string::npos)
    {
        if (!g_players.empty())
            g_players.at(0)->give_powerup(to_spawn.id);
    }
    else if (to_spawn.name == "ENT_TYPE_ITEM_PLAYERGHOST")
    {
        static const auto ana_spelunky = to_id("ENT_TYPE_CHAR_ANA_SPELUNKY");
        auto spawned = UI::spawn_playerghost(ana_spelunky + (rand() % 19), cpos.first, cpos.second, LAYER::PLAYER, g_vx, g_vy);
        get_entity_ptr(spawned)->apply_layer();
        if (!lock_entity && set_last && options["draw_hitboxes"])
            g_last_id = spawned;
        return spawned;
    }
    else if (to_spawn.name.find("ENT_TYPE_LIQUID") == std::string::npos)
    {
        bool snap = options["snap_to_grid"];
        if (to_spawn.name.find("ENT_TYPE_ACTIVEFLOOR") != std::string::npos)
            snap = true;
        if (std::find(std::begin(also_snap), std::end(also_snap), to_spawn.id) != std::end(also_snap))
        {
            snap = true;
        }
        else if (to_spawn.name.find("ENT_TYPE_FLOOR") != std::string::npos)
        {
            snap = true;
            g_vx = 0;
            g_vy = 0;

            auto old_block_id = UI::get_grid_entity_at(cpos.first, cpos.second, LAYER::PLAYER);
            if (old_block_id != -1)
            {
                auto old_block = get_entity_ptr(old_block_id);

                if (old_block)
                    smart_delete(old_block);
            }
            else
            {
                auto old_activefloor = UI::get_entity_at(std::round(cpos.first), std::round(cpos.second), false, 0.5f, 0x80);
                if (old_activefloor)
                    smart_delete(old_activefloor);
            }
        }
        int spawned = UI::spawn_entity(to_spawn.id, g_x, g_y, s, g_vx, g_vy, snap);
        get_entity_ptr(spawned)->apply_layer();
        if (to_spawn.name.find("ENT_TYPE_MOUNT") != std::string::npos)
        {
            auto mount = get_entity_ptr(spawned)->as<Mount>();
            mount->tame(true);
        }
        if (to_spawn.name.find("ENT_TYPE_FLOOR") != std::string::npos && options["spawn_floor_decorated"])
        {
            if (Floor* floor = get_entity_ptr(spawned)->as<Floor>())
            {
                if (flip && (to_spawn.name.find("ARROW_TRAP") != std::string::npos || to_spawn.name.find("LASER_TRAP") != std::string::npos || to_spawn.name.find("HORIZONTAL") != std::string::npos))
                {
                    floor->flags |= (1U << 16);
                    for (auto trig : floor->items.entities())
                        trig->flags |= (1U << 16);
                }
                uint32_t i_x = static_cast<uint32_t>(floor->x + 0.5f);
                uint32_t i_y = static_cast<uint32_t>(floor->y + 0.5f);
                HeapBase::get().state()->layer(floor->layer)->grid_entities[i_y][i_x] = floor;
                fix_decorations_at(floor->x, floor->y, (LAYER)floor->layer);
            }
        }
        if (flip)
        {
            auto ent = get_entity_ptr(spawned);
            if (ent)
                ent->flags |= (1U << 16);
        }
        if (to_spawn.id == also_snap[0])
        {
            auto ent = get_entity_ptr(spawned)->as<LogicalDoor>();
            ent->door_type = to_id("ENT_TYPE_FLOOR_DOOR_LAYER");
            ent->platform_type = to_id("ENT_TYPE_FLOOR_DOOR_PLATFORM");
        }
        if (!lock_entity && set_last && options["draw_hitboxes"])
            g_last_id = spawned;
        return spawned;
    }
    else
    {
        UI::spawn_liquid(to_spawn.id, cpos.first, cpos.second);
    }
    return -1;
}

void spawn_kit(Kit* kit)
{
    if (g_players.size() == 0)
        return;

    if ((g_state->screen < 11 || g_state->screen > 19) && g_state->screen != 1 && g_state->screen != 26)
        return;

    static const ENT_TYPE wearable[] = {
        to_id("ENT_TYPE_ITEM_CAPE"),
        to_id("ENT_TYPE_ITEM_VLADS_CAPE"),
        to_id("ENT_TYPE_ITEM_JETPACK"),
        to_id("ENT_TYPE_ITEM_TELEPORTER_BACKPACK"),
        to_id("ENT_TYPE_ITEM_HOVERPACK"),
        to_id("ENT_TYPE_ITEM_POWERPACK"),
    };

    std::stringstream textss(text);
    if (kit->items != "")
        textss.str(kit->items);
    uint32_t id;
    while (textss >> id)
    {
        EntityItem item = EntityItem{entity_full_names[id], id};
        int32_t spawned = -1;
        if (item.name.find("FLOOR_") != std::string::npos || item.name.find("DECORATION_") != std::string::npos || item.name.find("FX_") != std::string::npos || item.name.find("BG_") != std::string::npos || item.name.find("LOGICAL_") != std::string::npos)
        {
            continue;
        }
        else if (item.name.find("ENT_TYPE_ITEM_PICKUP") != std::string::npos)
        {
            spawned = UI::spawn_entity_over(id, g_players.at(0)->uid, 0.0f, 0.0f);
        }
        else if (item.name.find("ENT_TYPE_ITEM_POWERUP") != std::string::npos)
        {
            g_players.at(0)->give_powerup(id);
        }
        else
        {
            spawned = spawn_entityitem(item, false, false);
        }

        if (spawned == -1)
            continue;

        auto spawned_ent = get_entity_ptr(spawned)->as<Movable>();

        if (std::find(std::begin(wearable), std::end(wearable), id) != std::end(wearable) && g_players.at(0)->worn_backitem() == -1)
        {
            g_players.at(0)->pick_up(spawned_ent);
        }
        else if (item.name.find("MOUNT_") != std::string::npos)
        {
            auto spawned_mount = get_entity_ptr(spawned)->as<Mount>();
            spawned_mount->tame(true);
            if (!g_players.at(0)->overlay)
            {
                g_players.at(0)->standing_on_uid = -1;
                spawned_mount->carry(g_players.at(0));
            }
        }
        else if (g_players.at(0)->holding_uid == -1 && test_flag(spawned_ent->flags, 18))
        {
            g_players.at(0)->pick_up(spawned_ent);
        }
    }
}

void spawn_entities(bool s, std::string list = "")
{
    if (g_game_manager->pause_ui->visibility > 0)
        return;

    if ((g_state->screen < 11 || g_state->screen > 19) && g_state->screen != 1 && g_state->screen != 26)
        return;

    const auto pos = text.find_first_of(" ");
    if (list == "" && pos == std::string::npos && g_filtered_count > 0)
    {
        auto to_spawn = get_spawn_item();
        if (to_spawn.has_value())
            spawn_entityitem(to_spawn.value(), s);
    }
    else
    {
        std::stringstream textss(text);
        if (list != "")
            textss.str(list);
        uint32_t id;
        while (textss >> id)
        {
            spawn_entityitem(EntityItem{entity_full_names[id], id}, s);
        }
    }
}

void spawn_entity_over()
{
    if ((g_state->screen < 11 || g_state->screen > 19) && g_state->screen != 1 && g_state->screen != 26)
        return;

    static const auto turkey = to_id("ENT_TYPE_MOUNT_TURKEY");
    static const auto couch = to_id("ENT_TYPE_MOUNT_BASECAMP_COUCH");
    auto to_spawn = get_spawn_item();
    if (to_spawn.has_value())
    {
        const auto& item = to_spawn.value();
        if (item.name.find("ENT_TYPE_LIQUID") != std::string::npos)
        {
            auto cpos = UI::click_position(g_x, g_y);
            auto mpos = normalize(mouse_pos());
            auto cpos2 = UI::click_position(mpos.x, mpos.y);
            g_last_id = g_state->next_entity_uid;
            UI::spawn_liquid(item.id, cpos.first + 0.3f, cpos.second + 0.3f, 2 * (cpos2.first - cpos.first), 2 * (cpos2.second - cpos.second), 0, 1, INFINITY);
            return;
        }

        if (g_over_id == -1 || !get_entity_ptr(g_over_id))
            return;
        auto overlay = get_entity_ptr(g_over_id);

        if (item.name.find("ENT_TYPE_ITEM_POWERUP") != std::string::npos)
        {
            auto who = overlay->as<PowerupCapable>();
            who->give_powerup(item.id);
        }
        else if (item.name.find("ENT_TYPE_ITEM") != std::string::npos && (overlay->type->search_flags & ENTITY_MASK::FLOOR) == ENTITY_MASK::FLOOR)
        {
            int spawned = UI::spawn_entity_over(item.id, g_over_id, g_dx, g_dy);
            auto ent = get_entity_ptr(spawned);
            if (g_dx == 0 && g_dy == 0)
            {
                ent->set_draw_depth(9, 0);
                ent->flags = set_flag(ent->flags, 4);  // pass thru objects
                ent->flags = set_flag(ent->flags, 10); // no gravity
                ent->flags = clr_flag(ent->flags, 13); // collides walls
                if (!test_flag(g_state->special_visibility_flags, 1))
                    ent->flags = set_flag(ent->flags, 1); // invisible
            }
            if (item.id == to_id("ENT_TYPE_ITEM_EGGSAC"))
            {
                if (g_dx == -1 and g_dy == 0)
                    ent->angle = 3.14159265358979323846f / 2.0f;
                else if (g_dx == 1 and g_dy == 0)
                    ent->angle = 3.0f * 3.14159265358979323846f / 2.0f;
                else if (g_dx == 0 and g_dy == -1)
                    ent->angle = 3.14159265358979323846f;
            }
            if (!lock_entity)
                g_last_id = spawned;
        }
        else if (item.name.find("ENT_TYPE_MONS") != std::string::npos && overlay->type->id >= turkey && overlay->type->id <= couch)
        {
            auto mount = overlay->as<Mount>();
            int spawned = UI::spawn_entity(item.id, g_x, g_y, true, g_vx, g_vy, false);
            auto rider = get_entity_ptr(spawned)->as<Movable>();
            mount->carry(rider);
            rider->move_state = 0;
            if (!lock_entity)
                g_last_id = spawned;
        }
        else
        {
            int spawned = UI::spawn_entity_over(item.id, g_over_id, g_dx, g_dy);
            if (!lock_entity)
                g_last_id = spawned;
        }
    }
}

int pick_selected_entity(ImGuiInputTextCallbackData* data)
{
    if (data->EventFlag == ImGuiInputTextFlags_CallbackCompletion)
    {
        if (g_filtered_count == 0)
            return 1;
        if (g_current_item == 0 && (unsigned)g_filtered_count == g_items.size())
            return 1;
        std::string search(text);
        // while(!search.empty() && std::isspace(search.back()))
        // search.pop_back();
        const auto pos = search.find_last_of(" 0123456789");
        if (pos == std::string::npos)
        {
            search = "";
        }
        else
        {
            search = search.substr(0, pos) + " ";
        }
        std::stringstream searchss;
        searchss << search << g_items[g_filtered_items[g_current_item]].id << " ";
        search = searchss.str();
        data->DeleteChars(0, data->BufTextLen);
        data->InsertChars(0, search.c_str());
    }
    return 0;
}

bool update_entity()
{
    if (!visible("tool_entity_properties") && !options["draw_hitboxes"] && g_last_id == -1)
        return false;
    if (g_last_id > -1)
    {
        g_entity = get_entity_ptr(g_last_id);
        if (!g_entity)
            return false;

        return true;
    }
    g_entity = nullptr;
    return false;
}

bool fix_co_coordinates(std::pair<float, float>& cpos)
{
    bool fixed = false;
    float maxx = g_state->w * 10.0f + 2.5f;
    float minx = 2.5f;
    float maxy = 122.5f;
    float miny = 122.5f - g_state->h * 8.0f;
    while (cpos.first > maxx)
    {
        g_state->camera->focus_x -= g_state->w * 10.0f;
        g_state->camera->adjusted_focus_x -= g_state->w * 10.0f;
        g_state->camera->calculated_focus_x -= g_state->w * 10.0f;
        cpos.first -= g_state->w * 10.0f;
        fixed = true;
    }
    while (cpos.first < minx)
    {
        g_state->camera->focus_x += g_state->w * 10.0f;
        g_state->camera->adjusted_focus_x += g_state->w * 10.0f;
        g_state->camera->calculated_focus_x += g_state->w * 10.0f;
        cpos.first += g_state->w * 10.0f;
        fixed = true;
    }
    while (cpos.second > maxy)
    {
        g_state->camera->focus_y -= g_state->h * 8.0f;
        g_state->camera->adjusted_focus_y -= g_state->h * 8.0f;
        g_state->camera->calculated_focus_y -= g_state->h * 8.0f;
        cpos.second -= g_state->h * 8.0f;
        fixed = true;
    }
    while (cpos.second < miny)
    {
        g_state->camera->focus_y += g_state->h * 8.0f;
        g_state->camera->adjusted_focus_y += g_state->h * 8.0f;
        g_state->camera->calculated_focus_y += g_state->h * 8.0f;
        cpos.second += g_state->h * 8.0f;
        fixed = true;
    }
    return fixed;
}

void set_zoom()
{
    UI::zoom(g_zoom);
}

void force_lights()
{
    if (options["lights"])
    {
        if (!g_state->illumination && g_state->screen == 12)
            g_state->illumination = UI::create_illumination(Color::white(), 20000.0f, 172, 252);

        if (g_state->illumination)
        {
            g_state->illumination->enabled = true;
            g_state->illumination->layer = g_state->camera_layer;
        }
    }
}

void set_camera_bounds(bool enabled)
{
    enable_camera_bounds = enabled;
    if (enabled && g_state->theme != 10)
    {
        g_state->camera->bounds_left = 0.5f;
        g_state->camera->bounds_right = g_state->w * 10.0f + 4.5f;
        g_state->camera->bounds_top = 124.5f;
        g_state->camera->bounds_bottom = 120.5f - g_state->h * 8.0f;
        if (g_state->logic && g_state->logic->black_market) // if this pointer exists, so does an undiscovered black market, I hope
            g_state->camera->bounds_bottom += 32.0f;
    }
    else
    {
        g_state->camera->bounds_left = -FLT_MAX;
        g_state->camera->bounds_right = FLT_MAX;
        g_state->camera->bounds_top = FLT_MAX;
        g_state->camera->bounds_bottom = -FLT_MAX;
    }
}

void force_zoom()
{
    if (g_state->screen == 12 && !enable_camera_bounds)
        set_camera_bounds(enable_camera_bounds);
    if (g_zoom == 0.0f && g_state != 0 && (g_state->w != g_level_width) && (g_state->screen == 11 || g_state->screen == 12))
    {
        set_zoom();
        g_level_width = g_state->w;
    }
    if ((g_zoom > 13.5f || g_zoom == 0.0f) && g_state != 0 && g_state->screen == 11 && (enable_camp_camera || g_state->time_level == 1))
    {
        enable_camp_camera = false;
        UI::set_camp_camera_bounds_enabled(false);
        g_state->camera->bounds_left = 0.5;
        g_state->camera->bounds_right = 74.5;
        g_state->camera->bounds_top = 124.5;
        g_state->camera->bounds_bottom = 56.5;
    }
    else if (g_zoom == 13.5f && g_state != 0 && g_state->screen == 11 && g_state->time_level == 1)
    {
        enable_camp_camera = true;
        UI::set_camp_camera_bounds_enabled(true);
    }
    if (g_camera_inertia != -FLT_MAX)
        g_state->camera->inertia = g_camera_inertia;
}

void force_hud_flags()
{
    if (g_state == 0)
        return;
    if (!options["disable_pause"] && !ImGui::GetIO().WantCaptureKeyboard)
        g_state->level_flags |= 1U << 19;
    else if (!ImGui::GetIO().WantCaptureKeyboard)
        g_state->level_flags &= ~(1U << 19);
}

void toggle_noclip()
{
    g_players = UI::get_players();
    for (auto ent : g_players)
    {
        auto player = (Movable*)ent->topmost_mount();
        if (options["noclip"])
        {
            player->detach(false);
            player->type->max_speed = 0.3f;
        }
        else
        {
            player->flags &= ~(1U << 9);
            player->flags |= 1U << 10;
            player->flags &= ~(1U << 4);
            player->type->max_speed = 0.0725f;
        }
    }
    if (!options["noclip"] && g_state->items->player_select_slots[0].activated)
    {
        auto type = get_type(g_state->items->player_select_slots[0].character);
        if (type)
            type->max_speed = 0.0725f;
    }
}

bool should_speedhack()
{
    return (g_state->screen != 11 && g_state->screen != 12 && g_state->screen != 5 && g_state->screen != 4) || (g_state->screen != 5 && g_game_manager->pause_ui && g_game_manager->pause_ui->visibility > 0) || ((g_state->level_flags & (1U << 20)) > 0) || g_state->loading > 0 || (g_state->screen == 4 && g_game_manager->screen_menu->menu_text_opacity < 1.0f);
}

void force_cheats()
{
    g_players = UI::get_players();
    if (options["noclip"])
    {
        for (auto ent : g_players)
        {
            auto player = ent->topmost_mount()->as<Movable>();
            if (player->overlay)
            {
                if (player->state == 6 && (player->movex != 0 || player->movey != 0))
                {
                    auto [x, y] = UI::get_position(player);
                    UI::teleport_entity_abs(player, x + player->movex * 0.3f, y + player->movey * 0.07f, 0, 0);
                }
                else
                    player->overlay->remove_item(player, false);
            }
            player->standing_on_uid = -1;
            player->flags |= 1U << 9;
            player->flags &= ~(1U << 10);
            player->flags |= 1U << 4;
            player->velocityx = player->movex * player->type->max_speed;
            player->velocityy = player->movey * player->type->max_speed;
            if (g_state->theme == 10)
            {
                auto cpos = UI::get_position(player);
                if (fix_co_coordinates(cpos))
                {
                    UI::teleport_entity_abs(player, cpos.first, cpos.second, player->velocityx, player->velocityy);
                }
            }
        }
    }
    static const auto ink = to_id("ENT_TYPE_FX_INK_BLINDNESS");
    if (options["god_mode"])
    {
        for (auto ent : g_players)
        {
            // Remove icecage, stun, poison, curse, axo bubble, inkspit etc
            ent->frozen_timer = 0;
            ent->stun_timer = 0;
            ent->poison_tick_timer = -1;
            ent->onfire_effect_timer = 0;
            ent->wet_effect_timer = 0;
            ent->set_cursed(false, false);
            ent->more_flags &= ~(1U << 16);
            UI::destroy_entity_item_type(ent, ink);
            static auto spikes_item = to_id("ENT_TYPE_ITEM_SPIKES");
            if (ent->overlay && ent->overlay->type->id == spikes_item)
                ent->detach(false);
        }
    }
    if (options["fly_mode"])
    {
        for (auto ent : g_players)
        {
            auto player = (Movable*)(ent->topmost_mount());
            if ((player->buttons & 1) == 1 && player->velocityy < 0.18f)
                player->velocityy = 0.18f;
        }
    }
    if (options["speedhack"])
    {
        if (should_speedhack())
            UI::speedhack(10.0f);
        else if (g_speedhack_ui_multiplier == 10.0f && !should_speedhack())
            UI::speedhack(1.0f);
    }
}

void quick_start(uint8_t screen, uint8_t world, uint8_t level, uint8_t theme, std::optional<uint32_t> seed = std::nullopt)
{
    if (g_state->screen < 11)
    {
        if (seed.has_value())
            UI::init_seeded(seed.value());
        else
            UI::init_adventure();
    }
    else
    {
        if (seed.has_value())
            g_state->seed = seed.value();
    }
    g_state->screen_character_select->available_mine_entrances = 4;

    static const auto ana_spelunky = to_id("ENT_TYPE_CHAR_ANA_SPELUNKY");
    const auto ana_texture = get_type(ana_spelunky)->texture_id;

    g_state->items->player_select_slots[0].activated = true;
    g_state->items->player_select_slots[0].character = g_save->players[0] + ana_spelunky;
    g_state->items->player_select_slots[0].texture_id = g_save->players[0] + ana_texture;
    if (g_state->items->player_count < 1)
        g_state->items->player_count = 1;
    if (screen == 11) // this skips rope cutscene in camp
    {
        g_state->force_current_theme(17);
        g_state->screen = 11;
    }
    g_state->screen_next = screen;
    g_state->world_start = world;
    g_state->level_start = level;
    g_state->theme_start = theme;
    g_state->world_next = world;
    g_state->level_next = level;
    g_state->theme_next = theme;
    g_state->quest_flags |= 1;
    if (seed.has_value())
        g_state->quest_flags |= 0x40;
    g_state->fade_enabled = false;
    g_state->loading = 2;

    if (g_game_manager->main_menu_music)
    {
        g_game_manager->main_menu_music->kill(false);
        g_game_manager->main_menu_music = nullptr;
    }

    if (g_game_manager->game_props->input_index[0] == -1)
        g_game_manager->game_props->input_index[0] = 0;
    if (g_game_manager->game_props->input_index[4] == -1)
        g_game_manager->game_props->input_index[4] = 0;

    g_game_manager->screen_menu->loaded_once = true;
}

void restart_adventure()
{
    auto seed = UI::get_adventure_seed(true);
    if (g_state->screen < 11)
        quick_start(12, 1, 1, 1);
    if ((g_state->quest_flags & 0x40) == 0)
    {
        UI::set_adventure_seed(seed.first, seed.second);
        g_state->world_next = g_state->world_start;
        g_state->level_next = g_state->level_start;
        g_state->theme_next = g_state->theme_start;
        g_state->screen_next = 12;
        g_state->quest_flags |= 1;
        g_state->fade_enabled = false;
        g_state->loading = 2;
    }
    else
    {
        quick_start(12, 1, 1, 1);
    }
}

std::string get_clipboard()
{
    if (!OpenClipboard(nullptr))
        return "";
    HANDLE hData = GetClipboardData(CF_TEXT);
    if (hData == nullptr)
        return "";
    char* pszText = static_cast<char*>(GlobalLock(hData));
    if (pszText == nullptr)
        return "";
    std::string str(pszText);
    GlobalUnlock(hData);
    CloseClipboard();
    return str;
}

void set_clipboard(std::string str)
{
    const char* output = str.c_str();
    const size_t len = strlen(output) + 1;
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
    memcpy(GlobalLock(hMem), output, len);
    GlobalUnlock(hMem);
    OpenClipboard(0);
    EmptyClipboard();
    SetClipboardData(CF_TEXT, hMem);
    CloseClipboard();
}

struct VoidData
{
    uint32_t x;
    uint32_t y;
    /*uint32_t health;
    uint32_t bombs;
    uint32_t ropes;
    uint32_t width;
    uint32_t height;
    uint32_t world;
    uint32_t level;
    uint32_t theme;*/
};

struct VoidEntity
{
    uint32_t id;
    uint32_t x;
    uint32_t y;
    uint32_t flags;
    /*uint32_t layer;
    uint32_t health;
    uint32_t state;
    uint32_t ai;*/
};

void clear_void()
{
    constexpr auto clear_mask = ENTITY_MASK::ITEM | ENTITY_MASK::MOUNT | ENTITY_MASK::MONSTER | ENTITY_MASK::ACTIVEFLOOR | ENTITY_MASK::BG | ENTITY_MASK::FLOOR;
    for (auto uid : UI::get_entities_by({}, clear_mask, LAYER::FRONT))
    {
        auto ent = get_entity_ptr(uid);
        auto [x, y] = ent->abs_position();
        if (x > 2.5f && y < 122.5f && x < g_state->w * 10.0f + 2.5f && y > 122.5f - g_state->h * 8.0f)
            UI::safe_destroy(ent);
    }
}

void load_void(std::string data)
{
    data.erase(std::remove_if(data.begin(), data.end(), [](char c)
                              { return !std::isalnum(c); }),
               data.end());
    VoidData v;
    sscanf_s(data.c_str(), "V1%02X%02X", &v.x, &v.y);
    g_players = UI::get_players();
    UI::teleport_entity_abs(g_players[0], (float)v.x, (float)v.y, 0, 0);

    std::string ents = data.substr(6);
    if (ents.size() > 0)
        clear_void();
    while (ents.size() > 0)
    {
        VoidEntity e;
        std::string str = ents.substr(0, 8);
        std::string id36 = str.substr(0, 2);
        sscanf_s(str.c_str() + 2, "%02X%02X%02X", &e.x, &e.y, &e.flags);
        e.id = strtol(id36.c_str(), nullptr, 36);
        if (e.id <= 915)
        {
            auto uid = UI::spawn_grid(e.id, (float)e.x, (float)e.y, 0);
            auto ent = get_entity_ptr(uid);
            if ((ent->type->search_flags & ENTITY_MASK::FLOOR) == ENTITY_MASK::FLOOR)
            {
                fix_decorations_at((float)e.x, (float)e.y, LAYER::FRONT);
                Callback cb = {g_state->time_total + 2, [e]
                               {
                                   fix_decorations_at((float)e.x, (float)e.y, LAYER::FRONT);
                               }};
                callbacks.push_back(cb);
            }
            else if ((ent->type->search_flags & ENTITY_MASK::ITEM) == ENTITY_MASK::ITEM)
            {
                ent->y = e.y - 0.5f + ent->hitboxy - ent->offsety;
            }
            if (e.flags & 1)
            {
                ent->flags = set_flag(ent->flags, 17);
                for (auto trig : ent->items.entities())
                    trig->flags = set_flag(trig->flags, 17);
            }
        }
        ents = ents.substr(8);
    }
    // TODO: attach spikes, totems to floor
}

void import_void()
{
    auto clip = get_clipboard();
    clip.erase(std::remove_if(clip.begin(), clip.end(), [](char c)
                              { return !std::isalnum(c); }),
               clip.end());

    if (clip.size() > 2 && clip[0] == 'V' && clip[1] == '1') // void data, version 1 I guess
    {
        if (g_state->screen != 12 || g_players.empty())
        {
            g_load_void = clip;
            g_ui_scripts["void"]->set_enabled(true);
            quick_start(12, 1, 1, 1);
        }
        else if (g_ui_scripts["void"]->is_enabled())
        {
            load_void(clip);
        }
        else
        {
            g_load_void = clip;
            g_ui_scripts["void"]->set_enabled(true);
            g_state->quest_flags = 1;
            g_state->loading = 1;
        }
    }
}

std::string serialize_void()
{
    constexpr auto export_mask = ENTITY_MASK::MOUNT | ENTITY_MASK::MONSTER | ENTITY_MASK::ITEM | ENTITY_MASK::ACTIVEFLOOR | ENTITY_MASK::FLOOR;
    auto [px, py] = g_players[0]->abs_position();
    std::string v = fmt::format("V1{:02X}{:02X}", (uint8_t)(px + 0.5f), (uint8_t)(py + 0.5f));
    auto uids = g_selected_ids;
    if (uids.empty())
        uids = UI::get_entities_by({}, export_mask, LAYER::FRONT);
    for (auto uid : uids)
    {
        auto ent = get_entity_ptr(uid);
        if (!ent || !(ent->type->search_flags & export_mask))
            continue;
        auto [x, y] = ent->abs_position();
        if ((!ent->overlay || (ent->x != 0 || ent->y != 0)) && x > 2.5f && y < 122.5f && x < g_state->w * 10.0f + 2.5f && y > 122.5f - g_state->h * 8.0f)
        {
            char buf[4];
            _itoa_s(ent->type->id, buf, 4, 36);
            if (ent->type->id < 36)
            {
                buf[1] = buf[0];
                buf[0] = '0';
            }
            uint32_t flags = 0;
            if (test_flag(ent->flags, 17))
                flags = set_flag(flags, 1);
            v += fmt::format("{:2s}{:02X}{:02X}{:02X}", buf, (uint8_t)(x + 0.5f), (uint8_t)(y + 0.5f), (uint8_t)flags);
        }
    }
    return v;
}

void export_void()
{
    if (g_state->screen != 12 || g_players.empty())
        return;
    set_clipboard(serialize_void());
}

void force_kits()
{
    if (g_state->screen == 12 && g_state->time_total <= 1 && g_state->loading == 3 && g_state->time_startup > g_last_kit_spawn)
    {
        for (auto kit : kits)
        {
            if (kit->automatic)
                spawn_kit(kit);
        }

        if (g_load_void != "")
        {
            load_void(g_load_void);
            g_load_void = "";
        }

        g_last_kit_spawn = g_state->time_startup + 1;
    }
}

bool toggle_pause()
{
    return g_bucket->pause_api->toggle();
}

void frame_advance()
{
    g_bucket->pause_api->frame_advance();
}

void warp_inc(uint8_t w, uint8_t l, uint8_t t)
{
    if (options["warp_increments_level_count"] && g_state->screen == 12)
    {
        g_state->level_count += 1;
    }
    if (options["warp_transition"] && (g_state->quest_flags & 0x40) == 0)
    {
        UI::transition(w, l, t);
    }
    else
    {
        UI::warp(w, l, t);
    }
}

void warp_next_level(size_t num)
{
    if (g_state->screen == 13)
    {
        UI::warp(g_state->world_next, g_state->level_next, g_state->theme_next);
        return;
    }
    std::vector<std::tuple<uint8_t, uint8_t, uint8_t>> targets; // tuple = world, level, theme
    uint8_t target_world = 0;
    uint8_t target_level = 0;
    uint8_t target_theme = 0;
    int tnum = g_state->world * 100 + g_state->level;
    switch (tnum)
    {
    case 104:
    case 301:
        break;
    case 501:
        target_world = 6;
        target_level = 1;
        target_theme = 8;
        break;
    case 204:
        target_world = 3;
        target_level = 1;
        target_theme = 4;
        break;
    case 403:
        if (g_state->theme == 6 || g_state->theme == 11)
        {
            target_world = 4;
            target_level = 4;
            target_theme = 6;
        }
        else
        {
            target_world = 4;
            target_level = 4;
            target_theme = 5;
        }
        break;
    case 404:
        target_world = 5;
        target_level = 1;
        target_theme = 7;
        break;
    case 603:
        target_world = 6;
        target_level = 4;
        target_theme = 14;
        break;
    case 604:
        target_world = 7;
        target_level = 1;
        target_theme = 9;
        break;
    case 702:
        target_world = 7;
        target_level = 3;
        target_theme = 9;
        break;
    case 703:
        target_world = 7;
        target_level = 4;
        target_theme = 16;
        break;
    case 704:
        target_world = 8;
        target_level = 5;
        target_theme = 10;
        break;
    default:
        target_world = g_state->world;
        target_level = g_state->level + 1;
        target_theme = g_state->theme;
        break;
    }
    if (g_state->theme == 17)
    {
        target_world = 1;
        target_level = 1;
        target_theme = 1;
    }
    if (target_world > 0)
    {
        targets.emplace_back(target_world, target_level, target_theme);
    }

    for (auto doorid : UI::get_entities_by({(ENT_TYPE)CUSTOM_TYPE::EXITDOOR}, ENTITY_MASK::FLOOR, LAYER::BOTH))
    {
        ExitDoor* doorent = get_entity_ptr(doorid)->as<ExitDoor>();
        if (!doorent->special_door)
            continue;
        targets.emplace_back(doorent->world, doorent->level, doorent->theme);
    }

    if (g_state->theme == 11)
    {
        uint8_t world = 4;
        uint8_t level = 4;
        uint8_t theme = 12;
        targets.emplace_back(world, level, theme);
    }

    if (targets.empty())
    {
        target_world = g_state->world;
        target_level = g_state->level + 1;
        target_theme = g_state->theme;
        targets.emplace_back(target_world, target_level, target_theme);
    }

    if (num > targets.size() - 1 && targets.size() > 0)
    {
        num = targets.size() - 1;
    }
    if (targets.size() > num)
    {
        uint8_t world, level, theme;
        std::tie(world, level, theme) = targets.at(num);
        warp_inc(world, level, theme);
    }
}

void respawn()
{
    if (g_state->screen != 11 && g_state->screen != 12 && g_state->screen != 14)
    {
        if (g_state->screen > 11)
        {
            quick_start(12, g_state->world_start, g_state->level_start, g_state->theme_start);
        }
        else
        {
            quick_start(12, 1, 1, 1);
        }
        return;
    }
    if (g_state->screen == 14)
    {
        g_state->screen = 12;
        g_game_manager->journal_ui->fade_timer = 15;
        g_game_manager->journal_ui->state = 5;
        g_state->camera->focus_offset_x = 0;
        g_state->camera->focus_offset_y = 0;
        set_camera_bounds(true);
    }
    for (int8_t i = 0; i < g_state->items->player_count; ++i)
    {
        auto found = false;
        for (auto p : UI::get_players())
        {
            if (p->inventory_ptr->player_slot == i)
            {
                found = true;
                if (p->health == 0 || test_flag(p->flags, 29))
                {
                    p->health = 4;
                    p->flags = clr_flag(p->flags, 29);
                    p->set_behavior(1);
                }
            }
        }
        if (!found)
        {
            g_state->items->player_inventories[i].health = 4;
            UI::spawn_player(i);
        }
    }
}

bool pressed(std::string keyname, WPARAM wParam)
{
    if (g_bucket->overlunky->ignore_keys.contains(keyname))
        return false;

    if (keys.find(keyname) == keys.end() || (keys[keyname] & 0xff) == 0)
    {
        return false;
    }
    int64_t keycode = keys[keyname];
    if (ImGui::GetIO().KeyCtrl)
    {
        wParam += OL_KEY_CTRL;
    }
    if (ImGui::GetIO().KeyShift)
    {
        wParam += OL_KEY_SHIFT;
    }
    if (ImGui::GetIO().KeyAlt)
    {
        wParam += OL_KEY_ALT;
    }
    return wParam == (unsigned)keycode && !g_bucket->overlunky->ignore_keycodes.contains(keycode);
}

bool pressing(std::string keyname)
{
    if (g_bucket->overlunky->ignore_keys.contains(keyname))
        return false;

    if (keys.find(keyname) == keys.end() || (keys[keyname] & 0xff) == 0)
    {
        return false;
    }
    int64_t keycode = keys[keyname];
    int key = (int)(keycode & 0xff);
    int64_t wParam = key;

    if (ImGui::GetIO().KeyCtrl)
    {
        wParam += OL_KEY_CTRL;
    }
    if (ImGui::GetIO().KeyShift)
    {
        wParam += OL_KEY_SHIFT;
    }
    if (ImGui::GetIO().KeyAlt)
    {
        wParam += OL_KEY_ALT;
    }
    return wParam == (unsigned)keycode && (GetKeyState(key) & 0x8000) && !g_bucket->overlunky->ignore_keycodes.contains(keycode);
}

bool clicked(std::string keyname)
{
    if (g_bucket->overlunky->ignore_keys.contains(keyname))
        return false;

    int wParam = OL_BUTTON_MOUSE;
    if (keys.find(keyname) == keys.end() || (keys[keyname] & 0xff) == 0)
    {
        return false;
    }
    int64_t keycode = keys[keyname];
    if (ImGui::GetIO().KeyCtrl)
    {
        wParam += OL_KEY_CTRL;
    }
    if (ImGui::GetIO().KeyShift)
    {
        wParam += OL_KEY_SHIFT;
    }
    if (ImGui::GetIO().KeyAlt)
    {
        wParam += OL_KEY_ALT;
    }
    if ((keycode & OL_MOUSE_WHEEL) > 0)
    {
        if (ImGui::GetIO().MouseWheel > 0)
        {
            wParam += OL_WHEEL_UP;
        }
        else if (ImGui::GetIO().MouseWheel < 0)
        {
            wParam += OL_WHEEL_DOWN;
        }
    }
    else
    {
        for (int i = 0; i < ImGuiMouseButton_COUNT; i++)
        {
            if (ImGui::IsMouseClicked(i))
            {
                wParam += i + 1;
                break;
            }
        }
    }
    return wParam == keycode && !g_bucket->overlunky->ignore_keycodes.contains(keycode);
}

bool dblclicked(std::string keyname)
{
    if (g_bucket->overlunky->ignore_keys.contains(keyname))
        return false;

    int wParam = OL_BUTTON_MOUSE;
    if (keys.find(keyname) == keys.end() || (keys[keyname] & 0xff) == 0)
    {
        return false;
    }
    int64_t keycode = keys[keyname];
    if (ImGui::GetIO().KeyCtrl)
    {
        wParam += OL_KEY_CTRL;
    }
    if (ImGui::GetIO().KeyShift)
    {
        wParam += OL_KEY_SHIFT;
    }
    if (ImGui::GetIO().KeyAlt)
    {
        wParam += OL_KEY_ALT;
    }
    for (int i = 0; i < ImGuiMouseButton_COUNT; i++)
    {
        if (ImGui::IsMouseDoubleClicked(i))
        {
            wParam += i + 1;
            break;
        }
    }
    return wParam == keycode && !g_bucket->overlunky->ignore_keycodes.contains(keycode);
}

bool held(std::string keyname)
{
    if (g_bucket->overlunky->ignore_keys.contains(keyname))
        return false;

    int wParam = OL_BUTTON_MOUSE;
    if (keys.find(keyname) == keys.end() || (keys[keyname] & 0xff) == 0)
    {
        return false;
    }
    int64_t keycode = keys[keyname];
    if (ImGui::GetIO().KeyCtrl)
    {
        wParam += OL_KEY_CTRL;
    }
    if (ImGui::GetIO().KeyShift)
    {
        wParam += OL_KEY_SHIFT;
    }
    if (ImGui::GetIO().KeyAlt)
    {
        wParam += OL_KEY_ALT;
    }
    for (int i = 0; i < ImGuiMouseButton_COUNT; i++)
    {
        if (ImGui::IsMouseDown(i))
        {
            wParam += i + 1;
            break;
        }
    }
    return wParam == keycode && !g_bucket->overlunky->ignore_keycodes.contains(keycode);
}

bool released(std::string keyname)
{
    if (g_bucket->overlunky->ignore_keys.contains(keyname))
        return false;

    int wParam = OL_BUTTON_MOUSE;
    if (keys.find(keyname) == keys.end() || (keys[keyname] & 0xff) == 0)
    {
        return false;
    }
    int64_t keycode = keys[keyname];
    if (ImGui::GetIO().KeyCtrl)
    {
        wParam += OL_KEY_CTRL;
    }
    if (ImGui::GetIO().KeyShift)
    {
        wParam += OL_KEY_SHIFT;
    }
    if (ImGui::GetIO().KeyAlt)
    {
        wParam += OL_KEY_ALT;
    }
    for (int i = 0; i < ImGuiMouseButton_COUNT; i++)
    {
        if (ImGui::IsMouseReleased(i))
        {
            wParam += i + 1;
            break;
        }
    }
    return wParam == keycode && !g_bucket->overlunky->ignore_keycodes.contains(keycode);
}

bool dragging(std::string keyname)
{
    if (g_bucket->overlunky->ignore_keys.contains(keyname))
        return false;

    int wParam = OL_BUTTON_MOUSE;
    if (keys.find(keyname) == keys.end() || (keys[keyname] & 0xff) == 0)
    {
        return false;
    }
    int64_t keycode = keys[keyname];
    if (ImGui::GetIO().KeyCtrl)
    {
        wParam += OL_KEY_CTRL;
    }
    if (ImGui::GetIO().KeyShift)
    {
        wParam += OL_KEY_SHIFT;
    }
    if (ImGui::GetIO().KeyAlt)
    {
        wParam += OL_KEY_ALT;
    }
    for (int i = 0; i < ImGuiMouseButton_COUNT; i++)
    {
        if (ImGui::IsMouseDragging(i))
        {
            wParam += i + 1;
            break;
        }
    }
    return wParam == keycode && !g_bucket->overlunky->ignore_keycodes.contains(keycode);
}

bool dragged(std::string keyname) // unused
{
    // int wParam = OL_BUTTON_MOUSE;
    if (keys.find(keyname) == keys.end() || (keys[keyname] & 0xff) == 0)
    {
        return false;
    }
    int keycode = keys[keyname] & 0xff;
    for (int i = 0; i < ImGuiMouseButton_COUNT; i++)
    {
        if (keycode == i + 1)
        {
            return ImGui::GetMouseDragDelta(i).x > 0 || ImGui::GetMouseDragDelta(i).y > 0;
        }
    }
    return false;
}

float drag_delta(std::string keyname)
{
    // int wParam = OL_BUTTON_MOUSE;
    if (keys.find(keyname) == keys.end() || (keys[keyname] & 0xff) == 0)
    {
        return false;
    }
    int keycode = keys[keyname] & 0xff;
    for (int i = 0; i < ImGuiMouseButton_COUNT; i++)
    {
        if (keycode == i + 1)
        {
            return std::abs(ImGui::GetMouseDragDelta(i).x) + std::abs(ImGui::GetMouseDragDelta(i).y);
        }
    }
    return false;
}

float held_duration(std::string keyname)
{
    // int wParam = OL_BUTTON_MOUSE;
    if (keys.find(keyname) == keys.end() || (keys[keyname] & 0xff) == 0)
    {
        return false;
    }
    int keycode = keys[keyname] & 0xff;
    for (int i = 0; i < ImGuiMouseButton_COUNT; i++)
    {
        if (keycode == i + 1)
        {
            return ImGui::GetIO().MouseDownDuration[i];
        }
    }
    return -1.0;
}

float held_duration_last(std::string keyname)
{
    // int wParam = OL_BUTTON_MOUSE;
    if (keys.find(keyname) == keys.end() || (keys[keyname] & 0xff) == 0)
    {
        return false;
    }
    int keycode = keys[keyname] & 0xff;
    for (int i = 0; i < ImGuiMouseButton_COUNT; i++)
    {
        if (keycode == i + 1)
        {
            return ImGui::GetIO().MouseDownDurationPrev[i];
        }
    }
    return -1.0;
}

std::string last_word(std::string str)
{
    while (!str.empty() && std::isspace(str.back()))
        str.pop_back();
    const auto pos = str.find_last_of(" ");
    return pos == std::string::npos ? str : str.substr(pos + 1);
}

void update_filter(std::string s)
{
    int count = 0;
    std::string last = last_word(s);
    uint32_t searchid = 0;
    // auto res = std::from_chars(last.c_str(), last.c_str() + last.size(), searchid);
    for (unsigned int i = 0; i < g_items.size(); i++)
    {
        if (s[0] == '\0' || std::isspace(s.back()) || StrStrIA(g_items[i].name.data(), last.data()) || g_items[i].id == searchid)
        {
            if (g_items[i].id == 0 && s[0] != '\0')
                continue;
            g_filtered_items[count++] = i;
        }
    }
    g_filtered_count = count;
    g_current_item = 0;
    scroll_top = true;
}

void set_selected_type(uint32_t id)
{
    update_filter("");
    for (int j = 0; j < g_filtered_count; ++j)
    {
        if (g_items[g_filtered_items[j]].id == id)
        {
            g_current_item = j;
        }
    }
    g_last_type = id;
}

void toggle_lights()
{
    if (options["lights"] && g_state->illumination)
    {
        g_state->illumination->enabled = true;
    }
    else if (!options["lights"] && g_state->illumination)
    {
        g_state->illumination->layer = 0;
        if (test_flag(g_state->level_flags, 18)) // dark level
            g_state->illumination->enabled = false;
    }
}

void load_state(int slot)
{
    StateMemory* target = UI::get_save_state(slot);
    if (!target)
        return;
    if (g_state->screen == 14 && target->screen != 14)
    {
        g_state->screen = 12;
        g_game_manager->journal_ui->fade_timer = 15;
        g_game_manager->journal_ui->state = 5;
        g_state->camera->focus_offset_x = 0;
        g_state->camera->focus_offset_y = 0;
        set_camera_bounds(true);
    }
    UI::load_state_as_main(slot);
}

void clear_script_messages()
{
    for (auto& [name, script] : g_scripts)
        script->consume_messages();
    for (auto& [name, script] : g_ui_scripts)
        script->consume_messages();
    g_Console->consume_messages();
    g_ConsoleMessages.clear();
}

bool process_keys(UINT nCode, WPARAM wParam, [[maybe_unused]] LPARAM lParam)
{
    ImGuiContext& g = *GImGui;
    int repeat = (lParam >> 30) & 1U;
    auto& io = ImGui::GetIO();
    ImGuiWindow* current = g.NavWindow;

    if (nCode == WM_KEYUP && !io.WantCaptureKeyboard && !g_bucket->io->WantCaptureKeyboard.value_or(false))
    {
        if (pressed("speedhack_turbo", wParam))
        {
            UI::speedhack(g_speedhack_old_multiplier);
            g_speedhack_old_multiplier = 1.0f;
        }
        else if (pressed("speedhack_slow", wParam))
        {
            UI::speedhack(g_speedhack_old_multiplier);
            g_speedhack_old_multiplier = 1.0f;
        }
        else if (pressed("peek_layer", wParam) && peek_layer)
        {
            peek_layer = false;
            g_state->layer_transition_timer = 15;
            g_state->transition_to_layer = (g_state->camera_layer + 1) % 2;
            g_state->camera_layer = g_state->transition_to_layer;
            UI::set_camera_layer_control_enabled(!peek_layer);
        }
    }

    if (nCode != WM_KEYDOWN && nCode != WM_SYSKEYDOWN)
    {
        return false;
    }

    g_speedhack_ui_multiplier = UI::get_speedhack();

    if (current != nullptr && current == ImGui::FindWindowByName("KeyCapture"))
        return false;

    if (g_Console && g_Console->is_toggled())
    {
        if (pressed("console", wParam) || pressed("console_alt", wParam) || pressed("close_console", wParam))
        {
            g_Console->toggle();
        }
        return false;
    }
    else if (pressed("escape", wParam))
    {
        if (current != nullptr)
        {
            escape();
            return false;
        }
        return true;
    }

    if (g_bucket->io->WantCaptureKeyboard.value_or(false))
        return false;

    if (pressed("hide_ui", wParam))
    {
        hide_ui = !hide_ui;
    }
    else if (pressed("switch_ui", wParam))
    {
        options["menu_ui"] ^= true;
        hide_ui = false;
    }
    else if (pressed("tool_options", wParam))
    {
        toggle("tool_options");
    }

    if (!options["keyboard_control"])
        return false;

    if (pressed("tool_entity", wParam))
    {
        if (toggle("tool_entity"))
        {
            set_focus_entity = true;
        }
    }
    else if (pressed("tool_door", wParam))
    {
        if (toggle("tool_door"))
        {
            set_focus_world = true;
        }
    }
    else if (pressed("tool_camera", wParam))
    {
        toggle("tool_camera");
    }
    else if (pressed("tool_entity_properties", wParam))
    {
        if (toggle("tool_entity_properties"))
        {
            set_focus_uid = true;
        }
    }
    else if (pressed("tool_game_properties", wParam))
    {
        toggle("tool_game_properties");
    }
    else if (pressed("tool_debug", wParam))
    {
        toggle("tool_debug");
    }
    else if (pressed("tool_script", wParam))
    {
        toggle("tool_script");
    }
    else if (pressed("tool_save", wParam))
    {
        toggle("tool_save");
    }
    else if (pressed("tool_finder", wParam))
    {
        if (toggle("tool_finder"))
        {
            set_focus_finder = true;
        }
    }
    else if (pressed("tool_keys", wParam))
    {
        toggle("tool_keys");
    }
    else if (pressed("tool_style", wParam))
    {
        toggle("tool_style");
    }
    else if (pressed("tool_metrics", wParam))
    {
        show_app_metrics = !show_app_metrics;
    }
    else if (pressed("move_up", wParam) && active("tool_entity") && io.WantCaptureKeyboard)
    {
        g_current_item = std::min(std::max(g_current_item - 1, 0), g_filtered_count - 1);
        scroll_to_entity = true;
    }
    else if (pressed("move_down", wParam) && active("tool_entity") && io.WantCaptureKeyboard)
    {
        g_current_item = std::min(std::max(g_current_item + 1, 0), g_filtered_count - 1);
        scroll_to_entity = true;
    }
    else if (pressed("move_pageup", wParam) && active("tool_entity"))
    {
        int page = std::max((int)((current->Size.y - 100) / ImGui::GetTextLineHeightWithSpacing() / 2), 1);
        g_current_item = std::min(std::max(g_current_item - page, 0), g_filtered_count - 1);
        scroll_to_entity = true;
    }
    else if (pressed("move_pagedown", wParam) && active("tool_entity"))
    {
        int page = std::max((int)((current->Size.y - 100) / ImGui::GetTextLineHeightWithSpacing() / 2), 1);
        g_current_item = std::min(std::max(g_current_item + page, 0), g_filtered_count - 1);
        scroll_to_entity = true;
    }
    else if (pressed("enter", wParam) && focused("tool_entity"))
    {
        spawn_entities(false);
        return true;
    }
    else if (pressed("move_up", wParam) && focused("tool_entity_properties"))
    {
        g_last_id++;
        edit_last_id = true;
    }
    else if (pressed("move_down", wParam) && focused("tool_entity_properties"))
    {
        g_last_id--;
        edit_last_id = true;
    }
    else if (pressed("enter", wParam) && focused("tool_finder"))
    {
        run_finder = true;
        return true;
    }
    else if (pressed("move_up", wParam) && focused("tool_door"))
    {
        g_to = static_cast<uint8_t>(std::min(std::max(g_to - 1, 0), 15));
    }
    else if (pressed("move_down", wParam) && focused("tool_door"))
    {
        g_to = static_cast<uint8_t>(std::min(std::max(g_to + 1, 0), 15));
    }
    else if (pressed("enter", wParam) && focused("tool_door"))
    {
        warp_inc(g_world, g_level, g_to + 1);
    }

    if (io.WantCaptureKeyboard)
        return false;

    if (pressed("zoom_out", wParam))
    {
        if (g_zoom == 0.0f)
            g_zoom = UI::get_zoom_level();
        g_zoom += 1.0f;
        set_zoom();
    }
    else if (pressed("zoom_in", wParam))
    {
        if (g_zoom == 0.0f)
            g_zoom = UI::get_zoom_level();
        g_zoom -= 1.0f;
        set_zoom();
    }
    else if (pressed("zoom_reset", wParam))
    {
        g_zoom = 13.5f;
        UI::zoom_reset();
    }
    else if (pressed("zoom_2x", wParam))
    {
        g_zoom = 16.324f;
        set_zoom();
    }
    else if (pressed("zoom_3x", wParam))
    {
        g_zoom = 23.126f;
        set_zoom();
    }
    else if (pressed("zoom_4x", wParam))
    {
        g_zoom = 29.928f;
        set_zoom();
    }
    else if (pressed("zoom_5x", wParam))
    {
        g_zoom = 36.66f;
        set_zoom();
    }
    else if (pressed("zoom_auto", wParam))
    {
        g_zoom = 0.0f;
        set_zoom();
    }
    else if (pressed("toggle_uncapped_fps", wParam))
    {
        if (g_engine_fps != 60.0)
            g_engine_fps = 60.0;
        else
            g_engine_fps = 0;
        update_frametimes();
    }
    else if (pressed("respawn", wParam))
    {
        respawn();
    }
    else if (pressed("toggle_godmode", wParam))
    {
        options["god_mode"] = !options["god_mode"];
        UI::godmode(options["god_mode"]);
        if (options["god_mode"])
        {
            for (auto ent : g_players)
                ent->lock_input_timer = 0;
        }
    }
    else if (pressed("toggle_noclip", wParam))
    {
        options["noclip"] = !options["noclip"];
        toggle_noclip();
    }
    else if (pressed("toggle_flymode", wParam))
    {
        options["fly_mode"] = !options["fly_mode"];
    }
    else if (pressed("toggle_speedhack_auto", wParam))
    {
        options["speedhack"] = !options["speedhack"];
    }
    else if (pressed("toggle_hitboxes", wParam))
    {
        options["draw_hitboxes"] = !options["draw_hitboxes"];
    }
    else if (pressed("toggle_entity_info", wParam))
    {
        options["draw_entity_info"] = !options["draw_entity_info"];
    }
    else if (pressed("toggle_entity_tooltip", wParam))
    {
        options["draw_entity_tooltip"] = !options["draw_entity_tooltip"];
    }
    else if (pressed("toggle_grid", wParam))
    {
        options["draw_grid"] = !options["draw_grid"];
    }
    else if (pressed("toggle_path", wParam))
    {
        options["draw_path"] = !options["draw_path"];
    }
    else if (pressed("toggle_mouse", wParam))
    {
        options["mouse_control"] = !options["mouse_control"];
    }
    else if (pressed("toggle_snap", wParam))
    {
        options["snap_to_grid"] = !options["snap_to_grid"];
    }
    else if (pressed("toggle_pause", wParam))
    {
        toggle_pause();
    }
    else if (pressed("toggle_hud", wParam))
    {
        options["draw_hud"] = !options["draw_hud"];
    }
    else if (pressed("toggle_hotbar", wParam))
    {
        options["draw_hotbar"] = !options["draw_hotbar"];
    }
    else if (pressed("frame_advance", wParam) || pressed("frame_advance_alt", wParam))
    {
        frame_advance();
    }
    else if (pressed("toggle_disable_pause", wParam))
    {
        options["disable_pause"] = !options["disable_pause"];
        force_hud_flags();
    }
    else if (pressed("toggle_lights", wParam))
    {
        options["lights"] = !options["lights"];
        toggle_lights();
    }
    else if (pressed("toggle_ghost", wParam))
    {
        options["disable_ghost_timer"] = !options["disable_ghost_timer"];
        UI::set_time_ghost_enabled(!options["disable_ghost_timer"]);
        UI::set_time_jelly_enabled(!options["disable_ghost_timer"]);
        UI::set_cursepot_ghost_enabled(!options["disable_ghost_timer"]);
    }
    else if (pressed("toggle_void", wParam))
    {
        g_ui_scripts["void"]->set_enabled(!g_ui_scripts["void"]->is_enabled());
        g_state->quest_flags = 1;
        g_state->loading = 1;
    }
    else if (pressed("import_void", wParam))
    {
        import_void();
    }
    else if (pressed("export_void", wParam))
    {
        export_void();
    }
    else if (pressed("spawn_layer_door", wParam))
    {
        UI::spawn_backdoor(0.0, 0.0);
    }
    else if (pressed("camera_left", wParam))
    {
        g_state->camera->focused_entity_uid = -1;
        if (enable_camera_bounds)
        {
            g_state->camera->focus_x = g_state->camera->adjusted_focus_x;
            g_state->camera->focus_y = g_state->camera->adjusted_focus_y;
        }
        set_camera_bounds(false);
        g_state->camera->focus_x -= g_camera_speed;
        if (g_state->pause != 0 || !options["smooth_camera"])
            g_state->camera->adjusted_focus_x = g_state->camera->focus_x;
    }
    else if (pressed("camera_right", wParam))
    {
        g_state->camera->focused_entity_uid = -1;
        if (enable_camera_bounds)
        {
            g_state->camera->focus_x = g_state->camera->adjusted_focus_x;
            g_state->camera->focus_y = g_state->camera->adjusted_focus_y;
        }
        set_camera_bounds(false);
        g_state->camera->focus_x += g_camera_speed;
        if (g_state->pause != 0 || !options["smooth_camera"])
            g_state->camera->adjusted_focus_x = g_state->camera->focus_x;
    }
    else if (pressed("camera_up", wParam))
    {
        g_state->camera->focused_entity_uid = -1;
        if (enable_camera_bounds)
        {
            g_state->camera->focus_x = g_state->camera->adjusted_focus_x;
            g_state->camera->focus_y = g_state->camera->adjusted_focus_y;
        }
        set_camera_bounds(false);
        g_state->camera->focus_y += g_camera_speed;
        if (g_state->pause != 0 || !options["smooth_camera"])
            g_state->camera->adjusted_focus_y = g_state->camera->focus_y;
    }
    else if (pressed("camera_down", wParam))
    {
        g_state->camera->focused_entity_uid = -1;
        if (enable_camera_bounds)
        {
            g_state->camera->focus_x = g_state->camera->adjusted_focus_x;
            g_state->camera->focus_y = g_state->camera->adjusted_focus_y;
        }
        set_camera_bounds(false);
        g_state->camera->focus_y -= g_camera_speed;
        if (g_state->pause != 0 || !options["smooth_camera"])
            g_state->camera->adjusted_focus_y = g_state->camera->focus_y;
    }
    else if (pressed("camera_reset", wParam))
    {
        set_camera_bounds(true);
        if (g_players.size() > 0)
            g_state->camera->focused_entity_uid = g_players.at(0)->uid;
    }
    else if (pressed("spawn_entity", wParam))
    {
        spawn_entities(false);
    }
    else if (pressed("spawn_kit_1", wParam))
    {
        if (kits.size() > 0)
            spawn_kit(kits.at(0));
    }
    else if (pressed("spawn_kit_2", wParam))
    {
        if (kits.size() > 1)
            spawn_kit(kits.at(1));
    }
    else if (pressed("spawn_kit_3", wParam))
    {
        if (kits.size() > 2)
            spawn_kit(kits.at(2));
    }
    else if (pressed("spawn_kit_4", wParam))
    {
        if (kits.size() > 3)
            spawn_kit(kits.at(3));
    }
    else if (pressed("spawn_kit_5", wParam))
    {
        if (kits.size() > 4)
            spawn_kit(kits.at(4));
    }
    else if (pressed("spawn_kit_6", wParam))
    {
        if (kits.size() > 5)
            spawn_kit(kits.at(5));
    }
    else if (pressed("spawn_kit_7", wParam))
    {
        if (kits.size() > 6)
            spawn_kit(kits.at(6));
    }
    else if (pressed("spawn_kit_8", wParam))
    {
        if (kits.size() > 7)
            spawn_kit(kits.at(7));
    }
    else if (pressed("spawn_kit_9", wParam))
    {
        if (kits.size() > 8)
            spawn_kit(kits.at(8));
    }
    else if (pressed("hotbar_1", wParam))
    {
        if (hotbar.contains(0))
            set_selected_type(hotbar[0]);
    }
    else if (pressed("hotbar_2", wParam))
    {
        if (hotbar.contains(1))
            set_selected_type(hotbar[1]);
    }
    else if (pressed("hotbar_3", wParam))
    {
        if (hotbar.contains(2))
            set_selected_type(hotbar[2]);
    }
    else if (pressed("hotbar_4", wParam))
    {
        if (hotbar.contains(3))
            set_selected_type(hotbar[3]);
    }
    else if (pressed("hotbar_5", wParam))
    {
        if (hotbar.contains(4))
            set_selected_type(hotbar[4]);
    }
    else if (pressed("hotbar_6", wParam))
    {
        if (hotbar.contains(5))
            set_selected_type(hotbar[5]);
    }
    else if (pressed("hotbar_7", wParam))
    {
        if (hotbar.contains(6))
            set_selected_type(hotbar[6]);
    }
    else if (pressed("hotbar_8", wParam))
    {
        if (hotbar.contains(7))
            set_selected_type(hotbar[7]);
    }
    else if (pressed("hotbar_9", wParam))
    {
        if (hotbar.contains(8))
            set_selected_type(hotbar[8]);
    }
    else if (pressed("hotbar_0", wParam))
    {
        if (hotbar.contains(9))
            set_selected_type(hotbar[9]);
    }
    else if (pressed("spawn_warp_door", wParam))
    {
        int spawned = UI::spawn_door(0.0, 0.0, g_world, g_level, g_to + 1);
        if (!lock_entity)
            g_last_id = spawned;
    }
    else if (pressed("warp", wParam))
    {
        warp_inc(g_world, g_level, g_to + 1);
    }
    else if (pressed("warp_next_level_a", wParam))
    {
        warp_next_level(0);
    }
    else if (pressed("warp_next_level_b", wParam))
    {
        warp_next_level(1);
    }
    else if (pressed("reset_windows", wParam))
    {
        reset_windows();
    }
    else if (pressed("detach_tab", wParam))
    {
        detach(active_tab);
    }
    else if (pressed("save_settings", wParam))
    {
        ImGui::SaveIniSettingsToDisk(inifile);
        save_config(cfgfile);
    }
    else if (pressed("load_settings", wParam))
    {
        ImGui::LoadIniSettingsFromDisk(inifile);
        load_config(cfgfile);
        refresh_script_files();
        set_colors();
    }
    else if (pressed("reload_enabled_scripts", wParam))
    {
        reload_enabled_scripts();
    }
    else if (pressed("console", wParam) || pressed("console_alt", wParam))
    {
        g_Console->toggle();
    }
    else if (pressed("change_layer", wParam))
    {
        if (g_players.size() > 0)
        {
            auto layer_to = LAYER::FRONT;
            if (g_players.at(0)->layer == 0)
                layer_to = LAYER::BACK;
            g_players.at(0)->topmost_mount()->set_layer(layer_to);
            if (layer_to == LAYER::BACK || !g_state->illumination)
            {
                g_players.at(0)->emitted_light->enabled = true;
            }
            else
            {
                g_players.at(0)->emitted_light->enabled = false;
            }
        }
    }
    else if (pressed("quick_start", wParam))
    {
        if (g_state->screen < 12)
            quick_start(12, 1, 1, 1);
    }
    else if (pressed("quick_restart", wParam))
    {
        if (g_state->screen > 11)
        {
            if (g_state->quest_flags & 0x40)
            {
                std::random_device rd;
                static std::uniform_int_distribution<uint32_t> dist(1, UINT32_MAX);
                quick_start(12, 1, 1, 1, dist(rd));
            }
            else
            {
                quick_start(12, g_state->world_start, g_state->level_start, g_state->theme_start);
            }
        }
        else
        {
            quick_start(12, 1, 1, 1);
        }
    }
    else if (pressed("quick_restart_seed", wParam))
    {
        restart_adventure();
    }
    else if (pressed("quick_camp", wParam))
    {
        quick_start(11, 1, 1, 1);
    }
    else if (pressed("speedhack_increase", wParam))
    {
        if (should_speedhack())
            options["speedhack"] = false;
        g_speedhack_ui_multiplier += 0.1f;
        if (g_speedhack_ui_multiplier > 10.f)
            g_speedhack_ui_multiplier = 10.f;
        UI::speedhack(g_speedhack_ui_multiplier);
    }
    else if (pressed("speedhack_decrease", wParam))
    {
        if (should_speedhack())
            options["speedhack"] = false;
        g_speedhack_ui_multiplier -= 0.1f;
        if (g_speedhack_ui_multiplier < 0.1f)
            g_speedhack_ui_multiplier = 0.1f;
        UI::speedhack(g_speedhack_ui_multiplier);
    }
    else if (pressed("speedhack_10pct", wParam))
    {
        if (should_speedhack())
            options["speedhack"] = false;
        UI::speedhack(0.1f);
    }
    else if (pressed("speedhack_20pct", wParam))
    {
        if (should_speedhack())
            options["speedhack"] = false;
        UI::speedhack(0.2f);
    }
    else if (pressed("speedhack_30pct", wParam))
    {
        if (should_speedhack())
            options["speedhack"] = false;
        UI::speedhack(0.3f);
    }
    else if (pressed("speedhack_40pct", wParam))
    {
        if (should_speedhack())
            options["speedhack"] = false;
        UI::speedhack(0.4f);
    }
    else if (pressed("speedhack_50pct", wParam))
    {
        if (should_speedhack())
            options["speedhack"] = false;
        UI::speedhack(0.5f);
    }
    else if (pressed("speedhack_60pct", wParam))
    {
        if (should_speedhack())
            options["speedhack"] = false;
        UI::speedhack(0.6f);
    }
    else if (pressed("speedhack_70pct", wParam))
    {
        if (should_speedhack())
            options["speedhack"] = false;
        UI::speedhack(0.7f);
    }
    else if (pressed("speedhack_80pct", wParam))
    {
        if (should_speedhack())
            options["speedhack"] = false;
        UI::speedhack(0.8f);
    }
    else if (pressed("speedhack_90pct", wParam))
    {
        if (should_speedhack())
            options["speedhack"] = false;
        UI::speedhack(0.9f);
    }
    else if (pressed("speedhack_normal", wParam))
    {
        if (should_speedhack())
            options["speedhack"] = false;
        UI::speedhack(1.0f);
    }
    else if (pressed("speedhack_turbo", wParam) && !repeat)
    {
        if (should_speedhack())
            options["speedhack"] = false;
        g_speedhack_old_multiplier = g_speedhack_ui_multiplier;
        UI::speedhack(5.f); // TODO: configurable
    }
    else if (pressed("speedhack_slow", wParam) && !repeat)
    {
        if (should_speedhack())
            options["speedhack"] = false;
        g_speedhack_old_multiplier = g_speedhack_ui_multiplier;
        UI::speedhack(0.2f); // TODO: configurable
    }
    else if (pressed("destroy_grabbed", wParam))
    {
        auto selected = get_entity_ptr(g_last_id);
        if (selected)
            smart_delete(selected, true);
        if (!lock_entity)
            g_last_id = -1;
    }
    else if (pressed("destroy_selected", wParam))
    {
        for (auto selected_uid : g_selected_ids)
        {
            auto ent = get_entity_ptr(selected_uid);
            if (ent)
                smart_delete(ent, false);
        }
        g_selected_ids.clear();
    }
    else if (pressed("peek_layer", wParam) && !repeat)
    {
        peek_layer = true;
        UI::set_camera_layer_control_enabled(!peek_layer);
        g_state->layer_transition_timer = 15;
        g_state->transition_to_layer = (g_state->camera_layer + 1) % 2;
        g_state->camera_layer = g_state->transition_to_layer;
    }
    else if (pressed("save_state_1", wParam))
    {
        UI::save_main_state(1);
    }
    else if (pressed("save_state_2", wParam))
    {
        UI::save_main_state(2);
    }
    else if (pressed("save_state_3", wParam))
    {
        UI::save_main_state(3);
    }
    else if (pressed("save_state_4", wParam))
    {
        UI::save_main_state(4);
    }
    else if (pressed("load_state_1", wParam))
    {
        load_state(1);
    }
    else if (pressed("load_state_2", wParam))
    {
        load_state(2);
    }
    else if (pressed("load_state_3", wParam))
    {
        load_state(3);
    }
    else if (pressed("load_state_4", wParam))
    {
        load_state(4);
    }
    else if (pressed("clear_messages", wParam))
    {
        clear_script_messages();
    }
    else
    {
        return false;
    }
    return true;
}

void tooltip(const char* tip, bool force = false)
{
    if (!options["show_tooltips"] && !force)
        return;
    if (ImGui::IsItemHovered() || force)
    {
        auto base = ImGui::GetMainViewport();
        ImGui::SetNextWindowViewport(base->ID);
        ImGui::SetTooltip("%s", tip);
    }
}

void tooltip(const char* tip, const char* key)
{
    if (!options["show_tooltips"])
        return;
    if (ImGui::IsItemHovered())
    {
        auto base = ImGui::GetMainViewport();
        ImGui::SetNextWindowViewport(base->ID);
        if (key && keys[key])
        {
            ImGui::SetTooltip("(%s) %s", key_string(keys[key]).c_str(), tip);
        }
        else
        {
            ImGui::SetTooltip("%s", tip);
        }
    }
}

void indent(float x)
{
    if (!in_menu)
        ImGui::Indent(x);
}

void unindent(float x)
{
    if (!in_menu)
        ImGui::Unindent(x);
}

bool submenu(const char* title)
{
    if (in_menu)
    {
        ImGui::SetNextWindowSizeConstraints({300, 100}, {1000, -1});
        return ImGui::BeginMenu(title);
    }
    else
        return ImGui::CollapsingHeader(title);
}

void endmenu()
{
    if (in_menu)
        ImGui::EndMenu();
}

bool render_uid(int uid, const char* section, bool rembtn = false)
{
    std::string uidc = std::to_string(uid);
    auto ptype = UI::get_entity_type(uid);
    if (ptype == 0 || ptype == UINT32_MAX)
        return false;
    std::string typec = std::to_string(ptype);
    std::string pname = entity_names[ptype];
    ImGui::PushID(section);
    if (ImGui::Button(uidc.c_str()))
    {
        g_last_id = uid;
        update_entity();
    }
    ImGui::SameLine();
    ImGui::Text("%s", typec.c_str());
    ImGui::SameLine();
    ImGui::Text("%s", pname.c_str());
    auto remove = false;
    if (rembtn)
    {
        ImGui::SameLine();
        ImGui::PushID(uid);
        remove = ImGui::Button("Remove##RemoveItem");
        ImGui::PopID();
    }
    ImGui::PopID();
    return remove;
}

void render_light(const char* name, LightParams* light)
{
    ImGui::PushID(name);
    float color[4] = {light->red, light->green, light->blue};
    if (ImGui::ColorEdit3(name, color))
    {
        light->red = color[0];
        light->green = color[1];
        light->blue = color[2];
    }
    ImGui::DragFloat("Size##LightSize", &light->size, 0.5f);
    ImGui::PopID();
}

template <std::size_t SIZE, typename T>
void render_flags(const std::array<const char*, SIZE> names_array, T* flag_field, bool show_number = true)
{
    for (int idx{0}; idx < SIZE && idx < sizeof(T) * 8; ++idx)
    {
        // just simplified version of CheckboxFlagsT

        T value = (T)std::pow(2, idx);
        bool on = (*flag_field & value) == value;

        if (names_array[idx][0] != '\0' &&
            ImGui::Checkbox(show_number ? fmt::format("{}: {}", idx + 1, names_array[idx]).c_str() : names_array[idx], &on))
        {
            *flag_field ^= value;
        }
    }
}

template <std::size_t SIZE, typename T>
void render_flag(int idx, const std::array<const char*, SIZE> names_array, T* flag_field, bool show_number = true)
{
    T value = (T)std::pow(2, idx);
    bool on = (*flag_field & value) == value;

    if (names_array[idx][0] != '\0' &&
        ImGui::Checkbox(show_number ? fmt::format("{}: {}", idx + 1, names_array[idx]).c_str() : names_array[idx], &on))
    {
        *flag_field ^= value;
    }
}

void render_illumination(Illumination* light, const char* sect = "")
{
    ImGui::PushID(sect);
    if (light->entity_uid != -1)
        render_uid(light->entity_uid, sect);
    ImGui::Checkbox("Enabled##LightEnabled", &light->enabled);
    ImGui::Checkbox("Back layer##LightLayer", (bool*)&light->layer);
    render_light("Light 1##Light1", &light->light1);
    render_light("Light 2##Light2", &light->light2);
    render_light("Light 3##Light3", &light->light3);
    render_light("Light 4##Light4", &light->light4);
    ImGui::DragFloat("Brightness##LightBrightness", &light->brightness, 0.01f);
    ImGui::DragFloat("Brightness multiplier##LightBrightnessMulti", &light->brightness_multiplier, 0.01f);
    ImGui::InputFloat("Position X##LightPosX", &light->light_pos_x);
    ImGui::InputFloat("Position Y##LightPosY", &light->light_pos_y);
    ImGui::InputFloat("Offset X##LightPosX", &light->offset_x);
    ImGui::InputFloat("Offset Y##LightPosY", &light->offset_y);
    ImGui::DragFloat("Distortion##LightDistortion", &light->distortion, 0.01f);
    render_flags(illumination_flags, &light->flags);
    ImGui::PopID();
}

void render_liquid_pool(int i)
{
    ImGui::PushID(i);
    if (submenu(liquid_pool_names[i]))
    {
        auto engine = g_state->liquid_physics->pools[i].physics_engine;
        auto defaults = g_state->liquid_physics->pools[i].physics_defaults;
        if (engine)
        {
            ImGui::Text("Entity count: %u/%u", engine->entity_count, engine->allocated_size);
            ImGui::Checkbox("Pause physics##LiquidEnginePause", &engine->pause_physics);
            ImGui::DragFloat("Gravity##LiquidEngineGravity", &engine->gravity, 0.01f, -5.f, 5.f);
            ImGui::DragFloat("Elasticity##LiquidEngineElasticity", &engine->agitation, 0.01f, -1.f, 5.f);
        }
        ImGui::DragFloat("Default gravity##LiquidDefaultGravity", &defaults.gravity, 0.01f, -5.f, 5.f);
        ImGui::DragFloat("Default elasticity##LiquidDefaultElasticity", &defaults.agitation, 0.01f, -1.f, 5.f);
        endmenu();
    }
    ImGui::PopID();
}

void render_themes()
{
    // ImGui::ListBox with filter
    if (!ImGui::BeginCombo("Theme##Theme", themes[g_to]))
        return;
    for (uint8_t i = 0; i < 17; i++)
    {
        const bool item_selected = (i == g_to);
        const char* item_text = themes[i];

        ImGui::PushID(i);
        if (ImGui::Selectable(item_text, item_selected))
        {
            g_to = i;
        }
        if (item_selected)
            ImGui::SetItemDefaultFocus();
        ImGui::PopID();
    }
    ImGui::EndCombo();
}

const char* theme_name(int theme)
{
    if (theme < 1 || theme > 17)
        return "Crash City";
    return themes_short[theme - 1];
}

void render_narnia()
{
    if (submenu("Other game screens"))
    {
        int screen = -1;
        ImGui::PushID("WarpSpecial");
        for (unsigned int i = 0; i < 21; ++i)
        {
            if ((i >= 5 && i <= 10))
                continue;
            if (options["menu_ui"])
            {
                if (ImGui::MenuItem(screen_names[i]))
                    screen = i;
            }
            else
            {
                if (i % 2)
                    ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.5f);
                if (ImGui::Button(screen_names[i], ImVec2(ImGui::GetContentRegionMax().x * 0.5f, 0)))
                    screen = i;
            }
        }
        endmenu();
        if (screen != -1)
        {
            if (screen == 14)
            {
                if (g_state->screen == 11 or g_state->screen == 12)
                    UI::load_death_screen();
            }
            else if (g_state->screen != 12 && screen >= 11)
            {
                quick_start((uint8_t)screen, 1, 1, 1);
            }
            else
            {
                g_state->screen_next = screen;
                g_state->loading = 1;
            }
            if (screen >= 16 && screen <= 18)
            {
                g_state->win_state = 1;
                if (!g_state->end_spaceship_character)
                    g_state->end_spaceship_character = to_id("ENT_TYPE_CHAR_EGGPLANT_CHILD");
            }
            if (screen == 19)
            {
                g_state->world_next = 8;
                g_state->level_next = 99;
                g_state->theme_next = 10;
                if (!g_state->level_gen->theme_cosmicocean->sub_theme)
                    g_state->level_gen->theme_cosmicocean->sub_theme = g_state->level_gen->theme_dwelling;
                g_state->current_theme = g_state->level_gen->theme_cosmicocean;
                g_state->win_state = 3;
                if (g_state->level_count < 1)
                    g_state->level_count = 1;
            }
        }
        ImGui::PopID();
    }

    ImGui::Text("Next level");
    ImGui::SameLine(100.0f);

    if (g_state->screen != 13)
    {
        int n = 0;

        uint8_t target_world = 0;
        uint8_t target_level = 0;
        uint8_t target_theme = 0;
        int tnum = g_state->world * 100 + g_state->level;
        switch (tnum)
        {
        case 104:
        case 301:
            break;
        case 501:
            target_world = 6;
            target_level = 1;
            target_theme = 8;
            break;
        case 204:
            target_world = 3;
            target_level = 1;
            target_theme = 4;
            break;
        case 403:
            if (g_state->theme == 6 || g_state->theme == 11)
            {
                target_world = 4;
                target_level = 4;
                target_theme = 6;
            }
            else
            {
                target_world = 4;
                target_level = 4;
                target_theme = 5;
            }
            break;
        case 404:
            target_world = 5;
            target_level = 1;
            target_theme = 7;
            break;
        case 603:
            target_world = 6;
            target_level = 4;
            target_theme = 14;
            break;
        case 604:
            target_world = 7;
            target_level = 1;
            target_theme = 9;
            break;
        case 702:
            target_world = 7;
            target_level = 3;
            target_theme = 9;
            break;
        case 703:
            target_world = 7;
            target_level = 4;
            target_theme = 16;
            break;
        case 704:
            target_world = 8;
            target_level = 5;
            target_theme = 10;
            break;
        default:
            target_world = g_state->world;
            target_level = g_state->level + 1;
            target_theme = g_state->theme;
            break;
        }
        if (g_state->theme == 17)
        {
            target_world = 1;
            target_level = 1;
            target_theme = 1;
        }
        if (target_world > 0)
        {
            std::string buf = fmt::format("{}-{} {}", target_world, target_level, theme_name(target_theme));
            if (ImGui::Button(buf.c_str()))
            {
                warp_inc(target_world, target_level, target_theme);
            }
            n++;
        }

        for (auto doorid : UI::get_entities_by({(ENT_TYPE)CUSTOM_TYPE::EXITDOOR}, ENTITY_MASK::FLOOR, LAYER::BOTH))
        {
            ExitDoor* target = get_entity_ptr(doorid)->as<ExitDoor>();
            if (!target->special_door)
                continue;
            std::string buf = fmt::format("{}-{} {}", target->world, target->level, theme_name(target->theme));
            if (n > 0)
                ImGui::SameLine();
            if (ImGui::Button(buf.c_str()))
            {
                warp_inc(target->world, target->level, target->theme);
            }
            n++;
        }

        if (g_state->theme == 11)
        {
            target_world = 4;
            target_level = 4;
            target_theme = 12;

            std::string buf = fmt::format("{}-{} {}", target_world, target_level, theme_name(target_theme));
            ImGui::SameLine();
            if (ImGui::Button(buf.c_str()))
            {
                warp_inc(target_world, target_level, target_theme);
            }
            n++;
        }
        if (n == 0)
        {
            target_world = g_state->world;
            target_level = g_state->level + 1;
            target_theme = g_state->theme;
            std::string buf = fmt::format("{}-{} {}", target_world, target_level, theme_name(target_theme));
            if (ImGui::Button(buf.c_str()))
                warp_inc(target_world, target_level, target_theme);
        }
    }
    else
    {
        std::string buf = fmt::format("{}-{} {}", g_state->world_next, g_state->level_next, theme_name(g_state->theme_next));
        ImGui::SameLine(100.0f);
        if (ImGui::Button(buf.c_str()))
        {
            warp_inc(g_state->world_next, g_state->level_next, g_state->theme_next);
        }
    }

    ImGui::Text("Dwelling");
    ImGui::SameLine(100.0f);
    if (ImGui::Button("1-1##Warp1-1"))
        warp_inc(1, 1, 1);
    ImGui::SameLine();
    if (ImGui::Button("1-2##Warp1-2"))
        warp_inc(1, 2, 1);
    ImGui::SameLine();
    if (ImGui::Button("1-3##Warp1-3"))
        warp_inc(1, 3, 1);
    ImGui::SameLine();
    if (ImGui::Button("1-4##Warp1-4"))
        warp_inc(1, 4, 1);

    ImGui::Text("Jungle");
    ImGui::SameLine(100.0f);
    if (ImGui::Button("2-1##WarpJ2-1"))
        warp_inc(2, 1, 2);
    ImGui::SameLine();
    if (ImGui::Button("2-2##WarpJ2-2"))
        warp_inc(2, 2, 2);
    ImGui::SameLine();
    if (ImGui::Button("2-3##WarpJ2-3"))
        warp_inc(2, 3, 2);
    ImGui::SameLine();
    if (ImGui::Button("2-4##WarpJ2-4"))
        warp_inc(2, 4, 2);

    ImGui::Text("Volcana");
    ImGui::SameLine(100.0f);
    if (ImGui::Button("2-1##WarpV2-1"))
        warp_inc(2, 1, 3);
    ImGui::SameLine();
    if (ImGui::Button("2-2##WarpV2-2"))
        warp_inc(2, 2, 3);
    ImGui::SameLine();
    if (ImGui::Button("2-3##WarpV2-3"))
        warp_inc(2, 3, 3);
    ImGui::SameLine();
    if (ImGui::Button("2-4##WarpV2-4"))
        warp_inc(2, 4, 3);

    ImGui::Text("Olmec");
    ImGui::SameLine(100.0f);
    if (ImGui::Button("3-1##Warp3-1"))
        warp_inc(3, 1, 4);

    ImGui::Text("Tide Pool");
    ImGui::SameLine(100.0f);
    if (ImGui::Button("4-1##WarpP4-1"))
        warp_inc(4, 1, 5);
    ImGui::SameLine();
    if (ImGui::Button("4-2##WarpP4-2"))
        warp_inc(4, 2, 5);
    ImGui::SameLine();
    if (ImGui::Button("4-3##WarpP4-3"))
        warp_inc(4, 3, 5);
    ImGui::SameLine();
    if (ImGui::Button("4-4##WarpP4-4"))
        warp_inc(4, 4, 5);
    ImGui::SameLine();
    if (ImGui::Button("Abzu##WarpAbzu"))
        warp_inc(4, 4, 13);

    ImGui::Text("Temple");
    ImGui::SameLine(100.0f);
    if (ImGui::Button("4-1##WarpT4-1"))
        warp_inc(4, 1, 6);
    ImGui::SameLine();
    if (ImGui::Button("4-2##WarpT4-2"))
        warp_inc(4, 2, 6);
    ImGui::SameLine();
    if (ImGui::Button("4-3##WarpT4-3"))
        warp_inc(4, 3, 6);
    ImGui::SameLine();
    if (ImGui::Button("4-4##WarpT4-4"))
        warp_inc(4, 4, 6);
    ImGui::SameLine();
    if (ImGui::Button("CoG##WarpCoG"))
        warp_inc(4, 3, 11);
    ImGui::SameLine();
    if (ImGui::Button("Duat##WarpDuat"))
        warp_inc(4, 4, 12);

    ImGui::Text("Ice Caves");
    ImGui::SameLine(100.0f);
    if (ImGui::Button("5-1##Warp5-1"))
        warp_inc(5, 1, 7);

    ImGui::Text("Neo Babylon");
    ImGui::SameLine(100.0f);
    if (ImGui::Button("6-1##Warp6-1"))
        warp_inc(6, 1, 8);
    ImGui::SameLine();
    if (ImGui::Button("6-2##Warp6-2"))
        warp_inc(6, 2, 8);
    ImGui::SameLine();
    if (ImGui::Button("6-3##Warp6-3"))
        warp_inc(6, 3, 8);
    ImGui::SameLine();
    if (ImGui::Button("Tiamat##WarpTiamat"))
        warp_inc(6, 4, 14);

    ImGui::Text("Sunken City");
    ImGui::SameLine(100.0f);
    if (ImGui::Button("7-1##Warp7-1"))
        warp_inc(7, 1, 9);
    ImGui::SameLine();
    if (ImGui::Button("7-2##Warp7-2"))
        warp_inc(7, 2, 9);
    ImGui::SameLine();
    if (ImGui::Button("7-3##Warp7-3"))
        warp_inc(7, 3, 9);
    ImGui::SameLine();
    if (ImGui::Button("Hundun##WarpHundun"))
        warp_inc(7, 4, 16);
    ImGui::SameLine();
    if (ImGui::Button("EW##WarpEW"))
        warp_inc(7, 2, 15);

    ImGui::Text("Cosmic Ocean");
    ImGui::SameLine(100.0f);
    if (ImGui::Button("7-5##Warp7-5"))
        warp_inc(8, 5, 10);
    ImGui::SameLine();
    if (ImGui::Button("7-98##Warp7-98"))
        warp_inc(8, 98, 10);

    ImGui::Text("Base Camp");
    ImGui::SameLine(100.0f);
    if (ImGui::Button("Camp##WarpCamp"))
        warp_inc(1, 1, 17);

    ImGui::PushItemWidth(-ImGui::GetWindowWidth() * 0.5f);
    if (set_focus_world)
    {
        ImGui::SetKeyboardFocusHere();
        set_focus_world = false;
    }
    if (ImGui::DragScalar("World##WarpWorld", ImGuiDataType_U8, &g_world, 0.1f, &u8_one, &u8_seven)) {}
    if (ImGui::IsItemFocused())
        focused_tool = "tool_door";
    if (ImGui::DragScalar("Level##WarpLevel", ImGuiDataType_U8, &g_level, 0.1f, &u8_one, &u8_four)) {}
    if (ImGui::IsItemFocused())
        focused_tool = "tool_door";
    render_themes();
    ImGui::PopItemWidth();
    if (ImGui::Button("Instant warp##InstantWarp"))
    {
        warp_inc(g_world, g_level, g_to + 1);
    }
    tooltip("Warp to selected level, skipping transition.", "warp");
    ImGui::SameLine();
    if (ImGui::Button("Warp door##SpawnWarpDoor"))
    {
        int spawned = UI::spawn_door(g_x, g_y, g_world, g_level, g_to + 1);
        if (!lock_entity)
            g_last_id = spawned;
    }
    tooltip("Spawn an exit door to selected level.", "spawn_warp_door");
    ImGui::SameLine();
    if (ImGui::Button("Layer door##SpawnLayerDoor"))
    {
        UI::spawn_backdoor(g_x, g_y);
    }
    tooltip("Spawn a door to back layer.\nTip: You can instantly switch layers with (Shift+Tab).", "spawn_layer_door");
    ImGui::Checkbox("Warp increments level count normally", &options["warp_increments_level_count"]);
    tooltip("Simulate natural level progression when warping.");
    ImGui::Checkbox("Warp to transition in adventure mode", &options["warp_transition"]);
    tooltip("Simulate natural level progression even more.");
    if ((g_state->quest_flags & 0x40) == 0 && !options["warp_transition"])
        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Warning: Skipping transition in adventure mode\nwill result in different level generation.");
}

void set_camera_hack(bool enable)
{
    options["camera_hack"] = enable;
    if (g_ui_scripts.find("camera_hack") == g_ui_scripts.end())
        return;
    g_ui_scripts["camera_hack"]->set_enabled(options["camera_hack"]);
}

void render_camera()
{
    if (submenu("Focus and bounds"))
    {
        if (g_state->camera->focused_entity_uid != 0 && g_state->camera->focused_entity_uid != UINT32_MAX)
        {
            render_uid(g_state->camera->focused_entity_uid, "FocusedEntity");
            ImGui::SameLine(0, 4.0f);
            if (ImGui::Button("Unfocus"))
                g_state->camera->focused_entity_uid = -1;
            tooltip("Remove camera focus. Drag to move the camera around.", "mouse_camera_drag");
        }
        if (!g_players.empty())
        {
            if (g_state->camera->focused_entity_uid != g_players.at(0)->uid)
            {
                ImGui::SameLine(0, 4.0f);
                if (ImGui::Button("Focus player"))
                    g_state->camera->focused_entity_uid = g_players.at(0)->uid;
                tooltip("Focus the player.", "camera_reset");
            }
        }
        if (g_last_id != -1 && g_state->camera->focused_entity_uid != g_last_id)
        {
            ImGui::SameLine(0, 4.0f);
            if (ImGui::Button("Focus selected"))
                g_state->camera->focused_entity_uid = g_last_id;
            tooltip("Focus the selected entity");
        }

        auto [cx, cy] = Camera::get_position();
        ImGui::PushItemWidth(120.0f);
        ImGui::InputFloat("##CameraPosX", &cx, 0.1f, 1.0f);
        if (ImGui::IsItemEdited())
            HeapBase::get().state()->camera->set_position(cx, cy);
        ImGui::SameLine(0, 4.0f);
        ImGui::InputFloat("Position##CameraPosY", &cy, 0.1f, 1.0f);
        if (ImGui::IsItemEdited())
            HeapBase::get().state()->camera->set_position(cx, cy);

        ImGui::InputFloat("##CameraFocusX", &g_state->camera->focus_x, 0.1f, 1.0f);
        ImGui::SameLine(0, 4.0f);
        ImGui::InputFloat("Focus##CameraFocusY", &g_state->camera->focus_y, 0.1f, 1.0f);

        ImGui::InputFloat("##CameraAdjustedFocusX", &g_state->camera->adjusted_focus_x, 0.1f, 1.0f);
        ImGui::SameLine(0, 4.0f);
        ImGui::InputFloat("Adjusted Focus##CameraAdjustedFocusY", &g_state->camera->adjusted_focus_y, 0.1f, 1.0f);

        ImGui::InputFloat("##CameraCalculatedFocusX", &g_state->camera->calculated_focus_x, 0.1f, 1.0f);
        ImGui::SameLine(0, 4.0f);
        ImGui::InputFloat("Calculated Focus##CameraCalculatedFocusY", &g_state->camera->calculated_focus_y, 0.1f, 1.0f);

        ImGui::InputFloat("##CameraOffsetX", &g_state->camera->focus_offset_x, 0.1f, 1.0f);
        ImGui::SameLine(0, 4.0f);
        ImGui::InputFloat("Focus Offset##CameraOffsetY", &g_state->camera->focus_offset_y, 0.1f, 1.0f);
        ImGui::PopItemWidth();

        ImGui::PushItemWidth(244.0f);
        ImGui::InputFloat("Vertical Pan##CameraOffset", &g_state->camera->vertical_pan, 0.1f, 1.0f);
        ImGui::InputFloat4("Bounds##CameraBounds", &g_state->camera->bounds_left);
        ImGui::PopItemWidth();
        endmenu();
    }
    ImGui::PushItemWidth(-ImGui::GetContentRegionMax().x * 0.5f);
    if (ImGui::DragFloat("Zoom##ZoomLevel", &g_zoom, 0.5f, 0.5, 60.0, "%.2f"))
    {
        set_zoom();
    }
    tooltip("Zooming with the mouse wheel is easier.", "mouse_zoom_in");
    ImGui::PopItemWidth();
    if (ImGui::Button("Default"))
    {
        g_zoom = 13.5f;
        UI::zoom_reset();
    }
    tooltip("Default zoom level.", "zoom_reset");
    ImGui::SameLine();
    if (ImGui::Button("2x"))
    {
        g_zoom = 16.324f;
        set_zoom();
    }
    tooltip("2 room wide zoom level.", "zoom_2x");
    ImGui::SameLine();
    if (ImGui::Button("3x"))
    {
        g_zoom = 23.126f;
        set_zoom();
    }
    tooltip("3 room wide zoom level.", "zoom_3x");
    ImGui::SameLine();
    if (ImGui::Button("4x"))
    {
        g_zoom = 29.928f;
        set_zoom();
    }
    tooltip("4 room wide zoom level.", "zoom_4x");
    ImGui::SameLine();
    if (ImGui::Button("5x"))
    {
        g_zoom = 36.730f;
        set_zoom();
    }
    tooltip("5 room wide zoom level.", "zoom_5x");
    ImGui::SameLine();
    if (ImGui::Button("Auto"))
    {
        g_zoom = 0.0f;
        set_zoom();
    }
    tooltip("Automatically fit level width to screen.", "zoom_auto");
    if (ImGui::Checkbox("Enable default camera bounds##CameraBoundsLevel", &enable_camera_bounds))
        set_camera_bounds(enable_camera_bounds);
    tooltip("Disable to free the camera bounds in a level.\nAutomatically disabled when dragging.", "camera_reset");
    if (ImGui::Checkbox("Enable camp camera bounds##CameraBoundsCamp", &enable_camp_camera))
    {
        UI::set_camp_camera_bounds_enabled(enable_camp_camera);
        if (!enable_camp_camera && g_state->screen == 11)
        {
            g_state->camera->bounds_left = 0.5;
            g_state->camera->bounds_right = 74.5;
            g_state->camera->bounds_top = 124.5;
            g_state->camera->bounds_bottom = 56.5;
        }
    }
    tooltip("Disable to free the camera in camp.\nAutomatically disabled when zooming.");
    if (ImGui::Checkbox("Follow focused entity absolutely##CameraForcePlayer", &options["camera_hack"]))
    {
        set_camera_hack(options["camera_hack"]);
        enable_camera_bounds = !options["camera_hack"];
        set_camera_bounds(enable_camera_bounds);
        enable_camp_camera = !options["camera_hack"];
        UI::set_camp_camera_bounds_enabled(enable_camp_camera);
    }
    tooltip("Enable to always center the followed entity instantly\nwithout respecting level borders.");
    if (ImGui::Checkbox("Update camera position during pause##CameraPaused", &g_bucket->pause_api->update_camera))
        options["pause_update_camera"] = g_bucket->pause_api->update_camera;
    tooltip("Enable to follow the entity smoothly when paused\nor combine with speed=5 for instant camera that respects level borders.");
    static bool lock_inertia{false};
    if (ImGui::Checkbox("Lock current game camera speed##LockInertia", &lock_inertia))
    {
        if (lock_inertia)
            g_camera_inertia = g_state->camera->inertia;
        else
            g_camera_inertia = -FLT_MAX;
    }
    tooltip("Force selected game camera speed on the next level too.");

    ImGui::Checkbox("Smooth camera dragging", &options["smooth_camera"]);
    tooltip("Smooth camera movement when dragging, unless paused.");

    ImGui::PushItemWidth(0.25f * ImGui::GetContentRegionMax().x);
    if (ImGui::DragFloat("Game camera speed##CameraInertia", &g_state->camera->inertia, 0.1f, 0.1f, 5.0f))
    {
        if (lock_inertia)
            g_camera_inertia = g_state->camera->inertia;
        else
            g_camera_inertia = -FLT_MAX;
    }
    tooltip("Lower values moves slower and smoother,\nhigher values reduce lagging behind.\n5 = instantly move any distance");
    ImGui::SameLine();
    ImGui::Text("(%03d%%)", (int)(20.0f * g_state->camera->inertia));
    ImGui::SameLine();
    if (ImGui::Button("Reset##ResetCameraInertia"))
    {
        g_state->camera->inertia = 1.0f;
        g_camera_inertia = -FLT_MAX;
    }

    ImGui::DragFloat("UI camera speed##DragSpeed", &g_camera_speed, 0.1f, 0.1f, 10.0f);
    tooltip("Camera speed when dragging or moving with keyboard.");
    ImGui::SameLine();
    if (ImGui::Button("Reset##ResetUICameraSpeed"))
        g_camera_speed = 1.0f;
    ImGui::PopItemWidth();
}

void render_arrow()
{
    ImGui::GetIO().MouseDrawCursor = false;
    auto base = ImGui::GetMainViewport();
    ImVec2 pos = mouse_pos();
    ImVec2 line = ImVec2(pos.x - startpos.x, pos.y - startpos.y);
    float length = (float)std::sqrt(std::pow(line.x, 2) + std::pow(line.y, 2));
    float theta = 0.7f;
    float width = 10.0f + length / 15.0f;
    float tpoint = width / (2 * (std::tanf(theta) / 2) * length);
    ImVec2 point = ImVec2(pos.x + (-tpoint * line.x), pos.y + (-tpoint * line.y));
    ImVec2 normal = ImVec2(-line.x, line.y);
    float tnormal = width / (2 * length);
    ImVec2 leftpoint = ImVec2(point.x + tnormal * normal.y, point.y + tnormal * normal.x);
    ImVec2 rightpoint = ImVec2(point.x + (-tnormal * normal.y), point.y + (-tnormal * normal.x));
    auto* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddLine(base->Pos + ImVec2(startpos.x - 9, startpos.y - 9), base->Pos + ImVec2(startpos.x + 10, startpos.y + 10), ImColor(255, 255, 255, 200), 2);
    draw_list->AddLine(base->Pos + ImVec2(startpos.x - 9, startpos.y + 9), base->Pos + ImVec2(startpos.x + 10, startpos.y - 10), ImColor(255, 255, 255, 200), 2);
    draw_list->AddLine(base->Pos + startpos, base->Pos + pos, ImColor(255, 0, 0, 200), 2);
    draw_list->AddLine(base->Pos + leftpoint, base->Pos + ImVec2(pos.x, pos.y), ImColor(255, 0, 0, 200), 2);
    draw_list->AddLine(base->Pos + rightpoint, base->Pos + ImVec2(pos.x, pos.y), ImColor(255, 0, 0, 200), 2);
}

void render_cross()
{
    ImGui::GetIO().MouseDrawCursor = false;
    auto base = ImGui::GetMainViewport();
    auto* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddLine(base->Pos + ImVec2(startpos.x - 9, startpos.y - 9), base->Pos + ImVec2(startpos.x + 10, startpos.y + 10), ImColor(255, 255, 255, 200), 2);
    draw_list->AddLine(base->Pos + ImVec2(startpos.x - 9, startpos.y + 9), base->Pos + ImVec2(startpos.x + 10, startpos.y - 10), ImColor(255, 255, 255, 200), 2);
}

void render_select()
{
    ImGui::GetIO().MouseDrawCursor = false;
    auto base = ImGui::GetMainViewport();
    ImVec2 pos = mouse_pos();
    auto* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRectFilled(base->Pos + startpos, base->Pos + pos, ImColor(255, 255, 255, 60));
}

void select_entities()
{
    ImGui::GetIO().MouseDrawCursor = true;
    ImVec2 pos = mouse_pos();
    auto mask = safe_entity_mask;
    if (ImGui::GetIO().KeyShift) // TODO: Get the right modifier
    {
        mask = unsafe_entity_mask;
    }
    auto spos = normalize(startpos);
    auto spos2 = normalize(pos);
    auto [ax, ay] = UI::click_position(spos.x, spos.y);
    auto [bx, by] = UI::click_position(spos2.x, spos2.y);
    auto rax = std::min(ax, bx);
    auto ray = std::max(ay, by);
    auto rbx = std::max(ax, bx);
    auto rby = std::min(ay, by);
    auto box = AABB{rax, ray, rbx, rby};
    g_selected_ids = UI::get_entities_overlapping(mask, box, (LAYER)g_state->camera_layer);
}

void erase_entities()
{
    ImVec2 pos = mouse_pos();
    auto mask = safe_entity_mask;
    if (ImGui::GetIO().KeyShift) // TODO: Get the right modifier
    {
        mask = unsafe_entity_mask;
    }
    auto spos = normalize(pos);
    auto [ax, ay] = UI::click_position(spos.x, spos.y);
    auto box = AABB{ax, ay, ax, ay};
    for (auto erase_uid : UI::get_entities_overlapping(mask, box.extrude(0.1f), (LAYER)g_state->camera_layer))
    {
        auto erase = get_entity_ptr(erase_uid);
        if (erase)
            smart_delete(erase);
    }
}

void render_grid(ImColor gridcolor = ImColor(1.0f, 1.0f, 1.0f, 0.2f))
{
    if (g_state == 0 || (g_state->screen < 11 || g_state->screen > 13))
        return;
    auto base = ImGui::GetMainViewport();
    ImVec2 res = base->Size;
    auto* draw_list = ImGui::GetWindowDrawList();
    for (int x = -1; x < 86; x++)
    {
        std::pair<float, float> gridline = UI::screen_position(x + 0.5f, 0);
        if (std::abs(gridline.first) <= 1.0)
        {
            int width = 2;
            ImColor color = gridcolor;
            ImVec2 grids = screenify({gridline.first, gridline.second});
            if ((x % 10) - 2 == 0)
            {
                width = 4;
                color = ImColor(gridcolor.Value.x, gridcolor.Value.y, gridcolor.Value.z, 0.7f);
            }
            else
            {
                width = 2;
                color = ImColor(gridcolor.Value.x, gridcolor.Value.y, gridcolor.Value.z, 0.2f);
            }
            draw_list->AddLine(fix_pos(ImVec2(grids.x, 0)), fix_pos(ImVec2(grids.x, res.y)), color, static_cast<float>(width));
        }
    }
    for (int y = -1; y < 126; y++)
    {
        std::pair<float, float> gridline = UI::screen_position(0, y + 0.5f);
        if (std::abs(gridline.second) <= 1.0)
        {
            int width = 2;
            ImColor color = gridcolor;
            ImVec2 grids = screenify({gridline.first, gridline.second});
            if ((y % 8) - 2 == 0)
            {
                width = 4;
                color = ImColor(color.Value.x, color.Value.y, color.Value.z, 0.7f);
            }
            else
            {
                width = 2;
                color = ImColor(color.Value.x, color.Value.y, color.Value.z, 0.2f);
            }
            draw_list->AddLine(fix_pos(ImVec2(0, grids.y)), fix_pos(ImVec2(res.x, grids.y)), color, static_cast<float>(width));
        }
    }
    g_players = UI::get_players();
    for (auto player : g_players)
    {
        auto p_pos = UI::get_position(player);
        std::pair<float, float> gridline = UI::screen_position(std::round(p_pos.first - 0.5f) + 0.5f, std::round(p_pos.second) - 0.5f);
        ImVec2 grids = screenify({gridline.first, gridline.second});
        draw_list->AddLine(fix_pos(ImVec2(0, grids.y)), fix_pos(ImVec2(res.x, grids.y)), ImColor(255, 0, 255, 200), 2);
        draw_list->AddLine(fix_pos(ImVec2(grids.x, 0)), fix_pos(ImVec2(grids.x, res.y)), ImColor(255, 0, 255, 200), 2);
    }
    if (update_entity())
    {
        auto e_pos = UI::get_position(g_entity);
        std::pair<float, float> gridline = UI::screen_position(std::round(e_pos.first - 0.5f) + 0.5f, std::round(e_pos.second) - 0.5f);
        ImVec2 grids = screenify({gridline.first, gridline.second});
        draw_list->AddLine(fix_pos(ImVec2(0, grids.y)), fix_pos(ImVec2(res.x, grids.y)), ImColor(0, 255, 0, 200), 2);
        draw_list->AddLine(fix_pos(ImVec2(grids.x, 0)), fix_pos(ImVec2(grids.x, res.y)), ImColor(0, 255, 0, 200), 2);
    }
    for (unsigned int x = 0; x < g_state->w; ++x)
    {
        for (unsigned int y = 0; y < g_state->h; ++y)
        {
            auto room_temp = UI::get_room_template(x, y, g_state->camera_layer);
            if (room_temp.has_value())
            {
                auto room_name = UI::get_room_template_name(room_temp.value());
                auto room_pos = UI::get_room_pos(x, y);
                auto pos = UI::screen_position(room_pos.first, room_pos.second);
                ImVec2 spos = screenify({pos.first, pos.second});
                std::string room_text = fmt::format("{:d},{:d} {:s} ({:d})", x, y, room_name, room_temp.value());
                draw_list->AddText(fix_pos(ImVec2(spos.x + 5.0f, spos.y + 5.0f)), ImColor(1.0f, 1.0f, 1.0f, 1.0f), room_text.c_str());
            }
        }
    }
}

bool is_entrance_room(unsigned int x, unsigned int y)
{
    static const uint16_t rooms[] = {5, 6};
    auto room_temp = UI::get_room_template(x, y, 0);
    if (room_temp.has_value())
    {
        return std::find(std::begin(rooms), std::end(rooms), room_temp.value()) != std::end(rooms);
    }
    return false;
}

bool is_exit_room(unsigned int x, unsigned int y)
{
    static const uint16_t rooms[] = {7, 8, 129};
    auto room_temp = UI::get_room_template(x, y, 0);
    if (room_temp.has_value())
    {
        return std::find(std::begin(rooms), std::end(rooms), room_temp.value()) != std::end(rooms);
    }
    return false;
}

bool is_path_room(unsigned int x, unsigned int y)
{
    static const uint16_t rooms[] = {1, 2, 3, 4, 5, 6, 7, 8, 47, 48, 53, 102, 107, 109, 129, 130, 131};
    auto room_temp = UI::get_room_template(x, y, 0);
    if (room_temp.has_value())
    {
        return std::find(std::begin(rooms), std::end(rooms), room_temp.value()) != std::end(rooms);
    }
    return false;
}

bool is_lake_room(unsigned int x, unsigned int y)
{
    static const uint16_t rooms[] = {129, 130, 131};
    auto room_temp = UI::get_room_template(x, y, 0);
    if (room_temp.has_value())
    {
        return std::find(std::begin(rooms), std::end(rooms), room_temp.value()) != std::end(rooms);
    }
    return false;
}

std::optional<std::pair<unsigned int, unsigned int>> get_entrance()
{
    for (unsigned int x = 0; x < g_state->w; ++x)
    {
        for (unsigned int y = 0; y < g_state->h; ++y)
        {
            if (is_entrance_room(x, y))
                return {{x, y}};
        }
    }
    return std::nullopt;
}

std::optional<std::pair<unsigned int, unsigned int>> get_exit()
{
    for (unsigned int x = 0; x < g_state->w; ++x)
    {
        for (unsigned int y = 0; y < g_state->h; ++y)
        {
            if (is_exit_room(x, y))
                return {{x, y}};
        }
    }
    return std::nullopt;
}

bool is_visited(unsigned int x, unsigned int y, std::vector<std::pair<unsigned int, unsigned int>>& path)
{
    for (auto [vx, vy] : path)
    {
        if (x == vx && y == vy)
            return true;
    }
    return false;
}

bool get_next_room(unsigned int& x, unsigned int& y, int dy, std::vector<std::pair<unsigned int, unsigned int>>& path)
{
    auto exit = get_exit();
    auto dx = -1;
    if (exit.has_value())
    {
        auto [ex, ey] = exit.value();
        if (ex > x)
            dx = 1;
    }
    if (g_state->theme == 16 && y == 9 && !is_visited(x, y + dy, path) && is_path_room(x, y + dy) && !is_path_room(2, y))
    {
        y += dy;
        return true;
    }
    if (!is_visited(x + dx, y, path) && is_path_room(x + dx, y))
    {
        x += dx;
        return true;
    }
    dx *= -1;
    if (!is_visited(x + dx, y, path) && is_path_room(x + dx, y))
    {
        x += dx;
        return true;
    }
    if (!is_visited(x, y + dy, path) && is_path_room(x, y + dy))
    {
        y += dy;
        return true;
    }
    return false;
}

void render_path()
{
    if (g_state == 0 || g_state->screen != 12 || g_state->theme == 10)
        return;
    auto* draw_list = ImGui::GetWindowDrawList();
    std::vector<std::pair<unsigned int, unsigned int>> path;
    int dy = 1;
    if (g_state->theme == 9 || g_state->theme == 16)
        dy = -1;
    auto room = get_entrance();
    if (!room.has_value())
        return;
    auto [x, y] = room.value();
    path.push_back({x, y});
    while (get_next_room(x, y, dy, path))
    {
        path.push_back({x, y});
        if (is_exit_room(x, y))
            break;
    }
    if (path.size() < 2)
        return;

    std::vector<ImVec2> points;
    for (auto [px, py] : path)
    {
        auto room_pos = UI::get_room_pos(px, py);
        auto pos = UI::screen_position(room_pos.first + 5.0f, room_pos.second - 4.0f);
        ImVec2 spos = screenify({pos.first, pos.second});
        points.push_back(spos);
    }

    std::vector<ImVec2> midpoints;
    for (size_t i = 1; i < points.size(); ++i)
    {
        midpoints.push_back({(points.at(i).x + points.at(i - 1).x) / 2.0f, (points.at(i).y + points.at(i - 1).y) / 2.0f});
    }

    auto color = ImColor(0.0f, 1.0f, 0.0f, 0.2f);
    draw_list->PathClear();
    draw_list->PathLineTo(fix_pos(points.at(0)));
    for (size_t i = 0; i < midpoints.size(); ++i)
    {
        draw_list->PathBezierQuadraticCurveTo(fix_pos(points.at(i)), fix_pos(midpoints.at(i)));
    }
    draw_list->PathLineTo(fix_pos(points.at(points.size() - 1)));
    auto thick = screenify(UI::screen_distance(2.0f));
    draw_list->PathStroke(color, 0, thick);
}

std::string entity_tooltip(Entity* hovered)
{
    std::string coords;
    coords += fmt::format("{}, {} ({:.2f}, {:.2f})", hovered->uid, entity_names[hovered->type->id], hovered->abs_x == -FLT_MAX ? hovered->x : hovered->abs_x, hovered->abs_y == -FLT_MAX ? hovered->y : hovered->abs_y);
    if (hovered->type->id == to_id("ENT_TYPE_ITEM_POT") && hovered->as<Pot>()->inside > 0)
        coords += fmt::format(" ({})", entity_names[hovered->as<Pot>()->inside]);
    else if (hovered->type->id == to_id("ENT_TYPE_ITEM_PRESENT") || hovered->type->id == to_id("ENT_TYPE_ITEM_GHIST_PRESENT"))
        coords += fmt::format(" ({})", entity_names[hovered->as<Present>()->inside]);
    else if (hovered->type->id == to_id("ENT_TYPE_ITEM_COFFIN"))
        coords += fmt::format(" ({})", entity_names[hovered->as<Coffin>()->inside]);
    else if (hovered->type->id == to_id("ENT_TYPE_ITEM_CRATE") || hovered->type->id == to_id("ENT_TYPE_ITEM_DMCRATE") || hovered->type->id == to_id("ENT_TYPE_ITEM_ALIVE_EMBEDDED_ON_ICE"))
        coords += fmt::format(" ({})", entity_names[hovered->as<Container>()->inside]);
    else if (hovered->type->id == to_id("ENT_TYPE_ITEM_CHEST"))
    {
        auto chest = hovered->as<Chest>();
        if (chest->bomb)
            coords += " (BOMB)";
        if (chest->leprechaun)
            coords += " (LEPRECHAUN)";
    }
    else if (hovered->type->id == to_id("ENT_TYPE_ITEM_BOMB") or hovered->type->id == to_id("ENT_TYPE_ITEM_PASTEBOMB"))
    {
        auto bomb = hovered->as<Bomb>();
        coords += fmt::format(" ({} FUSE)", 150 - bomb->idle_counter);
    }
    else if (!!(hovered->type->search_flags & (ENTITY_MASK::MOUNT | ENTITY_MASK::PLAYER | ENTITY_MASK::MONSTER)))
    {
        auto ent = hovered->as<Movable>();
        coords += fmt::format(" ({} HP)", ent->health);
    }
    if (hovered->overlay)
    {
        coords += fmt::format("\nON: {}, {} ({:.2f}, {:.2f})", hovered->overlay->uid, entity_names[hovered->overlay->type->id], hovered->overlay->abs_x == -FLT_MAX ? hovered->overlay->x : hovered->overlay->abs_x, hovered->overlay->abs_y == -FLT_MAX ? hovered->overlay->y : hovered->overlay->abs_y);
    }
    if (!!(hovered->type->search_flags & (ENTITY_MASK::MOUNT | ENTITY_MASK::PLAYER | ENTITY_MASK::MONSTER | ENTITY_MASK::ITEM)) && hovered->as<Movable>()->last_owner_uid > -1)
    {
        auto ent = hovered->as<Movable>();
        auto owner = get_entity_ptr(ent->last_owner_uid);
        if (owner)
            coords += fmt::format("\nOWNER: {}, {} ({:.2f}, {:.2f})", owner->uid, entity_names[owner->type->id], owner->abs_x == -FLT_MAX ? owner->x : owner->abs_x, owner->abs_y == -FLT_MAX ? owner->y : owner->abs_y);
    }
    return coords;
}

void render_hitbox(Entity* ent, bool cross, ImColor color, bool filled = false, bool rounded = false)
{
    const auto type = ent->type->id;
    if (!type)
        return;

    std::pair<float, float> render_position = UI::get_position(ent, options["draw_hitboxes_interpolated"]);

    auto [originx, originy] =
        UI::screen_position(render_position.first, render_position.second);
    auto [boxa_x, boxa_y] =
        UI::screen_position(render_position.first - ent->hitboxx + ent->offsetx, render_position.second - ent->hitboxy + ent->offsety);
    auto [boxb_x, boxb_y] =
        UI::screen_position(render_position.first + ent->hitboxx + ent->offsetx, render_position.second + ent->hitboxy + ent->offsety);
    ImVec2 sorigin = screenify({originx, originy});
    ImVec2 spos = screenify({(boxa_x + boxb_x) / 2, (boxa_y + boxb_y) / 2});
    ImVec2 sboxa = screenify({boxa_x, boxb_y});
    ImVec2 sboxb = screenify({boxb_x, boxa_y});
    auto* draw_list = ImGui::GetWindowDrawList();
    if (cross)
    {
        draw_list->AddLine(fix_pos(sboxa), fix_pos(sorigin), color, 2);
        draw_list->AddLine(fix_pos(sboxb), fix_pos(sorigin), color, 2);
        draw_list->AddLine(fix_pos(ImVec2(sboxa.x, sboxb.y)), fix_pos(sorigin), color, 2);
        draw_list->AddLine(fix_pos(ImVec2(sboxb.x, sboxa.y)), fix_pos(sorigin), color, 2);
    }
    static const auto spark = to_id("ENT_TYPE_ITEM_SPARK");
    if (type == spark)
    {
        auto ent_spark = ent->as<Spark>();
        if (ent_spark->size >= 1.0)
            color = ImColor(255, 0, 0, 150);
    }
    else if ((ent->type->search_flags & ENTITY_MASK::EXPLOSION) == ENTITY_MASK::EXPLOSION)
    {
        color = ImColor(255, 0, 0, 150);
    }
    if (ent->shape == SHAPE::CIRCLE)
        if (filled)
            draw_list->AddCircleFilled(fix_pos(spos), sboxb.x - spos.x, color);
        else
            draw_list->AddCircle(fix_pos(spos), sboxb.x - spos.x, color, 0, 2.0f);
    else if (filled)
        draw_list->AddRectFilled(fix_pos(sboxa), fix_pos(sboxb), color, rounded ? 5.0f : 0.0f);
    else
        draw_list->AddRect(fix_pos(sboxa), fix_pos(sboxb), color, rounded ? 5.0f : 0.0f, 0, 2.0f);

    if (options["draw_entity_info"] && !cross && !filled)
        draw_list->AddText(fix_pos(ImVec2(sboxa.x, sboxb.y)), ImColor(1.0f, 1.0f, 1.0f, 0.8f), entity_tooltip(ent).c_str());

    if ((g_hitbox_mask & 0x8000) == 0)
        return;

    static const auto spark_trap = to_id("ENT_TYPE_FLOOR_SPARK_TRAP");
    static const auto wooden_arrow = to_id("ENT_TYPE_ITEM_WOODEN_ARROW");
    static const auto metal_arrow = to_id("ENT_TYPE_ITEM_METAL_ARROW");
    static const auto light_arrow = to_id("ENT_TYPE_ITEM_LIGHT_ARROW");
    static const auto bomb = to_id("ENT_TYPE_ITEM_BOMB");
    static const auto mine = to_id("ENT_TYPE_ITEM_LANDMINE");
    static const auto keg = to_id("ENT_TYPE_ACTIVEFLOOR_POWDERKEG");
    static const auto keg2 = to_id("ENT_TYPE_ACTIVEFLOOR_TIMEDPOWDERKEG");
    static const auto sun_generator = to_id("ENT_TYPE_FLOOR_SUNCHALLENGE_GENERATOR");
    static const auto shoppie_generator = to_id("ENT_TYPE_FLOOR_SHOPKEEPER_GENERATOR");

    if (type == spark_trap && ent->animation_frame == 7)
    {
        float distance = UI::get_spark_distance(ent->as<SparkTrap>());
        auto thick = UI::screen_distance(0.55f);
        auto sthick = screenify(thick);
        auto [radx, rady] = UI::screen_position(render_position.first + distance, render_position.second + distance);
        auto srad = screenify({radx, rady});
        draw_list->AddCircle(fix_pos(spos), srad.x - spos.x, ImColor(255, 0, 0, 40), 0, sthick);
    }
    else if ((type == sun_generator || type == shoppie_generator) && !g_players.empty())
    {
        auto gen = ent->as<Generator>();
        float rad = 8.0f;
        float min_dist = 100.0f;
        for (auto p : g_players)
        {
            auto [x, y] = UI::get_position(p);
            auto a = Vec2(x, y);
            auto b = Vec2(gen->x, gen->y);
            auto dist = a.distance_to(b);
            if (dist < min_dist)
                min_dist = dist;
        }
        bool enabled = min_dist < 8.0f && (type != sun_generator || gen->on_off);
        if (cross || enabled || min_dist < 8.0f)
        {
            auto [radx, rady] = UI::screen_position(render_position.first + rad, render_position.second + rad);
            auto srad = screenify({radx, rady});
            draw_list->AddCircle(fix_pos(spos), srad.x - spos.x, enabled ? ImColor(255, 0, 0, 80) : ImColor(0, 200, 128, 60), 0, 2.0f);
        }
    }
    else if (type == bomb or type == bomb + 1)
    {
        float rad = 1.6f;
        if (((Bomb*)ent)->scale_hor > 1.25f)
            rad = 2.6f;
        auto [radx, rady] = UI::screen_position(render_position.first + rad, render_position.second + rad);
        auto srad = screenify({radx, rady});
        draw_list->AddCircle(fix_pos(spos), srad.x - spos.x, ImColor(255, 0, 0, 150), 0, 2.0f);
    }
    else if (type == mine)
    {
        auto rpos = UI::screen_position(render_position.first, render_position.second);
        auto srpos = screenify({rpos.first, rpos.second});

        float rad = 1.6f;
        auto [radx, rady] = UI::screen_position(render_position.first + rad, render_position.second + rad);
        auto srad = screenify({radx, rady});
        draw_list->AddCircle(fix_pos(srpos), srad.x - spos.x, ImColor(255, 0, 0, 150), 0, 2.0f);
    }
    else if (type == keg || type == keg2)
    {
        float rad = 1.6f;
        auto [radx, rady] = UI::screen_position(render_position.first + rad, render_position.second + rad);
        auto srad = screenify({radx, rady});
        draw_list->AddCircle(fix_pos(spos), srad.x - spos.x, ImColor(255, 0, 0, 150), 0, 2.0f);
    }
    else if (type == wooden_arrow || type == metal_arrow || type == light_arrow)
    {
        ImVec2 ps = {render_position.first, render_position.second};
        float cosa = ImCos(ent->angle);
        float sina = ImSin(ent->angle);
        ImVec2 pa = {-ent->hitboxx, -ent->hitboxy};
        ImVec2 pb = {+ent->hitboxx, -ent->hitboxy};
        ImVec2 pc = {+ent->hitboxx, +ent->hitboxy};
        ImVec2 pd = {-ent->hitboxx, +ent->hitboxy};
        pa = ps + ImRotate(pa, cosa, sina);
        pb = ps + ImRotate(pb, cosa, sina);
        pc = ps + ImRotate(pc, cosa, sina);
        pd = ps + ImRotate(pd, cosa, sina);
        auto [pax, pay] = UI::screen_position(pa.x, pa.y);
        auto [pbx, pby] = UI::screen_position(pb.x, pb.y);
        auto [pcx, pcy] = UI::screen_position(pc.x, pc.y);
        auto [pdx, pdy] = UI::screen_position(pd.x, pd.y);
        const ImVec2 points[] = {fix_pos(screenify({pax, pay})), fix_pos(screenify({pbx, pby})), fix_pos(screenify({pcx, pcy})), fix_pos(screenify({pdx, pdy})), fix_pos(screenify({pax, pay}))};
        draw_list->AddPolyline(points, 5, ImColor(255, 0, 0, 150), 0, 2.0f);
    }
}

void fix_script_requires(Script auto* script)
{
    if (!script->is_enabled())
        return;
    for (auto req : script->consume_requires())
    {
        for (auto& [name, script2] : g_scripts)
        {
            if (script2->get_id() == req)
            {
                if (!script2->is_enabled())
                    script2->set_changed(true);
                script2->set_enabled(true);
            }
        }
    }
}

void render_script(Script auto* script, ImDrawList* draw_list)
{
    if (!script->is_enabled())
        return;
    script->draw(draw_list);
}

void set_pos(ImVec2 pos)
{
    g_x = normalize(pos).x;
    g_y = normalize(pos).y;
}

void set_vel(ImVec2 pos)
{
    g_vx = normalize(pos).x;
    g_vy = normalize(pos).y;
    auto cpos = UI::click_position(g_x, g_y);
    auto cpos2 = UI::click_position(g_vx, g_vy);
    g_dx = floor(cpos2.first + 0.5f) - floor(cpos.first + 0.5f);
    g_dy = floor(cpos2.second + 0.5f) - floor(cpos.second + 0.5f);
    g_vx = 2 * (g_vx - g_x);
    g_vy = 2 * (g_vy - g_y) * 0.5625f;
}

void render_messages()
{
    using namespace std::chrono_literals;
    auto now = std::chrono::system_clock::now();

    if (options["console_script_messages"])
    {
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::tm time_buf;
        localtime_s(&time_buf, &in_time_t);

        for (auto& [name, script] : g_scripts)
        {
            std::vector<ScriptMessage> messages;
            for (auto&& message : script->consume_messages())
                messages.push_back(message);
            if (messages.size() > 0)
                g_Console->push_history(fmt::format("--- [{}] at {:%Y-%m-%d %X}", script->get_name(), time_buf), std::move(messages));
        }
        {
            std::vector<ScriptMessage> messages;
            for (auto&& message : g_Console->consume_messages())
                messages.push_back(message);
            if (messages.size() > 0)
                g_Console->push_history(fmt::format("--- [Console] at {:%Y-%m-%d %X}", time_buf), std::move(messages));
        }
        return;
    }

    using Message = std::tuple<std::string, std::string, std::chrono::time_point<std::chrono::system_clock>, ImVec4>;
    std::vector<Message> queue;
    for (auto& script : g_scripts)
    {
        script.second->loop_messages(
            [&](const ScriptMessage& message)
            {
                if (options["fade_script_messages"] && now - 12s > message.time)
                    return;
                std::istringstream messages(message.message);
                while (!messages.eof())
                {
                    std::string mline;
                    getline(messages, mline);
                    queue.push_back(std::make_tuple(script.second->get_name(), mline, message.time, message.color));
                }
            });
    }
    for (auto&& message : g_Console->consume_messages())
    {
        g_ConsoleMessages.push_back(std::move(message));
    }
    for (auto& message : g_ConsoleMessages)
    {
        std::istringstream messages(message.message);
        while (!messages.eof())
        {
            std::string mline;
            getline(messages, mline);
            queue.push_back(std::make_tuple("Console", mline, message.time, message.color));
        }
    }
    std::erase_if(g_ConsoleMessages, [&](auto message)
                  { return options["fade_script_messages"] && now - 12s > message.time; });

    ImGuiIO& io = ImGui::GetIO();
    // ImGui::PushFont(bigfont);

    std::sort(queue.begin(), queue.end(), [](Message a, Message b)
              { return std::get<2>(a) < std::get<2>(b); });

    ImGui::SetNextWindowSize({-1, -1});
    auto base = ImGui::GetMainViewport();
    ImGui::SetNextWindowViewport(base->ID);
    ImGui::Begin(
        "Messages",
        NULL,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDocking);

    if (ImGui::IsWindowHovered())
        io.WantCaptureMouse = options["mouse_control"];

    const float font_size = (ImGui::GetCurrentWindow()->CalcFontSize() + ImGui::GetStyle().ItemSpacing.y);

    unsigned int logsize = (std::min)(30, (int)((io.DisplaySize.y - 300) / font_size));
    if (queue.size() > logsize)
    {
        std::vector<Message> newqueue(queue.end() - logsize, queue.end());
        queue = newqueue;
    }

    ImGui::SetWindowPos({base->Pos.x + 30.0f + 0.128f * base->Size.x * io.FontGlobalScale, base->Pos.y + base->Size.y - queue.size() * font_size - 40});
    for (const auto& message : queue)
    {
        float alpha = 1.0f - std::chrono::duration_cast<std::chrono::milliseconds>(now - std::get<2>(message)).count() / 12000.0f;
        if (!options["fade_script_messages"])
        {
            alpha = 0.8f;
        }
        ImVec4 color = std::get<3>(message);
        color.w = alpha;
        ImGui::TextColored(color, "[%s] %s", std::get<0>(message).c_str(), std::get<1>(message).c_str());
    }
    // ImGui::PopFont();
    ImGui::End();
}

void render_clickhandler()
{
    ImGuiIO& io = ImGui::GetIO();
    auto base = ImGui::GetMainViewport();
    ImGui::SetNextWindowBgAlpha(0.0f);
    auto main_dock = ImGui::DockSpaceOverViewport(base, ImGuiDockNodeFlags_PassthruCentralNode);
    auto space = ImGui::DockBuilderGetCentralNode(main_dock);
    g_Console.get()->set_geometry(space->Pos.x, space->Pos.y, space->Size.x, space->Size.y);
    if (g_Console->is_toggled())
    {
        auto console_height = (2.0f * ImGui::GetStyle().ItemSpacing.y + g_Console.get()->get_input_lines() * ImGui::GetTextLineHeight());
        if (options["menu_ui"])
            console_height += ImGui::GetTextLineHeight();
        ImGui::SetNextWindowSize({space->Size.x - 32.0f, space->Size.y - console_height});
        ImGui::SetNextWindowPos({space->Pos.x + 16.0f, space->Pos.y});
    }
    else
    {
        ImGui::SetNextWindowSize(space->Size);
        ImGui::SetNextWindowPos(space->Pos);
    }
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.0f, 0.0f});
    ImGui::SetNextWindowViewport(base->ID);
    ImGui::Begin(
        "Clickhandler",
        NULL,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDocking);
    if (ImGui::IsWindowHovered())
        io.WantCaptureMouse = options["mouse_control"];
    if (io.MouseWheel != 0 && ImGui::IsWindowHovered() && !g_bucket->io->WantCaptureMouse.value_or(false))
    {
        if (clicked("mouse_zoom_out") || (held("mouse_camera_drag") && io.MouseWheel < 0))
        {
            zooming = true;
            if (g_zoom == 0.0f)
                g_zoom = UI::get_zoom_level();
            g_zoom += g_zoom / 13.5f;
            set_zoom();
        }
        else if (clicked("mouse_zoom_in") || (held("mouse_camera_drag") && io.MouseWheel > 0))
        {
            zooming = true;
            if (g_zoom == 0.0f)
                g_zoom = UI::get_zoom_level();
            g_zoom -= g_zoom / 13.5f;
            set_zoom();
        }
    }
    if (options["draw_path"])
        render_path();
    if (options["draw_grid"])
        render_grid();
    if (options["draw_hitboxes"] && g_state->screen != 5)
    {
        static const auto olmec = to_id("ENT_TYPE_ACTIVEFLOOR_OLMEC");
        for (auto entity : UI::get_entities_by({}, (ENTITY_MASK)g_hitbox_mask, (LAYER)g_state->camera_layer))
        {
            auto ent = get_entity_ptr(entity);
            if (!ent)
                continue;

            if (ent->type->id == olmec)
            {
                render_hitbox(ent, false, ImColor(0, 255, 255, 150));
                continue;
            }

            if (!UI::has_active_render(ent) && !(ent->type->search_flags & (ENTITY_MASK::LOGICAL | ENTITY_MASK::LIQUID)))
                continue;

            if (!(ent->type->search_flags & ENTITY_MASK::PLAYER) || ent->as<Player>()->ai)
                render_hitbox(ent, false, ImColor(0, 255, 255, 150));
        }
        if ((g_hitbox_mask & 0x1) != 0)
        {
            g_players = UI::get_players();
            for (auto player : g_players)
            {
                render_hitbox(player, false, ImColor(255, 0, 255, 200));
            }
        }

        if ((g_hitbox_mask & 0x8000) != 0)
        {
            static const auto additional_fixed_entities = {
                to_id("ENT_TYPE_FLOOR_MOTHER_STATUE_PLATFORM"),
                to_id("ENT_TYPE_FLOOR_MOTHER_STATUE"),
                to_id("ENT_TYPE_ACTIVEFLOOR_EGGSHIPBLOCKER"),
                to_id("ENT_TYPE_FLOOR_SURFACE_HIDDEN"),
                to_id("ENT_TYPE_FLOOR_YAMA_PLATFORM"),
                to_id("ENT_TYPE_FLOOR_ARROW_TRAP"),
                to_id("ENT_TYPE_FLOOR_POISONED_ARROW_TRAP"),
                to_id("ENT_TYPE_FLOOR_TOTEM_TRAP"),
                to_id("ENT_TYPE_FLOOR_JUNGLE_SPEAR_TRAP"),
                to_id("ENT_TYPE_FLOOR_LION_TRAP"),
                to_id("ENT_TYPE_FLOOR_LASER_TRAP"),
                to_id("ENT_TYPE_FLOOR_SPARK_TRAP"),
                to_id("ENT_TYPE_FLOOR_SPIKEBALL_CEILING"),
                to_id("ENT_TYPE_FLOOR_SPRING_TRAP"),
                to_id("ENT_TYPE_FLOOR_BIGSPEAR_TRAP"),
                to_id("ENT_TYPE_FLOOR_STICKYTRAP_CEILING"),
                to_id("ENT_TYPE_FLOOR_DUSTWALL"),
                to_id("ENT_TYPE_FLOOR_TENTACLE_BOTTOM"),
                to_id("ENT_TYPE_FLOOR_TELEPORTINGBORDER"),
                to_id("ENT_TYPE_FLOOR_SPIKES"),
                to_id("ENT_TYPE_FLOOR_FACTORY_GENERATOR"),
                to_id("ENT_TYPE_FLOOR_SHOPKEEPER_GENERATOR"),
                to_id("ENT_TYPE_FLOOR_SUNCHALLENGE_GENERATOR"),
            };
            for (auto entity : UI::get_entities_by(additional_fixed_entities, ENTITY_MASK::FLOOR | ENTITY_MASK::ACTIVEFLOOR, (LAYER)g_state->camera_layer))
            {
                auto ent = get_entity_ptr(entity);
                render_hitbox(ent, false, ImColor(0, 255, 255, 150));
            }
            for (auto entity : UI::get_entities_by({(ENT_TYPE)CUSTOM_TYPE::TRIGGER}, ENTITY_MASK::LOGICAL, (LAYER)g_state->camera_layer))
            {
                auto ent = get_entity_ptr(entity);
                render_hitbox(ent, false, ImColor(255, 0, 0, 150));
            }
            for (auto entity : UI::get_entities_by({to_id("ENT_TYPE_LOGICAL_DOOR")}, ENTITY_MASK::LOGICAL, (LAYER)g_state->camera_layer)) // DOOR
            {
                auto ent = get_entity_ptr(entity);
                render_hitbox(ent, false, ImColor(255, 180, 45, 150), false, true);
            }

            // OOB kill bounds
            auto* draw_list = ImGui::GetBackgroundDrawList();
            std::pair<float, float> gridline = UI::screen_position(-0.5f, 0);
            ImVec2 grids = screenify({gridline.first, gridline.second});
            draw_list->AddLine(fix_pos(ImVec2(grids.x, base->Pos.y)), fix_pos(ImVec2(grids.x, base->Pos.y + base->Size.y)), ImColor(255, 0, 0, 150), 2.0f);
            gridline = UI::screen_position(static_cast<float>(10 * g_state->w) + 5.5f, 0);
            grids = screenify({gridline.first, gridline.second});
            draw_list->AddLine(fix_pos(ImVec2(grids.x, base->Pos.y)), fix_pos(ImVec2(grids.x, base->Pos.y + base->Size.y)), ImColor(255, 0, 0, 150), 2.0f);
            gridline = UI::screen_position(0, static_cast<float>(120 - 8 * g_state->h) - 4.0f);
            grids = screenify({gridline.first, gridline.second});
            draw_list->AddLine(fix_pos(ImVec2(base->Pos.x, grids.y)), fix_pos(ImVec2(base->Pos.x + base->Size.x, grids.y)), ImColor(255, 0, 0, 150), 2.0f);
        }
    }

    if (ImGui::IsMousePosValid())
    {
        ImVec2 mpos = normalize(mouse_pos());
        std::pair<float, float> cpos = UI::click_position(mpos.x, mpos.y);
        std::string coords = fmt::format("{:.2f}, {:.2f} ({:.2f}, {:.2f})", cpos.first, cpos.second, mpos.x, mpos.y);
        unsigned int mask = safe_entity_mask;
        if (ImGui::GetIO().KeyShift) // TODO: Get the right modifier from mouse_destroy_unsafe
        {
            mask = unsafe_entity_mask;
        }
        auto hovered = UI::get_entity_at(cpos.first, cpos.second, false, 2, mask);
        if (hovered)
        {
            if (options["draw_hitboxes"])
                render_hitbox(hovered, true, ImColor(50, 50, 255, 200));
            coords += "\n" + entity_tooltip(hovered);
            g_bucket->overlunky->hovered_uid = hovered->uid;
        }
        else
        {
            g_bucket->overlunky->hovered_uid = -1;
        }
        if (options["draw_entity_tooltip"] && ImGui::IsWindowHovered() && io.MouseDrawCursor)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {4.0f, 4.0f});
            tooltip(coords.c_str(), true);
            ImGui::PopStyleVar();
        }
    }

    static auto front_col = ImColor(0, 255, 51, 200);
    static auto back_col = ImColor(255, 160, 31, 200);
    static auto front_fill = ImColor(front_col);
    front_fill.Value.w = 0.25f;
    static auto back_fill = ImColor(back_col);
    back_fill.Value.w = 0.25f;
    if (update_entity())
    {
        auto this_layer = g_state->camera_layer == g_entity->layer;
        render_hitbox(g_entity, true, this_layer ? front_col : back_col);
    }
    for (auto entity : g_selected_ids)
    {
        auto ent = get_entity_ptr(entity);
        if (ent)
        {
            if (ent->layer == g_state->camera_layer)
                render_hitbox(ent, false, front_fill, true);
            else
                render_hitbox(ent, false, back_fill, true);
        }
    }

    for (auto& [name, script] : g_scripts)
    {
        fix_script_requires(script.get());
    }
    fix_script_requires(g_Console.get());
    auto* draw_list = ImGui::GetBackgroundDrawList();
    if (g_last_id > -1)
        g_Console.get()->set_selected_uid(g_last_id);
    for (auto& [name, script] : g_scripts)
    {
        render_script(script.get(), draw_list);
    }
    g_Console.get()->draw(draw_list);

    for (auto& [name, script] : g_ui_scripts)
    {
        fix_script_requires(script.get());
    }
    for (auto& [name, script] : g_ui_scripts)
    {
        render_script(script.get(), draw_list);
    }

    if (g_Console->has_new_history())
    {
        g_Console->save_history("console_history.txt");
    }
    if (g_state->screen == 29)
    {
        ImDrawList* dl = ImGui::GetBackgroundDrawList();
        const char* warningtext = " Overlunky does\nnot work online!";
        ImVec2 warningsize = hugefont->CalcTextSizeA(144.0, io.DisplaySize.x - 200, io.DisplaySize.x - 200, warningtext);
        dl->AddText(
            hugefont,
            144.0,
            fix_pos(ImVec2(io.DisplaySize.x / 2 - warningsize.x / 2, io.DisplaySize.y / 2 - warningsize.y / 2)),
            ImColor(1.0f, 1.0f, 1.0f, 0.4f),
            warningtext);
        const char* subtext = "Probably... Some things might, but don't just expect a random script to work.";
        ImVec2 subsize = font->CalcTextSizeA(18.0, io.DisplaySize.x - 200, io.DisplaySize.x - 200, subtext);
        dl->AddText(
            font,
            18.0,
            fix_pos(ImVec2(io.DisplaySize.x / 2 - subsize.x / 2, io.DisplaySize.y / 2 + warningsize.y / 2 + 20)),
            ImColor(1.0f, 1.0f, 1.0f, 0.4f),
            subtext);
    }
    using namespace std::chrono_literals;
    auto now = std::chrono::system_clock::now();
    if (options["mouse_control"] && now > last_focus_time + 200ms && (!options["menu_ui"] || mouse_pos().y > ImGui::GetTextLineHeight()) && !g_bucket->io->WantCaptureMouse.value_or(false))
    {
        ImGui::InvisibleButton("canvas", ImGui::GetContentRegionMax(), ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
        if (ImGui::BeginDragDropTarget())
        {
            if (const auto payload = ImGui::AcceptDragDropPayload("TAB", ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
            {
                detach(active_tab);
                detach_tab = active_tab;
            }
            ImGui::EndDragDropTarget();
        }

        if ((clicked("mouse_spawn_throw") || clicked("mouse_teleport_throw")))
        {
            startpos = mouse_pos();
        }
        else if ((clicked("mouse_spawn") || clicked("mouse_teleport")))
        {
            startpos = mouse_pos();
        }
        else if (clicked("mouse_spawn_over"))
        {
            io.MouseDrawCursor = false;
            startpos = mouse_pos();
            set_pos(startpos);
            auto ent = UI::get_entity_at(g_x, g_y, true, 2, safe_entity_mask);
            g_over_id = ent ? ent->uid : -1;
        }
        else if ((held("mouse_spawn_throw") || held("mouse_teleport_throw") || held("mouse_spawn_over")) && ImGui::IsWindowFocused())
        {
            render_arrow();
        }
        else if ((held("mouse_spawn") || held("mouse_teleport") || held("mouse_spawn_over")) && ImGui::IsWindowFocused())
        {
            startpos = mouse_pos();
            render_cross();
        }
        else if (released("mouse_spawn_throw") && ImGui::IsWindowFocused())
        {
            set_pos(startpos);
            set_vel(mouse_pos());
            spawn_entities(true);
            g_x = 0;
            g_y = 0;
            g_vx = 0;
            g_vy = 0;
        }
        else if (released("mouse_spawn") && ImGui::IsWindowFocused())
        {
            set_pos(startpos);
            spawn_entities(true);
            g_x = 0;
            g_y = 0;
            g_vx = 0;
            g_vy = 0;
        }
        else if (released("mouse_spawn_over") && ImGui::IsWindowFocused())
        {
            set_pos(startpos);
            set_vel(mouse_pos());
            spawn_entity_over();
            g_x = 0;
            g_y = 0;
            g_dx = 0;
            g_dy = 0;
            g_vx = 0;
            g_vy = 0;
            g_over_id = -1;
        }
        else if (clicked("mouse_draw") && ImGui::IsWindowFocused())
        {
            grid_x = UINT_MAX;
            grid_y = UINT_MAX;
        }
        else if (held("mouse_draw") && ImGui::IsWindowFocused())
        {
            auto [nx, ny] = normalize(mouse_pos());
            auto pos = UI::click_position(nx, ny);
            const uint32_t new_grid_x = static_cast<uint32_t>(std::round(pos.first));
            const uint32_t new_grid_y = static_cast<uint32_t>(std::round(pos.second));
            if (new_grid_x != grid_x || new_grid_y != grid_y)
            {
                grid_x = new_grid_x;
                grid_y = new_grid_y;
                set_pos(mouse_pos());
                g_vx = 0;
                g_vy = 0;
                spawn_entities(true);
            }
        }
        else if (held("mouse_erase"))
        {
            erase_entities();
        }
        else if (clicked("mouse_decorate"))
        {
            grid_x = UINT_MAX;
            grid_y = UINT_MAX;
        }
        else if (held("mouse_decorate"))
        {
            auto [nx, ny] = normalize(mouse_pos());
            auto pos = UI::click_position(nx, ny);
            const uint32_t new_grid_x = static_cast<uint32_t>(std::round(pos.first));
            const uint32_t new_grid_y = static_cast<uint32_t>(std::round(pos.second));
            if (new_grid_x != grid_x || new_grid_y != grid_y)
            {
                grid_x = new_grid_x;
                grid_y = new_grid_y;
                fix_decorations_at(std::round(pos.first), std::round(pos.second), (LAYER)g_state->camera_layer);
            }
        }
        else if (released("mouse_teleport_throw") && ImGui::IsWindowFocused() && !g_players.empty() && g_game_manager->pause_ui->visibility == 0)
        {
            set_pos(startpos);
            set_vel(mouse_pos());
            ImVec2 mpos = normalize(startpos);
            std::pair<float, float> cpos = UI::click_position(mpos.x, mpos.y);
            if (g_state->theme == 10)
                fix_co_coordinates(cpos);
            auto player = (Movable*)g_players.at(0)->topmost_mount();
            UI::teleport_entity_abs(player, cpos.first, cpos.second, g_vx, g_vy);
            g_x = 0;
            g_y = 0;
            g_vx = 0;
            g_vy = 0;
        }
        else if (released("mouse_teleport") && ImGui::IsWindowFocused() && !g_players.empty() && g_game_manager->pause_ui->visibility == 0)
        {
            set_pos(startpos);
            ImVec2 mpos = normalize(mouse_pos());
            std::pair<float, float> cpos = UI::click_position(mpos.x, mpos.y);
            if (g_state->theme == 10)
                fix_co_coordinates(cpos);
            auto player = (Movable*)g_players.at(0)->topmost_mount();
            UI::teleport_entity_abs(player, cpos.first, cpos.second, g_vx, g_vy);
            g_x = 0;
            g_y = 0;
            g_vx = 0;
            g_vy = 0;
        }
        else if (dblclicked("mouse_grab") || dblclicked("mouse_grab_unsafe"))
        {
            g_selected_ids.clear();
            if (!lock_entity)
                g_last_id = -1;
            update_filter("");
        }
        else if (clicked("mouse_grab") || clicked("mouse_grab_unsafe"))
        {
            startpos = mouse_pos();
            set_pos(startpos);
            unsigned int mask = safe_entity_mask;
            if (held("mouse_grab_unsafe"))
            {
                mask = unsafe_entity_mask;
            }
            g_held_entity = UI::get_entity_at(g_x, g_y, true, 2, mask);
            if (g_held_entity)
            {
                g_held_id = g_held_entity->uid;
                g_held_flags = g_held_entity->flags;
                set_selected_type(g_held_entity->type->id);
                if (pressing("hotbar_1"))
                {
                    hotbar[0] = g_held_entity->type->id;
                    save_config(cfgfile);
                }
                else if (pressing("hotbar_2"))
                {
                    hotbar[1] = g_held_entity->type->id;
                    save_config(cfgfile);
                }
                else if (pressing("hotbar_3"))
                {
                    hotbar[2] = g_held_entity->type->id;
                    save_config(cfgfile);
                }
                else if (pressing("hotbar_4"))
                {
                    hotbar[3] = g_held_entity->type->id;
                    save_config(cfgfile);
                }
                else if (pressing("hotbar_5"))
                {
                    hotbar[4] = g_held_entity->type->id;
                    save_config(cfgfile);
                }
                else if (pressing("hotbar_6"))
                {
                    hotbar[5] = g_held_entity->type->id;
                    save_config(cfgfile);
                }
                else if (pressing("hotbar_7"))
                {
                    hotbar[6] = g_held_entity->type->id;
                    save_config(cfgfile);
                }
                else if (pressing("hotbar_8"))
                {
                    hotbar[7] = g_held_entity->type->id;
                    save_config(cfgfile);
                }
                else if (pressing("hotbar_9"))
                {
                    hotbar[8] = g_held_entity->type->id;
                    save_config(cfgfile);
                }
                else if (pressing("hotbar_0"))
                {
                    hotbar[9] = g_held_entity->type->id;
                    save_config(cfgfile);
                }
            }
            if (!lock_entity)
            {
                g_last_id = g_held_id;
                edit_last_id = true;
            }
        }
        else if (clicked("mouse_select") || clicked("mouse_select_unsafe"))
        {
            startpos = mouse_pos();
            set_pos(startpos);
            render_select();
        }
        else if (held("mouse_grab_throw") && g_held_id > 0)
        {
            if (!throw_held)
            {
                startpos = mouse_pos();
                throw_held = true;
            }
            set_pos(startpos);
            auto held = get_entity_ptr(g_held_id);
            if (held && held->is_movable())
            {
                UI::move_entity(g_held_id, g_x, g_y, true, 0, 0, false);
                render_arrow();
            }
        }
        else if (held("mouse_select") || held("mouse_select_unsafe"))
        {
            set_pos(startpos);
            render_select();
        }
        else if (released("mouse_select") || released("mouse_select_unsafe"))
        {
            select_entities();
        }
        else if ((held("mouse_grab") || held("mouse_grab_unsafe")) && g_held_id > 0 && g_held_entity != 0)
        {
            startpos = mouse_pos();
            throw_held = false;
            io.MouseDrawCursor = false;
            set_pos(startpos);
            if (ImGui::IsMouseDragging(keys["mouse_grab"] & 0xff - 1) || ImGui::IsMouseDragging(keys["mouse_grab_unsafe"] & 0xff - 1))
            {
                if (g_held_entity && g_held_entity->is_movable())
                {
                    if (g_held_entity->is_movable())
                        g_held_entity->as<Movable>()->standing_on_uid = -1;

                    g_held_entity->flags |= 1U << 4;
                    g_held_entity->flags |= 1U << 9;
                    UI::move_entity(g_held_id, g_x, g_y, true, 0, 0, false);
                }
            }
        }
        if (released("mouse_grab_throw") && g_held_id > 0 && g_held_entity != 0)
        {
            throw_held = false;
            io.MouseDrawCursor = true;
            if (g_held_entity)
                g_held_entity->flags = g_held_flags;
            set_pos(startpos);
            set_vel(mouse_pos());
            if (g_held_entity && g_held_entity->is_movable() && drag_delta("mouse_grab_throw") > 10.0f)
                UI::move_entity(g_held_id, g_x, g_y, true, g_vx, g_vy, options["snap_to_grid"]);
            g_x = 0;
            g_y = 0;
            g_vx = 0;
            g_vy = 0;
            g_held_id = -1;
        }
        else if ((released("mouse_grab") || released("mouse_grab_unsafe")) && g_held_id > 0 && g_held_entity != 0)
        {
            throw_held = false;
            io.MouseDrawCursor = true;
            if (g_held_entity)
                g_held_entity->flags = g_held_flags;
            if (options["snap_to_grid"] && g_held_entity->is_movable() && (drag_delta("mouse_grab") > 10.0f || drag_delta("mouse_grab_unsafe") > 10.0f))
            {
                UI::move_entity(g_held_id, g_x, g_y, true, 0, 0, options["snap_to_grid"]);
            }
            else if (!g_held_entity->is_movable())
            {
            }
            g_x = 0;
            g_y = 0;
            g_vx = 0;
            g_vy = 0;
            g_held_id = -1;
        }
        else if (released("mouse_clone"))
        {
            set_pos(mouse_pos());
            UI::spawn_entity(to_id("ENT_TYPE_ITEM_CLONEGUNSHOT"), g_x, g_y, true, 0, 0, options["snap_to_grid"]);
            g_x = 0;
            g_y = 0;
            g_vx = 0;
            g_vy = 0;
        }
        else if (held("mouse_zap") && ImGui::GetFrameCount() > g_last_gun + ImGui::GetIO().Framerate / 5)
        {
            g_last_gun = ImGui::GetFrameCount();
            set_pos(mouse_pos());
            set_vel(ImVec2(mouse_pos().x, mouse_pos().y + 200));
            UI::spawn_entity(to_id("ENT_TYPE_ITEM_LAMASSU_LASER_SHOT"), g_x, g_y, true, g_vx, g_vy, options["snap_to_grid"]);
            g_x = 0;
            g_y = 0;
            g_vx = 0;
            g_vy = 0;
        }

        else if (dblclicked("mouse_camera_drag"))
        {
            set_camera_bounds(true);
            if (g_players.size() > 0)
                g_state->camera->focused_entity_uid = g_players.at(0)->uid;
        }

        else if (held("mouse_camera_drag") && drag_delta("mouse_camera_drag") < 10.0f && held_duration("mouse_camera_drag") > 0.8f && !zooming)
        {
            set_camera_bounds(true);
            if (g_players.size() > 0)
                g_state->camera->focused_entity_uid = g_players.at(0)->uid;
        }

        else if (released("mouse_camera_drag") && !dblclicked("mouse_camera_drag") && drag_delta("mouse_camera_drag") < 3.0f && held_duration_last("mouse_camera_drag") < 0.2f)
        {
            if (ImGui::IsMousePosValid())
            {
                auto ent = UI::get_entity_at(startpos.x, startpos.y, true, 2, safe_entity_mask);
                g_state->camera->focused_entity_uid = ent ? ent->uid : -1;
            }
            enable_camera_bounds = true;
            set_camera_bounds(enable_camera_bounds);
        }

        else if (clicked("mouse_camera_drag"))
        {
            zooming = false;
            if (ImGui::IsMousePosValid())
            {
                startpos = normalize(mouse_pos());
            }
        }

        else if (dragging("mouse_camera_drag") && drag_delta("mouse_camera_drag") > 10.0f)
        {
            if (ImGui::IsMousePosValid())
            {
                g_state->camera->focused_entity_uid = -1;
                ImVec2 mpos = normalize(mouse_pos());
                std::pair<float, float> oryginal_pos = UI::click_position(startpos.x, startpos.y);
                std::pair<float, float> current_pos = UI::click_position(mpos.x, mpos.y);

                g_state->camera->focus_x -= (current_pos.first - oryginal_pos.first) * g_camera_speed;
                g_state->camera->focus_y -= (current_pos.second - oryginal_pos.second) * g_camera_speed;
                if (g_state->pause != 0 || g_bucket->pause_api->paused() || !options["smooth_camera"])
                    HeapBase::get().state()->camera->set_position(g_state->camera->focus_x, g_state->camera->focus_y);
                startpos = normalize(mouse_pos());
                enable_camera_bounds = false;
                set_camera_bounds(enable_camera_bounds);
            }
        }

        else if (held("mouse_blast") && ImGui::GetFrameCount() > g_last_gun + ImGui::GetIO().Framerate / 10)
        {
            g_last_gun = ImGui::GetFrameCount();
            set_pos(mouse_pos());
            UI::spawn_entity(to_id("ENT_TYPE_FX_ALIENBLAST"), g_x, g_y, true, g_vx, g_vy, options["snap_to_grid"]);
            g_x = 0;
            g_y = 0;
            g_vx = 0;
            g_vy = 0;
        }
        else if (held("mouse_boom") && ImGui::GetFrameCount() > g_last_gun + ImGui::GetIO().Framerate / 5)
        {
            g_last_gun = ImGui::GetFrameCount();
            set_pos(mouse_pos());
            UI::spawn_entity(to_id("ENT_TYPE_FX_EXPLOSION"), g_x, g_y, true, g_vx, g_vy, options["snap_to_grid"]);
            g_x = 0;
            g_y = 0;
            g_vx = 0;
            g_vy = 0;
        }
        else if (held("mouse_big_boom") && ImGui::GetFrameCount() > g_last_gun + ImGui::GetIO().Framerate / 5)
        {
            g_last_gun = ImGui::GetFrameCount();
            set_pos(mouse_pos());
            UI::spawn_entity(to_id("ENT_TYPE_FX_POWEREDEXPLOSION"), g_x, g_y, true, g_vx, g_vy, options["snap_to_grid"]);
            g_x = 0;
            g_y = 0;
            g_vx = 0;
            g_vy = 0;
        }
        else if (held("mouse_nuke") && ImGui::GetFrameCount() > g_last_gun + ImGui::GetIO().Framerate / 5)
        {
            g_last_gun = ImGui::GetFrameCount();
            set_pos(mouse_pos());
            static const auto powered_explosion = to_id("ENT_TYPE_FX_POWEREDEXPLOSION");
            UI::spawn_entity(powered_explosion, g_x, g_y, true, g_vx, g_vy, options["snap_to_grid"]);
            UI::spawn_entity(powered_explosion, g_x - 0.2f, g_y, true, g_vx, g_vy, options["snap_to_grid"]);
            UI::spawn_entity(powered_explosion, g_x + 0.2f, g_y, true, g_vx, g_vy, options["snap_to_grid"]);
            UI::spawn_entity(powered_explosion, g_x, g_y - 0.3f, true, g_vx, g_vy, options["snap_to_grid"]);
            UI::spawn_entity(powered_explosion, g_x, g_y + 0.3f, true, g_vx, g_vy, options["snap_to_grid"]);
            UI::spawn_entity(powered_explosion, g_x + 0.15f, g_y + 0.2f, true, g_vx, g_vy, options["snap_to_grid"]);
            UI::spawn_entity(powered_explosion, g_x - 0.15f, g_y + 0.2f, true, g_vx, g_vy, options["snap_to_grid"]);
            UI::spawn_entity(powered_explosion, g_x + 0.15f, g_y - 0.2f, true, g_vx, g_vy, options["snap_to_grid"]);
            UI::spawn_entity(powered_explosion, g_x - 0.15f, g_y - 0.2f, true, g_vx, g_vy, options["snap_to_grid"]);
            g_x = 0;
            g_y = 0;
            g_vx = 0;
            g_vy = 0;
        }
        else if (released("mouse_destroy") || released("mouse_destroy_unsafe"))
        {
            ImVec2 pos = mouse_pos();
            set_pos(pos);
            unsigned int mask = safe_entity_mask;
            if (released("mouse_destroy_unsafe"))
            {
                mask = unsafe_entity_mask;
            }
            Entity* to_kill = UI::get_entity_at(g_x, g_y, true, 2, mask);
            if (to_kill)
                smart_delete(to_kill, mask == unsafe_entity_mask);
            g_x = 0;
            g_y = 0;
            g_vx = 0;
            g_vy = 0;
            g_held_id = -1;
        }
    }

    ImGui::End();
    ImGui::PopStyleVar();
}

void render_style_editor()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImGuiIO& io = ImGui::GetIO();
    if (options["menu_ui"])
    {
        if (ImGui::MenuItem("Randomize"))
        {
            options["inverted"] = (float)rand() / RAND_MAX < 0.5f;
            g_hue = (float)rand() / RAND_MAX;
            g_sat = (float)rand() / RAND_MAX;
            g_val = (float)rand() / RAND_MAX;
            style.Alpha = (float)rand() / RAND_MAX * 0.5f + 0.4f;
        }
        ImGui::Separator();
    }
    ImGui::TextWrapped("Leave empty to use embedded font 'Hack'. You must save and restart for font changes to take effect.");
    ImGui::InputText("Font file##FontFile", &fontfile);
    tooltip("Just the filename, e.g. comic.ttf");
    ImGui::DragFloat("Small print##FontSmall", &fontsize[0], 0.1f, 6.0f, 32.0f);
    ImGui::DragFloat("Medium print##FontMedium", &fontsize[1], 0.1f, 16.0f, 48.0f);
    ImGui::DragFloat("Large print##FontLarge", &fontsize[2], 0.1f, 24.0f, 96.0f);
    ImGui::Separator();
    ImGui::Checkbox("Inverted (black on light colors)##StyleInvert", &options["inverted"]);
    ImGui::Checkbox("Borders##StyleBorder", &options["borders"]);
    ImGui::DragFloat("Hue##StyleHue", &g_hue, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("Saturation##StyleSaturation", &g_sat, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("Lightness##StyleLightness", &g_val, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("Alpha##StyleAlpha", &style.Alpha, 0.01f, 0.2f, 1.0f);
    ImGui::DragFloat("Scale##StyleScale", &io.FontGlobalScale, 0.01f, 0.2f, 2.0f);
    if (!options["menu_ui"])
    {
        if (ImGui::Button("Randomize##StyleRandomize"))
        {
            options["inverted"] = (float)rand() / RAND_MAX < 0.5f;
            g_hue = (float)rand() / RAND_MAX;
            g_sat = (float)rand() / RAND_MAX;
            g_val = (float)rand() / RAND_MAX;
            style.Alpha = (float)rand() / RAND_MAX * 0.5f + 0.4f;
        }
        ImGui::SameLine();
        if (ImGui::Button("Save options##StyleSave"))
        {
            save_config(cfgfile);
        }
        ImGui::SameLine();
        if (ImGui::Button("Load options##StyleLoad"))
        {
            load_config(cfgfile);
            refresh_script_files();
        }
    }
    set_colors();
}

void render_keyconfig()
{
    ImGui::PushID("keyconfig");
    if (ImGui::Button("Reset all keys to defaults##ResetKeys"))
    {
        keys = default_keys;
        save_config(cfgfile);
    }
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Block game input when holding");
    ImGui::SameLine();
    ImGui::CheckboxFlags("Ctrl##IgnoreCtrl", &g_bucket->pause_api->modifiers_block, OL_KEY_CTRL);
    ImGui::SameLine();
    ImGui::CheckboxFlags("Alt##IgnoreAlt", &g_bucket->pause_api->modifiers_block, OL_KEY_ALT);
    ImGui::SameLine();
    ImGui::CheckboxFlags("Shift##IgnoreShift", &g_bucket->pause_api->modifiers_block, OL_KEY_SHIFT);
    if (ImGui::Checkbox("Clear blocked input, instead of just ignoring events##IgnoreClear", &g_bucket->pause_api->modifiers_clear_input))
        options["modifiers_clear_input"] = g_bucket->pause_api->modifiers_clear_input;

    if (g_bucket->overlunky->ignore_keys.size())
    {
        std::string s;
        for (auto const& e : g_bucket->overlunky->ignore_keys)
        {
            s += e;
            s += ',';
        }
        s.pop_back();
        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "The following keys are disabled by scripts:");
        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "%s", s.c_str());
    }
    if (g_bucket->overlunky->ignore_keycodes.size())
    {
        std::string s;
        for (auto const& e : g_bucket->overlunky->ignore_keycodes)
        {
            s += fmt::format("0x{:x}", e);
            s += ',';
        }
        s.pop_back();
        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "The following keycodes are disabled by scripts:");
        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "%s", s.c_str());
    }
    ImGui::BeginTable("##keyconfig", 4);
    ImGui::TableSetupColumn("Tool");
    ImGui::TableSetupColumn("Keys");
    ImGui::TableSetupColumn("Hex keycode");
    ImGui::TableSetupColumn("");
    ImGui::TableHeadersRow();
    for (const auto& kv : keys)
    {
        ImGui::PushID(kv.first.c_str());
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("%s", kv.first.c_str());
        ImGui::TableNextColumn();
        ImGui::Text("%s", key_string(keys[kv.first]).c_str());
        ImGui::TableNextColumn();
        ImGui::InputScalar("##keycode", ImGuiDataType_S64, &keys[kv.first], 0, 0, "%X", ImGuiInputTextFlags_CharsHexadecimal);
        ImGui::TableNextColumn();
        if (ImGui::Button("Set"))
        {
            g_change_key = kv.first;
            save_config(cfgfile);
        }
        ImGui::SameLine();
        if (ImGui::Button("Unset"))
        {
            keys[kv.first] = 0;
            save_config(cfgfile);
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset"))
        {
            keys[kv.first] = default_keys[kv.first];
            save_config(cfgfile);
        }
        ImGui::PopID();
    }
    ImGui::EndTable();
    ImGui::PopID();

    if (g_change_key != "")
    {
        ImGuiIO& io = ImGui::GetIO();
        io.WantCaptureKeyboard = true;
        auto base = ImGui::GetMainViewport();
        ImGui::SetNextWindowSize(base->Size);
        ImGui::SetNextWindowPos(base->Pos);
        ImGui::SetNextWindowViewport(base->ID);
        ImGui::SetNextWindowBgAlpha(0.75);
        ImGui::Begin(
            "KeyCapture",
            NULL,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking);
        ImGui::InvisibleButton("KeyCaptureCanvas", ImGui::GetContentRegionMax(), ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
        ImDrawList* dl = ImGui::GetForegroundDrawList();
        ImGui::PushFont(bigfont);
        std::string buf = fmt::format("Enter new key/button combo for {}.\nModifiers Ctrl, Alt and Shift are available.", g_change_key);
        ImVec2 textsize = ImGui::CalcTextSize(buf.c_str());
        dl->AddText({base->Pos.x + base->Size.x / 2 - textsize.x / 2, base->Pos.y + base->Size.y / 2 - textsize.y / 2}, ImColor(1.0f, 1.0f, 1.0f, .8f), buf.c_str());
        ImGui::PopFont();

        // Buttons
        for (size_t i = 0; i < 5; ++i)
        {
            if (io.MouseDown[i])
            {
                size_t keycode = 0x400 + i + 1;
                if (ImGui::GetIO().KeyCtrl)
                    keycode += 0x100;
                if (ImGui::GetIO().KeyShift)
                    keycode += 0x200;
                if (ImGui::GetIO().KeyAlt)
                    keycode += 0x800;
                keys[g_change_key] = keycode;
                save_config(cfgfile);
                g_change_key = "";
            }
        }

        // Wheel
        if (io.MouseWheel != 0)
        {
            size_t keycode = 0x400;
            if (io.MouseWheel < 0)
                keycode += OL_WHEEL_DOWN;
            else if (io.MouseWheel > 0)
                keycode += OL_WHEEL_UP;
            if (ImGui::GetIO().KeyCtrl)
                keycode += 0x100;
            if (ImGui::GetIO().KeyShift)
                keycode += 0x200;
            if (ImGui::GetIO().KeyAlt)
                keycode += 0x800;
            keys[g_change_key] = keycode;
            save_config(cfgfile);
            g_change_key = "";
        }

        // Keys
        for (size_t i = 0; i < 0xFF; ++i)
        {
            if (ImGui::IsKeyReleased((ImGuiKey)i))
            {
                size_t keycode = i;
                if (ImGui::GetIO().KeyCtrl)
                    keycode += 0x100;
                if (ImGui::GetIO().KeyShift)
                    keycode += 0x200;
                if (ImGui::GetIO().KeyAlt)
                    keycode += 0x800;
                keys[g_change_key] = keycode;
                save_config(cfgfile);
                g_change_key = "";
            }
        }
        ImGui::End();
    }
}

void force_level_size()
{
    if (g_ui_scripts.find("level_size") == g_ui_scripts.end())
        return;
    g_ui_scripts["level_size"]->update_code(fmt::format(
        R"(
set_callback(function()
    if state.screen == SCREEN.LEVEL then
        state.width, state.height = {}, {}
    end
end, ON.PRE_LEVEL_GENERATION)
)",
        g_force_level_width,
        g_force_level_height));
}

void render_options()
{
    if (options["menu_ui"] && !detached("tool_options"))
    {
        if (ImGui::MenuItem("Switch to windowed UI", key_string(keys["switch_ui"]).c_str()))
            options["menu_ui"] = false;
    }
    if (submenu("Game cheats"))
    {
        if (ImGui::Checkbox("God mode (players)##Godmode", &options["god_mode"]))
        {
            UI::godmode(options["god_mode"]);
        }
        tooltip("Make the players completely deathproof.", "toggle_godmode");
        if (ImGui::Checkbox("God mode (companions)##GodmodeCompanions", &options["god_mode_companions"]))
        {
            UI::godmode_companions(options["god_mode_companions"]);
        }
        tooltip("Make the hired hands completely deathproof.");
        if (ImGui::Checkbox("Disable death screen##NoDeath", &death_disable))
        {
            UI::death_enabled(!death_disable);
        }
        tooltip("Disable the death screen from popping up for any reason.");
        if (ImGui::Checkbox("Noclip##Noclip", &options["noclip"]))
        {
            toggle_noclip();
        }
        tooltip("Fly through walls and ignored by enemies.", "toggle_noclip");
        ImGui::Checkbox("Fly mode##FlyMode", &options["fly_mode"]);
        tooltip("Fly while holding the jump button.", "toggle_flymode");
        if (ImGui::Checkbox("Light dark levels and layers##DrawLights", &options["lights"]))
            toggle_lights();

        tooltip("Enables the default level lighting everywhere.", "toggle_lights");
        if (ImGui::CheckboxFlags("Force dark levels", &g_dark_mode, 1))
        {
            clr_flag(g_dark_mode, 2);
            g_ui_scripts["light"]->set_enabled(false);
            g_ui_scripts["dark"]->set_enabled(test_flag(g_dark_mode, 1));
        }
        tooltip("Forces every level dark, including bosses, CO or camp.");
        if (ImGui::CheckboxFlags("Disable dark levels", &g_dark_mode, 2))
        {
            clr_flag(g_dark_mode, 1);
            g_ui_scripts["dark"]->set_enabled(false);
            g_ui_scripts["light"]->set_enabled(test_flag(g_dark_mode, 2));
        }
        tooltip("Forces every level to be lit af.");
        if (ImGui::Checkbox("Disable ghosts and time jelly", &options["disable_ghost_timer"]))
        {
            UI::set_time_ghost_enabled(!options["disable_ghost_timer"]);
            UI::set_time_jelly_enabled(!options["disable_ghost_timer"]);
            UI::set_cursepot_ghost_enabled(!options["disable_ghost_timer"]);
        }
        tooltip("Disables the timed ghost and jelly.", "toggle_ghost");
        if (ImGui::Checkbox("Disable pause menu", &options["disable_pause"]))
        {
            force_hud_flags();
        }
        tooltip("Disables manual or automatic pausing when alt+tabbed.\nUncheck and you can always pause, even when ankhed.", "toggle_disable_pause");
        if (ImGui::Checkbox("Block Steam achievements", &options["disable_achievements"]))
        {
            if (options["disable_achievements"])
                UI::steam_achievements(false);
            else
                UI::steam_achievements(true);
        }
        tooltip("Enable this if playing on Steam and don't want\nto unlock everything when fooling around.");
        if (ImGui::Checkbox("Block game saves", &options["disable_savegame"]))
        {
            if (options["disable_savegame"])
                hook_savegame();
        }
        tooltip("Enable this if you want to keep your\nsave game unaffected by tomfoolery.");

        bool void_mode = g_ui_scripts["void"]->is_enabled();
        if (ImGui::Checkbox("Void sandbox mode", &void_mode))
        {
            g_ui_scripts["void"]->set_enabled(void_mode);
            g_state->quest_flags = 1;
            g_state->loading = 1;
        }
        tooltip("Just you and the level borders, all alone...", "toggle_void");
        static bool level_size = false;
        if (ImGui::Checkbox("Force level size", &level_size))
        {
            g_ui_scripts["level_size"]->set_enabled(level_size);
            if (level_size)
                force_level_size();
        }
        tooltip("Force level size, can cause crashes if used in the wrong place.");
        if (level_size)
        {
            if (ImGui::SliderInt("Width##ForceWidth", &g_force_level_width, 1, 8, "%d", ImGuiSliderFlags_AlwaysClamp))
                force_level_size();
            if (ImGui::SliderInt("Height##ForceHeight", &g_force_level_height, 1, 15, "%d", ImGuiSliderFlags_AlwaysClamp))
                force_level_size();
        }

        ImGui::Checkbox("Fast menus and transitions##SpeedHackMenu", &options["speedhack"]);
        tooltip("Enable 10x speedhack automatically when not controlling a character.", "toggle_speedhack_auto");

        ImGui::Checkbox("Uncap unfocused FPS on start", &options["uncap_unfocused_fps"]);
        tooltip("Sets the unfocused FPS to unlimited automatically.");

        g_speedhack_ui_multiplier = UI::get_speedhack();
        if (ImGui::SliderFloat("Speedhack##SpeedHack", &g_speedhack_ui_multiplier, 0.1f, 10.f, "%.2fx"))
        {
            if (should_speedhack())
                options["speedhack"] = false;
            UI::speedhack(g_speedhack_ui_multiplier);
        }
        tooltip("Slow down or speed up everything,\nlike in Cheat Engine.", "speedhack_decrease");
        ImGui::SameLine();
        if (ImGui::Button("Reset##ResetSpeedhack"))
        {
            g_speedhack_ui_multiplier = 1.0f;
            UI::speedhack(g_speedhack_ui_multiplier);
        }
        if (ImGui::SliderScalar("Engine FPS##EngineFPS", ImGuiDataType_Double, &g_engine_fps, &fps_min, &fps_max, "%f"))
            update_frametimes();
        tooltip("Set target engine FPS. Always capped by max GPU FPS.\n0 = as fast as it can go.");
        ImGui::SameLine();
        if (ImGui::Button("Reset##ResetFPS"))
        {
            g_engine_fps = 60.0;
            update_frametimes();
        }

        if (ImGui::SliderScalar("Unfocused FPS##UnfocusedFPS", ImGuiDataType_Double, &g_unfocused_fps, &fps_min, &fps_max, "%f"))
            update_frametimes();
        tooltip("Set target unfocused FPS. Always capped by max Engine FPS.\n0 = as fast as it can go.");
        ImGui::SameLine();
        if (ImGui::Button("Reset##ResetUnfocusedFPS"))
        {
            g_unfocused_fps = 33.0;
            update_frametimes();
        }
        endmenu();
    }

    if (submenu("User interface"))
    {
        ImGui::Checkbox("HD cursor", &options["hd_cursor"]);
        tooltip("Enable the Spelunky HD cursor :)");
        ImGui::Checkbox("Mouse controls##clickevents", &options["mouse_control"]);
        tooltip("Enables to spawn entities, teleport and pick entities with mouse.\nDisable for scripts that require mouse clicking.", "toggle_mouse");
        ImGui::Checkbox("Keyboard controls##keyevents", &options["keyboard_control"]);
        tooltip("Enables all hotkeys.\nDisable for scripts that extensively use keyboard.");
        ImGui::Checkbox("Snap to grid##Snap", &options["snap_to_grid"]);
        tooltip("Spawn items etc snapped to grid.\nAlways enabled for floor.", "toggle_snap");
        ImGui::Checkbox("Spawn floor decorated##Decorate", &options["spawn_floor_decorated"]);
        tooltip("Add decorations to spawned floor.");
        ImGui::Checkbox("Draw hitboxes##DrawEntityBox", &options["draw_hitboxes"]);
        tooltip("Draw hitboxes for all movable and hovered entities. Also mouse tooltips.", "toggle_hitboxes");
        ImGui::SameLine();
        ImGui::Checkbox("interpolated##DrawRealBox", &options["draw_hitboxes_interpolated"]);
        tooltip("Use interpolated render position for smoother hitboxes on high fps.\nActual game logic is not interpolated like this though.");
        ImGui::Checkbox("Draw hovered entity tooltip##DrawEntityTooltip", &options["draw_entity_tooltip"]);
        tooltip("Draw entity names, uids and some random stuff for hovered entities.", "toggle_entity_tooltip");
        ImGui::Checkbox("Draw all entity info##DrawEntityInfo", &options["draw_entity_info"]);
        tooltip("Draw entity names, uids and some random stuff next to all entities.", "toggle_entity_info");
        ImGui::Checkbox("Draw gridlines##DrawTileGrid", &options["draw_grid"]);
        tooltip("Show outlines of tiles and rooms, with roomtypes.", "toggle_grid");
        ImGui::Checkbox("Draw path##DrawTileGrid", &options["draw_path"]);
        tooltip("Show path rooms.", "toggle_path");
        ImGui::Checkbox("Draw HUD##DrawHUD", &options["draw_hud"]);
        tooltip("Show enabled cheats and random\ninteresting state variables on screen.", "toggle_hud");
        ImGui::Checkbox("Draw hotbar##DrawHotbar", &options["draw_hotbar"]);
        tooltip("Show spawner hotbar on the bottom of the screen.", "toggle_hotbar");
        if (ImGui::Checkbox("Drag windows outside the game window", &options["multi_viewports"]))
        {
            if (options["multi_viewports"])
                ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
            else
                ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable;
        }
        tooltip("Allow dragging tools outside the main game window, to different monitor etc.\nMay work smoother with the -oldflip game command line switch.");

        if (ImGui::Checkbox("Enable vsync", &options["vsync"]))
            imgui_vsync(options["vsync"]);
        tooltip("Disabling game vsync may affect performance with external windows,\nfor better or worse.");

        if (ImGui::Checkbox("Docking only while holding Shift", &options["docking_with_shift"]))
        {
            ImGui::GetIO().ConfigDockingWithShift = options["docking_with_shift"];
        }
        tooltip("Allow annoying window docking only while holding Shift");

        ImGui::Checkbox("Enable unsafe scripts", &options["enable_unsafe_scripts"]);
        tooltip("Allow using unsafe Lua libraries in scripts.");

        ImGui::Checkbox("Menu UI, instead of a floating window", &options["menu_ui"]);
        tooltip("Puts everything in a main menu instead of a floating window.\nYou can still create individual windows by dragging from the contents.", "switch_ui");

        if (ImGui::Checkbox("Alternative Console Ctrl key behavior", &options["console_alt_keys"]))
            g_Console.get()->set_alt_keys(options["console_alt_keys"]);
        tooltip("Hold Ctrl to execute and scroll history,\nenter and arrows only edit buffer.");

        ImGui::Checkbox("Show tooltips", &options["show_tooltips"]);
        tooltip("Am I annoying you already :(");
        if (ImGui::Checkbox("Automatically check for updates", &options["update_check"]))
        {
            if (options["update_check"])
                version_check(true);
        }
        tooltip("Check if you're running the latest build of Overlunky WHIP on start.");
        endmenu();
    }

    if (submenu("Hitbox entity types to draw"))
    {
        ImGui::PushID("HitboxMask");
        for (int i = 0; i < 15; i++)
        {
            if (i % 2)
                ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.5f);
            ImGui::CheckboxFlags(mask_names[i], &g_hitbox_mask, (int)std::pow(2, i));
        }
        ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.5f);
        ImGui::CheckboxFlags("Traps & Misc", &g_hitbox_mask, (int)std::pow(2, 15));
        tooltip("Some cherry-picked entities like traps and invisible walls.");
        if (ImGui::Button("Defaults##RestoreDefaultHitboxMask"))
            g_hitbox_mask = default_hitbox_mask;
        ImGui::PopID();
        endmenu();
    }

    if (submenu("Entity types to grab by default"))
    {
        ImGui::PushID("PickerMask");
        for (int i = 0; i < 15; i++)
        {
            if (i % 2)
                ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.5f);
            ImGui::CheckboxFlags(mask_names[i], &safe_entity_mask, (int)std::pow(2, i));
        }
        if (ImGui::Button("Any##AnyEntityMask"))
            safe_entity_mask = 0;
        ImGui::SameLine();
        if (ImGui::Button("Defaults##RestoreDefaultEntityMask"))
            safe_entity_mask = default_entity_mask;
        ImGui::PopID();
        endmenu();
    }

    if (submenu("Frame advance / Pause options"))
    {
        if (submenu("Current pause flags"))
        {
            ImGui::PushID("CurrentPauseFlags");
            auto old_flags = g_bucket->pause_api->pause;
            render_flags(pause_types, &g_bucket->pause_api->pause, false);
            if (g_bucket->pause_api->pause != old_flags)
                g_bucket->pause_api->apply();
            ImGui::PopID();
            endmenu();
        }
        if (submenu("Toggled pause flags"))
        {
            ImGui::PushID("TogglePauseFlags");
            render_flags(pause_types, &g_bucket->pause_api->pause_type, false);
            ImGui::PopID();
            endmenu();
        }
        if (submenu("Ignore freeze in screens"))
        {
            ImGui::PushID("IgnorePauseScreen");
            render_flags(pause_screens, &g_bucket->pause_api->ignore_screen);
            ImGui::PopID();
            endmenu();
        }
        if (submenu("Ignore triggers in screens"))
        {
            ImGui::PushID("IgnorePauseScreenTrigger");
            render_flags(screen_names, &g_bucket->pause_api->ignore_screen_trigger);
            ImGui::PopID();
            endmenu();
        }
        if (submenu("Automatic pause triggers"))
        {
            bool pause_level = UI::get_start_level_paused();
            if (ImGui::Checkbox("Disable fade unpause on screen load", &pause_level))
                UI::set_start_level_paused(pause_level);
            render_flags(pause_triggers, &g_bucket->pause_api->pause_trigger, false);
            if ((g_bucket->pause_api->pause_trigger & PAUSE_TRIGGER::SCREEN) != PAUSE_TRIGGER::NONE)
            {
                ImGui::SeparatorText("Pause on screens (or any)");
                render_flags(screen_names, &g_bucket->pause_api->pause_screen);
            }
            endmenu();
        }
        if (submenu("Automatic unpause triggers"))
        {
            render_flags(pause_triggers, &g_bucket->pause_api->unpause_trigger, false);
            if ((g_bucket->pause_api->unpause_trigger & PAUSE_TRIGGER::SCREEN) != PAUSE_TRIGGER::NONE)
            {
                ImGui::SeparatorText("Unpause on screens (or any)");
                render_flags(pause_screens, &g_bucket->pause_api->unpause_screen);
            }
            endmenu();
        }
        bool paused = g_bucket->pause_api->paused();
        if (ImGui::Checkbox("Paused##PauseSim", &paused))
            toggle_pause();
        tooltip("Toggle current pause API state according to the toggled type.", "toggle_pause");
        if (ImGui::Checkbox("Skip fades##SkipFades", &g_bucket->pause_api->skip_fade))
            options["skip_fades"] = g_bucket->pause_api->skip_fade;
        tooltip("Skips all fade to black / circle wipe transitions.");
        if (ImGui::Checkbox("Update camera position during pause##PauseCamera", &g_bucket->pause_api->update_camera))
            options["pause_update_camera"] = g_bucket->pause_api->update_camera;
        tooltip("Calls the vanilla camera update when it\nwould be skipped by freezing the state update.");
        if (ImGui::Checkbox("Run in last API instance##PauseLast", &g_bucket->pause_api->last_instance))
            options["pause_last_instance"] = g_bucket->pause_api->last_instance;
        tooltip("Runs freeze logic and triggers only in Playlunky\nwhen both modding tools are injected.");

        ImGui::Separator();
        ImGui::TextWrapped("- The %s and %s keys will only toggle the pause types listed above, i.e. blocking updates during a normal game pause won't interfere with the vanilla pause", key_string(keys["toggle_pause"]).c_str(), key_string(keys["frame_advance"]).c_str());
        ImGui::TextWrapped("- The freeze options will block the game in the specified callback (as well as enforce any selected normal pause flags, for now)");
        ImGui::TextWrapped("- Using the freeze options without a camera hack will induce weird camera flickering");
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
        if (keys["toggle_pause"] & VK_SPACE || keys["frame_advance"] & VK_SPACE || keys["frame_advance_alt"] & VK_SPACE)
            ImGui::TextWrapped("- frame_advance/toggle_pause is bound to Space, normal menu input using Space will be disabled (use Z)");
        if ((uint8_t)g_bucket->pause_api->pause_type & 0x3f || UI::get_start_level_paused())
            ImGui::TextWrapped("- Using vanilla state.pause flags is obsolete and advanced UI features for it might be removed soon");
        if ((g_bucket->pause_api->pause_type & PAUSE_TYPE::FADE) == PAUSE_TYPE::NONE && UI::get_start_level_paused())
            ImGui::TextWrapped("- You should enable toggling fade pauses if you enable the fade pause hack");
        ImGui::PopStyleColor();
        endmenu();
    }

    if (submenu("Window style"))
    {
        render_style_editor();
        endmenu();
    }
    if (submenu("Shortcut keys"))
    {
        render_keyconfig();
        endmenu();
    }

    if (options["menu_ui"])
    {
        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("Check for updates"))
                version_check(true);
            if (ImGui::MenuItem("Get latest version here"))
                ShellExecuteA(NULL, "open", "https://github.com/spelunky-fyi/overlunky/releases/tag/whip", NULL, NULL, SW_SHOWNORMAL);
            if (ImGui::MenuItem("README"))
                ShellExecuteA(NULL, "open", "https://github.com/spelunky-fyi/overlunky#overlunky", NULL, NULL, SW_SHOWNORMAL);
            if (ImGui::MenuItem("API Documentation"))
                ShellExecuteA(NULL, "open", "https://spelunky-fyi.github.io/overlunky/", NULL, NULL, SW_SHOWNORMAL);
            ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Save options", key_string(keys["save_settings"]).c_str()))
            save_config(cfgfile);
        if (ImGui::MenuItem("Load options", key_string(keys["load_settings"]).c_str()))
            load_config(cfgfile);
        return;
    }

    if (ImGui::Button("Save options"))
    {
        ImGui::SaveIniSettingsToDisk(inifile);
        save_config(cfgfile);
    }
    tooltip("Save current options to overlunky.ini.\nThis is not done automatically!", "save_settings");
    ImGui::SameLine();
    if (ImGui::Button("Load options"))
    {
        ImGui::LoadIniSettingsFromDisk(inifile);
        load_config(cfgfile);
        refresh_script_files();
        set_colors();
    }
    tooltip("Load overlunky.ini.", "load_settings");
}

void render_debug()
{
    ImGui::PushItemWidth(-ImGui::GetWindowWidth() * 0.5f);
    const auto selected_entity = get_entity_ptr(g_last_id);
    size_t entity_addr = reinterpret_cast<size_t>(selected_entity);
    size_t state_addr = reinterpret_cast<size_t>(g_state);
    size_t save_addr = reinterpret_cast<size_t>(g_save);
    ImGui::InputScalar("State##StatePointer", ImGuiDataType_U64, &state_addr, 0, 0, "%p", ImGuiInputTextFlags_ReadOnly);
    ImGui::InputScalar("Entity##EntityPointer", ImGuiDataType_U64, &entity_addr, 0, 0, "%p", ImGuiInputTextFlags_ReadOnly);
    ImGui::InputScalar("Save##SavePointer", ImGuiDataType_U64, &save_addr, 0, 0, "%p", ImGuiInputTextFlags_ReadOnly);
    ImGui::InputScalar(
        "Level flags##HudFlagsDebug",
        ImGuiDataType_U32,
        &g_state->level_flags,
        0,
        0,
        "%08X",
        ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AlwaysInsertMode | ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::InputScalar(
        "Journal flags##JournalFlagsDebug",
        ImGuiDataType_U32,
        &g_state->journal_flags,
        0,
        0,
        "%08X",
        ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AlwaysInsertMode | ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::InputScalar(
        "Quest flags##QuestFlagsDebug",
        ImGuiDataType_U32,
        &g_state->quest_flags,
        0,
        0,
        "%08X",
        ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AlwaysInsertMode | ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::PopItemWidth();
}

std::string gen_random(const int len)
{
    std::string tmp_s;
    static const char alphanum[] = "0123456789"
                                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                   "abcdefghijklmnopqrstuvwxyz";
    tmp_s.reserve(len);
    for (int i = 0; i < len; ++i)
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    return tmp_s;
}

void set_default_path(IFileDialog* dialog, std::wstring pathStr)
{
    const wchar_t* defaultPathW = pathStr.c_str();

    IShellItem* folder;
    HRESULT result = SHCreateItemFromParsingName(defaultPathW, NULL, IID_PPV_ARGS(&folder));

    // Valid non results.
    if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) || result == HRESULT_FROM_WIN32(ERROR_INVALID_DRIVE))
        return;

    if (!SUCCEEDED(result))
        return;

    dialog->SetFolder(folder);
    folder->Release();
}

std::vector<std::wstring> select_files(std::wstring default_path, DWORD type = 0)
{
    HRESULT hr = S_OK;
    std::vector<std::wstring> filePaths;

    IFileOpenDialog* fileDlg = NULL;
    hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&fileDlg));
    if (FAILED(hr))
        return filePaths;
    ON_SCOPE_EXIT(fileDlg->Release());
    set_default_path(fileDlg, default_path);

    IKnownFolderManager* pkfm = NULL;
    hr = CoCreateInstance(CLSID_KnownFolderManager, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pkfm));
    if (FAILED(hr))
        return filePaths;
    ON_SCOPE_EXIT(pkfm->Release());

    IKnownFolder* pKnownFolder = NULL;
    hr = pkfm->GetFolder(FOLDERID_PublicMusic, &pKnownFolder);
    if (FAILED(hr))
        return filePaths;
    ON_SCOPE_EXIT(pKnownFolder->Release());

    IShellItem* psi = NULL;
    hr = pKnownFolder->GetShellItem(0, IID_PPV_ARGS(&psi));
    if (FAILED(hr))
        return filePaths;
    ON_SCOPE_EXIT(psi->Release());

    hr = fileDlg->AddPlace(psi, FDAP_BOTTOM);

    DWORD dwOptions;
    fileDlg->GetOptions(&dwOptions);
    fileDlg->SetOptions(dwOptions | type);
    hr = fileDlg->Show(NULL);
    if (SUCCEEDED(hr))
    {
        IShellItemArray* pRets;
        hr = fileDlg->GetResults(&pRets);
        if (SUCCEEDED(hr))
        {
            DWORD count;
            pRets->GetCount(&count);
            for (DWORD i = 0; i < count; i++)
            {
                IShellItem* pRet;
                LPWSTR nameBuffer;
                pRets->GetItemAt(i, &pRet);
                pRet->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &nameBuffer);
                filePaths.push_back(std::wstring(nameBuffer));
                pRet->Release();
                CoTaskMemFree(nameBuffer);
            }
            pRets->Release();
        }
    }
    return filePaths;
}

void set_script_dir()
{
    TCHAR buffer[MAX_PATH] = {0};
    GetModuleFileName(NULL, buffer, MAX_PATH);
    auto path = std::filesystem::path(std::string(buffer)).parent_path();
    DEBUG("def {}", path.string());
    ShowCursor(true);
    auto folder = select_files(path.wstring(), FOS_PICKFOLDERS);
    if (!folder.empty())
    {
        scriptpath = std::string(cvt.to_bytes(folder.at(0)));
        std::replace(scriptpath.begin(), scriptpath.end(), '\\', '/');
        save_config(cfgfile);
        refresh_script_files();
    }
    ShowCursor(false);
}

void render_script_files()
{
    ImGui::PushID("files");
    int num = 0;
    for (auto& file : g_script_files)
    {
        auto id = file.string();
        std::replace(id.begin(), id.end(), '\\', '/');
        if (g_scripts.count(id) > 0)
            continue;

        ImGui::PushID(num++);
        auto buttpath = file.parent_path().filename() / file.filename();
        std::wstring wbuttstr = buttpath.wstring();
        std::string buttstr(cvt.to_bytes(wbuttstr));

        if (ImGui::Button(buttstr.c_str()))
        {
            load_script(file.wstring(), false);
        }
        ImGui::PopID();
    }
    std::filesystem::path path = scriptpath;
    std::string abspath = scriptpath;
    if (std::filesystem::exists(abspath) && std::filesystem::is_directory(abspath))
    {
        abspath = std::filesystem::absolute(path).string();
    }
    if (g_script_files.size() == 0)
    {
        ImGui::TextWrapped("No scripts found. Put .lua files in '%s' or change script_dir in the ini file and reload.", abspath.c_str());
    }
    else if (g_script_files.size() > 0 && num == 0)
    {
        ImGui::TextWrapped("All scripts found in '%s' are already loaded.", abspath.c_str());
    }
    if (ImGui::Button("Select script directory##SelectScriptDir"))
        set_script_dir();
    tooltip(scriptpath.c_str());
    if (ImGui::Button("Reload config and refresh scripts##RefreshScripts"))
    {
        load_config(cfgfile);
        refresh_script_files();
    }
    ImGui::PopID();
}

bool lower_test(char l, char r)
{
    return (std::tolower(l) == std::tolower(r));
}

bool find_match(std::string needle, std::string haystack)
{
    std::string::iterator fpos = std::search(haystack.begin(), haystack.end(), needle.begin(), needle.end(), lower_test);
    return fpos != haystack.end();
}

void render_scripts()
{
    ImGui::PushTextWrapPos(0.0f);
    ImGui::PopTextWrapPos();
    ImGui::Checkbox("Draw script messages##DrawScriptMessages", &options["draw_script_messages"]);
    ImGui::SameLine();
    ImGui::Checkbox("to console##ConsoleScriptMessages", &options["console_script_messages"]);
    ImGui::Checkbox("Fade script messages##FadeScriptMessages", &options["fade_script_messages"]);
    ImGui::SameLine();
    if (ImGui::Button("Clear##ClearMessages"))
        clear_script_messages();
    tooltip("Clear all script messages from screen", "clear_messages");
    if (ImGui::Checkbox("Load scripts from script directory##LoadScriptsDefault", &options["load_scripts"]))
        refresh_script_files();
    ImGui::SameLine();
    if (ImGui::Button("Change##SetScriptDir"))
        set_script_dir();
    tooltip(scriptpath.c_str());
    if (ImGui::Checkbox("Load scripts from Mods/Packs##LoadScriptsPacks", &options["load_packs"]))
        refresh_script_files();
    if (ImGui::Button("Create new quick script"))
    {
        std::string name = "_" + gen_random(16);
        SpelunkyScript* script = new SpelunkyScript(
            "meta.name = 'Script'\nmeta.version = '0.1'\nmeta.description = 'This script will not be saved anywhere but can be used to test things quickly!'\nmeta.author = 'You'\n\ncount = 0\nid = "
            "set_interval(function()\n  count = count + 1\n  message('Hello from your shiny new script')\n  if count > 4 then clear_callback(id) "
            "end\nend, 60)",
            name,
            g_SoundManager.get(),
            g_Console.get(),
            true);
        g_scripts[name] = std::unique_ptr<SpelunkyScript>(script);
    }
    ImGui::SameLine();
    static bool enabled_only{false};
    ImGui::Checkbox("Hide disabled##EnabledScriptsOnly", &enabled_only);
    static std::string script_filter;
    ImGui::InputText("Search##ScriptFilter", &script_filter);
    ImGui::SameLine();
    if (ImGui::Button("Clear##ClearScriptFilter"))
        script_filter.clear();
    ImGui::PushItemWidth(-1);
    int i = 0;
    std::vector<std::string> unload_scripts;
    ImVec4 origcolor = ImGui::GetStyle().Colors[ImGuiCol_Header];
    ImVec4 origtextcolor = ImGui::GetStyle().Colors[ImGuiCol_Text];
    float gray = (origcolor.x + origcolor.y + origcolor.z) / 3.0f;
    ImVec4 disabledcolor = ImVec4(gray, gray, gray, 0.5f);
    ImGui::Separator();
    for (auto& [script_name, script] : g_scripts)
    {
        if (enabled_only && !script->is_enabled())
            continue;
        if (!script_filter.empty() && !find_match(script_filter, script->get_name() + " " + script->get_path() + " " + script->get_id()))
            continue;
        ImGui::PushID(i);
        ImGui::PushID(script_name.c_str());
        std::string filename;
        size_t slash = script->get_file().find_last_of("/\\");
        if (slash != std::string::npos)
            filename = script->get_file().substr(slash + 1);
        std::string name = fmt::format("{} ({})", script->get_name(), filename);
        if (!script->is_enabled())
        {
            ImGui::PushStyleColor(ImGuiCol_Header, disabledcolor);
            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
        }
        else
        {
            ImGui::PushStyleColor(ImGuiCol_Header, origcolor);
            ImGui::PushStyleColor(ImGuiCol_Text, origtextcolor);
        }
        if (submenu(name.c_str()))
        {
            ImGui::PopStyleColor();
            ImGui::TextWrapped(
                "%s %s by %s (%s)",
                script->get_name().c_str(),
                script->get_version().c_str(),
                script->get_author().c_str(),
                script->get_id().c_str());
            ImGui::TextWrapped("%s", script->get_description().c_str());
            if (!script->get_unsafe() || options["enable_unsafe_scripts"])
            {
                static bool run_unsafe = false;
                if (script->get_unsafe() && !script->is_enabled())
                {
                    ImGui::Separator();
                    ImGui::PushTextWrapPos(0.0f);
                    ImGui::TextColored(
                        ImVec4(1.0f, 0.3f, 0.3f, 1.0f),
                        "Warning: This script uses unsafe commands and it could delete your files or download viruses. Only enable this script if "
                        "you trust the author, have read the whole script or made it yourself.");
                    ImGui::PopTextWrapPos();
                    ImGui::Checkbox("I understand the risks.", &run_unsafe);
                    ImGui::Separator();
                }
                if (!script->get_unsafe() || run_unsafe || script->is_enabled())
                {
                    bool enabled = script->is_enabled();
                    if (ImGui::Checkbox("Enabled##EnabledScript", &enabled))
                        script->set_enabled(enabled);
                    if (script->get_path() != "" && !script->get_path().starts_with("Mods/Packs"))
                    {
                        ImGui::SameLine();
                        bool autorun = std::find(g_script_autorun.begin(), g_script_autorun.end(), filename) != g_script_autorun.end();
                        if (ImGui::Checkbox("Autorun##AutorunScript", &autorun))
                        {
                            if (!autorun)
                                g_script_autorun.erase(std::remove(g_script_autorun.begin(), g_script_autorun.end(), filename), g_script_autorun.end());
                            else if (std::find(g_script_autorun.begin(), g_script_autorun.end(), filename) == g_script_autorun.end())
                                g_script_autorun.push_back(filename);
                            save_config(cfgfile);
                        }
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Reload##ReloadScript"))
                    {
                        load_script(script->get_file(), script->is_enabled());
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Unload##UnloadScript"))
                    {
                        unload_scripts.push_back(script->get_file());
                    }
                    else
                    {
                        ++i;
                    }
                    ImGui::PushItemWidth(-ImGui::GetWindowWidth() * 0.5f);
                    ImGui::Separator();
                    script->render_options();
                    ImGui::PopItemWidth();
                    const ImVec2 dump = {-1.0f, 300.0f};
                    if (ImGui::InputTextMultiline("##LuaScript", &script->get_code(), dump))
                    {
                        script->set_changed(true);
                    }
                    std::string result = script->get_result();
                    ImGui::InputText("##LuaResult", &result, ImGuiInputTextFlags_ReadOnly);
                }
            }
            else
            {
                ImGui::TextWrapped("\nYou have not enabled running unsafe scripts. Bye.");
            }
            endmenu();
        }
        else
        {
            ImGui::PopStyleColor();
            ++i;
        }
        ImGui::PopStyleColor();
        ImGui::PopID();
        ImGui::PopID();
    }
    for (auto& id : unload_scripts)
    {
        auto it = g_scripts.find(id);
        if (it != g_scripts.end())
            g_scripts.erase(id);
    }
    if (submenu("Load new script##LoadScriptFile"))
    {
        render_script_files();
        endmenu();
    }
    ImGui::PopItemWidth();
}

std::string format_time(int64_t frames)
{
    struct tm newtime;
    time_t secs = frames / 60;
    char time[32];
    gmtime_s(&newtime, &secs);
    size_t endpos = std::strftime(time, sizeof(time), "%H:%M:%S", &newtime);
    snprintf(time + endpos, sizeof time - endpos, ".%03d", (int)((frames % 60) * 1000 / 60));
    return time;
}

int parse_time(std::string time)
{
    std::tm tm = {};
    std::stringstream ss(time);
    ss >> std::get_time(&tm, "%H:%M:%S");
    const auto pos = time.find_last_of(".");
    int frames = 0;
    if (pos != std::string::npos)
    {
        double dec = 0;
        std::stringstream sff(time.substr(pos));
        sff >> dec;
        dec += 0.001;
        frames = static_cast<int>(dec * 60);
    }
    return 60 * (tm.tm_hour * 60 * 60 + tm.tm_min * 60 + tm.tm_sec) + frames;
}

void render_savegame()
{
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
    if (options["disable_savegame"])
    {
        ImGui::TextWrapped("Warning: You have blocked game saves in the options, all changes to the save file will be temporary unless you click the big button below...");
    }
    else
    {
        ImGui::TextWrapped("Warning: Changes to the game save are not saved to file automatically, you have to click the big button below...");
    }
    ImGui::PopStyleColor(1);

    ImGui::PushID("Journal");
    if (submenu("Journal"))
    {
        indent(16.0f);
        ImGui::PushID("Places");
        if (submenu("Places"))
        {
            for (int i = 0; i < 16; ++i)
            {
                ImGui::PushID(i);
                ImGui::Checkbox(places_flags[i], &g_save->places[i]);
                ImGui::PopID();
            }
            endmenu();
        }
        ImGui::PopID();
        ImGui::PushID("People");
        if (submenu("People"))
        {
            ImGui::Text("");
            ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.5f);
            ImGui::Text("Killed");
            ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.75f);
            ImGui::Text("By");
            for (int i = 0; i < 38; ++i)
            {
                ImGui::PushID(i);
                ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.245f);
                ImGui::Checkbox(people_flags[i], &g_save->people[i]);
                ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.5f);
                ImGui::PushID("killed");
                ImGui::DragInt("", &g_save->people_killed[i], 0.5f, 0, INT_MAX);
                ImGui::PopID();
                ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.75f);
                ImGui::PushID("killed_by");
                ImGui::DragInt("", &g_save->people_killed_by[i], 0.5f, 0, INT_MAX);
                ImGui::PopID();
                ImGui::PopItemWidth();
                ImGui::PopID();
            }
            endmenu();
        }
        ImGui::PopID();
        ImGui::PushID("Bestiary");
        if (submenu("Bestiary"))
        {
            ImGui::Text("");
            ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.5f);
            ImGui::Text("Killed");
            ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.75f);
            ImGui::Text("By");
            for (int i = 0; i < 78; ++i)
            {
                ImGui::PushID(i);
                ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.245f);
                ImGui::Checkbox(bestiary_flags[i], &g_save->bestiary[i]);
                ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.5f);
                ImGui::PushID("killed");
                ImGui::DragInt("", &g_save->bestiary_killed[i], 0.5f, 0, INT_MAX);
                ImGui::PopID();
                ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.75f);
                ImGui::PushID("killed_by");
                ImGui::DragInt("", &g_save->bestiary_killed_by[i], 0.5f, 0, INT_MAX);
                ImGui::PopID();
                ImGui::PopItemWidth();
                ImGui::PopID();
            }
            endmenu();
        }
        ImGui::PopID();
        ImGui::PushID("Items");
        if (submenu("Items"))
        {
            for (int i = 0; i < 54; ++i)
            {
                ImGui::PushID(i);
                ImGui::Checkbox(items_flags[i], &g_save->items[i]);
                ImGui::PopID();
            }
            endmenu();
        }
        ImGui::PopID();
        ImGui::PushID("Traps");
        if (submenu("Traps"))
        {
            for (int i = 0; i < 24; ++i)
            {
                ImGui::PushID(i);
                ImGui::Checkbox(traps_flags[i], &g_save->traps[i]);
                ImGui::PopID();
            }
            endmenu();
        }
        ImGui::PopID();
        unindent(16.0f);
        endmenu();
    }
    ImGui::PopID();

    ImGui::PushID("Characters");
    if (submenu("Characters"))
    {
        ImGui::Text("");
        ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.75f);
        ImGui::Text("Deaths");
        for (int i = 0; i < 20; ++i)
        {
            ImGui::PushID(i);
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.245f);
            ImGui::CheckboxFlags(people_flags[i], &g_save->characters, (int)std::pow(2, i));
            ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.75f);
            ImGui::PushID("character_deaths");
            ImGui::DragInt("", &g_save->character_deaths[i], 0.5f, 0, INT_MAX);
            ImGui::PopID();
            ImGui::PopItemWidth();
            ImGui::PopID();
        }
        endmenu();
    }
    ImGui::PopID();

    ImGui::PushID("Shortcuts");
    if (submenu("Shortcuts"))
    {
        int current = g_save->shortcuts;
        for (int i = 0; i < 11; ++i)
        {
            ImGui::PushID(i);
            ImGui::RadioButton(shortcut_flags[i], &current, i);
            ImGui::PopID();
        }
        if (g_save->shortcuts != static_cast<uint8_t>(current))
            g_save->shortcuts = static_cast<uint8_t>(current);
        endmenu();
    }
    ImGui::PopID();

    ImGui::PushID("Player Profile");
    if (submenu("Player Profile"))
    {
        ImGui::PushItemWidth(-ImGui::GetContentRegionAvail().x * 0.5f);
        ImGui::DragInt("Plays", &g_save->plays);
        ImGui::DragInt("Deaths", &g_save->deaths);
        ImGui::DragInt("Normal wins", &g_save->wins_normal);
        ImGui::DragInt("Hard wins", &g_save->wins_hard);
        ImGui::DragInt("Special wins", &g_save->wins_special);
        ImGui::DragScalar("Total score", ImGuiDataType_S64, &g_save->score_total, 1000.0f, &s64_zero, &s64_max);
        ImGui::DragInt("Top score", &g_save->score_top, 1000.0f, 0, INT_MAX);
        SliderByte("Deepest area", (char*)&g_save->deepest_area, 1, 8);
        SliderByte("Deepest level", (char*)&g_save->deepest_level, 1, 99);
        std::string besttime = format_time(g_save->time_best);
        if (ImGui::InputText("Best time##BestTime", &besttime))
        {
            g_save->time_best = parse_time(besttime);
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset##ResetBestTime"))
            g_save->time_best = -1;
        std::string totaltime = format_time(g_save->time_total);
        if (ImGui::InputText("Total time##BestTime", &totaltime))
        {
            g_save->time_total = parse_time(totaltime);
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset##ResetTotalTime"))
            g_save->time_total = -1;
        std::string tutorialtime = format_time(g_save->time_tutorial);
        if (ImGui::InputText("Tutorial time##TutorialTime", &tutorialtime))
        {
            g_save->time_tutorial = parse_time(tutorialtime);
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset##ResetTutorialTime"))
            g_save->time_tutorial = -1;
        ImGui::Checkbox("Completed normal", &g_save->completed_normal);
        ImGui::Checkbox("Completed ironman", &g_save->completed_ironman);
        ImGui::Checkbox("Completed hard", &g_save->completed_hard);
        ImGui::PopItemWidth();
        endmenu();
    }
    ImGui::PopID();

    ImGui::PushID("Last Game Played");
    if (submenu("Last Game Played"))
    {
        SliderByte("World##LastWorld", (char*)&g_save->world_last, 1, 8);
        SliderByte("Level##LastLevel", (char*)&g_save->level_last, 1, 99);
        SliderByte("Theme##LastTheme", (char*)&g_save->theme_last, 1, 16);
        ImGui::DragScalar("Score##LastScore", ImGuiDataType_S32, &g_save->score_last, 1000.0f, &s32_zero, &s32_max);
        std::string lasttime = format_time(g_save->time_last);
        if (ImGui::InputText("Time##LastTime", &lasttime))
        {
            g_save->time_last = parse_time(lasttime);
        }
        for (int i = 0; i < 9; ++i)
        {
            ImGui::PushID(i);
            ImGui::InputInt("Sticker", (int32_t*)&g_save->stickers[i]);
            ImGui::PopID();
        }
        endmenu();
    }
    ImGui::PopID();

    ImGui::PushID("Miscellaneous");
    if (submenu("Miscellaneous"))
    {
        bool tutorialcomplete = g_save->tutorial_state > 2;
        if (ImGui::Checkbox("Tutorial completed", &tutorialcomplete))
        {
            if (tutorialcomplete)
                g_save->tutorial_state = 4;
            else
                g_save->tutorial_state = 0;
        }
        ImGui::Checkbox("Seeded runs unlocked", &g_save->seeded_unlocked);
        ImGui::Checkbox("Profile seen", &g_save->profile_seen);
        for (int s = 0; s < 4; ++s)
        {
            auto lbl = fmt::format("Player {}", s + 1);
            int plr = g_save->players[s];
            if (ImGui::BeginCombo(lbl.c_str(), people_flags[plr]))
            {
                for (uint8_t i = 0; i < 20; ++i)
                {
                    bool isSelected = (plr == g_save->players[s]);
                    if (ImGui::Selectable(people_flags[i], isSelected))
                    {
                        g_save->players[s] = i;
                    }
                }
                ImGui::EndCombo();
            }
        }
        ImGui::SliderScalar("Rescued dogs", ImGuiDataType_U8, &g_save->pets_rescued[0], &u8_min, &u8_max);
        ImGui::SliderScalar("Rescued cats", ImGuiDataType_U8, &g_save->pets_rescued[1], &u8_min, &u8_max);
        ImGui::SliderScalar("Rescued hamsters", ImGuiDataType_U8, &g_save->pets_rescued[2], &u8_min, &u8_max);
        ImGui::InputText("Last daily", g_save->last_daily, sizeof(g_save->last_daily) + 1, 0);
        endmenu();
    }
    ImGui::PopID();

    ImGui::PushID("Feats");
    if (submenu("Steam Achievements"))
    {
        ImGui::Checkbox("I know what I'm doing, unlock editing!##EditAchievements", &edit_achievements);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
        ImGui::TextWrapped("Warning: Touching anything in here will edit your real Steam Achievements directly. If you're using the Steam emulator, this only affects the achievements saved in the emulator.");
        if (options["disable_achievements"])
        {
            ImGui::TextWrapped("Warning: You have blocked getting Steam Achievements in the options, but you can still reset them individually here.");
        }
        ImGui::PopStyleColor();
        ImGui::Separator();
        ImGui::BeginDisabled(!edit_achievements);
        for (size_t i = 0; i < g_AllAchievements.size(); ++i)
        {
            ImGui::PushID(g_AllAchievements[i]);
            bool achieved;
            bool found = get_steam_achievement(g_AllAchievements[i], &achieved);
            if (found)
            {
                if (ImGui::Checkbox(g_AchievementNames[i], &achieved))
                {
                    set_steam_achievement(g_AllAchievements[i], achieved);
                }
            }
            ImGui::PopID();
        }
        ImGui::EndDisabled();
        ImGui::Text(" ");
        ImGui::BeginDisabled(!edit_achievements || options["disable_achievements"]);
        if (ImGui::Button("Reset all Steam Achievements!"))
        {
            reset_all_steam_achievements();
        }
        ImGui::EndDisabled();
        endmenu();
    }
    else
    {
        edit_achievements = false;
    }
    ImGui::PopID();

    ImGui::PushID("UnlockAll");
    if (options["menu_ui"])
        ImGui::SetNextWindowSize({400, -1});
    if (submenu("Save changes or unlock everything"))
    {
        ImGui::PushFont(bigfont);
        ImGui::PushItemWidth(ImGui::GetContentRegionMax().x);
        if (ImGui::Button("Save changes", {ImGui::GetContentRegionMax().x, 0}))
        {
            const bool last_state = options["disable_savegame"];
            options["disable_savegame"] = false;
            UI::save_progress();
            options["disable_savegame"] = last_state;
        }
        if (ImGui::Button("Unlock Everything*", {ImGui::GetContentRegionMax().x, 0}))
        {
            g_save->tutorial_state = 4;
            g_save->profile_seen = true;
            g_save->seeded_unlocked = true;
            g_save->characters = 0xfffff;
            g_save->shortcuts = 0xa;
            g_save->deepest_area = 7;
            g_save->deepest_level = 4;
        }
        ImGui::PopItemWidth();
        ImGui::PopFont();
        ImGui::TextWrapped("*Tutorial, seeded, characters, shortcuts, camp");
        endmenu();
    }
    ImGui::PopID();
}

bool render_powerup(int uid, const char* section)
{
    std::string uidc = std::to_string(uid);
    int ptype = UI::get_entity_type(uid);
    if (ptype == 0)
        return false;
    std::string typec = std::to_string(ptype);
    const char* pname = entity_names[ptype].c_str();
    ImGui::PushID(section);
    if (ImGui::Button(uidc.c_str()))
    {
        g_last_id = uid;
        update_entity();
    }
    ImGui::SameLine();
    ImGui::Text("%s", typec.c_str());
    ImGui::SameLine();
    ImGui::Text("%s", pname);
    ImGui::SameLine();
    ImGui::PushID(uid);
    auto remove = ImGui::Button("Remove##RemovePowerup");
    ImGui::PopID();
    ImGui::PopID();
    return remove;
}

void render_state(const char* label, uint8_t state)
{
    if (state <= 22)
        ImGui::LabelText(label, "%s", char_states[state]);
    else
        ImGui::LabelText(label, "%s", std::to_string(state).c_str());
}

void render_ai(const char* label, int state)
{
    if (state == 0)
        ImGui::LabelText(label, "Idling");
    else if (state == 1)
        ImGui::LabelText(label, "Walking");
    else if (state == 2)
        ImGui::LabelText(label, "Jumping");
    else if (state == 4)
        ImGui::LabelText(label, "Dead");
    else if (state == 5)
        ImGui::LabelText(label, "Jumping");
    else if (state == 6)
        ImGui::LabelText(label, "Attacking");
    else if (state == 7)
        ImGui::LabelText(label, "Meleeing");
    else if (state == 11)
        ImGui::LabelText(label, "Rolling");
    else
    {
        std::string statec = std::to_string(state);
        ImGui::LabelText(label, "%s", statec.c_str());
    }
}

void render_screen(const char* label, uint32_t state)
{
    if (state <= 29)
        ImGui::LabelText(label, "%d: %s", state, screen_names[state]);
    else
        ImGui::LabelText(label, "%d", state);
}

void render_entity_finder()
{
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
    static std::string search_entity_name = "";
    if (set_focus_finder)
    {
        ImGui::SetKeyboardFocusHere();
        set_focus_finder = false;
    }
    ImGui::InputText("Text filter##EntitySearchName", &search_entity_name, ImGuiInputTextFlags_AutoSelectAll);
    if (ImGui::IsItemFocused())
        focused_tool = "tool_finder";
    static uint32_t search_entity_type = 0;
    ImGui::InputScalar("##EntityType", ImGuiDataType_U32, &search_entity_type, &u32_one);
    ImGui::SameLine();
    if (search_entity_type > 0)
    {
        ImGui::Text("%s", entity_names[search_entity_type].c_str());
        if (ImGui::IsItemFocused())
            focused_tool = "tool_finder";
    }
    else
    {
        ImGui::Text("ENT_TYPE");
        if (ImGui::IsItemFocused())
            focused_tool = "tool_finder";
    }

    static int search_entity_layer = -128;
    if (ImGui::RadioButton("Both layers", search_entity_layer == -128))
        search_entity_layer = -128;
    ImGui::SameLine();
    if (ImGui::RadioButton("Front", search_entity_layer == 0))
        search_entity_layer = 0;
    ImGui::SameLine();
    if (ImGui::RadioButton("Back", search_entity_layer == 1))
        search_entity_layer = 1;
    ImGui::SameLine();
    if (ImGui::RadioButton("Current layer", search_entity_layer == -1))
        search_entity_layer = -1;

    static int search_entity_depth[2] = {0, 52};
    ImGui::SliderInt2("Draw depth##EntitySearchDepth", search_entity_depth, 0, 52, "%d", ImGuiSliderFlags_AlwaysClamp);

    static int search_entity_mask = 0;
    if (submenu("Mask##EntitySearchMask"))
    {
        for (int i = 0; i < 15; i++)
        {
            if (i % 2)
                ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.5f);
            ImGui::CheckboxFlags(mask_names[i], &search_entity_mask, (int)std::pow(2, i));
        }
        endmenu();
    }

    static unsigned int search_entity_flags = 0;
    static unsigned int search_entity_not_flags = 0;
    static unsigned int search_entity_more_flags = 0;
    static unsigned int search_entity_not_more_flags = 0;
    static unsigned int search_entity_properties_flags = 0;
    static unsigned int search_entity_not_properties_flags = 0;

    static const ImVec4 green = ImVec4(0.2f, 1.0f, 0.2f, 0.6f);
    static const ImVec4 red = ImVec4(1.0f, 0.2f, 0.2f, 0.6f);
    static const ImVec4 white = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_CheckMark, white);

    if (submenu("Flags##EntitySearchFlags"))
    {
        ImGui::PushID("EntitySearchFlags");
        for (int i = 0; i < 32; i++)
        {
            ImGui::PushID(i);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, red);
            ImGui::CheckboxFlags("", &search_entity_not_flags, (int)std::pow(2, i));
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_FrameBg, green);
            ImGui::CheckboxFlags(entity_flags[i], &search_entity_flags, (int)std::pow(2, i));
            ImGui::PopStyleColor();
            ImGui::PopID();
        }
        ImGui::PopID();
        endmenu();
    }
    if (submenu("More Flags##EntitySearchMoreFlags"))
    {
        ImGui::PushID("EntitySearchMoreFlags");
        for (int i = 0; i < 23; i++)
        {
            ImGui::PushID(i);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, red);
            ImGui::CheckboxFlags("", &search_entity_not_more_flags, (int)std::pow(2, i));
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_FrameBg, green);
            ImGui::CheckboxFlags(more_flags[i], &search_entity_more_flags, (int)std::pow(2, i));
            ImGui::PopStyleColor();
            ImGui::PopID();
        }
        ImGui::PopID();
        endmenu();
    }
    if (submenu("Properties Flags##EntitySearchPropertiesFlags"))
    {
        ImGui::PushID("EntitySearchPropertiesFlags");
        for (int i = 0; i < 27; i++)
        {
            ImGui::PushID(i);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, red);
            ImGui::CheckboxFlags("", &search_entity_not_properties_flags, (int)std::pow(2, i));
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_FrameBg, green);
            ImGui::CheckboxFlags(entity_type_properties_flags[i], &search_entity_properties_flags, (int)std::pow(2, i));
            ImGui::PopStyleColor();
            ImGui::PopID();
        }
        ImGui::PopID();
        endmenu();
    }
    ImGui::PopStyleColor();
    ImGui::Text("");

    static bool run_filter = false;
    static bool extra_filter = false;
    if (ImGui::Button("Search##SearchEntities") || run_finder)
    {
        g_selected_ids = UI::get_entities_by({search_entity_type}, (ENTITY_MASK)search_entity_mask, (LAYER)search_entity_layer);
        run_filter = true;
        run_finder = false;
    }
    ImGui::SameLine();
    if (ImGui::Button("Filter##FilterEntities"))
    {
        run_filter = true;
        extra_filter = true;
    }
    if (run_filter)
    {
        run_filter = false;
        if (extra_filter)
        {
            if (search_entity_type != 0)
            {
                g_selected_ids.erase(std::remove_if(g_selected_ids.begin(), g_selected_ids.end(), [&](uint32_t filter_uid)
                                                    {
                                                        auto ent = get_entity_ptr(filter_uid);
                                                        if (!ent)
                                                            return true;
                                                        return ent->type->id != search_entity_type; }),
                                     g_selected_ids.end());
            }
            if (search_entity_mask != 0)
            {
                g_selected_ids.erase(std::remove_if(g_selected_ids.begin(), g_selected_ids.end(), [&](uint32_t filter_uid)
                                                    {
                                                        auto ent = get_entity_ptr(filter_uid);
                                                        if (!ent)
                                                            return true;
                                                        return ((int)ent->type->search_flags & search_entity_mask) == 0; }),
                                     g_selected_ids.end());
            }
            {
                g_selected_ids.erase(std::remove_if(g_selected_ids.begin(), g_selected_ids.end(), [&](uint32_t filter_uid)
                                                    {
                                                        auto ent = get_entity_ptr(filter_uid);
                                                        if (!ent)
                                                            return true;
                                                        return ent->layer != enum_to_layer((LAYER)search_entity_layer); }),
                                     g_selected_ids.end());
            }
            extra_filter = false;
        }
        if (search_entity_flags != 0)
        {
            g_selected_ids.erase(std::remove_if(g_selected_ids.begin(), g_selected_ids.end(), [&](uint32_t filter_uid)
                                                {
                                                    auto ent = get_entity_ptr(filter_uid);
                                                    if (!ent)
                                                        return true;
                                                    return (ent->flags & search_entity_flags) != search_entity_flags; }),
                                 g_selected_ids.end());
        }
        if (search_entity_not_flags != 0)
        {
            g_selected_ids.erase(std::remove_if(g_selected_ids.begin(), g_selected_ids.end(), [&](uint32_t filter_uid)
                                                {
                                                    auto ent = get_entity_ptr(filter_uid);
                                                    if (!ent)
                                                        return true;
                                                    return (ent->flags & search_entity_not_flags) != 0; }),
                                 g_selected_ids.end());
        }
        if (search_entity_more_flags != 0)
        {
            g_selected_ids.erase(std::remove_if(g_selected_ids.begin(), g_selected_ids.end(), [&](uint32_t filter_uid)
                                                {
                                                    auto ent = get_entity_ptr(filter_uid);
                                                    if (!ent)
                                                        return true;
                                                    return (ent->more_flags & search_entity_more_flags) != search_entity_more_flags; }),
                                 g_selected_ids.end());
        }
        if (search_entity_not_more_flags != 0)
        {
            g_selected_ids.erase(std::remove_if(g_selected_ids.begin(), g_selected_ids.end(), [&](uint32_t filter_uid)
                                                {
                                                    auto ent = get_entity_ptr(filter_uid);
                                                    if (!ent)
                                                        return true;
                                                    return (ent->more_flags & search_entity_not_more_flags) != 0; }),
                                 g_selected_ids.end());
        }
        if (search_entity_properties_flags != 0)
        {
            g_selected_ids.erase(std::remove_if(g_selected_ids.begin(), g_selected_ids.end(), [&](uint32_t filter_uid)
                                                {
                                                    auto ent = get_entity_ptr(filter_uid);
                                                    if (!ent)
                                                        return true;
                                                    return (ent->type->properties_flags & search_entity_properties_flags) != search_entity_properties_flags; }),
                                 g_selected_ids.end());
        }
        if (search_entity_not_properties_flags != 0)
        {
            g_selected_ids.erase(std::remove_if(g_selected_ids.begin(), g_selected_ids.end(), [&](uint32_t filter_uid)
                                                {
                                                    auto ent = get_entity_ptr(filter_uid);
                                                    if (!ent)
                                                        return true;
                                                    return (ent->type->properties_flags & search_entity_not_properties_flags) != 0; }),
                                 g_selected_ids.end());
        }
        if (search_entity_name != "")
        {
            g_selected_ids.erase(std::remove_if(g_selected_ids.begin(), g_selected_ids.end(), [&](uint32_t filter_uid)
                                                {
                                                    auto ent = get_entity_ptr(filter_uid);
                                                    if (!ent)
                                                        return true;
                                                    return !StrStrIA(entity_names[ent->type->id].c_str(), search_entity_name.c_str()); }),
                                 g_selected_ids.end());
        }
        if (search_entity_depth[0] > 0 || search_entity_depth[1] < 52)
        {
            g_selected_ids.erase(std::remove_if(g_selected_ids.begin(), g_selected_ids.end(), [&](uint32_t filter_uid)
                                                {
                                                    auto ent = get_entity_ptr(filter_uid);
                                                    if (!ent)
                                                        return true;
                                                    return ent->draw_depth < search_entity_depth[0] || ent->draw_depth > search_entity_depth[1]; }),
                                 g_selected_ids.end());
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset##ResetSearchEntities"))
    {
        search_entity_name = "";
        search_entity_type = 0;
        search_entity_mask = 0;
        search_entity_depth[0] = 0;
        search_entity_depth[1] = 52;
        search_entity_layer = -128;
        search_entity_flags = 0;
        search_entity_not_flags = 0;
        search_entity_more_flags = 0;
        search_entity_not_more_flags = 0;
        search_entity_properties_flags = 0;
        search_entity_not_properties_flags = 0;
    }
    ImGui::SameLine();
    if (ImGui::Button("Clear##ClearSelectedEntities"))
        g_selected_ids.clear();
    if (g_selected_ids.size() > 0)
    {
        ImGui::Text("");
        ImGui::Text("%d entities selected:", (int)g_selected_ids.size());
        if (ImGui::Button("Smart delete##SmartDestroySelectedEntities"))
        {
            for (auto selected_uid : g_selected_ids)
            {
                auto ent = get_entity_ptr(selected_uid);
                if (ent)
                    smart_delete(ent, false);
            }
            g_selected_ids.clear();
        }
        ImGui::SameLine();
        if (ImGui::Button("Kill##KillSelectedEntities"))
        {
            for (auto selected_uid : g_selected_ids)
            {
                auto ent = get_entity_ptr(selected_uid);
                if (ent)
                    ent->kill(true, nullptr);
            }
            g_selected_ids.clear();
        }
        ImGui::SameLine();
        if (ImGui::Button("Destroy##DestroySelectedEntities"))
        {
            for (auto selected_uid : g_selected_ids)
            {
                auto ent = get_entity_ptr(selected_uid);
                if (ent)
                    ent->destroy();
            }
            g_selected_ids.clear();
        }
        for (auto selected_uid : g_selected_ids)
        {
            render_uid(selected_uid, "Multiselect");
        }
    }
    ImGui::PopItemWidth();
}

void render_entity_props(int uid, bool detached = false)
{
    auto entity = get_entity_ptr(uid);
    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.5f);
    if (!detached)
    {
        ImGui::Checkbox("Lock to player one", &lock_player);
        tooltip("Automatically switch to player after new level.");
        if (lock_player)
        {
            if (!g_players.empty())
            {
                g_last_id = g_players.at(0)->uid;
            }
        }
        if (set_focus_uid)
        {
            ImGui::SetKeyboardFocusHere();
            set_focus_uid = false;
        }
        ImGui::InputInt("UID", &g_last_id, 1, 100, edit_last_id ? ImGuiInputTextFlags_ReadOnly : 0);
        edit_last_id = false;
        if (ImGui::IsItemFocused())
            focused_tool = "tool_entity_properties";
        tooltip("Use mouse to easily select or move entities around.", "mouse_grab");
        ImGui::SameLine();
        ImGui::Checkbox("Sticky", &lock_entity);
        tooltip("Check to keep this entity selected when spawning others.");
        ImGui::SameLine();
        if (ImGui::Button("Detach##DetachEntity"))
        {
            auto name = fmt::format("Entity {}", g_last_id);
            auto window = new EntityWindow({name, g_last_id, true});
            entity_windows[uid] = window;
        }
        tooltip("Detach entity to separate window.");
        if (!update_entity())
            return;
    }
    ImGui::PopItemWidth();
    if (entity == nullptr)
    {
        auto it = entity_windows.find(uid);
        if (it != entity_windows.end())
            entity_windows[uid]->open = false;
        return;
    }
    const auto is_movable = entity->is_movable();
    ImGui::PushItemWidth(-ImGui::GetContentRegionMax().x * 0.5f);
    render_uid(entity->uid, "EntityGeneral");
    if (ImGui::Button("Smart delete##SafeKillEntity"))
    {
        smart_delete(entity, true);
    }
    tooltip("Try to get rid of the entity in some\nsmart way that hopefully doesn't crash.\nWorks on most boss heads and jellies.", "mouse_destroy");
    ImGui::SameLine();
    if (ImGui::Button("Void##VoidEntity"))
    {
        entity->detach(true);
        entity->overlay = nullptr;
        entity->y -= 1000.0;
    }
    tooltip("Move the entity under the level,\nlike it just fell in to the void.");
    ImGui::SameLine();
    if (ImGui::Button("Kill##KillEntity"))
    {
        entity->kill(true, nullptr);
    }
    tooltip("Kill the entity,\nlike it received damage and died.");
    ImGui::SameLine();
    if (ImGui::Button("Rem##RemoveEntity"))
    {
        entity->remove();
    }
    tooltip("Move the entity to limbo layer,\nlike it exists but doesn't do anything.");
    ImGui::SameLine();
    if (ImGui::Button("Destroy##DestroyEntity"))
    {
        entity->destroy();
    }
    tooltip("Destroy the entity quietly,\nlike just get rid of it, no boom, drops or decorating.");
    if (submenu("State"))
    {
        auto overlay = entity->overlay;
        if (overlay)
        {
            if ((overlay->type->search_flags & ENTITY_MASK::MOUNT) == ENTITY_MASK::MOUNT)
            {
                ImGui::Text("Riding:");
                ImGui::SameLine();
                if (ImGui::Button("Unmount##UnmountRiding"))
                {
                    overlay->as<Mount>()->remove_rider();
                }
                else
                    render_uid(overlay->uid, "StateRiding");
            }
            else
            {
                ImGui::Text("Attached to:");
                ImGui::SameLine();
                if (ImGui::Button("Detach"))
                {
                    if ((entity->type->search_flags & ENTITY_MASK::PLAYER) == ENTITY_MASK::PLAYER)
                        entity->as<Player>()->let_go();
                    else
                        entity->detach(true);
                }
                else
                    render_uid(overlay->uid, "StateAttached");
            }
        }
        else
        {
            ImGui::Text("Attached to:");
            ImGui::Text("Nothing");
        }
        if (is_movable)
        {
            auto movable = entity->as<Movable>();
            ImGui::PushItemWidth(ImGui::GetContentRegionMax().x * 0.5f - 24.0f);
            ImGui::SetNextItemWidth(24.0f);
            ImGui::InputScalar("##EntityState", ImGuiDataType_U8, &movable->state);
            ImGui::SameLine(0, 4);
            render_state("Current state", movable->state); // TODO: allow change
            ImGui::SetNextItemWidth(24.0f);
            ImGui::InputScalar("##EntityLastState", ImGuiDataType_U8, &movable->last_state);
            ImGui::SameLine(0, 4);
            render_state("Last state", movable->last_state);
            ImGui::SetNextItemWidth(24.0f);
            ImGui::InputScalar("##EntityMoveState", ImGuiDataType_U8, &movable->move_state);
            ImGui::SameLine(0, 4);
            render_ai("AI state", movable->move_state); // TODO: allow change
            ImGui::PopItemWidth();
            const std::string current_behavior_str = fmt::format("{}", movable->get_behavior());
            if (ImGui::BeginCombo("Current Behavior##ChangeBehaviorCombo", current_behavior_str.c_str()))
            {
                for (auto& item : movable->behaviors_map)
                {
                    const std::string option_str = fmt::format("{}", item.first);
                    bool isSelected = movable->current_behavior == item.second;
                    if (ImGui::Selectable(option_str.c_str(), isSelected))
                    {
                        movable->current_behavior = item.second;
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::Text("Standing on:");
            if (movable->standing_on_uid != -1)
            {

                render_uid(movable->standing_on_uid, "StateStanding");
            }
            else
            {
                ImGui::Text("Nothing");
            }
            if (movable->holding_uid != -1)
            {
                ImGui::Text("Holding:");
                ImGui::SameLine();
                if (ImGui::Button("Drop##DropHolding"))
                {
                    auto holding = get_entity_ptr(movable->holding_uid);
                    if (holding)
                        movable->drop();
                    else
                        movable->holding_uid = -1;
                }
                render_uid(movable->holding_uid, "StateHolding");
            }
            if (movable->last_owner_uid != -1 || movable->owner_uid != -1)
            {
                ImGui::Text("Owner:");
                ImGui::SameLine();
                if (ImGui::Button("Clear##RemoveOwner"))
                {
                    movable->owner_uid = -1;
                    movable->last_owner_uid = -1;
                }
                render_uid(movable->owner_uid, "StateOwner");
                render_uid(movable->last_owner_uid, "StateOwnerLast");
            }
        }
        endmenu();
    }
    if (submenu("Position"))
    {
        auto movable = entity->as<Movable>();
        if (ImGui::Button("Change"))
        {
            auto layer_to = LAYER::FRONT;
            if (entity->layer == 0)
                layer_to = LAYER::BACK;
            entity->topmost_mount()->set_layer(layer_to);
        }
        ImGui::SameLine();
        switch (entity->layer)
        {
        case 0:
            ImGui::Text("Layer: FRONT");
            break;
        case 1:
            ImGui::Text("Layer: BACK");
            break;
        default:
            ImGui::Text("Layer: UNKNOWN");
            break;
        }
        ImGui::InputFloat("Position X##EntityPositionX", &entity->x, 0.2f, 1.0f);
        ImGui::InputFloat("Position Y##EntityPositionY", &entity->y, 0.2f, 1.0f);
        if (is_movable)
        {
            ImGui::InputFloat("Absolute X##EntityAbsoluteX", &entity->abs_x, 0.2f, 1.0f);
            ImGui::InputFloat("Absolute Y##EntityAbsoluteY", &entity->abs_y, 0.2f, 1.0f);
            ImGui::InputFloat("Velocity X##EntityVelocityX", &movable->velocityx, 0.2f, 1.0f);
            ImGui::InputFloat("Velocity Y##EntityVelocityY", &movable->velocityy, 0.2f, 1.0f);
        }
        ImGui::InputFloat("Angle##EntityAngle", &entity->angle, 0.2f, 1.0f);
        if (is_movable)
        {
            SliderByte("Falling timer##EntityFallingTimer", (char*)&movable->falling_timer, 0, 98);
            uint8_t falldamage = 0;
            if (movable->falling_timer >= 98)
                falldamage = 4;
            else if (movable->falling_timer >= 78)
                falldamage = 3;
            else if (movable->falling_timer >= 58)
                falldamage = 2;
            else if (movable->falling_timer >= 38)
                falldamage = 1;
            const char* damagenum[] = {"0", "1", "2", "4", "99"};
            SliderByte("Fall damage##EntityFallDamage", (char*)&falldamage, 0, 4, damagenum[falldamage]);
        }
        endmenu();
    }
    if (is_movable && submenu("Stats"))
    {
        auto movable = entity->as<Player>();
        ImGui::DragScalar("Health##EntityHealth", ImGuiDataType_U8, (char*)&movable->health, 0.5f, &u8_one, &u8_max);
        ImGui::DragScalar("Price##Price", ImGuiDataType_S32, (char*)&movable->price, 0.5f, &s32_min, &s32_max);
        if ((entity->type->search_flags & ENTITY_MASK::PLAYER) == ENTITY_MASK::PLAYER && movable->inventory_ptr != 0)
        {
            ImGui::DragScalar("Bombs##EntityBombs", ImGuiDataType_U8, (char*)&movable->inventory_ptr->bombs, 0.5f, &u8_one, &u8_max);
            ImGui::DragScalar("Ropes##EntityRopes", ImGuiDataType_U8, (char*)&movable->inventory_ptr->ropes, 0.5f, &u8_one, &u8_max);
            ImGui::DragInt("Money##EntityMoney", (int*)&movable->inventory_ptr->money, 20.0f, INT_MIN, INT_MAX, "%d");
            ImGui::DragInt("Level kills##EntityLevelKills", (int*)&movable->inventory_ptr->kills_level, 0.5f, 0, INT_MAX, "%d");
            ImGui::DragInt("Total kills##EntityTotalKills", (int*)&movable->inventory_ptr->kills_total, 0.5f, 0, INT_MAX, "%d");
        }
        endmenu();
    }
    if (submenu("Items & Powerups"))
    {
        static bool fx = false;
        ImGui::Checkbox("Show annoying FX items", &fx);
        ImGui::SeparatorText("Items");
        if (!(entity->type->search_flags & (ENTITY_MASK::PLAYER | ENTITY_MASK::MOUNT | ENTITY_MASK::MONSTER)))
        {
            int removed_uid = -1;
            for (auto ent : entity->items.entities())
            {
                if (fx || !(ent->type->search_flags & ENTITY_MASK::FX))
                    if (render_uid(ent->uid, "EntityItems", true))
                        removed_uid = ent->uid;
            }
            if (auto removed = get_entity_ptr(removed_uid))
                entity->remove_item(removed, true);
        }
        else
        {
            auto entity_pow = entity->as<PowerupCapable>();
            int removed_uid = -1;
            for (auto ent : entity->items.entities())
            {
                if ((fx || !(ent->type->search_flags & ENTITY_MASK::FX)) && !entity_pow->has_powerup(ent->type->id))
                    if (render_uid(ent->uid, "EntityItems", true))
                        removed_uid = ent->uid;
            }
            if (auto removed = get_entity_ptr(removed_uid))
                entity_pow->remove_item(removed, true);
            ImGui::SeparatorText("Powerups");
            int removed_powerup = 0;
            for (const auto& [powerup_id, powerup_entity] : entity_pow->powerups)
            {
                if (render_powerup(powerup_entity->uid, "Powerups"))
                    removed_powerup = powerup_entity->type->id;
            }
            if (removed_powerup)
                entity_pow->remove_powerup(removed_powerup);
            ImGui::PushItemWidth(160);
            static const char* chosenPowerup = "";
            static uint8_t chosenPowerupIndex = 0;
            static const char* powerupOptions[] = {
                "Alien compass",
                "Ankh",
                "Climbing gloves",
                "Compass",
                "Crown",
                "Eggplant crown",
                "Hedjet",
                "Kapala",
                "Parachute",
                "Paste",
                "Pitcher's mitt",
                "Skeleton key",
                "Spectacles",
                "Spike shoes",
                "Spring shoes",
                "Tablet of Destiny",
                "True crown",
                "Udjat eye"};
            static const uint32_t powerupTypeIDOptions[] = {
                to_id("ENT_TYPE_ITEM_POWERUP_SPECIALCOMPASS"),
                to_id("ENT_TYPE_ITEM_POWERUP_ANKH"),
                to_id("ENT_TYPE_ITEM_POWERUP_CLIMBING_GLOVES"),
                to_id("ENT_TYPE_ITEM_POWERUP_COMPASS"),
                to_id("ENT_TYPE_ITEM_POWERUP_CROWN"),
                to_id("ENT_TYPE_ITEM_POWERUP_EGGPLANTCROWN"),
                to_id("ENT_TYPE_ITEM_POWERUP_HEDJET"),
                to_id("ENT_TYPE_ITEM_POWERUP_KAPALA"),
                to_id("ENT_TYPE_ITEM_POWERUP_PARACHUTE"),
                to_id("ENT_TYPE_ITEM_POWERUP_PASTE"),
                to_id("ENT_TYPE_ITEM_POWERUP_PITCHERSMITT"),
                to_id("ENT_TYPE_ITEM_POWERUP_SKELETON_KEY"),
                to_id("ENT_TYPE_ITEM_POWERUP_SPECTACLES"),
                to_id("ENT_TYPE_ITEM_POWERUP_SPIKE_SHOES"),
                to_id("ENT_TYPE_ITEM_POWERUP_SPRING_SHOES"),
                to_id("ENT_TYPE_ITEM_POWERUP_TABLETOFDESTINY"),
                to_id("ENT_TYPE_ITEM_POWERUP_TRUECROWN"),
                to_id("ENT_TYPE_ITEM_POWERUP_UDJATEYE")};
            if (ImGui::BeginCombo("##AddPowerupCombo", chosenPowerup))
            {
                for (uint8_t i = 0; i < IM_ARRAYSIZE(powerupOptions); ++i)
                {
                    bool isSelected = (chosenPowerup == powerupOptions[i]);
                    if (ImGui::Selectable(powerupOptions[i], isSelected))
                    {
                        chosenPowerup = powerupOptions[i];
                        chosenPowerupIndex = i;
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::SameLine();
            if (ImGui::Button("Add Powerup##AddPowerupButton"))
            {
                entity_pow->give_powerup(powerupTypeIDOptions[chosenPowerupIndex]);
            }
            ImGui::PopItemWidth();
        }
        endmenu();
    }
    if (submenu("Global attributes") && entity->type)
    {
        ImGui::DragScalar("Damage##GlobalDamage", ImGuiDataType_U8, (char*)&entity->type->damage, 0.5f, &u8_one, &u8_max);
        ImGui::DragScalar("Health##GlobalLife", ImGuiDataType_U8, (char*)&entity->type->life, 0.5f, &u8_one, &u8_max);
        ImGui::DragFloat("Friction##GlobalFriction", &entity->type->friction, 0.01f, 0.0f, 10.0f, "%.5f");
        ImGui::DragFloat("Elasticity##GlobalElasticity", &entity->type->elasticity, 0.01f, 0.0f, 10.0f, "%.5f");
        ImGui::DragFloat("Weight##GlobalWeight", &entity->type->weight, 0.01f, 0.0f, 10.0f, "%.5f");
        ImGui::DragFloat("Acceleration##GlobalAcceleration", &entity->type->acceleration, 0.01f, 0.0f, 10.0f, "%.5f");
        ImGui::DragFloat("Max speed##GlobalMaxSpeed", &entity->type->max_speed, 0.01f, 0.0f, 10.0f, "%.5f");
        ImGui::DragFloat("Sprint factor##GlobalSprintFactor", &entity->type->sprint_factor, 0.01f, 0.0f, 10.0f, "%.5f");
        ImGui::DragFloat("Jump power##GlobalJumpPower", &entity->type->jump, 0.01f, 0.0f, 10.0f, "%.5f");
        ImGui::InputScalar("Mask:##SearchFlags", ImGuiDataType_U32, &entity->type->search_flags, 0, 0, "%08X", ImGuiInputTextFlags_ReadOnly);
        ImGui::SameLine();
        ImGui::TextUnformatted(mask_names[std::countr_zero((uint32_t)entity->type->search_flags)]);
        if (submenu("Properties flags"))
        {
            render_flags(entity_type_properties_flags, &entity->type->properties_flags);
            endmenu();
        }
        endmenu();
    }
    if (submenu("Special attributes"))
    {
        const auto entity_type = entity->type->id;
        if (entity_type == to_id("ENT_TYPE_ITEM_COFFIN"))
        {
            auto coffin = entity->as<Coffin>();
            static const auto eggplant_child = to_id("ENT_TYPE_CHAR_EGGPLANT_CHILD");
            static const auto ana_spelunky = to_id("ENT_TYPE_CHAR_ANA_SPELUNKY");
            ImGui::Text("Character in coffin:");
            ImGui::SliderInt("##CoffinSpawns", (int*)&coffin->inside, ana_spelunky, eggplant_child);
            if (coffin->inside == to_id("ENT_TYPE_CHAR_CLASSIC_GUY") + 1)
                coffin->inside = to_id("ENT_TYPE_CHAR_HIREDHAND");
            ImGui::SameLine();
            ImGui::Text("%s", entity_names[coffin->inside].c_str());
            ImGui::InputScalar("Timer##CoffinTimer", ImGuiDataType_U8, &coffin->timer, 0, 0, "%lld", ImGuiInputTextFlags_ReadOnly);
        }
        else if (
            entity_type == to_id("ENT_TYPE_ITEM_CRATE") || entity_type == to_id("ENT_TYPE_ITEM_PRESENT") ||
            entity_type == to_id("ENT_TYPE_ITEM_GHIST_PRESENT") || entity_type == to_id("ENT_TYPE_ITEM_POT") ||
            entity_type == to_id("ENT_TYPE_ITEM_DMCRATE") || entity_type == to_id("ENT_TYPE_ITEM_ALIVE_EMBEDDED_ON_ICE"))
        {
            auto container = entity->as<Container>();
            ImGui::Text("Item in container:");
            ImGui::InputInt("##EntitySpawns", (int*)&container->inside, 1, 10);
            if (container->inside > 0)
            {
                ImGui::SameLine();
                ImGui::Text("%s", entity_names[container->inside].c_str());
            }
        }
        else if (entity_type == to_id("ENT_TYPE_ITEM_MATTOCK"))
        {
            auto mattock = entity->as<Mattock>();
            ImGui::SliderScalar("Uses left##MattockUses", ImGuiDataType_U8, &mattock->remaining, &u8_min, &u8_max);
        }
        else if (
            entity_type == to_id("ENT_TYPE_FLOOR_DOOR_EXIT") || entity_type == to_id("ENT_TYPE_FLOOR_DOOR_STARTING_EXIT") ||
            entity_type == to_id("ENT_TYPE_FLOOR_DOOR_COG") || entity_type == to_id("ENT_TYPE_FLOOR_DOOR_EGGPLANT_WORLD"))
        {
            auto target = entity->as<ExitDoor>();
            ImGui::Text("Door target:");
            ImGui::Checkbox("Enabled##DoorEnabled", &target->special_door);
            ImGui::DragScalar("World##DoorWorldnumber", ImGuiDataType_U8, &target->world, 0.5f, &u8_one, &u8_max);
            ImGui::DragScalar("Level##DoorLevelnumber", ImGuiDataType_U8, &target->level, 0.5f, &u8_one, &u8_max);
            ImGui::DragScalar("Theme##DoorThemenumber", ImGuiDataType_U8, &target->theme, 0.2f, &u8_one, &u8_seventeen);
            ImGui::SameLine();
            ImGui::Text("%s", theme_name(target->theme));
        }
        else if (entity_type == to_id("ENT_TYPE_LOGICAL_PORTAL"))
        {
            auto target = entity->as<Portal>();
            ImGui::Text("Portal target:");
            ImGui::DragScalar("World##DoorWorldnumber", ImGuiDataType_U8, &target->world, 0.5f, &u8_one, &u8_max);
            ImGui::DragScalar("Level##DoorLevelnumber", ImGuiDataType_U8, &target->level, 0.5f, &u8_one, &u8_max);
            ImGui::DragScalar("Theme##DoorThemenumber", ImGuiDataType_U8, &target->theme, 0.2f, &u8_one, &u8_seventeen);
            ImGui::SameLine();
            ImGui::Text("%s", theme_name(target->theme));
        }
        else if (entity_type == to_id("ENT_TYPE_LOGICAL_DOOR"))
        {
            auto door = entity->as<LogicalDoor>();
            ImGui::Text("Door type:");
            ImGui::InputInt("##LogicalDoorDoor", (int*)&door->door_type, 1, 10);
            ImGui::SameLine();
            ImGui::Text("%s", entity_names[door->door_type].c_str());
            ImGui::Text("Platform type:");
            ImGui::InputInt("##LogicalDoorPlatform", (int*)&door->platform_type, 1, 10);
            ImGui::SameLine();
            ImGui::Text("%s", entity_names[door->platform_type].c_str());
            ImGui::Checkbox("Door spawned##LogicalDoorSpawned", &door->not_hidden);
            ImGui::Checkbox("Platform spawned##LogicalDoorPlatformSpawned", &door->platform_spawned);
        }
        else if (!!(entity->type->search_flags & (ENTITY_MASK::PLAYER | ENTITY_MASK::MOUNT | ENTITY_MASK::MONSTER)))
        {
            auto entity_player = entity->as<Player>();
            if ((entity->type->search_flags & ENTITY_MASK::PLAYER) == ENTITY_MASK::PLAYER && entity_player->ai != 0)
            {
                ImGui::InputScalar("AI state##AiState", ImGuiDataType_S8, &entity_player->ai->state, &u8_min, &s8_max);
                ImGui::InputScalar("Trust##AiTrust", ImGuiDataType_S8, &entity_player->ai->trust, &u8_min, &s8_max);
                ImGui::InputScalar("Whipped##AiWhipped", ImGuiDataType_S8, &entity_player->ai->whipped, &u8_min, &s8_max);
                if (entity_player->ai->target_uid != -1)
                {
                    ImGui::Text("Target:");
                    render_uid(entity_player->ai->target_uid, "Ai");
                }
            }
        }
        endmenu();
    }
    if (submenu("Color, Size, Texture"))
    {
        auto textureid = entity->get_texture();
        std::string texture = g_Console.get()->execute(fmt::format("return enum_get_name(TEXTURE, get_entity({}):get_texture()) or 'UNKNOWN'", uid), true);
        // std::string texturepath = g_Console.get()->execute(fmt::format("return get_texture_definition(get_entity({}):get_texture()).texture_path", uid), true);
        texture = "TEXTURE." + texture.substr(1, texture.length() - 2);
        // texturepath = texturepath.substr(1, texturepath.length() - 2);
        ImGui::ColorEdit4("Color", (float*)&entity->color);
        ImGui::DragFloat("Width##EntityWidth", &entity->w, 0.5f, 0.0, 10.0, "%.3f");
        ImGui::DragFloat("Height##EntityHeight", &entity->h, 0.5f, 0.0, 10.0, "%.3f");
        ImGui::DragFloat("Box width##EntityBoxWidth", &entity->hitboxx, 0.5f, 0.0, 10.0, "%.3f");
        ImGui::DragFloat("Box height##EntityBoxHeight", &entity->hitboxy, 0.5f, 0.0, 10.0, "%.3f");
        ImGui::Text("Hitbox shape:");
        ImGui::SameLine();
        int shape = static_cast<int>(entity->shape);
        if (ImGui::CheckboxFlags("Rectangle##EntityBoxRect", &shape, 1) && (shape & 1) != 0)
            shape = 1;
        ImGui::SameLine();
        if (ImGui::CheckboxFlags("Circle##EntityBoxCircle", &shape, 2) && (shape & 2) != 0)
            shape = 2;
        entity->shape = static_cast<SHAPE>(shape);
        ImGui::DragFloat("Offset X##EntityOffsetX", &entity->offsetx, 0.5f, -10.0, 10.0, "%.3f");
        ImGui::DragFloat("Offset Y##EntityOffsetY", &entity->offsety, 0.5f, -10.0, 10.0, "%.3f");
        uint8_t draw_depth = entity->draw_depth;
        if (ImGui::DragScalar("Draw depth##EntityDrawDepth", ImGuiDataType_U8, &draw_depth, 0.2f, &u8_zero, &u8_draw_depth_max))
            entity->set_draw_depth(draw_depth, 0);
        if (entity->rendering_info)
            ImGui::DragScalar("Shader##EntityShader", ImGuiDataType_U8, &entity->rendering_info->shader, 0.2f, &u8_zero, &u8_shader_max);
        ImGui::DragScalar("Animation frame##EntityAnimationFrame", ImGuiDataType_U16, &entity->animation_frame, 0.2f, &u16_zero, &u16_max);
        ImGui::InputText(fmt::format("Texture: {}##EntityTexture", textureid).c_str(), &texture, ImGuiInputTextFlags_ReadOnly);
        // ImGui::InputText("Texture path##EntityTexturePath", &texturepath, ImGuiInputTextFlags_ReadOnly);
        endmenu();
    }
    if (submenu("Flags"))
    {
        render_flags(entity_flags, &entity->flags);
        endmenu();
    }
    if (submenu("More Flags"))
    {
        render_flags(more_flags, &entity->more_flags);
        endmenu();
    }
    if (is_movable && submenu("Input Display"))
    {
        auto const movable_entity = entity->as<Movable>();
        ImVec2 region = ImGui::GetContentRegionMax();
        bool dirs[4] = {false, false, false, false};
        if (movable_entity->movex < 0.0f)
            dirs[0] = true;
        if (movable_entity->movey < 0.0f)
            dirs[1] = true;
        if (movable_entity->movey > 0.0f)
            dirs[2] = true;
        if (movable_entity->movex > 0.0f)
            dirs[3] = true;
        for (int i = 0; i < 4; i++)
        {
            ImGui::Checkbox(direction_flags[i], &dirs[i]);
            if (i < 3)
                ImGui::SameLine(region.x / 4 * (i + 1));
        }
        for (int i = 0; i < 6; i++)
        {
            int buttons = movable_entity->buttons;
            ImGui::CheckboxFlags(button_flags[i], &buttons, (int)std::pow(2, i));
            if (i < 5)
                ImGui::SameLine(region.x / 6 * (i + 1));
        }
        endmenu();
    }
    if ((entity->type->search_flags & ENTITY_MASK::PLAYER) == ENTITY_MASK::PLAYER && submenu("Illumination"))
    {
        auto entity_player = entity->as<Player>();
        if (entity_player->emitted_light)
            render_illumination(entity_player->emitted_light, "Entity illumination");
        endmenu();
    }
    ImGui::PopItemWidth();
}

void render_hotbar()
{
    if (g_Console->is_toggled() || hotbar.empty())
        return;

    ImGuiIO& io = ImGui::GetIO();
    auto base = ImGui::GetMainViewport();
    auto main_dock = ImGui::DockSpaceOverViewport(base, ImGuiDockNodeFlags_PassthruCentralNode);
    auto space = ImGui::DockBuilderGetCentralNode(main_dock);

    uint32_t max = 0;
    for (auto [i, id] : hotbar)
    {
        max = i;
    }
    const float iconsize = 64.0f;
    const float margin = base->Size.y / 50.0f;
    const float width = (max + 1) * (iconsize + 4.0f) + 4.0f;
    const float height = iconsize + margin;

    ImGui::SetNextWindowSize({width, height});
    ImGui::SetNextWindowPos({space->Pos.x + space->Size.x / 2 - width / 2, space->Pos.y + space->Size.y - height});
    ImGui::SetNextWindowViewport(base->ID);
    ImGui::Begin(
        "Hotbar",
        NULL,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking);
    if (ImGui::IsWindowHovered())
    {
        io.WantCaptureMouse = true;
        ImGui::SetWindowFocus();
    }
    ImGui::PushID("Hotbar");
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 4.0f);
    for (uint32_t i = 0; i <= max; ++i)
    {
        if (hotbar.contains(i))
        {
            ImGui::PushStyleColor(ImGuiCol_Button, {0, 0, 0, 0});
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, {0, 0, 0, 0});
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {1.0f, 1.0f, 1.0f, 0.3f});
            ImGui::PushStyleColor(ImGuiCol_Text, {1.0f, 1.0f, 1.0f, 1.0f});
            if (g_items[g_filtered_items[g_current_item]].id == hotbar[i])
                ImGui::PushStyleColor(ImGuiCol_Border, {1.0f, 1.0f, 1.0f, 0.8f});
            else
                ImGui::PushStyleColor(ImGuiCol_Border, {0, 0, 0, 0.8f});
            ImGui::PushID(i);
            std::string name = entity_names[hotbar[i]];
            // std::replace(name.begin(), name.end(), '_', '\n');
            name = name.substr(name.find_last_of('_') + 1);
            if (ImGui::Button(fmt::format("{}          \n\n\n{}", i + 1, name).c_str(), {iconsize, iconsize}))
            {
                set_selected_type(hotbar[i]);
            }
            else if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
            {
                hotbar.erase(i);
                save_config(cfgfile);
            }
            tooltip("(Mouse Left) to set spawned entity, (Mouse Right) to remove");
            ImGui::PopID();
            ImGui::PopStyleColor(5);
        }
        if (i != max)
            ImGui::SameLine((i + 1) * (iconsize + 4.0f), 4.0f);
    }
    ImGui::PopStyleVar();
    ImGui::PopID();
    ImGui::End();
}

void render_hotbar_textures()
{
    if (g_Console->is_toggled() || hotbar.empty())
        return;

    static const auto chest = to_id("ENT_TYPE_ITEM_CHEST");

    auto base = ImGui::GetMainViewport();
    auto main_dock = ImGui::DockSpaceOverViewport(base, ImGuiDockNodeFlags_PassthruCentralNode);
    auto space = ImGui::DockBuilderGetCentralNode(main_dock);

    uint32_t max = 0;
    for (auto [i, id] : hotbar)
    {
        max = i;
    }
    const float iconsize = 64.0f;
    const float margin = base->Size.y / 50.0f;
    const float width = (max + 1) * (iconsize + 4.0f) + 4.0f;
    const float height = iconsize + margin;

    ImVec2 size = {width, height};
    ImVec2 pos = {space->Pos.x + space->Size.x / 2 - width / 2, space->Pos.y + space->Size.y - height};

    auto* textures = get_textures();

    for (uint32_t i = 0; i <= max; ++i)
    {
        if (hotbar.contains(i))
        {
            auto type = get_type(hotbar[i]);
            if (!type)
                continue;
            auto texture = get_texture(type->texture_id);
            if (type->texture_id < 0)
            {
                const auto theme = g_state->current_theme ? g_state->current_theme : g_state->level_gen->themes[0];
                texture = get_texture(theme->get_dynamic_texture((DYNAMIC_TEXTURE)type->texture_id));
            }
            if (!texture)
                continue;
            auto def = get_texture_definition(texture->id);
            int32_t tx = type->tile_x, ty = type->tile_y;
            if ((uint32_t)tx > def.sub_image_width / def.tile_width)
            {
                tx = type->tile_x % (def.sub_image_width / def.tile_width);
                ty = (uint32_t)floor(type->tile_x / (def.sub_image_width / def.tile_width));
            }
            if (!type->animations.empty())
            {
                auto anim = type->animations.begin()->second;
                if (type->animations.contains(0))
                    anim = type->animations[0];
                tx = anim.first_tile % (def.sub_image_width / def.tile_width);
                ty = (uint32_t)floor(anim.first_tile / (def.sub_image_height / def.tile_height));
            }
            float uv_left = (texture->tile_width_fraction * tx) + texture->offset_x_weird_math;
            float uv_right = uv_left + texture->tile_width_fraction - texture->one_over_width;
            float uv_top = (texture->tile_height_fraction * ty) + texture->offset_y_weird_math;
            float uv_bottom = uv_top + texture->tile_height_fraction - texture->one_over_height;

            const Quad source(
                // bottom left:
                uv_left,
                uv_bottom,
                // bottom right:
                uv_right,
                uv_bottom,
                // top right:
                uv_right,
                uv_top,
                // top left:
                uv_left,
                uv_top);

            ImVec2 sa{pos.x + i * (iconsize + 4.0f) + 8.0f - base->Pos.x, pos.y - base->Pos.y + 8.0f};
            ImVec2 sb{pos.x + i * (iconsize + 4.0f) + iconsize - base->Pos.x, pos.y + iconsize - base->Pos.y};
            auto a = normalize(sa);
            auto b = normalize(sb);
            auto dest = Quad(AABB(a.x, a.y, b.x, b.y));
            RenderAPI::get().draw_screen_texture(textures->texture_map[0], source, dest, Color::black(), 0x29);
            if (!(hotbar[i] != chest && hotbar[i] != chest + 1 && texture->id == 373 && tx == 0 && ty == 0))
                RenderAPI::get().draw_screen_texture(texture, source, dest, Color::white(), 0x29);
        }
    }
}

struct TextureViewer
{
    TEXTURE id;
    int32_t uid;
    Quad src;
    Quad dest;
};

static TextureViewer texture_viewer{0, -1};
void render_vanilla_stuff()
{
    if (!hide_ui && options["draw_hotbar"])
        render_hotbar_textures();

    auto& render = RenderAPI::get();
    auto* textures = get_textures();
    if (texture_viewer.id < 0 || texture_viewer.id > 0x192 || !visible("tool_texture") || (options["menu_ui"] && !detached("tool_texture")) || hide_ui)
        return;
    // black bg hack
    render.draw_screen_texture(textures->texture_map[0], texture_viewer.src, texture_viewer.dest, Color::black(), 0x29);
    // draw vanilla texture right under the gui window
    render.draw_screen_texture(textures->texture_map[texture_viewer.id], texture_viewer.src, texture_viewer.dest, Color::white(), 0x29);
}

void render_texture_viewer()
{
    static std::map<TEXTURE, std::string> items;
    if (items.empty())
    {
        for (TEXTURE i = 0; i < 0x192; ++i)
        {
            auto def = get_texture_definition(i);
            auto slash = def.texture_path.find_last_of("/\\") + 1;
            std::string path = def.texture_path.substr(slash, def.texture_path.length() - slash);
            if (i > 0)
                path = path.substr(0, path.length() - 4);
            if (def.width > 0)
                items[i] = fmt::format("{:03d}: {}", i, path);
        }
    }
    const char* preview = items.contains(texture_viewer.id) ? items[texture_viewer.id].c_str() : "No texture selected";
    if (ImGui::BeginCombo("Texture##PickTexture", preview))
    {
        for (TEXTURE i = 0; i < 0x192; ++i)
        {
            const bool selected = (texture_viewer.id == i);
            if (items.contains(i) && ImGui::Selectable(items[i].c_str(), selected))
                texture_viewer.id = i;

            if (selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    if (texture_viewer.uid != g_last_id)
    {
        Entity* ent = get_entity_ptr(g_last_id);
        if (ent)
            texture_viewer.id = ent->get_texture();
        texture_viewer.uid = g_last_id;
    }
    Entity* ent = get_entity_ptr(texture_viewer.uid);

    auto pos = ImGui::GetWindowPos();
    auto size = ImGui::GetWindowSize();
    auto base = ImGui::GetMainViewport();
    auto dl = ImGui::GetBackgroundDrawList();
    if (texture_viewer.id < 0 || texture_viewer.id > 0x192 || !visible("tool_texture"))
    {
        dl->AddRectFilled({pos.x, pos.y}, {pos.x + size.x, pos.y + size.y}, ImGui::GetColorU32(ImGuiCol_WindowBg));
        return;
    }
    auto def = get_texture_definition(texture_viewer.id);
    std::string name = g_Console.get()->execute(fmt::format("return enum_get_name(TEXTURE, {}) or 'UNKNOWN'", texture_viewer.id), true);
    name = "TEXTURE." + name.substr(1, name.length() - 2);
    ImGui::InputText("ID", &name, ImGuiInputTextFlags_ReadOnly);
    ImGui::LabelText("Path", "%s", def.texture_path.c_str());
    ImGui::LabelText("Size", "%dx%d", def.width, def.height);
    ImGui::LabelText("Tile Size", "%dx%d", def.tile_width, def.tile_height);
    ImGui::LabelText("Offset", "%d,%d", def.sub_image_offset_x, def.sub_image_offset_y);
    ImGui::LabelText("Sub Image Size", "%dx%d", def.sub_image_width, def.sub_image_height);
    static bool draw_frames = true;
    static bool draw_grid = true;
    static bool draw_animations = true;
    ImGui::Checkbox("Draw tiles", &draw_grid);
    ImGui::SameLine();
    ImGui::Checkbox("Draw frames", &draw_frames);
    ImGui::SameLine();
    ImGui::Checkbox("Draw entity animations", &draw_animations);
    // end of real window contents
    texture_viewer.src = Quad(AABB(0, 0, 1, 1));
    ImGui::SetWindowSize({ImGui::GetWindowSize().x, ImGui::GetStyle().FramePadding.y * 2 + ImGui::GetStyle().WindowPadding.y * 2 + ImGui::GetTextLineHeight() + ImGui::GetCurrentWindow()->ContentSize.y + ImGui::GetWindowSize().x / def.width * def.height});
    ImVec2 sa{pos.x - base->Pos.x, pos.y + ImGui::GetStyle().FramePadding.y * 2 + ImGui::GetStyle().WindowPadding.y * 2 + ImGui::GetTextLineHeight() + ImGui::GetCurrentWindow()->ContentSize.y - base->Pos.y};
    ImVec2 sb{pos.x + size.x - base->Pos.x, pos.y + ImGui::GetStyle().FramePadding.y * 2 + ImGui::GetStyle().WindowPadding.y * 2 + ImGui::GetTextLineHeight() + ImGui::GetCurrentWindow()->ContentSize.y - base->Pos.y + (float)def.height / (float)def.width * size.x};
    ImVec2 ga{pos.x, pos.y + ImGui::GetStyle().FramePadding.y * 2 + ImGui::GetStyle().WindowPadding.y * 2 + ImGui::GetTextLineHeight() + ImGui::GetCurrentWindow()->ContentSize.y};
    ImVec2 gb{pos.x + size.x, pos.y + ImGui::GetStyle().FramePadding.y * 2 + ImGui::GetStyle().WindowPadding.y * 2 + ImGui::GetTextLineHeight() + ImGui::GetCurrentWindow()->ContentSize.y + (float)def.height / (float)def.width * size.x};
    auto a = normalize(sa);
    auto b = normalize(sb);
    texture_viewer.dest = Quad(AABB(a.x, a.y, b.x, b.y));
    pos = ImGui::GetWindowPos();
    size = ImGui::GetWindowSize();
    base = ImGui::GetMainViewport();
    auto f = (sb.x - sa.x) / def.width;
    if (draw_animations && ent && ent->get_texture() == texture_viewer.id)
    {
        std::map<std::tuple<uint32_t, uint32_t, bool>, int> overlap;
        for (const auto& [id, anim] : ent->type->animations)
        {
            uint32_t x = def.sub_image_offset_x + def.tile_width * (anim.first_tile % (def.sub_image_width / def.tile_width));
            uint32_t y = def.sub_image_offset_y + def.tile_height * (uint32_t)floor(anim.first_tile / (def.sub_image_height / def.tile_height));
            bool rev = anim.count < 0;
            auto key = std::make_tuple(x, y, rev);
            float tx = 0;
            if (rev)
            {
                tx = -20.0f * overlap[key] - 20.0f;
                x += def.tile_width;
            }
            else
            {
                tx = 20.0f * overlap[key] + 6.0f;
            }
            auto ax = ga.x + f * x;
            auto ay = ga.y + f * y;
            auto bx = ga.x + f * x + f * def.tile_width * anim.count;
            auto by = ga.y + f * y + f * def.tile_height;
            int border = 0xbbbb66ff;
            float thick = 2.0f;
            if (rev)
                dl->AddRect({ax - 3, ay + 3}, {bx + 3, by - 3}, border, 0, 0, thick);
            else
                dl->AddRect({ax + 3, ay + 3}, {bx - 3, by - 3}, border, 0, 0, thick);
            dl->AddText({ax + tx, ay + f * def.tile_height - ImGui::GetTextLineHeight() - 4}, 0xffffffff, fmt::format("{}", anim.id).c_str());
            overlap[key]++;
        }
    }

    int i = 0;
    for (uint32_t y = def.sub_image_offset_y; y < std::min(def.height, def.sub_image_height + def.sub_image_offset_y); y += def.tile_height)
    {
        for (uint32_t x = def.sub_image_offset_x; x < std::min(def.width, def.sub_image_width + def.sub_image_offset_x); x += def.tile_width)
        {
            int border = 0x55ffffff;
            float thick = 1.0f;
            bool current = false;
            if (ent && ent->animation_frame == i && ent->get_texture() == texture_viewer.id)
            {
                border = 0xff00ff00;
                thick = 3.0f;
                current = true;
            }
            auto ax = ga.x + f * x;
            auto ay = ga.y + f * y;
            auto bx = ga.x + f * x + f * def.tile_width;
            auto by = ga.y + f * y + f * def.tile_height;
            if (draw_grid || current)
                dl->AddRect({ax, ay}, {bx, by}, border, 0, 0, thick);
            if (draw_frames)
                dl->AddText({ax + 3, ay + 1}, 0xffffffff, fmt::format("{}", i).c_str());
            ++i;
        }
    }
    dl->AddRectFilled({pos.x, pos.y}, {pos.x + size.x, pos.y + ImGui::GetStyle().FramePadding.y * 2 + ImGui::GetStyle().WindowPadding.y * 2 + ImGui::GetTextLineHeight() + ImGui::GetCurrentWindow()->ContentSize.y}, ImGui::GetColorU32(ImGuiCol_WindowBg));
}

void force_time()
{
    if (g_state == 0)
        return;
    if (freeze_last && g_last_time == -1)
    {
        g_last_time = g_state->time_last_level;
    }
    else if (!freeze_last)
    {
        g_last_time = -1;
    }
    else if (g_last_time >= 0)
    {
        g_state->time_last_level = g_last_time;
    }
    if (freeze_level && g_level_time == -1)
    {
        g_level_time = g_state->time_level;
    }
    else if (!freeze_level)
    {
        g_level_time = -1;
    }
    else if (g_level_time >= 0)
    {
        g_state->time_level = g_level_time;
    }
    if (freeze_total && g_total_time == -1)
    {
        g_total_time = g_state->time_total;
    }
    else if (!freeze_total)
    {
        g_total_time = -1;
    }
    else if (g_total_time >= 0)
    {
        g_state->time_total = g_total_time;
    }

    if (quest_yang_state.has_value())
        g_state->quests->yang_state = quest_yang_state.value();
    if (quest_sisters_state.has_value())
        g_state->quests->jungle_sisters_flags = quest_sisters_state.value();
    if (quest_horsing_state.has_value())
        g_state->quests->van_horsing_state = quest_horsing_state.value();
    if (quest_sparrow_state.has_value())
        g_state->quests->sparrow_state = quest_sparrow_state.value();
    if (quest_tusk_state.has_value())
        g_state->quests->madame_tusk_state = quest_tusk_state.value();
    if (quest_beg_state.has_value())
        g_state->quests->beg_state = quest_beg_state.value();

    for (auto it = callbacks.begin(); it != callbacks.end();)
    {
        if (g_state->time_total >= it->frame)
        {
            it->func();
            it = callbacks.erase(it);
        }
        else
        {
            ++it;
        }
    }

    if (g_state->loading > 0)
    {
        g_selected_ids.clear();
        g_last_id = -1;
        edit_last_id = true;
        g_entity = nullptr;
    }
}

void render_timer()
{
    ImGui::PushFont(bigfont);
    ImGui::Text("%s", format_time(g_state->time_total).c_str());
    ImGui::PopFont();
}

void render_players()
{
    update_players();
    for (auto player : g_players)
    {
        ImGui::Text("%d:", player->input_ptr->player_slot + 1);
        ImGui::SameLine();
        render_uid(player->uid, "players");
    }
}

void render_game_props()
{
    if (g_state == 0)
        return;
    ImGui::PushItemWidth(-ImGui::GetWindowWidth() * 0.5f);
    if (submenu("Seed"))
    {
        static std::random_device rd;
        if (ImGui::MenuItem("Restart this run with same seed##RestartWithSeed", key_string(keys["quick_restart_seed"]).c_str()))
        {
            restart_adventure();
        }
        if (ImGui::MenuItem("New random adventure run", key_string(keys["quick_restart"]).c_str()))
        {
            g_state->quest_flags = 1;
            UI::init_adventure();
            static std::uniform_int_distribution<uint64_t> dist(1, UINT64_MAX);
            UI::set_adventure_seed(dist(rd), dist(rd));
            quick_start(12, 1, 1, 1);
        }
        if (ImGui::MenuItem("New random seeded run", key_string(keys["quick_restart"]).c_str()))
        {
            static std::uniform_int_distribution<uint32_t> dist(1, UINT32_MAX);
            quick_start(12, 1, 1, 1, dist(rd));
        }

        static bool first_run{true};
        if (g_bucket->adventure_seed.first == 0 && first_run)
            UI::get_adventure_seed(true);
        first_run = false;
        std::string adventure_seed_first = fmt::format("{:016X}", (uint64_t)g_bucket->adventure_seed.first);
        std::string adventure_seed_second = fmt::format("{:016X}", (uint64_t)g_bucket->adventure_seed.second);

        if (ImGui::MenuItem("Copy current seed to clipboard"))
        {
            std::string clip_str;
            if (g_state->quest_flags & 0x40)
                clip_str = fmt::format("{:08X}", g_state->seed);
            else
                clip_str = adventure_seed_first + " " + adventure_seed_second;
            set_clipboard(clip_str);
        }
        auto seed_str = fmt::format("{:08X}", g_state->seed);
        ImGui::InputText("Seeded seed", &seed_str, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase | ImGuiInputTextFlags_AutoSelectAll);
        if (ImGui::IsItemDeactivatedAfterEdit())
        {
            uint32_t new_seed;
            std::stringstream ss;
            ss << std::hex << seed_str;
            ss >> new_seed;
            if (new_seed != g_state->seed)
                quick_start(12, 1, 1, 1, new_seed);
        }
        ImGui::Separator();
        ImGui::InputText("Adventure seed##AdventureSeedFirst", &adventure_seed_first, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase | ImGuiInputTextFlags_AutoSelectAll);
        if (ImGui::IsItemDeactivatedAfterEdit())
        {
            uint64_t new_seed;
            std::stringstream ss;
            ss << std::hex << adventure_seed_first;
            ss >> new_seed;
            g_bucket->adventure_seed.first = (int64_t)new_seed;
        }
        ImGui::InputText("(whole run)##AdventureSeedSecond", &adventure_seed_second, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase | ImGuiInputTextFlags_AutoSelectAll);
        if (ImGui::IsItemDeactivatedAfterEdit())
        {
            uint64_t new_seed;
            std::stringstream ss;
            ss << std::hex << adventure_seed_second;
            ss >> new_seed;
            g_bucket->adventure_seed.second = (int64_t)new_seed;
        }
        ImGui::Separator();
        auto current_seed = UI::get_adventure_seed(false);
        std::string current_seed_first = fmt::format("{:016X}", (uint64_t)current_seed.first);
        std::string current_seed_second = fmt::format("{:016X}", (uint64_t)current_seed.second);
        ImGui::InputText("Adventure seed##CurrentSeedFirst", &current_seed_first, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase | ImGuiInputTextFlags_AutoSelectAll);
        if (ImGui::IsItemDeactivatedAfterEdit())
        {
            uint64_t new_seed;
            std::stringstream ss;
            ss << std::hex << current_seed_first;
            ss >> new_seed;
            UI::set_adventure_seed((int64_t)new_seed, current_seed.second);
        }
        ImGui::InputText("(current level)##CurrentSeedSecond", &current_seed_second, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase | ImGuiInputTextFlags_AutoSelectAll);
        if (ImGui::IsItemDeactivatedAfterEdit())
        {
            uint64_t new_seed;
            std::stringstream ss;
            ss << std::hex << current_seed_second;
            ss >> new_seed;
            UI::set_adventure_seed(current_seed.first, (int64_t)new_seed);
        }
        endmenu();
    }
    if (submenu("State"))
    {
        for (int i = 1; i <= 4; ++i)
        {
            if (ImGui::Button(fmt::format(" {} ##SaveState{}", i, i).c_str()))
                UI::save_main_state(i);
            tooltip("Save current level state", fmt::format("save_state_{}", i).c_str());
            ImGui::SameLine();
        }
        ImGui::Text("Save state");

        for (int i = 1; i <= 4; ++i)
        {
            bool valid = UI::get_save_state(i) != nullptr;
            ImGui::BeginDisabled(!valid);
            if (ImGui::Button(fmt::format(" {} ##LoadState{}", i, i).c_str()))
                load_state(i);
            ImGui::EndDisabled();
            tooltip("Load current level state", fmt::format("load_state_{}", i).c_str());
            ImGui::SameLine();
        }
        ImGui::Text("Load state");

        render_screen("Current screen", g_state->screen);
        render_screen("Last screen", g_state->screen_last);
        render_screen("Next screen", g_state->screen_next);
        std::string gamestate = "";
        if (g_state->ingame)
            gamestate += "Game ";
        if (g_state->playing)
            gamestate += "Level ";
        if (gamestate == "")
            gamestate += "Menu ";
        if (g_state->pause)
            gamestate += "Pause ";
        ImGui::LabelText("Game state", "%s", gamestate.c_str());
        endmenu();
    }
    if (submenu("Timer"))
    {
        render_timer();
        std::string lasttime = format_time(g_state->time_last_level);
        std::string leveltime = format_time(g_state->time_level);
        std::string totaltime = format_time(g_state->time_total);
        ImGui::Text("Frz");
        ImGui::Checkbox("##FreezeLast", &freeze_last);
        tooltip("Freeze time to manipulate ghost spawns or other logic.");
        ImGui::SameLine();
        if (ImGui::InputText("Last level##LastTime", &lasttime))
        {
            g_last_time = parse_time(lasttime);
            g_state->time_last_level = parse_time(lasttime);
        }
        ImGui::Checkbox("##FreezeLevel", &freeze_level);
        tooltip("Freeze time to manipulate ghost spawns or other logic.");
        ImGui::SameLine();
        if (ImGui::InputText("Level##LevelTime", &leveltime))
        {
            g_level_time = parse_time(leveltime);
            g_state->time_level = parse_time(leveltime);
        }
        ImGui::Checkbox("##FreezeTotal", &freeze_total);
        tooltip("Freeze time to manipulate ghost spawns or other logic.");
        ImGui::SameLine();
        if (ImGui::InputText("Total##TotalTime", &totaltime))
        {
            g_total_time = parse_time(totaltime);
            g_state->time_total = g_total_time;
        }
        endmenu();
    }
    if (submenu("Level"))
    {
        ImGui::InputInt2("Level size##LevelSize", (int*)&g_state->w, ImGuiInputTextFlags_ReadOnly);
        ImGui::DragScalar("World##Worldnumber", ImGuiDataType_U8, (char*)&g_state->world, 0.5f, &u8_one, &u8_max);
        ImGui::DragScalar("Level##Levelnumber", ImGuiDataType_U8, (char*)&g_state->level, 0.5f, &u8_one, &u8_max);
        ImGui::DragScalar("Theme ##Themenumber", ImGuiDataType_U8, (char*)&g_state->theme, 0.2f, &u8_one, &u8_seventeen);
        ImGui::SameLine();
        ImGui::Text("%s", theme_name(g_state->theme));
        ImGui::DragScalar("Next World##Worldnext", ImGuiDataType_U8, (char*)&g_state->world_next, 0.5f, &u8_one, &u8_max);
        ImGui::DragScalar("Next Level##Levelnext", ImGuiDataType_U8, (char*)&g_state->level_next, 0.5f, &u8_one, &u8_max);
        ImGui::DragScalar("Next Theme##Themenext", ImGuiDataType_U8, (char*)&g_state->theme_next, 0.2f, &u8_one, &u8_seventeen);
        ImGui::SameLine();
        ImGui::Text("%s", theme_name(g_state->theme_next));
        ImGui::DragScalar("Start World##Worldnext", ImGuiDataType_U8, (char*)&g_state->world_start, 0.5f, &u8_one, &u8_max);
        ImGui::DragScalar("Start Level##Levelnext", ImGuiDataType_U8, (char*)&g_state->level_start, 0.5f, &u8_one, &u8_max);
        ImGui::DragScalar("Start Theme##Themenext", ImGuiDataType_U8, (char*)&g_state->theme_start, 0.2f, &u8_one, &u8_seventeen);
        ImGui::SameLine();
        ImGui::Text("%s", theme_name(g_state->theme_start));
        ImGui::DragScalar("Levels completed##LevelsCompleted", ImGuiDataType_U8, (char*)&g_state->level_count, 0.5f, &u8_zero, &u8_max);
        endmenu();
    }
    if (submenu("Quests"))
    {
        indent(16.0f);

        if (submenu("Yang"))
        {
            if (ImGui::Checkbox("Freeze current state##FreezeQuestYang", &freeze_quest_yang))
            {
                if (freeze_quest_yang)
                    quest_yang_state = g_state->quests->yang_state;
                else
                    quest_yang_state.reset();
            }
            int yang_state = static_cast<int>(g_state->quests->yang_state);
            ImGui::RadioButton("Angry##QuestYangAngry", &yang_state, -1);
            ImGui::RadioButton("Not started##QuestYangDefault", &yang_state, 0);
            ImGui::RadioButton("Pen spawned##QuestYangPen", &yang_state, 2);
            ImGui::RadioButton("Delivered##QuestYangDelivered", &yang_state, 3);
            ImGui::RadioButton("Shop spawned##QuestYangShop", &yang_state, 4);
            ImGui::RadioButton("1 turkey bought##QuestYang1", &yang_state, 5);
            ImGui::RadioButton("2 turkeys bought##QuestYang2", &yang_state, 6);
            ImGui::RadioButton("3 turkeys bought##QuestYang3", &yang_state, 7);
            g_state->quests->yang_state = static_cast<int8_t>(yang_state);
            endmenu();
        }

        if (submenu("Jungle Sisters"))
        {
            if (ImGui::Checkbox("Freeze current state##FreezeQuestSisters", &freeze_quest_sisters))
            {
                if (freeze_quest_sisters)
                    quest_sisters_state = g_state->quests->jungle_sisters_flags;
                else
                    quest_sisters_state.reset();
            }
            int sisters_state = static_cast<int>(g_state->quests->jungle_sisters_flags);
            bool angry = sisters_state == -1;
            if (ImGui::Checkbox("Angry##QuestSistersAngry", &angry))
            {
                if (angry)
                    sisters_state = -1;
                else
                    sisters_state = 0;
            }
            if (!angry)
            {
                ImGui::CheckboxFlags("Saved Parsley##QuestSistersParsley", &sisters_state, 1);
                ImGui::CheckboxFlags("Saved Parsnip##QuestSistersParsnip", &sisters_state, 2);
                ImGui::CheckboxFlags("Saved Parmesan##QuestSistersParmesan", &sisters_state, 4);
                ImGui::CheckboxFlags("Met at Tide Pool door##QuestSistersTidePool", &sisters_state, 8);
                ImGui::CheckboxFlags("Met in wet fur##QuestSistersIceCaves", &sisters_state, 32);
                ImGui::CheckboxFlags("Met in palace##QuestSistersNeoBabylon", &sisters_state, 16);
            }
            g_state->quests->jungle_sisters_flags = static_cast<int8_t>(sisters_state);
            endmenu();
        }

        if (submenu("Van Horsing"))
        {
            if (ImGui::Checkbox("Freeze current state##FreezeQuestHorsing", &freeze_quest_horsing))
            {
                if (freeze_quest_horsing)
                    quest_horsing_state = g_state->quests->van_horsing_state;
                else
                    quest_horsing_state.reset();
            }
            int horsing_state = static_cast<int>(g_state->quests->van_horsing_state);
            ImGui::RadioButton("Angry##QuestHorsingAngry", &horsing_state, -1);
            ImGui::RadioButton("Not started##QuestHorsingDefault", &horsing_state, 0);
            ImGui::RadioButton("Cell spawned##QuestHorsingCell", &horsing_state, 1);
            ImGui::RadioButton("Got diamond##QuestHorsingDiamond", &horsing_state, 2);
            ImGui::RadioButton("Spawned in Castle##QuestHorsingVlad", &horsing_state, 3);
            ImGui::RadioButton("Shot Vlad##QuestHorsingVladShot", &horsing_state, 4);
            ImGui::RadioButton("Spawned in Temple##QuestHorsingTemple", &horsing_state, 5);
            ImGui::RadioButton("Got alien compass##QuestHorsingCompass", &horsing_state, 6);
            ImGui::RadioButton("Palace basement ending##QuestHorsingTusk", &horsing_state, 7);
            g_state->quests->van_horsing_state = static_cast<int8_t>(horsing_state);
            endmenu();
        }

        if (submenu("Sparrow"))
        {
            if (ImGui::Checkbox("Freeze current state##FreezeQuestSparrow", &freeze_quest_sparrow))
            {
                if (freeze_quest_sparrow)
                    quest_sparrow_state = g_state->quests->sparrow_state;
                else
                    quest_sparrow_state.reset();
            }
            int sparrow_state = static_cast<int>(g_state->quests->sparrow_state);
            ImGui::RadioButton("Angry##QuestSparrowAngry", &sparrow_state, -2);
            ImGui::RadioButton("Dead##QuestSparrowDead", &sparrow_state, -1);
            ImGui::RadioButton("Not started##QuestSparrowDefault", &sparrow_state, 0);
            ImGui::RadioButton("Thief##QuestSparrowThief", &sparrow_state, 1);
            ImGui::RadioButton("Finished level as thief##QuestSparrowThiefLevel", &sparrow_state, 2);
            ImGui::RadioButton("Spawned in first hideout##QuestSparrowFirst", &sparrow_state, 3);
            ImGui::RadioButton("Got ropes##QuestSparrowRopes", &sparrow_state, 4);
            ImGui::RadioButton("Stole Tusk idol##QuestSparrowTusk", &sparrow_state, 5);
            ImGui::RadioButton("Spawned in Neo Babylon##QuestSparrowSecond", &sparrow_state, 6);
            ImGui::RadioButton("Met in Neo Babylon##QuestSparrowSecondComplete", &sparrow_state, 7);
            ImGui::RadioButton("Palace basement ending##QuestSparrowTusk", &sparrow_state, 8);
            ImGui::RadioButton("Final reward thrown##QuestSparrowFinalRewardThrown", &sparrow_state, 9);
            g_state->quests->sparrow_state = static_cast<int8_t>(sparrow_state);
            endmenu();
        }

        if (submenu("Madame Tusk"))
        {
            if (ImGui::Checkbox("Freeze current state##FreezeQuestTusk", &freeze_quest_tusk))
            {
                if (freeze_quest_tusk)
                    quest_tusk_state = g_state->quests->madame_tusk_state;
                else
                    quest_tusk_state.reset();
            }
            int tusk_state = static_cast<int>(g_state->quests->madame_tusk_state);
            ImGui::RadioButton("Angry##QuestTuskAngry", &tusk_state, -2);
            ImGui::RadioButton("Dead##QuestTuskDead", &tusk_state, -1);
            ImGui::RadioButton("Not started##QuestTuskDefault", &tusk_state, 0);
            ImGui::RadioButton("Dice house spawned##QuestTuskDice", &tusk_state, 1);
            ImGui::RadioButton("High roller##QuestTuskHighRoller", &tusk_state, 2);
            ImGui::RadioButton("Palace ending##QuestTuskPalace", &tusk_state, 3);
            g_state->quests->madame_tusk_state = static_cast<int8_t>(tusk_state);
            endmenu();
        }

        if (submenu("Beg"))
        {
            if (ImGui::Checkbox("Freeze current state##FreezeQuestBeg", &freeze_quest_beg))
            {
                if (freeze_quest_beg)
                    quest_beg_state = g_state->quests->beg_state;
                else
                    quest_beg_state.reset();
            }
            int beg_state = static_cast<int>(g_state->quests->beg_state);
            ImGui::RadioButton("Angry##QuestBegAngry", &beg_state, -1);
            ImGui::RadioButton("Not started##QuestBegDefault", &beg_state, 0);
            ImGui::RadioButton("Altar destroyed##QuestBegAltar", &beg_state, 1);
            ImGui::RadioButton("Spawned with bombs##QuestBegBombs", &beg_state, 2);
            ImGui::RadioButton("Got bombs##QuestBegGotBombs", &beg_state, 3);
            ImGui::RadioButton("Spawned with true crown##QuestBegCrown", &beg_state, 4);
            ImGui::RadioButton("Got true crown##QuestBegGotCrown", &beg_state, 5);
            g_state->quests->beg_state = static_cast<int8_t>(beg_state);
            endmenu();
        }

        unindent(16.0f);
        endmenu();
    }
    if (submenu("Street cred"))
    {
        ImGui::DragScalar("Shoppie aggro##ShoppieAggro", ImGuiDataType_U8, &g_state->shoppie_aggro, 0.5f, &u8_min, &u8_max, "%d");
        ImGui::DragScalar("Shoppie aggro levels##ShoppieAggroLevels", ImGuiDataType_U8, &g_state->shoppie_aggro_levels, 0.5f, &u8_min, &u8_max, "%d");
        ImGui::DragScalar("Tun aggro##MerchantAggro", ImGuiDataType_U8, &g_state->merchant_aggro, 0.5f, &u8_min, &u8_max, "%d");
        ImGui::DragScalar("NPC kills##NPCKills", ImGuiDataType_U8, (char*)&g_state->kills_npc, 0.5f, &u8_zero, &u8_max);
        ImGui::DragScalar("Kali favor##PorFavor", ImGuiDataType_S8, (char*)&g_state->kali_favor, 0.5f, &s8_min, &s8_max);
        ImGui::DragScalar("Kali status##KaliStatus", ImGuiDataType_S8, (char*)&g_state->kali_status, 0.5f, &s8_min, &s8_max);
        ImGui::DragScalar("Altars destroyed##KaliAltars", ImGuiDataType_S8, (char*)&g_state->kali_altars_destroyed, 0.5f, &s8_min, &s8_max);
        endmenu();
    }
    if (submenu("Players"))
    {
        if (ImGui::MenuItem("Respawn dead players"))
            respawn();
        if (ImGui::SliderScalar("Number of players##SetNumPlayers", ImGuiDataType_U8, &g_state->items->player_count, &u8_one, &u8_four, "%d", ImGuiSliderFlags_AlwaysClamp))
        {
            std::array<bool, 4> active_players{false, false, false, false};
            for (int i = 0; i < 4; ++i)
            {
                g_state->items->player_select_slots[i].activated = g_state->items->player_count >= i + 1;
                for (auto player : g_players)
                {
                    if (player->input_ptr->player_slot == i && g_state->items->player_count < i + 1)
                        player->kill(true, nullptr);
                    else if (player->input_ptr->player_slot >= 0)
                        active_players[player->input_ptr->player_slot] = true;
                }
                for (auto uid : UI::get_entities_by({to_id("ENT_TYPE_ITEM_PLAYERGHOST")}, ENTITY_MASK::ITEM, LAYER::BOTH))
                {
                    auto ghost = get_entity_ptr(uid)->as<PlayerGhost>();
                    if (ghost->player_inputs && ghost->player_inputs->player_slot == i && g_state->items->player_count < i + 1)
                        ghost->destroy();
                }
            }
            auto [spawn_x, spawn_y] = UI::spawn_position();
            if (g_players.size() > 0)
            {
                spawn_x = g_players.at(0)->abs_x;
                spawn_y = g_players.at(0)->abs_y;
            }
            for (uint8_t i = 0; i < g_state->items->player_count; ++i)
            {
                if (!active_players[i])
                {
                    g_state->items->player_inventories[i].health = 4;
                    auto uid = g_state->next_entity_uid;
                    UI::spawn_player(i);
                    auto player = get_entity_ptr(uid)->as<Player>();
                    player->set_position(spawn_x, spawn_y);
                }
            }
        }
        ImGui::SeparatorText("Players");
        render_players();
        ImGui::SeparatorText("Player inputs");
        ImGui::PushID("PlayerInputIndex");
        for (uint8_t i = 0; i < 5; ++i)
        {
            ImGui::PushID(i);
            auto label = i < 4 ? fmt::format("Player {}##PlayerInput{}", i + 1, i) : "Menu?";
            auto index = g_game_manager->game_props->input_index[i];
            if (ImGui::BeginCombo(label.c_str(), player_inputs[index + 1]))
            {
                for (int8_t j = -1; j < 12; j++)
                {
                    const bool item_selected = (j == index);
                    const char* item_text = player_inputs[j + 1];

                    ImGui::PushID(j);
                    if (ImGui::Selectable(item_text, item_selected))
                        g_game_manager->game_props->input_index[i] = j;
                    if (item_selected)
                        ImGui::SetItemDefaultFocus();
                    ImGui::PopID();
                }
                ImGui::EndCombo();
            }
            ImGui::PopID();
        }
        ImGui::PopID();
        endmenu();
    }
    if (submenu("Level flags"))
    {
        render_flags(level_flags, &g_state->level_flags);
        endmenu();
    }
    if (submenu("Quest flags"))
    {
        render_flags(quest_flags, &g_state->quest_flags);
        endmenu();
    }
    if (submenu("Journal flags"))
    {
        render_flags(journal_flags, &g_state->journal_flags);
        endmenu();
    }
    if (submenu("Presence flags"))
    {
        render_flags(presence_flags, &g_state->presence_flags);
        endmenu();
    }
    if (submenu("Special visibility flags"))
    {
        render_flags(special_visibility_flags, &g_state->special_visibility_flags);
        endmenu();
    }
    if (submenu("Level generation flags"))
    {
        ImGui::SeparatorText("Flags 1");
        render_flags(levelgen_flags, &g_state->level_gen->flags);
        ImGui::SeparatorText("Flags 2");
        render_flags(levelgen_flags2, &g_state->level_gen->flags2);
        ImGui::SeparatorText("Flags 3");
        render_flags(levelgen_flags3, &g_state->level_gen->flags3);
        if (g_state->current_theme)
        {
            ImGui::SeparatorText("Theme flags");
            ImGui::Checkbox("Allow beehives##ThemeBeeHive", &g_state->current_theme->allow_beehive);
            ImGui::Checkbox("Allow leprechauns##ThemeLeprechaun", &g_state->current_theme->allow_leprechaun);
        }
        endmenu();
    }
    if (submenu("Renderer flags"))
    {
        auto game_api = GameAPI::get();
        ImGui::SeparatorText("Flags 1");
        render_flags(renderer_flags1, &game_api->renderer->flags1);
        endmenu();
    }
    if (submenu("Procedural chances"))
    {
        static auto hide_zero = true;
        ImGui::Checkbox("Hide 0% chances", &hide_zero);
        static auto render_procedural_chance = [](uint32_t id, const LevelChanceDef& def)
        {
            int inverse_chance = g_state->level_gen->get_procedural_spawn_chance(id);
            std::string name = std::string(g_state->level_gen->get_procedural_spawn_chance_name(id).value_or(fmt::format("{}", id)));
            if (def.chances.empty() || (hide_zero && inverse_chance == 0))
                return;
            float chance = inverse_chance > 0 ? 100.f / static_cast<float>(inverse_chance) : 0;
            std::string all = fmt::format("{}", def.chances[0]);
            for (auto i = 1; i < def.chances.size(); ++i)
                all += "," + fmt::format("{}", def.chances[i]);
            std::string str = fmt::format("{:.3f}% ({})", chance, all);
            ImGui::Text("%s", name.c_str());
            auto w = ImGui::GetItemRectSize().x;
            ImGui::SameLine(std::max(0.5f * ImGui::GetContentRegionMax().x, w), 4.f);
            ImGui::Text("%s", str.c_str());
        };

        static auto render_chance = [](int inverse_chance, const char* name)
        {
            if (hide_zero && inverse_chance == 0)
                return;
            float chance = inverse_chance > 0 ? 100.f / static_cast<float>(inverse_chance) : 0;
            std::string str = fmt::format("{:.3f}%", chance);
            ImGui::Text("%s", name);
            auto w = ImGui::GetItemRectSize().x;
            ImGui::SameLine(std::max(0.5f * ImGui::GetContentRegionMax().x, w), 4.f);
            ImGui::Text("%s", str.c_str());
        };

        ImGui::SeparatorText("Monster chances");
        for (auto& [id, def] : g_state->level_gen->data->level_monster_chances)
            render_procedural_chance(id, def);

        ImGui::SeparatorText("Trap chances");
        for (auto& [id, def] : g_state->level_gen->data->level_trap_chances)
            render_procedural_chance(id, def);

        ImGui::SeparatorText("Level chances");
        if (g_state->current_theme)
            render_chance(g_state->current_theme->get_shop_chance(), "shop");
        for (auto i = 0; i < 15; ++i)
            render_chance(g_state->level_gen->data->level_config[i], level_chances[i]);
        endmenu();
    }
    if (submenu("AI targets"))
    {
        for (size_t x = 0; x < 8; ++x)
        {
            auto ai_target = g_state->ai_targets[x];
            if (ai_target.ai_uid == 0)
            {
                continue;
            }
            auto ai_entity = get_entity_ptr(ai_target.ai_uid);
            auto target = ai_target.target_uid;
            if (ai_entity == nullptr || (ai_entity->type->search_flags & ENTITY_MASK::PLAYER) != ENTITY_MASK::PLAYER)
            {
                continue;
            }
            ImGui::Text("%s", entity_names[ai_entity->type->id].c_str());
            ImGui::SameLine();
            ImGui::Text(": ");
            ImGui::SameLine();
            if (std::cmp_equal(target, -1))
            {
                ImGui::Text("Nothing");
            }
            else
            {
                auto target_entity = get_entity_ptr(target);
                if (target_entity != nullptr)
                {
                    ImGui::Text("%s", entity_names[target_entity->type->id].c_str());
                }
                else
                {
                    ImGui::Text("Invalid target uid: %s", std::to_string(target).c_str());
                }
            }
        }
        endmenu();
    }
    if (submenu("Global illumination"))
    {
        if (g_state->illumination)
            render_illumination(g_state->illumination, "Global illumination");
        endmenu();
    }
    if (submenu("Liquid pools"))
    {
        static bool global_pause = false;
        bool global_pause_changed = ImGui::Checkbox("Global pause physics##LiquidGlobalPause", &global_pause);
        static float global_gravity = 1.0f;
        bool global_gravity_changed = ImGui::DragFloat("Global gravity##LiquidGlobalGravity", &global_gravity, 0.01f, -5.f, 5.f);
        for (int i = 0; i < 5; ++i)
        {
            render_liquid_pool(i);
            if (g_state->liquid_physics->pools[i].physics_engine)
            {
                if (global_pause_changed)
                    g_state->liquid_physics->pools[i].physics_engine->pause_physics = global_pause;
                if (global_gravity_changed)
                    g_state->liquid_physics->pools[i].physics_engine->gravity = global_gravity;
            }
        }
        endmenu();
    }
    ImGui::PopItemWidth();
}

void load_font()
{
    ImGuiIO& io = ImGui::GetIO();
    io.FontAllowUserScaling = false;
    PWSTR fontdir;

    ImFontConfig font_config;
    font_config.MergeMode = true;
    font_config.EllipsisChar = u'\u2026';
    static const ImWchar emoji_range[] = {0x1, 0x1FFFF, 0};
    static const ImWchar ellipsis_range[] = {0x2026, 0x2026, 0}; // one of the asian fonts has a stupid huge ellipsis, we get it explicitly from segoe

    std::string font_default;
    std::string font_jp;
    std::string font_ko;
    std::string font_ru;
    std::string font_zhcn;
    std::string font_zhtw;
    std::string font_emoji;

    if (SHGetKnownFolderPath(FOLDERID_Fonts, 0, NULL, &fontdir) == S_OK)
    {
        std::string fontpath_jp(cvt.to_bytes(fontdir) + "\\YuGothB.ttc");
        if (GetFileAttributesA(fontpath_jp.c_str()) != INVALID_FILE_ATTRIBUTES)
            font_jp = fontpath_jp;

        std::string fontpath_jp2(cvt.to_bytes(fontdir) + "\\Meiryo.ttc");
        if (font_jp == "" && GetFileAttributesA(fontpath_jp2.c_str()) != INVALID_FILE_ATTRIBUTES)
            font_jp = fontpath_jp2;

        std::string fontpath_ko(cvt.to_bytes(fontdir) + "\\malgunbd.ttf");
        if (GetFileAttributesA(fontpath_ko.c_str()) != INVALID_FILE_ATTRIBUTES)
            font_ko = fontpath_ko;

        std::string fontpath_ko2(cvt.to_bytes(fontdir) + "\\Gulim.ttc");
        if (font_ko == "" && GetFileAttributesA(fontpath_ko2.c_str()) != INVALID_FILE_ATTRIBUTES)
            font_ko = fontpath_ko2;

        std::string fontpath_ru(cvt.to_bytes(fontdir) + "\\segoeuib.ttf");
        if (GetFileAttributesA(fontpath_ru.c_str()) != INVALID_FILE_ATTRIBUTES)
            font_ru = fontpath_ru;

        std::string fontpath_zhcn(cvt.to_bytes(fontdir) + "\\simsun.ttc");
        if (GetFileAttributesA(fontpath_zhcn.c_str()) != INVALID_FILE_ATTRIBUTES)
            font_zhcn = fontpath_zhcn;

        std::string fontpath_zhtw(cvt.to_bytes(fontdir) + "\\msjh.ttc");
        if (GetFileAttributesA(fontpath_zhtw.c_str()) != INVALID_FILE_ATTRIBUTES)
            font_zhtw = fontpath_zhtw;

        std::string fontpath_emoji(cvt.to_bytes(fontdir) + "\\seguiemj.ttf");
        if (GetFileAttributesA(fontpath_emoji.c_str()) != INVALID_FILE_ATTRIBUTES)
            font_emoji = fontpath_emoji;

        if (fontfile != "")
        {
            std::string fontpath(cvt.to_bytes(fontdir) + "\\" + fontfile);
            if (GetFileAttributesA(fontpath.c_str()) != INVALID_FILE_ATTRIBUTES)
            {
                font_default = fontpath;
            }
            else if (SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &fontdir) == S_OK)
            {
                std::string localfontpath(cvt.to_bytes(fontdir) + "\\Microsoft\\Windows\\Fonts\\" + fontfile);
                if (GetFileAttributesA(localfontpath.c_str()) != INVALID_FILE_ATTRIBUTES)
                {
                    font_default = localfontpath;
                }
            }
        }
        CoTaskMemFree(fontdir);
    }

    if (font_default != "")
        font = io.Fonts->AddFontFromFileTTF(font_default.c_str(), fontsize[0]);
    else
        font = io.Fonts->AddFontFromMemoryCompressedTTF(OLFont_compressed_data, OLFont_compressed_size, fontsize[0]);

    if (font_ru != "")
    {
        io.Fonts->AddFontFromFileTTF(font_ru.c_str(), fontsize[0], &font_config, io.Fonts->GetGlyphRangesCyrillic());
        io.Fonts->AddFontFromFileTTF(font_ru.c_str(), fontsize[0], &font_config, ellipsis_range);
    }
    if (font_jp != "")
        io.Fonts->AddFontFromFileTTF(font_jp.c_str(), fontsize[0], &font_config, io.Fonts->GetGlyphRangesJapanese());
    if (font_ko != "")
        io.Fonts->AddFontFromFileTTF(font_ko.c_str(), fontsize[0], &font_config, io.Fonts->GetGlyphRangesKorean());
    if (font_zhcn != "")
        io.Fonts->AddFontFromFileTTF(font_zhcn.c_str(), fontsize[0], &font_config, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
    if (font_zhtw != "")
        io.Fonts->AddFontFromFileTTF(font_zhtw.c_str(), fontsize[0], &font_config, io.Fonts->GetGlyphRangesChineseFull());
    if (font_emoji != "")
        io.Fonts->AddFontFromFileTTF(font_emoji.c_str(), fontsize[0], &font_config, emoji_range);

    bigfont = io.Fonts->AddFontFromMemoryCompressedTTF(OLFont_compressed_data, OLFont_compressed_size, fontsize[1]);
    hugefont = io.Fonts->AddFontFromMemoryCompressedTTF(OLFont_compressed_data, OLFont_compressed_size, fontsize[2]);
}

void render_spawner()
{
    int n = 0;
    for (auto kit : kits)
    {
        ImGui::PushID(kit->items.c_str());
        std::string search = "";
        std::stringstream sss(kit->items);
        int item = 0;
        while (sss >> item)
        {
            std::string name = entity_names[item];
            name = name.substr(name.find_last_of("_") + 1);
            if (search.find(name) == std::string::npos)
                search += name + " ";
        }
        if (search.length() > 1)
        {
            search.pop_back();
        }
        if (search.empty())
            search = kit->items;
        ImGui::Text("%d:", n + 1);
        ImGui::SameLine();
        ImGui::TextWrapped("%s", search.c_str());
        ImGui::PushID(2 * n);
        if (ImGui::Button("X"))
        {
            kits.erase(kits.begin() + n);
            save_config(cfgfile);
        }
        tooltip("Delete kit.");
        ImGui::SameLine();
        ImGui::PopID();

        ImGui::PushID(4 * n);
        if (ImGui::Button("Load"))
        {
            text = kit->items;
            update_filter(text);
            // spawn_entities(false);
        }
        tooltip("Edit kit or spawn later with mouse.");
        ImGui::SameLine();
        ImGui::PopID();

        ImGui::PushID(8 * n);
        if (ImGui::Button("Spawn"))
        {
            spawn_kit(kit);
        }
        tooltip("Spawn saved kit where you're standing,\nautomatically equipping anything wearable.", "spawn_kit_1");
        ImGui::SameLine();
        ImGui::PopID();

        ImGui::PushID(16 * n);
        ImGui::Checkbox("Auto spawn", &kit->automatic);
        tooltip("Spawn automatically on new game.", "");
        ImGui::SameLine();
        ImGui::PopID();

        ImGui::PushID(32 * n);
        if (ImGui::Button("Add item"))
        {
            if (g_current_item > 0 || (unsigned)g_filtered_count < g_items.size())
            {
                EntityItem to_add = g_items[g_filtered_items[g_current_item]];
                trim(kit->items);
                kit->items = fmt::format("{} {}", kit->items, to_add.id);
                trim(kit->items);
                save_config(cfgfile);
            }
        }
        tooltip("Add selected item to this kit.", "");
        ImGui::PopID();

        ImGui::PopID();
        n++;
        ImGui::Separator();
    }
    ImVec2 region = ImGui::GetContentRegionMax();

    ImGui::PushID("HotbarSet");
    ImGui::Text("Add selected item to hotbar:");
    const float buttonwidth = 1.0f / 11.0f * region.x - 4.0f;
    for (uint32_t i = 0; i < 10; ++i)
    {
        if (!hotbar.contains(i))
            ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
        else
            ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_Button]);
        if (ImGui::Button(fmt::format("{}", i < 9 ? i + 1 : 0).c_str(), {buttonwidth, 0}))
        {
            hotbar[i] = g_items[g_filtered_items[g_current_item]].id;
            save_config(cfgfile);
        }
        ImGui::PopStyleColor();
        ImGui::SameLine(0, 4.0f);
    }
    if (ImGui::Button("+", {buttonwidth, 0}))
    {
        for (uint32_t i = 0; i < 20; ++i)
        {
            if (!hotbar.contains(i))
            {
                hotbar[i] = g_items[g_filtered_items[g_current_item]].id;
                save_config(cfgfile);
                break;
            }
        }
        save_config(cfgfile);
    }
    ImGui::PopID();

    if (set_focus_entity)
    {
        ImGui::SetKeyboardFocusHere();
        set_focus_entity = false;
    }
    ImGui::PushItemWidth(0.6667f * region.x);
    if (ImGui::InputText("##Input", &text, ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_AutoSelectAll, pick_selected_entity))
    {
        update_filter(text);
    }
    if (ImGui::IsItemFocused())
        focused_tool = "tool_entity";
    tooltip("Search for entities to spawn. Hit TAB to add the selected id to list.");
    ImGui::SameLine(0, 4.0f);
    if (ImGui::Button("Save kit", ImVec2(0.16665f * region.x - 4.0f, 0.0f)))
    {
        save_search();
    }
    tooltip("Save entity id(s) or selected item as a kit for quick use later.");
    ImGui::SameLine(0, 4.0f);
    if (ImGui::Button("Spawn", ImVec2(0.16665f * region.x - 4.0f, 0.0f)))
    {
        spawn_entities(false);
    }
    tooltip("Spawn selected entity where you're standing.", "spawn_entity");
    ImGui::PopItemWidth();

    ImGui::PushItemWidth(region.x);
    ImVec2 boxsize = {-1, -1};
    if (options["menu_ui"] && !detached("tool_entity"))
        boxsize = {392.0f, 392.0f};
    if (!ImGui::ListBoxHeader("##Entities", boxsize))
        return;
    ImGuiListClipper clipper;
    clipper.Begin(g_filtered_count, -1.0f);
    int select_last_index = 0;
    if (g_last_type >= 0 && g_filtered_count == g_items.size())
    {
        for (int j = 0; j < g_filtered_count; ++j)
        {
            if (g_items[j].id == (uint32_t)g_last_type)
            {
                select_last_index = j;
                scroll_top = false;
            }
        }
        clipper.ForceDisplayRangeByIndices(select_last_index, select_last_index + 1);
    }
    if (scroll_top)
    {
        scroll_top = false;
        ImGui::SetScrollHereY();
    }

    while (clipper.Step())
    {
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
        {
            const bool select_last = (g_last_type >= 0 && g_items[g_filtered_items[i]].id == (uint32_t)g_last_type);
            if (select_last)
            {
                g_current_item = i;
                g_last_type = -1;
                scroll_to_entity = true;
            }
            const bool item_selected = (i == g_current_item);
            std::stringstream item_ss;
            item_ss << g_items[g_filtered_items[i]].id;
            std::string item_id = item_ss.str();
            std::string item_name = g_items[g_filtered_items[i]].name.c_str();
            std::string item_concat = item_id + ": " + item_name.substr(9);
            const char* item_text = item_concat.c_str();
            ImGui::PushID(i);
            if (ImGui::Selectable(item_text, item_selected))
            {
                g_last_type = -1;
                g_current_item = i;
            }
            if (item_selected)
            {
                if (scroll_to_entity)
                {
                    ImGui::SetScrollHereY();
                    scroll_to_entity = false;
                }
                // ImGui::SetItemDefaultFocus();
            }
            ImGui::PopID();
        }
    }
    ImGui::ListBoxFooter();
    ImGui::PopItemWidth();
}

std::string hud_input(int buttons)
{
    std::wstring input = L"JWBRSDMT";
    for (int i = 0; i < 8; ++i)
    {
        if (!(buttons & (int)std::pow(2, i)))
            input[i] = ' ';
    }
    if (buttons & 0x100)
        input += L"←";
    else if (buttons & 0x200)
        input += L"→";
    if (buttons & 0x400)
        input += L"↑";
    else if (buttons & 0x800)
        input += L"↓";
    return std::string(cvt.to_bytes(input));
}

AABB player_hud_position(int p = 0)
{
    float ax = -0.98f;
    float f = 1.0f;
    uint32_t hs = get_setting(GAME_SETTING::HUD_SIZE).value_or(0);
    if (hs == 0 || g_state->items->player_count > 3)
        f = 1.0f;
    else if (hs == 1 || g_state->items->player_count > 2)
        f = 1.15f;
    else
        f = 1.3f;
    float w = 0.32f * f;

    float ay = 0.94f - (1.0f - f) * 0.1f;
    float h = 0.2f * f;

    return AABB(ax + p * w + 0.02f * f, ay, ax + p * w + w - 0.02f * f, ay - h);
}

void render_prohud()
{
    static float engine_fps = 0;
    static std::chrono::time_point<std::chrono::system_clock> last_time;
    static uint32_t last_frame;
    auto this_frame = UI::get_frame_count();
    auto frame_delta = this_frame - last_frame;
    auto this_time = std::chrono::system_clock::now();
    auto time_delta = std::chrono::duration<float>(this_time - last_time);
    if (time_delta.count() > 1.0f)
    {
        engine_fps = static_cast<float>(frame_delta) / time_delta.count();
        last_frame = this_frame;
        last_time = this_time;
    }
    g_speedhack_ui_multiplier = UI::get_speedhack();

    const auto& io = ImGui::GetIO();
    auto base = ImGui::GetMainViewport();
    ImDrawList* dl = ImGui::GetBackgroundDrawList(base);
    auto topmargin = 0.0f;
    if (options["menu_ui"] && !hide_ui)
        topmargin = ImGui::GetTextLineHeight();
    std::string buf = fmt::format("TIMER:{}/{} GLOBAL:{:#06} FRAME:{:#06} START:{:#06} TOTAL:{:#06} LEVEL:{:#06} COUNT:{} SCREEN:{} SIZE:{}x{} PAUSE:{} FPS:{:.2f} ENGINE:{:.2f} TARGET:{:.2f}", format_time(g_state->time_level), format_time(g_state->time_total), API::get_global_frame_count(), UI::get_frame_count(), g_state->time_startup, g_state->time_total, g_state->time_level, g_state->level_count, g_state->screen, g_state->w, g_state->h, g_state->pause, io.Framerate, engine_fps, g_engine_fps);
    ImVec2 textsize = ImGui::CalcTextSize(buf.c_str());
    dl->AddText({base->Pos.x + base->Size.x / 2 - textsize.x / 2, base->Pos.y + 2 + topmargin}, ImColor(1.0f, 1.0f, 1.0f, .5f), buf.c_str());

    buf = fmt::format("{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}", (options["god_mode"] ? "GODMODE " : ""), (options["god_mode_companions"] ? "HHGODMODE " : ""), (options["noclip"] ? "NOCLIP " : ""), (options["fly_mode"] ? "FLY " : ""), (options["lights"] ? "LIGHTS " : ""), (test_flag(g_dark_mode, 1) ? "DARK " : ""), (test_flag(g_dark_mode, 2) ? "NODARK " : ""), (options["disable_ghost_timer"] ? "NOGHOST " : ""), (options["disable_achievements"] ? "NOSTEAM " : ""), (options["disable_savegame"] ? "NOSAVE " : ""), (options["disable_pause"] ? "NOPAUSE " : ""), (g_zoom != 13.5 ? fmt::format("ZOOM:{:.2f} ", g_zoom) : ""), (options["speedhack"] ? "TURBO " : ""), (g_speedhack_ui_multiplier != 1.0 ? fmt::format("SPEEDHACK:{:.2f}x ", g_speedhack_ui_multiplier) : ""), (!options["mouse_control"] ? "NOMOUSE " : ""), (!options["keyboard_control"] ? "NOKEYBOARD " : ""));
    textsize = ImGui::CalcTextSize(buf.c_str());
    dl->AddText({base->Pos.x + base->Size.x / 2 - textsize.x / 2, base->Pos.y + textsize.y + 4 + topmargin}, ImColor(1.0f, 1.0f, 1.0f, .5f), buf.c_str());

    auto type = spawned_type();
    buf = fmt::format("{}", (type == "" ? "" : fmt::format("SPAWN:{}{}", type, options["snap_to_grid"] ? " SNAP" : "")));
    textsize = ImGui::CalcTextSize(buf.c_str());
    dl->AddText({base->Pos.x + base->Size.x / 2 - textsize.x / 2, base->Pos.y + textsize.y * 2 + 4 + topmargin}, ImColor(1.0f, 1.0f, 1.0f, .5f), buf.c_str());

    if (g_bucket->pause_api->get_pause() != PAUSE_TYPE::NONE)
    {
        auto col = ImColor(0.3f, 1.0f, 0.3f, 0.7f);
        if (!g_bucket->pause_api->paused() || g_state->pause > 0)
            col = ImColor(1.0f, 1.0f, 0.3f, 0.7f);
        if (g_bucket->pause_api->paused() && (g_bucket->pause_api->blocked || g_state->pause > 0))
            col = ImColor(1.0f, 0.3f, 0.3f, 0.7f);
        buf = "||";
        ImGui::PushFont(bigfont);
        textsize = ImGui::CalcTextSize(buf.c_str());
        dl->AddText({base->Pos.x + base->Size.x / 2 - textsize.x / 2, base->Pos.y + 80}, col, buf.c_str());
        ImGui::PopFont();
    }

    static std::array<int, 4> inputs{0};
    static uint32_t last_input_frame = 0;
    if (g_state->time_level != last_input_frame)
    {
        for (int i = 0; i < g_state->items->player_count; ++i)
            inputs[i] = (int)g_state->player_inputs->player_slots[i].buttons_gameplay;
        last_input_frame = g_state->time_level;
    }
    for (int i = 0; i < g_state->items->player_count; ++i)
    {
        auto pos = player_hud_position(i);
        auto w = (pos.right - pos.left) / 20.f;
        auto pw = screenify(w);
        auto [ax, ay] = fix_pos(screenify({pos.left, pos.top}));
        auto [bx, by] = fix_pos(screenify({pos.right, pos.bottom}));
        buf = hud_input((int)g_state->player_inputs->player_slots[i].buttons_gameplay);
        textsize = ImGui::CalcTextSize(buf.c_str());
        dl->AddText({ax + pw, by}, ImColor(1.0f, 1.0f, 1.0f, .5f), buf.c_str());

        buf = hud_input(inputs[i]);
        textsize = ImGui::CalcTextSize(buf.c_str());
        dl->AddText({ax + pw, by + textsize.y}, ImColor(0.5f, 1.0f, 0.5f, .5f), buf.c_str());
    }
}

void render_tool(std::string tool)
{
    active_tab = tool;
    if (tool == "tool_entity")
        render_spawner();
    else if (tool == "tool_door")
        render_narnia();
    else if (tool == "tool_camera")
        render_camera();
    else if (tool == "tool_entity_properties")
        render_entity_props(g_last_id);
    else if (tool == "tool_game_properties")
        render_game_props();
    else if (tool == "tool_script")
        render_scripts();
    else if (tool == "tool_options")
        render_options();
    else if (tool == "tool_style")
        render_style_editor();
    else if (tool == "tool_debug")
        render_debug();
    else if (tool == "tool_save")
        render_savegame();
    else if (tool == "tool_keys")
        render_keyconfig();
    else if (tool == "tool_finder")
        render_entity_finder();
    else if (tool == "tool_texture")
        render_texture_viewer();
}

bool is_tab_open(std::string name)
{
    return std::find(tabs_open.begin(), tabs_open.end(), name) != tabs_open.end();
}

bool is_tab_detached(std::string name)
{
    return std::find(tabs_detached.begin(), tabs_detached.end(), name) != tabs_detached.end();
}

void imgui_pre_init(ImGuiContext*)
{
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDrawCursor = true;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    if (!detect_wine())
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigViewportsNoTaskBarIcon = true;
}

void add_ui_script(std::string name, bool enable, std::string code)
{
    if (g_ui_scripts.find(name) == g_ui_scripts.end())
    {
        SpelunkyScript* script = new SpelunkyScript(
            code,
            name,
            g_SoundManager.get(),
            g_Console.get(),
            enable);
        g_ui_scripts[name] = std::unique_ptr<SpelunkyScript>(script);
    }
}

void imgui_init(ImGuiContext*)
{
    if (std::setlocale(LC_CTYPE, ".UTF-8") == nullptr)
    {
        ERR("Can not set code-page to utf-8, some scripts may cause a crash...");
    }

    show_cursor();
    load_config(cfgfile);
    load_font();
    load_cursor();
    refresh_script_files();
    autorun_scripts();
    set_colors();
    version_check();
    windows["tool_entity"] = new Window({"Spawner", is_tab_detached("tool_entity"), is_tab_open("tool_entity")});
    windows["tool_door"] = new Window({"Warp", is_tab_detached("tool_door"), is_tab_open("tool_door")});
    windows["tool_camera"] = new Window({"Camera", is_tab_detached("tool_camera"), is_tab_open("tool_camera")});
    windows["tool_entity_properties"] = new Window({"Entity", is_tab_detached("tool_entity_properties"), is_tab_open("tool_entity_properties")});
    windows["tool_game_properties"] = new Window({"Game", is_tab_detached("tool_game_properties"), is_tab_open("tool_game_properties")});
    windows["tool_options"] = new Window({"Options", is_tab_detached("tool_options"), is_tab_open("tool_options")});
    windows["tool_debug"] = new Window({"Debug", is_tab_detached("tool_debug"), is_tab_open("tool_debug")});
    windows["tool_style"] = new Window({"Style", is_tab_detached("tool_style"), is_tab_open("tool_style")});
    windows["tool_script"] = new Window({"Scripts", is_tab_detached("tool_script"), is_tab_open("tool_script")});
    windows["tool_save"] = new Window({"Savegame", is_tab_detached("tool_save"), is_tab_open("tool_save")});
    windows["tool_keys"] = new Window({"Keys", is_tab_detached("tool_keys"), is_tab_open("tool_keys")});
    windows["tool_finder"] = new Window({"Finder", is_tab_detached("tool_finder"), is_tab_open("tool_finder")});
    windows["tool_texture"] = new Window({"Texture viewer", is_tab_detached("tool_texture"), is_tab_open("tool_texture")});
    // windows["tool_sound"] = new Window({"Sound player", is_tab_detached("tool_sound"), is_tab_open("tool_sound")});

    add_ui_script("camera_hack", false, R"(
lastpos = Vec2:new()
set_callback(function()
    local e = get_entity(state.camera.focused_entity_uid)
    if not e then return end
    local x,y,l = get_render_position(e.uid)
    local pos = Vec2:new(x, y)
    if pos:distance_to(lastpos) > 1 then x,y,l = get_position(e.uid) end
    lastpos = pos
    x = x + state.camera.focus_offset_x
    y = y + state.camera.vertical_pan + state.camera.focus_offset_y
    set_camera_position(x, y)
end, ON.RENDER_PRE_GAME))");
    add_ui_script("dark", false, "set_callback(function() state.level_flags = set_flag(state.level_flags, 18) end, ON.POST_ROOM_GENERATION)");
    add_ui_script("light", false, "set_callback(function() state.level_flags = clr_flag(state.level_flags, 18) end, ON.POST_ROOM_GENERATION)");
    add_ui_script("void", false, R"(
qflags = {2,3,5,17,18,19,25,26,27}
disable_virts = {2,3,4,5,6,7,8,9,10,11,12,15,16,17,18,19,20}
hooks = {}
function add_hook(t, cb)
    hooks[#hooks+1] = {t=t, cb=cb}
end
function clear_hooks()
    for i,h in pairs(hooks) do
        state.level_gen.themes[h.t]:clear_virtual(h.cb)
    end
    hooks = {}
end
function init_hooks()
    clear_hooks()
    for t=THEME.DWELLING,THEME.HUNDUN do
        for i,v in pairs(disable_virts) do
            add_hook(t, state.level_gen.themes[t]:set_pre_virtual(v, function() return true end))
        end
        add_hook(t, state.level_gen.themes[t]:set_pre_reset_theme_flags(function()
            for i=LEVEL_CONFIG.BACK_ROOM_CHANCE,LEVEL_CONFIG.MACHINE_REWARDROOM_CHANCE do
                set_level_config(i, 0)
            end
            state.current_theme.allow_beehive = false
            state.current_theme.allow_leprechaun = false
            for i,v in pairs(qflags) do
                state.quest_flags = set_flag(state.quest_flags, v)
            end
            state.quests.yang_state = 7
            state.quests.jungle_sisters_flags = 63
            state.quests.van_horsing_state = 7
            state.quests.sparrow_state = 8
            state.quests.madame_tusk_state = 3
            return true
        end))
        add_hook(t, state.level_gen.themes[t]:set_pre_spawn_effects(function()
            if state.current_theme:get_loop() then
                state.camera.bounds_left = -math.huge
                state.camera.bounds_right = math.huge
                state.camera.bounds_top = math.huge
                state.camera.bounds_bottom = -math.huge
            else
                state.camera.bounds_left = 0.5
                state.camera.bounds_right = state.width * 10.0 + 4.5
                state.camera.bounds_top = 124.5
                state.camera.bounds_bottom = 120.5 - state.height * 8.0
            end
            return true
        end))
        add_hook(t, state.level_gen.themes[t]:set_pre_spawn_level(function(theme)
            theme:spawn_border()
            return true
        end))
        add_hook(t, state.level_gen.themes[t]:set_pre_spawn_players(function()
            local ax, ay, bx, by = get_bounds()
            state.level_gen.spawn_x, state.level_gen.spawn_y = math.floor(state.width*10/2+2), by+0.5
            if state.theme == THEME.OLMEC then
                if state.width == 1 then state.level_gen.spawn_x = 4 end
                spawn(ENT_TYPE.ACTIVEFLOOR_OLMEC, state.level_gen.spawn_x+5, by+2, LAYER.FRONT, 0, 0)
            end
            spawn(ENT_TYPE.LOGICAL_PLATFORM_SPAWNER, state.level_gen.spawn_x, state.level_gen.spawn_y-1, LAYER.FRONT, 0, 0)
        end))
    end
end

set_callback(init_hooks, ON.LOAD)
set_callback(init_hooks, ON.SCRIPT_ENABLE)
set_callback(clear_hooks, ON.SCRIPT_DISABLE)
)");
    add_ui_script("level_size", false, "");
}

void imgui_draw()
{
    auto base = ImGui::GetMainViewport();
    ImGuiContext& g = *GImGui;

    if (get_setting(GAME_SETTING::WINDOW_MODE) == 0u)
        ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable;
    else if (options["multi_viewports"])
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    for (int i = 1; i < g.PlatformIO.Viewports.Size; i++)
    {
        ImGuiViewport* viewport = g.PlatformIO.Viewports[i];
        viewport->Flags |= ImGuiViewportFlags_NoFocusOnClick | ImGuiViewportFlags_NoFocusOnAppearing | ImGuiViewportFlags_OwnedByApp;
    }

    render_clickhandler();
    if (!hide_ui && options["draw_hotbar"])
        render_hotbar();
    if (options["draw_hud"])
        render_prohud();
    if (options["draw_script_messages"] && !g_Console->is_toggled())
        render_messages();

    focused_tool = "";

    float toolwidth = 0.12f * ImGui::GetIO().DisplaySize.x * ImGui::GetIO().FontGlobalScale;
    if (!hide_ui)
    {
        ImGui::SetNextWindowPos(base->Pos, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize({600, base->Size.y / 2}, ImGuiCond_FirstUseEver);
        if (options["menu_ui"])
        {
            in_menu = true;
            ImGui::PushID("MainMenu");
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {0, 0});
            if (ImGui::BeginMainMenuBar())
            {
                ImGui::PopStyleVar();
                for (size_t i = 0; i < tab_order_main.size(); ++i)
                {
                    const auto& tab = tab_order_main[i];
                    if (windows[tab]->detached)
                        continue;
                    ImGui::SetNextWindowSizeConstraints({300.0f, 100.0f}, {500.0f, base->Size.y - 50.0f});
                    if (tab == "tool_entity")
                        ImGui::SetNextWindowSizeConstraints({400.0f, 100.0f}, {400.0f, base->Size.y - 50.0f});
                    if (windows[tab]->popup)
                    {
                        ImGui::OpenPopup(windows[tab]->name.c_str());
                        windows[tab]->popup = false;
                    }
                    if (ImGui::BeginMenu(windows[tab]->name.c_str(), true))
                    {
                        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {2, 2});
                        ImGui::GetIO().WantCaptureKeyboard = true;
                        render_tool(tab);
                        ImGui::PopStyleVar();
                        ImGui::EndMenu();
                    }
                    if (ImGui::GetIO().MouseClicked[1] && mouse_pos().y < ImGui::GetTextLineHeight() && ImGui::IsItemHovered())
                        detach(tab);
                }
                if (ImGui::BeginMenu("Assets"))
                {
                    for (size_t i = 0; i < tab_order_extra.size(); ++i)
                    {
                        const auto& tab = tab_order_extra[i];
                        if (ImGui::MenuItem(windows[tab]->name.c_str()))
                            detach(tab);
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
            }
            ImGui::PopID();
            in_menu = false;
        }
        else
        {
            ImGui::Begin("Overlunky", NULL, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiViewportFlags_NoTaskBarIcon | ImGuiViewportFlags_NoDecoration);
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("Tools"))
                {
                    for (size_t i = 0; i < tab_order.size() - 5; ++i)
                    {
                        const auto& tab = tab_order[i];
                        if (ImGui::MenuItem(windows[tab]->name.c_str(), key_string(keys[tab]).c_str()))
                        {
                            toggle(tab);
                        }
                    }
                    for (size_t i = 0; i < tab_order_extra.size(); ++i)
                    {
                        const auto& tab = tab_order_extra[i];
                        if (ImGui::MenuItem(windows[tab]->name.c_str(), key_string(keys[tab]).c_str()))
                        {
                            toggle(tab);
                        }
                    }
                    ImGui::Separator();
                    if (ImGui::MenuItem("Detach tab", key_string(keys["detach_tab"]).c_str()))
                    {
                        detach(active_tab);
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Settings"))
                {
                    if (ImGui::MenuItem("Switch to menu UI", key_string(keys["switch_ui"]).c_str()))
                        options["menu_ui"] = true;
                    ImGui::Separator();
                    for (size_t i = tab_order.size() - 4; i < tab_order.size(); ++i)
                    {
                        const auto& tab = tab_order[i];
                        if (ImGui::MenuItem(windows[tab]->name.c_str(), key_string(keys[tab]).c_str()))
                        {
                            toggle(tab);
                        }
                    }
                    ImGui::Separator();
                    if (ImGui::MenuItem("Save options", key_string(keys["save_settings"]).c_str()))
                    {
                        ImGui::SaveIniSettingsToDisk(inifile);
                        save_config(cfgfile);
                    }
                    if (ImGui::MenuItem("Load options", key_string(keys["load_settings"]).c_str()))
                    {
                        ImGui::LoadIniSettingsFromDisk(inifile);
                        load_config(cfgfile);
                        refresh_script_files();
                        set_colors();
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Help"))
                {
                    if (ImGui::MenuItem("Check for updates"))
                        version_check(true);
                    if (ImGui::MenuItem("Get latest version here"))
                        ShellExecuteA(NULL, "open", "https://github.com/spelunky-fyi/overlunky/releases/tag/whip", NULL, NULL, SW_SHOWNORMAL);
                    if (ImGui::MenuItem("README"))
                        ShellExecuteA(NULL, "open", "https://github.com/spelunky-fyi/overlunky#overlunky", NULL, NULL, SW_SHOWNORMAL);
                    if (ImGui::MenuItem("API Documentation"))
                        ShellExecuteA(NULL, "open", "https://spelunky-fyi.github.io/overlunky/", NULL, NULL, SW_SHOWNORMAL);
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            if (ImGui::BeginTabBar("##TabBar"))
            {
                ImGuiTabItemFlags flags = 0;
                for (const auto& tab : tab_order)
                {
                    flags = 0;
                    if (activate_tab == tab)
                    {
                        flags = ImGuiTabItemFlags_SetSelected;
                        activate_tab = "";
                        active_tab = "";
                    }
                    if (!detached(tab) && ImGui::BeginTabItem(windows[tab]->name.c_str(), &windows[tab]->open, flags))
                    {
                        if (ImGui::BeginDragDropSource())
                        {
                            ImGui::SetDragDropPayload("TAB", NULL, 0);
                            ImGui::Text("Drag outside main window\nto detach %s", windows[tab]->name.c_str());
                            ImGui::EndDragDropSource();
                        }
                        ImGui::BeginChild("ScrollableTool");
                        render_tool(tab);
                        ImGui::EndChild();
                        ImGui::EndTabItem();
                    }
                }
                ImGui::EndTabBar();
            }
            int tabnum = 0;
            for (auto& window : windows)
            {
                if (window.second->open && !window.second->detached)
                    ++tabnum;
            }
            if (tabnum == 0)
            {
                ImGui::TextWrapped("Looks like you closed all your tabs. Good thing we have a menubar now!");
                if (ImGui::Button("Restore all tabs"))
                {
                    for (auto& window : windows)
                    {
                        window.second->open = true;
                    }
                }
            }
            ImGui::End();
        }
        for (const auto& tab : windows)
        {
            if (!tab.second->detached)
                continue;
            ImGui::SetNextWindowSize({toolwidth, toolwidth}, ImGuiCond_Once);
            int flags = ImGuiViewportFlags_NoTaskBarIcon;
            if (tab.first == "tool_texture")
            {
                ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
                flags |= ImGuiWindowFlags_NoBackground;
            }
            ImGui::Begin(tab.second->name.c_str(), &tab.second->detached, flags);
            ImGui::PushID(tab.second->name.c_str());
            if (tab.first != "tool_texture")
                ImGui::BeginChild("ScrollableTool");
            render_tool(tab.first);
            if (tab.first != "tool_texture")
                ImGui::EndChild();
            ImGui::PopID();
            ImGui::End();
        }
        if (detach_tab != "")
        {
            auto win = ImGui::FindWindowByName(windows[detach_tab]->name.c_str());
            if (win)
            {
                auto pos = ImGui::GetIO().MousePos;
                win->Pos.x = pos.x - 32.0f;
                win->Pos.y = pos.y - 8.0f;
                if (win->Pos.x < 0.0f)
                    win->Pos.x = 0.0f;
                if (win->Pos.y < 0.0f)
                    win->Pos.y = 0.0f;
            }
            detach_tab = "";
        }
    }

    if (show_app_metrics)
    {
        ImGui::ShowMetricsWindow(&show_app_metrics);
        ImGui::Begin("Styles");
        ImGui::ShowStyleEditor();
        ImGui::End();
    }

    for (auto it = entity_windows.begin(); it != entity_windows.end();)
    {
        ImGui::PushID(it->second->uid);
        ImGui::Begin(it->second->name.c_str(), &it->second->open);
        render_entity_props(it->second->uid, true);
        ImGui::End();
        ImGui::PopID();
        if (!it->second->open)
            entity_windows.erase(it++);
        else
            ++it;
    }

    if (options["hd_cursor"])
        render_cursor();
    else
        g.Style.MouseCursorScale = 1.0f;
}

void check_focus()
{
    if (last_focus != UI::get_focus())
    {
        if (UI::get_focus())
            last_focus_time = std::chrono::system_clock::now();
        last_focus = UI::get_focus();
    }
}

void update_bucket()
{
    if (g_bucket->overlunky->set_selected_uid.has_value())
    {
        g_last_id = g_bucket->overlunky->set_selected_uid.value();
        g_entity = get_entity_ptr(g_last_id);
        g_bucket->overlunky->set_selected_uid = std::nullopt;
    }
    if (g_bucket->overlunky->set_selected_uids.has_value())
    {
        g_selected_ids = g_bucket->overlunky->set_selected_uids.value();
        g_bucket->overlunky->set_selected_uids = std::nullopt;
    }
    g_bucket->overlunky->selected_uid = g_last_id;
    g_bucket->overlunky->selected_uids = g_selected_ids;
    g_bucket->overlunky->keys = keys;

    for (const auto& [k, v] : g_bucket->overlunky->set_options)
    {
        if (!legal_options.contains(k))
            continue;

        if (options.contains(k))
            if (auto* val = std::get_if<bool>(&v))
                options[k] = *val;

        if (k == "disable_ghost_timer")
        {
            UI::set_time_ghost_enabled(!options["disable_ghost_timer"]);
            UI::set_time_jelly_enabled(!options["disable_ghost_timer"]);
            UI::set_cursepot_ghost_enabled(!options["disable_ghost_timer"]);
        }
        else if (k == "god_mode")
        {
            UI::godmode(options["god_mode"]);
        }
        else if (k == "god_mode_companions")
        {
            UI::godmode_companions(options["god_mode_companions"]);
        }
        else if (k == "noclip")
        {
            toggle_noclip();
        }
        else if (k == "lights")
        {
            toggle_lights();
        }
        else if (k == "skip_fades") // Deprecated
        {
            if (auto* val = std::get_if<bool>(&v))
            {
                options["skip_fades"] = *val;
                g_bucket->pause_api->skip_fade = *val;
            }
        }
        else if (k == "pause_type") // Deprecated
        {
            if (auto* val = std::get_if<int64_t>(&v))
                g_bucket->pause_api->pause_type = (PAUSE_TYPE)*val;
        }
        else if (k == "paused") // Deprecated
        {
            if (auto* val = std::get_if<bool>(&v))
                g_bucket->pause_api->set_paused(*val);
        }
        else if (k == "skip") // Deprecated
        {
            frame_advance();
        }
        else if (k == "camera_hack")
        {
            if (auto* val = std::get_if<int64_t>(&v))
            {
                options["camera_hack"] = (uint32_t)*val;
                set_camera_hack(options["camera_hack"]);
            }
        }
    }
    g_bucket->overlunky->set_options.clear();

    for (auto& [k, v] : options)
    {
        g_bucket->overlunky->options[k] = options[k];
    }
    g_bucket->overlunky->options["pause_type"] = (int64_t)g_bucket->pause_api->pause_type; // Deprecated
    g_bucket->overlunky->options["paused"] = g_bucket->pause_api->paused();                // Deprecated
}

void post_draw()
{
    check_focus();
    update_players();
    force_kits();
    force_zoom();
    force_hud_flags();
    force_time();
    force_cheats();
    force_lights();
    update_bucket();
}

void create_box(std::vector<EntityItem> items)
{
    std::vector<EntityItem> new_items(items);
    new_items.emplace(new_items.begin(), "ENT_TYPE_Use entity picker or select entity to spawn:", 0);

    std::sort(new_items.begin(), new_items.end());

    std::vector<int> new_filtered_items(new_items.size());
    for (unsigned int i = 0; i < new_items.size(); i++)
    {
        new_filtered_items[i] = i;
        entity_names[new_items[i].id] = new_items[i].name.substr(9);
        entity_full_names[new_items[i].id] = new_items[i].name;
    }

    // TODO: add atomic and wrap it as struct
    {
        g_current_item = 0;
        g_items = new_items;
        g_filtered_items = new_filtered_items;
        g_filtered_count = static_cast<int>(g_items.size());
    }
}

std::string make_save_path(std::string_view script_path, std::string_view script_name)
{
    std::string save_path{script_path};
    save_path += "/save_";
    save_path += script_name;
    save_path += ".dat";
    return save_path;
}

void init_ui(ImGuiContext* ctx)
{
    g_SoundManager = std::make_unique<SoundManager>(&LoadAudioFile);

    API::init(g_SoundManager.get());
    API::post_init();

    g_state = HeapBase::get_main().state();
    g_save = UI::savedata();
    g_game_manager = get_game_manager();
    g_bucket = Bucket::get();
    g_bucket->overlunky = new Overlunky();

    g_Console = std::make_unique<SpelunkyConsole>(g_SoundManager.get());
    g_Console->set_max_history_size(1000);
    g_Console->load_history("console_history.txt");

    register_on_input(&process_keys);
    imgui_pre_init(ctx);
    register_imgui_init(&imgui_init);
    register_imgui_draw(&imgui_draw);
    register_post_draw(&post_draw);

    register_make_save_path(&make_save_path);

    register_on_load_file(&load_file_as_dds_if_image);

    auto& render_api = RenderAPI::get();
    render_api.set_advanced_hud();

    const std::string version_string = fmt::format("Overlunky {}", get_version());
    const float scale{0.0004f};

    static TextRenderingInfo tri{};
    tri.set_text(version_string, 0, 0, scale, scale, 1, 0);

    const auto [w, h] = tri.text_size();
    tri.y = -1.0f + std::abs(h) / 2.0f;

    render_api.set_post_render_game(
        []()
        {
            auto& render_api_l = RenderAPI::get();
            static const float color[4]{1.0f, 1.0f, 1.0f, 0.3f};
            render_vanilla_stuff();
            render_version_warning();
            render_api_l.draw_text(&tri, color);
        });
}

void reload_enabled_scripts()
{
    for (auto& [script_name, script] : g_scripts)
    {
        if (script->is_enabled())
        {
            load_script(script->get_file(), true);
        }
    }
}
