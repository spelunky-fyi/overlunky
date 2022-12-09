#include "injector.h"

#include <Psapi.h>     // for GetModuleBaseNameA
#include <TlHelp32.h>  // for PROCESSENTRY32, CreateToolhelp32Snapshot, Pro...
#include <algorithm>   // for transform
#include <cctype>      // for tolower
#include <cstring>     // for size_t, strrchr, NULL
#include <new>         // for operator new
#include <type_traits> // for move, forward
#include <utility>     // for min
#include <vector>      // for vector, _Vector_iterator, _Vector_const_iterator

#include "logger.h" // for PANIC, DEBUG

std::vector<MemoryMap> memory_map(const Process& proc)
{
    std::vector<MemoryMap> result;
    size_t cur = 0;
    MEMORY_BASIC_INFORMATION mbf = {};
    char buffer[0x1000] = {};

    while (VirtualQueryEx(proc.handle, (LPVOID)cur, &mbf, sizeof(mbf)) != 0)
    {
        auto res = GetModuleBaseNameA(proc.handle, (HMODULE)cur, buffer, sizeof(buffer));
        if (res != 0)
        {
            result.push_back(MemoryMap{cur, std::string(buffer)});
        };
        cur += mbf.RegionSize;
    }
    return result;
}

std::string to_lower(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c)
                   { return (unsigned char)std::tolower(c); });
    return s;
}

size_t find_base(Process proc, std::string name)
{
    auto map = memory_map(proc);
    name = to_lower(name);
    for (auto&& item : map)
    {
        if (name.find(to_lower(item.name)) != std::string::npos)
            return item.addr;
    }

    PANIC("Cannot find library in the target process: {}", name);
}

LPVOID alloc(Process proc, size_t size)
{
    auto res = VirtualAllocEx(proc.handle, nullptr, (size + 0xFFF) & ~0xFFF, MEM_COMMIT, 0x40);
    if (res == NULL)
    {
        PANIC("Allocation failed: {:#x}", GetLastError());
    }
    // DEBUG("Allocated memory: {}", res);
    return res;
}

void write_mem(Process proc, LPVOID addr, std::string str)
{
    WriteProcessMemory(proc.handle, addr, str.data(), str.size(), NULL);
}

LPVOID alloc_str(Process proc, std::string str)
{
    auto addr = alloc(proc, str.size() + 1);
    write_mem(proc, addr, str);
    return addr;
}

LPTHREAD_START_ROUTINE find_function(const Process& proc, const std::string& library, const std::string& function)
{
    auto library_ptr = (size_t)LoadLibraryA(library.data());

    if (library_ptr == 0)
    {
        PANIC("Cannot find the address of the library in current process: {}", library.data());
    }

    auto addr = (size_t)GetProcAddress((HMODULE)library_ptr, function.data());

    if (addr == 0)
    {
        PANIC(
            "Cannot find the address of the function in current process: {} :: "
            "{}",
            library.data(),
            function.data());
    }

    return reinterpret_cast<LPTHREAD_START_ROUTINE>(addr - library_ptr + find_base(proc, library));
}

void call(const Process& proc, LPTHREAD_START_ROUTINE addr, LPVOID args)
{
    // DEBUG("Calling: {}", (void*)addr);
    auto handle = CreateRemoteThread(proc.handle, nullptr, 0, addr, args, 0, nullptr);
    WaitForSingleObject(handle, INFINITE);
}

void inject_dll(const Process& proc, const std::string& name)
{
    auto str = alloc_str(proc, name);
    INFO("Injecting DLL into process... {}", name);
    call(proc, find_function(proc, "KERNEL32.DLL", "LoadLibraryA"), str);
}

std::vector<ProcessInfo> get_processes()
{
    // No unicode
#undef Process32First
#undef Process32Next
#undef PROCESSENTRY32
    std::vector<ProcessInfo> res;
    auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == nullptr)
        return {};

    PROCESSENTRY32 ppe = {sizeof(ppe)};
    auto proc = Process32First(snapshot, &ppe);

    while (proc)
    {
        auto name = ppe.szExeFile;
        if (auto delim = strrchr(name, '\\'))
            name = delim;
        res.push_back({name, ppe.th32ProcessID});
        proc = Process32Next(snapshot, &ppe);
    }
    return res;
}

std::optional<Process> find_process(std::string name)
{
    for (auto& proc : get_processes())
    {
        if (proc.name == name)
        {
            return Process{OpenProcess(PROCESS_ALL_ACCESS, 0, proc.pid), proc};
        }
    }
    return {};
}
