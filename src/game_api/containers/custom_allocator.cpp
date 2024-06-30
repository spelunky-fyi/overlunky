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
    static void* _alloc_base = OnHeapPointer<void>(*(size_t*)get_address("malloc_base"sv)).decode(); // probably should be decode_local
    return _malloc(_alloc_base, size);
}
void custom_free(void* mem)
{
    static CustomFreeFun* _free = (CustomFreeFun*)get_address("custom_free"sv);
    static void* _alloc_base = OnHeapPointer<void>(*(size_t*)get_address("malloc_base"sv)).decode(); // probably should be decode_local
    _free(_alloc_base, mem);
}
