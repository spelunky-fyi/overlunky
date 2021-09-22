#include "search.hpp"

// clang-format off
#include <Windows.h>
#include <Psapi.h>
// clang-format on

#include <algorithm>

#include "logger.h"
#include "memory.hpp"

// Decodes the program counter inside an instruction
// The default simple variant is 3 bytes instruction, 4 bytes rel. address, 0 bytes suffix:
//      e.g.  movups xmm0, ptr[XXXXXXXX] = 0F1005 XXXXXXXX
// Some instructions have 2 bytes instruction, so specify 2 for opcode_offset
//      e.g.  call ptr[XXXXXXXX] = FF15 XXXXXXXX
// Some (write) instructions have a value after the program counter to be extracted, so specify the opcode_suffix_offset
//      e.g.  mov word ptr[XXXXXXXX], 1 = 66:C705 XXXXXXXX 0100 (opcode_suffix_offset = 2)
size_t decode_pc(const char* exe, size_t offset, uint8_t opcode_offset, uint8_t opcode_suffix_offset)
{
    off_t rel = *(int32_t*)(&exe[offset + opcode_offset]);
    return offset + rel + opcode_offset + 4 + opcode_suffix_offset;
}

size_t decode_imm(const char* exe, size_t offset, uint8_t opcode_offset)
{
    return *(uint32_t*)(&exe[offset + opcode_offset]);
}

PIMAGE_NT_HEADERS RtlImageNtHeader(_In_ PVOID Base)
{
    static HMODULE ntdll_dll = GetModuleHandleA("ntdll.dll");
    static auto proc = (decltype(RtlImageNtHeader)*)GetProcAddress(ntdll_dll, "RtlImageNtHeader");
    return proc(Base);
}

size_t find_inst(const char* exe, std::string_view needle, size_t start, std::string_view pattern_name)
{
    static const std::size_t exe_size = [exe]()
    {
        if (PIMAGE_NT_HEADERS pinth = RtlImageNtHeader((PVOID)exe))
        {
            return (std::size_t)(pinth->OptionalHeader.BaseOfCode) + pinth->OptionalHeader.SizeOfCode;
        }
        return 0ull;
    }();

    const std::size_t needle_length = needle.size();

    for (std::size_t j = start; j < exe_size - needle_length; j++)
    {
        bool found = true;
        for (std::size_t k = 0; k < needle_length && found; k++)
        {
            found = needle[k] == '*' || needle[k] == *(exe + j + k);
        }

        if (found)
        {
            return j;
        }
    }

    std::string message;
    if (pattern_name.empty())
    {
        message = fmt::format("Failed finding pattern '{}' in Spel2.exe", ByteStr{needle});
    }
    else
    {
        message = fmt::format("Failed finding pattern '' ('{}') in Spel2.exe", pattern_name, ByteStr{needle});
    }

    if (MessageBox(NULL, message.c_str(), NULL, MB_OKCANCEL) == IDCANCEL)
    {
        std::terminate();
    }
    return SIZE_MAX;
}

size_t find_after_bundle(size_t exe)
{
    auto offset = 0x1000;

    while (true)
    {
        uint32_t* cur = (uint32_t*)(exe + offset);
        uint32_t l0 = cur[0], l1 = cur[1];
        if (l0 == 0 && l1 == 0)
        {
            break;
        }
        offset += (8 + l0 + l1);
    }

    using namespace std::string_view_literals;
    return find_inst((char*)exe, "\x55\x41\x57\x41\x56\x41\x55\x41\x54"sv, offset);
}

std::unordered_map<std::string_view, size_t (*)(Memory mem, const char* exe)> g_address_rules{
    {
        "entity_factory"sv,
        [](Memory mem, const char* exe)
        {
            size_t addr = find_inst(exe, "\x48\x83\xc6\x08\x41\x8b\x44\x24\x18"sv, mem.after_bundle, "entity_factory"sv);
            return mem.at_exe(decode_pc(exe, addr - 0xc));
        },
    },
    {
        "load_item"sv,
        [](Memory mem, const char* exe)
        {
            size_t addr = find_inst(exe, "\x83\x80\x44\x01\x00\x00\xFF"s, mem.after_bundle, "load_item"sv);
            return function_start(mem.at_exe(addr));
        },
    },
    {
        "get_virtual_function_address"sv,
        [](Memory mem, const char* exe)
        {
            // Rev.Eng.: Look at any entity in memory, dereference the __vftable to see the big table of pointers
            // scroll up to the first one, and find a reference to that
            size_t addr = find_inst(exe, "\x48\x8D\x0D\x03\x79\x51\x00"s, mem.after_bundle, "get_virtual_function_address"sv);
            return mem.at_exe(decode_pc(exe, addr));
        },
    },
};
std::unordered_map<std::string_view, size_t> g_cached_addresses;

void preload_addresses()
{
    Memory mem = Memory::get();
    const char* exe = mem.exe();
    for (auto [address_name, rule] : g_address_rules)
    {
        size_t address = rule(mem, exe);
        if (address > 0ull)
        {
            g_cached_addresses[address_name] = address;
        }
    }
}
size_t load_address(std::string_view address_name)
{
    auto it = g_address_rules.find(address_name);
    if (it != g_address_rules.end())
    {
        Memory mem = Memory::get();
        size_t address = it->second(mem, mem.exe());
        if (address > 0ull)
        {
            g_cached_addresses[address_name] = address;
            return address;
        }
    }
    return 0ull;
}
size_t get_address(std::string_view address_name)
{
    auto it = g_cached_addresses.find(address_name);
    if (it != g_cached_addresses.end())
    {
        return it->second;
    }
    return load_address(address_name);
}
