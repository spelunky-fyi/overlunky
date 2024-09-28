#include "window_api.hpp"

#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>
#include <d3d11.h>
#include <detours.h>
#include <imgui.h>
#include <imgui_internal.h>

#include <atomic>
#include <chrono>

#include "bucket.hpp"
#include "logger.h"
#include "memory.hpp"
#include "script/lua_backend.hpp"
#include "state.hpp"

bool detect_wine()
{
    static const HMODULE hntdll = GetModuleHandle("ntdll.dll");
    if (!hntdll)
        return false;
    static const void* wgv = GetProcAddress(hntdll, "wine_get_version");
    if (!wgv)
        return false;
    return true;
}

UINT g_SyncInterval{1};
IDXGISwapChain* g_SwapChain{nullptr};
ID3D11Device* g_Device{nullptr};
ID3D11DeviceContext* g_Context{nullptr};
ID3D11RenderTargetView* g_MainRenderTargetView{nullptr};
HWND g_Window{nullptr};

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
WNDPROC g_OrigWndProc{nullptr};

using PresentPtr = HRESULT(STDMETHODCALLTYPE*)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
PresentPtr g_OrigSwapChainPresent{nullptr};

using ResizeBuffersPtr = HRESULT(STDMETHODCALLTYPE*)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);
ResizeBuffersPtr g_OrigSwapChainResizeBuffers{nullptr};

OnInputCallback g_OnInputCallback{nullptr};
ImguiInitCallback g_ImguiPreInitCallback{nullptr};
ImguiInitCallback g_ImguiInitCallback{nullptr};
ImguiDrawCallback g_ImguiDrawCallback{nullptr};
PreDrawCallback g_PreDrawCallback{nullptr};
PostDrawCallback g_PostDrawCallback{nullptr};
OnQuitCallback g_OnQuitCallback{nullptr};

constexpr USHORT g_HidKeyboard = 6;
HWND g_LastRegisteredRawInputWindow{nullptr};

auto g_MouseLastActivity = std::chrono::system_clock::now();
ImVec2 g_CursorLastPos = ImVec2(0, 0);
std::atomic<std::int32_t> g_ShowCursor{0};

using DestroyGameManager = void(void*);
DestroyGameManager* g_destroy_game_manager_trampoline{nullptr};
void destroy_game_manager(void* game_manager)
{
    if (g_OnQuitCallback)
    {
        g_OnQuitCallback();
    }
    g_destroy_game_manager_trampoline(game_manager);
}

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
    static const auto bucket = Bucket::get();

    if (message == WM_HOTKEY)
        LuaBackend::wm_hotkey((int)wParam);
    else if (message == WM_ACTIVATE)
        LuaBackend::wm_activate((bool)wParam);

    bucket->pause_api->modifiers_down = 0;
    if (ImGui::GetIO().KeyCtrl)
        bucket->pause_api->modifiers_down |= 0x100;
    if (ImGui::GetIO().KeyShift)
        bucket->pause_api->modifiers_down |= 0x200;
    if (ImGui::GetIO().KeyAlt)
        bucket->pause_api->modifiers_down |= 0x800;

    bool consumed_input = g_OnInputCallback ? g_OnInputCallback(message, wParam, lParam) : false;

    /*if (bucket->io->WantCaptureKeyboard.value_or(false) && (message == WM_KEYDOWN || message == WM_KEYUP))
        consumed_input = true;*/

    if (get_forward_events() && bucket->io->WantCaptureMouse.value_or(false) && message >= WM_LBUTTONDOWN && message <= WM_MOUSEWHEEL)
        consumed_input = true;

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
    if (g_ImguiPreInitCallback)
    {
        g_ImguiPreInitCallback(imgui_context);
    }

    ImGui_ImplWin32_Init(g_Window);
    ImGui_ImplDX11_Init(g_Device, g_Context);

    if (g_ImguiInitCallback)
    {
        g_ImguiInitCallback(imgui_context);
    }
}

void imgui_mouse_activity()
{
    using namespace std::chrono_literals;

    auto& io = ImGui::GetIO();
    auto now = std::chrono::system_clock::now();

    if (io.MousePos.x != g_CursorLastPos.x || io.MousePos.y != g_CursorLastPos.y)
    {
        g_MouseLastActivity = now;
        g_CursorLastPos = io.MousePos;
        if (g_ShowCursor.load() > 0)
        {
            io.MouseDrawCursor = true;
        }
    }
    else if (g_MouseLastActivity + 2s < now)
    {
        io.MouseDrawCursor = false;
    }

    int num_mouse_buttons_down = 0;
    for (int i = 0; i < ImGuiMouseButton_COUNT; i++)
    {
        if (ImGui::IsMouseDown(i))
        {
            num_mouse_buttons_down++;
        }
    }
    if (num_mouse_buttons_down != 0 && g_MouseLastActivity + 2s < now && g_ShowCursor.load() > 0)
    {
        g_MouseLastActivity = now;
        if (g_ShowCursor.load() > 0)
        {
            io.MouseDrawCursor = true;
        }
    }
}

void create_render_target()
{
    ID3D11Texture2D* pBackBuffer;
    g_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
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

// This is for Wine, it doesn't seem to use rawinput or the wndproc
HHOOK g_kbHook{NULL};
LRESULT CALLBACK hkKeyboard(const int code, const WPARAM wParam, const LPARAM lParam)
{
    if (wParam == WM_KEYDOWN && ImGui::GetIO().WantCaptureKeyboard)
        return 0;
    return CallNextHookEx(g_kbHook, code, wParam, lParam);
}

static bool skip_hkPresent = false;
HRESULT STDMETHODCALLTYPE hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
    ImGuiContext& g = *GImGui;
    static const auto bucket = Bucket::get();
    SyncInterval = g_SyncInterval;

    if (skip_hkPresent)
        return g_OrigSwapChainPresent(pSwapChain, SyncInterval, Flags);

    static bool init = false;
    if (!init)
    {
        if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&g_Device)))
        {
            g_Device->GetImmediateContext(&g_Context);
            DXGI_SWAP_CHAIN_DESC sd;
            pSwapChain->GetDesc(&sd);
            g_Window = sd.OutputWindow;
            g_OrigWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(g_Window, GWLP_WNDPROC, LONG_PTR(hkWndProc)));
            g_kbHook = SetWindowsHookEx(WH_KEYBOARD_LL, hkKeyboard, NULL, 0);
            create_render_target();
            init_imgui();
            init = true;
        }
        else
        {
            return g_OrigSwapChainPresent(pSwapChain, SyncInterval, Flags);
        }
    }

    if (bucket->count > 1)
    {
        if (!get_forward_events())
        {
            bucket->io->WantCaptureMouse = ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) && g.HoveredWindow && strcmp(g.HoveredWindow->Name, "Clickhandler");
            bucket->io->WantCaptureKeyboard = ImGui::GetIO().WantCaptureKeyboard;
        }
        else
        {
            if (bucket->io->WantCaptureKeyboard.has_value())
                ImGui::GetIO().WantCaptureKeyboard = bucket->io->WantCaptureKeyboard.value();
            if (bucket->io->WantCaptureMouse.has_value())
                ImGui::GetIO().WantCaptureMouse = bucket->io->WantCaptureMouse.value();
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

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        skip_hkPresent = true;
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        skip_hkPresent = false;
    }

    {
        if (ImGui::GetIO().WantCaptureKeyboard || bucket->io->WantCaptureKeyboard.value_or(false))
        {
            if (HWND window = HID_GetRegisteredDeviceWindow(g_HidKeyboard))
            {
                g_LastRegisteredRawInputWindow = window;
                HID_UnregisterDevice(g_HidKeyboard);
            }
        }
        else if (g_LastRegisteredRawInputWindow != nullptr)
        {
            static std::int32_t s_RecoverRawInputFrame{0};
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

    imgui_mouse_activity();
    if (g_PostDrawCallback)
    {
        g_PostDrawCallback();
    }

    if (get_forward_events() || bucket->count == 1)
    {
        bucket->io->WantCaptureKeyboard = std::nullopt;
        bucket->io->WantCaptureMouse = std::nullopt;
    }

    return g_OrigSwapChainPresent(pSwapChain, SyncInterval, Flags);
}

HRESULT STDMETHODCALLTYPE
hkResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
    cleanup_render_target();
    const HRESULT result = g_OrigSwapChainResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
    create_render_target();
    return result;
}

template <class FunT>
void hook_virtual_function(FunT hook_fun, FunT& orig_fun, int vtable_index)
{
    if (FunT* vtable_ptr = vtable_find<FunT>(g_SwapChain, vtable_index))
    {
        DWORD oldProtect;
        if (!VirtualProtect(reinterpret_cast<LPVOID>(reinterpret_cast<uintptr_t>(vtable_ptr) & ~0xFFF), 0x1000, PAGE_READWRITE, &oldProtect))
        {
            PANIC("VirtualProtect error: {:#x}\n", GetLastError());
        }

        if (*vtable_ptr == nullptr)
        {
            PANIC("DirectX 11 is not initialized yet.");
        }

        orig_fun = *vtable_ptr;
        *vtable_ptr = hook_fun;
    }
};

void hook_steam_overlay()
{
    char* steam_overlay = (char*)GetModuleHandleA("gameoverlayrenderer64.dll");
    // TODO: Yeah I could've probably figured these out from the vtable or made patterns
    // but I don't think they change a lot anyway
    // Update 2 days later: They immediately changed for the first time in years
    // const size_t present_offset = 0x88E30;
    // const size_t resize_offset = 0x890F0;
    if (steam_overlay == nullptr || detect_wine())
    {
        // Steam overlay is not loaded, so we're probably running on steam emu
        // Just do the old vtable hook and call it a day
        DEBUG("No Steam Overlay detected, hooking D3D...");

        // https://github.com/Rebzzel/kiero/blob/master/METHODSTABLE.txt#L249
        hook_virtual_function(&hkPresent, g_OrigSwapChainPresent, 8);
        // https://github.com/Rebzzel/kiero/blob/master/METHODSTABLE.txt#L254
        hook_virtual_function(&hkResizeBuffers, g_OrigSwapChainResizeBuffers, 13);
        return;
    }

    // Steam overlay is loaded and it would crash if we hook the vtable
    // Lets detour the steam overlay instead!
    DEBUG("Steam detected, hooking Steam Overlay...");

    size_t present_offset = find_inst(steam_overlay, "\x48\x8b\x4f\x40\x48\x8d\x15"sv, 0, 0x99999, "steam_overlay_present"sv, false);
    size_t resize_offset = find_inst(steam_overlay, "\x48\x8b\x4f\x68\x48\x8d\x15"sv, 0, 0x99999, "steam_overlay_resize"sv, false);

    if (present_offset == 0 || resize_offset == 0)
    {
        DEBUG("Couldn't find steam overlay functions... Hooking D3D directly, but it will probably crash.");

        // https://github.com/Rebzzel/kiero/blob/master/METHODSTABLE.txt#L249
        hook_virtual_function(&hkPresent, g_OrigSwapChainPresent, 8);
        // https://github.com/Rebzzel/kiero/blob/master/METHODSTABLE.txt#L254
        hook_virtual_function(&hkResizeBuffers, g_OrigSwapChainResizeBuffers, 13);
        return;
    }

    auto present_offset2 = *(int*)(steam_overlay + present_offset + 7);
    auto resize_offset2 = *(int*)(steam_overlay + resize_offset + 7);
    present_offset = present_offset + 7 + 4 + present_offset2;
    resize_offset = resize_offset + 7 + 4 + resize_offset2;

    g_OrigSwapChainPresent = (PresentPtr)(steam_overlay + present_offset);
    g_OrigSwapChainResizeBuffers = (ResizeBuffersPtr)(steam_overlay + resize_offset);

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    DetourAttach((void**)&g_OrigSwapChainPresent, hkPresent);
    DetourAttach((void**)&g_OrigSwapChainResizeBuffers, hkResizeBuffers);

    const LONG error = DetourTransactionCommit();
    if (error != NO_ERROR)
    {
        PANIC("Failed hooking Steam Overlay: {}\n", error);
    }
}

bool init_hooks(void* swap_chain_ptr)
{
    g_SwapChain = reinterpret_cast<IDXGISwapChain*>(swap_chain_ptr);
    if (SUCCEEDED(g_SwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&g_Device)))
    {
        g_Device->GetImmediateContext(&g_Context);
    }

    // void* present_ptr = (void*)vtable_find<void*>(g_SwapChain, 8);
    // size_t present_fun = *(size_t*)present_ptr;
    // DEBUG("present vtable {} {}", present_ptr, (void*)present_fun);

    // void* resize_ptr = (void*)vtable_find<void*>(g_SwapChain, 13);
    // size_t resize_fun = *(size_t*)resize_ptr;
    // DEBUG("resize vtable {} {}", resize_ptr, (void*)resize_fun);

    hook_steam_overlay();
    // hook_virtual_function(&hkPresent, g_OrigSwapChainPresent, 8);
    // hook_virtual_function(&hkResizeBuffers, g_OrigSwapChainResizeBuffers, 13);

    {
        g_destroy_game_manager_trampoline = (DestroyGameManager*)get_address("destroy_game_manager"sv);

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());

        DetourAttach((void**)&g_destroy_game_manager_trampoline, destroy_game_manager);

        const LONG error = DetourTransactionCommit();
        if (error != NO_ERROR)
        {
            DEBUG("Failed hooking DestroyGameManager: {}\n", error);
        }
    }

    return true;
}

void register_on_input(OnInputCallback on_input)
{
    g_OnInputCallback = on_input;
}
void register_imgui_pre_init(ImguiInitCallback imgui_init)
{
    g_ImguiPreInitCallback = imgui_init;
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
void register_on_quit(OnQuitCallback on_quit)
{
    g_OnQuitCallback = on_quit;
}

HWND get_window()
{
    return g_Window;
}

void show_cursor()
{
    if (g_ShowCursor.fetch_add(1) == 0)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.MouseDrawCursor = true;
    }
}
void hide_cursor()
{
    if (g_ShowCursor.fetch_sub(1) == 1)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.MouseDrawCursor = false;
    }
}

void imgui_vsync(bool enable)
{
    g_SyncInterval = (UINT)enable;
}

ID3D11Device* get_device()
{
    return g_Device;
}
