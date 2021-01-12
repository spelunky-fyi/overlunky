#define NOMINMAX
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/examples/imgui_impl_win32.h"
#include "imgui/examples/imgui_impl_dx11.h"
#include <d3d11.h>
#include "ui.hpp"
#include "injected-dll/src/ui.rs.h"
#include <Windows.h>
#include <Shlwapi.h>
#include <Shlobj.h>
#include <algorithm>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include <locale>
#include <codecvt>
#include <map>
#include <iomanip>

const USHORT HID_MOUSE    = 2;
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
    { "enter", 0x0d },
    { "escape", 0x1b },
    { "move_left", 0x25 },
    { "move_up", 0x26 },
    { "move_right", 0x27 },
    { "move_down", 0x28 },
    { "move_pageup", 0x21 },
    { "move_pagedown", 0x22 },
    { "toggle_mouse", 0x14d },
    { "toggle_godmode", 0x147 },
    { "toggle_snap", 0x153 },
    { "toggle_pause", 0x150 },
    { "toggle_disable_input", 0x14b },
    { "toggle_allow_pause", 0x350 },
    { "tool_entity", 0x70 },
    { "tool_door", 0x71 },
    { "tool_camera", 0x72 },
    { "tool_options", 0x78 },
    { "tool_debug", 0x37b },
    { "tool_metrics", 0x349 },
    { "reset_windows", 0x352 },
    { "reset_windows_vertical", 0x356 },
    { "save_settings", 0x353 },
    { "load_settings", 0x34c },
    { "set_colors", 0x355 },
    { "spawn_entity", 0x10d },
    { "spawn_layer_door", 0x20d },
    { "spawn_warp_door", 0x30d },
    { "hide_ui", 0x7a },
    { "zoom_in", 0x1bc },
    { "zoom_out", 0x1be },
    { "zoom_default", 0x132 },
    { "zoom_3x", 0x133 },
    { "zoom_4x", 0x134 },
    { "zoom_5x", 0x135 },
    { "zoom_auto", 0x130 },
    { "teleport", 0x320 },
    { "teleport_left", 0x325 },
    { "teleport_up", 0x326 },
    { "teleport_right", 0x327 },
    { "teleport_down", 0x328 },
    { "coordinate_left", 0x125 },
    { "coordinate_up", 0x126 },
    { "coordinate_right", 0x127 },
    { "coordinate_down", 0x128 },
    { "mouse_spawn", 0x401 },
    { "mouse_spawn_throw", 0x401 },
    { "mouse_teleport", 0x402 },
    { "mouse_teleport_throw", 0x402 },
    { "mouse_grab", 0x403 },
    { "mouse_grab_unsafe", 0x603 },
    { "mouse_grab_throw", 0x503 },
    { "mouse_zap", 0x404 },
    { "mouse_blast", 0x504 },
    { "mouse_boom", 0x0 },
    { "mouse_big_boom", 0x604 },
    { "mouse_nuke", 0x704 },
    { "mouse_clone", 0x505 },
    { "mouse_destroy", 0x405 },
    { "mouse_destroy_unsafe", 0x605 },
    //{ "", 0x },
};


std::map<std::string, std::string> windows;

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
struct CXXEntityItem
{
    std::string name;
    uint16_t id;

    CXXEntityItem(std::string name, uint16_t id) : name(name), id(id) {}

    bool operator < (const CXXEntityItem& item) const
    {
        return id < item.id;
    }
};

float g_x = 0, g_y = 0, g_vx = 0, g_vy = 0, g_zoom = 13.5, g_hue = 0;
ImVec2 startpos;
int g_held_entity = 0, g_last_entity = 0, g_current_item = 0, g_filtered_count = 0, g_level = 1, g_world = 1, g_to = 0, g_last_frame = 0, g_last_gun = 0;
unsigned int g_entity_flags = 0, g_hud_flags = 8, g_last_hud_flags = 8;
std::vector<CXXEntityItem> g_items;
std::vector<int> g_filtered_items;
bool set_focus_entity = false, set_focus_world = false, set_focus_zoom = false, scroll_to_entity = false, scroll_top = false, click_spawn = false, click_teleport = false, hidegui = false, clickevents = false, file_written = false, god = false, hidedebug = true, snap_to_grid = false, throw_held = false, paused = false, disable_input = true, capture_last = false, register_keys = false, reset_windows = false, reset_windows_vertical = false, show_app_metrics = false, change_colors = false, hud_allow_pause = true;

static char text[500];
const char* themes[] = { "1: Dwelling", "2: Jungle", "2: Volcana", "3: Olmec", "4: Tide Pool", "4: Temple", "5: Ice Caves", "6: Neo Babylon", "7: Sunken City", "8: Cosmic Ocean", "4: City of Gold", "4: Duat", "4: Abzu", "6: Tiamat", "7: Eggplant World", "7: Hundun" };
const char* entity_flags[] = { "1: Invisible", "2: ", "3: ", "4: Passes through objects", "5: Passes through everything", "6: Take no damage", "7: Throwable/Knockbackable", "8: ", "9: ", "10: ", "11: ", "12: ", "13: Collides walls", "14: ", "15: Can be stomped", "16: ", "17: Facing left", "18: Pickupable", "19: ", "20: Enterable (door)", "21: ", "22: ", "23: ", "24: ", "25: Passes through player", "26: ", "27: ", "28: Pause AI and physics", "29: Dead", "30: ", "31: ", "32: " };
const char* inifile = "imgui.ini";
const std::string hotkeyfile = "hotkeys.ini";

ImVec4 hue_shift(ImVec4 in, float hue)
{
    float U = cos(hue * 3.14159265 / 180);
    float W = sin(hue * 3.14159265 / 180);
    ImVec4 out = ImVec4(
        (.299+.701*U+.168*W)*in.x
        + (.587-.587*U+.330*W)*in.y
        + (.114-.114*U-.497*W)*in.z,
        (.299-.299*U-.328*W)*in.x
        + (.587+.413*U+.035*W)*in.y
        + (.114-.114*U+.292*W)*in.z,
        (.299-.3*U+1.25*W)*in.x
        + (.587-.588*U-1.05*W)*in.y
        + (.114+.886*U-.203*W)*in.z,
        ((float) rand() / RAND_MAX)*0.5+0.5
    );
    return out;
}

void set_colors()
{
    ImVec4* colors = ImGui::GetStyle().Colors;
    for (int i = 0; i < ImGuiCol_COUNT; i++)
    {
        ImVec4 color = colors[i];
        ImVec4 new_color = hue_shift(color, g_hue);
        colors[i] = new_color;
    }
    ImGuiStyle& style = ImGui::GetStyle();
    style.Alpha = ((float) rand() / RAND_MAX)*0.4+0.5;
    style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_TitleBg] = style.Colors[ImGuiCol_WindowBg];
    style.Colors[ImGuiCol_ScrollbarBg] = style.Colors[ImGuiCol_WindowBg];
    style.Colors[ImGuiCol_ScrollbarGrab] = style.Colors[ImGuiCol_FrameBg];
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = style.Colors[ImGuiCol_FrameBgHovered];
    style.Colors[ImGuiCol_ScrollbarGrabActive] = style.Colors[ImGuiCol_FrameBgActive];
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0, 0, 0, 0);
    style.WindowPadding = ImVec2(4, 4);
    style.WindowRounding = 0;
    style.FrameRounding = 0;
    style.PopupRounding = 0;
    style.GrabRounding = 0;
    style.WindowBorderSize = 0;
    style.FrameBorderSize = 0;
    style.PopupBorderSize = 0;
}

bool process_keys(
    _In_ int nCode,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam);

bool process_resizing(
    _In_ int nCode,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam);

std::string key_string(int keycode)
{
    UCHAR virtualKey = keycode & 0xff;
    CHAR szName[128];
    int result = 0;
    std::string name;
    if(keycode & 0xff == 0) {
        name = "Disabled";
    }
    else if(!(keycode & 0x400)) // keyboard
    {
        UINT scanCode = MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC);
        switch (virtualKey)
        {
            case VK_LEFT: case VK_UP: case VK_RIGHT: case VK_DOWN:
            case VK_RCONTROL: case VK_RMENU:
            case VK_LWIN: case VK_RWIN: case VK_APPS:
            case VK_PRIOR: case VK_NEXT:
            case VK_END: case VK_HOME:
            case VK_INSERT: case VK_DELETE:
            case VK_DIVIDE:
            case VK_NUMLOCK:
                scanCode |= KF_EXTENDED;
            default:
                result = GetKeyNameTextA(scanCode << 16, szName, 128);
        }
        if(result == 0)
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

    if(keycode & 0x200)
    {
        name = "Shift+"+name;
    }
    if(keycode & 0x100)
    {
        name = "Ctrl+"+name;
    }
    return name;
}

void save_hotkeys(std::string file)
{
    std::ofstream writeData (file);
    writeData << "# Overlunky hotkeys" << std::endl
        << "# Syntax:" << std::endl << "# function = keycode_in_hex" << std::endl
        << "# For modifiers, add 0x100 for Ctrl or 0x200 for Shift" << std::endl
        << "# For mouse buttons, add 0x400" << std::endl
        << "# Set to 0x0 to disable key" << std::endl
        << "# Example: G is 0x47, so Ctrl+G is 0x147, 0x402 is Mouse2 etc" << std::endl
        << "# Get more hex keycodes from https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes" << std::endl
        << "# If you mess this file up, you can just delete it and run overlunky to get the defaults back" << std::endl;
    for (const auto& kv : keys)
    {
        writeData << std::left << std::setw(24) << kv.first << " = " << std::hex << "0x" << std::setw(8) << kv.second << "# " << key_string(keys[kv.first])<< std::endl;
    }
    writeData.close();
}

void load_hotkeys(std::string file)
{
    std::ifstream data(file);
    if(!data.fail())
    {
        std::string line;
        char inikey[32];
        int inival;
        while(std::getline(data, line))
        {
            if(line[0] != '#')
            {
                if(sscanf(line.c_str(), "%s = %i", inikey, &inival))
                {
                    std::string keyname(inikey);
                    if(keys.find(keyname) != keys.end())
                    {
                        keys[keyname] = inival;
                    }
                }
            }
        }
        data.close();
    }
    save_hotkeys(file);
}

HWND FindTopWindow(DWORD pid)
{
    std::pair<HWND, DWORD> params = { 0, pid };

    // Enumerate the windows using a lambda to process each window
    BOOL bResult = EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL 
    {
        auto pParams = (std::pair<HWND, DWORD>*)(lParam);

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
    }, (LPARAM)&params);

    if (!bResult && GetLastError() == -1 && params.first)
    {
        return params.first;
    }

    return 0;
}

LRESULT CALLBACK hkWndProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    if(process_resizing(message, wParam, lParam)) {
        gamewindow = FindTopWindow(GetCurrentProcessId());
        return CallWindowProc(orig_wndproc, window, message, wParam, lParam);
    }
    if(!process_keys(message, wParam, lParam)) {
        ImGui_ImplWin32_WndProcHandler(window, message, wParam, lParam);
    }
    if(ImGui::GetIO().WantCaptureKeyboard && message == WM_KEYDOWN) {
        return 0;
    } else {
        return CallWindowProc(orig_wndproc, window, message, wParam, lParam);
    }
}

LRESULT CALLBACK msg_hook(
    _In_ int nCode,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam)
{
    auto msg = (MSG *)(lParam);

    if (msg->hwnd != window)
        return 0;

    if(process_keys(msg->message, msg->wParam, msg->lParam))
        return 0;
    if(process_resizing(msg->message, msg->wParam, msg->lParam))
        return 0;

    return ImGui_ImplWin32_WndProcHandler(msg->hwnd, msg->message, msg->wParam, msg->lParam);
    // TODO: if ImGui::GetIO().WantCaptureKeyboard == true, can we block keyboard message going to existing WndProc?
}

bool toggle(std::string tool) {
    const char* name = windows[tool].c_str();
    ImGuiContext& g = *GImGui;
    ImGuiWindow* current = g.NavWindow;
    ImGuiWindow* win = ImGui::FindWindowByName(name);
    if(win != NULL) {
        if(win->Collapsed || win != current) {
            win->Collapsed = false;
            ImGui::FocusWindow(win);
            //ImGui::CaptureMouseFromApp(true);
            return true;
        } else {
            win->Collapsed = true;
            ImGui::FocusWindow(NULL);
            //ImGui::CaptureMouseFromApp(false);
        }
    }
    return false;
}

void escape() {
    ImGui::SetWindowFocus(nullptr);
    /*ImGuiWindow* win = ImGui::FindWindowByName("Clickhandler");
    ImGui::FocusWindow(win);*/
}

void spawn_entities(bool s) {
    if(g_current_item == 0 && g_filtered_count == g_items.size()) return;
    std::string search(text);
    const auto pos = search.find_first_of(" ");
    if(pos == std::string::npos && g_filtered_count > 0) {
        g_last_entity = spawn_entity(g_items[g_filtered_items[g_current_item]].id, g_x, g_y, s, g_vx, g_vy, snap_to_grid);
        g_entity_flags = get_entity_flags(g_last_entity);
    } else {
        std::string texts(text);
        std::stringstream textss(texts);
        int id;
        std::vector<int> ents;
        while(textss >> id) {
            g_last_entity = spawn_entity(id, g_x, g_y, s, g_vx, g_vy, snap_to_grid);
            g_entity_flags = get_entity_flags(g_last_entity);
        }
    }
}

int pick_selected_entity(ImGuiInputTextCallbackData* data)
{
    if (data->EventFlag == ImGuiInputTextFlags_CallbackCompletion)
    {
        if(g_filtered_count == 0) return 1;
        if(g_current_item == 0 && g_filtered_count == g_items.size()) return 1;
        std::string search(text);
        while(!search.empty() && std::isspace(search.back())) search.pop_back();
        const auto pos = search.find_last_of(" ");
        if(pos == std::string::npos)
        {
            search = "";
        } else {
            search = search.substr(0, pos)+" ";
        }
        std::stringstream searchss;
        searchss << search << g_items[g_filtered_items[g_current_item]].id << " ";
        search = searchss.str();
        data->DeleteChars(0, data->BufTextLen);
        data->InsertChars(0, search.data());
    }
    return 0;
}

void set_zoom() {
    zoom(g_zoom);
}

void force_hud_flags() {
    g_hud_flags = get_hud_flags();
    if(hud_allow_pause) g_hud_flags |= 1UL << 3;
    else g_hud_flags &= ~(1UL << 3);
    set_hud_flags(g_hud_flags);
}

LRESULT CALLBACK window_hook(
    _In_ int nCode,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam)
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
    if(keys.find(keyname) == keys.end() || keys[keyname] & 0xff == 0)
    {
        return false;
    }
    int keycode = keys[keyname];
    if(GetAsyncKeyState(VK_CONTROL))
    {
        wParam += 0x100;
    }
    if(GetAsyncKeyState(VK_SHIFT))
    {
        wParam += 0x200;
    }
    return wParam == keycode;
}

bool clicked(std::string keyname) {
    int wParam = 0x400;
    if(keys.find(keyname) == keys.end() || keys[keyname] & 0xff == 0)
    {
        return false;
    }
    int keycode = keys[keyname];
    if(GetAsyncKeyState(VK_CONTROL))
    {
        wParam += 0x100;
    }
    if(GetAsyncKeyState(VK_SHIFT))
    {
        wParam += 0x200;
    }
    for(int i = 0; i < ImGuiMouseButton_COUNT; i++)
    {
        if(ImGui::IsMouseClicked(i))
        {
            wParam += i+1;
            break;
        }
    }
    return wParam == keycode;
}

bool held(std::string keyname) {
    int wParam = 0x400;
    if(keys.find(keyname) == keys.end() || keys[keyname] & 0xff == 0)
    {
        return false;
    }
    int keycode = keys[keyname];
    if(GetAsyncKeyState(VK_CONTROL))
    {
        wParam += 0x100;
    }
    if(GetAsyncKeyState(VK_SHIFT))
    {
        wParam += 0x200;
    }
    for(int i = 0; i < ImGuiMouseButton_COUNT; i++)
    {
        if(ImGui::IsMouseDown(i))
        {
            wParam += i+1;
            break;
        }
    }
    return wParam == keycode;
}

bool released(std::string keyname) {
    int wParam = 0x400;
    if(keys.find(keyname) == keys.end() || keys[keyname] & 0xff == 0)
    {
        return false;
    }
    int keycode = keys[keyname];
    if(GetAsyncKeyState(VK_CONTROL))
    {
        wParam += 0x100;
    }
    if(GetAsyncKeyState(VK_SHIFT))
    {
        wParam += 0x200;
    }
    for(int i = 0; i < ImGuiMouseButton_COUNT; i++)
    {
        if(ImGui::IsMouseReleased(i))
        {
            wParam += i+1;
            break;
        }
    }
    return wParam == keycode;
}

bool active(std::string window)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* current = g.NavWindow;
    return current == ImGui::FindWindowByName(windows[window].c_str());
}

bool process_keys(
    _In_ int nCode,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam)
{
    if(nCode != WM_KEYDOWN)
    {
        return false;
    }

    if(pressed("hide_ui", wParam)) {
        hidegui = !hidegui;
    }
    else if (pressed("tool_entity", wParam))
    {
        if(toggle("tool_entity")) {
            set_focus_entity = true;
        }
    }
    else if (pressed("tool_door", wParam))
    {
        if(toggle("tool_door")) {
            set_focus_world = true;
        }
    }
    else if (pressed("tool_camera", wParam))
    {
        if(toggle("tool_camera")) {
            set_focus_zoom = true;
        }
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
        god = !god;
        godmode(god);
    }
    else if (pressed("toggle_mouse", wParam))
    {
        clickevents = !clickevents;
    }
    else if (pressed("toggle_snap", wParam))
    {
        snap_to_grid = !snap_to_grid;
    }
    else if (pressed("toggle_pause", wParam))
    {
        paused = !paused;
        if(paused) set_pause(0x20);
        else set_pause(0);
    }
    else if (pressed("toggle_allow_pause", wParam))
    {
        hud_allow_pause = !hud_allow_pause;
        force_hud_flags();
    }
    else if (pressed("toggle_disable_input", wParam))
    {
        disable_input = !disable_input;
    }
    else if (pressed("teleport_left", wParam))
    {
        teleport(-3, 0, false, 0, 0, snap_to_grid);
    }
    else if (pressed("teleport_right", wParam))
    {
        teleport(3, 0, false, 0, 0, snap_to_grid);
    }
    else if (pressed("teleport_up", wParam))
    {
        teleport(0, 3, false, 0, 0, snap_to_grid);
    }
    else if (pressed("teleport_down", wParam))
    {
        teleport(0, -3, false, 0, 0, snap_to_grid);
    }
    else if (pressed("spawn_layer_door", wParam))
    {
        spawn_backdoor(0.0, 0.0);
    }
    else if(pressed("teleport", wParam))
    {
        teleport(g_x, g_y, false, 0, 0, snap_to_grid);
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
        spawn_door(0.0, 0.0, g_world, g_level, 1, g_to+1);
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
        ImGuiContext& g = *GImGui;
        ImGuiWindow* current = g.NavWindow;
        int page = std::max((int)((current->Size.y-100) / ImGui::GetTextLineHeightWithSpacing() / 2), 1);
        g_current_item = std::min(std::max(g_current_item - page, 0), g_filtered_count - 1);
        scroll_to_entity = true;
    }
    else if (pressed("move_pagedown", wParam) && active("tool_entity"))
    {
        ImGuiContext& g = *GImGui;
        ImGuiWindow* current = g.NavWindow;
        int page = std::max((int)((current->Size.y-100) / ImGui::GetTextLineHeightWithSpacing() / 2), 1);
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
        spawn_door(0.0, 0.0, g_world, g_level, 1, g_to+1);
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
    else if(pressed("tool_debug", wParam))
    {
        hidedebug = !hidedebug;
    }
    else if(pressed("reset_windows", wParam))
    {
        reset_windows = true;
    }
    else if(pressed("reset_windows_vertical", wParam))
    {
        reset_windows = true;
        reset_windows_vertical = true;
    }
    else if(pressed("save_settings", wParam))
    {
        ImGui::SaveIniSettingsToDisk(inifile);
        save_hotkeys(hotkeyfile);
    }
    else if(pressed("load_settings", wParam))
    {
        ImGui::LoadIniSettingsFromDisk(inifile);
        load_hotkeys(hotkeyfile);
    }
    else if(pressed("set_colors", wParam))
    {
        change_colors = true;
    }
    else if(pressed("tool_metrics", wParam))
    {
        show_app_metrics = !show_app_metrics;
    }
    else if(pressed("escape", wParam))
    {
        escape();
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

    freopen("CONOUT$", "w", stdout);

    /*if (!SetWindowsHookExA(WH_GETMESSAGE, msg_hook, 0, GetCurrentThreadId()))
    {
        printf("Message hook error: 0x%x\n", GetLastError());
    }
    if (!SetWindowsHookExA(WH_CALLWNDPROC, window_hook, 0, GetCurrentThreadId()))
    {
        printf("WndProc hook error: 0x%x\n", GetLastError());
    }*/
}

std::string last_word(std::string str)
{
    while(!str.empty() && std::isspace(str.back())) str.pop_back();
    const auto pos = str.find_last_of(" ");
    return pos == std::string::npos ? str : str.substr(pos+1);
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
    catch(const std::exception &err)
    {
    }
    for (int i = 0; i < g_items.size(); i++)
    {
        if (s[0] == '\0' || std::isspace(search.back()) || StrStrIA(g_items[i].name.data(), last.data()) || g_items[i].id == searchid)
        {
            if(g_items[i].id == 0 && s[0] != '\0') continue;
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

void render_list()
{
    // ImGui::ListBox with filter
    if (!ImGui::ListBoxHeader("##Entities", {-1, -1}))
        return;
    bool value_changed = false;
    ImGuiListClipper clipper;
    clipper.Begin(g_filtered_count, ImGui::GetTextLineHeightWithSpacing());
    if(scroll_top)
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
            std::string item_concat = item_id+": "+item_name.substr(9);
            const char *item_text = item_concat.c_str();
            ImGui::PushID(i);
            if (ImGui::Selectable(item_text, item_selected))
            {
                g_current_item = i;
                value_changed = true;
            }
            if (item_selected)
            {
                if(scroll_to_entity)
                {
                    ImGui::SetScrollHereY();
                    scroll_to_entity = false;
                }
                //ImGui::SetItemDefaultFocus();
            }
            ImGui::PopID();
        }
    }
    ImGui::ListBoxFooter();
}

void render_themes()
{
    // ImGui::ListBox with filter
    if (!ImGui::BeginCombo("##Theme", themes[g_to]))
        return;
    bool value_changed = false;
    for (int i = 0; i < 16; i++)
    {
        const bool item_selected = (i == g_to);
        const char* item_text = themes[i];

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

void render_input()
{
    if (set_focus_entity)
    {
        ImGui::SetKeyboardFocusHere();
        set_focus_entity = false;
    }
    ImVec2 region = ImGui::GetContentRegionMax();
    ImGui::PushItemWidth(region.x-70);
    if (ImGui::InputText("##Input", text, sizeof(text), ImGuiInputTextFlags_CallbackCompletion, pick_selected_entity))
    {
        update_filter(text);
    }
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::PushItemWidth(60);
    if(ImGui::Button("Spawn")) {
        spawn_entities(false);
    }
    ImGui::PopItemWidth();
}

void render_narnia()
{
    static char world[3];
    static char level[3];
    static int from = 0;
    static int to = 0;
    ImGui::Text("Area");
    ImGui::SameLine(50);
    ImGui::Text("Level");
    ImGui::SameLine(100);
    ImGui::Text("Theme");
    ImGui::SetNextItemWidth(40);
    if(set_focus_world) {
        ImGui::SetKeyboardFocusHere();
        set_focus_world = false;
    }
    if(ImGui::InputText("##World", world, sizeof(world), ImGuiInputTextFlags_CharsDecimal, NULL)) {
        g_world = atoi(world);
        if(g_world < 1) {
            g_world = 1;
        }
    }
    ImGui::SameLine(50);
    ImGui::SetNextItemWidth(44);
    if(ImGui::InputText("##Level", level, sizeof(level), ImGuiInputTextFlags_CharsDecimal, NULL)) {
        g_level = atoi(level);
        if(g_level < 1) {
            g_level = 1;
        }
    }
    ImGui::SameLine(100);
    ImGui::SetNextItemWidth(200);
    render_themes();
    if(ImGui::Button("Spawn warp door")) {
        spawn_door(g_x, g_y, g_world, g_level, 1, g_to+1);
    }
    ImGui::SameLine();
    if(ImGui::Button("Spawn layer door")) {
        spawn_backdoor(g_x, g_y);
    }
}

void render_camera()
{
    if (set_focus_zoom)
    {
        ImGui::SetKeyboardFocusHere();
        set_focus_zoom = false;
    }
    ImVec2 region = ImGui::GetContentRegionMax();
    ImGui::PushItemWidth(50);
    ImGui::Text("Zoom:");
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::PushItemWidth(region.x-60);
    if(ImGui::InputFloat("##ZoomLevel", &g_zoom, 1.0, 1.0, 2, 0)) {
        set_zoom();
    }
    ImGui::PopItemWidth();
    ImGui::Text("Set to:");
    ImGui::SameLine();
    if(ImGui::Button("Default")) {
        g_zoom = 13.5;
        set_zoom();
    }
    ImGui::SameLine();
    if(ImGui::Button("3x")) {
        g_zoom = 23.08;
        set_zoom();
    }
    ImGui::SameLine();
    if(ImGui::Button("4x")) {
        g_zoom = 29.87;
        set_zoom();
    }
    ImGui::SameLine();
    if(ImGui::Button("5x")) {
        g_zoom = 36.66;
        set_zoom();
    }
    ImGui::SameLine();
    if(ImGui::Button("Auto")) {
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
    float width = 10+length/15;
    float tpoint = width / (2 * (tanf(theta) / 2) * length);
    ImVec2 point = ImVec2(pos.x + (-tpoint * line.x), pos.y + (-tpoint * line.y));
    ImVec2 normal = ImVec2(-line.x, line.y);
    float tnormal = width / (2 * length);
    ImVec2 leftpoint = ImVec2(point.x + tnormal * normal.y, point.y + tnormal * normal.x);
    ImVec2 rightpoint = ImVec2(point.x + (-tnormal * normal.y), point.y + (-tnormal * normal.x));
    auto* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddLine(ImVec2(startpos.x-9, startpos.y-9), ImVec2(startpos.x+10, startpos.y+10), ImColor(255, 255, 255, 200), 2);
    draw_list->AddLine(ImVec2(startpos.x-9, startpos.y+9), ImVec2(startpos.x+10, startpos.y-10), ImColor(255, 255, 255, 200), 2);
    draw_list->AddLine(startpos, pos, ImColor(255, 0, 0, 200), 2);
    draw_list->AddLine(leftpoint, ImVec2(pos.x, pos.y), ImColor(255, 0, 0, 200), 2);
    draw_list->AddLine(rightpoint, ImVec2(pos.x, pos.y), ImColor(255, 0, 0, 200), 2);
}

void render_cross()
{
    ImGuiIO &io = ImGui::GetIO();
    ImVec2 res = io.DisplaySize;
    ImVec2 pos = ImGui::GetMousePos();
    auto* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddLine(ImVec2(startpos.x-9, startpos.y-9), ImVec2(startpos.x+10, startpos.y+10), ImColor(255, 255, 255, 200), 2);
    draw_list->AddLine(ImVec2(startpos.x-9, startpos.y+9), ImVec2(startpos.x+10, startpos.y-10), ImColor(255, 255, 255, 200), 2);
}

ImVec2 normalize(ImVec2 pos)
{
    ImGuiIO &io = ImGui::GetIO();
    ImVec2 res = io.DisplaySize;
    if(res.x/res.y > 1.78)
    {
        pos.x -= (res.x-res.y/9*16)/2;
        res.x = res.y/9*16;
    }
    else if(res.x/res.y < 1.77)
    {
        pos.y -= (res.y-res.x/16*9)/2;
        res.y = res.x/16*9;
    }
    ImVec2 normal = ImVec2((pos.x-res.x/2)*(1.0/(res.x/2)), -(pos.y-res.y/2)*(1.0/(res.y/2)));
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
    g_vx = 2*(g_vx - g_x);
    g_vy = 2*(g_vy - g_y)*0.5625;
}

void render_clickhandler()
{
    if(clickevents)
    {
        ImGuiIO &io = ImGui::GetIO();
        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::SetNextWindowPos({0, 0});
        ImGui::Begin("Clickhandler", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
        ImGui::InvisibleButton("canvas", ImGui::GetContentRegionMax(), ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);

        if((clicked("mouse_spawn_throw") || clicked("mouse_teleport_throw")) && ImGui::IsWindowFocused())
        {
            io.MouseDrawCursor = false;
            startpos = ImGui::GetMousePos();
        }
        else if((clicked("mouse_spawn") || clicked("mouse_teleport")) && ImGui::IsWindowFocused())
        {
            io.MouseDrawCursor = false;
            startpos = ImGui::GetMousePos();
        }
        else if((held("mouse_spawn_throw") || held("mouse_teleport_throw")) && ImGui::IsWindowFocused())
        {
            render_arrow();
        }
        else if((held("mouse_spawn") || held("mouse_teleport")) && ImGui::IsWindowFocused())
        {
            startpos = ImGui::GetMousePos();
            render_cross();
        }
        else if(released("mouse_spawn_throw") && ImGui::IsWindowFocused())
        {
            set_pos(startpos);
            set_vel(ImGui::GetMousePos());
            spawn_entities(true);
            g_x = 0; g_y = 0; g_vx = 0; g_vy = 0;
        }
        else if(released("mouse_spawn") && ImGui::IsWindowFocused())
        {
            set_pos(startpos);
            spawn_entities(true);
            g_x = 0; g_y = 0; g_vx = 0; g_vy = 0;
        }
        else if(released("mouse_teleport_throw") && ImGui::IsWindowFocused())
        {
            set_pos(startpos);
            set_vel(ImGui::GetMousePos());
            teleport(g_x, g_y, true, g_vx, g_vy, snap_to_grid);
            g_x = 0; g_y = 0; g_vx = 0; g_vy = 0;
        }
        else if(released("mouse_teleport") && ImGui::IsWindowFocused())
        {
            set_pos(startpos);
            teleport(g_x, g_y, true, g_vx, g_vy, snap_to_grid);
            g_x = 0; g_y = 0; g_vx = 0; g_vy = 0;
        }
        else if(clicked("mouse_grab") || clicked("mouse_grab_unsafe"))
        {
            ImVec2 pos = ImGui::GetMousePos();
            set_pos(pos);
            unsigned int mask = 0b01111111;
            if(held("mouse_grab_unsafe"))
            {
                mask = 0xffffffff;
            }
            g_held_entity = get_entity_at(g_x, g_y, true, 2, mask);
            g_entity_flags = get_entity_flags(g_held_entity);
            g_entity_flags |= 1 << 4;
            set_entity_flags(g_held_entity, g_entity_flags);
            g_x = 0; g_y = 0; g_vx = 0; g_vy = 0;
            g_last_entity = g_held_entity;
            startpos = pos;
        }
        else if(held("mouse_grab_throw") && g_held_entity > 0)
        {
            if(!throw_held)
            {
                startpos = ImGui::GetMousePos();
                throw_held = true;
            }
            set_pos(startpos);
            move_entity(g_held_entity, g_x, g_y, true, 0, 0, false);
            render_arrow();
        }
        else if((held("mouse_grab") || held("mouse_grab_unsafe")) && g_held_entity > 0)
        {
            startpos = ImGui::GetMousePos();
            throw_held = false;
            io.MouseDrawCursor = false;
            set_pos(ImGui::GetMousePos());
            move_entity(g_held_entity, g_x, g_y, true, 0, 0, false);
        }
        if(released("mouse_grab_throw") && g_held_entity > 0)
        {
            throw_held = false;
            io.MouseDrawCursor = true;
            g_entity_flags = get_entity_flags(g_held_entity);
            g_entity_flags &= ~(1 << 4);
            set_entity_flags(g_held_entity, g_entity_flags);
            set_pos(startpos);
            set_vel(ImGui::GetMousePos());
            move_entity(g_held_entity, g_x, g_y, true, g_vx, g_vy, snap_to_grid);
            g_x = 0; g_y = 0; g_vx = 0; g_vy = 0; g_held_entity = 0;
        }
        else if((released("mouse_grab") || released("mouse_grab_unsafe")) && g_held_entity > 0)
        {
            throw_held = false;
            io.MouseDrawCursor = true;
            g_entity_flags = get_entity_flags(g_held_entity);
            g_entity_flags &= ~(1 << 4);
            set_entity_flags(g_held_entity, g_entity_flags);
            move_entity(g_held_entity, g_x, g_y, true, 0, 0, snap_to_grid);
            g_x = 0; g_y = 0; g_vx = 0; g_vy = 0; g_held_entity = 0;
        }
        else if(released("mouse_clone"))
        {
            set_pos(ImGui::GetMousePos());
            spawn_entity(426, g_x, g_y, true, 0, 0, snap_to_grid);
            g_x = 0; g_y = 0; g_vx = 0; g_vy = 0;
        }
        else if(held("mouse_zap") && ImGui::GetFrameCount() > g_last_gun + ImGui::GetIO().Framerate/5)
        {
            g_last_gun = ImGui::GetFrameCount();
            set_pos(ImGui::GetMousePos());
            set_vel(ImVec2(ImGui::GetMousePos().x, ImGui::GetMousePos().y+200));
            spawn_entity(380, g_x, g_y, true, g_vx, g_vy, snap_to_grid);
            g_x = 0; g_y = 0; g_vx = 0; g_vy = 0;
        }
        else if(held("mouse_blast") && ImGui::GetFrameCount() > g_last_gun + ImGui::GetIO().Framerate/10)
        {
            g_last_gun = ImGui::GetFrameCount();
            set_pos(ImGui::GetMousePos());
            spawn_entity(687, g_x, g_y, true, g_vx, g_vy, snap_to_grid);
            g_x = 0; g_y = 0; g_vx = 0; g_vy = 0;
        }
        else if(held("mouse_boom") && ImGui::GetFrameCount() > g_last_gun + ImGui::GetIO().Framerate/5)
        {
            g_last_gun = ImGui::GetFrameCount();
            set_pos(ImGui::GetMousePos());
            spawn_entity(630, g_x, g_y, true, g_vx, g_vy, snap_to_grid);
            g_x = 0; g_y = 0; g_vx = 0; g_vy = 0;
        }
        else if(held("mouse_big_boom") && ImGui::GetFrameCount() > g_last_gun + ImGui::GetIO().Framerate/5)
        {
            g_last_gun = ImGui::GetFrameCount();
            set_pos(ImGui::GetMousePos());
            spawn_entity(631, g_x, g_y, true, g_vx, g_vy, snap_to_grid);
            g_x = 0; g_y = 0; g_vx = 0; g_vy = 0;
        }
        else if(held("mouse_nuke") && ImGui::GetFrameCount() > g_last_gun + ImGui::GetIO().Framerate/5)
        {
            g_last_gun = ImGui::GetFrameCount();
            set_pos(ImGui::GetMousePos());
            spawn_entity(631, g_x, g_y, true, g_vx, g_vy, snap_to_grid);
            spawn_entity(631, g_x-0.2, g_y, true, g_vx, g_vy, snap_to_grid);
            spawn_entity(631, g_x+0.2, g_y, true, g_vx, g_vy, snap_to_grid);
            spawn_entity(631, g_x, g_y-0.3, true, g_vx, g_vy, snap_to_grid);
            spawn_entity(631, g_x, g_y+0.3, true, g_vx, g_vy, snap_to_grid);
            spawn_entity(631, g_x+0.15, g_y+0.2, true, g_vx, g_vy, snap_to_grid);
            spawn_entity(631, g_x-0.15, g_y+0.2, true, g_vx, g_vy, snap_to_grid);
            spawn_entity(631, g_x+0.15, g_y-0.2, true, g_vx, g_vy, snap_to_grid);
            spawn_entity(631, g_x-0.15, g_y-0.2, true, g_vx, g_vy, snap_to_grid);
            g_x = 0; g_y = 0; g_vx = 0; g_vy = 0;
        }
        else if(released("mouse_destroy") || released("mouse_destroy_unsafe"))
        {
            ImVec2 pos = ImGui::GetMousePos();
            set_pos(pos);
            unsigned int mask = 0b01111111;
            if(released("mouse_destroy_unsafe"))
            {
                mask = 0xffffffff;
            }
            g_held_entity = get_entity_at(g_x, g_y, true, 2, mask);
            if(g_held_entity > 0)
            {
                // lets just sweep this under the rug for now :D
                move_entity(g_held_entity, 0, -1000, false, 0, 0, true);
            }
            g_x = 0; g_y = 0; g_vx = 0; g_vy = 0; g_held_entity = 0;
        }
        int buttons = 0;
        for(int i = 0; i < ImGuiMouseButton_COUNT; i++) {
            if(ImGui::IsMouseDown(i))
            {
                buttons+=i;
            }
        }
        if(buttons == 0)
        {
            io.MouseDrawCursor = true;
        }
        ImGui::End();
    }
}

void render_options()
{
    ImGui::Checkbox("Mouse controls##clickevents", &clickevents);
    if(ImGui::Checkbox("God Mode##Godmode", &god)) {
        godmode(god);
    }
    ImGui::Checkbox("Snap to grid##Snap", &snap_to_grid);    
    if(ImGui::Checkbox("Pause game engine##PauseSim", &paused))
    {
        if(paused) set_pause(0x20);
        else set_pause(0);
    }
    if(ImGui::Checkbox("Allow pause menu (on lost focus)", &hud_allow_pause))
    {
        force_hud_flags();
    }
    ImGui::Checkbox("Disable game keys when typing##Typing", &disable_input);
}

void render_debug()
{
    ImGui::TextWrapped("Weird tools here. Don't ask.");
    if(ImGui::Button("List items"))
    {
        list_items();
    }
    ImGui::SameLine();
    if(ImGui::Button("Player status"))
    {
        player_status();
    }
    ImGui::Text("Entity ID:");
    ImGui::SameLine();
    ImGui::InputInt("##EntityID", &g_last_entity, 1, 1, 0);
    /*if(ImGui::Button("Get flags"))
    {
        g_entity_flags = get_entity_flags(g_last_entity);
    }
    ImGui::SameLine();
    if(ImGui::Button("Set flags"))
    {
        set_entity_flags(g_last_entity, g_entity_flags);
    }*/
    unsigned int old_flags = g_entity_flags;
    ImGui::Text("Flags:");
    for(int i = 0; i < 32; i++) {
        ImGui::CheckboxFlags(entity_flags[i], &g_entity_flags, pow(2, i));
    }
    if(old_flags != g_entity_flags)
    {
        old_flags = g_entity_flags;
        set_entity_flags(g_last_entity, g_entity_flags);
    }
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

bool process_resizing(_In_ int nCode,
                      _In_ WPARAM wParam,
                      _In_ LPARAM lParam)
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

HRESULT __stdcall hkPresent(IDXGISwapChain *pSwapChain, UINT SyncInterval, UINT Flags)
{
    static bool init = false;
    static ImFont *font;
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
        else {
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
            }

            CoTaskMemFree(fontdir);
        }

        if (!font)
        {
            font = io.Fonts->AddFontDefault();
        }

        load_hotkeys(hotkeyfile);
        windows["tool_entity"] = "Entity spawner ("+key_string(keys["tool_entity"])+")";
        windows["tool_door"] = "Door to anywhere ("+key_string(keys["tool_door"])+")";
        windows["tool_camera"] = "Camera ("+key_string(keys["tool_camera"])+")";
        windows["tool_options"] = "Options ("+key_string(keys["tool_options"])+")";
        windows["tool_debug"] = "Debug ("+key_string(keys["tool_debug"])+")";
        windows["entities"] = "##Entities";
        g_hue = ((float) rand() / RAND_MAX)*180;
        set_colors();
    }

    if(change_colors)
    {
        g_hue = ((float) rand() / RAND_MAX)*180;
        change_colors = false;
        set_colors();
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    render_clickhandler();
    ImGui::SetNextWindowSize({10, 10});
    ImGui::SetNextWindowPos({0, ImGui::GetIO().DisplaySize.y - 30});
    ImGui::Begin("Overlay", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, .1f), "OL");
    ImGui::End();
    int win_condition = ImGuiCond_FirstUseEver;
    if(reset_windows)
    {
        win_condition = ImGuiCond_Always;
    }
    float lastwidth = 0;
    float lastheight = 0;
    float toolwidth = 300*ImGui::GetIO().FontGlobalScale;
    if (!hidegui)
    {
        if(reset_windows_vertical)
        {
            ImGui::SetNextWindowSize({toolwidth, -1}, win_condition);
            ImGui::Begin(windows["tool_options"].c_str());
            ImGui::PushItemWidth(-1);
            render_options();
            ImGui::PopItemWidth();
            lastwidth += ImGui::GetWindowWidth();
            lastheight += ImGui::GetWindowHeight();
            ImGui::SetWindowPos({0, ImGui::GetIO().DisplaySize.y-lastheight}, win_condition);    
            ImGui::End();

            ImGui::SetNextWindowSize({toolwidth, -1}, win_condition);
            ImGui::Begin(windows["tool_camera"].c_str());
            ImGui::PushItemWidth(-1);
            render_camera();
            ImGui::PopItemWidth();
            lastwidth += ImGui::GetWindowWidth();
            lastheight += ImGui::GetWindowHeight();
            ImGui::SetWindowPos({0, ImGui::GetIO().DisplaySize.y-lastheight}, win_condition);    
            ImGui::End();

            ImGui::SetNextWindowSize({toolwidth, -1}, win_condition);
            ImGui::Begin(windows["tool_door"].c_str());
            ImGui::PushItemWidth(-1);
            render_narnia();
            ImGui::PopItemWidth();
            lastwidth += ImGui::GetWindowWidth();
            lastheight += ImGui::GetWindowHeight();
            ImGui::SetWindowPos({0, ImGui::GetIO().DisplaySize.y-lastheight}, win_condition);
            ImGui::End();

            ImGui::SetNextWindowSize({toolwidth, ImGui::GetIO().DisplaySize.y-lastheight}, win_condition);
            ImGui::Begin(windows["tool_entity"].c_str());
            ImGui::PushItemWidth(-1);
            ImGui::Text("Spawning at x: %+.2f, y: %+.2f", g_x, g_y);
            render_input();
            render_list();
            ImGui::PopItemWidth();
            lastwidth += ImGui::GetWindowWidth();
            lastheight += ImGui::GetWindowHeight();
            ImGui::SetWindowPos({0, 0}, win_condition);    
            ImGui::End();
        }
        else
        {
            ImGui::SetNextWindowSize({toolwidth, toolwidth-100}, win_condition);
            ImGui::SetNextWindowPos({0, 0}, win_condition);
            ImGui::Begin(windows["tool_entity"].c_str());
            ImGui::PushItemWidth(-1);
            ImGui::Text("Spawning at x: %+.2f, y: %+.2f", g_x, g_y);
            render_input();
            render_list();
            ImGui::PopItemWidth();
            lastwidth += ImGui::GetWindowWidth();
            lastheight += ImGui::GetWindowHeight();
            ImGui::End();

            ImGui::SetNextWindowSize({toolwidth, -1}, win_condition);
            ImGui::SetNextWindowPos({lastwidth, 0}, win_condition);
            ImGui::Begin(windows["tool_door"].c_str());
            ImGui::PushItemWidth(-1);
            render_narnia();
            ImGui::PopItemWidth();
            lastwidth += ImGui::GetWindowWidth();
            lastheight += ImGui::GetWindowHeight();
            ImGui::End();

            ImGui::SetNextWindowSize({toolwidth, -1}, win_condition);
            ImGui::SetNextWindowPos({lastwidth, 0}, win_condition);
            ImGui::Begin(windows["tool_camera"].c_str());
            ImGui::PushItemWidth(-1);
            render_camera();
            ImGui::PopItemWidth();
            lastwidth += ImGui::GetWindowWidth();
            lastheight += ImGui::GetWindowHeight();
            ImGui::End();

            ImGui::SetNextWindowSize({toolwidth, -1}, win_condition);
            ImGui::SetNextWindowPos({ImGui::GetIO().DisplaySize.x-toolwidth, 0}, win_condition);
            ImGui::Begin(windows["tool_options"].c_str());
            ImGui::PushItemWidth(-1);
            render_options();
            ImGui::PopItemWidth();
            lastwidth = ImGui::GetWindowWidth();
            lastheight = ImGui::GetWindowHeight();
            ImGui::End();
        }

        if(!hidedebug)
        {
            ImGui::SetNextWindowSize({toolwidth, ImGui::GetIO().DisplaySize.y-lastheight}, win_condition);
            ImGui::SetNextWindowPos({ImGui::GetIO().DisplaySize.x-toolwidth, lastheight}, win_condition);
            if(reset_windows_vertical)
            {
                ImGui::SetNextWindowSize({toolwidth, ImGui::GetIO().DisplaySize.y}, win_condition);
                ImGui::SetNextWindowPos({ImGui::GetIO().DisplaySize.x-toolwidth, 0}, win_condition);    
            }
            ImGui::Begin(windows["tool_debug"].c_str());
            ImGui::PushItemWidth(-1);
            render_debug();
            ImGui::PopItemWidth();
            ImGui::End();
        }
    }

    if(show_app_metrics)
    {
        ImGui::ShowMetricsWindow(&show_app_metrics);
        ImGui::Begin("Styles");
        ImGui::ShowStyleEditor();
        ImGui::End();
    }

    ImGui::Render();

    if(reset_windows)
    {
        reset_windows = false;
        reset_windows_vertical = false;
        ImGui::SaveIniSettingsToDisk(inifile);
    }

    if(!file_written)
        write_file();

    if(ImGui::GetFrameCount() > g_last_frame + ImGui::GetIO().Framerate/4)
    {
        if(g_zoom == 0.0) set_zoom();
        force_hud_flags();
        if(g_last_entity != 0) g_entity_flags = get_entity_flags(g_last_entity);
        g_last_frame = ImGui::GetFrameCount();
    }
    if(disable_input && capture_last == false && ImGui::GetIO().WantCaptureKeyboard && (active("tool_entity") || active("tool_door") || active("tool_camera")))
    {
        HID_RegisterDevice(window, HID_KEYBOARD);
        capture_last = true;
        /*paused = true;
        g_hud_flags = 0;
        set_pause(0x20);
        set_hud_flags(g_hud_flags);*/
    }
    else if(disable_input && capture_last == true && !ImGui::GetIO().WantCaptureKeyboard)
    {
        //HID_UnregisterDevice(HID_KEYBOARD);
        //FindTopWindow(GetCurrentProcessId());
        //HID_RegisterDevice(gamewindow, HID_KEYBOARD);
        capture_last = false;
        register_keys = true;
        /*paused = false;
        g_hud_flags = 8;
        set_pause(0);
        set_hud_flags(g_hud_flags);*/
    } else if(register_keys) {
        register_keys = false;
        gamewindow = FindTopWindow(GetCurrentProcessId());
        HID_RegisterDevice(gamewindow, HID_KEYBOARD);
    }
    if(!(active("tool_entity") || active("tool_door") || active("tool_camera")))
    {
        ImGuiIO &io = ImGui::GetIO();
        io.WantCaptureKeyboard = false;
        io.NavActive = false;
    }

    pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    return oPresent(pSwapChain, SyncInterval, Flags);
}

void create_box(rust::Vec<rust::String> names, rust::Vec<uint16_t> ids)
{
    std::vector<CXXEntityItem> new_items;
    new_items.emplace_back("ENT_TYPE_Select entity to spawn:", 0); // :D
    if (names.size())
    {
        new_items.reserve(names.size());
        for (int i = 0; i < names.size(); i++)
        {
            new_items.emplace_back(std::string(names[i].data(), names[i].size()), ids[i]);
        }
    }

    std::sort(new_items.begin(), new_items.end());

    std::vector<int> new_filtered_items(new_items.size());
    for (int i = 0; i < new_items.size(); i++)
    {
        new_filtered_items[i] = i;
    }

    // TODO: add atomic and wrap it as struct
    {
        g_current_item = 0;
        g_items = new_items;
        g_filtered_items = new_filtered_items;
        g_filtered_count = g_items.size();
    }
}

#define THROW(fmt, ...)                               \
    {                                                 \
        char buf[0x1000];                             \
        snprintf(buf, sizeof(buf), fmt, __VA_ARGS__); \
        throw std::runtime_error(strdup(buf));        \
    }

template <typename T>
PresentPtr &vtable_find(T *obj, int index)
{
    void ***ptr = reinterpret_cast<void ***>(obj);
    if (!ptr[0])
        return *reinterpret_cast<PresentPtr *>(nullptr);
    return *reinterpret_cast<PresentPtr *>(&ptr[0][index]);
}

bool init_hooks(size_t _ptr)
{
    pSwapChain = reinterpret_cast<IDXGISwapChain *>(_ptr);
    PresentPtr &ptr = vtable_find(pSwapChain, 8);
    DWORD oldProtect;
    if (!VirtualProtect(
            reinterpret_cast<LPVOID>(reinterpret_cast<uintptr_t>(&ptr) & ~0xFFF),
            0x1000, PAGE_READWRITE, &oldProtect))
        THROW("VirtualProtect error: 0x%x\n", GetLastError());

    if (!ptr)
    {
        THROW("DirectX 11 is not initialized yet.");
    }

    oPresent = ptr;
    ptr = hkPresent;
    return true;
}
