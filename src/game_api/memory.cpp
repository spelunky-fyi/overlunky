#include "memory.hpp"

#include <cstdlib>       // for exit
#include <cstring>       // for memcpy
#include <functional>    // for equal_to
#include <new>           // for operator new
#include <unordered_map> // for unordered_map, _Umap_traits<>::allocator_type
#include <utility>       // for min, max
#include <vector>        // for vector, _Vector_iterator, _Vector_const_ite...

ExecutableMemory::ExecutableMemory(std::string_view raw_code)
{
    SYSTEM_INFO system_info;
    GetSystemInfo(&system_info);

    auto const page_size = system_info.dwPageSize;
    auto const alloc_size = (raw_code.size() / page_size + 1) * page_size;

    auto const memory = (std::byte*)VirtualAlloc(nullptr, page_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (memory)
    {
        std::memcpy(memory, raw_code.data(), raw_code.size());

        DWORD dummy;
        VirtualProtect(memory, alloc_size, PAGE_EXECUTE_READ, &dummy);

        code = storage_t{memory};
    }
    else
    {
        std::exit(1);
    }
}

void ExecutableMemory::deleter_t::operator()(std::byte* mem) const
{
    VirtualFree(mem, 0, MEM_RELEASE);
}

size_t round_up(size_t i, size_t div)
{
    return ((i + div - 1) / div) * div;
}

void write_mem_prot(size_t addr, std::string_view payload, bool prot)
{
    DWORD old_protect = 0;
    auto page = addr & ~0xFFF;
    auto size = round_up((addr + payload.size() - page), 0x1000);
    if (prot)
    {
        VirtualProtect((void*)page, size, PAGE_EXECUTE_READWRITE, &old_protect);
    }
    memcpy((void*)addr, payload.data(), payload.size());
    if (prot)
    {
        VirtualProtect((LPVOID)page, size, old_protect, &old_protect);
    }
}
void write_mem_prot(size_t addr, std::string payload, bool prot)
{
    write_mem_prot(addr, std::string_view{payload}, prot);
}
void write_mem(size_t addr, std::string payload)
{
    write_mem_prot(addr, payload, false);
}

// Looks for padding between functions, which sometimes does not exist, in that case
// you might be able to specify a different distinct byte
size_t function_start(size_t off, uint8_t outside_byte)
{
    off &= ~0xf;
    while (memory_read<uint8_t>(off - 1) != outside_byte)
    {
        off -= 0x10;
    }
    return off;
}

LPVOID alloc_mem_rel32(size_t addr, size_t size)
{
    const size_t limit_addr = Memory::get().exe_ptr;
    LPVOID new_array = nullptr;
    size_t test_addr;

    test_addr = addr + 0x10000; // dunno why
    if (test_addr <= INT32_MAX)
        test_addr = 8;
    else
        test_addr -= INT32_MAX;

    for (; test_addr < limit_addr; test_addr += 0x100000)
    {
        new_array = VirtualAlloc((LPVOID)test_addr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (new_array)
            break;
    }
    return new_array;
}

std::unordered_map<std::string, std::vector<RecoverableMemory>> original_memory;

void write_mem_recoverable(std::string name, size_t addr, std::string_view payload, bool prot)
{
    if (!original_memory.contains(name))
    {
        char* old_data = (char*)VirtualAlloc(0, payload.size(), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (old_data)
        {
            memcpy(old_data, (char*)addr, payload.size());
            original_memory[name] = std::vector<RecoverableMemory>{{addr, old_data, payload.size(), prot}};
        }
    }
    else
    {
        bool new_addr = true;
        for (auto& it : original_memory[name])
        {
            if (it.address == addr)
            {
                new_addr = false;
                break;
            }
        }
        if (new_addr)
        {
            char* old_data = (char*)VirtualAlloc(0, payload.size(), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
            if (old_data)
            {
                memcpy(old_data, (char*)addr, payload.size());
                original_memory[name].push_back(RecoverableMemory{addr, old_data, payload.size(), prot});
            }
        }
    }
    write_mem_prot(addr, payload, prot);
}

void recover_mem(std::string name, size_t addr)
{
    if (original_memory.contains(name))
    {
        for (auto& it : original_memory[name])
            if (!addr || addr == it.address)
                write_mem_prot(it.address, std::string_view{it.old_data, it.size}, it.prot_used);
    }
}
