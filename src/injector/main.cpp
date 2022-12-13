#include <Windows.h> // for PROCESS_INFORMATION, CloseHandle, GetEnvironm...
#include <chrono>    // for operator<=>, operator-, operator+, operator""s
#include <compare>   // for operator<, operator<=, operator>
#include <conio.h>
#include <ctime> // for errno_t
#include <detours.h>
#include <filesystem> // for exists, path
#include <fstream>    // for basic_ostream, basic_ofstream, ofstream, basi...
#include <iostream>
#include <locale>   // for num_put, num_get
#include <new>      // for operator new
#include <optional> // for optional
#include <sstream>  // for basic_stringstream
#include <stdint.h> // for uint64_t
#include <stdio.h>  // for NULL, fclose, sprintf_s, fflush, fopen_s, fwrite
#include <stdlib.h>
#include <string.h>    // for strlen
#include <string>      // for char_traits, string, basic_string, operator==
#include <string_view> // for string_view
#include <strsafe.h>   // for DWORD
#include <thread>      // for sleep_for
#include <type_traits> // for move
#include <utility>     // for max, min
#include <wininet.h>   // for InternetCloseHandle, InternetOpenA, InternetG...

#include "cmd_line.h"  // for GetCmdLineParam, CmdLineParser
#include "injector.h"  // for Process, ProcessInfo, call, find_function
#include "logger.h"    // for INFO, PANIC
#include "version.hpp" // for get_version

#pragma comment(lib, "wininet.lib")
#pragma warning(disable : 4706 4996)

namespace fs = std::filesystem;
using namespace std::chrono_literals;
bool g_console = false;
std::string g_exe = "Spel2.exe";

fs::path get_dll_path(const char* rel_path)
{
    char buf[0x1000];
    GetModuleFileNameA(NULL, buf, sizeof(buf));
    fs::path path(buf);
    return path.parent_path().concat(rel_path);
}

bool auto_update(const char* sURL, const char* sSaveFilename, bool& yes)
{
    const char* sHeader = NULL;
    const int BUFFER_SIZE = 32768;
    DWORD iFlags;
    const char* sAgent = "Overlunky Updater";
    HINTERNET hInternet;
    HINTERNET hConnect;
    char acBuffer[BUFFER_SIZE];
    DWORD iReadBytes;
    FILE* pFile = NULL;
    DWORD iBytesToRead = 0;
    DWORD iReadBytesOfRq = 4;
    DWORD response_headers_size = 4096;
    char response_headers[4096];
    std::string old_version = "";
    std::string new_version = "";
    int answer;

    // Get connection state
    InternetGetConnectedState(&iFlags, 0);
    if (iFlags & INTERNET_CONNECTION_OFFLINE)
    {
        INFO("AutoUpdate: Can't connect to the internet");
        return false;
    }

    // Open internet session
    if (!(iFlags & INTERNET_CONNECTION_PROXY))
    {
        hInternet = InternetOpenA(sAgent, INTERNET_OPEN_TYPE_PRECONFIG_WITH_NO_AUTOPROXY, NULL, NULL, 0);
    }
    else
    {
        hInternet = InternetOpenA(sAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    }
    if (hInternet)
    {
        if (sHeader == NULL)
        {
            sHeader = "Accept: */*\r\n\r\n";
        }

        // Get URL
        hConnect = InternetOpenUrlA(hInternet, sURL, sHeader, lstrlenA(sHeader), INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_RELOAD, 0);
        if (!hConnect)
        {
            InternetCloseHandle(hInternet);
            INFO("AutoUpdate: Can't get release information from github");
            return false;
        }

        // Get current version
        std::ifstream viStream("Overlunky.version");
        if (!viStream.fail())
        {
            std::stringstream buffer;
            buffer << viStream.rdbuf();
            old_version = buffer.str();
            if (old_version.find("disabled") != std::string::npos)
            {
                INFO("AutoUpdate: Disabled, delete Overlunky.version or run with --update to enable again.");
                return true;
            }
        }
        else
        {
            answer = MessageBoxA(NULL, "Overlunky WHIP can update automatically!\nDo you want to enable automatic updates?", "Overlunky Update", MB_ICONASTERISK | MB_YESNO);
            if (answer == IDNO)
            {
                std::ofstream voStream("Overlunky.version");
                if (!voStream.fail())
                {
                    voStream << "disabled" << std::endl;
                    voStream.close();
                }
                INFO("AutoUpdate: Disabled, delete Overlunky.version or run with --update to enable again.");
                return true;
            }
        }

        // Get headers
        if (!HttpQueryInfo(hConnect, HTTP_QUERY_RAW_HEADERS_CRLF, (LPVOID)response_headers, &response_headers_size, NULL))
        {
            INFO("AutoUpdate: Can't get version information from github");
            return false;
        }
        std::stringstream buffer(response_headers);
        std::string line;
        while (std::getline(buffer, line))
        {
            if (line.find("Last-Modified:") != std::string::npos)
                new_version = line;
        }
        if (!fs::exists(sSaveFilename))
        {
            INFO("AutoUpdate: {} is missing, updating!", sSaveFilename);
        }
        else if (old_version == new_version)
        {
            INFO("AutoUpdate: Running latest version!");
            return true;
        }
        else if (old_version == "")
            INFO("AutoUpdate: No old version information found, updating just in case!");
        else
            INFO("AutoUpdate: New version found, updating!");
        answer = MessageBoxA(NULL, "New Overlunky WHIP version available!\nDo you want to update?", "Overlunky Update", MB_ICONASTERISK | MB_YESNO);
        if (answer == IDNO)
            return true;

        remove("Overlunky.dll.old");
        rename("Overlunky.dll", "Overlunky.dll.old");

        INFO("AutoUpdate: Downloading {}...", sSaveFilename);

        // Open file to write
        errno_t err;
        if ((err = fopen_s(&pFile, sSaveFilename, "wb")) != 0)
        {
            InternetCloseHandle(hInternet);
            INFO("AutoUpdate: Can't write new file");
            return false;
        }

        // Get content size
        if (!HttpQueryInfo(hConnect, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, (LPVOID)&iBytesToRead, &iReadBytesOfRq, NULL))
        {
            iBytesToRead = 0;
        }

        do
        {
            if (!InternetReadFile(hConnect, acBuffer, BUFFER_SIZE, &iReadBytes))
            {
                fclose(pFile);
                InternetCloseHandle(hInternet);
                INFO("AutoUpdate: Can't read file from github");
                return false;
            }
            if (iReadBytes > 0)
            {
                fwrite(acBuffer, sizeof(char), iReadBytes, pFile);
            }
            if (iReadBytes <= 0)
            {
                break;
            }
        } while (TRUE);
        fflush(pFile);
        fclose(pFile);
        InternetCloseHandle(hInternet);
    }
    else
    {
        INFO("AutoUpdate: Can't connect to github");
        return false;
    }

    std::ofstream vStream("Overlunky.version");
    if (!vStream.fail())
    {
        vStream << new_version;
        vStream.close();
    }
    yes = true;
    return true;
}

bool update_launcher(const char* sURL, const char* sSaveFilename)
{
    const char* sHeader = NULL;
    const int BUFFER_SIZE = 32768;
    DWORD iFlags;
    const char* sAgent = "Overlunky Updater";
    HINTERNET hInternet;
    HINTERNET hConnect;
    char acBuffer[BUFFER_SIZE];
    DWORD iReadBytes;
    FILE* pFile = NULL;
    DWORD iBytesToRead = 0;
    DWORD iReadBytesOfRq = 4;

    // Get connection state
    InternetGetConnectedState(&iFlags, 0);
    if (iFlags & INTERNET_CONNECTION_OFFLINE)
    {
        INFO("AutoUpdate: Can't connect to the internet");
        return false;
    }

    // Open internet session
    if (!(iFlags & INTERNET_CONNECTION_PROXY))
    {
        hInternet = InternetOpenA(sAgent, INTERNET_OPEN_TYPE_PRECONFIG_WITH_NO_AUTOPROXY, NULL, NULL, 0);
    }
    else
    {
        hInternet = InternetOpenA(sAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    }
    if (hInternet)
    {
        if (sHeader == NULL)
        {
            sHeader = "Accept: */*\r\n\r\n";
        }

        // Get URL
        hConnect = InternetOpenUrlA(hInternet, sURL, sHeader, lstrlenA(sHeader), INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_RELOAD, 0);
        if (!hConnect)
        {
            InternetCloseHandle(hInternet);
            INFO("AutoUpdate: Can't get release information from github");
            return false;
        }

        remove("Overlunky.exe.old");
        rename("Overlunky.exe", "Overlunky.exe.old");

        INFO("AutoUpdate: Downloading {}...", sSaveFilename);

        // Open file to write
        errno_t err;
        if ((err = fopen_s(&pFile, sSaveFilename, "wb")) != 0)
        {
            InternetCloseHandle(hInternet);
            INFO("AutoUpdate: Can't write new file");
            return false;
        }

        // Get content size
        if (!HttpQueryInfo(hConnect, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, (LPVOID)&iBytesToRead, &iReadBytesOfRq, NULL))
        {
            iBytesToRead = 0;
        }

        do
        {
            if (!InternetReadFile(hConnect, acBuffer, BUFFER_SIZE, &iReadBytes))
            {
                fclose(pFile);
                InternetCloseHandle(hInternet);
                INFO("AutoUpdate: Can't read file from github");
                return false;
            }
            if (iReadBytes > 0)
            {
                fwrite(acBuffer, sizeof(char), iReadBytes, pFile);
            }
            if (iReadBytes <= 0)
            {
                break;
            }
        } while (TRUE);
        fflush(pFile);
        fclose(pFile);
        InternetCloseHandle(hInternet);
    }
    else
    {
        INFO("AutoUpdate: Can't connect to github");
        return false;
    }

    return true;
}

void wait()
{
    if (g_console)
    {
        while (true)
        {
            if (auto res = find_process(g_exe))
                std::this_thread::sleep_for(1s);
            else
                break;
        }
    }
}

bool inject_search(fs::path overlunky_path)
{
    SetConsoleTitle("Overlunky | Start your game or press ENTER to launch ../Spel2.exe!");
    INFO("Searching for Spel2.exe process...");
    INFO("Start your game or press ENTER to launch ../Spel2.exe!");
    Process proc;
    while (true)
    {
        if (auto res = find_process("Spel2.exe"))
        {
            proc = res.value();
            break;
        }
        if (kbhit())
        {
            if (getche() == '\r')
            {
                return true;
            }
        }
        std::this_thread::sleep_for(200ms);
    }
    SetConsoleTitle("Overlunky");
    INFO("Found Spel2.exe PID: {}", proc.info.pid);
    inject_dll(proc, overlunky_path.string());
    INFO("DLL injected");
    wait();
    return false;
}

bool launch(fs::path exe_path, fs::path overlunky_path, bool& do_inject)
{
    auto exe_dir = fs::canonical(exe_path).parent_path();
    auto cwd = fs::current_path();
    g_exe = exe_path.filename().string();

    char dll_path[MAX_PATH] = {};
    sprintf_s(dll_path, MAX_PATH, "%s", overlunky_path.string().c_str());

    const char* dll_paths[] = {
        dll_path,
    };

    if (do_inject)
        INFO("Launching game... {}", exe_path.string());
    else
        INFO("Launching game with DLL... {}", exe_path.string());

    const auto child_env = []()
    {
        std::string child_env_ = "SteamAppId=418530";

        const auto this_env = GetEnvironmentStrings();
        auto lpszVariable = this_env;
        while (*lpszVariable)
        {
            child_env_ += '\0';
            child_env_ += lpszVariable;
            lpszVariable += strlen(lpszVariable) + 1;
        }
        FreeEnvironmentStrings(this_env);

        child_env_ += '\0';
        return child_env_;
    }();

    PROCESS_INFORMATION pi{};
    STARTUPINFOA si{};
    si.cb = sizeof(STARTUPINFO);

    if (!do_inject && DetourCreateProcessWithDlls(NULL, (LPSTR)exe_path.string().c_str(), NULL, NULL, TRUE, CREATE_DEFAULT_ERROR_MODE, (LPVOID)child_env.c_str(), exe_dir.string().c_str(), &si, &pi, 1, dll_paths, NULL))
    {
        INFO("Game launched with DLL");
        wait();
        CloseHandle(pi.hThread);
        return true;
    }
    else if (CreateProcess((LPSTR)exe_path.string().c_str(), NULL, NULL, NULL, TRUE, 0, (LPVOID)child_env.c_str(), exe_dir.string().c_str(), &si, &pi))
    {
        auto proc = Process{pi.hProcess, {g_exe, pi.dwProcessId}};
        INFO("Game launched, injecting DLL...");
        inject_dll(proc, overlunky_path.string());
        INFO("DLL injected");
        wait();
        CloseHandle(pi.hThread);
        return false;
    }

    return false;
}

std::string get_dll_version(fs::path overlunky_path)
{
    static const HMODULE dll = LoadLibraryEx(overlunky_path.string().c_str(), NULL, DONT_RESOLVE_DLL_REFERENCES);
    if (!dll)
        return "UNKNOWN";
    typedef const char*(__stdcall * dll_version_fun)();
    dll_version_fun dll_version = nullptr;
    dll_version = reinterpret_cast<dll_version_fun>(GetProcAddress(dll, "dll_version"));
    if (!dll_version)
        return "UNKNOWN";
    return std::string((*dll_version)());
}

int main(int argc, char** argv)
{
    CmdLineParser cmd_line_parser(argc, argv);

    bool info_dump = GetCmdLineParam<bool>(cmd_line_parser, "info_dump", false);
    auto overlunky_path = get_dll_path(info_dump ? "\\info_dump.dll" : "\\Overlunky.dll");
    bool version_info = GetCmdLineParam<bool>(cmd_line_parser, "version", false);
    bool help = GetCmdLineParam<bool>(cmd_line_parser, "help", false);
    if (help)
    {
        INFO("Usage:");
        INFO("Without --launch_game the launcher will search for a process called Spel2.exe and inject OL when found.");
        INFO("You can press ENTER to stop searching and try to launch the game from the parent folder.");
        INFO("Command line switches:");
        INFO("  --launch_game [path]    launch ../Spel2.exe, path/Spel2.exe, or a specific exe, and load OL with Detours");
        INFO("  --console               keep console open to debug scripts etc");
        INFO("  --inject                use the old injection method instead of Detours with --launch_game");
        INFO("  --info_dump             output a bunch of game data to 'Spelunky 2/game_data'");
        INFO("  --update                reset AutoUpdate setting and update launcher and DLL to the latest WHIP build");
        INFO("  --update_launcher       update launcher to the latest WHIP build");
        INFO("  --help                  show this helpful help");
        INFO("  --version               show version information");
        return 0;
    }

    std::string version(get_version());
    INFO("Overlunky EXE version: {}", version);
    if (version_info)
    {
        if (fs::exists(overlunky_path))
        {
            std::string dllversion(get_dll_version(overlunky_path));
            INFO("Overlunky DLL version: {}", dllversion);
        }
        else
        {
            INFO("Overlunky DLL version: MISSING");
        }
        return 0;
    }
    bool do_update = GetCmdLineParam<bool>(cmd_line_parser, "update", false);
    bool do_update_launcher = GetCmdLineParam<bool>(cmd_line_parser, "update_launcher", false);

    if (do_update)
        remove("Overlunky.version");

    if (version.find(".") == std::string::npos || do_update)
    {
        bool updated = false;
        auto_update("https://github.com/spelunky-fyi/overlunky/releases/download/whip/Overlunky.dll", "Overlunky.dll", updated);
        if (updated)
            do_update_launcher = true;
    }
    else if (!do_update)
    {
        INFO("AutoUpdate: Disabled on stable releases. Get the WHIP build to get automatic updates or run with --update.");
    }
    if (do_update_launcher)
    {
        if (update_launcher("https://github.com/spelunky-fyi/overlunky/releases/download/whip/Overlunky.exe", "Overlunky.exe"))
            INFO("AutoUpdate: Launcher was updated, you might want to restart it.");
    }

    if (!fs::exists(overlunky_path))
    {
        PANIC("DLL not found! {}", overlunky_path.string().data());
    }
    else
    {
        std::string dllversion(get_dll_version(overlunky_path));
        INFO("Overlunky DLL version: {}", dllversion);
    }

    auto launch_game_default = GetCmdLineParam<bool>(cmd_line_parser, "launch_game", false);
    auto launch_game = GetCmdLineParam<std::string_view>(cmd_line_parser, "launch_game", "");
    if (launch_game.empty() && launch_game_default)
        launch_game = "../Spel2.exe";

    bool do_inject = GetCmdLineParam<bool>(cmd_line_parser, "inject", false);
    g_console = GetCmdLineParam<bool>(cmd_line_parser, "console", false);
    if (info_dump)
    {
        do_inject = true;
        g_console = false;
    }
    fs::path exe;

    if (!launch_game.empty())
    {
        auto launch_path = fs::canonical(launch_game);
        if (fs::is_directory(launch_path))
            exe = launch_path / "Spel2.exe";
        else if (fs::is_regular_file(launch_path))
            exe = launch_path;
    }

    if (fs::exists(exe))
    {
        if (launch(exe, overlunky_path, do_inject))
            return 0;
    }
    else
    {
        if (inject_search(overlunky_path))
        {
            launch(fs::canonical("../Spel2.exe"), overlunky_path, do_inject);
        }
    }
    return 0;
}
