#include <chrono>
#include <filesystem>
#include <thread>

#include "cmd_line.h"
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

int main(int argc, char** argv) {
    CmdLineParser cmd_line_parser(argc, argv);

    auto overlunky_path = get_dll_path();

    if (!fs::exists(overlunky_path)) {
        PANIC("DLL not found! {}", overlunky_path.string().data());
    }

    INFO("Overlunky version: " TOSTRING(GIT_VERSION));

    Process game_proc = [&cmd_line_parser]() {
        std::string_view spel2_exe_path = GetCmdLineParam<std::string_view>(
            cmd_line_parser, "launch_game", "");
        if (spel2_exe_path.empty() || !fs::exists(spel2_exe_path)) {
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
            INFO("Found Spel2.exe PID: {}", proc.info.pid);
            if (!started) {
                INFO(
                    "Game was just started, waiting a few seconds for it to "
                    "load before injecting...");
                std::this_thread::sleep_for(1s);
            }
            return proc;
        } else {
            INFO(
                "Game path was passed on cmd line, launching game directly...");

            char spel2_cmd_line[MAX_PATH];
            sprintf_s(spel2_cmd_line, "%.*s",
                      static_cast<int>(spel2_exe_path.size()),
                      spel2_exe_path.data());

            const std::string spel2_dir =
                fs::path(spel2_exe_path).parent_path().string();

            STARTUPINFO si{};
            si.cb = sizeof(si);

            PROCESS_INFORMATION pi{};

            if (CreateProcess(spel2_cmd_line, NULL, NULL, NULL, TRUE, 0, NULL,
                              spel2_dir.c_str(), &si, &pi)) {
                WaitForInputIdle(pi.hProcess, 1000);

                INFO("Waiting a few seconds before injecting...");
                std::this_thread::sleep_for(5s);

                Process proc{pi.hProcess, ProcessInfo{"Spel2", pi.dwProcessId}};

                CloseHandle(pi.hThread);

                return proc;
            }
        }
    }();

    inject_dll(game_proc, overlunky_path.string());
    call(game_proc, find_function(game_proc, overlunky_path.string(), "run"),
         (LPVOID)(uint64_t)GetCurrentProcessId());
}
