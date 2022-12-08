#include <Windows.h> // for PROCESS_INFORMATION, CloseHandle, GetEnvironm...
#include <chrono>    // for operator<=>, operator-, operator+, operator""s
#include <compare>   // for operator<, operator<=, operator>
#include <ctime>     // for errno_t
#include <detours.h>
#include <filesystem> // for exists, path
#include <fstream>    // for basic_ostream, basic_ofstream, ofstream, basi...
#include <iostream>
#include <locale>      // for num_put, num_get
#include <new>         // for operator new
#include <optional>    // for optional
#include <sstream>     // for basic_stringstream
#include <stdint.h>    // for uint64_t
#include <stdio.h>     // for NULL, fclose, sprintf_s, fflush, fopen_s, fwrite
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

fs::path get_dll_path(const char* rel_path)
{
    char buf[0x1000];
    GetModuleFileNameA(NULL, buf, sizeof(buf));
    fs::path path(buf);
    return path.parent_path().concat(rel_path);
}

bool auto_update(const char* sURL, const char* sSaveFilename, const char* sHeader = NULL)
{
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
        std::ifstream viStream("overlunky.version");
        if (!viStream.fail())
        {
            std::stringstream buffer;
            buffer << viStream.rdbuf();
            old_version = buffer.str();
            if (old_version.find("disabled") != std::string::npos)
            {
                INFO("AutoUpdate: Disabled, delete overlunky.version to enable again.");
                return true;
            }
        }
        else
        {
            answer = MessageBoxA(NULL, "Overlunky WHIP can update automatically!\nDo you want to enable automatic updates?", "Overlunky Update", MB_ICONASTERISK | MB_YESNO);
            if (answer == IDNO)
            {
                std::ofstream voStream("overlunky.version");
                if (!voStream.fail())
                {
                    voStream << "disabled" << std::endl;
                    voStream.close();
                }
                INFO("AutoUpdate: Disabled, delete overlunky.version to enable again.");
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

    std::ofstream vStream("overlunky.version");
    if (!vStream.fail())
    {
        vStream << new_version;
        vStream.close();
    }
    return true;
}

int main(int argc, char** argv)
{
    std::string version(get_version());
    INFO("Overlunky launcher version: {}", version);

    if (version.find(".") == std::string::npos)
    {
        auto_update("https://github.com/spelunky-fyi/overlunky/releases/download/whip/injected.dll", "injected.dll");
    }
    else
    {
        INFO("AutoUpdate: Disabled on stable releases. Get the WHIP build to get automatic updates.");
    }

    CmdLineParser cmd_line_parser(argc, argv);

    bool info_dump = GetCmdLineParam<bool>(cmd_line_parser, "info_dump", false);
    auto overlunky_path = get_dll_path(info_dump ? "\\info_dump.dll" : "\\injected.dll");

    if (!fs::exists(overlunky_path))
    {
        PANIC("DLL not found! {}", overlunky_path.string().data());
    }

    SECURITY_ATTRIBUTES sa{};
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;

    STARTUPINFOA si{};
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags |= STARTF_USESTDHANDLES;

    std::string_view launch_exe = GetCmdLineParam<std::string_view>(cmd_line_parser, "launch_exe", "");
    std::string_view launch_game = GetCmdLineParam<std::string_view>(cmd_line_parser, "launch_game", "");
    std::string_view launch_playlunky = GetCmdLineParam<std::string_view>(cmd_line_parser, "launch_playlunky", "");

    bool do_inject = false;

    if (!launch_game.empty())
        launch_exe = fmt::format("{}/Spel2.exe", launch_game);

    if (!launch_playlunky.empty())
    {
        launch_exe = fmt::format("{}/playlunky_launcher.exe", launch_playlunky);
        do_inject = true;
    }

    if (!launch_exe.empty() && fs::exists(launch_exe))
    {
        INFO("Launching {}", launch_exe);

        const std::string exe_path{launch_exe};
        fs::path exe_dir{exe_path};

        char dir_path[MAX_PATH] = {};
        GetCurrentDirectoryA(MAX_PATH, dir_path);

        char injected_dll_path[MAX_PATH] = {};
        sprintf_s(injected_dll_path, MAX_PATH, "%s/injected.dll", dir_path);

        const char* dll_paths[] = {
            injected_dll_path,
        };

        const auto child_env = []()
        {
            std::string child_env = "SteamAppId=418530";

            const auto this_env = GetEnvironmentStrings();
            auto lpszVariable = this_env;
            while (*lpszVariable)
            {
                child_env += '\0';
                child_env += lpszVariable;
                lpszVariable += strlen(lpszVariable) + 1;
            }
            FreeEnvironmentStrings(this_env);

            child_env += '\0';
            return child_env;
        }();

        PROCESS_INFORMATION pi{};

        if (!do_inject && DetourCreateProcessWithDlls(NULL, (LPSTR)exe_path.c_str(), NULL, NULL, TRUE, CREATE_DEFAULT_ERROR_MODE, (LPVOID)child_env.c_str(), exe_dir.parent_path().string().c_str(), &si, &pi, sizeof(dll_paths) / sizeof(const char*), dll_paths, NULL))
        {
            CloseHandle(pi.hThread);
        }
        else if (CreateProcess((LPSTR)exe_path.c_str(), NULL, NULL, NULL, TRUE, 0, (LPVOID)child_env.c_str(), exe_dir.parent_path().string().c_str(), &si, &pi))
        {
            CloseHandle(pi.hThread);
        }
    }
    else
    {
        do_inject = true;
    }

    if (do_inject)
    {
        INFO("Searching for Spel2.exe process... Pro tip: You have to launch it yourself.");
        Process proc;
        while (true)
        {
            if (auto res = find_process("Spel2.exe"))
            {
                proc = res.value();
                break;
            }
            std::this_thread::sleep_for(1s);
        };
        INFO("Found Spel2.exe PID: {}", proc.info.pid);
        inject_dll(proc, overlunky_path.string());
    }

    return 0;
}
