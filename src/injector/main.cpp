#include <chrono>
#include <filesystem>
#include <thread>

#include "cmd_line.h"
#include "injector.h"
#include "logger.h"
#include "version.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <strsafe.h>
#include <wininet.h>

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
    const int iInterval = 250;
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
        std::ifstream vStream("overlunky.version");
        if (!vStream.fail())
        {
            std::stringstream buffer;
            buffer << vStream.rdbuf();
            old_version = buffer.str();
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
        if (old_version == new_version)
        {
            INFO("AutoUpdate: Running latest version!");
            return true;
        }
        INFO("AutoUpdate: New version found! Updating...");

        // Open file to write
        if (!(pFile = fopen(sSaveFilename, "wb")))
        {
            InternetCloseHandle(hInternet);
            INFO("AutoUpdate: Can't write new dll file");
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
    auto_update("https://github.com/Dregu/overlunky/releases/download/whip/injected.dll", "injected.dll");

    CmdLineParser cmd_line_parser(argc, argv);

    bool info_dump = GetCmdLineParam<bool>(cmd_line_parser, "info_dump", false);
    auto overlunky_path = get_dll_path(info_dump ? "\\info_dump.dll" : "\\injected.dll");

    if (!fs::exists(overlunky_path))
    {
        PANIC("DLL not found! {}", overlunky_path.string().data());
    }

    INFO("Overlunky launcher version: {}", get_version());

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
