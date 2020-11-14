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

int g_x = 0, g_y = 0;
int g_current_item = 0, g_filtered_count = 0;
std::vector<CXXEntityItem> g_items;
std::vector<int> g_filtered_items;

// Set focus on search box
bool set_focus = false;

bool process_keys(
    _In_ int nCode,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam);

LRESULT CALLBACK window_hook(
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

    return ImGui_ImplWin32_WndProcHandler(msg->hwnd, msg->message, msg->wParam, msg->lParam);
    // TODO: if ImGui::GetIO().WantCaptureKeyboard == true, can we block keyboard message going to existing WndProc?
}

bool process_keys(
    _In_ int nCode,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam)
{
    if (nCode != WM_KEYDOWN)
        return false;

    switch (ImGui::GetIO().WantCaptureKeyboard)
    {
    case false:
    {
        // Out-window keys
        if (wParam == VK_F1)
        {
            set_focus = true;
            return true;
        }
        break;
    }
    case true:
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
        case VK_F1:
            ImGui::FocusWindow(NULL);
            return true;
        }

        if (enter && g_items.size())
        {
            spawn_entity(g_items[g_filtered_items[g_current_item]].id, g_x, g_y);
            return true;
        }

        if (x == 0 && y == 0)
            return false;

        auto ctrl = GetAsyncKeyState(VK_CONTROL);
        if (ctrl & 0x8000)
        {
            g_x += x;
            g_y += y;
        }
        else
        {
            // List navigation
            if (y != 0)
                g_current_item = std::min(std::max(g_current_item - y, 0), (int)g_items.size() - 1);
            if (x == 0)
                return false;
        }
        return true;
    }
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

    MEMORY_BASIC_INFORMATION mbi;
    HMODULE hMod;
    if (VirtualQuery(window_hook, &mbi, sizeof(mbi)))
    {
        hMod = (HMODULE)mbi.AllocationBase;
    }
    else
    {
        printf("VirtualQuery Error: 0x%x\n", GetLastError());
    }

    if (!SetWindowsHookExA(WH_GETMESSAGE, window_hook, 0, GetCurrentThreadId()))
    {
        printf("Error: 0x%x\n", GetLastError());
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
    printf("%d / %d\n", count, g_items.size());
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

void render_input()
{
    static char text[100];
    if (set_focus)
    {
        ImGui::SetKeyboardFocusHere();
        set_focus = false;
    }
    if (ImGui::InputText("##Input", text, sizeof(text), 0, NULL))
    {
        update_filter(text);
    }
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
            ID3D11Texture2D *pBackBuffer;
            pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pBackBuffer);
            pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
            pBackBuffer->Release();
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

    ImGui::Begin("Entity spawner (F1)");

    ImGui::SetWindowSize({500, 500}, ImGuiCond_FirstUseEver);
    ImGui::PushItemWidth(-1);

    ImGui::Text("Spawning at x: %+d, y: %+d (Ctrl+Arrow)", g_x, g_y);
    render_input();
    render_list();

    ImGui::PopItemWidth();
    ImGui::End();

    ImGui::Render();

    pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    return oPresent(pSwapChain, SyncInterval, Flags);
}

char *my_strdup(const char *s, int size)
{
    auto result = new char[size + 1];
    memcpy(result, s, size);
    result[size] = '\0';
    return result;
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
    IDXGISwapChain *pSwapChain = reinterpret_cast<IDXGISwapChain *>(_ptr);
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
