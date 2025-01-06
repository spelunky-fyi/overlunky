#include "thread_utils.hpp"

#include <TlHelp32.h>   // for CreateToolhelp32Snapshot, THREADENTRY32, Thr...
#include <Windows.h>    // for HANDLE, GetCurrentProcessId, GetCurrentThread
#include <winternl.h>   // for KPRIORITY, NTSTATUS, CLIENT_ID, THREADINFOCLASS
#include <wtypesbase.h> // for ULONG

#include "logger.h"          // for DEBUG
#include "memory.hpp"        // for memory_read
#include "script/events.hpp" // for pre_copy_state_event

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

void HeapClone(HeapBase heap_to, uint64_t heap_container_from)
{
    auto heap_from = memory_read<uint64_t>(heap_container_from + 0x88);
    HeapBase heap_base_from = reinterpret_cast<HeapBase&>(heap_from);
    pre_copy_state_event(heap_base_from, heap_to);
}

// Original function params: clone_heap(ThreadStorageContainer to, ThreadStorageContainer from)
// HeapContainer has heap1 and heap2 variables, and some sort of timer, that just increases constantly, I guess to handle the rollback and multi-threaded stuff
// The rest of what HeapContainer has is unknown for now
// After writing to a chosen storage from the content of `from->heap1`, sets `to->heap2` to the newly copied thread storage
void init_heap_clone_hook()
{
    auto heap_clone = get_address("heap_clone");
    // Hook the function after it has chosen a thread storage to write to, and pass it to the hook
    size_t heap_clone_redirect_from_addr = heap_clone + 0x65;
    const std::string redirect_code = fmt::format(
        "\x51"             // PUSH       RCX
        "\x52"             // PUSH       RDX
        "\x41\x50"         // PUSH       R8
        "\x41\x51"         // PUSH       R9
        "\x48\x83\xEC\x28" // SUB        RSP, 28 // Shadow space + Stack alignment
        "\x4C\x89\xC9"     // MOV        RCX, R9 == heap_to
        "\x48\xb8{}"       // MOV        RAX, &HeapClone
        "\xff\xd0"         // CALL       RAX
        "\x48\x83\xC4\x28" // ADD        RSP, 28
        "\x41\x59"         // POP        R9
        "\x41\x58"         // POP        R8
        "\x5A"             // POP        RDX
        "\x59"sv,          // POP        RCX
        to_le_bytes(&HeapClone));

    patch_and_redirect(heap_clone_redirect_from_addr, 7, redirect_code, false, 0, false);
}

void HeapBase::copy_to(HeapBase other) const
{
    if (is_null() || other.is_null())
        return;

    auto fromBaseState = address();
    auto toBaseState = other.address();
    size_t iterIdx = 1;
    do
    {
        size_t copyContent = *(size_t*)((fromBaseState - 8) + iterIdx * 8);
        // variable used to fix pointers that point somewhere in the same Thread
        size_t diff = toBaseState - fromBaseState;
        if (copyContent >= fromBaseState + 0x2000000 || copyContent <= fromBaseState)
        {
            diff = 0;
        }
        *(size_t*)(toBaseState + iterIdx * 8 + -8) = diff + copyContent;

        // Almost same code as before, but on the next value, idk why
        copyContent = *(size_t*)(fromBaseState + iterIdx * 8);
        diff = toBaseState - fromBaseState;
        if (copyContent >= fromBaseState + 0x2000000 || copyContent <= fromBaseState)
        {
            diff = 0;
        }
        *(size_t*)(toBaseState + iterIdx * 8) = diff + copyContent;

        iterIdx = iterIdx + 2;
    } while (iterIdx != 0x400001);
};
