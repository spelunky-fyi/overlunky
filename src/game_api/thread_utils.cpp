#include "thread_utils.hpp"

#include <TlHelp32.h>
#include <Windows.h>

#include "memory.hpp"

HANDLE get_main_thread()
{
    static const auto main_thread = []
    {
        HANDLE main_thread = NULL;

        DWORD pid = GetCurrentProcessId();
        auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
        auto entry = THREADENTRY32{
            sizeof(THREADENTRY32),
        };
        auto keep = Thread32First(snapshot, &entry);
        while (keep)
        {
            if (entry.th32OwnerProcessID == pid)
            {
                main_thread = OpenThread(THREAD_ALL_ACCESS, 0, entry.th32ThreadID);
                break;
            }
            keep = Thread32Next(snapshot, &entry);
        }

        if (main_thread == NULL)
        {
            DEBUG("Didn't not get the thread. Process id: {}", pid);
        }

        return main_thread;
    }();
    return main_thread;
}

size_t* get_thread_heap_base(HANDLE thread)
{
    THREAD_BASIC_INFORMATION tib{};
    using FuncPtr = NTSTATUS(NTAPI*)(
        IN HANDLE ThreadHandle,
        IN THREADINFOCLASS ThreadInformationClass,
        OUT PVOID ThreadInformation,
        IN ULONG ThreadInformationLength,
        OUT PULONG ReturnLength OPTIONAL);
    static const auto NtQueryInformationThread_ptr = reinterpret_cast<FuncPtr>(GetProcAddress(GetModuleHandle("ntdll.dll"), "NtQueryInformationThread"));
    NtQueryInformationThread_ptr(thread, (_THREADINFOCLASS)0, (&tib), sizeof(THREAD_BASIC_INFORMATION), nullptr);
    return (size_t*)(read_u64(((uint64_t*)tib.TebBaseAddress)[11]) + 0x120);
}

size_t heap_base()
{
    static const auto main = get_main_thread();
    static const size_t* this_thread_heap_base_addr = get_thread_heap_base(main);
    return *this_thread_heap_base_addr;
}

size_t local_heap_base()
{
    thread_local const size_t* this_thread_heap_base_addr = get_thread_heap_base(GetCurrentThread());
    return *this_thread_heap_base_addr;
}
