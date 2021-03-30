#include <Windows.h>

#include <chrono>
#include <iostream>
#include <thread>

#include "logger.h"
#include "memory.h"
#include "render_api.hpp"
#include "state.hpp"
#include "ui.hpp"
#include "window_api.hpp"

using namespace std::chrono_literals;

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL, // handle to DLL module
    DWORD fdwReason,    // reason for calling function
    LPVOID lpReserved)  // reserved
{
    return TRUE; // Successful DLL_PROCESS_ATTACH.
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
    if (std::getenv("OL_DEBUG") || 1)
    {
        AttachConsole(pid);
        SetConsoleCtrlHandler(ctrl_handler, 1);

        freopen("1.json", "w", stdout);
        freopen("CONOUT$", "w", stderr);
        freopen("CONIN$", "r", stdin);
    }
}

using namespace nlohmann;

void dump()
{
    json entities(json::array());
    auto items = list_entities();
    std::sort(items.begin(), items.end(), [](EntityItem &a, EntityItem &b) -> bool { return a.id < b.id; });
    for (auto &ent : items)
    {
        EntityDB *db = get_type(ent.id);
        if (!db)
            break;
        json j(*db);
        j["name"] = ent.name;
        entities.push_back(j);
    }
    DEBUG("{}", entities.dump(2));
}

extern "C" __declspec(dllexport) void run(DWORD pid)
{
    attach_stdout(pid);
    FILE *fp = fopen("spelunky.log", "a");
    if (fp)
    {
        fputs("Overlunky loaded\n", fp);
        fclose(fp);
    }
    DEBUG("Game injected! Press Ctrl+C to detach this window from the process.");

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

    dump();
    return;
    auto api = RenderAPI::get();
    init_ui();
    init_hooks((void *)api.swap_chain());
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
