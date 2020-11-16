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
};

float g_x = 0, g_y = 0;
int g_current_item = 0, g_filtered_count = 0;
int g_level = 1, g_world = 1, g_to = 0;
std::vector<CXXEntityItem> g_items;
std::vector<int> g_filtered_items;
static char text[500];

// Set focus on search box
bool set_focus_entity = false;
bool set_focus_world = false;
bool click_spawn = false;
bool click_teleport = false;
bool hidegui = false;
bool clickevents = false;

const char* themes[] = { "1: Dwelling", "2: Jungle", "2: Volcana", "3: Olmec", "4: Tide Pool", "4: Temple", "5: Ice Caves", "6: Neo Babylon", "7: Sunken City", "8: Cosmic Ocean", "4: City of Gold", "4: Duat", "4: Abzu", "6: Tiamat", "7: Eggplant World", "7: Hundun" };

bool process_mouse(
    _In_ int nCode,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam);

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

    if (process_keys(msg->message, msg->wParam, msg->lParam))
        return 0;

    if (process_mouse(msg->message, msg->wParam, msg->lParam))
        return 0;

    if (process_resizing(msg->message, msg->wParam, msg->lParam))
        return 0;

    return ImGui_ImplWin32_WndProcHandler(msg->hwnd, msg->message, msg->wParam, msg->lParam);
    // TODO: if ImGui::GetIO().WantCaptureKeyboard == true, can we block keyboard message going to existing WndProc?
}

bool process_mouse(
    _In_ int nCode,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam)
{
    ImGuiIO &io = ImGui::GetIO();
    ImGuiWindow* win = ImGui::FindWindowByName("Entity spawner (F1)");
    if(io.WantCaptureMouse || win->Collapsed || !clickevents) {
        return false;
    }
    if(ImGui::IsMouseReleased(0) && click_spawn == false)
    {
        ImVec2 res = io.DisplaySize;
        ImVec2 pos = ImGui::GetMousePos();
        g_x = (pos.x-res.x/2)*(10/(res.x/2));
        g_y = -(pos.y-res.y/2)*(5.5/(res.y/2));
        click_spawn = true;
    }
    if(click_spawn)
    {
        click_spawn = false;
        spawn_entity(g_items[g_filtered_items[g_current_item]].id, g_x, g_y);
        g_x = 0; g_y = 0;
    }
    if(ImGui::IsMouseReleased(1) && click_teleport == false)
    {
        ImVec2 res = io.DisplaySize;
        ImVec2 pos = ImGui::GetMousePos();
        g_x = (pos.x-res.x/2)*(10.0/(res.x/2));
        g_y = -(pos.y-res.y/2)*(5.5/(res.y/2));
        click_teleport = true;
    }
    if(click_teleport)
    {
        click_teleport = false;
        teleport(g_x, g_y);
        g_x = 0; g_y = 0;
    }
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

bool toggle(const char* name) {
    ImGuiContext& g = *GImGui;
    ImGuiWindow* current = g.NavWindow;
    ImGuiWindow* win = ImGui::FindWindowByName(name);
    if(win != NULL) {
        if(win->Collapsed || win != current) {
            win->Collapsed = false;
            ImGui::FocusWindow(win);
            return true;
        } else {
            win->Collapsed = true;
            ImGui::FocusWindow(NULL);
        }
    }
    return false;
}

void spawn_entities() {
    if(g_filtered_count > 0) {
        spawn_entity(g_items[g_filtered_items[g_current_item]].id, g_x, g_y);
    } else {
        std::string texts(text);
        std::stringstream textss(texts);
        int id;
        std::vector<int> ents;
        while(textss >> id) {
            spawn_entity(id, g_x, g_y);
        }
    }
}

bool process_keys(
    _In_ int nCode,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam)
{
    if (nCode != WM_KEYDOWN)
        return false;

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

        auto ctrl = GetAsyncKeyState(VK_CONTROL);
        if(ctrl & 0x8000 && enter)
        {
            teleport(g_x, g_y);
            return true;
        }
        else if (enter && current == ImGui::FindWindowByName("Entity spawner (F1)"))
        {
            spawn_entities();
            return true;
        }
        else if (enter && current == ImGui::FindWindowByName("Door to anywhere (F2)"))
        {
            spawn_entity(770, g_x, g_y);
            spawn_door(0.0, 0.0, g_world, g_level, 1, g_to+1);
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
            const char *item_text = g_items[g_filtered_items[i]].name.data();

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
    if (ImGui::InputText("##Input", text, sizeof(text), 0, NULL))
    {
        update_filter(text);
    }
    if(ImGui::Button("Spawn entity")) {
        spawn_entities();
    }
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
    ImGui::Text("Theme (Arrows)");
    ImGui::SetNextItemWidth(40);
    if(set_focus_world) {
        ImGui::SetKeyboardFocusHere();
        set_focus_world = false;
    }
    if(ImGui::InputText("##World", world, sizeof(world), 0, NULL)) {
        g_world = atoi(world);
        if(g_world < 1) {
            g_world = 1;
        }
    }
    ImGui::SameLine(52);
    ImGui::SetNextItemWidth(44);
    if(ImGui::InputText("##Level", level, sizeof(level), 0, NULL)) {
        g_level = atoi(level);
        if(g_level < 1) {
            g_level = 1;
        }
    }
    ImGui::SameLine(100);
    ImGui::SetNextItemWidth(200);
    render_themes();
    if(ImGui::Button("Spawn door")) {
        spawn_entity(770, g_x, g_y);
        spawn_door(g_x, g_y, g_world, g_level, 1, g_to+1);
    }
    ImGui::Text("Don't use this in camp!");
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
    if(!hidegui) {
        ImGui::SetNextWindowSize({400, 400}, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos({0, 0}, ImGuiCond_FirstUseEver);
        ImGui::Begin("Entity spawner (F1)");
        ImGui::PushItemWidth(-1);
        //ImGui::Text("Ctrl+Enter or right click to teleport");
        ImGui::Text("Ctrl+Enter to teleport");
        // apparently this doesn't even work in release build, go figure
        //ImGui::Text("Left click to spawn at cursor");
        //ImGui::Checkbox("##clickevents", &clickevents);
        //ImGui::SameLine();
        //ImGui::Text("Enable click events (buggy)");
        ImGui::Text("Spawning at x: %+.2f, y: %+.2f (Ctrl+Arrow)", g_x, g_y);
        ImGui::Text("Enter many #IDs separated by space to spawn a bunch of stuff at once.");
        render_input();
        render_list();
        ImGui::PopItemWidth();
        ImGui::End();

        ImGui::SetNextWindowSize({300, 200}, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos({400, 0}, ImGuiCond_FirstUseEver);
        ImGui::Begin("Door to anywhere (F2)");
        ImGui::PushItemWidth(-1);
        ImGui::Text("Spawn a door to:");
        render_narnia();
        ImGui::PopItemWidth();
        ImGui::End();
    }

    ImGui::Render();

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
