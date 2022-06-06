#include "custom_allocator.hpp"

#include "memory.hpp"
#include "thread_utils.hpp"

using MallocFun = void*(void*, std::size_t);
using FreeFun = void* (void*, void*);

void* custom_malloc(std::size_t size)
{
    static MallocFun* _malloc = (MallocFun*)get_address("custom_malloc"sv);
    static void* _alloc_base = OnHeapPointer<void>(read_u64(Memory::get().at_exe(0x22dd96b8))).decode();
    return _malloc(_alloc_base, size);
}
void custom_free(void* mem)
{
    static FreeFun* _free = (FreeFun*)get_address("custom_free"sv);
    static void* _alloc_base = OnHeapPointer<void>(read_u64(Memory::get().at_exe(0x22dd96b8))).decode();
    _free(_alloc_base, mem);
}
