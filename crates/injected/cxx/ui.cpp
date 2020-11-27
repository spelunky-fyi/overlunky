#define NOMINMAX
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/examples/imgui_impl_win32.h"
#include "imgui/examples/imgui_impl_dx11.h"
#include "ui.hpp"
#include "injected-dll/src/ui.rs.h"

#include <Windows.h>
#include <Shlwapi.h>
#include <algorithm>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>

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

float g_x = 0, g_y = 0, g_zoom = 13.5;
int g_current_item = 0, g_filtered_count = 0;
int g_level = 1, g_world = 1, g_to = 0;
std::vector<CXXEntityItem> g_items;
std::vector<int> g_filtered_items;
static char text[500];

// Set focus on search box
bool set_focus_entity = false;
bool set_focus_world = false;
bool set_focus_zoom = false;
bool click_spawn = false;
bool click_teleport = false;
bool hidegui = false;
bool clickevents = false;
bool file_written = false;
bool god = false;

const char* themes[] = { "1: Dwelling", "2: Jungle", "2: Volcana", "3: Olmec", "4: Tide Pool", "4: Temple", "5: Ice Caves", "6: Neo Babylon", "7: Sunken City", "8: Cosmic Ocean", "4: City of Gold", "4: Duat", "4: Abzu", "6: Tiamat", "7: Eggplant World", "7: Hundun" };

bool process_keys(
    _In_ int nCode,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam);

bool process_resizing(
    _In_ int nCode,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam);

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

bool toggle(const char* name) {
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
        spawn_entity(g_items[g_filtered_items[g_current_item]].id, g_x, g_y, s);
    } else {
        std::string texts(text);
        std::stringstream textss(texts);
        int id;
        std::vector<int> ents;
        while(textss >> id) {
            spawn_entity(id, g_x, g_y, s);
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

bool process_keys(
    _In_ int nCode,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam)
{
    if (nCode != WM_KEYDOWN)
        return false;

    auto ctrl = GetAsyncKeyState(VK_CONTROL);

    if(wParam == VK_F11) {
        hidegui = !hidegui;
        return true;
    }
    else if (wParam == VK_F1)
    {
        if(toggle("Entity spawner (F1)")) {
            set_focus_entity = true;
        }
        return true;
    }
    else if (wParam == VK_F2)
    {
        if(toggle("Door to anywhere (F2)")) {
            set_focus_world = true;
        }
        return true;
    }
    else if (wParam == VK_F3)
    {
        if(toggle("Camera (F3)")) {
            set_focus_zoom = true;
        }
        return true;
    }
    else if (wParam == VK_F9)
    {
        toggle("Help and Options (F9)");
        return true;
    }
    else if (ctrl & 0x8000 && wParam == VK_OEM_PERIOD)
    {
        g_zoom += 1.0;
        set_zoom();
        return true;
    }
    else if (ctrl & 0x8000 && wParam == VK_OEM_COMMA)
    {
        g_zoom -= 1.0;
        set_zoom();
        return true;
    }

    ImGuiContext& g = *GImGui;
    ImGuiWindow* current = g.NavWindow;

    if(ImGui::GetIO().WantCaptureKeyboard)
    { // In-window keys
        int x = 0, y = 0;
        bool enter = false;

        switch (wParam)
        {
        case VK_LEFT:
            x = -1;
            break;
        case VK_RIGHT:
            x = 1;
            break;
        case VK_UP:
            y = 1;
            break;
        case VK_DOWN:
            y = -1;
            break;
        case VK_RETURN:
            enter = true;
            break;
        }

        if(ctrl & 0x8000 && enter)
        {
            teleport(g_x, g_y, false);
            return true;
        }
        else if (enter && current == ImGui::FindWindowByName("Entity spawner (F1)"))
        {
            spawn_entities(false);
            return true;
        }
        else if (enter && current == ImGui::FindWindowByName("Door to anywhere (F2)"))
        {
            spawn_entity(775, g_x, g_y, false);
            spawn_door(0.0, 0.0, g_world, g_level, 1, g_to+1);
            return true;
        }
        else if (enter && current == ImGui::FindWindowByName("Camera (F3)"))
        {
            set_zoom();
            return true;
        }

        if (x == 0 && y == 0)
            return false;

        if (ctrl & 0x8000)
        {
            g_x += x;
            g_y += y;
        }
        else
        {
            // List navigation
            if (y != 0) {
                if(current == ImGui::FindWindowByName("Entity spawner (F1)"))
                    g_current_item = std::min(std::max(g_current_item - y, 0), (int)g_items.size() - 1);
                else if(current == ImGui::FindWindowByName("Door to anywhere (F2)"))
                    g_to = std::min(std::max(g_to - y, 0), 15);
            }
            if (x != 0)
                return false;
        }
        return true;
    }
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
                ImGui::SetItemDefaultFocus();
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
    if(ImGui::Button("Spawn")) {
        spawn_entity(775, g_x, g_y, false);
        spawn_door(g_x, g_y, g_world, g_level, 1, g_to+1);
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
    ImGui::Text("Lock to:");
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
    if(ImGui::IsMouseReleased(0) && ImGui::IsWindowFocused())
    {
        ImVec2 res = io.DisplaySize;
        ImVec2 pos = ImGui::GetMousePos();
        g_x = (pos.x-res.x/2)*(1.0/(res.x/2));
        g_y = -(pos.y-res.y/2)*(1.0/(res.y/2));
        spawn_entities(true);
        g_x = 0; g_y = 0;
    }
    if(ImGui::IsMouseReleased(1) && ImGui::IsWindowFocused())
    {
        ImVec2 res = io.DisplaySize;
        ImVec2 pos = ImGui::GetMousePos();
        g_x = (pos.x-res.x/2)*(1.0/(res.x/2));
        g_y = -(pos.y-res.y/2)*(1.0/(res.y/2));
        teleport(g_x, g_y, true);
        g_x = 0; g_y = 0;
    }
    ImGui::End();
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
    static ImFont *font1;
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
        font1 = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeuib.ttf", 20.0f);
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    if(clickevents) {
        render_clickhandler();
    }
    ImGui::SetNextWindowSize({10, 10});
    ImGui::SetNextWindowPos({0, ImGui::GetIO().DisplaySize.y-30});
    ImGui::Begin("Overlay", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, .1f), "OL");
    ImGui::End();
    if(!hidegui) {
        ImGui::SetNextWindowSize({400, 300}, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos({0, 0}, ImGuiCond_FirstUseEver);
        ImGui::Begin("Entity spawner (F1)");
        ImGui::PushItemWidth(-1);
        ImGui::Text("Spawning at x: %+.2f, y: %+.2f", g_x, g_y);
        render_input();
        render_list();
        ImGui::PopItemWidth();
        ImGui::End();

        ImGui::SetNextWindowSize({300, 125}, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos({400, 0}, ImGuiCond_FirstUseEver);
        ImGui::Begin("Door to anywhere (F2)");
        ImGui::PushItemWidth(-1);
        render_narnia();
        ImGui::PopItemWidth();
        ImGui::End();

        ImGui::SetNextWindowSize({300, 125}, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos({700, 0}, ImGuiCond_FirstUseEver);
        ImGui::Begin("Camera (F3)");
        ImGui::PushItemWidth(-1);
        render_camera();
        ImGui::PopItemWidth();
        ImGui::End();

        ImGui::SetNextWindowSize({400, 300}, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos({ImGui::GetIO().DisplaySize.x-400, 0}, ImGuiCond_FirstUseEver);
        ImGui::Begin("Help and Options (F9)");
        ImGui::PushItemWidth(-1);
        ImGui::Checkbox("##clickevents", &clickevents);
        ImGui::SameLine();
        ImGui::Text("Enable click to spawn/teleport");
        if(ImGui::Checkbox("##Godmode", &god)) {
            godmode(god);
        }
        ImGui::SameLine();
        ImGui::Text("Enable peaceful mode");
        ImGui::Text("Keys:");
        if(clickevents) {
            ImGui::Text("- (Enter) or (Mouse L) Use focused tool");
            ImGui::Text("- (Ctrl+Enter) or (Mouse R) Teleport");
        } else {
            ImGui::Text("- (Enter) Use focused tool");
            ImGui::Text("- (Ctrl+Enter) Teleport");
        }
        ImGui::Text("- (Arrows) Change selection in lists");
        ImGui::Text("- (Ctrl+Arrows) Change spawning coordinates");
        ImGui::Text("- (Ctrl+Comma/Period) Change zoom level");
        ImGui::Text("Write many numerical IDs separated by space in");
        ImGui::Text("the entity spawner to spawn many items at once.");
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
