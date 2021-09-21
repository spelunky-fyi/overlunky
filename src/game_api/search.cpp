#include "search.hpp"

// clang-format off
#include <Windows.h>
#include <Psapi.h>
// clang-format on

#include <algorithm>

#include "logger.h"

// Decodes the program counter inside an instruction
// The default simple variant is 3 bytes instruction, 4 bytes rel. address, 0 bytes suffix:
//      e.g.  movups xmm0, ptr[XXXXXXXX] = 0F1005 XXXXXXXX
// Some instructions have 2 bytes instruction, so specify 2 for opcode_offset
//      e.g.  call ptr[XXXXXXXX] = FF15 XXXXXXXX
// Some (write) instructions have a value after the program counter to be extracted, so specify the opcode_suffix_offset
//      e.g.  mov word ptr[XXXXXXXX], 1 = 66:C705 XXXXXXXX 0100 (opcode_suffix_offset = 2)
size_t decode_pc(char* exe, size_t offset, uint8_t opcode_offset, uint8_t opcode_suffix_offset)
{
    off_t rel = *(int32_t*)(&exe[offset + opcode_offset]);
    return offset + rel + opcode_offset + 4 + opcode_suffix_offset;
}

size_t decode_imm(char* exe, size_t offset, uint8_t opcode_offset)
{
    return *(uint32_t*)(&exe[offset + opcode_offset]);
}

PIMAGE_NT_HEADERS RtlImageNtHeader(_In_ PVOID Base)
{
    static HMODULE ntdll_dll = GetModuleHandleA("ntdll.dll");
    static auto proc = (decltype(RtlImageNtHeader)*)GetProcAddress(ntdll_dll, "RtlImageNtHeader");
    return proc(Base);
}

size_t find_inst(char* exe, std::string_view needle, size_t start)
{
    static const std::size_t exe_size = [exe]()
    {
        if (PIMAGE_NT_HEADERS pinth = RtlImageNtHeader(exe))
        {
            return (std::size_t)(pinth->OptionalHeader.BaseOfCode + pinth->OptionalHeader.SizeOfCode);
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

    if (MessageBox(NULL, fmt::format("Failed finding pattern '{}' in Spel2.exe", ByteStr{needle}).c_str(), NULL, MB_OKCANCEL) == IDCANCEL)
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
