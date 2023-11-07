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

    size_t test_addr = addr + 0x10000; // dunno why, without this it can get address that is more than 32bit away

    if (test_addr <= INT32_MAX) // redunded check as you probably won't get address that is less than INT32_MAX from "zero"
        test_addr = 8;          // but i did it just in case so you can't get overflow, also can't feed 0 to the VirtualAlloc as that just means: find memory wherever
    else
        test_addr -= INT32_MAX;

    // align to 4KB memory page size
    test_addr = (test_addr + 0xFFF) & ~0xFFF;

    for (; test_addr < limit_addr; test_addr += 0x1000) // add 4KB memory page size
    {
        new_array = VirtualAlloc((LPVOID)test_addr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (new_array)
            break;
    }
    return new_array;
}

std::unordered_map<std::string, std::vector<RecoverableMemory>> original_memory;
std::unordered_set<std::string> edited_memory;

void write_mem_recoverable(std::string name, size_t addr, std::string_view payload, bool prot)
{
    auto map_it = original_memory.find(name);
    if (map_it == original_memory.end())
    {
        char* old_data = (char*)VirtualAlloc(0, payload.size(), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (old_data)
        {
            memcpy(old_data, (char*)addr, payload.size());
            original_memory.emplace(name, std::vector<RecoverableMemory>{{addr, old_data, payload.size(), prot}});
        }
    }
    else
    {
        bool new_addr = true;
        for (auto& it : map_it->second)
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
                map_it->second.emplace_back(addr, old_data, payload.size(), prot);
            }
        }
    }
    write_mem_prot(addr, payload, prot);
    edited_memory.insert(name);
}

void recover_mem(std::string name, size_t addr)
{
    if (original_memory.contains(name))
    {
        for (auto& it : original_memory[name])
            if (!addr || addr == it.address)
                write_mem_prot(it.address, std::string_view{it.old_data, it.size}, it.prot_used);
        edited_memory.erase(name);
    }
}

bool mem_written(std::string name)
{
    return edited_memory.contains(name);
}

size_t patch_and_redirect(size_t addr, size_t replace_size, const std::string_view payload, bool just_nop, size_t return_to_addr, bool game_code_first)
{
    constexpr auto jump_size = 5;

    if (replace_size < jump_size)
        return 0;

    size_t data_size_to_move = replace_size;
    if (just_nop)
    {
        data_size_to_move = 0;
    }
    const size_t target = std::max(return_to_addr, addr);
    const auto new_memory_size = payload.size() + data_size_to_move + jump_size;

    auto new_code = (char*)alloc_mem_rel32(target, new_memory_size);
    if (new_code == nullptr)
        return 0;

    if (game_code_first && !just_nop)
    {
        std::memcpy(new_code, (void*)addr, data_size_to_move);
        std::memcpy(new_code + data_size_to_move, payload.data(), payload.size());
    }
    else
    {
        std::memcpy(new_code + data_size_to_move, payload.data(), payload.size());

        if (!just_nop)
            std::memcpy(new_code, (void*)addr, data_size_to_move);
    }

    size_t return_addr = addr + replace_size;
    if (return_to_addr != 0)
    {
        return_addr = return_to_addr;
    }
    int32_t rel_back = static_cast<int32_t>(return_addr - (size_t)(new_code + new_memory_size));
    const std::string jump_back = fmt::format("\xE9{}"sv, to_le_bytes(rel_back));
    std::memcpy(new_code + payload.size() + data_size_to_move, jump_back.data(), jump_size);

    DWORD dummy;
    VirtualProtect(new_code, new_memory_size, PAGE_EXECUTE_READ, &dummy);

    int32_t rel = static_cast<int32_t>((size_t)new_code - (addr + jump_size));
    const std::string redirect_code = fmt::format("\xE9{}{}"sv, to_le_bytes(rel), get_nop(replace_size - jump_size));
    write_mem_prot(addr, redirect_code, true);
    return (size_t)new_code;
}

std::string get_nop(size_t size, bool true_nop)
{
    if (true_nop)
        return std::string(size, '\x90');

    switch (size)
    {
    case 0:
        return "";
    case 1:
        return "\x90"s;
    case 2:
        return "\x66\x90"s;
    case 3:
        return "\x0F\x1F\x00"s;
    case 4:
        return "\x0F\x1F\x40\x00"s;
    case 5:
        return "\x0F\x1F\x44\x00\x00"s;
    case 6:
        return "\x66\x0F\x1F\x44\x00\x00"s;
    case 7:
        return "\x0F\x1F\x80\x00\x00\x00\x00"s;
    case 8:
        return "\x0F\x1F\x84\x00\x00\x00\x00\x00"s;
    case 9:
        return "\x66\x0F\x1F\x84\x00\x00\x00\x00\x00"s;
    case 10:
        return "\x66\x2E\x0F\x1F\x84\x00\x00\x00\x00\x00"s;
    default:
    {
        std::string ret_str;
        size_t remaning = size;

        for (uint8_t idx = 10; idx > 0; --idx)
        {
            size_t d_t = remaning / idx;
            if (d_t > 0)
            {
                std::string c_nop = get_nop(idx);
                for (; d_t > 0; --d_t)
                {
                    ret_str += c_nop;
                    remaning -= idx;
                }
            }
        }
        return ret_str;
    }
    }
}
