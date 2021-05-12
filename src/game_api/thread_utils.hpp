#pragma once

#include <Windows.h>
#include <winternl.h>

#include <cstdint>

HANDLE get_main_thread();

typedef struct _THREAD_BASIC_INFORMATION
{
    NTSTATUS ExitStatus;
    PVOID TebBaseAddress;
    CLIENT_ID ClientId;
    KAFFINITY AffinityMask;
    KPRIORITY Priority;
    KPRIORITY BasePriority;
} THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;

size_t heap_base();

// I found some places that use offset from heap base, so wrote this class
template <typename T> class OnHeapPointer
{
    int64_t ptr_;

  public:
    explicit OnHeapPointer(size_t ptr) : ptr_(ptr)
    {
    }

    T* decode()
    {
        return reinterpret_cast<T*>(ptr_ + heap_base());
    }

    T* operator->()
    {
        return decode();
    }
};

struct CriticalSection
{
    HANDLE thread;

    CriticalSection()
    {
        thread = get_main_thread();
        SuspendThread(thread);
    }

    ~CriticalSection()
    {
        ResumeThread(thread);
    }
};
