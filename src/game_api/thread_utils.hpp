#pragma once

#include <Windows.h> // for ResumeThread, SuspendThread, HANDLE
#include <cstddef>   // for size_t
#include <cstdint>   // for int64_t

HANDLE get_main_thread();
size_t heap_base();
size_t local_heap_base();

// Used for objects that are allocated with the game's custom allocator
template <typename T>
class OnHeapPointer
{
    int64_t ptr_;

  public:
    OnHeapPointer() = default;
    explicit OnHeapPointer(size_t ptr)
        : ptr_(ptr)
    {
    }

    T* decode()
    {
        return reinterpret_cast<T*>(ptr_ + heap_base());
    }

    T* decode_local()
    {
        return reinterpret_cast<T*>(ptr_ + local_heap_base());
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
