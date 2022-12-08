#include <Windows.h>    // for AttachConsole, DWORD, FreeConsole, SetCons...
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
    size_t env_var_size;
    getenv_s(&env_var_size, NULL, 0, "OL_DEBUG");
    if (env_var_size > 0)
    {
        AttachConsole(pid);
        SetConsoleCtrlHandler(ctrl_handler, 1);

        FILE* stream;
        freopen_s(&stream, "CONOUT$", "w", stdout);
        freopen_s(&stream, "CONOUT$", "w", stderr);
        freopen_s(&stream, "CONIN$", "r", stdin);
    }
}

extern "C" __declspec(dllexport) void run(DWORD pid = 0)
{
    if (pid)
        attach_stdout(pid);
    DEBUG("Game injected! Press Ctrl+C to detach this window from the process.");

    register_application_version(fmt::format("Overlunky {}", get_version()));
    preload_addresses();

    while (true)
    {
        auto entities = list_entities();
        if (entities.size() >= 850)
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
    init_ui();
    init_hooks((void*)api.swap_chain());
    size_t env_var_size;
    getenv_s(&env_var_size, NULL, 0, "OL_DEBUG");
    if (env_var_size > 0)
    {
        DEBUG("Running in debug mode.");
        do
        {
            std::string line;
            std::getline(std::cin, line);
            if (std::cin.fail() || std::cin.eof())
            {
                std::cin.clear();
            }
        } while (true);
    }
}

BOOL WINAPI DllMain([[maybe_unused]] HINSTANCE hinst, DWORD dwReason, [[maybe_unused]] LPVOID reserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hinst);
        std::thread thr(run, 0);
        thr.detach();
    }
    return TRUE;
}
