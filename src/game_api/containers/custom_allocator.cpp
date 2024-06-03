#include "custom_allocator.hpp"

#include <string> // for operator""sv

#include "memory.hpp"       // for memory_read, Memory
#include "search.hpp"       // for get_address
#include "thread_utils.hpp" // for OnHeapPointer

using CustomMallocFun = void*(void*, std::size_t);
using CustomFreeFun = void*(void*, void*);

void* custom_malloc(std::size_t size)
{
    static CustomMallocFun* _malloc = (CustomMallocFun*)get_address("custom_malloc"sv);
    static size_t _heap_ptr_malloc_base = *reinterpret_cast<size_t*>(get_address("malloc_base"sv));
    void* _alloc_base = OnHeapPointer<void>(_heap_ptr_malloc_base).decode_local();
    return _malloc(_alloc_base, size);
}
void custom_free(void* mem)
{
    static CustomFreeFun* _free = (CustomFreeFun*)get_address("custom_free"sv);
    static size_t _heap_ptr_malloc_base = *reinterpret_cast<size_t*>(get_address("malloc_base"sv));
    void* _alloc_base = OnHeapPointer<void>(_heap_ptr_malloc_base).decode_local();
    _free(_alloc_base, mem);
}
