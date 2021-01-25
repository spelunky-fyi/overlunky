#include "thread_utils.h"
#include <Windows.h>
#include <TlHelp32.h>
#include "memory.h"

HANDLE get_main_thread() {
    ONCE(HANDLE) {
        HANDLE main_thread;
        DWORD pid = GetCurrentProcessId();
        auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
        auto entry = THREADENTRY32{
                sizeof(THREADENTRY32),
        };
        auto keep = Thread32First(snapshot, &entry);
        while (keep) {
            if (entry.th32OwnerProcessID == pid) {
                main_thread = OpenThread(THREAD_ALL_ACCESS, 0, entry.th32ThreadID);
                break;
            }
            keep = Thread32Next(snapshot, &entry);
        }
        return res = main_thread;
    }
}

size_t heap_base() {
    auto main = get_main_thread();
    THREAD_BASIC_INFORMATION tib = {};
    using FuncPtr = NTSTATUS(NTAPI *)(
            IN HANDLE ThreadHandle,
            IN THREADINFOCLASS ThreadInformationClass,
            OUT PVOID ThreadInformation,
            IN ULONG ThreadInformationLength,
            OUT PULONG ReturnLength OPTIONAL);
    static FuncPtr NtQueryInformationThread_ptr;
    if (NtQueryInformationThread_ptr == nullptr) {
        NtQueryInformationThread_ptr = reinterpret_cast<FuncPtr>(GetProcAddress(LoadLibraryA("ntdll.dll"),
                                                                                "NtQueryInformationThread"));
    }
    {
        NtQueryInformationThread_ptr(
                main,
                (_THREADINFOCLASS) 0,
                (&tib),
                sizeof(THREAD_BASIC_INFORMATION),
                nullptr);

        auto result = ((uint64_t *) tib.TebBaseAddress)[11];
        return read_u64(read_u64((result)) + 0x130);
    }
}
