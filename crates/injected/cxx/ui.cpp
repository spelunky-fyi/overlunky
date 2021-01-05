#define NOMINMAX
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/examples/imgui_impl_win32.h"
#include "imgui/examples/imgui_impl_dx11.h"
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

std::map<std::string, int> keys{
    { "enter", 0x0d },
    { "move_left", 0x25 },
    { "move_up", 0x26 },
    { "move_right", 0x27 },
    { "move_down", 0x28 },
    { "toggle_mouse", 0x14d },
    { "toggle_godmode", 0x147 },
    { "toggle_snap", 0x153 },
    { "tool_entity", 0x70 },
    { "tool_door", 0x71 },
    { "tool_camera", 0x72 },
    { "tool_options", 0x78 },
    { "tool_debug", 0x344 },
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
    { "teleport", 0x320 },
    { "teleport_left", 0x325 },
    { "teleport_up", 0x326 },
    { "teleport_right", 0x327 },
    { "teleport_down", 0x328 },
    { "coordinate_left", 0x125 },
    { "coordinate_up", 0x126 },
    { "coordinate_right", 0x127 },
    { "coordinate_down", 0x128 },
    //{ "", 0x },
};

std::map<std::string, std::string> windows;

IDXGISwapChain *pSwapChain;
ID3D11Device *pDevice;
ID3D11DeviceContext *pContext;
ID3D11RenderTargetView *mainRenderTargetView;
HWND window;

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

float g_x = 0, g_y = 0, g_vx = 0, g_vy = 0, g_zoom = 13.5;
ImVec2 startpos;
int g_current_item = 0, g_filtered_count = 0;
int g_level = 1, g_world = 1, g_to = 0;
std::vector<CXXEntityItem> g_items;
std::vector<int> g_filtered_items;
static char text[500];

// Set focus on search box
bool set_focus_entity = false;
bool set_focus_world = false;
bool set_focus_zoom = false;
bool scroll_to_entity = false;
bool click_spawn = false;
bool click_teleport = false;
bool hidegui = false;
bool clickevents = false;
bool file_written = false;
bool god = false;
bool hidedebug = true;
bool snap_to_grid = true;

const char* themes[] = { "1: Dwelling", "2: Jungle", "2: Volcana", "3: Olmec", "4: Tide Pool", "4: Temple", "5: Ice Caves", "6: Neo Babylon", "7: Sunken City", "8: Cosmic Ocean", "4: City of Gold", "4: Duat", "4: Abzu", "6: Tiamat", "7: Eggplant World", "7: Hundun" };

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
    UINT scanCode = MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC);

    CHAR szName[128];
    int result = 0;
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
        return "Mystery key";
    }

    std::string name(szName);
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
        << "# For example: G is 0x47, so Ctrl+G is 0x147 etc" << std::endl
        << "# Get more hex keycodes from https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes" << std::endl
        << "# If you mess this file up, you can just delete it and run overlunky to get the defaults back" << std::endl;
    for (const auto& kv : keys)
    {
        writeData << std::left << std::setw(16) << kv.first << " = " << std::hex << "0x" << std::setw(8) << kv.second << "# " << key_string(keys[kv.first])<< std::endl;
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

LRESULT CALLBACK msg_hook(
    _In_ int nCode,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam)
{
    IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
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

void spawn_entities(bool s) {
    if(g_filtered_count > 0) {
        spawn_entity(g_items[g_filtered_items[g_current_item]].id, g_x, g_y, s, g_vx, g_vy, snap_to_grid);
    } else {
        std::string texts(text);
        std::stringstream textss(texts);
        int id;
        std::vector<int> ents;
        while(textss >> id) {
            spawn_entity(id, g_x, g_y, s, g_vx, g_vy, snap_to_grid);
        }
    }
}

void set_zoom() {
    zoom(g_zoom);
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
    if(keys.find(keyname) == keys.end())
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
    else if (pressed("toggle_godmode", wParam))
    {
        god = !god;
        godmode(god);
    }
    else if (pressed("toggle_mouse", wParam)) // M
    {
        clickevents = !clickevents;
    }
    else if (pressed("toggle_snap", wParam)) // M
    {
        snap_to_grid = !snap_to_grid;
    }
    else if (pressed("teleport_left", wParam))
    {
        teleport(-3, 0, false, 0, 0);
    }
    else if (pressed("teleport_right", wParam))
    {
        teleport(3, 0, false, 0, 0);
    }
    else if (pressed("teleport_up", wParam))
    {
        teleport(0, 3, false, 0, 0);
    }
    else if (pressed("teleport_down", wParam))
    {
        teleport(0, -3, false, 0, 0);
    }
    else if (pressed("spawn_layer_door", wParam))
    {
        spawn_backdoor(0.0, 0.0);
    }
    else if(pressed("teleport", wParam))
    {
        teleport(g_x, g_y, false, 0, 0);
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
    else if (pressed("move_up", wParam) && (active("tool_entity") || active("entities")))
    {
        g_current_item = std::min(std::max(g_current_item - 1, 0), (int)g_items.size() - 1);
        scroll_to_entity = true;
    }
    else if (pressed("move_down", wParam) && (active("tool_entity") || active("entities")))
    {
        g_current_item = std::min(std::max(g_current_item + 1, 0), (int)g_items.size() - 1);
        scroll_to_entity = true;
    }
    else if (pressed("enter", wParam) && (active("tool_entity") || active("entities")))
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
    else if(pressed("tool_debug", wParam)) {
        hidedebug = !hidedebug;
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

    if (!SetWindowsHookExA(WH_GETMESSAGE, msg_hook, 0, GetCurrentThreadId()))
    {
        printf("Message hook error: 0x%x\n", GetLastError());
    }
    if (!SetWindowsHookExA(WH_CALLWNDPROC, window_hook, 0, GetCurrentThreadId()))
    {
        printf("WndProc hook error: 0x%x\n", GetLastError());
    }
}

void update_filter(const char *s)
{
    int count = 0;
    for (int i = 0; i < g_items.size(); i++)
    {
        if (s[0] == '\0' || StrStrIA(g_items[i].name.data(), s))
        {
            g_filtered_items[count++] = i;
        }
    }
    g_filtered_count = count;
    g_current_item = 0;
}

void write_file()
{
    std::ofstream file;
    file.open("entities.txt");
    for (int i = 0; i < g_items.size(); i++)
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
    while (clipper.Step())
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
                ImGui::SetItemDefaultFocus();
            }
            ImGui::PopID();
        }
    ImGui::ListBoxFooter();
}

void render_themes()
{
    // ImGui::ListBox with filter
    if (!ImGui::BeginCombo("##Theme", themes[g_to]))
        return;
    bool value_changed = false;
    ImGuiListClipper clipper;
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
    if (ImGui::InputText("##Input", text, sizeof(text), 0, NULL))
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
    ImGui::SameLine(53);
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
    ImGui::SameLine(52);
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
}

void render_clickhandler()
{
    ImGuiIO &io = ImGui::GetIO();
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::SetNextWindowPos({0, 0});
    ImGui::Begin("Clickhandler", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
    ImGui::InvisibleButton("canvas", ImGui::GetContentRegionMax(), ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);

    if((ImGui::IsMouseClicked(0) || ImGui::IsMouseClicked(1)) && ImGui::IsWindowFocused())
    {
        io.MouseDrawCursor = false;
        ImVec2 res = io.DisplaySize;
        ImVec2 pos = ImGui::GetMousePos();
        startpos = pos;
    }
    if((ImGui::IsMouseDown(0) || ImGui::IsMouseDown(1)) && ImGui::IsWindowFocused())
    {
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
    if(ImGui::IsMouseReleased(0) && ImGui::IsWindowFocused())
    {
        ImVec2 res = io.DisplaySize;
        ImVec2 pos = ImGui::GetMousePos();
        g_x = (startpos.x-res.x/2)*(1.0/(res.x/2));
        g_y = -(startpos.y-res.y/2)*(1.0/(res.y/2));
        g_vx = (pos.x-res.x/2)*(1.0/(res.x/2));
        g_vy = -(pos.y-res.y/2)*(1.0/(res.y/2));
        g_vx = (g_vx - g_x);
        g_vy = (g_vy - g_y)*0.5625;
        spawn_entities(true);
        g_x = 0; g_y = 0; g_vx = 0; g_vy = 0;

    }
    if(ImGui::IsMouseReleased(1) && ImGui::IsWindowFocused())
    {
        ImVec2 res = io.DisplaySize;
        ImVec2 pos = ImGui::GetMousePos();
        g_x = (startpos.x-res.x/2)*(1.0/(res.x/2));
        g_y = -(startpos.y-res.y/2)*(1.0/(res.y/2));
        g_vx = (pos.x-res.x/2)*(1.0/(res.x/2));
        g_vy = -(pos.y-res.y/2)*(1.0/(res.y/2));
        g_vx = (g_vx - g_x);
        g_vy = (g_vy - g_y)*0.5625;
        teleport(g_x, g_y, true, g_vx, g_vy);
        g_x = 0; g_y = 0; g_vx = 0; g_vy = 0;
    }
    if(ImGui::IsMouseReleased(0) || ImGui::IsMouseReleased(1)) {
        io.MouseDrawCursor = true;
    }
    ImGui::End();
}

void render_options()
{
    ImGui::Checkbox("##clickevents", &clickevents);
    ImGui::SameLine();
    ImGui::Text("Mouse controls");
    if(ImGui::Checkbox("##Godmode", &god)) {
        godmode(god);
    }
    ImGui::SameLine();
    ImGui::Text("God mode");
    ImGui::Checkbox("##Snap", &snap_to_grid);
    ImGui::SameLine();
    ImGui::Text("Snap entities to grid");
}

void render_debug()
{
    ImGui::Text("You're not supposed to be here!");
    if(ImGui::Button("List items"))
    {
        list_items();
    }
    ImGui::SameLine();
    if(ImGui::Button("Player status"))
    {
        player_status();
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
            create_render_target();
            init_imgui();
            init = true;
        }

        else
            return oPresent(pSwapChain, SyncInterval, Flags);

        ImGuiIO &io = ImGui::GetIO();
        PWSTR fontdir;
        if (SHGetKnownFolderPath(FOLDERID_Fonts, 0, NULL, &fontdir) == S_OK)
        {
            using cvt_type = std::codecvt_utf8<wchar_t>;
            std::wstring_convert<cvt_type, wchar_t> cvt;

            std::string fontpath(cvt.to_bytes(fontdir) + "\\segoeuib.ttf");
            if (GetFileAttributesA(fontpath.c_str()) != INVALID_FILE_ATTRIBUTES)
            {
                font = io.Fonts->AddFontFromFileTTF(fontpath.c_str(), 20.0f);
            }

            CoTaskMemFree(fontdir);
        }

        if (!font)
        {
            font = io.Fonts->AddFontDefault();
        }

        load_hotkeys("hotkeys.ini");
        windows["tool_entity"] = "Entity spawner ("+key_string(keys["tool_entity"])+")";
        windows["tool_door"] = "Door to anywhere ("+key_string(keys["tool_door"])+")";
        windows["tool_camera"] = "Camera ("+key_string(keys["tool_camera"])+")";
        windows["tool_options"] = "Options ("+key_string(keys["tool_options"])+")";
        windows["tool_debug"] = "Debug ("+key_string(keys["tool_debug"])+")";
        windows["entities"] = "##Entities";
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    if (clickevents)
    {
        render_clickhandler();
    }
    ImGui::SetNextWindowSize({10, 10});
    ImGui::SetNextWindowPos({0, ImGui::GetIO().DisplaySize.y - 30});
    ImGui::Begin("Overlay", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, .1f), "OL");
    ImGui::End();
    if (!hidegui)
    {
        ImGui::SetNextWindowSize({400, 300}, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos({0, 0}, ImGuiCond_FirstUseEver);
        ImGui::Begin(windows["tool_entity"].c_str());
        ImGui::PushItemWidth(-1);
        ImGui::Text("Spawning at x: %+.2f, y: %+.2f", g_x, g_y);
        render_input();
        render_list();
        ImGui::PopItemWidth();
        ImGui::End();

        ImGui::SetNextWindowSize({300, 125}, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos({400, 0}, ImGuiCond_FirstUseEver);
        ImGui::Begin(windows["tool_door"].c_str());
        ImGui::PushItemWidth(-1);
        render_narnia();
        ImGui::PopItemWidth();
        ImGui::End();

        ImGui::SetNextWindowSize({300, 125}, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos({700, 0}, ImGuiCond_FirstUseEver);
        ImGui::Begin(windows["tool_camera"].c_str());
        ImGui::PushItemWidth(-1);
        render_camera();
        ImGui::PopItemWidth();
        ImGui::End();

        ImGui::SetNextWindowSize({400, 150}, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos({ImGui::GetIO().DisplaySize.x-400, 0}, ImGuiCond_FirstUseEver);
        ImGui::Begin(windows["tool_options"].c_str());
        ImGui::PushItemWidth(-1);
        render_options();
        ImGui::PopItemWidth();
        ImGui::End();
    }

    if(!hidedebug)
    {
        ImGui::SetNextWindowSize({400, ImGui::GetIO().DisplaySize.y/2}, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos({ImGui::GetIO().DisplaySize.x-400, ImGui::GetIO().DisplaySize.y/2}, ImGuiCond_FirstUseEver);
        ImGui::Begin(windows["tool_debug"].c_str());
        ImGui::PushItemWidth(-1);
        render_debug();
        ImGui::PopItemWidth();
        ImGui::End();
    }

    ImGui::Render();

    if(!file_written)
        write_file();


    pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    return oPresent(pSwapChain, SyncInterval, Flags);
}

void create_box(rust::Vec<rust::String> names, rust::Vec<uint16_t> ids)
{
    std::vector<CXXEntityItem> new_items;
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
