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
#include <regex>
#include <string>

#pragma warning(push, 0)
#include <toml.hpp>
#pragma warning(pop)
#include <fmt/chrono.h>
#include <fmt/core.h>

#include "olfont.h"

#include "console.hpp"
#include "custom_types.hpp"
#include "entities_chars.hpp"
#include "entities_floors.hpp"
#include "entities_items.hpp"
#include "entities_logical.hpp"
#include "entities_mounts.hpp"
#include "file_api.hpp"
#include "flags.hpp"
#include "game_manager.hpp"
#include "items.hpp"
#include "logger.h"
#include "math.hpp"
#include "savedata.hpp"
#include "screen.hpp"
#include "script.hpp"
#include "sound_manager.hpp" // TODO: remove from here?
#include "state.hpp"
#include "version.hpp"
#include "window_api.hpp"

#include "decode_audio_file.hpp"

#pragma warning(disable : 4366)

template <class T>
concept Script = std::is_same_v<T, SpelunkyConsole> || std::is_same_v<T, SpelunkyScript>;

std::unique_ptr<SoundManager> g_SoundManager;

std::unique_ptr<SpelunkyConsole> g_Console;
std::deque<ScriptMessage> g_ConsoleMessages;

std::map<std::string, std::unique_ptr<SpelunkyScript>> g_scripts;
std::map<std::string, std::unique_ptr<SpelunkyScript>> g_ui_scripts;
std::vector<std::filesystem::path> g_script_files;
std::vector<std::string> g_script_autorun;

std::map<std::string, int64_t> keys{
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
    {"toggle_snap", OL_KEY_CTRL | 'S'},
    {"toggle_pause", OL_KEY_CTRL | VK_SPACE},
    {"toggle_disable_pause", OL_KEY_CTRL | OL_KEY_SHIFT | 'P'},
    {"toggle_grid", OL_KEY_CTRL | OL_KEY_SHIFT | 'G'},
    {"toggle_hitboxes", OL_KEY_CTRL | OL_KEY_SHIFT | 'H'},
    {"toggle_hud", OL_KEY_CTRL | 'H'},
    {"toggle_lights", OL_KEY_CTRL | 'L'},
    {"toggle_ghost", OL_KEY_CTRL | 'O'},
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
    {"spawn_layer_door", OL_KEY_SHIFT | VK_RETURN},
    {"spawn_warp_door", OL_KEY_CTRL | OL_KEY_SHIFT | VK_RETURN},
    {"destroy_grabbed", VK_DELETE},
    {"destroy_selected", OL_KEY_ALT | VK_DELETE},
    {"warp", OL_KEY_CTRL | 'W'},
    {"warp_next_level_a", OL_KEY_CTRL | 'A'},
    {"warp_next_level_b", OL_KEY_CTRL | 'B'},
    {"hide_ui", VK_F11},
    {"zoom_in", OL_KEY_CTRL | VK_OEM_COMMA},
    {"zoom_out", OL_KEY_CTRL | VK_OEM_PERIOD},
    {"zoom_default", OL_KEY_CTRL | '2'},
    {"zoom_3x", OL_KEY_CTRL | '3'},
    {"zoom_4x", OL_KEY_CTRL | '4'},
    {"zoom_5x", OL_KEY_CTRL | '5'},
    {"zoom_auto", OL_KEY_CTRL | '0'},
    {"teleport", 0x0},
    {"teleport_left", 0x0},
    {"teleport_up", 0x0},
    {"teleport_right", 0x0},
    {"teleport_down", 0x0},
    {"camera_left", OL_KEY_SHIFT | VK_LEFT},
    {"camera_up", OL_KEY_SHIFT | VK_UP},
    {"camera_right", OL_KEY_SHIFT | VK_RIGHT},
    {"camera_down", OL_KEY_SHIFT | VK_DOWN},
    {"coordinate_left", 0x0},
    {"coordinate_up", 0x0},
    {"coordinate_right", 0x0},
    {"coordinate_down", 0x0},
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
    {"quick_camp", OL_KEY_CTRL | 'C'},
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
    //{ "", 0x },
};

struct Window
{
    std::string name;
    bool detached;
    bool open;
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

float g_x = 0, g_y = 0, g_vx = 0, g_vy = 0, g_dx = 0, g_dy = 0, g_zoom = 13.5f, g_hue = 0.63f, g_sat = 0.66f, g_val = 0.66f, g_camera_speed = 1.0f;
ImVec2 startpos;
int g_held_id = -1, g_last_id = -1, g_over_id = -1, g_current_item = 0, g_filtered_count = 0, g_last_frame = 0,
    g_last_gun = 0, g_last_time = -1, g_level_time = -1, g_total_time = -1, g_pause_time = -1,
    g_force_width = 0, g_force_height = 0, g_pause_at = -1, g_hitbox_mask = 0x80BF, g_last_type = -1;
unsigned int g_level_width = 0, grid_x = 0, grid_y = 0;
uint8_t g_level = 1, g_world = 1, g_to = 0;
uint32_t g_held_flags = 0, g_dark_mode = 0;
std::vector<EntityItem> g_items;
std::vector<int> g_filtered_items;
std::vector<std::string> saved_entities;
std::vector<Player*> g_players;
std::vector<uint32_t> g_selected_ids;
bool set_focus_entity = false, set_focus_world = false, set_focus_zoom = false, set_focus_finder = false, scroll_to_entity = false, scroll_top = false, click_teleport = false,
     throw_held = false, paused = false, show_app_metrics = false, lock_entity = false, lock_player = false,
     freeze_last = false, freeze_level = false, freeze_total = false, hide_ui = false,
     enable_noclip = false, load_script_dir = true, load_packs_dir = false, enable_camp_camera = true, enable_camera_bounds = true, freeze_quest_yang = false, freeze_quest_sisters = false, freeze_quest_horsing = false, freeze_quest_sparrow = false, freeze_quest_tusk = false, freeze_quest_beg = false, run_finder = false;
std::optional<int8_t> quest_yang_state, quest_sisters_state, quest_horsing_state, quest_sparrow_state, quest_tusk_state, quest_beg_state;
Entity* g_entity = 0;
Entity* g_held_entity = 0;
StateMemory* g_state = 0;
SaveData* g_save = 0;
GameManager* g_game_manager = 0;
std::map<int, std::string> entity_names;
std::map<int, std::string> entity_full_names;
std::string active_tab = "", activate_tab = "", detach_tab = "";
std::vector<std::string> tab_order = {"tool_entity", "tool_door", "tool_camera", "tool_entity_properties", "tool_game_properties", "tool_save", "tool_finder", "tool_script", "tool_options", "tool_style", "tool_keys", "tool_debug"};
std::vector<std::string> tabs_open = {"tool_entity", "tool_door", "tool_camera", "tool_entity_properties", "tool_game_properties", "tool_finder"};
std::vector<std::string> tabs_detached = {};

std::string text;
std::string g_change_key = "";

const char* inifile = "imgui.ini";
const std::string cfgfile = "overlunky.ini";
std::string scriptpath = "Overlunky/Scripts";

std::string fontfile = "";
std::vector<float> fontsize = {14.0f, 32.0f, 72.0f};

[[maybe_unused]] const char s8_zero = 0, s8_one = 1, s8_min = -128, s8_max = 127;
[[maybe_unused]] const ImU8 u8_zero = 0, u8_one = 1, u8_min = 0, u8_max = 255, u8_four = 4, u8_seven = 7, u8_seventeen = 17, u8_draw_depth_max = 53;
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

std::map<std::string, bool> options = {
    {"mouse_control", true},
    {"keyboard_control", true},
    {"god_mode", false},
    {"god_mode_companions", false},
    {"noclip", false},
    {"snap_to_grid", false},
    {"spawn_floor_decorated", true},
    {"disable_pause", false},
    {"draw_grid", false},
    {"draw_hitboxes", false},
    {"enable_unsafe_scripts", false},
    {"warp_increments_level_count", true},
    {"warp_transition", false},
    {"lights", false},
    {"disable_achievements", true},
    {"disable_savegame", true},
    {"disable_ghost_timer", false},
    {"draw_hud", true},
    {"draw_script_messages", true},
    {"fade_script_messages", true},
    {"console_script_messages", false},
    {"draw_hitboxes_interpolated", true},
    {"show_tooltips", true},
    {"smooth_camera", true},
    {"multi_viewports", true},
    {"menu_ui", true}};

bool g_speedhack_hooked = false;
float g_speedhack_multiplier = 1.0;
float g_speedhack_old_multiplier = 1.0;
LARGE_INTEGER g_speedhack_prev;
LARGE_INTEGER g_speedhack_current;
LARGE_INTEGER g_speedhack_fake;
PVOID g_oldqpc;

#define PtrFromRva(base, rva) (((PBYTE)base) + rva)

// I didn't write this one, I just found it in the shady parts of the internet
// This could probably be done with detours and speedhack should be part of the api anyway...
BOOL HookIAT(const char* szModuleName, const char* szFuncName, PVOID pNewFunc, PVOID* pOldFunc)
{
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)GetModuleHandle(NULL);
    PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)PtrFromRva(pDosHeader, pDosHeader->e_lfanew);

    // Make sure we have valid data
    if (pNtHeader->Signature != IMAGE_NT_SIGNATURE)
        return FALSE;

    // Grab a pointer to the import data directory
    PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)PtrFromRva(pDosHeader, pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

    for (UINT uIndex = 0; pImportDescriptor[uIndex].Characteristics != 0; uIndex++)
    {
        char* szDllName = (char*)PtrFromRva(pDosHeader, pImportDescriptor[uIndex].Name);

        // Is this our module?
        if (_strcmpi(szDllName, szModuleName) != 0)
            continue;

        if (!pImportDescriptor[uIndex].FirstThunk || !pImportDescriptor[uIndex].OriginalFirstThunk)
            return FALSE;

        PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)PtrFromRva(pDosHeader, pImportDescriptor[uIndex].FirstThunk);
        PIMAGE_THUNK_DATA pOrigThunk = (PIMAGE_THUNK_DATA)PtrFromRva(pDosHeader, pImportDescriptor[uIndex].OriginalFirstThunk);

        for (; pOrigThunk->u1.Function != NULL; pOrigThunk++, pThunk++)
        {
            // We can't process ordinal imports just named
            if (pOrigThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG)
                continue;

            PIMAGE_IMPORT_BY_NAME import = (PIMAGE_IMPORT_BY_NAME)PtrFromRva(pDosHeader, pOrigThunk->u1.AddressOfData);

            // Is this our function?
            if (_strcmpi(szFuncName, (char*)import->Name) != 0)
                continue;

            DWORD dwJunk = 0;
            MEMORY_BASIC_INFORMATION mbi;

            // Make the memory section writable
            VirtualQuery(pThunk, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
            if (!VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &mbi.Protect))
                return FALSE;

            // Save the old pointer
            *pOldFunc = (PVOID*)(DWORD_PTR)pThunk->u1.Function;

// Write the new pointer based on CPU type
#ifdef _WIN64
            pThunk->u1.Function = (ULONGLONG)(DWORD_PTR)pNewFunc;
#else
            pThunk->u1.Function = (DWORD)(DWORD_PTR)pNewFunc;
#endif

            if (VirtualProtect(mbi.BaseAddress, mbi.RegionSize, mbi.Protect, &dwJunk))
                return TRUE;
        }
    }
    return FALSE;
}

bool __stdcall QueryPerformanceCounterHook(LARGE_INTEGER* counter)
{
    QueryPerformanceCounter(&g_speedhack_current);
    g_speedhack_fake.QuadPart += (long long)((g_speedhack_current.QuadPart - g_speedhack_prev.QuadPart) * g_speedhack_multiplier);
    g_speedhack_prev = g_speedhack_current;
    *counter = g_speedhack_fake;
    return true;
}

void speedhack(float multiplier = g_speedhack_multiplier)
{
    g_speedhack_multiplier = multiplier;
    if (!g_speedhack_hooked)
    {
        QueryPerformanceCounter(&g_speedhack_prev);
        g_speedhack_fake = g_speedhack_prev;
        HookIAT("kernel32.dll", "QueryPerformanceCounter", QueryPerformanceCounterHook, &g_oldqpc);
        g_speedhack_hooked = true;
    }
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
    float col_main_sat = g_sat;
    float col_main_val = g_val;
    float col_area_sat = g_sat * 0.77f;
    float col_area_val = g_val * 0.60f;
    float col_back_sat = g_sat * 0.33f;
    float col_back_val = g_val * 0.20f;

    ImGuiStyle& style = ImGui::GetStyle();

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
    style.Colors[ImGuiCol_DockingPreview] = ImVec4(col_area.x, col_area.y, col_area.z, 0.6f);
    style.WindowPadding = ImVec2(4, 4);
    style.WindowRounding = 0;
    style.FrameRounding = 0;
    style.PopupRounding = 0;
    style.GrabRounding = 0;
    style.TabRounding = 0;
    style.ScrollbarRounding = 0;
    style.WindowBorderSize = 0;
    style.FrameBorderSize = 0;
    style.PopupBorderSize = 0;
    style.DisplaySafeAreaPadding = {0, 0};
}

void load_script(std::string file, bool enable = true)
{
    std::ifstream data(file);
    std::ostringstream buf;
    if (!data.fail())
    {
        buf << data.rdbuf();
        /*size_t slash = file.find_last_of("/\\");
        if (slash != std::string::npos)
            file = file.substr(slash + 1);*/
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
            name = "Mystery key";
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
    if (keycode & OL_KEY_CTRL)
    {
        name = "Ctrl+" + name;
    }
    if (keycode & OL_KEY_ALT)
    {
        name = "Alt+" + name;
    }
    return name;
}

bool SliderByte(const char* label, char* value, char min = 0, char max = 0, const char* format = "%lld")
{
    return ImGui::SliderScalar(label, ImGuiDataType_U8, value, &min, &max, format);
}

void refresh_script_files()
{
    std::regex luareg("\\.lua$", std::regex_constants::icase);
    std::regex mainluareg("^main\\.lua$", std::regex_constants::icase);
    g_script_files.clear();
    if (load_script_dir && std::filesystem::exists(scriptpath) && std::filesystem::is_directory(scriptpath))
    {
        for (const auto& file : std::filesystem::directory_iterator(scriptpath))
        {
            if (std::regex_search(file.path().string(), luareg))
            {
                g_script_files.push_back(file.path());
            }
        }
    }
    else if (!load_script_dir && std::filesystem::exists(scriptpath) && std::filesystem::is_directory(scriptpath))
    {
        std::vector<std::string> unload_scripts;
        for (const auto& script : g_scripts)
        {
            if (!script.second->is_enabled() && std::filesystem::equivalent(std::filesystem::path(script.second->get_path()), std::filesystem::path(scriptpath)))
            {
                unload_scripts.push_back(script.second->get_file());
            }
        }
        for (auto id : unload_scripts)
        {
            auto it = g_scripts.find(id);
            if (it != g_scripts.end())
                g_scripts.erase(id);
        }
    }

    if (load_packs_dir && std::filesystem::exists("Mods/Packs") && std::filesystem::is_directory("Mods/Packs"))
    {
        for (const auto& file : std::filesystem::recursive_directory_iterator("Mods/Packs"))
        {
            if (std::regex_search(file.path().filename().string(), mainluareg))
            {
                g_script_files.push_back(file.path());
            }
        }
    }
    else if (!load_packs_dir && std::filesystem::exists("Mods/Packs") && std::filesystem::is_directory("Mods/Packs"))
    {
        std::vector<std::string> unload_scripts;
        for (const auto& script : g_scripts)
        {
            if (!script.second->is_enabled() && std::filesystem::equivalent(std::filesystem::path(script.second->get_path()).parent_path(), std::filesystem::path("Mods/Packs")))
            {
                unload_scripts.push_back(script.second->get_file());
            }
        }
        for (auto id : unload_scripts)
        {
            auto it = g_scripts.find(id);
            if (it != g_scripts.end())
                g_scripts.erase(id);
        }
    }

    for (auto file : g_script_files)
    {
        load_script(file.string(), false);
    }
}

void autorun_scripts()
{
    for (auto file : g_script_autorun)
    {
        std::string script = scriptpath + "/" + file;
        if (std::filesystem::exists(script) && std::filesystem::is_regular_file(script))
        {
            load_script(script, true);
        }
    }
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

    writeData << "kits = [";
    for (unsigned int i = 0; i < saved_entities.size(); i++)
    {
        writeData << std::endl
                  << "  \"" << saved_entities[i] << "\"";
        if (i < saved_entities.size() - 1)
            writeData << ",";
    }
    if (!saved_entities.empty())
        writeData << std::endl;
    writeData << "]" << std::endl;

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
    for (auto [name, window] : windows)
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
    saved_entities = toml::find_or<std::vector<std::string>>(opts, "kits", {});
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
    if (options["multi_viewports"])
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    else
        ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable;
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
        activate_tab = tool;
        return true;
    }
    else
    {
        ImGuiWindow* win = ImGui::FindWindowByName("Overlunky");
        if (win)
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
    ImGuiContext& g = *GImGui;
    ImGuiWindow* current = g.NavWindow;
    if (current == NULL)
        return false;
    // while (current->ParentWindow != NULL)
    //     current = current->ParentWindow;
    if (detached(window))
    {
        if (windows.find(window) == windows.end())
            return false;
        return current == ImGui::FindWindowByName(windows[window]->name.c_str());
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
    saved_entities.push_back(text);
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
    if ((ent->type->id >= first_door && ent->type->id <= first_door + 15) || ent->type->id == logical_door)
    {
        auto pos = ent->position();
        auto layer = (LAYER)ent->layer;
        UI::cleanup_at(pos.first, pos.second, layer, ent->type->id);
    }
    if (ent->type->search_flags & 0x180)
    {
        auto pos = ent->position();
        auto layer = (LAYER)ent->layer;
        ENT_TYPE type = ent->type->id;
        Callback cb = {g_state->time_total + 1, [pos, layer, type]
                       {
                           fix_decorations_at(std::round(pos.first), std::round(pos.second), layer);
                           UI::cleanup_at(std::round(pos.first), std::round(pos.second), layer, type);
                       }};
        callbacks.push_back(cb);
    }
    UI::safe_destroy(ent, unsafe);
}

void reset_windows()
{
    for (auto [name, window] : windows)
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

std::string spawned_type()
{
    const auto pos = text.find_first_of(" ");
    if (pos == std::string::npos && g_filtered_count > 0)
    {
        auto to_spawn = g_items[g_filtered_items[g_current_item]];
        if (g_current_item == 0 && (unsigned)g_filtered_count == g_items.size())
        {
            if (g_entity)
            {
                return entity_full_names[g_entity->type->id];
            }
            else if (g_last_type >= 0)
            {
                return entity_full_names[g_last_type];
            }
        }
        else if (g_current_item > 0 && g_filtered_count > 0)
        {
            to_spawn = g_items[g_filtered_items[g_current_item]];
            return to_spawn.name;
        }
        return "";
    }
    else
    {
        return text;
    }
}

void spawn_entities(bool s, std::string list = "")
{
    if (g_game_manager->pause_ui->visibility > 0)
        return;

    const auto pos = text.find_first_of(" ");
    if (list == "" && pos == std::string::npos && g_filtered_count > 0)
    {
        auto to_spawn = g_items[g_filtered_items[g_current_item]];
        if (g_current_item == 0 && (unsigned)g_filtered_count == g_items.size())
        {
            if (g_entity)
            {
                to_spawn = EntityItem{entity_full_names[g_entity->type->id], g_entity->type->id};
            }
            else if (g_last_type >= 0)
            {
                to_spawn = EntityItem{entity_full_names[g_last_type], (uint32_t)g_last_type};
            }
            else
            {
                return;
            }
        }
        std::pair<float, float> cpos = UI::click_position(g_x, g_y);
        if (to_spawn.name.find("ENT_TYPE_CHAR") != std::string::npos)
        {
            int spawned = UI::spawn_companion(to_spawn.id, cpos.first, cpos.second, LAYER::PLAYER);
            if (!lock_entity)
                g_last_id = spawned;
        }
        else if (to_spawn.name.find("ENT_TYPE_LIQUID") == std::string::npos)
        {
            bool snap = options["snap_to_grid"];
            if (to_spawn.name.find("ENT_TYPE_FLOOR") != std::string::npos)
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
            if (to_spawn.name.find("ENT_TYPE_FLOOR") != std::string::npos && options["spawn_floor_decorated"])
            {
                if (Floor* floor = get_entity_ptr(spawned)->as<Floor>())
                {
                    if (floor->get_decoration_entity_type() != -1)
                    {
                        floor->fix_decorations(true, false);
                    }
                    auto fpos = floor->position();
                    auto layer = (LAYER)floor->layer;
                    Callback cb = {g_state->time_total + 2, [fpos, layer]
                                   {
                                       fix_decorations_at(fpos.first, fpos.second, layer);
                                   }};
                    callbacks.push_back(cb);
                }
            }
            if (!lock_entity)
                g_last_id = spawned;
        }
        else
        {
            UI::spawn_liquid(to_spawn.id, cpos.first, cpos.second);
        }
    }
    else
    {
        std::stringstream textss(text);
        if (list != "")
            textss.str(list);
        int id;
        std::vector<int> ents;
        int spawned{-1};
        while (textss >> id)
        {
            spawned = UI::spawn_entity(id, g_x, g_y, s, g_vx, g_vy, options["snap_to_grid"]);
        }
        if (!lock_entity)
            g_last_id = spawned;
    }
}

void spawn_entity_over()
{
    if (g_filtered_count > 0)
    {
        if (g_current_item == 0 && (unsigned)g_filtered_count == g_items.size())
            return;
        if (g_items[g_filtered_items[g_current_item]].name.find("ENT_TYPE_LIQUID") == std::string::npos)
        {
            int spawned = UI::spawn_entity_over(g_items[g_filtered_items[g_current_item]].id, g_over_id, g_dx, g_dy);
            if (!lock_entity)
                g_last_id = spawned;
        }
        else
        {
            auto cpos = UI::click_position(g_x, g_y);
            auto mpos = normalize(mouse_pos());
            auto cpos2 = UI::click_position(mpos.x, mpos.y);
            g_last_id = g_state->next_entity_uid;
            UI::spawn_liquid(g_items[g_filtered_items[g_current_item]].id, cpos.first + 0.3f, cpos.second + 0.3f, 2 * (cpos2.first - cpos.first), 2 * (cpos2.second - cpos.second), 0, 1, INFINITY);
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
        {
            g_state->illumination = UI::create_illumination(Color::white(), 20000.0f, 172, 252);
        }
        if (g_state->illumination)
        {
            g_state->illumination->enabled = true;
            if (g_state->camera_layer == 1)
                g_state->illumination->flags |= 1U << 16;
            else
                g_state->illumination->flags &= ~(1U << 16);
        }
    }
    else
    {
        if (g_state->illumination && test_flag(g_state->level_flags, 18))
            g_state->illumination->enabled = false;
    }
}

void set_camera_bounds(bool enabled)
{
    if (enabled)
    {
        g_state->camera->bounds_left = 0.5f;
        g_state->camera->bounds_right = g_state->w * 10.0f + 4.5f;
        g_state->camera->bounds_top = 124.5f;
        g_state->camera->bounds_bottom = 120.5f - g_state->h * 8.0f;
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
            if (player->overlay)
                player->overlay->remove_item(player->uid);
            player->overlay = NULL;
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
}

void force_noclip()
{
    g_players = UI::get_players();
    if (options["noclip"])
    {
        for (auto ent : g_players)
        {
            auto player = (Movable*)(ent->topmost_mount());
            if (player->overlay)
            {
                if (player->state == 6 && (player->movex != 0 || player->movey != 0))
                {
                    auto [x, y] = UI::get_position(player);
                    player->teleport_abs(x + player->movex * 0.3f, y + player->movey * 0.07f, 0, 0);
                }
                else
                    player->overlay->remove_item(player->uid);
            }
            player->overlay = NULL;
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
                    player->teleport_abs(cpos.first, cpos.second, player->velocityx, player->velocityy);
                }
            }
        }
    }
}

void frame_advance()
{
    if (g_state->pause == 0 && g_pause_at != -1 && (unsigned)g_pause_at <= UI::get_frame_count())
    {
        g_state->pause = 0x2;
        g_pause_at = -1;
    }
}

void quick_start(uint8_t screen, uint8_t world, uint8_t level, uint8_t theme)
{
    static const auto ana_spelunky = to_id("ENT_TYPE_CHAR_ANA_SPELUNKY");
    const auto ana_texture = get_type(ana_spelunky)->texture;

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
    g_state->quest_flags = g_state->quest_flags | 1;
    g_state->fadein = 1;
    g_state->fadeout = 1;
    g_state->loading = 1;
}

void warp_inc(uint8_t w, uint8_t l, uint8_t t)
{
    if (options["warp_increments_level_count"] && g_state->screen == 12)
    {
        g_state->level_count += 1;
    }
    if (options["warp_transition"])
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

    for (auto doorid : UI::get_entities_by({(ENT_TYPE)CUSTOM_TYPE::EXITDOOR}, 0x100, LAYER::BOTH))
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

bool pressed(std::string keyname, WPARAM wParam)
{
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
    return wParam == (unsigned)keycode;
}

bool clicked(std::string keyname)
{
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
    return wParam == keycode;
}

bool dblclicked(std::string keyname)
{
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
    return wParam == keycode;
}

bool held(std::string keyname)
{
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
    return wParam == keycode;
}

bool released(std::string keyname)
{
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
    return wParam == keycode;
}

bool dragging(std::string keyname)
{
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
    return wParam == keycode;
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

bool process_keys(UINT nCode, WPARAM wParam, [[maybe_unused]] LPARAM lParam)
{
    ImGuiContext& g = *GImGui;

    if (nCode == WM_KEYUP)
    {
        if (pressed("speedhack_turbo", wParam))
        {
            speedhack(g_speedhack_old_multiplier);
            g_speedhack_old_multiplier = 1.0f;
        }
        else if (pressed("speedhack_slow", wParam))
        {
            speedhack(g_speedhack_old_multiplier);
            g_speedhack_old_multiplier = 1.0f;
        }
    }

    if (nCode != WM_KEYDOWN && nCode != WM_SYSKEYDOWN)
    {
        return false;
    }

    int repeat = (lParam >> 30) & 1U;
    auto& io = ImGui::GetIO();
    ImGuiWindow* current = g.NavWindow;

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

    if (pressed("hide_ui", wParam))
    {
        hide_ui = !hide_ui;
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
        toggle("tool_entity_properties");
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
    else if (pressed("enter", wParam) && active("tool_entity") && io.WantCaptureKeyboard)
    {
        spawn_entities(false);
        return true;
    }
    else if (pressed("enter", wParam) && active("tool_finder") && io.WantCaptureKeyboard)
    {
        run_finder = true;
        return true;
    }
    else if (pressed("move_up", wParam) && active("tool_door") && io.WantCaptureKeyboard)
    {
        g_to = static_cast<uint8_t>(std::min(std::max(g_to - 1, 0), 15));
    }
    else if (pressed("move_down", wParam) && active("tool_door") && io.WantCaptureKeyboard)
    {
        g_to = static_cast<uint8_t>(std::min(std::max(g_to + 1, 0), 15));
    }
    else if (pressed("enter", wParam) && active("tool_door") && io.WantCaptureKeyboard)
    {
        int spawned = UI::spawn_door(0.0, 0.0, g_world, g_level, g_to + 1);
        if (!lock_entity)
            g_last_id = spawned;
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
    else if (pressed("zoom_default", wParam))
    {
        g_zoom = 13.5f;
        set_zoom();
    }
    else if (pressed("zoom_3x", wParam))
    {
        g_zoom = 23.08f;
        set_zoom();
    }
    else if (pressed("zoom_4x", wParam))
    {
        g_zoom = 29.87f;
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
    else if (pressed("toggle_godmode", wParam))
    {
        options["god_mode"] = !options["god_mode"];
        UI::godmode(options["god_mode"]);
    }
    else if (pressed("toggle_noclip", wParam))
    {
        options["noclip"] = !options["noclip"];
        toggle_noclip();
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
        g_pause_at = -1;
        if (g_state->pause == 0)
        {
            g_state->pause = 0x2;
            paused = true;
        }
        else
        {
            g_state->pause = 0;
            paused = false;
        }
    }
    else if (pressed("toggle_hud", wParam))
    {
        options["draw_hud"] = !options["draw_hud"];
    }
    else if (pressed("frame_advance", wParam) || pressed("frame_advance_alt", wParam))
    {
        if (g_state->pause == 0x2)
        {
            g_pause_at = UI::get_frame_count() + 1;
            g_state->pause = 0;
        }
    }
    else if (pressed("toggle_disable_pause", wParam))
    {
        options["disable_pause"] = !options["disable_pause"];
        force_hud_flags();
    }
    else if (pressed("toggle_lights", wParam))
    {
        options["lights"] = !options["lights"];
        if (options["lights"] && g_state->illumination)
        {
            g_state->illumination->flags |= (1U << 24);
        }
        else if (!options["lights"] && g_state->illumination)
        {
            g_state->illumination->flags &= ~(1U << 16);
            if ((g_state->level_flags & (1U << 17)) > 0)
                g_state->illumination->flags &= ~(1U << 24);
        }
    }
    else if (pressed("toggle_ghost", wParam))
    {
        options["disable_ghost_timer"] = !options["disable_ghost_timer"];
        UI::set_time_ghost_enabled(!options["disable_ghost_timer"]);
        UI::set_time_jelly_enabled(!options["disable_ghost_timer"]);
        UI::set_cursepot_ghost_enabled(!options["disable_ghost_timer"]);
    }
    else if (pressed("teleport_left", wParam))
    {
        UI::teleport(-3, 0, false, 0, 0, options["snap_to_grid"]);
    }
    else if (pressed("teleport_right", wParam))
    {
        UI::teleport(3, 0, false, 0, 0, options["snap_to_grid"]);
    }
    else if (pressed("teleport_up", wParam))
    {
        UI::teleport(0, 3, false, 0, 0, options["snap_to_grid"]);
    }
    else if (pressed("teleport_down", wParam))
    {
        UI::teleport(0, -3, false, 0, 0, options["snap_to_grid"]);
    }
    else if (pressed("spawn_layer_door", wParam))
    {
        UI::spawn_backdoor(0.0, 0.0);
    }
    else if (pressed("teleport", wParam))
    {
        UI::teleport(g_x, g_y, false, 0, 0, options["snap_to_grid"]);
    }
    else if (pressed("camera_left", wParam))
    {
        if (g_state->camera->focused_entity_uid == -1)
            g_state->camera->focus_x -= 0.2f;
        if (g_state->pause != 0 || !options["smooth_camera"])
            g_state->camera->adjusted_focus_x = g_state->camera->focus_x;
    }
    else if (pressed("camera_right", wParam))
    {
        if (g_state->camera->focused_entity_uid == -1)
            g_state->camera->focus_x += 0.2f;
        if (g_state->pause != 0 || !options["smooth_camera"])
            g_state->camera->adjusted_focus_x = g_state->camera->focus_x;
    }
    else if (pressed("camera_up", wParam))
    {
        if (g_state->camera->focused_entity_uid == -1)
            g_state->camera->focus_y += 0.2f;
        if (g_state->pause != 0 || !options["smooth_camera"])
            g_state->camera->adjusted_focus_y = g_state->camera->focus_y;
    }
    else if (pressed("camera_down", wParam))
    {
        if (g_state->camera->focused_entity_uid == -1)
            g_state->camera->focus_y -= 0.2f;
        if (g_state->pause != 0 || !options["smooth_camera"])
            g_state->camera->adjusted_focus_y = g_state->camera->focus_y;
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
    else if (pressed("spawn_kit_1", wParam))
    {
        if (saved_entities.size() > 0)
            spawn_entities(false, saved_entities.at(0));
    }
    else if (pressed("spawn_kit_2", wParam))
    {
        if (saved_entities.size() > 1)
            spawn_entities(false, saved_entities.at(1));
    }
    else if (pressed("spawn_kit_3", wParam))
    {
        if (saved_entities.size() > 2)
            spawn_entities(false, saved_entities.at(2));
    }
    else if (pressed("spawn_kit_4", wParam))
    {
        if (saved_entities.size() > 3)
            spawn_entities(false, saved_entities.at(3));
    }
    else if (pressed("spawn_kit_5", wParam))
    {
        if (saved_entities.size() > 4)
            spawn_entities(false, saved_entities.at(4));
    }
    else if (pressed("spawn_kit_6", wParam))
    {
        if (saved_entities.size() > 5)
            spawn_entities(false, saved_entities.at(5));
    }
    else if (pressed("spawn_kit_7", wParam))
    {
        if (saved_entities.size() > 6)
            spawn_entities(false, saved_entities.at(6));
    }
    else if (pressed("spawn_kit_8", wParam))
    {
        if (saved_entities.size() > 7)
            spawn_entities(false, saved_entities.at(7));
    }
    else if (pressed("spawn_kit_9", wParam))
    {
        if (saved_entities.size() > 8)
            spawn_entities(false, saved_entities.at(8));
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
            g_players.at(0)->set_layer(layer_to);
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
            quick_start(12, g_state->world_start, g_state->level_start, g_state->theme_start);
        }
        else
        {
            quick_start(12, 1, 1, 1);
        }
    }
    else if (pressed("quick_camp", wParam))
    {
        quick_start(11, 1, 1, 1);
    }
    else if (pressed("speedhack_increase", wParam))
    {
        g_speedhack_multiplier += 0.1f;
        if (g_speedhack_multiplier > 10.f)
            g_speedhack_multiplier = 10.f;
        speedhack();
    }
    else if (pressed("speedhack_decrease", wParam))
    {
        g_speedhack_multiplier -= 0.1f;
        if (g_speedhack_multiplier < 0.1f)
            g_speedhack_multiplier = 0.1f;
        speedhack();
    }
    else if (pressed("speedhack_10pct", wParam))
    {
        g_speedhack_multiplier = 0.1f;
        speedhack(0.1f);
    }
    else if (pressed("speedhack_20pct", wParam))
    {
        speedhack(0.2f);
    }
    else if (pressed("speedhack_30pct", wParam))
    {
        speedhack(0.3f);
    }
    else if (pressed("speedhack_40pct", wParam))
    {
        speedhack(0.4f);
    }
    else if (pressed("speedhack_50pct", wParam))
    {
        speedhack(0.5f);
    }
    else if (pressed("speedhack_60pct", wParam))
    {
        speedhack(0.6f);
    }
    else if (pressed("speedhack_70pct", wParam))
    {
        speedhack(0.7f);
    }
    else if (pressed("speedhack_80pct", wParam))
    {
        speedhack(0.8f);
    }
    else if (pressed("speedhack_90pct", wParam))
    {
        speedhack(0.9f);
    }
    else if (pressed("speedhack_normal", wParam))
    {
        speedhack(1.0f);
    }
    else if (pressed("speedhack_turbo", wParam) && !repeat)
    {
        g_speedhack_old_multiplier = g_speedhack_multiplier;
        speedhack(5.f); // TODO: configurable
    }
    else if (pressed("speedhack_slow", wParam) && !repeat)
    {
        g_speedhack_old_multiplier = g_speedhack_multiplier;
        speedhack(0.2f); // TODO: configurable
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
    else
    {
        return false;
    }
    return true;
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

void tooltip(const char* tip)
{
    if (!options["show_tooltips"])
        return;
    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("%s", tip);
    }
}

void tooltip(const char* tip, const char* key)
{
    if (!options["show_tooltips"])
        return;
    if (ImGui::IsItemHovered())
    {
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

void render_uid(int uid, const char* section, bool rembtn = false)
{
    std::string uidc = std::to_string(uid);
    auto ptype = UI::get_entity_type(uid);
    if (ptype == 0 || ptype == UINT32_MAX)
        return;
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
    if (rembtn)
    {
        ImGui::SameLine();
        ImGui::PushID(uid);
        if (ImGui::Button("X"))
            g_entity->remove_item(uid);
        ImGui::PopID();
    }
    ImGui::PopID();
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
    for (int i = 0; i < 11; i++)
    {
        ImGui::CheckboxFlags(illumination_flags[i], &light->flags, (int)std::pow(2, i));
    }
    ImGui::PopID();
}

void render_liquid_pool(int i)
{
    ImGui::PushID(i);
    if (ImGui::CollapsingHeader(liquid_pool_names[i]))
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
    }
    ImGui::PopID();
}

void render_list()
{
    // ImGui::ListBox with filter
    ImVec2 boxsize = {-1, -1};
    if (options["menu_ui"] && !detached("tool_entity"))
        boxsize = {400.0f, 400.0f};
    if (!ImGui::ListBoxHeader("##Entities", boxsize))
        return;
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
            std::string item_name = g_items[g_filtered_items[i]].name.c_str();
            std::string item_concat = item_id + ": " + item_name.substr(9);
            const char* item_text = item_concat.c_str();
            ImGui::PushID(i);
            if (ImGui::Selectable(item_text, item_selected))
            {
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

void render_input()
{
    int n = 0;
    for (auto i : saved_entities)
    {
        ImGui::PushID(i.c_str());
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
        tooltip("Delete kit.");
        ImGui::SameLine();
        ImGui::PopID();

        ImGui::PushID(4 * n);
        if (ImGui::Button("Load"))
        {
            text = i;
            update_filter(text);
            // spawn_entities(false);
        }
        tooltip("Edit kit or spawn later with mouse.");
        ImGui::SameLine();
        ImGui::PopID();

        ImGui::PushID(8 * n);
        if (ImGui::Button("Spawn"))
        {
            spawn_entities(false, i);
        }
        tooltip("Spawn saved kit where you're standing.", "spawn_kit_1");
        ImGui::PopID();
        ImGui::SameLine();
        ImGui::Text("%d:", n + 1);
        ImGui::PopID();
        ImGui::SameLine();
        ImGui::TextWrapped("%s", search.c_str());
        n++;
    }
    if (set_focus_entity)
    {
        ImGui::SetKeyboardFocusHere();
        set_focus_entity = false;
    }
    ImVec2 region = ImGui::GetContentRegionMax();
    ImGui::PushItemWidth(region.x - 110);
    if (ImGui::InputText("##Input", &text, ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_AutoSelectAll, pick_selected_entity))
    {
        update_filter(text);
    }
    tooltip("Search for entities to spawn. Hit TAB to add the selected id to list.");
    ImGui::PopItemWidth();
    ImGui::SameLine();
    if (ImGui::Button("Save"))
    {
        save_search();
    }
    tooltip("Save entity id(s) as a kit for quick use later.");
    ImGui::SameLine();
    if (ImGui::Button("Spawn"))
    {
        spawn_entities(false);
    }
    tooltip("Spawn selected entity where you're standing.", "spawn_entity");
}

const char* theme_name(int theme)
{
    if (theme < 1 || theme > 17)
        return "Crash City";
    return themes_short[theme - 1];
}

void render_narnia()
{
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

        for (auto doorid : UI::get_entities_by({(ENT_TYPE)CUSTOM_TYPE::EXITDOOR}, 0x100, LAYER::BOTH))
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
    if (ImGui::DragScalar("Level##WarpLevel", ImGuiDataType_U8, &g_level, 0.1f, &u8_one, &u8_four)) {}
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
    ImGui::Checkbox("Increment level count on warp", &options["warp_increments_level_count"]);
    tooltip("Simulate natural level progression when warping.");
    ImGui::Checkbox("Warp to transition instead", &options["warp_transition"]);
    tooltip("Simulate natural level progression even more.");
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
    tooltip("Zooming with the mouse wheel is easier.", "mouse_zoom_in");
    ImGui::PopItemWidth();
    if (ImGui::Button("Default"))
    {
        g_zoom = 13.5f;
        set_zoom();
    }
    tooltip("Default zoom level.", "zoom_default");
    ImGui::SameLine();
    if (ImGui::Button("3x"))
    {
        g_zoom = 23.08f;
        set_zoom();
    }
    tooltip("3 room wide zoom level.", "zoom_3x");
    ImGui::SameLine();
    if (ImGui::Button("4x"))
    {
        g_zoom = 29.87f;
        set_zoom();
    }
    tooltip("4 room wide zoom level.", "zoom_4x");
    ImGui::SameLine();
    if (ImGui::Button("5x"))
    {
        g_zoom = 36.66f;
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
    render_uid(g_state->camera->focused_entity_uid, "FocusedEntity");
    ImGui::SameLine();
    if (ImGui::Button("!X"))
        g_state->camera->focused_entity_uid = -1;
    tooltip("Remove camera focus.");
    ImGui::SameLine();
    if (ImGui::Button("Focus player"))
    {
        if (!g_players.empty())
        {
            g_state->camera->focused_entity_uid = g_players.at(0)->uid;
        }
    }
    tooltip("Doubleclick to return camera to player. Hold to drag camera around.", "mouse_camera_drag");
    ImGui::SameLine();
    if (ImGui::Button("Focus Selected"))
    {
        g_state->camera->focused_entity_uid = g_last_id;
    }
    tooltip("Click to focus other entities. Hold to move camera around.", "mouse_camera_drag");
    ImGui::InputFloat("Camera Focus X##CameraFocusX", &g_state->camera->focus_x, 0.2f, 1.0f);
    ImGui::InputFloat("Camera Focus Y##CameraFocusY", &g_state->camera->focus_y, 0.2f, 1.0f);
    ImGui::InputFloat("Camera Real X##CameraRealX", &g_state->camera->adjusted_focus_x, 0.2f, 1.0f);
    ImGui::InputFloat("Camera Real Y##CameraRealY", &g_state->camera->adjusted_focus_y, 0.2f, 1.0f);
    if (ImGui::CollapsingHeader("Camera Bounds"))
    {
        ImGui::InputFloat("Top##CameraBoundTop", &g_state->camera->bounds_top, 0.2f, 1.0f);
        ImGui::InputFloat("Bottom##CameraBoundBottom", &g_state->camera->bounds_bottom, 0.2f, 1.0f);
        ImGui::InputFloat("Left##CameraBoundLeft", &g_state->camera->bounds_left, 0.2f, 1.0f);
        ImGui::InputFloat("Right##CameraBoundRight", &g_state->camera->bounds_right, 0.2f, 1.0f);
        if (ImGui::Checkbox("Enable default camera bounds##CameraBoundsLevel", &enable_camera_bounds))
        {
            set_camera_bounds(enable_camera_bounds);
        }
        tooltip("Disable to free the camera bounds in a level.\nAutomatically disabled when dragging.");
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
    }
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

void render_hitbox(Entity* ent, bool cross, ImColor color, bool filled = false)
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
    ImVec2 sboxa = screenify({boxa_x, boxa_y});
    ImVec2 sboxb = screenify({boxb_x, boxb_y});
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
    if (ent->shape == SHAPE::CIRCLE)
        if (filled)
            draw_list->AddCircleFilled(fix_pos(spos), sboxb.x - spos.x, color);
        else
            draw_list->AddCircle(fix_pos(spos), sboxb.x - spos.x, color, 0, 2.0f);
    else if (filled)
        draw_list->AddRectFilled(fix_pos(sboxa), fix_pos(sboxb), color);
    else
        draw_list->AddRect(fix_pos(sboxa), fix_pos(sboxb), color, 0.0f, 0, 2.0f);

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

    if (type == spark_trap && ent->animation_frame == 7)
    {
        // TODO: get the real distance from game (can be changed thru API)
        auto [radx, rady] = UI::screen_position(render_position.first + 3, render_position.second + 3);
        auto srad = screenify({radx, rady});
        draw_list->AddCircle(fix_pos(spos), srad.x - spos.x, ImColor(255, 0, 0, 150), 0, 2.0f);
    }
    else if (type == bomb)
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

void update_script(Script auto* script)
{
    if (!script->is_enabled())
        return;
    script->run();
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
        std::vector<ScriptMessage> messages;
        for (auto& [name, script] : g_scripts)
        {
            for (auto&& message : script->consume_messages())
                messages.push_back(message);
            if (messages.size() > 0)
                g_Console->push_history(fmt::format("--- [{}] at {:%Y-%m-%d %X}", script->get_name(), time_buf), std::move(messages));
        }
        messages.clear();
        {
            for (auto&& message : g_Console->consume_messages())
                messages.push_back(message);
            if (messages.size() > 0)
                g_Console->push_history(fmt::format("--- [Console] at {:%Y-%m-%d %X}", time_buf), std::move(messages));
        }
        return;
    }

    using Message = std::tuple<std::string, std::string, std::chrono::time_point<std::chrono::system_clock>, ImVec4>;
    std::vector<Message> queue;
    for (auto& [name, script] : g_scripts)
    {
        for (auto message : script->get_messages())
        {
            if (options["fade_script_messages"] && now - 12s > message.time)
                continue;
            std::istringstream messages(message.message);
            while (!messages.eof())
            {
                std::string mline;
                getline(messages, mline);
                queue.push_back(std::make_tuple(script->get_name(), mline, message.time, message.color));
            }
        }
    }
    for (auto&& message : g_Console->consume_messages())
    {
        g_ConsoleMessages.push_back(std::move(message));
    }
    for (auto message : g_ConsoleMessages)
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
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDocking);

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
    for (auto message : queue)
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
    if (io.MouseWheel != 0 && ImGui::IsWindowHovered())
    {
        if (clicked("mouse_zoom_out") || (held("mouse_camera_drag") && io.MouseWheel < 0))
        {
            if (g_zoom == 0.0f)
                g_zoom = UI::get_zoom_level();
            g_zoom += g_zoom / 13.5f;
            set_zoom();
        }
        else if (clicked("mouse_zoom_in") || (held("mouse_camera_drag") && io.MouseWheel > 0))
        {
            if (g_zoom == 0.0f)
                g_zoom = UI::get_zoom_level();
            g_zoom -= g_zoom / 13.5f;
            set_zoom();
        }
    }
    if (options["draw_grid"])
    {
        render_grid();
    }
    if (options["draw_hitboxes"] && g_state->screen != 5)
    {
        static const auto olmec = to_id("ENT_TYPE_ACTIVEFLOOR_OLMEC");
        for (auto entity : UI::get_entities_by({}, g_hitbox_mask, (LAYER)g_state->camera_layer))
        {
            auto ent = get_entity_ptr(entity);
            if (!ent)
                continue;

            if (ent->type->id == olmec)
            {
                render_hitbox(ent, false, ImColor(0, 255, 255, 150));
                continue;
            }

            if (!UI::has_active_render(ent) && (ent->type->search_flags & 0x7000) == 0)
                continue;

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
            };
            for (auto entity : UI::get_entities_by(additional_fixed_entities, 0x180, LAYER::PLAYER)) // FLOOR | ACTIVEFLOOR
            {
                auto ent = get_entity_ptr(entity);
                render_hitbox(ent, false, ImColor(0, 255, 255, 150));
            }
            for (auto entity : UI::get_entities_by({(ENT_TYPE)CUSTOM_TYPE::TRIGGER}, 0x1000, LAYER::PLAYER)) // LOGICAL
            {
                auto ent = get_entity_ptr(entity);
                render_hitbox(ent, false, ImColor(255, 0, 0, 150));
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

        if (ImGui::IsMousePosValid())
        {
            ImVec2 mpos = normalize(mouse_pos());
            std::pair<float, float> cpos = UI::click_position(mpos.x, mpos.y);
            // std::pair<float, float> campos = get_camera_position();
            ImDrawList* dl = ImGui::GetBackgroundDrawList();
            std::string buf = fmt::format("{:.2f}, {:.2f}", cpos.first, cpos.second);
            // char buf2[32];
            // sprintf(buf2, "Camera: %0.2f, %0.2f", campos.first, campos.second);
            dl->AddText(ImVec2(io.MousePos.x + 16, io.MousePos.y), ImColor(1.0f, 1.0f, 1.0f, 1.0f), buf.c_str());
            // dl->AddText(ImVec2(io.MousePos.x + 16, io.MousePos.y + 16), ImColor(1.0f, 1.0f, 1.0f, 1.0f), buf2);
            unsigned int mask = safe_entity_mask;
            if (ImGui::GetIO().KeyShift) // TODO: Get the right modifier from mouse_destroy_unsafe
            {
                mask = unsafe_entity_mask;
            }
            auto hovered = UI::get_entity_at(cpos.first, cpos.second, false, 2, mask);
            if (hovered != nullptr)
            {
                render_hitbox(hovered, true, ImColor(50, 50, 255, 200));
                std::string buf3 = fmt::format("{}, {}", hovered->uid, entity_names[hovered->type->id]);
                dl->AddText(ImVec2(io.MousePos.x + 16, io.MousePos.y + 16), ImColor(1.0f, 1.0f, 1.0f, 1.0f), buf3.c_str());
            }
        }
    }

    if (update_entity())
    {
        render_hitbox(g_entity, true, ImColor(0, 255, 0, 200));
    }
    static auto front_col = ImColor(0, 255, 51, 100);
    static auto back_col = ImColor(255, 160, 31, 100);
    for (auto entity : g_selected_ids)
    {
        auto ent = get_entity_ptr(entity);
        if (ent)
        {
            if (ent->layer == g_state->camera_layer)
                render_hitbox(ent, false, front_col, true);
            else
                render_hitbox(ent, false, back_col, true);
        }
    }

    for (auto& [name, script] : g_scripts)
    {
        fix_script_requires(script.get());
    }
    fix_script_requires(g_Console.get());
    auto* draw_list = ImGui::GetBackgroundDrawList();
    for (auto& [name, script] : g_scripts)
    {
        update_script(script.get());
    }
    if (g_last_id > -1)
        g_Console.get()->set_selected_uid(g_last_id);
    update_script(g_Console.get());
    for (auto& [name, script] : g_scripts)
    {
        render_script(script.get(), draw_list);
    }
    render_script(g_Console.get(), draw_list);

    for (auto& [name, script] : g_ui_scripts)
    {
        fix_script_requires(script.get());
    }
    for (auto& [name, script] : g_ui_scripts)
    {
        update_script(script.get());
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
            ImVec2(io.DisplaySize.x / 2 - warningsize.x / 2, io.DisplaySize.y / 2 - warningsize.y / 2),
            ImColor(1.0f, 1.0f, 1.0f, 0.4f),
            warningtext);
        const char* subtext = "Probably... Some things might, but don't just expect a random script to work.";
        ImVec2 subsize = font->CalcTextSizeA(18.0, io.DisplaySize.x - 200, io.DisplaySize.x - 200, subtext);
        dl->AddText(
            font,
            18.0,
            ImVec2(io.DisplaySize.x / 2 - subsize.x / 2, io.DisplaySize.y / 2 + warningsize.y / 2 + 20),
            ImColor(1.0f, 1.0f, 1.0f, 0.4f),
            subtext);
    }
    if (options["mouse_control"] && UI::get_focus())
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
            player->teleport_abs(cpos.first, cpos.second, g_vx, g_vy);
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
            player->teleport_abs(cpos.first, cpos.second, g_vx, g_vy);
            g_x = 0;
            g_y = 0;
            g_vx = 0;
            g_vy = 0;
        }
        else if (dblclicked("mouse_grab") || dblclicked("mouse_grab_unsafe"))
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
                g_last_type = g_held_entity->type->id;
            }
            if (!lock_entity)
                g_last_id = g_held_id;
            g_current_item = 0;
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
                g_last_type = g_held_entity->type->id;
            }
            if (!lock_entity)
                g_last_id = g_held_id;
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
            if (g_held_entity && g_held_entity->is_movable())
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
            if (options["snap_to_grid"] && g_held_entity->is_movable())
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
            if (!g_players.empty())
            {
                g_state->camera->focused_entity_uid = g_players.at(0)->uid;
            }
            enable_camera_bounds = true;
            set_camera_bounds(enable_camera_bounds);
        }

        else if (held("mouse_camera_drag") && drag_delta("mouse_camera_drag") < 10.0f && held_duration("mouse_camera_drag") > 0.5f)
        {
            if (!g_players.empty())
            {
                g_state->camera->focused_entity_uid = g_players.at(0)->uid;
            }
            enable_camera_bounds = true;
            set_camera_bounds(enable_camera_bounds);
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
                if (g_state->pause != 0 || !options["smooth_camera"])
                {
                    g_state->camera->adjusted_focus_x = g_state->camera->focus_x;
                    g_state->camera->adjusted_focus_y = g_state->camera->focus_y;
                }
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

void render_options()
{
    if (options["menu_ui"] && !detached("tool_options"))
    {
        if (ImGui::MenuItem("Switch to windowed UI"))
            options["menu_ui"] = false;
        if (ImGui::MenuItem("Save options"))
            save_config(cfgfile);
        if (ImGui::MenuItem("Load options"))
            load_config(cfgfile);
        ImGui::Separator();
    }
    ImGui::Text("Game cheats");
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
    if (ImGui::Checkbox("Noclip##Noclip", &options["noclip"]))
    {
        toggle_noclip();
    }
    tooltip("Fly through walls and ignored by enemies.", "toggle_noclip");
    ImGui::Checkbox("Light dark levels and layers##DrawLights", &options["lights"]);
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
    if (ImGui::SliderFloat("Speedhack##SpeedHack", &g_speedhack_multiplier, 0.1f, 5.f))
    {
        speedhack();
    }
    tooltip("Slow down or speed up everything,\nlike in Cheat Engine.", "speedhack_decrease");
    ImGui::Separator();

    ImGui::Text("User interface");
    ImGui::Checkbox("Mouse controls##clickevents", &options["mouse_control"]);
    tooltip("Enables to spawn entities, teleport and pick entities with mouse.\nDisable for scripts that require mouse clicking.", "toggle_mouse");
    ImGui::Checkbox("Keyboard controls##keyevents", &options["keyboard_control"]);
    tooltip("Enables all hotkeys.\nDisable for scripts that extensively use keyboard.");
    ImGui::Checkbox("Snap to grid##Snap", &options["snap_to_grid"]);
    tooltip("Spawn items etc snapped to grid.\nAlways enabled for floor.", "toggle_snap");
    ImGui::Checkbox("Spawn floor decorated##Decorate", &options["spawn_floor_decorated"]);
    tooltip("Add decorations to spawned floor.");
    ImGui::Checkbox("Draw hitboxes##DrawEntityBox", &options["draw_hitboxes"]);
    tooltip("Draw hitboxes for all movable and hovered entities.", "toggle_hitboxes");
    ImGui::SameLine();
    ImGui::Checkbox("interpolated##DrawRealBox", &options["draw_hitboxes_interpolated"]);
    tooltip("Use interpolated render position for smoother hitboxes on hifps.\nActual game logic is not interpolated like this though.");
    ImGui::Checkbox("Smooth camera dragging", &options["smooth_camera"]);
    tooltip("Smooth camera movement when dragging, unless paused.");
    ImGui::SliderFloat("Camera speed##DragSpeed", &g_camera_speed, 1.0f, 5.0f);
    tooltip("Faster camera movement when dragging.");
    ImGui::Checkbox("Draw gridlines##DrawTileGrid", &options["draw_grid"]);
    tooltip("Show outlines of tiles and rooms, with roomtypes.", "toggle_grid");
    ImGui::Checkbox("Draw HUD##DrawHUD", &options["draw_hud"]);
    tooltip("Show enabled cheats and random\ninteresting state variables on screen.", "toggle_hud");

    if (ImGui::Checkbox("Drag windows outside the game window", &options["multi_viewports"]))
    {
        if (options["multi_viewports"])
            ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        else
            ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable;
    }
    tooltip("Allow dragging tools outside the main game window, to different monitor etc.");

    ImGui::Checkbox("Menu UI, instead of a floating window", &options["menu_ui"]);
    tooltip("Puts everything in a main menu instead of a floating window.\nYou can still create individual windows by dragging from the contents.");

    ImGui::Checkbox("Show tooltips", &options["show_tooltips"]);
    tooltip("Am I annoying you already :(");

    if (ImGui::CollapsingHeader("Hitbox entity types to draw"))
    {
        ImGui::PushID("HitboxMask");
        ImGui::Indent(16.0f);
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
        ImGui::Unindent(16.0f);
        ImGui::PopID();
    }

    if (ImGui::CollapsingHeader("Entity types to grab by default"))
    {
        ImGui::PushID("PickerMask");
        ImGui::Indent(16.0f);
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
        ImGui::Unindent(16.0f);
        ImGui::PopID();
    }

    if (options["menu_ui"])
        return;
    if (ImGui::Button("Edit style"))
    {
        toggle("tool_style");
    }
    tooltip("Edit the colors and fonts of Overlunky windows,\nor adjust the global scale if things are looking too big for you.", "tool_style");
    ImGui::SameLine();
    if (ImGui::Button("Edit keys"))
    {
        toggle("tool_keys");
    }
    tooltip("Edit the hotkeys for all the tools.", "tool_keys");
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

void render_script_files()
{
    ImGui::PushID("files");
    int num = 0;
    for (auto file : g_script_files)
    {
        ImGui::PushID(num++);
        std::string buttstr = file.parent_path().filename().string() + "/" + file.filename().string();
        if (ImGui::Button(buttstr.c_str()))
        {
            load_script(file.string().c_str(), false);
        }
        ImGui::PopID();
    }
    if (g_script_files.size() == 0)
    {
        std::filesystem::path path = scriptpath;
        std::string abspath = scriptpath;
        if (std::filesystem::exists(abspath) && std::filesystem::is_directory(abspath))
        {
            abspath = std::filesystem::absolute(path).string();
        }
        ImGui::TextWrapped("No scripts found. Put .lua files in '%s' or change script_dir in the ini file and reload.", abspath.c_str());
    }
    if (ImGui::Button("Refresh##RefreshScripts"))
    {
        refresh_script_files();
    }
    if (ImGui::Button("Create new quick script"))
    {
        std::string name = gen_random(16);
        SpelunkyScript* script = new SpelunkyScript(
            "meta.name = 'Script'\nmeta.version = '0.1'\nmeta.description = 'Shiny new script'\nmeta.author = 'You'\n\ncount = 0\nid = "
            "set_interval(function()\n  count = count + 1\n  message('Hello from your shiny new script')\n  if count > 4 then clear_callback(id) "
            "end\nend, 60)",
            name,
            g_SoundManager.get(),
            g_Console.get(),
            true);
        g_scripts[name] = std::unique_ptr<SpelunkyScript>(script);
    }
    ImGui::PopID();
}

void render_scripts()
{
    ImGui::PushTextWrapPos(0.0f);
    ImGui::PopTextWrapPos();
    ImGui::Checkbox("Draw script messages##DrawScriptMessages", &options["draw_script_messages"]);
    ImGui::SameLine();
    ImGui::Checkbox("to console##ConsoleScriptMessages", &options["console_script_messages"]);
    ImGui::Checkbox("Fade script messages##FadeScriptMessages", &options["fade_script_messages"]);
    if (ImGui::Checkbox("Load scripts from default directory##LoadScriptsDefault", &load_script_dir))
        refresh_script_files();
    if (ImGui::Checkbox("Load scripts from Mods/Packs##LoadScriptsPacks", &load_packs_dir))
        refresh_script_files();
    ImGui::PushItemWidth(-1);
    int i = 0;
    std::vector<std::string> unload_scripts;
    ImVec4 origcolor = ImGui::GetStyle().Colors[ImGuiCol_Header];
    float gray = (origcolor.x + origcolor.y + origcolor.z) / 3.0f;
    ImVec4 disabledcolor = ImVec4(gray, gray, gray, 0.5f);
    for (auto& [script_name, script] : g_scripts)
    {
        ImGui::PushID(i);
        std::string filename;
        size_t slash = script->get_file().find_last_of("/\\");
        if (slash != std::string::npos)
            filename = script->get_file().substr(slash + 1);
        std::string name = fmt::format("{} ({})", script->get_name(), filename);
        if (!script->is_enabled())
        {
            ImGui::PushStyleColor(ImGuiCol_Header, disabledcolor);
        }
        else
        {
            ImGui::PushStyleColor(ImGuiCol_Header, origcolor);
        }
        if (ImGui::CollapsingHeader(name.c_str()))
        {
            ImGui::Text(
                "%s %s by %s (%s)",
                script->get_name().c_str(),
                script->get_version().c_str(),
                script->get_author().c_str(),
                script->get_id().c_str());
            ImGui::TextWrapped("%s", script->get_description().c_str());
            if (!script->get_unsafe() || options["enable_unsafe_scripts"])
            {
                static bool run_unsafe = false;
                if (script->get_unsafe())
                {
                    ImGui::PushTextWrapPos(0.0f);
                    ImGui::TextColored(
                        ImVec4(1.0f, 0.3f, 0.3f, 1.0f),
                        "Warning: This script uses unsafe commands and it could delete your files or download viruses. Only enable this script if "
                        "you trust the author, have read the whole script or made it yourself.");
                    ImGui::PopTextWrapPos();
                    ImGui::Checkbox("I understand the risks.", &run_unsafe);
                }
                if (!script->get_unsafe() || run_unsafe)
                {
                    if (script->is_enabled() && ImGui::Button("Disable##DisableScript"))
                    {
                        script->set_enabled(false);
                    }
                    else if (!script->is_enabled() && ImGui::Button("Enable##EnableScript"))
                    {
                        script->set_enabled(true);
                        script->set_changed(true);
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Unload##UnloadScript"))
                    {
                        unload_scripts.push_back(script->get_file());
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Reload##ReloadScript"))
                    {
                        load_script(script->get_file(), script->is_enabled());
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
                    ImGui::InputText("##LuaResult", &script->get_result(), ImGuiInputTextFlags_ReadOnly);
                }
            }
            else
            {
                ImGui::TextWrapped("\nYou have not enabled running unsafe scripts. Bye.");
            }
        }
        else
        {
            ++i;
        }
        ImGui::PopStyleColor();
        ImGui::PopID();
    }
    for (auto id : unload_scripts)
    {
        auto it = g_scripts.find(id);
        if (it != g_scripts.end())
            g_scripts.erase(id);
    }
    if (ImGui::CollapsingHeader("Load new script##LoadScriptFile"))
    {
        render_script_files();
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
    if (options["disable_savegame"])
    {
        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Note: You have blocked game saves in the options...");
    }
    ImGui::PushID("Journal");
    if (ImGui::CollapsingHeader("Journal"))
    {
        ImGui::Indent(16.0f);
        ImGui::PushID("Places");
        if (ImGui::CollapsingHeader("Places"))
        {
            for (int i = 0; i < 16; ++i)
            {
                ImGui::PushID(i);
                ImGui::Checkbox(places_flags[i], &g_save->places[i]);
                ImGui::PopID();
            }
        }
        ImGui::PopID();
        ImGui::PushID("People");
        if (ImGui::CollapsingHeader("People"))
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
        }
        ImGui::PopID();
        ImGui::PushID("Bestiary");
        if (ImGui::CollapsingHeader("Bestiary"))
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
        }
        ImGui::PopID();
        ImGui::PushID("Items");
        if (ImGui::CollapsingHeader("Items"))
        {
            for (int i = 0; i < 54; ++i)
            {
                ImGui::PushID(i);
                ImGui::Checkbox(items_flags[i], &g_save->items[i]);
                ImGui::PopID();
            }
        }
        ImGui::PopID();
        ImGui::PushID("Traps");
        if (ImGui::CollapsingHeader("Traps"))
        {
            for (int i = 0; i < 24; ++i)
            {
                ImGui::PushID(i);
                ImGui::Checkbox(traps_flags[i], &g_save->traps[i]);
                ImGui::PopID();
            }
        }
        ImGui::PopID();
        ImGui::Unindent(16.0f);
    }
    ImGui::PopID();

    ImGui::PushID("Characters");
    if (ImGui::CollapsingHeader("Characters"))
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
    }
    ImGui::PopID();

    ImGui::PushID("Shortcuts");
    if (ImGui::CollapsingHeader("Shortcuts"))
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
    }
    ImGui::PopID();

    ImGui::PushID("Player Profile");
    if (ImGui::CollapsingHeader("Player Profile"))
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
    }
    ImGui::PopID();

    ImGui::PushID("Last Game Played");
    if (ImGui::CollapsingHeader("Last Game Played"))
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
            ImGui::InputInt("Sticker", &g_save->stickers[i]);
            ImGui::PopID();
        }
    }
    ImGui::PopID();

    ImGui::PushID("Miscellaneous");
    if (ImGui::CollapsingHeader("Miscellaneous"))
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
    }
    ImGui::PopID();

    ImGui::PushID("UnlockAll");
    if (ImGui::CollapsingHeader("Big scary button to unlock everything"))
    {
        ImGui::PushFont(bigfont);
        ImGui::PushItemWidth(ImGui::GetContentRegionMax().x);
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
    }
    ImGui::PopID();
}

void render_powerup(int uid, const char* section)
{
    std::string uidc = std::to_string(uid);
    int ptype = UI::get_entity_type(uid);
    if (ptype == 0)
        return;
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
    if (ImGui::Button("X"))
    {
        g_entity->as<Player>()->remove_powerup(ptype);
    }
    ImGui::PopID();
    ImGui::PopID();
}

void render_state(const char* label, int state)
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
        std::string statec = std::to_string(state);
        ImGui::LabelText(label, "%s", statec.c_str());
    }
}

void render_ai(const char* label, int state)
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
        std::string statec = std::to_string(state);
        ImGui::LabelText(label, "%s", statec.c_str());
    }
}

void render_screen(const char* label, int state)
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
    else if (state == 27)
        ImGui::LabelText(label, "27 Arena scores");
    else if (state == 28)
        ImGui::LabelText(label, "28 Loading online");
    else if (state == 29)
        ImGui::LabelText(label, "29 Lobby");
    else
    {
        std::string statec = std::to_string(state);
        ImGui::LabelText(label, "%s", statec.c_str());
    }
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

    static uint32_t search_entity_type = 0;
    ImGui::InputScalar("##EntityType", ImGuiDataType_U32, &search_entity_type, &u32_one);
    ImGui::SameLine();
    if (search_entity_type > 0)
    {
        ImGui::Text("%s", entity_names[search_entity_type].c_str());
    }
    else
    {
        ImGui::Text("ENT_TYPE");
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
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.25f);
    if (ImGui::SliderInt("##EntitySearchDepthMin", &search_entity_depth[0], 0, 52, "%d", ImGuiSliderFlags_AlwaysClamp))
    {
        if (search_entity_depth[1] < search_entity_depth[0])
            search_entity_depth[1] = search_entity_depth[0];
    }
    ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.25f);
    if (ImGui::SliderInt("Draw depth##EntitySearchDepthMax", &search_entity_depth[1], 0, 52, "%d", ImGuiSliderFlags_AlwaysClamp))
    {
        if (search_entity_depth[0] > search_entity_depth[1])
            search_entity_depth[0] = search_entity_depth[1];
    }
    ImGui::PopItemWidth();
    // ImGui::DragIntRange2("Draw depth##EntitySearchDepth", &search_entity_depth[0], &search_entity_depth[1], 1.0f, 0, 52);

    static int search_entity_mask = 0;
    if (ImGui::CollapsingHeader("Mask##EntitySearchMask"))
    {
        for (int i = 0; i < 15; i++)
        {
            if (i % 2)
                ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.5f);
            ImGui::CheckboxFlags(mask_names[i], &search_entity_mask, (int)std::pow(2, i));
        }
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

    if (ImGui::CollapsingHeader("Flags##EntitySearchFlags"))
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
    }
    if (ImGui::CollapsingHeader("More Flags##EntitySearchMoreFlags"))
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
    }
    if (ImGui::CollapsingHeader("Properties Flags##EntitySearchPropertiesFlags"))
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
    }
    ImGui::PopStyleColor();
    ImGui::Text("");

    static bool run_filter = false;
    static bool extra_filter = false;
    if (ImGui::Button("Search##SearchEntities") || run_finder)
    {
        g_selected_ids = UI::get_entities_by({search_entity_type}, search_entity_mask, (LAYER)search_entity_layer);
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
                                                        return (ent->type->search_flags & search_entity_mask) == 0; }),
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
        ImGui::InputInt("UID", &g_last_id);
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
        ImGui::PopItemWidth();
        if (!update_entity())
            return;
    }
    if (entity == nullptr)
    {
        auto it = entity_windows.find(uid);
        if (it != entity_windows.end())
            entity_windows[uid]->open = false;
        return;
    }
    const auto is_movable = entity->is_movable();
    ImGui::PushItemWidth(-ImGui::GetWindowWidth() * 0.5f);
    render_uid(entity->uid, "EntityGeneral");
    if (ImGui::Button("Smart delete##SafeKillEntity"))
    {
        smart_delete(entity, true);
    }
    tooltip("Try to get rid of the entity in some\nsmart way that hopefully doesn't crash.\nWorks on most boss heads and jellies.", "mouse_destroy");
    ImGui::SameLine();
    if (ImGui::Button("Void##VoidEntity"))
    {
        if (entity->overlay && entity->overlay->is_movable())
        {
            auto mount = entity->overlay->as<Movable>();
            if (mount->holding_uid == entity->uid)
            {
                mount->holding_uid = -1;
            }
        }
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
    if (ImGui::CollapsingHeader("State"))
    {
        auto overlay = entity->overlay;
        if (overlay && !IsBadReadPtr(overlay, 0x178))
        {
            if (overlay->type->search_flags & 0x2) // MOUNT
            {
                ImGui::Text("Riding:");
                ImGui::SameLine();
                if (ImGui::Button("Unmount##UnmountRiding"))
                {
                    overlay->as<Mount>()->remove_rider();
                }
                render_uid(overlay->uid, "StateRiding");
            }
            else
            {
                ImGui::Text("Attached to:");
                ImGui::SameLine();
                if (ImGui::Button("Detach"))
                {
                    if (entity->type->search_flags & 0x1) // PLAYER
                    {
                        entity->as<Player>()->let_go();
                    }
                    else if (overlay->is_movable() && overlay->as<Movable>()->holding_uid == entity->uid)
                    {
                        overlay->as<Movable>()->drop(entity);
                    }
                    else
                    {
                        overlay->remove_item_ptr(entity);
                    }
                }
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
            render_state("Current state", movable->state); // TODO: allow change
            render_state("Last state", movable->last_state);
            render_ai("AI state", movable->move_state); // TODO: allow change
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
                        movable->drop(holding);
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
    }
    if (ImGui::CollapsingHeader("Position"))
    {
        auto movable = entity->as<Movable>();
        if (ImGui::Button("Change"))
        {
            auto layer_to = LAYER::FRONT;
            if (entity->layer == 0)
                layer_to = LAYER::BACK;
            entity->set_layer(layer_to);
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
        ImGui::InputFloat("Position Y##EntityPositionX", &entity->y, 0.2f, 1.0f);
        if (is_movable)
        {
            ImGui::InputFloat("Velocity X##EntityVelocityX", &movable->velocityx, 0.2f, 1.0f);
            ImGui::InputFloat("Velocity y##EntityVelocityY", &movable->velocityy, 0.2f, 1.0f);
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
    }
    if (is_movable && ImGui::CollapsingHeader("Stats"))
    {
        auto movable = entity->as<Player>();
        ImGui::DragScalar("Health##EntityHealth", ImGuiDataType_U8, (char*)&movable->health, 0.5f, &u8_one, &u8_max);
        ImGui::DragScalar("Price##Price", ImGuiDataType_S32, (char*)&movable->price, 0.5f, &s32_min, &s32_max);
        if ((entity->type->search_flags & 0x1) && movable->inventory_ptr != 0)
        {
            ImGui::DragScalar("Bombs##EntityBombs", ImGuiDataType_U8, (char*)&movable->inventory_ptr->bombs, 0.5f, &u8_one, &u8_max);
            ImGui::DragScalar("Ropes##EntityRopes", ImGuiDataType_U8, (char*)&movable->inventory_ptr->ropes, 0.5f, &u8_one, &u8_max);
            ImGui::DragInt("Money##EntityMoney", (int*)&movable->inventory_ptr->money, 20.0f, INT_MIN, INT_MAX, "%d");
            ImGui::DragInt("Level kills##EntityLevelKills", (int*)&movable->inventory_ptr->kills_level, 0.5f, 0, INT_MAX, "%d");
            ImGui::DragInt("Total kills##EntityTotalKills", (int*)&movable->inventory_ptr->kills_total, 0.5f, 0, INT_MAX, "%d");
        }
    }
    if (ImGui::CollapsingHeader("Items"))
    {
        for (uint32_t item_uid : entity->items.uids())
        {
            render_uid(item_uid, "EntityItems", true);
        }
    }
    if (ImGui::CollapsingHeader("Global attributes") && entity->type)
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
        ImGui::InputScalar("Search flags##SearchFlags", ImGuiDataType_U32, &entity->type->search_flags, 0, 0, "%p", ImGuiInputTextFlags_ReadOnly);
        if (ImGui::CollapsingHeader("Properties flags"))
        {
            for (int i = 0; i < 32; i++)
            {
                ImGui::CheckboxFlags(entity_type_properties_flags[i], &entity->type->properties_flags, (int)std::pow(2, i));
            }
        }
    }
    if (ImGui::CollapsingHeader("Special attributes"))
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
        else if (entity->type->search_flags & 0x7) // PLYAER, MOUNT, MONSTER
        {
            auto entity_pow = entity->as<PowerupCapable>();
            for (const auto& [powerup_id, powerup_entity] : entity_pow->powerups)
            {
                render_powerup(powerup_entity->uid, "Powerups");
            }
            ImGui::Text("  Add: ");
            ImGui::SameLine();
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
            if (ImGui::Button("Add##AddPowerupButton"))
            {
                entity_pow->give_powerup(powerupTypeIDOptions[chosenPowerupIndex]);
            }
            ImGui::PopItemWidth();
            auto entity_player = entity->as<Player>();
            if ((entity->type->search_flags & 0x1) && entity_player->ai != 0)
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
    }
    if (ImGui::CollapsingHeader("Color, Size, Texture"))
    {
        auto textureid = entity->get_texture();
        std::string texture = g_Console.get()->execute(fmt::format("return enum_get_name(TEXTURE, get_entity({}):get_texture()) or 'UNKNOWN'", uid));
        // std::string texturepath = g_Console.get()->execute(fmt::format("return get_texture_definition(get_entity({}):get_texture()).texture_path", uid));
        texture = texture.substr(1, texture.length() - 2);
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
            entity->set_draw_depth(draw_depth);

        ImGui::DragScalar("Animation frame##EntityAnimationFrame", ImGuiDataType_U16, &entity->animation_frame, 0.2f, &u16_zero, &u16_max);
        ImGui::InputText(fmt::format("Texture: {}##EntityTexture", textureid).c_str(), &texture, ImGuiInputTextFlags_ReadOnly);
        // ImGui::InputText("Texture path##EntityTexturePath", &texturepath, ImGuiInputTextFlags_ReadOnly);
    }
    if (ImGui::CollapsingHeader("Flags"))
    {
        for (int i = 0; i < 32; i++)
        {
            ImGui::CheckboxFlags(entity_flags[i], &entity->flags, (int)std::pow(2, i));
        }
    }
    if (ImGui::CollapsingHeader("More Flags"))
    {
        for (int i = 0; i < 32; i++)
        {
            ImGui::CheckboxFlags(more_flags[i], &entity->more_flags, (int)std::pow(2, i));
        }
    }
    if (is_movable && ImGui::CollapsingHeader("Input Display"))
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
    }
    if ((entity->type->search_flags & 0x1) && ImGui::CollapsingHeader("Illumination"))
    {
        auto entity_player = entity->as<Player>();
        if (entity_player->emitted_light)
            render_illumination(entity_player->emitted_light, "Entity illumination");
    }
    ImGui::PopItemWidth();
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
        g_entity = nullptr;
    }
}

void render_timer()
{
    int frames = g_state->time_total;
    time_t secs = frames / 60;
    struct tm newtime;
    char time[10];
    gmtime_s(&newtime, &secs);
    std::strftime(time, sizeof(time), "%H:%M:%S", &newtime);
    std::stringstream ss;
    ss << "Total: " << time << "." << std::setfill('0') << std::setw(3) << floor((frames % 60) * (1000.0 / 60.0));
    ImGui::PushFont(bigfont);
    ImGui::Text("%s", ss.str().c_str());
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
        ImGui::LabelText("Game state", "%s", gamestate.c_str());
        if (ImGui::Checkbox("Pause game engine##PauseSim", &paused))
        {
            if (paused)
                g_state->pause = 0x2;
            else
                g_state->pause = 0;
        }
        tooltip("Pause time while still being able to teleport, spawn and move entities", "toggle_pause");
    }
    if (ImGui::CollapsingHeader("Timer"))
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
    }
    if (ImGui::CollapsingHeader("Level"))
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
    }
    if (ImGui::CollapsingHeader("Quests"))
    {
        ImGui::Indent(16.0f);

        if (ImGui::CollapsingHeader("Yang"))
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
        }

        if (ImGui::CollapsingHeader("Jungle Sisters"))
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
        }

        if (ImGui::CollapsingHeader("Van Horsing"))
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
            ImGui::RadioButton("Got alien compass##QuestHorsingComplass", &horsing_state, 6);
            ImGui::RadioButton("Palace basement ending##QuestHorsingTusk", &horsing_state, 7);
            g_state->quests->van_horsing_state = static_cast<int8_t>(horsing_state);
        }

        if (ImGui::CollapsingHeader("Sparrow"))
        {
            if (ImGui::Checkbox("Freeze current state##FreezeQuestSparrow", &freeze_quest_sparrow))
            {
                if (freeze_quest_sparrow)
                    quest_sparrow_state = g_state->quests->sparrow_state;
                else
                    quest_sparrow_state.reset();
            }
            int sparrow_state = static_cast<int>(g_state->quests->sparrow_state);
            ImGui::RadioButton("Angry##QuestSparrowAngry", &sparrow_state, -1);
            ImGui::RadioButton("Not started##QuestSparrowDefault", &sparrow_state, 0);
            ImGui::RadioButton("Thief##QuestSparrowThief", &sparrow_state, 1);
            ImGui::RadioButton("Finished level as thief##QuestSparrowThiefLevel", &sparrow_state, 2);
            ImGui::RadioButton("Spawned in first hideout##QuestSparrowFirst", &sparrow_state, 3);
            ImGui::RadioButton("Got ropes##QuestSparrowRopes", &sparrow_state, 4);
            ImGui::RadioButton("Stole Tusk idol##QuestSparrowTusk", &sparrow_state, 5);
            ImGui::RadioButton("Spawned in Neo Babylon##QuestSparrowSecond", &sparrow_state, 6);
            ImGui::RadioButton("Met in Neo Babylon##QuestSparrowSecondComplete", &sparrow_state, 7);
            ImGui::RadioButton("Palace basement ending##QuestSparrowTusk", &sparrow_state, 8);
            g_state->quests->sparrow_state = static_cast<int8_t>(sparrow_state);
        }

        if (ImGui::CollapsingHeader("Madame Tusk"))
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
        }

        if (ImGui::CollapsingHeader("Beg"))
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
        }

        ImGui::Unindent(16.0f);
    }
    if (ImGui::CollapsingHeader("Street cred"))
    {
        ImGui::DragScalar("Shoppie aggro##ShoppieAggro", ImGuiDataType_U8, &g_state->shoppie_aggro, 0.5f, &u8_min, &u8_max, "%d");
        ImGui::DragScalar("Shoppie aggro levels##ShoppieAggroLevels", ImGuiDataType_U8, &g_state->shoppie_aggro_levels, 0.5f, &u8_min, &u8_max, "%d");
        ImGui::DragScalar("Tun aggro##MerchantAggro", ImGuiDataType_U8, &g_state->merchant_aggro, 0.5f, &u8_min, &u8_max, "%d");
        ImGui::DragScalar("NPC kills##NPCKills", ImGuiDataType_U8, (char*)&g_state->kills_npc, 0.5f, &u8_zero, &u8_max);
        ImGui::DragScalar("Kali favor##PorFavor", ImGuiDataType_S8, (char*)&g_state->kali_favor, 0.5f, &s8_min, &s8_max);
        ImGui::DragScalar("Kali status##KaliStatus", ImGuiDataType_S8, (char*)&g_state->kali_status, 0.5f, &s8_min, &s8_max);
        ImGui::DragScalar("Altars destroyed##KaliAltars", ImGuiDataType_S8, (char*)&g_state->kali_altars_destroyed, 0.5f, &s8_min, &s8_max);
    }
    if (ImGui::CollapsingHeader("Players"))
    {
        render_players();
    }
    if (ImGui::CollapsingHeader("Level flags"))
    {
        for (int i = 0; i < 32; i++)
        {
            ImGui::CheckboxFlags(level_flags[i], &g_state->level_flags, (int)std::pow(2, i));
        }
    }
    if (ImGui::CollapsingHeader("Quest flags"))
    {
        for (int i = 0; i < 32; i++)
        {
            ImGui::CheckboxFlags(quest_flags[i], &g_state->quest_flags, (int)std::pow(2, i));
        }
    }
    if (ImGui::CollapsingHeader("Journal flags"))
    {
        for (int i = 0; i < 21; i++)
        {
            ImGui::CheckboxFlags(journal_flags[i], &g_state->journal_flags, (int)std::pow(2, i));
        }
    }
    if (ImGui::CollapsingHeader("Presence flags"))
    {
        for (int i = 0; i < 11; i++)
        {
            ImGui::CheckboxFlags(presence_flags[i], &g_state->presence_flags, (int)std::pow(2, i));
        }
    }
    if (ImGui::CollapsingHeader("Special visibility flags"))
    {
        for (int i = 0; i < 32; i++)
        {
            ImGui::CheckboxFlags(special_visibility_flags[i], &g_state->special_visibility_flags, (int)std::pow(2, i));
        }
    }
    if (ImGui::CollapsingHeader("AI targets"))
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
            if (ai_entity == nullptr || (ai_entity->type->search_flags & 1) != 1)
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
    }
    if (ImGui::CollapsingHeader("Global illumination"))
    {
        if (g_state->illumination)
            render_illumination(g_state->illumination, "Global illumination");
    }
    if (ImGui::CollapsingHeader("Liquid pools"))
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
    }
    ImGui::PopItemWidth();
}

void load_font()
{
    ImGuiIO& io = ImGui::GetIO();
    io.FontAllowUserScaling = false;
    PWSTR fontdir;
    if (SHGetKnownFolderPath(FOLDERID_Fonts, 0, NULL, &fontdir) == S_OK)
    {
        if (fontfile != "")
        {
            using cvt_type = std::codecvt_utf8<wchar_t>;
            std::wstring_convert<cvt_type, wchar_t> cvt;

            std::string fontpath(cvt.to_bytes(fontdir) + "\\" + fontfile);
            if (GetFileAttributesA(fontpath.c_str()) != INVALID_FILE_ATTRIBUTES)
            {
                font = io.Fonts->AddFontFromFileTTF(fontpath.c_str(), fontsize[0]);
                bigfont = io.Fonts->AddFontFromFileTTF(fontpath.c_str(), fontsize[1]);
                hugefont = io.Fonts->AddFontFromFileTTF(fontpath.c_str(), fontsize[2]);
            }
            else if (SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &fontdir) == S_OK)
            {
                std::string localfontpath(cvt.to_bytes(fontdir) + "\\Microsoft\\Windows\\Fonts\\" + fontfile);
                if (GetFileAttributesA(localfontpath.c_str()) != INVALID_FILE_ATTRIBUTES)
                {
                    font = io.Fonts->AddFontFromFileTTF(localfontpath.c_str(), fontsize[0]);
                    bigfont = io.Fonts->AddFontFromFileTTF(localfontpath.c_str(), fontsize[1]);
                    hugefont = io.Fonts->AddFontFromFileTTF(localfontpath.c_str(), fontsize[2]);
                }
            }
        }
        CoTaskMemFree(fontdir);
    }

    if (!font || !bigfont || !hugefont)
    {
        font = io.Fonts->AddFontFromMemoryCompressedTTF(OLFont_compressed_data, OLFont_compressed_size, fontsize[0]);
        bigfont = io.Fonts->AddFontFromMemoryCompressedTTF(OLFont_compressed_data, OLFont_compressed_size, fontsize[1]);
        hugefont = io.Fonts->AddFontFromMemoryCompressedTTF(OLFont_compressed_data, OLFont_compressed_size, fontsize[2]);
    }
}

void render_style_editor()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImGuiIO& io = ImGui::GetIO();
    if (options["menu_ui"])
    {
        if (ImGui::MenuItem("Randomize"))
        {
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
    ImGui::DragFloat("Hue##StyleHue", &g_hue, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("Saturation##StyleSaturation", &g_sat, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("Lightness##StyleLightness", &g_val, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("Alpha##StyleAlpha", &style.Alpha, 0.01f, 0.2f, 1.0f);
    ImGui::DragFloat("Scale##StyleScale", &io.FontGlobalScale, 0.01f, 0.2f, 2.0f);
    if (!options["menu_ui"])
    {
        if (ImGui::Button("Randomize##StyleRandomize"))
        {
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
    ImGui::BeginTable("##keyconfig", 4);
    ImGui::TableSetupColumn("Tool");
    ImGui::TableSetupColumn("Keys");
    ImGui::TableSetupColumn("Keycode");
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
        ImGui::InputScalar("##keycode", ImGuiDataType_S64, &keys[kv.first], 0, 0, "0x%X", ImGuiInputTextFlags_CharsHexadecimal);
        ImGui::TableNextColumn();
        if (ImGui::Button("Capture"))
        {
            g_change_key = kv.first;
        }
        ImGui::SameLine();
        if (ImGui::Button("Disable"))
        {
            keys[kv.first] = 0;
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
                if (io.KeysDown[VK_CONTROL])
                    keycode += 0x100;
                if (io.KeysDown[VK_SHIFT])
                    keycode += 0x200;
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
            if (io.KeysDown[VK_CONTROL])
                keycode += 0x100;
            if (io.KeysDown[VK_SHIFT])
                keycode += 0x200;
            keys[g_change_key] = keycode;
            save_config(cfgfile);
            g_change_key = "";
        }

        // Keys
        for (size_t i = 0; i < VK_LSHIFT; ++i)
        {
            if (ImGui::IsKeyDown((ImGuiKey)i))
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

void render_spawner()
{
    render_input();
    render_list();
}

void render_prohud()
{
    auto io = ImGui::GetIO();
    auto base = ImGui::GetMainViewport();
    ImDrawList* dl = ImGui::GetBackgroundDrawList(base);
    auto topmargin = 0.0f;
    if (options["menu_ui"])
        topmargin = ImGui::GetTextLineHeight();
    std::string buf = fmt::format("FRAME:{:#06} TOTAL:{:#06} LEVEL:{:#06} COUNT:{} SCREEN:{} SIZE:{}x{} PAUSE:{} FPS:{:.0f}", UI::get_frame_count(), g_state->time_total, g_state->time_level, g_state->level_count, g_state->screen, g_state->w, g_state->h, g_state->pause, io.Framerate);
    ImVec2 textsize = ImGui::CalcTextSize(buf.c_str());
    dl->AddText({base->Pos.x + base->Size.x / 2 - textsize.x / 2, base->Pos.y + 2 + topmargin}, ImColor(1.0f, 1.0f, 1.0f, .5f), buf.c_str());

    buf = fmt::format("{}{}{}{}{}{}{}{}{}{}{}{}{}{}", (options["god_mode"] ? "GODMODE " : ""), (options["god_mode_companions"] ? "HHGODMODE " : ""), (options["noclip"] ? "NOCLIP " : ""), (options["lights"] ? "LIGHTS " : ""), (test_flag(g_dark_mode, 1) ? "DARK " : ""), (test_flag(g_dark_mode, 2) ? "NODARK " : ""), (options["disable_ghost_timer"] ? "NOGHOST " : ""), (options["disable_achievements"] ? "NOSTEAM " : ""), (options["disable_savegame"] ? "NOSAVE " : ""), (options["disable_pause"] ? "NOPAUSE " : ""), (g_zoom != 13.5 ? fmt::format("ZOOM:{} ", g_zoom) : ""), (g_speedhack_multiplier != 1.0 ? fmt::format("SPEEDHACK:{} ", g_speedhack_multiplier) : ""), (!options["mouse_control"] ? "NOMOUSE " : ""), (!options["keyboard_control"] ? "NOKEYBOARD " : ""));
    textsize = ImGui::CalcTextSize(buf.c_str());
    dl->AddText({base->Pos.x + base->Size.x / 2 - textsize.x / 2, base->Pos.y + textsize.y + 4 + topmargin}, ImColor(1.0f, 1.0f, 1.0f, .5f), buf.c_str());

    auto type = spawned_type();
    buf = fmt::format("{}", (type == "" ? "" : fmt::format("SPAWN:{}", type)));
    textsize = ImGui::CalcTextSize(buf.c_str());
    dl->AddText({base->Pos.x + base->Size.x / 2 - textsize.x / 2, base->Pos.y + textsize.y * 2 + 4 + topmargin}, ImColor(1.0f, 1.0f, 1.0f, .5f), buf.c_str());
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
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigViewportsNoTaskBarIcon = true;
}

void imgui_init(ImGuiContext*)
{
    show_cursor();
    load_config(cfgfile);
    load_font();
    refresh_script_files();
    autorun_scripts();
    set_colors();
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

    if (g_ui_scripts.find("dark") == g_ui_scripts.end())
    {
        SpelunkyScript* script = new SpelunkyScript(
            "set_callback(function() state.level_flags = set_flag(state.level_flags, 18) end, ON.POST_ROOM_GENERATION)",
            "dark",
            g_SoundManager.get(),
            g_Console.get(),
            false);
        g_ui_scripts["dark"] = std::unique_ptr<SpelunkyScript>(script);
    }
    if (g_ui_scripts.find("light") == g_ui_scripts.end())
    {
        SpelunkyScript* script = new SpelunkyScript(
            "set_callback(function() state.level_flags = clr_flag(state.level_flags, 18) end, ON.POST_ROOM_GENERATION)",
            "light",
            g_SoundManager.get(),
            g_Console.get(),
            false);
        g_ui_scripts["light"] = std::unique_ptr<SpelunkyScript>(script);
    }
}

void imgui_draw()
{
    auto base = ImGui::GetMainViewport();
    ImGuiContext& g = *GImGui;

    for (int i = 1; i < g.PlatformIO.Viewports.Size; i++)
    {
        ImGuiViewport* viewport = g.PlatformIO.Viewports[i];
        viewport->Flags |= ImGuiViewportFlags_NoFocusOnClick | ImGuiViewportFlags_NoFocusOnAppearing | ImGuiViewportFlags_OwnedByApp;
    }

    ImDrawList* dl = ImGui::GetBackgroundDrawList(base);
    std::string buf = fmt::format("Overlunky {}", get_version());
    ImVec2 textsize = ImGui::CalcTextSize(buf.c_str());

    dl->AddText({base->Pos.x + base->Size.x / 2 - textsize.x / 2, base->Pos.y + base->Size.y - textsize.y - 2}, ImColor(1.0f, 1.0f, 1.0f, .3f), buf.c_str());

    render_clickhandler();
    if (options["draw_hud"])
        render_prohud();
    if (options["draw_script_messages"])
        render_messages();

    float toolwidth = 0.12f * ImGui::GetIO().DisplaySize.x * ImGui::GetIO().FontGlobalScale;
    if (!hide_ui)
    {
        ImGui::SetNextWindowPos(base->Pos, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize({600, base->Size.y / 2}, ImGuiCond_FirstUseEver);
        if (options["menu_ui"])
        {
            ImGui::PushID("MainMenu");
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {0, 0});
            if (ImGui::BeginMainMenuBar())
            {
                ImGui::PopStyleVar();
                for (size_t i = 0; i < tab_order.size(); ++i)
                {
                    auto tab = tab_order[i];
                    if (windows[tab]->detached)
                        continue;
                    ImGui::SetNextWindowSizeConstraints({300.0f, 100.0f}, {600.0f, base->Size.y - 50.0f});
                    if (ImGui::BeginMenu(windows[tab]->name.c_str(), true))
                    {
                        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {2, 2});
                        active_tab = tab;
                        if (ImGui::BeginDragDropSource())
                        {
                            ImGui::SetDragDropPayload("TAB", NULL, 0);
                            ImGui::Text("Drag outside the menu\nto detach %s", windows[tab]->name.c_str());
                            ImGui::EndDragDropSource();
                        }
                        ImGui::GetIO().WantCaptureKeyboard = true;
                        render_tool(tab);
                        ImGui::PopStyleVar();
                        ImGui::EndMenu();
                    }
                }
                ImGui::EndMainMenuBar();
            }
            ImGui::PopID();
        }
        else
        {
            ImGui::Begin("Overlunky", NULL, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiViewportFlags_NoTaskBarIcon | ImGuiViewportFlags_NoDecoration);
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("Tools"))
                {
                    for (size_t i = 0; i < tab_order.size() - 4; ++i)
                    {
                        auto tab = tab_order[i];
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
                    if (ImGui::MenuItem("Switch to menu UI"))
                        options["menu_ui"] = true;
                    ImGui::Separator();
                    for (size_t i = tab_order.size() - 4; i < tab_order.size(); ++i)
                    {
                        auto tab = tab_order[i];
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
                for (auto tab : tab_order)
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
                        active_tab = tab;
                        ImGui::BeginChild("ScrollableTool");
                        render_tool(tab);
                        ImGui::EndChild();
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
                ImGui::TextWrapped("Looks like you closed all your tabs. Good thing we have a menubar now!");
                if (ImGui::Button("Restore all tabs"))
                {
                    for (auto window : windows)
                    {
                        window.second->open = true;
                    }
                }
            }
            ImGui::End();
        }
        for (auto tab : windows)
        {
            if (!tab.second->detached)
                continue;
            ImGui::SetNextWindowSize({toolwidth, toolwidth}, ImGuiCond_Once);
            ImGui::Begin(tab.second->name.c_str(), &tab.second->detached, ImGuiViewportFlags_NoTaskBarIcon);
            ImGui::PushID(tab.second->name.c_str());
            ImGui::BeginChild("ScrollableTool");
            render_tool(tab.first);
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
}

void post_draw()
{
    update_players();
    force_zoom();
    force_hud_flags();
    force_time();
    force_noclip();
    force_lights();
    frame_advance();
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

void init_ui()
{
    g_SoundManager = std::make_unique<SoundManager>(&LoadAudioFile);

    g_state = get_state_ptr();
    g_save = UI::savedata();
    g_game_manager = get_game_manager();

    g_Console = std::make_unique<SpelunkyConsole>(g_SoundManager.get());
    g_Console->set_max_history_size(1000);
    g_Console->load_history("console_history.txt");

    register_on_input(&process_keys);
    register_imgui_pre_init(&imgui_pre_init);
    register_imgui_init(&imgui_init);
    register_imgui_draw(&imgui_draw);
    register_post_draw(&post_draw);

    register_make_save_path(&make_save_path);

    register_on_load_file(&load_file_as_dds_if_image);
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
