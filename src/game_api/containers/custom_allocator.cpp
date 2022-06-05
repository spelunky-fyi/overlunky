#include "custom_allocator.hpp"

#include "memory.hpp"
#include "thread_utils.hpp"

using MallocFun = void*(std::size_t, std::size_t);
using FreeFun = void*(std::size_t, void*);

void* custom_malloc(std::size_t size)
{
    static MallocFun* _malloc = *(MallocFun**)get_address("custom_malloc"sv);
    return _malloc(heap_base(), size);
}
void custom_free(void* mem)
{
    static FreeFun* _free = *(FreeFun**)get_address("custom_free"sv);
    _free(heap_base(), mem);
}
