#include "window_api.hpp"

#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>
#include <d3d11.h>
#include <imgui.h>
#include <imgui_internal.h>

#include "logger.h"
#include "state.hpp"

IDXGISwapChain *g_SwapChain{nullptr};
ID3D11Device *g_Device{nullptr};
ID3D11DeviceContext *g_Context{nullptr};
ID3D11RenderTargetView *g_MainRenderTargetView{nullptr};
HWND g_Window{nullptr};

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
WNDPROC g_OrigWndProc{nullptr};

using PresentPtr = HRESULT(STDMETHODCALLTYPE *)(IDXGISwapChain *pSwapChain, UINT SyncInterval, UINT Flags);
PresentPtr g_OrigSwapChainPresent{nullptr};

using ResizeBuffersPtr = HRESULT(STDMETHODCALLTYPE *)(IDXGISwapChain *, UINT, UINT, UINT, DXGI_FORMAT, UINT);
ResizeBuffersPtr g_OrigSwapChainResizeBuffers{nullptr};

OnInputCallback g_OnInputCallback{nullptr};
ImguiInitCallback g_ImguiInitCallback{nullptr};
ImguiDrawCallback g_ImguiDrawCallback{nullptr};
PreDrawCallback g_PreDrawCallback{nullptr};
PostDrawCallback g_PostDrawCallback{nullptr};

constexpr USHORT g_HidKeyboard = 6;
HWND g_LastRegisteredRawInputWindow{nullptr};

HWND HID_GetRegisteredDeviceWindow(USHORT usage)
{
    constexpr UINT max_raw_devices = 3;
    RAWINPUTDEVICE raw_devices[max_raw_devices];
    UINT num_raw_devices = max_raw_devices;
    UINT num_registered_raw_devices = GetRegisteredRawInputDevices(raw_devices, &num_raw_devices, sizeof(RAWINPUTDEVICE));
    if (num_registered_raw_devices > 0)
    {
        for (UINT i = 0; i < num_registered_raw_devices; i++)
        {
            if (raw_devices[i].usUsage == usage)
            {
                return raw_devices[i].hwndTarget;
            }
        }
    }
    return nullptr;
}

bool HID_RegisterDevice(HWND hTarget, USHORT usage)
{
    RAWINPUTDEVICE hid;
    hid.usUsagePage = 1;
    hid.usUsage = usage;
    hid.dwFlags = RIDEV_DEVNOTIFY | RIDEV_INPUTSINK;
    hid.hwndTarget = hTarget;

    return RegisterRawInputDevices(&hid, 1, sizeof(RAWINPUTDEVICE));
}

bool HID_UnregisterDevice(USHORT usage)
{
    RAWINPUTDEVICE hid;
    hid.usUsagePage = 1;
    hid.usUsage = usage;
    hid.dwFlags = RIDEV_REMOVE;
    hid.hwndTarget = NULL;

    return RegisterRawInputDevices(&hid, 1, sizeof(RAWINPUTDEVICE));
}

LRESULT CALLBACK hkWndProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    bool consumed_input = g_OnInputCallback ? g_OnInputCallback(message, wParam, lParam) : false;
    if (!consumed_input)
    {
        LRESULT imgui_result = ImGui_ImplWin32_WndProcHandler(window, message, wParam, lParam);
        if (imgui_result != 0)
        {
            return imgui_result;
        }
    }
    if (ImGui::GetIO().WantCaptureKeyboard && message == WM_KEYDOWN)
    {
        return DefWindowProc(window, message, wParam, lParam);
    }
    return CallWindowProc(g_OrigWndProc, window, message, wParam, lParam);
}

void init_imgui()
{
    ImGuiContext* imgui_context = ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
    io.MouseDrawCursor = true;
    ImGui_ImplWin32_Init(g_Window);
    ImGui_ImplDX11_Init(g_Device, g_Context);

    if (g_ImguiInitCallback)
    {
        g_ImguiInitCallback(imgui_context);
    }
}

void create_render_target()
{
    ID3D11Texture2D *pBackBuffer;
    g_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pBackBuffer);
    assert(pBackBuffer != nullptr);
    g_Device->CreateRenderTargetView(pBackBuffer, NULL, &g_MainRenderTargetView);
    pBackBuffer->Release();
}

void cleanup_render_target()
{
    if (g_MainRenderTargetView)
    {
        g_MainRenderTargetView->Release();
        g_MainRenderTargetView = NULL;
    }
}

HRESULT STDMETHODCALLTYPE hkPresent(IDXGISwapChain *pSwapChain, UINT SyncInterval, UINT Flags)
{
    static bool init = false;
    if (!init)
    {
        if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void **)&g_Device)))
        {
            g_Device->GetImmediateContext(&g_Context);
            DXGI_SWAP_CHAIN_DESC sd;
            pSwapChain->GetDesc(&sd);
            g_Window = sd.OutputWindow;
            g_OrigWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(g_Window, GWLP_WNDPROC, LONG_PTR(hkWndProc)));
            create_render_target();
            init_imgui();
            init = true;
        }
        else
        {
            return g_OrigSwapChainPresent(pSwapChain, SyncInterval, Flags);
        }
    }

    if (g_PreDrawCallback)
    {
        g_PreDrawCallback();
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (g_ImguiDrawCallback)
    {
        g_ImguiDrawCallback();
    }

    ImGui::Render();
    g_Context->OMSetRenderTargets(1, &g_MainRenderTargetView, NULL);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    {
        if (ImGui::GetIO().WantCaptureKeyboard)
        {
            if (HWND window = HID_GetRegisteredDeviceWindow(g_HidKeyboard))
            {
                g_LastRegisteredRawInputWindow = window;
                HID_UnregisterDevice(g_HidKeyboard);
            }
        }
        else if (g_LastRegisteredRawInputWindow != nullptr)
        {
            static std::uint32_t s_RecoverRawInputFrame{0};
            if (s_RecoverRawInputFrame == 0)
            {
                s_RecoverRawInputFrame = ImGui::GetFrameCount() + 10;
            }
            else if (ImGui::GetFrameCount() > s_RecoverRawInputFrame)
            {
                HID_RegisterDevice(g_LastRegisteredRawInputWindow, g_HidKeyboard);
                g_LastRegisteredRawInputWindow = nullptr;
            }
        }
    }

    if (g_PostDrawCallback)
    {
        g_PostDrawCallback();
    }

    return g_OrigSwapChainPresent(pSwapChain, SyncInterval, Flags);
}

HRESULT STDMETHODCALLTYPE
hkResizeBuffers(IDXGISwapChain *pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
    cleanup_render_target();
    const HRESULT result = g_OrigSwapChainResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
    create_render_target();
    return result;
}

template <class FunT, typename T> FunT &vtable_find(T *obj, int index)
{
    void ***ptr = reinterpret_cast<void ***>(obj);
    if (!ptr[0])
        return *static_cast<FunT *>(nullptr);
    return *reinterpret_cast<FunT *>(&ptr[0][index]);
}

template <class FunT> void hook_virtual_function(FunT hook_fun, FunT &orig_fun, int vtable_index)
{
    FunT &vtable_ptr = vtable_find<FunT>(g_SwapChain, vtable_index);

    DWORD oldProtect;
    if (!VirtualProtect(reinterpret_cast<LPVOID>(reinterpret_cast<uintptr_t>(&vtable_ptr) & ~0xFFF), 0x1000, PAGE_READWRITE, &oldProtect))
    {
        PANIC("VirtualProtect error: {:#x}\n", GetLastError());
    }

    if (!vtable_ptr)
    {
        PANIC("DirectX 11 is not initialized yet.");
    }

    orig_fun = vtable_ptr;
    vtable_ptr = hook_fun;
};

bool init_hooks(void *swap_chain_ptr)
{
    g_SwapChain = reinterpret_cast<IDXGISwapChain *>(swap_chain_ptr);

    // https://github.com/Rebzzel/kiero/blob/master/METHODSTABLE.txt#L249
    hook_virtual_function(&hkPresent, g_OrigSwapChainPresent, 8);
    // https://github.com/Rebzzel/kiero/blob/master/METHODSTABLE.txt#L254
    hook_virtual_function(&hkResizeBuffers, g_OrigSwapChainResizeBuffers, 13);

    return true;
}

void register_on_input(OnInputCallback on_input)
{
    g_OnInputCallback = on_input;
}
void register_imgui_init(ImguiInitCallback imgui_init)
{
    g_ImguiInitCallback = imgui_init;
}
void register_imgui_draw(ImguiDrawCallback imgui_draw)
{
    g_ImguiDrawCallback = imgui_draw;
}
void register_pre_draw(PreDrawCallback pre_draw)
{
    g_PreDrawCallback = pre_draw;
}
void register_post_draw(PostDrawCallback post_draw)
{
    g_PostDrawCallback = post_draw;
}

HWND get_window()
{
    return g_Window;
}
