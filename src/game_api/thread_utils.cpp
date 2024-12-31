#include "thread_utils.hpp"

#include <TlHelp32.h>   // for CreateToolhelp32Snapshot, THREADENTRY32, Thr...
#include <Windows.h>    // for HANDLE, GetCurrentProcessId, GetCurrentThread
#include <winternl.h>   // for KPRIORITY, NTSTATUS, CLIENT_ID, THREADINFOCLASS
#include <wtypesbase.h> // for ULONG

#include "logger.h"   // for DEBUG
#include "memory.hpp" // for memory_read

constexpr size_t TEB_OFFSET = 0x120;

HANDLE get_main_thread()
{
    static const auto main_thread = []
    {
        HANDLE main_thread_handle = NULL;

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
                main_thread_handle = OpenThread(THREAD_ALL_ACCESS, 0, entry.th32ThreadID);
                break;
            }
            keep = Thread32Next(snapshot, &entry);
        }

        if (main_thread_handle == NULL)
        {
            DEBUG("Didn't not get the thread. Process id: {}", pid);
        }

        return main_thread_handle;
    }();
    return main_thread;
}

typedef struct _THREAD_BASIC_INFORMATION
{
    NTSTATUS ExitStatus;
    PVOID TebBaseAddress;
    CLIENT_ID ClientId;
    KAFFINITY AffinityMask;
    KPRIORITY Priority;
    KPRIORITY BasePriority;
} THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;
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
    return (size_t*)(memory_read<uint64_t>(((uint64_t*)tib.TebBaseAddress)[11]) + TEB_OFFSET);
}

HeapBase HeapBase::get_main()
{
    static const auto main_thread = get_main_thread();
    static const uintptr_t* this_thread_heap_base_addr = get_thread_heap_base(main_thread);
    return *this_thread_heap_base_addr;
}

HeapBase HeapBase::get()
{
    thread_local const uintptr_t* this_thread_heap_base_addr = get_thread_heap_base(GetCurrentThread());
    if (this_thread_heap_base_addr == nullptr || *this_thread_heap_base_addr == NULL) // keeping for now just to be sure
        return get_main();
    return *this_thread_heap_base_addr;
}

HeapBase HeapBase::get(uint8_t slot)
{
    if (slot >= MAX_SAVE_SLOTS)
        return NULL;
    static HeapBase* save_slots = reinterpret_cast<HeapBase*>(get_address("save_states"));
    return *(save_slots + slot);
}
