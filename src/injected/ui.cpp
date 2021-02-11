#define NOMINMAX

#include "ui.hpp"

#include <ShlObj.h>
#include <Shlwapi.h>
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>
#include <d3d11.h>
#include <imgui.h>
#include <imgui_internal.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <codecvt>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <locale>
#include <map>
#include <string>
#include <toml.hpp>

#include "entity.hpp"
#include "logger.h"
#include "rpc.hpp"
#include "script.hpp"
#include "state.hpp"

std::vector<Script *> g_scripts;
std::vector<std::filesystem::path> g_script_files;
std::vector<std::string> g_script_autorun;

const USHORT HID_MOUSE = 2;
const USHORT HID_KEYBOARD = 6;

bool HID_RegisterDevice(HWND hTarget, USHORT usage)
{
    RAWINPUTDEVICE hid;
    hid.usUsagePage = 1;
    hid.usUsage = usage;
    hid.dwFlags = RIDEV_DEVNOTIFY | RIDEV_INPUTSINK;
    hid.hwndTarget = hTarget;

    return !!RegisterRawInputDevices(&hid, 1, sizeof(RAWINPUTDEVICE));
}

void HID_UnregisterDevice(USHORT usage)
{
    RAWINPUTDEVICE hid;
    hid.usUsagePage = 1;
    hid.usUsage = usage;
    hid.dwFlags = RIDEV_REMOVE;
    hid.hwndTarget = NULL;

    RegisterRawInputDevices(&hid, 1, sizeof(RAWINPUTDEVICE));
}

std::map<std::string, int> keys{
    {"enter", 0x0d},
    {"escape", 0x1b},
    {"move_left", 0x25},
    {"move_up", 0x26},
    {"move_right", 0x27},
    {"move_down", 0x28},
    {"move_pageup", 0x21},
    {"move_pagedown", 0x22},
    {"toggle_mouse", 0x14d},
    {"toggle_godmode", 0x147},
    {"toggle_noclip", 0x146},
    {"toggle_snap", 0x153},
    {"toggle_pause", 0x150},
    {"toggle_disable_input", 0x14b},
    {"toggle_disable_input_alt", 0x34b},
    {"toggle_disable_pause", 0x350},
    {"toggle_grid", 0x347},
    {"toggle_hitboxes", 0x348},
    {"tool_entity", 0x70},
    {"tool_door", 0x71},
    {"tool_camera", 0x72},
    {"tool_entity_properties", 0x73},
    {"tool_game_properties", 0x74},
    {"tool_options", 0x78},
    {"tool_debug", 0x37b},
    {"tool_metrics", 0x349},
    {"tool_style", 0x355},
    {"tool_script", 0x77},
    {"reset_windows", 0x352},
    {"reset_windows_vertical", 0x356},
    {"tabbed_interface", 0x354},
    {"detach_tab", 0x344},
    {"save_settings", 0x353},
    {"load_settings", 0x34c},
    {"spawn_entity", 0x10d},
    {"spawn_layer_door", 0x20d},
    {"spawn_warp_door", 0x30d},
    {"hide_ui", 0x7a},
    {"zoom_in", 0x1bc},
    {"zoom_out", 0x1be},
    {"zoom_default", 0x132},
    {"zoom_3x", 0x133},
    {"zoom_4x", 0x134},
    {"zoom_5x", 0x135},
    {"zoom_auto", 0x130},
    {"teleport", 0x320},
    {"teleport_left", 0x325},
    {"teleport_up", 0x326},
    {"teleport_right", 0x327},
    {"teleport_down", 0x328},
    {"coordinate_left", 0x125},
    {"coordinate_up", 0x126},
    {"coordinate_right", 0x127},
    {"coordinate_down", 0x128},
    {"mouse_spawn", 0x401},
    {"mouse_spawn_throw", 0x401},
    {"mouse_teleport", 0x402},
    {"mouse_teleport_throw", 0x402},
    {"mouse_grab", 0x403},
    {"mouse_grab_unsafe", 0x603},
    {"mouse_grab_throw", 0x503},
    {"mouse_zap", 0x404},
    {"mouse_blast", 0x504},
    {"mouse_boom", 0x0},
    {"mouse_big_boom", 0x604},
    {"mouse_nuke", 0x704},
    {"mouse_clone", 0x505},
    {"mouse_destroy", 0x405},
    {"mouse_destroy_unsafe", 0x605},
    //{ "", 0x },
};

struct Window{
    std::string name;
    bool detached;
    bool open;
};
std::map<std::string, Window*> windows;

IDXGISwapChain *pSwapChain;
ID3D11Device *pDevice;
ID3D11DeviceContext *pContext;
ID3D11RenderTargetView *mainRenderTargetView;
HWND window;
HWND gamewindow;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
WNDPROC orig_wndproc = nullptr;

using PresentPtr = HRESULT(STDMETHODCALLTYPE *)(IDXGISwapChain *pSwapChain, UINT SyncInterval, UINT Flags);
PresentPtr oPresent;

// Global state
struct EntityCache
{
    Movable *entity;
    int type;
};

static ImFont *font, *bigfont;

float g_x = 0, g_y = 0, g_vx = 0, g_vy = 0, g_zoom = 13.5, g_hue = 0.63, g_sat = 0.66, g_val = 0.66;
ImVec2 startpos;
int g_held_id = 0, g_last_id = 0, g_current_item = 0, g_filtered_count = 0, g_level = 1, g_world = 1, g_to = 0, g_last_frame = 0, g_last_gun = 0,
    g_entity_type = 0, g_last_time = -1, g_level_time = -1, g_total_time = -1, g_pause_time = -1, g_level_width = 0, g_level_height = 0,
    g_force_width = 0, g_force_height = 0, register_keys = 0, register_keys_alt = 0;
uint32_t g_held_flags = 0;
uintptr_t g_entity_addr = 0, g_state_addr = 0;
std::vector<EntityItem> g_items;
std::vector<int> g_filtered_items;
std::vector<std::string> saved_entities;
std::vector<Player *> g_players;
bool set_focus_entity = false, set_focus_world = false, set_focus_zoom = false, scroll_to_entity = false, scroll_top = false, click_teleport = false,
     file_written = false, show_debug = false, throw_held = false, paused = false, capture_last = false, capture_last_alt = false,
     show_app_metrics = false, hud_dark_level = false, lock_entity = false, lock_player = false, freeze_last = false, freeze_level = false,
     freeze_total = false, hide_ui = false, change_colors = false, dark_mode = false, enable_noclip = false;
Movable *g_entity = 0;
Movable *g_held_entity = 0;
Inventory *g_inventory = 0;
StateMemory *g_state = 0;
std::map<int, std::string> entity_names;
std::map<int, EntityCache> entity_cache;
int cache_player = 0;
std::string active_tab = "", activate_tab = "";
std::vector<std::string> tab_order = {
    "tool_entity",
    "tool_door",
    "tool_camera",
    "tool_entity_properties",
    "tool_game_properties",
    "tool_script",
    "tool_options",
    "tool_style",
    "tool_debug"};

static char text[500];
const char *themes[] = {
    "1: Dwelling",
    "2: Jungle",
    "2: Volcana",
    "3: Olmec",
    "4: Tide Pool",
    "4: Temple",
    "5: Ice Caves",
    "6: Neo Babylon",
    "7: Sunken City",
    "8: Cosmic Ocean",
    "4: City of Gold",
    "4: Duat",
    "4: Abzu",
    "6: Tiamat",
    "7: Eggplant World",
    "7: Hundun",
    "0: Base camp"};
const char *themes_short[] = {
    "Dwelling",
    "Jungle",
    "Volcana",
    "Olmec",
    "Tide Pool",
    "Temple",
    "Ice Caves",
    "Neo Babylon",
    "Sunken City",
    "Cosmic Ocean",
    "City of Gold",
    "Duat",
    "Abzu",
    "Tiamat",
    "Eggplant World",
    "Hundun",
    "Base camp"};
const char *entity_flags[] = {
    "1: Invisible",
    "2: ",
    "3: Solid (wall)",
    "4: Passes through objects",
    "5: Passes through everything",
    "6: Take no damage",
    "7: Throwable/Knockbackable",
    "8: ",
    "9: ",
    "10: No gravity",
    "11: ",
    "12: Stunnable",
    "13: Collides walls",
    "14: ",
    "15: Can be stomped",
    "16: ",
    "17: Facing left",
    "18: Pickupable",
    "19: ",
    "20: Enable button prompt",
    "21: ",
    "22: Locked",
    "23: ",
    "24: ",
    "25: Passes through player",
    "26: ",
    "27: ",
    "28: Pause AI and physics",
    "29: Dead",
    "30: ",
    "31: ",
    "32: "};
const char *more_flags[] = {
    "1: ",
    "2: ",
    "3: ",
    "4: ",
    "5: ",
    "6: ",
    "7: ",
    "8: ",
    "9: ",
    "10: ",
    "11: Swimming",
    "12: ",
    "13: ",
    "14: Falling",
    "15: Cursed effect",
    "16: Disable input",
    "17: ",
    "18: ",
    "19: ",
    "20: ",
    "21: ",
    "22: ",
    "23: ",
    "24: ",
    "25: ",
    "26: ",
    "27: ",
    "28: ",
    "29: ",
    "30: ",
    "31: ",
    "32: "};
const char *hud_flags[] = {
    "1: ",
    "2: ",
    "3: ",
    "4: ",
    "5: ",
    "6: ",
    "7: ",
    "8: ",
    "9: ",
    "10: Angry shopkeeper",
    "11: Angry Tun",
    "12: ",
    "13: ",
    "14: Angry Yang",
    "15: Angry Tusk",
    "16: Angry Waddler",
    "17: ",
    "18: ",
    "19: Dark level",
    "20: Allow pause",
    "21: Hide hud, transition",
    "22: Hide hud, ?",
    "23: Have clover",
    "24: ",
    "25: ",
    "26: ",
    "27: ",
    "28: ",
    "29: ",
    "30: ",
    "31: ",
    "32: "};
const char *journal_flags[] = {
    "1: I was a pacifist",
    "2: I was a vegan",
    "3: I was a vegetarian",
    "4: I was a petty criminal",
    "5: I was a wanted criminal",
    "6: I was a crime lord",
    "7: I was a king",
    "8: I was a queen",
    "9: I was a fool",
    "10: I was an eggplant",
    "11: I didn't care for treasure",
    "12: I liked pets",
    "13: I loved pets",
    "14: I took damage",
    "15: I survived death once",
    "16: I slayed Kingu",
    "17: I slayed Osiris",
    "18: I defeated Tiamat",
    "19: I defeated Hundun",
    "20: I became one with the Cosmos",
    "21: I eventually died",
    "22: ",
    "23: ",
    "24: ",
    "25: ",
    "26: ",
    "27: ",
    "28: ",
    "29: ",
    "30: ",
    "31: ",
    "32: "};
/*const char *empty_flags[] = {
    "1: ",  "2: ",  "3: ",  "4: ",  "5: ",  "6: ",  "7: ",  "8: ",  "9: ",  "10: ", "11: ", "12: ", "13: ", "14: ", "15: ", "16: ",
    "17: ", "18: ", "19: ", "20: ", "21: ", "22: ", "23: ", "24: ", "25: ", "26: ", "27: ", "28: ", "29: ", "30: ", "31: ", "32: "};*/

const char *button_flags[] = {"Jp", "Wp", "Bm", "Rp", "Rn", "Dr"};
const char *direction_flags[] = {"Left", "Down", "Up", "Right"};

const char *inifile = "imgui.ini";
const std::string cfgfile = "overlunky.ini";
std::string scriptpath = "Overlunky/Scripts";

const char s8_zero = 0, s8_one = 1, s8_min = -128, s8_max = 127;
const ImU8 u8_zero = 0, u8_one = 1, u8_min = 0, u8_max = 255, u8_four = 4, u8_seven = 7, u8_seventeen = 17;
const short s16_zero = 0, s16_one = 1, s16_min = -32768, s16_max = 32767;
const ImU16 u16_zero = 0, u16_one = 1, u16_min = 0, u16_max = 65535;
const ImS32 s32_zero = 0, s32_one = 1, s32_min = INT_MIN / 2, s32_max = INT_MAX / 2, s32_hi_a = INT_MAX / 2 - 100, s32_hi_b = INT_MAX / 2;
const ImU32 u32_zero = 0, u32_one = 1, u32_min = 0, u32_max = UINT_MAX / 2, u32_hi_a = UINT_MAX / 2 - 100, u32_hi_b = UINT_MAX / 2;
const ImS64 s64_zero = 0, s64_one = 1, s64_min = LLONG_MIN / 2, s64_max = LLONG_MAX / 2, s64_hi_a = LLONG_MAX / 2 - 100, s64_hi_b = LLONG_MAX / 2;
const ImU64 u64_zero = 0, u64_one = 1, u64_thousand = 1000, u64_charmin = 194, u64_charmax = 216, u64_min = 0, u64_max = ULLONG_MAX / 2,
            u64_hi_a = ULLONG_MAX / 2 - 100, u64_hi_b = ULLONG_MAX / 2;
const float f32_zero = 0.f, f32_one = 1.f, f32_lo_a = -10000000000.0f, f32_hi_a = +10000000000.0f;
const double f64_zero = 0., f64_one = 1., f64_lo_a = -1000000000000000.0, f64_hi_a = +1000000000000000.0;

std::map<std::string, bool> options = {
    {"mouse_control", true},
    {"god_mode", false},
    {"noclip", false},
    {"snap_to_grid", false},
    {"stack_horizontally", false},
    {"stack_vertically", false},
    {"disable_pause", false},
    {"disable_input", true},
    {"disable_input_alt", false},
    {"draw_grid", false},
    {"draw_hitboxes", false},
    {"tabbed_interface", true}};

ImVec4 hue_shift(ImVec4 in, float hue)
{
    float U = cos(hue * 3.14159265 / 180);
    float W = sin(hue * 3.14159265 / 180);
    ImVec4 out = ImVec4(
        (.299 + .701 * U + .168 * W) * in.x + (.587 - .587 * U + .330 * W) * in.y + (.114 - .114 * U - .497 * W) * in.z,
        (.299 - .299 * U - .328 * W) * in.x + (.587 + .413 * U + .035 * W) * in.y + (.114 - .114 * U + .292 * W) * in.z,
        (.299 - .3 * U + 1.25 * W) * in.x + (.587 - .588 * U - 1.05 * W) * in.y + (.114 + .886 * U - .203 * W) * in.z,
        in.w); //((float)rand() / RAND_MAX) * 0.5 + 0.5);
    return out;
}

void set_colors()
{
    float col_main_sat = g_sat;
    float col_main_val = g_val;
    float col_area_sat = g_sat * 0.77;
    float col_area_val = g_val * 0.60;
    float col_back_sat = g_sat * 0.33;
    float col_back_val = g_val * 0.20;

    ImGuiStyle &style = ImGui::GetStyle();

    ImVec4 col_text = ImColor::HSV(g_hue, 15.f / 255.f, 245.f / 255.f);
    ImVec4 col_main = ImColor::HSV(g_hue, col_main_sat, col_main_val);
    ImVec4 col_back = ImColor::HSV(g_hue, col_back_sat, col_back_val);
    ImVec4 col_area = ImColor::HSV(g_hue, col_area_sat, col_area_val);

    style.Colors[ImGuiCol_Text] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(col_text.x, col_text.y, col_text.z, 0.58f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(col_back.x, col_back.y, col_back.z, 1.00f);
    style.Colors[ImGuiCol_Border] = ImVec4(col_text.x, col_text.y, col_text.z, 0.30f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.68f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(col_back.x, col_back.y, col_back.z, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(col_back.x, col_back.y, col_back.z, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(col_area.x, col_area.y, col_area.z, 0.57f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(col_main.x, col_main.y, col_main.z, 0.31f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(col_main.x, col_main.y, col_main.z, 0.80f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(col_main.x, col_main.y, col_main.z, 0.24f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(col_main.x, col_main.y, col_main.z, 0.44f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    style.Colors[ImGuiCol_Header] = ImVec4(col_main.x, col_main.y, col_main.z, 0.76f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(col_main.x, col_main.y, col_main.z, 0.20f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.43f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0, 0, 0, 0);
    style.Colors[ImGuiCol_Separator] = ImVec4(col_main.x, col_main.y, col_main.z, 0.50f);
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    style.Colors[ImGuiCol_Tab] = ImVec4(col_back.x, col_back.y, col_back.z, 0.86f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(col_main.x, col_main.y, col_main.z, 0.80f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(col_area.x, col_area.y, col_area.z, 0.60f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(col_area.x, col_area.y, col_area.z, 0.80f);
    style.WindowPadding = ImVec2(4, 4);
    style.WindowRounding = 0;
    style.FrameRounding = 0;
    style.PopupRounding = 0;
    style.GrabRounding = 0;
    style.TabRounding = 0;
    style.WindowBorderSize = 0;
    style.FrameBorderSize = 0;
    style.PopupBorderSize = 0;
}

void load_script(std::string file)
{
    std::ifstream data(file);
    std::ostringstream buf;
    if (!data.fail())
    {
        buf << data.rdbuf();
        size_t slash = file.find_last_of("/\\");
        if (slash != std::string::npos)
            file = file.substr(slash + 1);
        Script *script = new Script(buf.str(), file);
        g_scripts.push_back(script);
        data.close();
    }
}

bool process_keys(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam);

bool process_resizing(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam);

std::string key_string(int keycode)
{
    UCHAR virtualKey = keycode & 0xff;
    CHAR szName[128];
    int result = 0;
    std::string name;
    if (keycode & 0xff == 0)
    {
        name = "Disabled";
    }
    else if (!(keycode & 0x400)) // keyboard
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
        default:
            result = GetKeyNameTextA(scanCode << 16, szName, 128);
        }
        if (result == 0)
        {
            name = "Mystery key";
        }
        std::string keyname(szName);
        name = keyname;
    }
    else // mouse
    {
        std::stringstream buttonss;
        buttonss << "Mouse" << (keycode & 0xff);
        name = buttonss.str();
    }

    if (keycode & 0x200)
    {
        name = "Shift+" + name;
    }
    if (keycode & 0x100)
    {
        name = "Ctrl+" + name;
    }
    return name;
}

bool InputString(const char *label, std::string *str, ImGuiInputTextFlags flags = 0)
{
    return ImGui::InputText(label, (char *)str->c_str(), 9, flags);
}

bool InputStringMultiline(const char *label, std::string *str, const ImVec2 &size, ImGuiInputTextFlags flags = 0)
{
    return ImGui::InputTextMultiline(label, (char *)str->c_str(), str->capacity() + 1, size, flags);
}

void refresh_script_files()
{
    g_script_files.clear();
    if (std::filesystem::exists(scriptpath) && std::filesystem::is_directory(scriptpath))
    {
        for (const auto &file : std::filesystem::directory_iterator(scriptpath))
        {
            g_script_files.push_back(file.path());
        }
    }
}

void autorun_scripts()
{
    for (auto file : g_script_autorun)
    {
        std::string script = scriptpath + "/" + file;
        if (std::filesystem::exists(script) && std::filesystem::is_regular_file(script))
        {
            load_script(script);
        }
    }
}

void save_config(std::string file)
{
    std::ofstream writeData(file);
    writeData << "# Overlunky hotkeys" << std::endl
              << "# Syntax:" << std::endl
              << "# function = keycode_in_hex" << std::endl
              << "# For modifiers, add 0x100 for Ctrl or 0x200 for Shift" << std::endl
              << "# For mouse buttons, add 0x400" << std::endl
              << "# Set to 0x0 to disable key" << std::endl
              << "# Example: G is 0x47, so Ctrl+G is 0x147, 0x402 is Mouse2 etc" << std::endl
              << "# Get more hex keycodes from https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes" << std::endl
              << "# If you mess this file up, you can just delete it and run overlunky to get the defaults back" << std::endl
              << "[hotkeys]" << std::endl;
    for (const auto &kv : keys)
    {
        writeData << std::left << std::setw(24) << kv.first << " = " << std::hex << "0x" << std::setw(8) << kv.second << "# "
                  << key_string(keys[kv.first]) << std::endl;
    }

    writeData << "\n[options] # 0 or 1 unless stated otherwise (default state "
                 "of options)\n";
    for (const auto &kv : options)
    {
        writeData << kv.first << " = " << std::dec << kv.second << std::endl;
    }

    ImGuiStyle &style = ImGui::GetStyle();
    writeData << "hue = " << std::fixed << std::setprecision(2) << g_hue << " # float, 0.0 - 1.0" << std::endl;
    writeData << "saturation = " << std::fixed << std::setprecision(2) << g_sat << " # float, 0.0 - 1.0" << std::endl;
    writeData << "lightness = " << std::fixed << std::setprecision(2) << g_val << " # float, 0.0 - 1.0" << std::endl;
    writeData << "alpha = " << std::fixed << std::setprecision(2) << style.Alpha << " # float, 0.0 - 1.0" << std::endl;
    writeData << "scale = " << std::fixed << std::setprecision(2) << ImGui::GetIO().FontGlobalScale << " # float, 0.3 - 2.0" << std::endl;

    writeData << "kits = [";
    for (int i = 0; i < saved_entities.size(); i++)
    {
        writeData << std::endl << "  \"" << saved_entities[i] << "\"";
        if (i < saved_entities.size() - 1)
            writeData << ",";
    }
    if (!saved_entities.empty())
        writeData << std::endl;
    writeData << "]" << std::endl;

    writeData << "# Script filenames to load automatically on start. Example: autorun_scripts = [\"foo.lua\", \"bar.lua\"]" << std::endl;
    writeData << "autorun_scripts = [";
    for (int i = 0; i < g_script_autorun.size(); i++)
    {
        writeData << std::endl << "  \"" << g_script_autorun[i] << "\"";
        if (i < g_script_autorun.size() - 1)
            writeData << ",";
    }
    if (!g_script_autorun.empty())
        writeData << std::endl;
    writeData << "]" << std::endl;

    writeData << "# Directory where the scripts are loaded from. You can use relative or absolute path." << std::endl
              << "# A relative path is relative to the game directory. Use forward / slashes." << std::endl;
    writeData << "script_dir = \"" << scriptpath << "\"" << std::endl;

    writeData.close();
}

void load_config(std::string file)
{
    toml::value data;
    try
    {
        data = toml::parse(cfgfile);
    }
    catch (std::exception &)
    {
        save_config(file);
        return;
    }

    toml::value hotkeys;
    try
    {
        hotkeys = toml::find(data, "hotkeys");
    }
    catch (std::exception &)
    {
        save_config(file);
        return;
    }
    for (const auto &kv : keys)
    {
        keys[kv.first] = toml::find_or<toml::integer>(hotkeys, kv.first, kv.second);
    }

    toml::value opts;
    try
    {
        opts = toml::find(data, "options");
    }
    catch (std::exception &)
    {
        save_config(file);
        return;
    }
    for (const auto &kv : options)
    {
        options[kv.first] = (bool)toml::find_or<int>(opts, kv.first, (int)kv.second);
    }
    ImGuiStyle &style = ImGui::GetStyle();
    g_hue = toml::find_or<float>(opts, "hue", 0.63);
    g_sat = toml::find_or<float>(opts, "saturation", 0.66);
    g_val = toml::find_or<float>(opts, "lightness", 0.66);
    style.Alpha = toml::find_or<float>(opts, "alpha", 0.66);
    ImGui::GetIO().FontGlobalScale = toml::find_or<float>(opts, "scale", 1.0);
    saved_entities = toml::find_or<std::vector<std::string>>(opts, "kits", {});
    g_script_autorun = toml::find_or<std::vector<std::string>>(opts, "autorun_scripts", {});
    scriptpath = toml::find_or<std::string>(opts, "script_dir", "Overlunky/Scripts");
    godmode(options["god_mode"]);
    save_config(file);
}

HWND FindTopWindow(DWORD pid)
{
    std::pair<HWND, DWORD> params = {0, pid};

    // Enumerate the windows using a lambda to process each window
    BOOL bResult = EnumWindows(
        [](HWND hwnd, LPARAM lParam) -> BOOL {
            auto pParams = (std::pair<HWND, DWORD> *)(lParam);

            DWORD processId;
            if (GetWindowThreadProcessId(hwnd, &processId) && processId == pParams->second && hwnd != window)
            {
                // Stop enumerating
                SetLastError(-1);
                pParams->first = hwnd;
                return FALSE;
            }

            // Continue enumerating
            return TRUE;
        },
        (LPARAM)&params);

    if (!bResult && GetLastError() == -1 && params.first)
    {
        return params.first;
    }

    return 0;
}

LRESULT CALLBACK hkWndProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (process_resizing(message, wParam, lParam))
    {
        gamewindow = FindTopWindow(GetCurrentProcessId());
        return CallWindowProc(orig_wndproc, window, message, wParam, lParam);
    }
    if (!process_keys(message, wParam, lParam))
    {
        ImGui_ImplWin32_WndProcHandler(window, message, wParam, lParam);
    }
    if (ImGui::GetIO().WantCaptureKeyboard && message == WM_KEYDOWN)
    {
        return 0;
    }
    else
    {
        return CallWindowProc(orig_wndproc, window, message, wParam, lParam);
    }
}

bool detached(std::string window)
{
    return windows[window]->detached;
}

bool toggle(std::string tool)
{
    if (!options["tabbed_interface"] || detached(tool))
    {
        const char *name = windows[tool]->name.c_str();
        ImGuiContext &g = *GImGui;
        ImGuiWindow *current = g.NavWindow;
        ImGuiWindow *win = ImGui::FindWindowByName(name);
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
    else
    {
        ImGuiWindow *win = ImGui::FindWindowByName("Overlunky");
        win->Collapsed = false;
        for (auto window : windows)
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

bool active(std::string window)
{
    ImGuiContext &g = *GImGui;
    ImGuiWindow *current = g.NavWindow;
    if (!options["tabbed_interface"] || detached(window))
    {
        return current == ImGui::FindWindowByName(windows[window]->name.c_str());
    }
    else
    {
        return current == ImGui::FindWindowByName("Overlunky") && active_tab == window;
    }
}

void detach(std::string window)
{
    windows[window]->detached = true;
}

void attach(std::string window)
{
    windows[window]->detached = false;
}

bool visible(std::string window)
{
    if (!options["tabbed_interface"] || detached(window))
    {
        ImGuiWindow *win = ImGui::FindWindowByName(windows[window]->name.c_str());
        if (win != NULL)
            return !win->Collapsed;
        return false;
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
    std::string search(text);
    saved_entities.push_back(search);
    save_config(cfgfile);
}

int entity_type(int uid)
{
    return (int)get_entity_type(uid);
}

Movable *entity_ptr(int uid)
{
    return (Movable *)get_entity_ptr(uid);
}

bool update_players()
{
    g_players = get_players();
    return true;
}

bool update_entity_cache()
{
    if (g_players.size() > 0)
    {
        if (g_players.at(0)->uid != cache_player)
        {
            entity_cache.clear();
            cache_player = g_players.at(0)->uid;
            return true;
        }
        return false;
    }
    cache_player = 0;
    return false;
}

void spawn_entities(bool s, std::string list = "")
{
    std::string search(text);
    const auto pos = search.find_first_of(" ");
    if (list == "" && pos == std::string::npos && g_filtered_count > 0)
    {
        if (g_current_item == 0 && g_filtered_count == g_items.size())
            return;
        int spawned = spawn_entity(g_items[g_filtered_items[g_current_item]].id, g_x, g_y, s, g_vx, g_vy, options["snap_to_grid"]);
        if (!lock_entity)
            g_last_id = spawned;
    }
    else
    {
        std::string texts(text);
        if (list != "")
            texts = list;
        std::stringstream textss(texts);
        int id;
        std::vector<int> ents;
        int spawned;
        while (textss >> id)
        {
            spawned = spawn_entity(id, g_x, g_y, s, g_vx, g_vy, options["snap_to_grid"]);
        }
        if (!lock_entity)
            g_last_id = spawned;
    }
}

int pick_selected_entity(ImGuiInputTextCallbackData *data)
{
    if (data->EventFlag == ImGuiInputTextFlags_CallbackCompletion)
    {
        if (g_filtered_count == 0)
            return 1;
        if (g_current_item == 0 && g_filtered_count == g_items.size())
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
        data->InsertChars(0, search.data());
    }
    return 0;
}

const char *entity_name(int id)
{
    for (int i = 0; i < g_items.size(); i++)
    {
        if (g_items[i].id == id)
        {
            return g_items[i].name.data();
        }
    }
    return "";
}

bool update_entity()
{
    if (!visible("tool_entity_properties"))
        return false;
    if (g_last_id != 0)
    {
        g_entity_type = entity_type(g_last_id);
        g_entity = entity_ptr(g_last_id);
        g_entity_addr = reinterpret_cast<uintptr_t>(g_entity);
        if (IsBadWritePtr(g_entity, 0x178))
            g_entity = nullptr;
        if (g_entity && (g_entity_type >= 194 && g_entity_type <= 213))
        {
            g_inventory = (struct Inventory *)g_entity->inventory_ptr;
            if (IsBadWritePtr(g_inventory, 0x1428))
                g_inventory = nullptr;
            return true;
        }
        else
        {
            g_inventory = nullptr;
            return true;
        }
    }
    else
    {
        g_entity = nullptr;
        g_inventory = nullptr;
        g_entity_addr = 0;
    }
    return false;
}

void set_zoom()
{
    zoom(g_zoom);
}

void force_zoom()
{
    if (g_zoom == 0.0 && g_state != 0 && (g_state->w != g_level_width))
    {
        set_zoom();
        g_level_width = g_state->w;
    }
}

void force_hud_flags()
{
    if (g_state == 0)
        return;
    if (!options["disable_pause"] && !ImGui::GetIO().WantCaptureKeyboard && !register_keys_alt)
        g_state->hud_flags |= 1U << 19;
    else if (!ImGui::GetIO().WantCaptureKeyboard)
        g_state->hud_flags &= ~(1U << 19);
    if (hud_dark_level)
        g_state->hud_flags |= 1U << 17;
    else
        g_state->hud_flags &= ~(1U << 17);
}

void force_noclip()
{
    g_players = get_players();
    if (options["noclip"])
    {
        for (auto player : g_players)
        {
            player->standing_on_uid = -1;
            player->flags |= 1U << 9;
            player->flags |= 1U << 4;
            player->velocityx = player->movex * player->type->max_speed;
            player->velocityy = player->movey * player->type->max_speed;
        }
    }
}

LRESULT CALLBACK window_hook(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
    auto msg = (CWPSTRUCT *)(lParam);
    if (msg->hwnd != window)
        return 0;

    if (process_resizing(msg->message, msg->wParam, msg->lParam))
        return 0;

    return 0;
}

bool pressed(std::string keyname, int wParam)
{
    if (keys.find(keyname) == keys.end() || (keys[keyname] & 0xff) == 0)
    {
        return false;
    }
    int keycode = keys[keyname];
    if (GetAsyncKeyState(VK_CONTROL))
    {
        wParam += 0x100;
    }
    if (GetAsyncKeyState(VK_SHIFT))
    {
        wParam += 0x200;
    }
    return wParam == keycode;
}

bool clicked(std::string keyname)
{
    int wParam = 0x400;
    if (keys.find(keyname) == keys.end() || (keys[keyname] & 0xff) == 0)
    {
        return false;
    }
    int keycode = keys[keyname];
    if (GetAsyncKeyState(VK_CONTROL))
    {
        wParam += 0x100;
    }
    if (GetAsyncKeyState(VK_SHIFT))
    {
        wParam += 0x200;
    }
    for (int i = 0; i < ImGuiMouseButton_COUNT; i++)
    {
        if (ImGui::IsMouseClicked(i))
        {
            wParam += i + 1;
            break;
        }
    }
    return wParam == keycode;
}

bool held(std::string keyname)
{
    int wParam = 0x400;
    if (keys.find(keyname) == keys.end() || (keys[keyname] & 0xff) == 0)
    {
        return false;
    }
    int keycode = keys[keyname];
    if (GetAsyncKeyState(VK_CONTROL))
    {
        wParam += 0x100;
    }
    if (GetAsyncKeyState(VK_SHIFT))
    {
        wParam += 0x200;
    }
    for (int i = 0; i < ImGuiMouseButton_COUNT; i++)
    {
        if (ImGui::IsMouseDown(i))
        {
            wParam += i + 1;
            break;
        }
    }
    return wParam == keycode;
}

bool released(std::string keyname)
{
    int wParam = 0x400;
    if (keys.find(keyname) == keys.end() || (keys[keyname] & 0xff) == 0)
    {
        return false;
    }
    int keycode = keys[keyname];
    if (GetAsyncKeyState(VK_CONTROL))
    {
        wParam += 0x100;
    }
    if (GetAsyncKeyState(VK_SHIFT))
    {
        wParam += 0x200;
    }
    for (int i = 0; i < ImGuiMouseButton_COUNT; i++)
    {
        if (ImGui::IsMouseReleased(i))
        {
            wParam += i + 1;
            break;
        }
    }
    return wParam == keycode;
}

bool process_keys(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
    if (nCode != WM_KEYDOWN)
    {
        return false;
    }

    if (ImGui::GetIO().WantCaptureKeyboard && active("tool_script"))
        return false;

    int repeat = (lParam >> 30) & 1U;

    if (pressed("hide_ui", wParam))
    {
        hide_ui = !hide_ui;
    }
    else if (pressed("tool_entity", wParam))
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
        if (toggle("tool_camera"))
        {
            set_focus_zoom = true;
        }
    }
    else if (pressed("tool_entity_properties", wParam))
    {
        toggle("tool_entity_properties");
    }
    else if (pressed("tool_game_properties", wParam))
    {
        toggle("tool_game_properties");
    }
    else if (pressed("tool_options", wParam))
    {
        toggle("tool_options");
    }
    else if (pressed("zoom_out", wParam))
    {
        g_zoom += 1.0;
        set_zoom();
    }
    else if (pressed("zoom_in", wParam))
    {
        g_zoom -= 1.0;
        set_zoom();
    }
    else if (pressed("zoom_default", wParam))
    {
        g_zoom = 13.5;
        set_zoom();
    }
    else if (pressed("zoom_3x", wParam))
    {
        g_zoom = 23.08;
        set_zoom();
    }
    else if (pressed("zoom_4x", wParam))
    {
        g_zoom = 29.87;
        set_zoom();
    }
    else if (pressed("zoom_5x", wParam))
    {
        g_zoom = 36.66;
        set_zoom();
    }
    else if (pressed("zoom_auto", wParam))
    {
        g_zoom = 0.0;
        set_zoom();
    }
    else if (pressed("toggle_godmode", wParam))
    {
        options["god_mode"] = !options["god_mode"];
        godmode(options["god_mode"]);
    }
    else if (pressed("toggle_noclip", wParam))
    {
        options["noclip"] = !options["noclip"];
        g_players = get_players();
        for (auto player : g_players)
        {
            if (options["noclip"])
            {
                player->type->max_speed = 0.3;
            }
            else
            {
                player->flags &= ~(1U << 9);
                player->flags &= ~(1U << 4);
                player->type->max_speed = 0.0725;
            }
        }
    }
    else if (pressed("toggle_hitboxes", wParam))
    {
        options["draw_hitboxes"] = !options["draw_hitboxes"];
    }
    else if (pressed("toggle_grid", wParam))
    {
        options["draw_grid"] = !options["draw_grid"];
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
        paused = !paused;
        if (paused)
            g_state->pause = 0x20;
        else
            g_state->pause = 0;
    }
    else if (pressed("toggle_disable_pause", wParam))
    {
        options["disable_pause"] = !options["disable_pause"];
        force_hud_flags();
    }
    else if (pressed("toggle_disable_input", wParam))
    {
        options["disable_input"] = !options["disable_input"];
    }
    else if (pressed("toggle_disable_input_alt", wParam))
    {
        options["disable_input_alt"] = !options["disable_input_alt"];
    }
    else if (pressed("teleport_left", wParam))
    {
        teleport(-3, 0, false, 0, 0, options["snap_to_grid"]);
    }
    else if (pressed("teleport_right", wParam))
    {
        teleport(3, 0, false, 0, 0, options["snap_to_grid"]);
    }
    else if (pressed("teleport_up", wParam))
    {
        teleport(0, 3, false, 0, 0, options["snap_to_grid"]);
    }
    else if (pressed("teleport_down", wParam))
    {
        teleport(0, -3, false, 0, 0, options["snap_to_grid"]);
    }
    else if (pressed("spawn_layer_door", wParam))
    {
        spawn_backdoor(0.0, 0.0);
    }
    else if (pressed("teleport", wParam))
    {
        teleport(g_x, g_y, false, 0, 0, options["snap_to_grid"]);
    }
    else if (pressed("coordinate_left", wParam))
    {
        g_x -= 1;
    }
    else if (pressed("coordinate_right", wParam))
    {
        g_x += 1;
    }
    else if (pressed("coordinate_up", wParam))
    {
        g_y += 1;
    }
    else if (pressed("coordinate_down", wParam))
    {
        g_y -= 1;
    }
    else if (pressed("spawn_entity", wParam))
    {
        spawn_entities(false);
    }
    else if (pressed("spawn_warp_door", wParam))
    {
        int spawned = spawn_door(0.0, 0.0, g_world, g_level, g_to + 1);
        if (!lock_entity)
            g_last_id = spawned;
    }
    else if (pressed("move_up", wParam) && active("tool_entity"))
    {
        g_current_item = std::min(std::max(g_current_item - 1, 0), g_filtered_count - 1);
        scroll_to_entity = true;
    }
    else if (pressed("move_down", wParam) && active("tool_entity"))
    {
        g_current_item = std::min(std::max(g_current_item + 1, 0), g_filtered_count - 1);
        scroll_to_entity = true;
    }
    else if (pressed("move_pageup", wParam) && active("tool_entity"))
    {
        ImGuiContext &g = *GImGui;
        ImGuiWindow *current = g.NavWindow;
        int page = std::max((int)((current->Size.y - 100) / ImGui::GetTextLineHeightWithSpacing() / 2), 1);
        g_current_item = std::min(std::max(g_current_item - page, 0), g_filtered_count - 1);
        scroll_to_entity = true;
    }
    else if (pressed("move_pagedown", wParam) && active("tool_entity"))
    {
        ImGuiContext &g = *GImGui;
        ImGuiWindow *current = g.NavWindow;
        int page = std::max((int)((current->Size.y - 100) / ImGui::GetTextLineHeightWithSpacing() / 2), 1);
        g_current_item = std::min(std::max(g_current_item + page, 0), g_filtered_count - 1);
        scroll_to_entity = true;
    }
    else if (pressed("enter", wParam) && active("tool_entity"))
    {
        spawn_entities(false);
    }
    else if (pressed("move_up", wParam) && active("tool_door"))
    {
        g_to = std::min(std::max(g_to - 1, 0), 15);
    }
    else if (pressed("move_down", wParam) && active("tool_door"))
    {
        g_to = std::min(std::max(g_to + 1, 0), 15);
    }
    else if (pressed("enter", wParam) && active("tool_door"))
    {
        int spawned = spawn_door(0.0, 0.0, g_world, g_level, g_to + 1);
        if (!lock_entity)
            g_last_id = spawned;
    }
    else if (pressed("move_up", wParam) && active("tool_camera"))
    {
        g_zoom -= 1.0;
        set_zoom();
    }
    else if (pressed("move_down", wParam) && active("tool_camera"))
    {
        g_zoom += 1.0;
        set_zoom();
    }
    else if (pressed("enter", wParam) && active("tool_camera"))
    {
        set_zoom();
    }
    else if (pressed("tool_debug", wParam))
    {
        if(!options["tabbed_interface"])
            show_debug = !show_debug;
        else
            toggle("tool_debug");
    }
    else if (pressed("tool_script", wParam))
    {
        toggle("tool_script");
    }
    else if (pressed("reset_windows", wParam))
    {
        options["stack_horizontally"] = !options["stack_horizontally"];
        if (options["stack_horizontally"])
        {
            options["stack_vertically"] = false;
            options["tabbed_interface"] = false;
        }
    }
    else if (pressed("reset_windows_vertical", wParam))
    {
        options["stack_vertically"] = !options["stack_vertically"];
        if (options["stack_vertically"])
        {
            options["stack_horizontally"] = false;
            options["tabbed_interface"] = false;
        }
    }
    else if (pressed("tabbed_interface", wParam))
    {
        options["tabbed_interface"] = !options["tabbed_interface"];
        if (options["tabbed_interface"])
        {
            options["stack_horizontally"] = false;
            options["stack_vertically"] = false;
        }
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
    else if (pressed("tool_style", wParam))
    {
        if(!options["tabbed_interface"])
            change_colors = !change_colors;
        else
            toggle("tool_style");
    }
    else if (pressed("tool_metrics", wParam))
    {
        show_app_metrics = !show_app_metrics;
    }
    else if (pressed("escape", wParam))
    {
        escape();
    }
    else if (pressed("move_down", wParam) && (float)rand() / RAND_MAX > 0.99 && !repeat)
    {
        spawn_entity(372, 0, -0.5, false, 0, 0, false);
    }
    else
    {
        return false;
    }
    return true;
}

void init_imgui()
{
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
    io.MouseDrawCursor = true;
    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX11_Init(pDevice, pContext);

    /*if (!SetWindowsHookExA(WH_GETMESSAGE, msg_hook, 0, GetCurrentThreadId()))
    {
        printf("Message hook error: 0x%x\n", GetLastError());
    }
    if (!SetWindowsHookExA(WH_CALLWNDPROC, window_hook, 0,
    GetCurrentThreadId()))
    {
        printf("WndProc hook error: 0x%x\n", GetLastError());
    }*/
}

std::string last_word(std::string str)
{
    while (!str.empty() && std::isspace(str.back()))
        str.pop_back();
    const auto pos = str.find_last_of(" ");
    return pos == std::string::npos ? str : str.substr(pos + 1);
}

void update_filter(const char *s)
{
    int count = 0;
    std::string search(s);
    std::string last = last_word(search);
    int searchid = 0;
    try
    {
        searchid = stoi(last);
    }
    catch (const std::exception &err)
    {
    }
    for (int i = 0; i < g_items.size(); i++)
    {
        if (s[0] == '\0' || std::isspace(search.back()) || StrStrIA(g_items[i].name.data(), last.data()) || g_items[i].id == searchid)
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

void write_file()
{
    std::ofstream file;
    file.open("entities.txt");
    for (int i = 1; i < g_items.size(); i++)
    {
        file << g_items[i].id << ": " << g_items[i].name.data() << std::endl;
    }
    file.close();
    file_written = true;
}

void render_int(const char *label, int state)
{
    char statec[15];
    itoa(state, statec, 10);
    ImGui::LabelText(label, statec);
}

void render_list()
{
    // ImGui::ListBox with filter
    if (!ImGui::ListBoxHeader("##Entities", {-1, -1}))
        return;
    bool value_changed = false;
    ImGuiListClipper clipper;
    clipper.Begin(g_filtered_count, ImGui::GetTextLineHeightWithSpacing());
    if (scroll_top)
    {
        scroll_top = false;
        ImGui::SetScrollHereY();
    }
    while (clipper.Step())
    {
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
        {
            const bool item_selected = (i == g_current_item);
            std::stringstream item_ss;
            item_ss << g_items[g_filtered_items[i]].id;
            std::string item_id = item_ss.str();
            std::string item_name = g_items[g_filtered_items[i]].name.data();
            std::string item_concat = item_id + ": " + item_name.substr(9);
            const char *item_text = item_concat.c_str();
            ImGui::PushID(i);
            if (ImGui::Selectable(item_text, item_selected))
            {
                g_current_item = i;
                value_changed = true;
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
}

void render_themes()
{
    // ImGui::ListBox with filter
    if (!ImGui::BeginCombo("Theme##Theme", themes[g_to]))
        return;
    bool value_changed = false;
    for (int i = 0; i < 17; i++)
    {
        const bool item_selected = (i == g_to);
        const char *item_text = themes[i];

        ImGui::PushID(i);
        if (ImGui::Selectable(item_text, item_selected))
        {
            g_to = i;
            value_changed = true;
        }
        if (item_selected)
            ImGui::SetItemDefaultFocus();
        ImGui::PopID();
    }
    ImGui::EndCombo();
}

std::string unique_label(std::string label)
{
    label += "##";
    label += rand();
    return label;
}

void render_input()
{
    int n = 0;
    for (auto i : saved_entities)
    {
        ImGui::PushID(i.data());
        std::string search = "";
        std::stringstream sss(i);
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
            search = i;

        ImGui::PushID(2 * n);
        if (ImGui::Button("X"))
        {
            saved_entities.erase(saved_entities.begin() + n);
            save_config(cfgfile);
        }
        ImGui::SameLine();
        ImGui::PopID();

        ImGui::PushID(4 * n);
        if (ImGui::Button("Load"))
        {
            strcpy(text, i.data());
            update_filter(text);
            // spawn_entities(false);
        }
        ImGui::SameLine();
        ImGui::PopID();

        ImGui::PushID(8 * n);
        if (ImGui::Button("Spawn"))
        {
            spawn_entities(false, i);
        }
        ImGui::PopID();

        ImGui::PopID();
        ImGui::SameLine();
        ImGui::TextWrapped(search.data());
        n++;
    }
    if (set_focus_entity)
    {
        ImGui::SetKeyboardFocusHere();
        set_focus_entity = false;
    }
    ImVec2 region = ImGui::GetContentRegionMax();
    ImGui::PushItemWidth(region.x - 110);
    if (ImGui::InputText("##Input", text, sizeof(text), ImGuiInputTextFlags_CallbackCompletion, pick_selected_entity))
    {
        update_filter(text);
    }
    ImGui::PopItemWidth();
    ImGui::SameLine();
    if (ImGui::Button("Save"))
    {
        save_search();
    }
    ImGui::SameLine();
    if (ImGui::Button("Spawn"))
    {
        spawn_entities(false);
    }
}

void render_narnia()
{
    ImGui::PushItemWidth(-ImGui::GetWindowWidth() * 0.5f);
    if (set_focus_world)
    {
        ImGui::SetKeyboardFocusHere();
        set_focus_world = false;
    }
    if (ImGui::DragScalar("World##WarpWorld", ImGuiDataType_U8, &g_world, 0.1f, &u8_one, &u8_seven)) {}
    if (ImGui::DragScalar("Level##WarpLevel", ImGuiDataType_U8, &g_level, 0.1f, &u8_one, &u8_four)) {}
    render_themes();
    if (ImGui::Button("Warp door"))
    {
        int spawned = spawn_door(g_x, g_y, g_world, g_level, g_to + 1);
        if (!lock_entity)
            g_last_id = spawned;
    }
    ImGui::SameLine();
    if (ImGui::Button("Layer door"))
    {
        spawn_backdoor(g_x, g_y);
    }
    ImGui::PopItemWidth();
}

void render_camera()
{
    if (set_focus_zoom)
    {
        ImGui::SetKeyboardFocusHere();
        set_focus_zoom = false;
    }
    ImGui::PushItemWidth(-ImGui::GetContentRegionMax().x * 0.5f);
    if (ImGui::DragFloat("Zoom##ZoomLevel", &g_zoom, 0.5f, 0.5, 60.0, "%.2f"))
    {
        set_zoom();
    }
    ImGui::PopItemWidth();
    if (ImGui::Button("Default"))
    {
        g_zoom = 13.5;
        set_zoom();
    }
    ImGui::SameLine();
    if (ImGui::Button("3x"))
    {
        g_zoom = 23.08;
        set_zoom();
    }
    ImGui::SameLine();
    if (ImGui::Button("4x"))
    {
        g_zoom = 29.87;
        set_zoom();
    }
    ImGui::SameLine();
    if (ImGui::Button("5x"))
    {
        g_zoom = 36.66;
        set_zoom();
    }
    ImGui::SameLine();
    if (ImGui::Button("Auto"))
    {
        g_zoom = 0.0;
        set_zoom();
    }
}

void render_arrow()
{
    ImGuiIO &io = ImGui::GetIO();
    ImVec2 res = io.DisplaySize;
    ImVec2 pos = ImGui::GetMousePos();
    ImVec2 line = ImVec2(pos.x - startpos.x, pos.y - startpos.y);
    float length = sqrt(pow(line.x, 2) + pow(line.y, 2));
    float theta = 0.7;
    float width = 10 + length / 15;
    float tpoint = width / (2 * (tanf(theta) / 2) * length);
    ImVec2 point = ImVec2(pos.x + (-tpoint * line.x), pos.y + (-tpoint * line.y));
    ImVec2 normal = ImVec2(-line.x, line.y);
    float tnormal = width / (2 * length);
    ImVec2 leftpoint = ImVec2(point.x + tnormal * normal.y, point.y + tnormal * normal.x);
    ImVec2 rightpoint = ImVec2(point.x + (-tnormal * normal.y), point.y + (-tnormal * normal.x));
    auto *draw_list = ImGui::GetWindowDrawList();
    draw_list->AddLine(ImVec2(startpos.x - 9, startpos.y - 9), ImVec2(startpos.x + 10, startpos.y + 10), ImColor(255, 255, 255, 200), 2);
    draw_list->AddLine(ImVec2(startpos.x - 9, startpos.y + 9), ImVec2(startpos.x + 10, startpos.y - 10), ImColor(255, 255, 255, 200), 2);
    draw_list->AddLine(startpos, pos, ImColor(255, 0, 0, 200), 2);
    draw_list->AddLine(leftpoint, ImVec2(pos.x, pos.y), ImColor(255, 0, 0, 200), 2);
    draw_list->AddLine(rightpoint, ImVec2(pos.x, pos.y), ImColor(255, 0, 0, 200), 2);
}

void render_cross()
{
    ImGuiIO &io = ImGui::GetIO();
    ImVec2 res = io.DisplaySize;
    auto *draw_list = ImGui::GetWindowDrawList();
    draw_list->AddLine(ImVec2(startpos.x - 9, startpos.y - 9), ImVec2(startpos.x + 10, startpos.y + 10), ImColor(255, 255, 255, 200), 2);
    draw_list->AddLine(ImVec2(startpos.x - 9, startpos.y + 9), ImVec2(startpos.x + 10, startpos.y - 10), ImColor(255, 255, 255, 200), 2);
}

void render_grid(ImColor gridcolor = ImColor(1.0f, 1.0f, 1.0f, 0.2f))
{
    if (g_state == 0 || (g_state->screen != 11 && g_state->screen != 12))
        return;
    ImGuiIO &io = ImGui::GetIO();
    ImVec2 res = io.DisplaySize;
    auto *draw_list = ImGui::GetWindowDrawList();
    for (int x = -1; x < 96; x++)
    {
        std::pair<float, float> gridline = screen_position((float)x + 0.5, 0);
        if (abs(gridline.first) <= 1.0)
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
            draw_list->AddLine(ImVec2(grids.x, 0), ImVec2(grids.x, res.y), color, width);
        }
    }
    for (int y = -1; y < 128; y++)
    {
        std::pair<float, float> gridline = screen_position(0, (float)y + 0.5);
        if (abs(gridline.second) <= 1.0)
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
            draw_list->AddLine(ImVec2(0, grids.y), ImVec2(res.x, grids.y), color, width);
        }
    }
    g_players = get_players();
    for (auto player : g_players)
    {
        std::pair<float, float> gridline = screen_position(round(player->position().first - 0.5) + 0.5, round(player->position().second) - 0.5);
        ImVec2 grids = screenify({gridline.first, gridline.second});
        draw_list->AddLine(ImVec2(0, grids.y), ImVec2(res.x, grids.y), ImColor(255, 0, 255, 200), 2);
        draw_list->AddLine(ImVec2(grids.x, 0), ImVec2(grids.x, res.y), ImColor(255, 0, 255, 200), 2);
    }
    if (update_entity())
    {
        std::pair<float, float> gridline = screen_position(round(g_entity->position().first - 0.5) + 0.5, round(g_entity->position().second) - 0.5);
        ImVec2 grids = screenify({gridline.first, gridline.second});
        draw_list->AddLine(ImVec2(0, grids.y), ImVec2(res.x, grids.y), ImColor(0, 255, 0, 200), 2);
        draw_list->AddLine(ImVec2(grids.x, 0), ImVec2(grids.x, res.y), ImColor(0, 255, 0, 200), 2);
    }
}

void render_hitbox(Movable *ent, bool cross, ImColor color)
{
    if(IsBadReadPtr(ent, 0x178))
        return;
    if (ent->items_count > 0)
    {
        int *pitems = (int *)ent->items_ptr;
        for (int i = 0; i < ent->items_count; i++)
        {
            render_hitbox(entity_ptr(pitems[i]), false, ImColor(255, 0, 0, 150));
        }
    }
    const int type = entity_type(ent->uid);
    if (!type || ((type >= 538 && type <= 555) || type == 648))
        return; // powerups
    std::pair<float, float> pos = screen_position(ent->position().first, ent->position().second);
    std::pair<float, float> boxa =
        screen_position(ent->position().first - ent->hitboxx + ent->offsetx, ent->position().second - ent->hitboxy + ent->offsety);
    std::pair<float, float> boxb =
        screen_position(ent->position().first + ent->hitboxx + ent->offsetx, ent->position().second - ent->hitboxy + ent->offsety);
    std::pair<float, float> boxc =
        screen_position(ent->position().first + ent->hitboxx + ent->offsetx, ent->position().second + ent->hitboxy + ent->offsety);
    std::pair<float, float> boxd =
        screen_position(ent->position().first - ent->hitboxx + ent->offsetx, ent->position().second + ent->hitboxy + ent->offsety);
    ImVec2 spos = screenify({pos.first, pos.second});
    ImVec2 sboxa = screenify({boxa.first, boxa.second});
    ImVec2 sboxb = screenify({boxb.first, boxb.second});
    ImVec2 sboxc = screenify({boxc.first, boxc.second});
    ImVec2 sboxd = screenify({boxd.first, boxd.second});
    auto *draw_list = ImGui::GetWindowDrawList();
    if (cross)
    {
        draw_list->AddLine(ImVec2(spos.x - 9, spos.y - 9), ImVec2(spos.x + 10, spos.y + 10), ImColor(0, 255, 0, 200), 2);
        draw_list->AddLine(ImVec2(spos.x - 9, spos.y + 9), ImVec2(spos.x + 10, spos.y - 10), ImColor(0, 255, 0, 200), 2);
    }
    draw_list->AddLine(sboxa, sboxb, color, 2);
    draw_list->AddLine(sboxb, sboxc, color, 2);
    draw_list->AddLine(sboxc, sboxd, color, 2);
    draw_list->AddLine(sboxd, sboxa, color, 2);
}

void render_script(Script *script)
{
    auto *draw_list = ImGui::GetBackgroundDrawList();
    script->run(draw_list);
}

ImVec2 normalize(ImVec2 pos)
{
    ImGuiIO &io = ImGui::GetIO();
    ImVec2 res = io.DisplaySize;
    if (res.x / res.y > 1.78)
    {
        pos.x -= (res.x - res.y / 9 * 16) / 2;
        res.x = res.y / 9 * 16;
    }
    else if (res.x / res.y < 1.77)
    {
        pos.y -= (res.y - res.x / 16 * 9) / 2;
        res.y = res.x / 16 * 9;
    }
    ImVec2 normal = ImVec2((pos.x - res.x / 2) * (1.0 / (res.x / 2)), -(pos.y - res.y / 2) * (1.0 / (res.y / 2)));
    return normal;
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
    g_vx = 2 * (g_vx - g_x);
    g_vy = 2 * (g_vy - g_y) * 0.5625;
}

void render_script_options(Script *script)
{
    for (auto &option : script->options)
    {
        if (int *val = std::get_if<int>(&option.second.value))
        {
            int min = std::get<int>(option.second.min);
            int max = std::get<int>(option.second.max);
            if (ImGui::DragInt(option.second.desc.data(), val, 0.5f, min, max))
            {
                option.second.value = *val;
                script->lua["options"][option.first] = *val;
            }
        }
        else if (bool *val = std::get_if<bool>(&option.second.value))
        {
            if (ImGui::Checkbox(option.second.desc.data(), val))
            {
                option.second.value = *val;
                script->lua["options"][option.first] = *val;
            }
        }
    }
}

void render_messages(Script *script)
{
    auto now = std::chrono::system_clock::now();
    ImGuiIO &io = ImGui::GetIO();
    ImGui::SetNextWindowSize({-1, -1});
    ImGui::Begin(
        "Messages",
        NULL,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
    using namespace std::chrono_literals;
    ImGui::PushFont(bigfont);
    for (auto message : script->messages)
    {
        if (now - 10s > message.second)
            continue;
        const float alpha = 1.0f - std::chrono::duration_cast<std::chrono::milliseconds>(now - message.second).count() / 10000.0f;
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, alpha), "[%s] %s", script->meta.name.data(), message.first.data());
    }
    ImGui::PopFont();
    ImGui::SetWindowPos({30.0f + 0.128f * io.DisplaySize.x * io.FontGlobalScale, io.DisplaySize.y - ImGui::GetWindowHeight() - 20});
    ImGui::End();
}

void render_messages()
{
    using namespace std::chrono_literals;
    using Message = std::tuple<std::string, std::string, std::chrono::time_point<std::chrono::system_clock>>;
    auto now = std::chrono::system_clock::now();
    std::vector<Message> queue;
    for (auto script : g_scripts)
    {
        for (auto message : script->messages)
        {
            if (now - 10s > message.second)
                continue;
            queue.push_back(std::make_tuple(script->meta.name, message.first, message.second));
        }
    }
    ImGuiIO &io = ImGui::GetIO();
    ImGui::PushFont(bigfont);

    std::sort(queue.begin(), queue.end(), [](Message a, Message b) { return std::get<2>(a) < std::get<2>(b); });

    ImGui::SetNextWindowSize({-1, -1});
    ImGui::Begin(
        "Messages",
        NULL,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);

    const float fontsize = (ImGui::GetCurrentWindow()->CalcFontSize() + ImGui::GetStyle().ItemSpacing.y);

    int logsize = std::min(30, (int)((io.DisplaySize.y - 300) / fontsize));
    if (queue.size() > logsize)
    {
        std::vector<Message> newqueue(queue.end() - logsize, queue.end());
        queue = newqueue;
    }

    ImGui::SetWindowPos({30.0f + 0.128f * io.DisplaySize.x * io.FontGlobalScale, io.DisplaySize.y - queue.size() * fontsize - 20});
    for (auto message : queue)
    {
        const float alpha = 1.0f - std::chrono::duration_cast<std::chrono::milliseconds>(now - std::get<2>(message)).count() / 10000.0f;
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, alpha), "[%s] %s", std::get<0>(message).data(), std::get<1>(message).data());
    }
    ImGui::PopFont();
    ImGui::End();
}

void render_clickhandler()
{
    ImGuiIO &io = ImGui::GetIO();
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::SetNextWindowPos({0, 0});
    ImGui::Begin(
        "Clickhandler",
        NULL,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
    if (options["draw_grid"])
    {
        render_grid();
    }
    if (options["draw_hitboxes"])
    {
        for (auto test : get_entities_by(0, 255, -1))
        {
            render_hitbox(entity_ptr(test), false, ImColor(0, 255, 255, 150));
        }
        g_players = get_players();
        for (auto player : g_players)
        {
            render_hitbox(player, false, ImColor(255, 0, 255, 200));
        }
    }
    if (options["draw_hitboxes"] && update_entity())
    {
        render_hitbox(g_entity, true, ImColor(0, 255, 0, 200));
    }
    for (auto script : g_scripts)
    {
        render_script(script);
    }
    if (options["mouse_control"])
    {
        ImGui::InvisibleButton("canvas", ImGui::GetContentRegionMax(), ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);

        if ((clicked("mouse_spawn_throw") || clicked("mouse_teleport_throw")) && ImGui::IsWindowFocused())
        {
            io.MouseDrawCursor = false;
            startpos = ImGui::GetMousePos();
        }
        else if ((clicked("mouse_spawn") || clicked("mouse_teleport")) && ImGui::IsWindowFocused())
        {
            io.MouseDrawCursor = false;
            startpos = ImGui::GetMousePos();
        }
        else if ((held("mouse_spawn_throw") || held("mouse_teleport_throw")) && ImGui::IsWindowFocused())
        {
            render_arrow();
        }
        else if ((held("mouse_spawn") || held("mouse_teleport")) && ImGui::IsWindowFocused())
        {
            startpos = ImGui::GetMousePos();
            render_cross();
        }
        else if (released("mouse_spawn_throw") && ImGui::IsWindowFocused())
        {
            set_pos(startpos);
            set_vel(ImGui::GetMousePos());
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
        else if (released("mouse_teleport_throw") && ImGui::IsWindowFocused())
        {
            set_pos(startpos);
            set_vel(ImGui::GetMousePos());
            teleport(g_x, g_y, true, g_vx, g_vy, options["snap_to_grid"]);
            g_x = 0;
            g_y = 0;
            g_vx = 0;
            g_vy = 0;
        }
        else if (released("mouse_teleport") && ImGui::IsWindowFocused())
        {
            set_pos(startpos);
            teleport(g_x, g_y, true, g_vx, g_vy, options["snap_to_grid"]);
            g_x = 0;
            g_y = 0;
            g_vx = 0;
            g_vy = 0;
        }
        else if (clicked("mouse_grab") || clicked("mouse_grab_unsafe"))
        {
            startpos = ImGui::GetMousePos();
            set_pos(startpos);
            unsigned int mask = 0b01111111;
            if (held("mouse_grab_unsafe"))
            {
                mask = 0xffffffff;
            }
            g_held_id = get_entity_at(g_x, g_y, true, 1, mask);
            g_held_entity = entity_ptr(g_held_id);
            if (g_held_entity)
                g_held_flags = g_held_entity->flags;
            if (!lock_entity)
                g_last_id = g_held_id;
        }
        else if (held("mouse_grab_throw") && g_held_id > 0)
        {
            if (!throw_held)
            {
                startpos = ImGui::GetMousePos();
                throw_held = true;
            }
            set_pos(startpos);
            move_entity(g_held_id, g_x, g_y, true, 0, 0, false);
            render_arrow();
        }
        else if ((held("mouse_grab") || held("mouse_grab_unsafe")) && g_held_id > 0 && g_held_entity != 0)
        {
            startpos = ImGui::GetMousePos();
            throw_held = false;
            io.MouseDrawCursor = false;
            set_pos(startpos);
            if (ImGui::IsMouseDragging(keys["mouse_grab"] & 0xff - 1) || ImGui::IsMouseDragging(keys["mouse_grab_unsafe"] & 0xff - 1))
            {
                if (g_held_entity)
                {
                    g_held_entity->standing_on_uid = -1;
                    g_held_entity->flags |= 1U << 4;
                    g_held_entity->flags |= 1U << 9;
                }
                move_entity(g_held_id, g_x, g_y, true, 0, 0, false);
            }
        }
        if (released("mouse_grab_throw") && g_held_id > 0 && g_entity != 0)
        {
            throw_held = false;
            io.MouseDrawCursor = true;
            if (g_held_entity)
                g_held_entity->flags = g_held_flags;
            set_pos(startpos);
            set_vel(ImGui::GetMousePos());
            move_entity(g_held_id, g_x, g_y, true, g_vx, g_vy, options["snap_to_grid"]);
            g_x = 0;
            g_y = 0;
            g_vx = 0;
            g_vy = 0;
            g_held_id = 0;
        }
        else if ((released("mouse_grab") || released("mouse_grab_unsafe")) && g_held_id > 0 && g_held_entity != 0)
        {
            throw_held = false;
            io.MouseDrawCursor = true;
            if (g_held_entity)
                g_held_entity->flags = g_held_flags;
            if (options["snap_to_grid"])
            {
                move_entity(g_held_id, g_x, g_y, true, 0, 0, options["snap_to_grid"]);
            }
            g_x = 0;
            g_y = 0;
            g_vx = 0;
            g_vy = 0;
            g_held_id = 0;
        }
        else if (released("mouse_clone"))
        {
            set_pos(ImGui::GetMousePos());
            spawn_entity(426, g_x, g_y, true, 0, 0, options["snap_to_grid"]);
            g_x = 0;
            g_y = 0;
            g_vx = 0;
            g_vy = 0;
        }
        else if (held("mouse_zap") && ImGui::GetFrameCount() > g_last_gun + ImGui::GetIO().Framerate / 5)
        {
            g_last_gun = ImGui::GetFrameCount();
            set_pos(ImGui::GetMousePos());
            set_vel(ImVec2(ImGui::GetMousePos().x, ImGui::GetMousePos().y + 200));
            spawn_entity(380, g_x, g_y, true, g_vx, g_vy, options["snap_to_grid"]);
            g_x = 0;
            g_y = 0;
            g_vx = 0;
            g_vy = 0;
        }
        else if (held("mouse_blast") && ImGui::GetFrameCount() > g_last_gun + ImGui::GetIO().Framerate / 10)
        {
            g_last_gun = ImGui::GetFrameCount();
            set_pos(ImGui::GetMousePos());
            spawn_entity(687, g_x, g_y, true, g_vx, g_vy, options["snap_to_grid"]);
            g_x = 0;
            g_y = 0;
            g_vx = 0;
            g_vy = 0;
        }
        else if (held("mouse_boom") && ImGui::GetFrameCount() > g_last_gun + ImGui::GetIO().Framerate / 5)
        {
            g_last_gun = ImGui::GetFrameCount();
            set_pos(ImGui::GetMousePos());
            spawn_entity(630, g_x, g_y, true, g_vx, g_vy, options["snap_to_grid"]);
            g_x = 0;
            g_y = 0;
            g_vx = 0;
            g_vy = 0;
        }
        else if (held("mouse_big_boom") && ImGui::GetFrameCount() > g_last_gun + ImGui::GetIO().Framerate / 5)
        {
            g_last_gun = ImGui::GetFrameCount();
            set_pos(ImGui::GetMousePos());
            spawn_entity(631, g_x, g_y, true, g_vx, g_vy, options["snap_to_grid"]);
            g_x = 0;
            g_y = 0;
            g_vx = 0;
            g_vy = 0;
        }
        else if (held("mouse_nuke") && ImGui::GetFrameCount() > g_last_gun + ImGui::GetIO().Framerate / 5)
        {
            g_last_gun = ImGui::GetFrameCount();
            set_pos(ImGui::GetMousePos());
            spawn_entity(631, g_x, g_y, true, g_vx, g_vy, options["snap_to_grid"]);
            spawn_entity(631, g_x - 0.2, g_y, true, g_vx, g_vy, options["snap_to_grid"]);
            spawn_entity(631, g_x + 0.2, g_y, true, g_vx, g_vy, options["snap_to_grid"]);
            spawn_entity(631, g_x, g_y - 0.3, true, g_vx, g_vy, options["snap_to_grid"]);
            spawn_entity(631, g_x, g_y + 0.3, true, g_vx, g_vy, options["snap_to_grid"]);
            spawn_entity(631, g_x + 0.15, g_y + 0.2, true, g_vx, g_vy, options["snap_to_grid"]);
            spawn_entity(631, g_x - 0.15, g_y + 0.2, true, g_vx, g_vy, options["snap_to_grid"]);
            spawn_entity(631, g_x + 0.15, g_y - 0.2, true, g_vx, g_vy, options["snap_to_grid"]);
            spawn_entity(631, g_x - 0.15, g_y - 0.2, true, g_vx, g_vy, options["snap_to_grid"]);
            g_x = 0;
            g_y = 0;
            g_vx = 0;
            g_vy = 0;
        }
        else if (released("mouse_destroy") || released("mouse_destroy_unsafe"))
        {
            ImVec2 pos = ImGui::GetMousePos();
            set_pos(pos);
            unsigned int mask = 0b01111111;
            if (released("mouse_destroy_unsafe"))
            {
                mask = 0xffffffff;
            }
            g_held_id = get_entity_at(g_x, g_y, true, 2, mask);
            if (g_held_id > 0)
            {
                // lets just sweep this under the rug for now :D
                move_entity(g_held_id, 0, -1000, false, 0, 0, true);
            }
            g_x = 0;
            g_y = 0;
            g_vx = 0;
            g_vy = 0;
            g_held_id = 0;
        }
        int buttons = 0;
        for (int i = 0; i < ImGuiMouseButton_COUNT; i++)
        {
            if (ImGui::IsMouseDown(i))
            {
                buttons += i;
            }
        }
        if (buttons == 0)
        {
            io.MouseDrawCursor = true;
        }
    }

    ImGui::End();
}

void render_options()
{
    ImGui::Checkbox("Mouse controls##clickevents", &options["mouse_control"]);
    if (ImGui::Checkbox("God mode##Godmode", &options["god_mode"]))
    {
        godmode(options["god_mode"]);
    }
    if (ImGui::Checkbox("Noclip##Noclip", &options["noclip"]))
    {
        g_players = get_players();
        for (auto player : g_players)
        {
            if (options["noclip"])
            {
                player->type->max_speed = 0.4;
            }
            else
            {
                player->flags &= ~(1U << 9);
                player->flags &= ~(1U << 4);
                player->type->max_speed = 0.0725;
            }
        }
    }
    ImGui::Checkbox("Snap to grid##Snap", &options["snap_to_grid"]);
    if (ImGui::Checkbox("Disable pause menu", &options["disable_pause"]))
    {
        force_hud_flags();
    }
    ImGui::Checkbox("Disable input when typing (hook rawinput)##DisableInputRaw", &options["disable_input"]);
    ImGui::Checkbox("Disable input when typing (alternative mode)##DisableInputFlags", &options["disable_input_alt"]);
    if (ImGui::Checkbox("Stack windows horizontally", &options["stack_horizontally"]))
    {
        options["stack_vertically"] = false;
        options["tabbed_interface"] = false;
    }
    if (ImGui::Checkbox("Stack windows vertically", &options["stack_vertically"]))
    {
        options["stack_horizontally"] = false;
        options["tabbed_interface"] = false;
    }
    if (ImGui::Checkbox("Single tabbed window", &options["tabbed_interface"]))
    {
        options["stack_horizontally"] = false;
        options["stack_vertically"] = false;
    }
    ImGui::Checkbox("Draw hitboxes##DrawEntityBox", &options["draw_hitboxes"]);
    ImGui::Checkbox("Draw gridlines##DrawTileGrid", &options["draw_grid"]);
}

void render_debug()
{
    ImGui::PushItemWidth(-ImGui::GetWindowWidth() * 0.5f);
    ImGui::InputScalar("State##StatePointer", ImGuiDataType_U64, &g_state_addr, 0, 0, "%p", ImGuiInputTextFlags_ReadOnly);
    ImGui::InputScalar("Entity##EntityPointer", ImGuiDataType_U64, &g_entity_addr, 0, 0, "%p", ImGuiInputTextFlags_ReadOnly);
    ImGui::InputScalar(
        "Level flags##HudFlagsDebug",
        ImGuiDataType_U32,
        &g_state->hud_flags,
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
    ImGui::PopItemWidth();
}

void render_script_files()
{
    ImGui::PushID("files");
    int num = 0;
    for (auto file : g_script_files)
    {
        ImGui::PushID(num++);
        if (ImGui::Button(file.filename().string().data()))
        {
            load_script(file.string().data());
        }
        ImGui::PopID();
    }
    if (g_script_files.size() == 0)
    {
        std::filesystem::path path = scriptpath;
        std::string abspath = std::filesystem::absolute(path).string();
        ImGui::TextWrapped("No scripts found. Put .lua files in '%s' or change script_dir in the ini file and reload.", abspath.data());
    }
    if (ImGui::Button("Refresh##RefreshScripts"))
    {
        refresh_script_files();
    }
    if (ImGui::Button("Create new quick script"))
    {
        Script *script = new Script(
            "meta.name = 'Script'\nmeta.version = '0.1'\nmeta.description = 'Shiny new script'\nmeta.author = 'You'\n\ncount = 0\nid = "
            "set_interval(function()\n  count = count + 1\n  message('Hello from your shiny new script')\n  if count > 4 then clear_callback(id) "
            "end\nend, 60)",
            "Script");
        g_scripts.push_back(script);
    }
    ImGui::PopID();
}

void render_scripts()
{
    ImGui::PushTextWrapPos(0.0f);
    ImGui::TextColored(
        ImVec4(1.0f, 0.3f, 0.3f, 1.0f),
        "Note: The Lua API is unstable, not ready and it WILL change, probably a lot. You can play around with it, but don't be surprised if none of your scripts work next week.");
    ImGui::PopTextWrapPos();
    ImGui::PushItemWidth(-1);
    for (int i = 0; i < g_scripts.size();)
    {
        ImGui::PushID(i);
        Script *script = g_scripts[i];
        char name[255];
        sprintf(name, "%s (%s)", script->meta.name.data(), script->meta.file.data());
        if (ImGui::CollapsingHeader(name, ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("%s %s by %s", script->meta.name.data(), script->meta.version.data(), script->meta.author.data());
            ImGui::TextWrapped(script->meta.description.data());
            if (script->enabled && ImGui::Button("Disable##DisableScript"))
            {
                script->enabled = false;
            }
            else if (!script->enabled && ImGui::Button("Enable##EnableScript"))
            {
                script->enabled = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("Unload##UnloadScript"))
            {
                g_scripts.erase(g_scripts.begin() + i);
            }
            else
            {
                ++i;
            }
            ImGui::PushItemWidth(-ImGui::GetWindowWidth() * 0.5f);
            render_script_options(script);
            ImGui::PopItemWidth();
            if (ImGui::InputTextMultiline("##LuaScript", script->code, sizeof(script->code), {-1, 300}))
            {
                script->changed = true;
            }
            InputString("##LuaResult", &script->result, ImGuiInputTextFlags_ReadOnly);
        }
        else
        {
            ++i;
        }
        ImGui::PopID();
    }
    if (ImGui::CollapsingHeader("Load new script##LoadScriptFile"), ImGuiTreeNodeFlags_DefaultOpen)
    {
        render_script_files();
    }
    ImGui::PopItemWidth();
}

bool SliderByte(const char *label, char *value, char min = 0, char max = 0, const char *format = "%lld")
{
    return ImGui::SliderScalar(label, ImGuiDataType_U8, value, &min, &max, format);
}

bool DragByte(const char *label, char *value, float speed = 1.0f, char min = 0, char max = 0, const char *format = "%lld")
{
    return ImGui::DragScalar(label, ImGuiDataType_U8, value, speed, &min, &max, format);
}

void render_uid(int uid, const char *section, bool rembtn = false)
{
    char uidc[10];
    itoa(uid, uidc, 10);
    int ptype = entity_type(uid);
    if (ptype == 648 || ptype == 0)
        return;
    char typec[10];
    itoa(ptype, typec, 10);
    const char *pname = entity_names[ptype].data();
    ImGui::PushID(section);
    if (ImGui::Button(uidc))
    {
        g_last_id = uid;
        update_entity();
    }
    ImGui::SameLine();
    ImGui::Text(typec);
    ImGui::SameLine();
    ImGui::Text(pname);
    if (rembtn)
    {
        ImGui::SameLine();
        ImGui::PushID(uid);
        if (ImGui::Button("X(!)"))
            g_entity->remove_item(uid);
        ImGui::PopID();
    }
    ImGui::PopID();
}

void render_state(const char *label, int state)
{
    if (state == 0)
        ImGui::LabelText(label, "0 Flailing");
    else if (state == 1)
        ImGui::LabelText(label, "1 Standing");
    else if (state == 2)
        ImGui::LabelText(label, "2 Sitting");
    else if (state == 4)
        ImGui::LabelText(label, "4 Hanging");
    else if (state == 5)
        ImGui::LabelText(label, "5 Ducking");
    else if (state == 6)
        ImGui::LabelText(label, "6 Climbing");
    else if (state == 7)
        ImGui::LabelText(label, "7 Pushing");
    else if (state == 8)
        ImGui::LabelText(label, "8 Jumping");
    else if (state == 9)
        ImGui::LabelText(label, "9 Falling");
    else if (state == 10)
        ImGui::LabelText(label, "10 Dropping");
    else if (state == 12)
        ImGui::LabelText(label, "12 Attacking");
    else if (state == 17)
        ImGui::LabelText(label, "17 Throwing");
    else if (state == 18)
        ImGui::LabelText(label, "18 Stunned");
    else if (state == 19)
        ImGui::LabelText(label, "19 Entering");
    else if (state == 20)
        ImGui::LabelText(label, "20 Loading");
    else if (state == 21)
        ImGui::LabelText(label, "21 Exiting");
    else if (state == 22)
        ImGui::LabelText(label, "22 Dying");
    else
    {
        char statec[10];
        itoa(state, statec, 10);
        ImGui::LabelText(label, statec);
    }
}

void render_ai(const char *label, int state)
{
    if (state == 0)
        ImGui::LabelText(label, "0 Idling");
    else if (state == 1)
        ImGui::LabelText(label, "1 Walking");
    else if (state == 2)
        ImGui::LabelText(label, "2 Jumping");
    else if (state == 4)
        ImGui::LabelText(label, "4 Dead");
    else if (state == 5)
        ImGui::LabelText(label, "5 Jumping");
    else if (state == 6)
        ImGui::LabelText(label, "6 Attacking");
    else if (state == 7)
        ImGui::LabelText(label, "7 Meleeing");
    else if (state == 11)
        ImGui::LabelText(label, "11 Rolling");
    else
    {
        char statec[10];
        itoa(state, statec, 10);
        ImGui::LabelText(label, statec);
    }
}

void render_screen(const char *label, int state)
{
    if (state == 0)
        ImGui::LabelText(label, "0 Logo");
    else if (state == 1)
        ImGui::LabelText(label, "1 Intro");
    else if (state == 2)
        ImGui::LabelText(label, "2 Prologue");
    else if (state == 3)
        ImGui::LabelText(label, "3 Title");
    else if (state == 4)
        ImGui::LabelText(label, "4 Main menu");
    else if (state == 5)
        ImGui::LabelText(label, "5 Options");
    else if (state == 7)
        ImGui::LabelText(label, "7 Leaderboards");
    else if (state == 8)
        ImGui::LabelText(label, "8 Seed input");
    else if (state == 9)
        ImGui::LabelText(label, "9 Character select");
    else if (state == 10)
        ImGui::LabelText(label, "10 Team select");
    else if (state == 11)
        ImGui::LabelText(label, "11 Camp");
    else if (state == 12)
        ImGui::LabelText(label, "12 Level");
    else if (state == 13)
        ImGui::LabelText(label, "13 Level transition");
    else if (state == 14)
        ImGui::LabelText(label, "14 Death");
    else if (state == 15)
        ImGui::LabelText(label, "15 Spaceship");
    else if (state == 16)
        ImGui::LabelText(label, "16 Ending");
    else if (state == 17)
        ImGui::LabelText(label, "17 Credits");
    else if (state == 18)
        ImGui::LabelText(label, "18 Scores");
    else if (state == 19)
        ImGui::LabelText(label, "19 Constellation");
    else if (state == 20)
        ImGui::LabelText(label, "20 Recap");
    else if (state == 21)
        ImGui::LabelText(label, "21 Arena menu");
    else if (state == 25)
        ImGui::LabelText(label, "25 Arena intro");
    else if (state == 26)
        ImGui::LabelText(label, "26 Arena match");
    else if (state == 28)
        ImGui::LabelText(label, "28 Loading online");
    else if (state == 29)
        ImGui::LabelText(label, "28 Lobby");
    else
    {
        char statec[10];
        itoa(state, statec, 10);
        ImGui::LabelText(label, statec);
    }
}

const char *theme_name(int theme)
{
    if (theme < 1 || theme > 17)
        return "Crash City";
    return themes_short[theme - 1];
}

void render_entity_props()
{
    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.5f);
    ImGui::Checkbox("Lock to player one", &lock_player);
    if (lock_player)
    {
        if (!g_players.empty())
        {
            g_last_id = g_players.at(0)->uid;
        }
    }
    ImGui::InputInt("Set UID", &g_last_id);
    ImGui::SameLine();
    ImGui::Checkbox("Sticky", &lock_entity);
    ImGui::PopItemWidth();
    if (!update_entity())
        return;
    if (g_entity == 0)
        return;
    ImGui::PushItemWidth(-ImGui::GetWindowWidth() * 0.5f);
    render_uid(g_entity->uid, "EntityGeneral");
    ImGui::SameLine();
    if (ImGui::Button("Void(!)##DeleteEntity"))
    {
        if (g_entity->overlay)
        {
            Movable *mount = (Movable *)g_entity->overlay;
            if (mount->holding_uid == g_entity->uid)
            {
                mount->holding_uid = -1;
            }
        }
        g_entity->overlay = nullptr;
        g_entity->y -= 1000.0;
    }
    if (ImGui::CollapsingHeader("State"))
    {
        render_state("Current state", g_entity->state);
        render_state("Last state", g_entity->last_state);
        render_ai("AI state", g_entity->move_state);
        if (g_entity->standing_on_uid != -1)
        {
            ImGui::Text("Standing on:");
            render_uid(g_entity->standing_on_uid, "StateStanding");
        }
        if (g_entity->holding_uid != -1)
        {
            ImGui::Text("Holding:");
            ImGui::SameLine();
            if (ImGui::Button("Drop(!)##DropHolding"))
            {
                Movable *holding = entity_ptr(g_entity->holding_uid);
                holding->x = g_entity->x;
                holding->y = g_entity->y;
                holding->overlay = 0;
                g_entity->holding_uid = -1;
            }
            render_uid(g_entity->holding_uid, "StateHolding");
        }
        auto *overlay = (Movable *)g_entity->overlay;
        if (!IsBadReadPtr(overlay, 0x178))
        {
            ImGui::Text("Riding:");
            ImGui::SameLine();
            if (ImGui::Button("Unmount##UnmountRiding"))
            {
                auto *mount = (Movable *)g_entity->overlay;
                if (mount->holding_uid == g_entity->uid)
                {
                    mount->holding_uid = -1;
                }
                g_entity->x = mount->x;
                g_entity->y = mount->y;
                g_entity->overlay = 0;
            }
            render_uid(overlay->uid, "StateRiding");
        }
        if (g_entity->last_owner_uid != -1)
        {
            ImGui::Text("Owner / Attacker:");
            ImGui::SameLine();
            if (ImGui::Button("Remove##RemoveOwner"))
            {
                g_entity->owner_uid = -1;
                g_entity->last_owner_uid = -1;
            }
            render_uid(g_entity->last_owner_uid, "StateOwner");
        }
    }
    if (ImGui::CollapsingHeader("Position"))
    {
        ImGui::InputFloat("Position X##EntityPositionX", &g_entity->x, 0.2, 1.0);
        ImGui::InputFloat("Position Y##EntityPositionX", &g_entity->y, 0.2, 1.0);
        ImGui::InputFloat("Velocity X##EntityVelocityX", &g_entity->velocityx, 0.2, 1.0);
        ImGui::InputFloat("Velocity y##EntityVelocityY", &g_entity->velocityy, 0.2, 1.0);
    }
    if (ImGui::CollapsingHeader("Stats"))
    {
        ImGui::DragScalar("Health##EntityHealth", ImGuiDataType_U8, (char *)&g_entity->health, 0.5f, &u8_one, &u8_max);
        if (g_inventory != 0)
        {
            ImGui::DragScalar("Bombs##EntityBombs", ImGuiDataType_U8, (char *)&g_inventory->bombs, 0.5f, &u8_one, &u8_max);
            ImGui::DragScalar("Ropes##EntityRopes", ImGuiDataType_U8, (char *)&g_inventory->ropes, 0.5f, &u8_one, &u8_max);
            ImGui::DragInt("Money##EntityMoney", (int *)&g_inventory->money, 20.0f, INT_MIN, INT_MAX, "%d");
            ImGui::DragInt("Level kills##EntityLevelKills", (int *)&g_inventory->kills_level, 0.5f, 0, INT_MAX, "%d");
            ImGui::DragInt("Total kills##EntityTotalKills", (int *)&g_inventory->kills_total, 0.5f, 0, INT_MAX, "%d");
        }
    }
    if (ImGui::CollapsingHeader("Items"))
    {
        if (g_entity->items_count > 0)
        {
            int *pitems = (int *)g_entity->items_ptr;
            for (int i = 0; i < g_entity->items_count; i++)
            {
                render_uid(pitems[i], "EntityItems", true);
            }
        }
    }
    if (ImGui::CollapsingHeader("Global attributes") && g_entity->type)
    {
        ImGui::DragScalar("Damage##GlobalDamage", ImGuiDataType_U8, (char *)&g_entity->type->damage, 0.5f, &u8_one, &u8_max);
        ImGui::DragScalar("Health##GlobalLife", ImGuiDataType_U8, (char *)&g_entity->type->life, 0.5f, &u8_one, &u8_max);
        ImGui::DragFloat("Friction##GlobalFriction", &g_entity->type->friction, 0.01f, 0.0f, 10.0f, "%.5f");
        ImGui::DragFloat("Elasticity##GlobalElasticity", &g_entity->type->elasticity, 0.01f, 0.0f, 10.0f, "%.5f");
        ImGui::DragFloat("Weight##GlobalWeight", &g_entity->type->weight, 0.01f, 0.0f, 10.0f, "%.5f");
        ImGui::DragFloat("Acceleration##GlobalAcceleration", &g_entity->type->acceleration, 0.01f, 0.0f, 10.0f, "%.5f");
        ImGui::DragFloat("Max speed##GlobalMaxSpeed", &g_entity->type->max_speed, 0.01f, 0.0f, 10.0f, "%.5f");
        ImGui::DragFloat("Sprint factor##GlobalSprintFactor", &g_entity->type->sprint_factor, 0.01f, 0.0f, 10.0f, "%.5f");
        ImGui::DragFloat("Jump power##GlobalJumpPower", &g_entity->type->jump, 0.01f, 0.0f, 10.0f, "%.5f");
        ImGui::InputScalar("Search flags##SearchFlags", ImGuiDataType_U32, &g_entity->type->search_flags, 0, 0, "%p", ImGuiInputTextFlags_ReadOnly);
    }
    if (ImGui::CollapsingHeader("Special attributes"))
    {
        if (g_entity_type == 435)
        {
            ImGui::Text("Character in coffin:");
            ImGui::SliderInt("##CoffinSpawns", (int *)&g_entity->inside, 194, 216);
            if (g_entity->inside == 214)
                g_entity->inside = 215;
            ImGui::SameLine();
            ImGui::Text(entity_names[g_entity->inside].data());
            ImGui::InputScalar("Timer##CoffinTimer", ImGuiDataType_U32, (int *)&g_entity->i12c, 0, 0, "%lld", ImGuiInputTextFlags_ReadOnly);
        }
        else if (g_entity_type == 402 || g_entity_type == 422 || g_entity_type == 423 || g_entity_type == 475)
        {
            ImGui::Text("Item in container:");
            ImGui::InputInt("##EntitySpawns", (int *)&g_entity->inside, 1, 10);
            if (g_entity->inside > 0)
            {
                ImGui::SameLine();
                ImGui::Text(entity_names[g_entity->inside].data());
            }
        }
        else if (g_entity_type == 575)
        {
            ImGui::SliderInt("Uses left##MattockUses", (int *)&g_entity->inside, 1, 255);
        }
        else if (g_entity_type == 23 || g_entity_type == 25 || g_entity_type == 31 || g_entity_type == 36)
        {
            Target *target = reinterpret_cast<Target *>(&g_entity->anim_func);
            ImGui::Text("Door target:");
            ImGui::Checkbox("Enabled##DoorEnabled", (bool *)&target->enabled);
            ImGui::DragScalar("World##DoorWorldnumber", ImGuiDataType_U8, &target->world, 0.5f, &u8_one, &u8_max);
            ImGui::DragScalar("Level##DoorLevelnumber", ImGuiDataType_U8, &target->level, 0.5f, &u8_one, &u8_max);
            ImGui::DragScalar("Theme##DoorThemenumber", ImGuiDataType_U8, &target->theme, 0.2f, &u8_one, &u8_seventeen);
            ImGui::SameLine();
            ImGui::Text(theme_name(target->theme));
        }
        else
        {
            ImGui::InputScalar(
                "Data##UnknownSpecialAttribute",
                ImGuiDataType_U64,
                (size_t *)&g_entity->inside,
                0,
                0,
                "%p",
                ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AlwaysInsertMode | ImGuiInputTextFlags_CharsHexadecimal);
        }
    }
    if (ImGui::CollapsingHeader("Style"))
    {
        ImGui::ColorEdit4("Color", (float *)&g_entity->color);
        ImGui::DragFloat("Width##EntityWidth", &g_entity->w, 0.5f, 0.0, 10.0, "%.3f");
        ImGui::DragFloat("Height##EntityHeight", &g_entity->h, 0.5f, 0.0, 10.0, "%.3f");
        ImGui::DragFloat("Box width##EntityBoxWidth", &g_entity->hitboxx, 0.5f, 0.0, 10.0, "%.3f");
        ImGui::DragFloat("Box height##EntityBoxHeight", &g_entity->hitboxy, 0.5f, 0.0, 10.0, "%.3f");
        ImGui::DragFloat("Offset X##EntityOffsetX", &g_entity->offsetx, 0.5f, -10.0, 10.0, "%.3f");
        ImGui::DragFloat("Offset Y##EntityOffsetY", &g_entity->offsety, 0.5f, -10.0, 10.0, "%.3f");
    }
    if (ImGui::CollapsingHeader("Flags"))
    {
        for (int i = 0; i < 32; i++)
        {
            ImGui::CheckboxFlags(entity_flags[i], &g_entity->flags, pow(2, i));
        }
    }
    if (ImGui::CollapsingHeader("More Flags"))
    {
        for (int i = 0; i < 32; i++)
        {
            ImGui::CheckboxFlags(more_flags[i], &g_entity->more_flags, pow(2, i));
        }
    }
    if (ImGui::CollapsingHeader("Input Display"))
    {
        ImVec2 region = ImGui::GetContentRegionMax();
        bool dirs[4] = {false, false, false, false};
        if (g_entity->movex < 0.0)
            dirs[0] = true;
        if (g_entity->movey < 0.0)
            dirs[1] = true;
        if (g_entity->movey > 0.0)
            dirs[2] = true;
        if (g_entity->movex > 0.0)
            dirs[3] = true;
        for (int i = 0; i < 4; i++)
        {
            ImGui::Checkbox(direction_flags[i], &dirs[i]);
            if (i < 3)
                ImGui::SameLine(region.x / 4 * (i + 1));
        }
        for (int i = 0; i < 6; i++)
        {
            ImGui::CheckboxFlags(button_flags[i], &g_entity->buttons, pow(2, i));
            if (i < 5)
                ImGui::SameLine(region.x / 6 * (i + 1));
        }
    }
    ImGui::PopItemWidth();
}

std::string format_time(int frames)
{
    time_t secs = frames / 60;
    char time[10];
    std::strftime(time, sizeof(time), "%H:%M:%S", std::gmtime(&secs));
    return std::string(time);
}

int parse_time(std::string time)
{
    std::tm tm = {};
    std::stringstream ss(time);
    ss >> std::get_time(&tm, "%H:%M:%S");
    return 60 * (tm.tm_hour * 60 * 60 + tm.tm_min * 60 + tm.tm_sec);
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
}

void render_timer()
{
    int frames = g_state->time_total;
    time_t secs = frames / 60;
    char time[10];
    std::strftime(time, sizeof(time), "%H:%M:%S", std::gmtime(&secs));
    std::stringstream ss;
    ss << "Total: " << time << "." << std::setfill('0') << std::setw(3) << floor((frames % 60) * (1000.0 / 60.0));
    ImGui::PushFont(bigfont);
    ImGui::Text(ss.str().data());
    ImGui::PopFont();
}

void render_players()
{
    update_players();
    for (auto player : g_players)
    {
        render_uid(player->uid, "players");
    }
}

void render_game_props()
{
    if (g_state == 0)
        return;
    ImGui::PushItemWidth(-ImGui::GetWindowWidth() * 0.5f);
    if (ImGui::CollapsingHeader("State"))
    {
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
        ImGui::LabelText("Game state", gamestate.data());
        if (ImGui::Checkbox("Pause game engine##PauseSim", &paused))
        {
            if (paused)
                g_state->pause = 0x20;
            else
                g_state->pause = 0;
        }
    }
    if (ImGui::CollapsingHeader("Timer"))
    {
        render_timer();
        std::string lasttime = format_time(g_state->time_last_level);
        std::string leveltime = format_time(g_state->time_level);
        std::string totaltime = format_time(g_state->time_total);
        ImGui::Text("Frz");
        ImGui::Checkbox("##FreezeLast", &freeze_last);
        ImGui::SameLine();
        if (InputString("Last level##LastTime", &lasttime))
        {
            g_last_time = parse_time(lasttime);
            g_state->time_last_level = parse_time(lasttime);
        }
        ImGui::Checkbox("##FreezeLevel", &freeze_level);
        ImGui::SameLine();
        if (InputString("Level##LevelTime", &leveltime))
        {
            g_level_time = parse_time(leveltime);
            g_state->time_level = parse_time(leveltime);
        }
        ImGui::Checkbox("##FreezeTotal", &freeze_total);
        ImGui::SameLine();
        if (InputString("Total##TotalTime", &totaltime))
        {
            g_total_time = parse_time(totaltime);
            g_state->time_total = g_total_time;
        }
    }
    if (ImGui::CollapsingHeader("Level"))
    {
        ImGui::InputInt2("Level size##LevelSize", (int *)&g_state->w, ImGuiInputTextFlags_ReadOnly);
        ImGui::DragScalar("World##Worldnumber", ImGuiDataType_U8, (char *)&g_state->world, 0.5f, &u8_one, &u8_max);
        ImGui::DragScalar("Level##Levelnumber", ImGuiDataType_U8, (char *)&g_state->level, 0.5f, &u8_one, &u8_max);
        ImGui::DragScalar("Theme ##Themenumber", ImGuiDataType_U8, (char *)&g_state->theme, 0.2f, &u8_one, &u8_seventeen);
        ImGui::SameLine();
        ImGui::Text(theme_name(g_state->theme));
        ImGui::DragScalar("To World##Worldnext", ImGuiDataType_U8, (char *)&g_state->world_next, 0.5f, &u8_one, &u8_max);
        ImGui::DragScalar("To Level##Levelnext", ImGuiDataType_U8, (char *)&g_state->level_next, 0.5f, &u8_one, &u8_max);
        ImGui::DragScalar("To Theme##Themenext", ImGuiDataType_U8, (char *)&g_state->theme_next, 0.2f, &u8_one, &u8_seventeen);
        ImGui::SameLine();
        ImGui::Text(theme_name(g_state->theme_next));
        ImGui::DragScalar("Levels completed##LevelsCompleted", ImGuiDataType_U8, (char *)&g_state->level_count, 0.5f, &u8_zero, &u8_max);
        if (ImGui::Checkbox("Force dark level##ToggleDarkMode", &dark_mode))
        {
            darkmode(dark_mode);
        }
    }
    if (ImGui::CollapsingHeader("Street cred"))
    {
        ImGui::DragScalar("Shoppie aggro##ShoppieAggro", ImGuiDataType_U8, &g_state->shoppie_aggro, 0.5f, &u8_min, &u8_max, "%d");
        ImGui::DragScalar("Shoppie aggro levels##ShoppieAggroLevels", ImGuiDataType_U8, &g_state->shoppie_aggro_levels, 0.5f, &u8_min, &u8_max, "%d");
        ImGui::DragScalar("Tun aggro##MerchantAggro", ImGuiDataType_U8, &g_state->merchant_aggro, 0.5f, &u8_min, &u8_max, "%d");
        ImGui::DragScalar("NPC kills##NPCKills", ImGuiDataType_U8, (char *)&g_state->kills_npc, 0.5f, &u8_zero, &u8_max);
        ImGui::DragScalar("Kali favor##PorFavor", ImGuiDataType_S8, (char *)&g_state->kali_favor, 0.5f, &s8_min, &s8_max);
        ImGui::DragScalar("Kali status##KaliStatus", ImGuiDataType_S8, (char *)&g_state->kali_status, 0.5f, &s8_min, &s8_max);
        ImGui::DragScalar("Altars destroyed##KaliAltars", ImGuiDataType_S8, (char *)&g_state->kali_altars_destroyed, 0.5f, &s8_min, &s8_max);
    }
    if (ImGui::CollapsingHeader("Players"))
    {
        render_players();
    }
    if (ImGui::CollapsingHeader("Level flags"))
    {
        for (int i = 0; i < 32; i++)
        {
            ImGui::CheckboxFlags(hud_flags[i], &g_state->hud_flags, pow(2, i));
        }
    }
    if (ImGui::CollapsingHeader("Journal flags"))
    {
        for (int i = 0; i < 21; i++)
        {
            ImGui::CheckboxFlags(journal_flags[i], &g_state->journal_flags, pow(2, i));
        }
    }
    ImGui::PopItemWidth();
}

void render_style_editor()
{
    ImGuiStyle &style = ImGui::GetStyle();
    ImGuiIO &io = ImGui::GetIO();
    ImGui::DragFloat("Hue##StyleHue", &g_hue, 0.01, 0.0, 1.0);
    ImGui::DragFloat("Saturation##StyleSaturation", &g_sat, 0.01, 0.0, 1.0);
    ImGui::DragFloat("Lightness##StyleLightness", &g_val, 0.01, 0.0, 1.0);
    ImGui::DragFloat("Alpha##StyleAlpha", &style.Alpha, 0.01, 0.2, 1.0);
    ImGui::DragFloat("Scale##StyleScale", &io.FontGlobalScale, 0.01, 0.2, 2.0);
    if (ImGui::Button("Randomize##StyleRandomize"))
    {
        ImGuiStyle &style = ImGui::GetStyle();
        g_hue = (float)rand() / RAND_MAX;
        g_sat = (float)rand() / RAND_MAX;
        g_val = (float)rand() / RAND_MAX;
        style.Alpha = (float)rand() / RAND_MAX * 0.5 + 0.4;
    }
    ImGui::SameLine();
    if (ImGui::Button("Save##StyleSave"))
    {
        save_config(cfgfile);
    }
    ImGui::SameLine();
    if (ImGui::Button("Load##StyleLoad"))
    {
        load_config(cfgfile);
        refresh_script_files();
    }
    set_colors();
}

void render_spawner()
{
    ImGui::Text("Spawning at x: %+.2f, y: %+.2f", g_x, g_y);
    render_input();
    render_list();
}

void create_render_target()
{
    ID3D11Texture2D *pBackBuffer;
    pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pBackBuffer);
    pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
    pBackBuffer->Release();
}

void cleanup_render_target()
{
    if (mainRenderTargetView)
    {
        mainRenderTargetView->Release();
        mainRenderTargetView = NULL;
    }
}

bool process_resizing(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
    static bool on_titlebar = false;
    switch (nCode)
    {
    case WM_NCLBUTTONDOWN:
        return on_titlebar = true;
    case WM_LBUTTONUP:
        if (on_titlebar && GetCapture() == window)
        {
            on_titlebar = false;
            return true;
        }
        break;
    case WM_SIZE:
        // When display mode is changed
        if (pDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            cleanup_render_target();
            pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            create_render_target();
        }
        break;
    }
    return false;
}

void render_tool(std::string tool)
{
    if (tool == "tool_entity")
        render_spawner();
    else if (tool == "tool_door")
        render_narnia();
    else if (tool == "tool_camera")
        render_camera();
    else if (tool == "tool_entity_properties")
        render_entity_props();
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
}

HRESULT __stdcall hkPresent(IDXGISwapChain *pSwapChain, UINT SyncInterval, UINT Flags)
{
    static bool init = false;
    // https://github.com/Rebzzel/kiero/blob/master/METHODSTABLE.txt#L249

    if (!init)
    {
        if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void **)&pDevice)))
        {
            pDevice->GetImmediateContext(&pContext);
            DXGI_SWAP_CHAIN_DESC sd;
            pSwapChain->GetDesc(&sd);
            window = sd.OutputWindow;
            gamewindow = window;
            orig_wndproc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(window, GWLP_WNDPROC, LONG_PTR(hkWndProc)));
            create_render_target();
            init_imgui();
            init = true;
        }
        else
        {
            return oPresent(pSwapChain, SyncInterval, Flags);
        }
        ImGuiIO &io = ImGui::GetIO();
        io.FontAllowUserScaling = true;
        PWSTR fontdir;
        if (SHGetKnownFolderPath(FOLDERID_Fonts, 0, NULL, &fontdir) == S_OK)
        {
            using cvt_type = std::codecvt_utf8<wchar_t>;
            std::wstring_convert<cvt_type, wchar_t> cvt;

            std::string fontpath(cvt.to_bytes(fontdir) + "\\segoeuib.ttf");
            if (GetFileAttributesA(fontpath.c_str()) != INVALID_FILE_ATTRIBUTES)
            {
                font = io.Fonts->AddFontFromFileTTF(fontpath.c_str(), 18.0f);
                bigfont = io.Fonts->AddFontFromFileTTF(fontpath.c_str(), 32.0f);
            }

            CoTaskMemFree(fontdir);
        }

        if (!font)
        {
            font = io.Fonts->AddFontDefault();
        }
        load_config(cfgfile);
        refresh_script_files();
        autorun_scripts();
        set_colors();
        windows["tool_entity"] = new Window({"Spawner (" + key_string(keys["tool_entity"]) + ")", false, true});
        windows["tool_door"] = new Window({"Door (" + key_string(keys["tool_door"]) + ")", false, true});
        windows["tool_camera"] = new Window({"Camera (" + key_string(keys["tool_camera"]) + ")", false, true});
        windows["tool_entity_properties"] = new Window({"Entity (" + key_string(keys["tool_entity_properties"]) + ")", false, true});
        windows["tool_game_properties"] = new Window({"Game (" + key_string(keys["tool_game_properties"]) + ")", false, true});
        windows["tool_options"] = new Window({"Options (" + key_string(keys["tool_options"]) + ")", false, true});
        windows["tool_debug"] = new Window({"Debug (" + key_string(keys["tool_debug"]) + ")", false, false});
        windows["tool_style"] = new Window({"Style (" + key_string(keys["tool_style"]) + ")", false, false});
        windows["tool_script"] = new Window({"Scripts (" + key_string(keys["tool_script"]) + ")", false, false});
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowSize({-1, 20});
    ImGui::Begin(
        "Overlay",
        NULL,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, .3f), "Overlunky");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, .3f), TOSTRING(GIT_VERSION));
    ImGui::SetWindowPos({ImGui::GetIO().DisplaySize.x / 2 - ImGui::GetWindowWidth() / 2, ImGui::GetIO().DisplaySize.y - 30}, ImGuiCond_Always);
    ImGui::End();

    render_messages();
    render_clickhandler();

    int win_condition = ImGuiCond_FirstUseEver;
    if (options["stack_horizontally"] || options["stack_vertically"])
    {
        win_condition = ImGuiCond_Always;
    }
    float lastwidth = 0;
    float lastheight = 0;
    float toolwidth = 0.128 * ImGui::GetIO().DisplaySize.x * ImGui::GetIO().FontGlobalScale;
    if (!hide_ui)
    {
        if (options["tabbed_interface"])
        {
            ImGui::SetNextWindowPos({0, 0}, ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize({600, ImGui::GetIO().DisplaySize.y/2}, ImGuiCond_FirstUseEver);
            ImGui::Begin("Overlunky", NULL);
            if (ImGui::BeginTabBar("##TabBar"))
            {
                ImGuiTabItemFlags flags = 0;
                for (auto tab : tab_order)
                {
                    flags = 0;
                    if (activate_tab == tab)
                    {
                        flags = ImGuiTabItemFlags_SetSelected;
                        activate_tab = "";
                        active_tab = "";
                    }
                    if (!detached(tab) && ImGui::BeginTabItem(windows[tab]->name.data(), &windows[tab]->open, flags))
                    {
                        active_tab = tab;
                        render_tool(tab);
                        ImGui::EndTabItem();
                    }
                }
                ImGui::EndTabBar();
            }
            int tabnum = 0;
            for (auto window : windows)
            {
                if (window.second->open && !window.second->detached)
                    ++tabnum;
            }
            if (tabnum == 0)
            {
                ImGui::TextWrapped("Looks like you closed all your tabs. You can use the F-keys to open closed tabs or click here:");
                if(ImGui::Button("Restore tabs"))
                {
                    for (auto window : windows)
                    {
                        window.second->open = true;
                    }
                }
            }
            ImGui::End();

            for (auto tab : windows)
            {
                if(!tab.second->detached)
                    continue;
                ImGui::SetNextWindowSize({toolwidth, toolwidth}, ImGuiCond_Once);
                ImGui::Begin(tab.second->name.data(), &tab.second->detached);
                render_tool(tab.first);
                ImGui::SetWindowPos(
                {ImGui::GetIO().DisplaySize.x / 2 - ImGui::GetWindowWidth() / 2, ImGui::GetIO().DisplaySize.y / 2 - ImGui::GetWindowHeight() / 2}, ImGuiCond_Once);
                ImGui::End();
            }
        }
        else if (options["stack_vertically"])
        {
            ImGui::SetNextWindowSize({toolwidth, -1}, win_condition);
            ImGui::Begin(windows["tool_options"]->name.c_str());
            render_options();
            lastwidth += ImGui::GetWindowWidth();
            lastheight += ImGui::GetWindowHeight();
            ImGui::SetWindowPos({0, ImGui::GetIO().DisplaySize.y - lastheight}, win_condition);
            ImGui::End();

            ImGui::SetNextWindowSize({toolwidth, -1}, win_condition);
            ImGui::Begin(windows["tool_camera"]->name.c_str());
            render_camera();
            lastwidth += ImGui::GetWindowWidth();
            lastheight += ImGui::GetWindowHeight();
            ImGui::SetWindowPos({0, ImGui::GetIO().DisplaySize.y - lastheight}, win_condition);
            ImGui::End();

            ImGui::SetNextWindowSize({toolwidth, -1}, win_condition);
            ImGui::Begin(windows["tool_door"]->name.c_str());
            render_narnia();
            lastwidth += ImGui::GetWindowWidth();
            lastheight += ImGui::GetWindowHeight();
            ImGui::SetWindowPos({0, ImGui::GetIO().DisplaySize.y - lastheight}, win_condition);
            ImGui::End();

            ImGui::SetNextWindowSize({toolwidth, ImGui::GetIO().DisplaySize.y - lastheight}, win_condition);
            ImGui::Begin(windows["tool_entity"]->name.c_str());
            render_spawner();
            lastwidth += ImGui::GetWindowWidth();
            lastheight += ImGui::GetWindowHeight();
            ImGui::SetWindowPos({0, 0}, win_condition);
            ImGui::End();

            ImGui::SetNextWindowSize({toolwidth, ImGui::GetIO().DisplaySize.y / 3}, win_condition);
            ImGui::Begin(windows["tool_entity_properties"]->name.c_str());
            render_entity_props();
            lastwidth += ImGui::GetWindowWidth();
            lastheight += ImGui::GetWindowHeight();
            ImGui::SetWindowPos({ImGui::GetIO().DisplaySize.x - toolwidth, 0}, win_condition);
            ImGui::End();

            ImGui::SetNextWindowSize({toolwidth, ImGui::GetIO().DisplaySize.y / 3}, win_condition);
            ImGui::Begin(windows["tool_game_properties"]->name.c_str());
            render_game_props();
            lastwidth += ImGui::GetWindowWidth();
            lastheight += ImGui::GetWindowHeight();
            ImGui::SetWindowPos({ImGui::GetIO().DisplaySize.x - toolwidth, ImGui::GetIO().DisplaySize.y / 3}, win_condition);
            ImGui::End();

            ImGui::SetNextWindowSize({toolwidth, ImGui::GetIO().DisplaySize.y / 3}, win_condition);
            ImGui::Begin(windows["tool_script"]->name.c_str());
            render_scripts();
            lastwidth += ImGui::GetWindowWidth();
            lastheight += ImGui::GetWindowHeight();
            ImGui::SetWindowPos({ImGui::GetIO().DisplaySize.x - toolwidth, 2*ImGui::GetIO().DisplaySize.y / 3}, win_condition);
            ImGui::End();
        }
        else
        {
            ImGui::SetNextWindowSize({toolwidth, toolwidth}, win_condition);
            ImGui::SetNextWindowPos({0, 0}, win_condition);
            ImGui::Begin(windows["tool_entity"]->name.c_str());
            render_spawner();
            lastwidth += ImGui::GetWindowWidth();
            lastheight += ImGui::GetWindowHeight();
            ImGui::End();

            ImGui::SetNextWindowSize({toolwidth, -1}, win_condition);
            ImGui::SetNextWindowPos({lastwidth, 0}, win_condition);
            ImGui::Begin(windows["tool_door"]->name.c_str());
            render_narnia();
            lastwidth += ImGui::GetWindowWidth();
            lastheight += ImGui::GetWindowHeight();
            ImGui::End();

            ImGui::SetNextWindowSize({toolwidth, -1}, win_condition);
            ImGui::SetNextWindowPos({lastwidth, 0}, win_condition);
            ImGui::Begin(windows["tool_camera"]->name.c_str());
            render_camera();
            lastwidth += ImGui::GetWindowWidth();
            lastheight += ImGui::GetWindowHeight();
            ImGui::End();

            ImGui::SetNextWindowSize({toolwidth, -1}, win_condition);
            ImGui::SetNextWindowPos({lastwidth, 0}, win_condition);
            ImGui::Begin(windows["tool_entity_properties"]->name.c_str());
            render_entity_props();
            lastwidth += ImGui::GetWindowWidth();
            lastheight += ImGui::GetWindowHeight();
            ImGui::End();

            ImGui::SetNextWindowSize({toolwidth, -1}, win_condition);
            ImGui::SetNextWindowPos({lastwidth, 0}, win_condition);
            ImGui::Begin(windows["tool_game_properties"]->name.c_str());
            render_game_props();
            lastwidth += ImGui::GetWindowWidth();
            lastheight += ImGui::GetWindowHeight();
            ImGui::End();

            ImGui::SetNextWindowSize({toolwidth, -1}, win_condition);
            ImGui::SetNextWindowPos({ImGui::GetIO().DisplaySize.x - toolwidth, 0}, win_condition);
            ImGui::Begin(windows["tool_options"]->name.c_str());
            render_options();
            lastwidth = ImGui::GetWindowWidth();
            lastheight = ImGui::GetWindowHeight();
            ImGui::End();

            ImGui::SetNextWindowSize({toolwidth, -1}, win_condition);
            ImGui::SetNextWindowPos({ImGui::GetIO().DisplaySize.x - toolwidth * 2, 0}, win_condition);
            ImGui::Begin(windows["tool_script"]->name.c_str());
            render_scripts();
            ImGui::End();
        }

        if (show_debug && !options["tabbed_interface"])
        {
            ImGui::SetNextWindowSize({toolwidth, -1}, win_condition);
            ImGui::SetNextWindowPos({ImGui::GetIO().DisplaySize.x - toolwidth * 3, 0}, win_condition);
            ImGui::Begin(windows["tool_debug"]->name.c_str(), &show_debug);
            render_debug();
            ImGui::End();
        }

        if (change_colors && !options["tabbed_interface"])
        {
            ImGui::Begin(windows["tool_style"]->name.c_str(), &change_colors);
            ImGui::SetWindowSize({-1, -1}, ImGuiCond_Always);
            render_style_editor();
            ImGui::SetWindowPos(
                {ImGui::GetIO().DisplaySize.x / 2 - ImGui::GetWindowWidth() / 2, ImGui::GetIO().DisplaySize.y / 2 - ImGui::GetWindowHeight() / 2});
            ImGui::End();
        }
    }

    if (show_app_metrics)
    {
        ImGui::ShowMetricsWindow(&show_app_metrics);
        ImGui::Begin("Styles");
        ImGui::ShowStyleEditor();
        ImGui::End();
    }

    ImGui::Render();

    if (!file_written)
        write_file();

    if (options["disable_input"] && capture_last == false && ImGui::GetIO().WantCaptureKeyboard)
    {
        HID_RegisterDevice(window, HID_KEYBOARD);
        capture_last = true;
    }
    else if (capture_last == true && !ImGui::GetIO().WantCaptureKeyboard)
    {
        capture_last = false;
        register_keys = ImGui::GetFrameCount() + 10;
    }
    else if (register_keys && ImGui::GetFrameCount() > register_keys)
    {
        register_keys = 0;
        gamewindow = FindTopWindow(GetCurrentProcessId());
        HID_RegisterDevice(gamewindow, HID_KEYBOARD);
    }

    if (options["disable_input_alt"] && ImGui::GetIO().WantCaptureKeyboard)
    {
        g_players = get_players();
        for (auto player : g_players)
        {
            player->more_flags |= 1U << 15;
        }
        if (g_state != 0)
            g_state->hud_flags &= ~(1U << 19);
        register_keys_alt = ImGui::GetFrameCount() + ImGui::GetIO().Framerate / 10;
    }
    else if (!ImGui::GetIO().WantCaptureKeyboard && register_keys_alt && ImGui::GetFrameCount() > register_keys_alt)
    {
        register_keys_alt = 0;
        for (auto player : g_players)
        {
            player->more_flags &= ~(1U << 15);
        }
        if (g_state != 0 && !options["disable_pause"])
            g_state->hud_flags |= 1U << 19;
    }

    pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    update_players();
    force_zoom();
    force_hud_flags();
    force_time();
    force_noclip();

    return oPresent(pSwapChain, SyncInterval, Flags);
}

void create_box(std::vector<EntityItem> items)
{
    std::vector<EntityItem> new_items(items);
    new_items.emplace(new_items.begin(), "ENT_TYPE_Select entity to spawn:", 0); // :D

    std::sort(new_items.begin(), new_items.end());

    std::vector<int> new_filtered_items(new_items.size());
    for (int i = 0; i < new_items.size(); i++)
    {
        new_filtered_items[i] = i;
        entity_names[new_items[i].id] = new_items[i].name.substr(9);
    }

    // TODO: add atomic and wrap it as struct
    {
        g_current_item = 0;
        g_items = new_items;
        g_filtered_items = new_filtered_items;
        g_filtered_count = g_items.size();
    }
}

#define THROW(fmt, ...)                                                                                                                              \
    {                                                                                                                                                \
        char buf[0x1000];                                                                                                                            \
        snprintf(buf, sizeof(buf), fmt, __VA_ARGS__);                                                                                                \
        throw std::runtime_error(strdup(buf));                                                                                                       \
    }

template <typename T> PresentPtr &vtable_find(T *obj, int index)
{
    void ***ptr = reinterpret_cast<void ***>(obj);
    if (!ptr[0])
        return *static_cast<PresentPtr *>(nullptr);
    return *reinterpret_cast<PresentPtr *>(&ptr[0][index]);
}

bool init_hooks(size_t _ptr)
{
    g_state = (struct StateMemory *)get_state_ptr();
    g_state_addr = reinterpret_cast<uintptr_t>(g_state);

    pSwapChain = reinterpret_cast<IDXGISwapChain *>(_ptr);
    PresentPtr &ptr = vtable_find(pSwapChain, 8);
    DWORD oldProtect;
    if (!VirtualProtect(reinterpret_cast<LPVOID>(reinterpret_cast<uintptr_t>(&ptr) & ~0xFFF), 0x1000, PAGE_READWRITE, &oldProtect))
    {
        PANIC("VirtualProtect error: {:#x}\n", GetLastError());
    }

    if (!ptr)
    {
        PANIC("DirectX 11 is not initialized yet.");
    }

    oPresent = ptr;
    ptr = hkPresent;
    return true;
}
