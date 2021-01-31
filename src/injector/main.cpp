#include <chrono>
#include <filesystem>
#include <thread>

#include "injector.h"
#include "logger.h"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

namespace fs = std::filesystem;
using namespace std::chrono_literals;

fs::path get_dll_path() {
    char buf[0x1000];
    GetModuleFileNameA(NULL, buf, sizeof(buf));
    fs::path path(buf);
    return path.parent_path().concat("\\injected.dll");
}

int main() {
    auto path = get_dll_path();

    if (!fs::exists(path)) {
        PANIC("DLL not found! %s", path.string().data());
    }

    INFO("Overlunky version: " TOSTRING(GIT_VERSION));

    {
        INFO("Searching for Spel2.exe process...");
        bool started = true;
        Process proc;
        while (true) {
            if (auto res = find_process("Spel2.exe")) {
                proc = res.value();
                break;
            }
            started = false;
            std::this_thread::sleep_for(1s);
        };
        INFO("Found Spel2.exe PID: %d", proc.info.pid);
        if (!started) {
            INFO(
                "Game was just started, waiting a few seconds for it to load "
                "before injecting...");
            std::this_thread::sleep_for(1s);
        }
        inject_dll(proc, path.string());
        call(proc, find_function(proc, path.string(), "run"),
             (LPVOID)(uint64_t)GetCurrentProcessId());
    }
}
