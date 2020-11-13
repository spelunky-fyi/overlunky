#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_win32.h"
#include "imgui/examples/imgui_impl_dx11.h"
#include "ui.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

ID3D11Device *pDevice;
ID3D11DeviceContext *pContext;
ID3D11RenderTargetView *mainRenderTargetView;
HWND window;

using PresentPtr = HRESULT(STDMETHODCALLTYPE *)(IDXGISwapChain *pSwapChain, UINT SyncInterval, UINT Flags);
PresentPtr oPresent;

LRESULT CALLBACK window_hook(
    _In_ int nCode,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam)
{
    IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    auto cwp = (MSG *)(lParam);

    if (cwp->hwnd != window)
        return 0;

    switch (cwp->message)
    {
    case WM_SETCURSOR:
    case WM_SYNCPAINT:
        break;
    default:
        printf("0x%x 0x%x 0x%x\n", cwp->message, cwp->wParam, cwp->lParam);
    }
    return ImGui_ImplWin32_WndProcHandler(cwp->hwnd, cwp->message, cwp->lParam, cwp->wParam);
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
    HMODULE mod;
    if (VirtualQuery(window_hook, &mbi, sizeof(mbi)))
    {
        mod = (HMODULE)mbi.AllocationBase;
    } else {
        printf("VirtualQuery Error: 0x%x\n", GetLastError());
    }

    if (!SetWindowsHookExA(WH_GETMESSAGE, window_hook, NULL, GetCurrentThreadId()))
    {
        printf("Error: 0x%x\n", GetLastError());
    }
}

int g_current_item = 0, g_items_count = 0;
const char **g_items;

HRESULT __stdcall hkPresent(IDXGISwapChain *pSwapChain, UINT SyncInterval, UINT Flags)
{
    static bool init = false;
    static ImFont *font1, *font2;
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
        font1 = io.Fonts->AddFontDefault();
        font2 = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeuib.ttf", 20.0f);
    }

    ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Entity spawner");
    ImGui::PushFont(font2);

    ImGui::SetWindowSize({500, 500}, ImGuiCond_FirstUseEver);
    ImGui::PushItemWidth(-1);
    static char text[100];
    ImGui::InputText("", text, sizeof(text), 0, NULL);
    ImGui::ListBox("", &g_current_item, g_items, g_items_count, -1);

    ImGui::PopItemWidth();
    ImGui::PopFont();
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

void create_box(rust::Vec<rust::String> items)
{
    const char **new_items, **old_items = g_items;
    int old_items_count = g_items_count;
    if (items.size())
    {
        new_items = new const char *[items.size()];
        int index = 0;
        for (auto &i : items)
        {
            new_items[index++] = my_strdup(i.data(), i.size());
        }
    }

    // TODO: add atomic and wrap it as struct
    g_current_item = 0;
    g_items = new_items;
    g_items_count = items.size();
    if (old_items)
    {
        for (int i = 0; i < old_items_count; i++)
            delete old_items[i];
        delete[] old_items;
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
