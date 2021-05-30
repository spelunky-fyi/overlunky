#include "virtual_table.hpp"
#include "logger.h"
#include "memory.hpp"

size_t get_virtual_function_address(VTABLE_OFFSET tableEntry, uint32_t relativeOffset)
{
    auto mem = Memory::get();

    static size_t firstTableEntry = 0;
    if (firstTableEntry == 0)
    {
        auto offset = find_inst(mem.exe(), "\x4C\x89\x6C\x24\x20\xFF\x15"s, mem.after_bundle);
        firstTableEntry = mem.at_exe(decode_pc(mem.exe(), offset + 5, 2));
    }

    if (firstTableEntry == 0)
    {
        return 0;
    }
    size_t* func_address = reinterpret_cast<size_t*>(firstTableEntry + ((static_cast<uint32_t>(tableEntry) + relativeOffset) * sizeof(size_t)));
    return *func_address - mem.exe_ptr;
}
