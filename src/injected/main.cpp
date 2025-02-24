#include <Windows.h> // for AttachConsole, DWORD, FreeConsole, SetCons...

#include <TlHelp32.h>   // for PROCESSENTRY32, CreateToolhelp32Snapshot, Pro...
#include <chrono>       // for operator<=>, operator-, operator+, operato...
#include <compare>      // for operator<, operator<=, operator>
#include <cstdio>       // for freopen_s, fclose, fopen_s, fputs, FILE, NULL
#include <cstdlib>      // for getenv_s
#include <fmt/format.h> // for check_format_string, format, vformat
#include <iostream>     // for basic_istream, istream, cin, basic_streambuf
#include <locale>       // for num_get, num_put
#include <new>          // for operator new
#include <string>       // for allocator, getline, string
#include <thread>       // for sleep_for
#include <type_traits>  // for move
#include <utility>      // for max, min
#include <vector>       // for vector

#include "entity.hpp"     // for EntityItem, list_entities
#include "logger.h"       // for DEBUG
#include "render_api.hpp" // for RenderAPI
#include "search.hpp"     // for preload_addresses, register_application_ve...
#include "ui.hpp"         // for create_box, init_ui
#include "version.hpp"    // for get_version
#include "window_api.hpp" // for init_hooks

using namespace std::chrono_literals;

struct ProcessInfo
{
    std::string name;
    DWORD pid;
};

struct Process
{
    HANDLE handle;
    ProcessInfo info;
};

std::vector<ProcessInfo> get_processes()
{
    // No unicode
#undef Process32First
#undef Process32Next
#undef PROCESSENTRY32
    std::vector<ProcessInfo> res;
    auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == nullptr)
        return {};

    PROCESSENTRY32 ppe = {sizeof(ppe)};
    auto proc = Process32First(snapshot, &ppe);

    while (proc)
    {
        auto name = ppe.szExeFile;
        if (auto delim = strrchr(name, '\\'))
            name = delim;
        res.push_back({name, ppe.th32ProcessID});
        proc = Process32Next(snapshot, &ppe);
    }
    return res;
}

std::optional<Process> find_process(std::string name)
{
    for (auto& proc : get_processes())
    {
        if (proc.name == name)
        {
            return Process{OpenProcess(PROCESS_ALL_ACCESS, 0, proc.pid), proc};
        }
    }
    return {};
}

BOOL WINAPI ctrl_handler(DWORD ctrl_type)
{
    switch (ctrl_type)
    {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
    case CTRL_CLOSE_EVENT:
    {
        DEBUG("Console detached, you can now close this window.");
        FreeConsole();
        return TRUE;
    }
    }
    return TRUE;
}

void attach_stdout(DWORD pid)
{
    AttachConsole(pid);
    SetConsoleCtrlHandler(ctrl_handler, 1);

    FILE* stream;
    freopen_s(&stream, "CONOUT$", "w", stdout);
    freopen_s(&stream, "CONOUT$", "w", stderr);
    // freopen_s(&stream, "CONIN$", "r", stdin);
    INFO("Do not close this window or the game will also die. Press Ctrl+C to detach this window from the game process.");
}

void run()
{
    std::this_thread::sleep_for(2s);
    Process proc;
    if (auto res = find_process("Overlunky.exe"))
    {
        proc = res.value();
        attach_stdout(proc.info.pid);
    }

    register_application_version(fmt::format("Overlunky {}", get_version()));
    preload_addresses();

    while (true)
    {
        auto entities = list_entities();
        if (entities.size() >= 876)
        {
            DEBUG("Found {} entities, that's enough", entities.size());
            std::this_thread::sleep_for(100ms);
            create_box(entities);
            DEBUG("Added {} entities", entities.size());
            break;
        }
        else if (entities.size() > 0)
        {
            DEBUG("Found {} entities", entities.size());
        }
        std::this_thread::sleep_for(100ms);
    }

    auto& api = RenderAPI::get();
    register_imgui_pre_init(&init_ui);
    init_hooks((void*)api.swap_chain());
}

extern "C" __declspec(dllexport) const char* dll_version()
{
    return get_version_cstr();
}

BOOL WINAPI DllMain([[maybe_unused]] HINSTANCE hinst, DWORD dwReason, [[maybe_unused]] LPVOID reserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hinst);
        std::thread thr(run);
        thr.detach();
    }
    return TRUE;
}
