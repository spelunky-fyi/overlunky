#include "virtual_table.hpp"
#include "logger.h"
#include "memory.hpp"

size_t get_virtual_function_address(VTABLE_OFFSET tableEntry, uint32_t relativeOffset)
{
    static auto firstTableEntry = get_address("virtual_functions_table");

    auto mem = Memory::get();
    if (firstTableEntry == 0)
    {
        return 0;
    }
    size_t* func_address = reinterpret_cast<size_t*>(firstTableEntry + ((static_cast<size_t>(tableEntry) + relativeOffset) * sizeof(size_t)));
    return *func_address - mem.exe_ptr;
}
