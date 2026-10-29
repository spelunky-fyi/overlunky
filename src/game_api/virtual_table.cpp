#include "virtual_table.hpp"

#include "memory.hpp"      // for Memory
#include "search.hpp"      // for get_address
#include "vtable_hook.hpp" // for get_hook_function

size_t get_virtual_function_address(VTABLE_OFFSET table_entry, uint32_t function_index)
{
    static auto first_table_entry = get_address("virtual_functions_table");

    auto mem = Memory::get();
    if (first_table_entry == 0)
    {
        return 0;
    }
    void** vtable = reinterpret_cast<void**>(first_table_entry + static_cast<size_t>(table_entry) * sizeof(size_t));
    // If the function is hooked the vtable points to our detour, return the original game function instead
    if (void* original_function = get_hook_function(&vtable, function_index))
    {
        return reinterpret_cast<size_t>(original_function) - mem.exe_ptr;
    }
    return reinterpret_cast<size_t>(vtable[function_index]) - mem.exe_ptr;
}

size_t get_virtual_function_address(void* object, uint32_t function_index)
{
    auto v_table = *static_cast<size_t**>(object);
    return *(v_table + function_index);
}
