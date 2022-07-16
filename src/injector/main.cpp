#include <Windows.h>   // for PROCESS_INFORMATION, CloseHandle, GetEnvironm...
#include <chrono>      // for operator<=>, operator-, operator+, operator""s
#include <compare>     // for operator<, operator<=, operator>
#include <cstdint>     // for uint64_t
#include <filesystem>  // for exists, path
#include <fstream>     // for filesystem
#include <new>         // for operator new
#include <optional>    // for optional
#include <stdio.h>     // for NULL, sprintf_s
#include <string.h>    // for strlen
#include <string>      // for string, basic_string
#include <string_view> // for string_view
#include <thread>      // for sleep_for
#include <utility>     // for max, min

#include "cmd_line.h"  // for GetCmdLineParam, CmdLineParser
#include "injector.h"  // for Process, ProcessInfo, call, find_function
#include "logger.h"    // for INFO, PANIC
#include "version.hpp" // for get_version

namespace fs = std::filesystem;
using namespace std::chrono_literals;

fs::path get_dll_path(const char* rel_path)
{
    char buf[0x1000];
    GetModuleFileNameA(NULL, buf, sizeof(buf));
    fs::path path(buf);
    return path.parent_path().concat(rel_path);
}

int main(int argc, char** argv)
{
    CmdLineParser cmd_line_parser(argc, argv);

    bool info_dump = GetCmdLineParam<bool>(cmd_line_parser, "info_dump", false);
    auto overlunky_path = get_dll_path(info_dump ? "\\info_dump.dll" : "\\injected.dll");

    if (!fs::exists(overlunky_path))
    {
        PANIC("DLL not found! {}", overlunky_path.string().data());
    }

    INFO("Overlunky version: {}", get_version());

    Process game_proc = [&cmd_line_parser]()
    {
        std::string_view spel2_exe_dir = GetCmdLineParam<std::string_view>(cmd_line_parser, "launch_game", "");
        std::string_view playlunky_exe_dir = GetCmdLineParam<std::string_view>(cmd_line_parser, "launch_playlunky", "");
        if (spel2_exe_dir.empty() || !fs::exists(spel2_exe_dir))
        {
            if (!playlunky_exe_dir.empty() && fs::exists(playlunky_exe_dir))
            {
                INFO("Launching playlunky...");

                const std::string spel2_dir{playlunky_exe_dir};

                char spel2_cmd_line[MAX_PATH];
                sprintf_s(spel2_cmd_line, "%s/playlunky_launcher.exe", spel2_dir.c_str());

                STARTUPINFO si{};
                si.cb = sizeof(si);

                const auto child_env = []()
                {
                    std::string childenv = "SteamAppId=418530";

                    const auto this_env = GetEnvironmentStrings();
                    auto lpszVariable = this_env;
                    while (*lpszVariable)
                    {
                        childenv += '\0';
                        childenv += lpszVariable;
                        lpszVariable += strlen(lpszVariable) + 1;
                    }
                    FreeEnvironmentStrings(this_env);

                    childenv += '\0';
                    return childenv;
                }();

                PROCESS_INFORMATION pi{};

                if (CreateProcess(spel2_cmd_line, NULL, NULL, NULL, TRUE, 0, (LPVOID)child_env.c_str(), spel2_dir.c_str(), &si, &pi))
                {
                    WaitForInputIdle(pi.hProcess, 1000);

                    INFO("Waiting a few seconds before injecting...");
                    std::this_thread::sleep_for(5s);

                    CloseHandle(pi.hThread);
                }
            }

            INFO("Searching for Spel2.exe process... Pro tip: You have to launch it yourself.");
            bool started = true;
            Process proc;
            while (true)
            {
                if (auto res = find_process("Spel2.exe"))
                {
                    proc = res.value();
                    break;
                }
                started = false;
                std::this_thread::sleep_for(1s);
            };
            INFO("Found Spel2.exe PID: {}", proc.info.pid);
            if (!started)
            {
                INFO("Game was just started, waiting a few seconds for it to "
                     "load before injecting...");
                std::this_thread::sleep_for(1s);
            }
            return proc;
        }
        else
        {
            INFO("Game path was passed on cmd line, launching game directly...");

            const std::string spel2_dir{spel2_exe_dir};

            char spel2_cmd_line[MAX_PATH];
            sprintf_s(spel2_cmd_line, "%s/Spel2.exe", spel2_dir.c_str());

            STARTUPINFO si{};
            si.cb = sizeof(si);

            const auto child_env = []()
            {
                std::string childenv = "SteamAppId=418530";

                const auto this_env = GetEnvironmentStrings();
                auto lpszVariable = this_env;
                while (*lpszVariable)
                {
                    childenv += '\0';
                    childenv += lpszVariable;
                    lpszVariable += strlen(lpszVariable) + 1;
                }
                FreeEnvironmentStrings(this_env);

                childenv += '\0';
                return childenv;
            }();

            PROCESS_INFORMATION pi{};

            if (CreateProcess(spel2_cmd_line, NULL, NULL, NULL, TRUE, 0, (LPVOID)child_env.c_str(), spel2_dir.c_str(), &si, &pi))
            {
                WaitForInputIdle(pi.hProcess, 1000);

                INFO("Waiting a few seconds before injecting...");
                std::this_thread::sleep_for(5s);

                Process proc{pi.hProcess, ProcessInfo{"Spel2", pi.dwProcessId}};

                CloseHandle(pi.hThread);

                return proc;
            }
        }

        return Process{};
    }();

    inject_dll(game_proc, overlunky_path.string());
    call(game_proc, find_function(game_proc, overlunky_path.string(), "run"), (LPVOID)(uint64_t)GetCurrentProcessId());
}
