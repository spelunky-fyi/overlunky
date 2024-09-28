#include "virtual_table.hpp"

#include "memory.hpp" // for Memory
#include "search.hpp" // for get_address

size_t get_virtual_function_address(VTABLE_OFFSET table_entry, uint32_t function_index)
{
    static auto first_table_entry = get_address("virtual_functions_table");

    auto& mem = Memory::get();
    if (first_table_entry == 0)
    {
        return 0;
    }
    size_t* func_address = reinterpret_cast<size_t*>(first_table_entry + ((static_cast<size_t>(table_entry) + function_index) * sizeof(size_t)));
    return *func_address - mem.exe_address();
}
