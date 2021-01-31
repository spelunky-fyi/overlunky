#pragma once

#include <Windows.h>
#include <winternl.h>

#include <cstdint>

HANDLE get_main_thread();

typedef struct _THREAD_BASIC_INFORMATION {
    NTSTATUS ExitStatus;
    PVOID TebBaseAddress;
    CLIENT_ID ClientId;
    KAFFINITY AffinityMask;
    KPRIORITY Priority;
    KPRIORITY BasePriority;
} THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;

size_t heap_base();

struct CriticalSection {
    HANDLE thread;

    CriticalSection() {
        thread = get_main_thread();
        SuspendThread(thread);
    }

    ~CriticalSection() { ResumeThread(thread); }
};
