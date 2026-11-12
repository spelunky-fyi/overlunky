#pragma once

#include <Windows.h> // for LPTHREAD_START_ROUTINE, DWORD, HANDLE, LPVOID

#include <string> // for string

struct ProcessInfo
{
    std::string name;
    DWORD pid;
};

struct Process
{
    HANDLE handle{NULL};
    ProcessInfo info;

    Process() = default;
    Process(HANDLE h, ProcessInfo pi)
        : handle(h), info(pi){};
    Process(const Process&) = delete;
    Process& operator=(const Process&) = delete;
    Process(Process&& other) noexcept
        : handle(other.handle), info(std::move(other.info))
    {
        other.handle = NULL;
    }
    Process& operator=(Process&& other) noexcept
    {
        if (this != &other)
        {
            if (valid())
                CloseHandle(handle);

            handle = other.handle;
            info = std::move(other.info);
            other.handle = NULL;
        }
        return *this;
    }
    bool valid() const noexcept
    {
        return handle && handle != INVALID_HANDLE_VALUE;
    }
    ~Process()
    {
        if (valid())
            CloseHandle(handle);
    }
};

void inject_dll(const Process& proc, const std::string& name);
LPTHREAD_START_ROUTINE find_function(const Process& proc, const std::string& library, const std::string& function);
void call(const Process& proc, LPTHREAD_START_ROUTINE addr, LPVOID args);
Process find_process(std::string name);
bool find_dll_in_process(DWORD pid, const std::string& name);
