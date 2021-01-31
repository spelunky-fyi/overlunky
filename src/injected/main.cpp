#include <Windows.h>

#include <chrono>
#include <iostream>
#include <thread>

#include "logger.h"
#include "memory.h"
#include "render_api.hpp"
#include "state.hpp"
#include "ui.hpp"

using namespace std::chrono_literals;

BOOL WINAPI DllMain(HINSTANCE hinstDLL,  // handle to DLL module
                    DWORD fdwReason,     // reason for calling function
                    LPVOID lpReserved)   // reserved
{
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}

BOOL WINAPI ctrl_handler(DWORD ctrl_type) {
    switch (ctrl_type) {
        case CTRL_C_EVENT:
        case CTRL_BREAK_EVENT:
        case CTRL_CLOSE_EVENT: {
            DEBUG("Console detached, you can now close this window.");
            FreeConsole();
            return TRUE;
        }
    }
    return TRUE;
}

void attach_stdout(DWORD pid) {
    if (false) {
        AttachConsole(pid);
        SetConsoleCtrlHandler(ctrl_handler, 1);
    }
}

extern "C" __declspec(dllexport) void run(DWORD pid) {
    attach_stdout(pid);
    FILE *fp = fopen("spelunky.log", "w");
    if (!fp) {
        PANIC("%d", errno);
    }
    {
        fputs("Overlunky loaded\n", fp);
        fclose(fp);
    }
    DEBUG(
        "Game injected! Press Ctrl+C to detach this window from the process.");
    auto state = State::get();
    while (true) {
        auto entities = list_entities();
        if (entities.size() >= 850) {
            DEBUG("Found {:?} entities, that's enough", entities.size());
            std::this_thread::sleep_for(100ms);
            create_box(entities);
            DEBUG("Added {:?} entities", entities.size());
            break;
        } else if (entities.size() > 0) {
            DEBUG("Found {:?} entities", entities.size());
        }
        std::this_thread::sleep_for(100ms);
    }

    auto api = RenderAPI::get();
    init_hooks(api.swap_chain());
    if (false) {
        {
            DEBUG("Enter entity #IDs to spawn, one per line >");
            std::string line;
            std::getline(std::cin, line);
            if (int id = atoi(line.data())) {
                // This is RAII-style implementation for suspending the main
                // thread, for preventing race conditions.
                CriticalSection lock;

                auto player = state.items()->player(0);
                if (!player) {
                    INFO(
                        "Player not initialized yet. Select a character "
                        "first!");
                } else {
                    auto [x, y] = player->position();
                    auto layer = player->layer();
                    state.layer(layer)->spawn_entity(id, x, y, false, 0.0, 0.0,
                                                     false);
                }
            }
        }
    }
}
