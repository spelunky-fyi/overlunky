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

    return find_inst((char*)exe, "\x55\x41\x57\x41\x56\x41\x55\x41\x54"sv, offset);
}

class PatternCommandBuffer
{
  public:
    PatternCommandBuffer() = default;
    PatternCommandBuffer(const PatternCommandBuffer&) = default;
    PatternCommandBuffer(PatternCommandBuffer&&) noexcept = default;
    PatternCommandBuffer& operator=(const PatternCommandBuffer&) = default;
    PatternCommandBuffer& operator=(PatternCommandBuffer&&) noexcept = default;

    PatternCommandBuffer& find_inst(std::string_view pattern)
    {
        commands.push_back({CommandType::FindInst, {.pattern = pattern}});
        return *this;
    }
    PatternCommandBuffer& offset(int64_t offset)
    {
        commands.push_back({CommandType::Offset, {.offset = offset}});
        return *this;
    }
    PatternCommandBuffer& decode_pc(uint8_t opcode_prefix = 3, uint8_t opcode_suffix = 0)
    {
        commands.push_back({CommandType::DecodePC, {.decode_pc_prefix_suffix = {opcode_prefix, opcode_suffix}}});
        return *this;
    }
    PatternCommandBuffer& decode_imm(uint8_t opcode_prefix = 3)
    {
        commands.push_back({CommandType::DecodeIMM, {.decode_imm_prefix = opcode_prefix}});
        return *this;
    }
    PatternCommandBuffer& decode_call()
    {
        commands.push_back({CommandType::DecodeCall});
        return *this;
    }
    PatternCommandBuffer& at_exe()
    {
        commands.push_back({CommandType::AtExe});
        return *this;
    }
    PatternCommandBuffer& function_start()
    {
        commands.push_back({CommandType::FunctionStart});
        return *this;
    }

    size_t operator()(Memory mem, const char* exe, std::string_view address_name) const
    {
        size_t offset = mem.after_bundle;

        for (auto& [command, data] : commands)
        {
            switch (command)
            {
            case CommandType::FindInst:
                offset = ::find_inst(exe, data.pattern, offset, address_name);
                break;
            case CommandType::Offset:
                offset = offset + data.offset;
                break;
            case CommandType::DecodePC:
                offset = ::decode_pc(exe, offset, data.decode_pc_prefix_suffix.first, data.decode_pc_prefix_suffix.second);
                break;
            case CommandType::DecodeIMM:
                offset = ::decode_imm(exe, offset, data.decode_imm_prefix);
                break;
            case CommandType::DecodeCall:
                offset = mem.decode_call(offset);
                break;
            case CommandType::AtExe:
                offset = mem.at_exe(offset);
                break;
            case CommandType::FunctionStart:
                offset = ::function_start(offset);
                break;
            default:
                DEBUG("Unkown command...");
                break;
            }
        }

        return offset;
    }

  private:
    enum class CommandType
    {
        FindInst,
        Offset,
        DecodePC,
        DecodeIMM,
        DecodeCall,
        AtExe,
        FunctionStart,
    };
    union CommandData
    {
        std::string_view pattern;
        int64_t offset;
        std::pair<uint8_t, uint8_t> decode_pc_prefix_suffix;
        uint8_t decode_imm_prefix;
    };
    struct Command
    {
        CommandType command;
        CommandData data;
    };
    std::vector<Command> commands;
};

std::unordered_map<std::string_view, std::function<size_t(Memory mem, const char* exe, std::string_view address_name)>> g_address_rules{
    {
        "entity_factory"sv,
        PatternCommandBuffer{}
            .find_inst("\x48\x83\xc6\x08\x41\x8b\x44\x24\x18"sv)
            .offset(-0xc)
            .decode_pc()
            .at_exe(),
    },
    {
        "load_item"sv,
        PatternCommandBuffer{}
            .find_inst("\x83\x80\x44\x01\x00\x00\xFF"sv)
            .at_exe()
            .function_start(),
    },
    {
        "get_virtual_function_address"sv,
        // Rev.Eng.: Look at any entity in memory, dereference the __vftable to see the big table of pointers
        // scroll up to the first one, and find a reference to that
        PatternCommandBuffer{}
            .find_inst("\x48\x8D\x0D\x03\x79\x51\x00"sv)
            .decode_pc()
            .at_exe(),
    },
    {
        "fmod_studio"sv,
        // Rev.Eng.: Break at startup on FMOD::Studio::System::initialize, the first parameter passed is the system-pointer-pointer
        PatternCommandBuffer{}
            .find_inst("\xBA\x05\x01\x02\x00"sv)
            .offset(-0x7)
            .decode_pc()
            .at_exe(),
    },
    {
        "fmod_event_properties"sv,
        // Rev.Eng.: Find a call to FMOD::Studio::EventDescription::getParameterDescriptionByName, the second parameter is the name of the event
        // Said name comes from an array that is being looped, said array is a global of type EventParameters
        PatternCommandBuffer{}
            .find_inst("\x48\x8d\x9d\x30\x01\x00\x00"sv)
            .offset(-0x7)
            .decode_pc()
            .offset(0x30)
            .at_exe(),
    },
    {
        "fmod_event_map"sv,
        // Rev.Eng.: Find a call to FMOD::Studio::System::getEvent (should be before the call to FMOD::Studio::EventDescription::getParameterDescriptionByName)
        // The third parameter is an event-pointer-pointer, the second parameter to the enclosing function is the event-id and will be used further down
        // to emplace a struct in an unordered_map (as seen by the strings inside the emplace function), that unordered_map is a global of type EventMap
        PatternCommandBuffer{}
            .find_inst("\x89\x58\x10\x48\x8d\x48\x18\x41\xb8\x88\x01\x00\x00"sv)
            .offset(0x40)
            .find_inst("\xf3"sv)
            .decode_pc(4)
            .at_exe(),
    },
    {
        "level_gen_load_level_file"sv,
        // Rev.Eng.: Search for string "generic.lvl", it is used in a call to this function
        PatternCommandBuffer{}
            .find_inst("\x45\x84\xed\x74\x0f"sv)
            .offset(0x1)
            .find_inst("\x45\x84\xed\x74\x0f"sv)
            .find_inst("\xe8"sv)
            .decode_call()
            .at_exe(),
    },
};
std::unordered_map<std::string_view, size_t> g_cached_addresses;

void preload_addresses()
{
    Memory mem = Memory::get();
    const char* exe = mem.exe();
    for (auto [address_name, rule] : g_address_rules)
    {
        size_t address = rule(mem, exe, address_name);
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
        size_t address = it->second(mem, mem.exe(), address_name);
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
