#include "search.hpp"

// clang-format off
#include <Windows.h>
#include <Psapi.h>
// clang-format on

#include <algorithm>

#include "memory.hpp"
#include "virtual_table.hpp"

// Decodes the program counter inside an instruction
// The default simple variant is 3 bytes instruction, 4 bytes rel. address, 0 bytes suffix:
//      e.g.  movups xmm0, ptr[XXXXXXXX] = 0F1005 XXXXXXXX
// Some instructions have 2 bytes instruction, so specify 2 for opcode_offset
//      e.g.  call ptr[XXXXXXXX] = FF15 XXXXXXXX
// Some (write) instructions have a value after the program counter to be extracted, so specify the opcode_suffix_offset
//      e.g.  mov word ptr[XXXXXXXX], 1 = 66:C705 XXXXXXXX 0100 (opcode_suffix_offset = 2)
size_t decode_pc(const char* exe, size_t offset, uint8_t opcode_offset, uint8_t opcode_suffix_offset, uint8_t opcode_addr_size)
{
    off_t rel;
    switch (opcode_addr_size)
    {
    case 1:
        rel = *(int8_t*)(&exe[offset + opcode_offset]);
        break;
    case 2:
        rel = *(int16_t*)(&exe[offset + opcode_offset]);
        break;
    case 4:
    default:
        rel = *(int32_t*)(&exe[offset + opcode_offset]);
        break;
    }
    return offset + rel + opcode_offset + opcode_addr_size + opcode_suffix_offset;
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

const char* current_spelunky_version()
{
    static const char* version = "unknown!";
    static bool version_searched = false;
    if (!version_searched)
    {
        version_searched = true;
        auto memory = Memory::get();
        PIMAGE_NT_HEADERS nt_header = RtlImageNtHeader((PVOID)memory.exe());
        size_t rdata_start = 0;
        size_t rdata_size = 0;
        IMAGE_SECTION_HEADER* section_header = (IMAGE_SECTION_HEADER*)(nt_header + 1);
        for (int i = 0; i < nt_header->FileHeader.NumberOfSections; i++)
        {
            char* name = (char*)section_header->Name;
            if (memcmp(name, ".rdata", 6) == 0)
            {
                rdata_start = (size_t)(memory.exe() + section_header->VirtualAddress);
                rdata_size = section_header->Misc.VirtualSize;
                break;
            }
            section_header++;
        }
        if (rdata_start > 0 && rdata_size > 0)
        {
            std::string_view needle = "\x31\x2E**\x2E**\x00"sv;
            const size_t needle_length = needle.size();
            const char* rdata = (const char*)rdata_start;
            size_t offset = 0;
            for (size_t j = 0; j < rdata_size - needle_length; j++)
            {
                bool found = true;
                for (size_t k = 0; k < needle_length && found; k++)
                {
                    found = needle[k] == '*' || needle[k] == *(rdata + j + k);
                }
                if (found)
                {
                    offset = rdata_start + j;
                    break;
                }
            }
            if (offset != 0)
            {
                version = static_cast<const char*>((void*)offset);
            }
        }
    }
    return version;
}

static std::vector<std::string> g_registered_applications = {};
void register_application_version(std::string s)
{
    g_registered_applications.emplace_back(std::move(s));
}

std::string application_versions()
{
    if (g_registered_applications.empty())
    {
        return "No application versions registered";
    }
    std::stringstream ss;
    for (const auto& s : g_registered_applications)
    {
        ss << s << "\n";
    }
    return ss.str();
}

std::string get_error_information()
{
    return fmt::format("\n\nRunning Spelunky 2: {}\nSupported Spelunky 2: 1.25.0b\n\n{}", current_spelunky_version(), application_versions());
}

size_t find_inst(const char* exe, std::string_view needle, size_t start, std::optional<size_t> end, std::string_view pattern_name, bool is_required)
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
    const std::size_t search_end = end.value_or(exe_size);

    for (std::size_t j = start; j < search_end - needle_length; j++)
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

    std::string error_message;
    if (pattern_name.empty())
    {
        error_message = fmt::format("Failed finding pattern '{}' in Spel2.exe{}", ByteStr{needle}, get_error_information());
    }
    else
    {
        error_message = fmt::format("Failed finding pattern '{}' ('{}') in Spel2.exe{}", pattern_name, ByteStr{needle}, get_error_information());
    }

    if (is_required)
    {
        if (MessageBox(NULL, error_message.c_str(), NULL, MB_OKCANCEL) == IDCANCEL)
        {
            std::terminate();
        }
        return 0ull;
    }
    else
    {
        throw std::logic_error{error_message};
    }
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

    PatternCommandBuffer& set_optional(bool optional)
    {
        commands.push_back({CommandType::SetOptional, {.optional = optional}});
        return *this;
    }
    PatternCommandBuffer& get_address(std::string_view address_name)
    {
        commands.push_back({CommandType::GetAddress, {.address_name = address_name}});
        return *this;
    }
    PatternCommandBuffer& get_virtual_function_address(VTABLE_OFFSET table_offset, VIRT_FUNC function_index)
    {
        commands.push_back({CommandType::GetVirtualFunctionAddress, {.get_vfunc_addr_args = {.table_offset = table_offset, .function_index = function_index}}});
        return *this;
    }
    PatternCommandBuffer& find_inst(std::string_view pattern)
    {
        commands.push_back({CommandType::FindInst, {.find_inst_args = {pattern}}});
        return *this;
    }
    PatternCommandBuffer& find_after_inst(std::string_view pattern)
    {
        return find_inst(pattern).offset(pattern.size());
    }
    PatternCommandBuffer& find_next_inst(std::string_view pattern)
    {
        return offset(0x1).find_inst(pattern);
    }
    PatternCommandBuffer& find_inst_in_range(std::string_view pattern, size_t range)
    {
        commands.push_back({CommandType::FindInst, {.find_inst_args = {.pattern = pattern, .range = range}}});
        return *this;
    }
    PatternCommandBuffer& find_after_inst_in_range(std::string_view pattern, size_t range)
    {
        return find_inst_in_range(pattern, range).offset(pattern.size());
    }
    PatternCommandBuffer& find_next_inst_in_range(std::string_view pattern, size_t range)
    {
        return offset(0x1).find_inst_in_range(pattern, range);
    }
    PatternCommandBuffer& offset(int64_t offset)
    {
        commands.push_back({CommandType::Offset, {.offset = offset}});
        return *this;
    }
    PatternCommandBuffer& decode_pc(uint8_t opcode_prefix = 3, uint8_t opcode_suffix = 0, uint8_t opcode_addr = 4)
    {
        commands.push_back({CommandType::DecodePC, {.decode_pc_args = {opcode_prefix, opcode_suffix, opcode_addr}}});
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

    // Rapid prototyping only please
    PatternCommandBuffer& from_exe_base(uint64_t offset)
    {
        commands.push_back({CommandType::FromExeBase, {.base_offset = offset}});
        commands.push_back({CommandType::AtExe});
        return *this;
    }

    std::optional<size_t> operator()(Memory mem, const char* exe, std::string_view address_name) const
    {
        size_t offset = mem.after_bundle;
        bool optional{false};

#ifdef DEBUG
        static constexpr auto debug_pattern = ""sv;
        if constexpr (!debug_pattern.empty())
        {
            if (address_name == debug_pattern)
            {
                __debugbreak();
            }
        }
#endif // DEBUG

        for (auto& [command, data] : commands)
        {
            switch (command)
            {
            case CommandType::SetOptional:
                optional = data.optional;
                break;
            case CommandType::GetAddress:
                offset = ::get_address(data.address_name);
                if (optional && offset == 0)
                {
                    return 0;
                }
                else
                {
                    offset -= (size_t)exe;
                }
                break;
            case CommandType::GetVirtualFunctionAddress:
                offset = ::get_virtual_function_address(data.get_vfunc_addr_args.table_offset, static_cast<uint32_t>(data.get_vfunc_addr_args.function_index));
                break;
            case CommandType::FindInst:
                try
                {
                    if (data.find_inst_args.range.has_value())
                    {
                        offset = ::find_inst(exe, data.find_inst_args.pattern, offset, offset + data.find_inst_args.range.value(), address_name, !optional);
                    }
                    else
                    {
                        offset = ::find_inst(exe, data.find_inst_args.pattern, offset, std::nullopt, address_name, !optional);
                    }
                }
                catch (const std::logic_error&)
                {
                    return 0;
                }
                break;
            case CommandType::Offset:
                offset = offset + data.offset;
                break;
            case CommandType::DecodePC:
                offset = std::apply([=](auto... args)
                                    { return ::decode_pc(exe, offset, args...); },
                                    data.decode_pc_args.as_tuple());
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
            case CommandType::FromExeBase:
                offset = data.base_offset;
                break;
            default:
                DEBUG("Unkown command...");
                break;
            }
        }

        return offset;
    }

  private:
    struct DecodePcArgs
    {
        uint8_t opcode_offset;
        uint8_t opcode_suffix_offset;
        uint8_t opcode_addr_size;

        std::tuple<uint8_t, uint8_t, uint8_t> as_tuple() const
        {
            return {opcode_offset, opcode_suffix_offset, opcode_addr_size};
        }
    };
    struct FindInstArgs
    {
        std::string_view pattern;
        std::optional<size_t> range;
    };
    struct GetVirtualFunctionAddressArgs
    {
        VTABLE_OFFSET table_offset;
        VIRT_FUNC function_index;
    };

    enum class CommandType
    {
        SetOptional,
        GetAddress,
        GetVirtualFunctionAddress,
        FindInst,
        Offset,
        DecodePC,
        DecodeIMM,
        DecodeCall,
        AtExe,
        FunctionStart,
        FromExeBase,
    };
    union CommandData
    {
        bool optional;
        std::string_view address_name;
        FindInstArgs find_inst_args;
        int64_t offset;
        DecodePcArgs decode_pc_args;
        uint8_t decode_imm_prefix;
        GetVirtualFunctionAddressArgs get_vfunc_addr_args;
        uint64_t base_offset;
    };
    struct Command
    {
        CommandType command;
        CommandData data;
    };
    std::vector<Command> commands;
};

using AddressRule = std::function<std::optional<size_t>(Memory mem, const char* exe, std::string_view address_name)>;
std::unordered_map<std::string_view, AddressRule> g_address_rules{
    {
        "game_malloc"sv,
        PatternCommandBuffer{}
            .find_inst("\x45\x84\xe4\x0f\x84"sv)
            .offset(-0x10)
            .find_inst("\xff\x15"sv)
            .decode_pc(2)
            .at_exe(),
    },
    {
        "game_free"sv,
        PatternCommandBuffer{}
            .find_inst("\x48\x83\x7e\x18\x00"sv)
            .offset(-0x10)
            .find_inst("\xff\x15"sv)
            .decode_pc(2)
            .at_exe(),
    },
    {
        "read_encrypted_file"sv,
        PatternCommandBuffer{}
            .find_inst("\x41\xb8\x50\x46\x00\x00"sv)
            .find_inst("\xe8"sv)
            .find_next_inst("\xe8"sv)
            .decode_call()
            .at_exe(),
    },
    {
        "state_location"sv,
        PatternCommandBuffer{}
            .find_inst("\x49\x0F\x44\xC0"sv)
            .find_next_inst("\x49\x0F\x44\xC0"sv)
            .offset(-0x19)
            .find_inst("\x48\x8B"sv)
            .decode_pc()
            .at_exe(),
    },
    {
        "game_manager"sv,
        PatternCommandBuffer{}
            .find_inst("\xC6\x80\x39\x01\x00\x00\x00\x48"sv)
            .offset(0x7)
            .decode_pc()
            .at_exe(),
    },
    {
        "destroy_game_manager"sv,
        // Called soon after `DispatchMessageA` if `message == 0x12`
        PatternCommandBuffer{}
            .find_after_inst("\x48\x8b\x8d\xe8\x10\x00\x00\x48\x8b\x71\x08"sv)
            .find_inst("\xe8"sv)
            .decode_call()
            .at_exe(),
    },
    {
        "write_load_opt"sv,
        PatternCommandBuffer{}
            .find_after_inst("\x41\xB8\x50\x00\x00\x00\x45\x31\xC9"sv)
            .at_exe(),
    },
    {
        "camera_position"sv,
        // Go stand still in a level. In Cheat Engine, do a few incremental searches for the x-position
        // (as hex value, look at state.camera in the plugin). There will be several matches: 16 times in
        // 100 bytes unknown structs, 2 referencing state.camera.adjusted and calculated position
        // and the final one we want. Put a write bp on that address.
        PatternCommandBuffer{}
            .find_inst("\xF3\x0F\x11\x05****\xF3\x0F\x10\x42\x14"sv)
            .decode_pc(4)
            .at_exe(),
    },
    {
        "render_api_callback"sv,
        // Break at startup on SteamAPI_RegisterCallback, it gets called twice, second time
        // to hook the Steam overlay, at the beginning of that function is the pointer we need
        PatternCommandBuffer{}
            .find_inst("\x70\x08\x00\x00\xFE\xFF\xFF\xFF\x48\x8B\x05"sv)
            .offset(0x8)
            .decode_pc()
            .at_exe(),
    },
    {
        "render_api_offset"sv,
        PatternCommandBuffer{}
            .find_inst("\xBA\xF0\xFF\xFF\xFF\x41\xB8\x00\x00\x00\x90"sv)
            .offset(0x11)
            .decode_imm(),
    },
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
        "add_to_layer"sv,
        // Used in load_item as `add_to_layer(layer, spawned_entity)`
        PatternCommandBuffer{}
            .find_inst("\x48\x83\xC1\x08\x45\x31\xC0\xE8"sv)
            .at_exe()
            .function_start(),
    },
    {
        "remove_from_layer"sv,
        // Set a data-bp on player.layer, then go through a layer door
        // Should hit the bp where it runs player.layer = 2, that is this function
        PatternCommandBuffer{}
            .find_inst("\x48\x8b\x4d\xe0\x48\x89\xf2\xe8****\x48"sv)
            .offset(0x7)
            .decode_call()
            .at_exe(),
    },
    {
        "spawn_entity"sv,
        // First call in `load_item` is to this function
        PatternCommandBuffer{}
            .find_inst("\x44\x88\xb8\xa0\x00\x00\x00\xf3\x0f\x11\x78\x40"sv)
            .at_exe()
            .function_start(),
    },
    {
        "setup_lake_impostor"sv,
        // After a call to `load_item` that spawns `0x38f` or `0x392` the spawned entity is passed to this function
        PatternCommandBuffer{}
            .find_inst("\x0f\x28\xd6\xe8****\x48\x8b\x86\x20"sv)
            .find_next_inst("\x0f\x28\xd6\xe8****\x48\x8b\x86\x20"sv)
            .offset(0x3)
            .decode_call()
            .at_exe(),
    },
    {
        "add_item_ptr"sv,
        // Used in spawn_entity as `add_item_ptr(overlay + 0x18, spawned_entity, false)`
        PatternCommandBuffer{}
            .find_inst("\xe8****\x44\x88\x76"sv)
            .decode_call()
            .at_exe(),
    },
    {
        "spawn_liquid"sv,
        // See tile code for water (0xea for 1.23.3) in handle_tile_code, last call before returning
        PatternCommandBuffer{}
            .find_inst("\xE8****\xE9****\x48\x81\xC6"sv)
            .decode_call()
            .at_exe(),
    },
    {
        "virtual_functions_table"sv,
        // Look at any entity in memory, dereference the __vftable to see the big table of pointers
        // scroll up to the first one, and find a reference to that
        PatternCommandBuffer{}
            .find_inst("\x48\x8D\x0D****\x48\x89\x0D****\x48\xC7\x05"sv)
            .decode_pc()
            .at_exe(),
    },
    {
        "fmod_studio"sv,
        // Break at startup on FMOD::Studio::System::initialize, the first parameter passed is the system-pointer-pointer
        PatternCommandBuffer{}
            .set_optional(true)
            .find_inst("\xba\x03\x02\x02\x00"sv)
            .offset(-0x7)
            .decode_pc()
            .at_exe(),
    },
    {
        "fmod_event_properties"sv,
        // Find a call to FMOD::Studio::EventDescription::getParameterDescriptionByName, the second parameter is the name of the event
        // Said name comes from an array that is being looped, said array is a global of type EventParameters
        PatternCommandBuffer{}
            .set_optional(true)
            .find_inst("\x48\x8d\x9d\x38\x01\x00\x00"sv)
            .offset(-0x7)
            .decode_pc()
            .offset(0x30)
            .at_exe(),
    },
    {
        "fmod_event_map"sv,
        // Find a call to FMOD::Studio::System::getEvent (should be before the call to FMOD::Studio::EventDescription::getParameterDescriptionByName)
        // The third parameter is an event-pointer-pointer, the second parameter to the enclosing function is the event-id and will be used further down
        // to emplace a struct in an unordered_map (as seen by the strings inside the emplace function), that unordered_map is a global of type EventMap
        PatternCommandBuffer{}
            .set_optional(true)
            .find_after_inst("\x48\x89\xf8\x48\xd1\xe8\x83\xe7\x01\x48\x09\xc7\xf3\x48\x0f\x2a\xc7"sv)
            .find_next_inst("\xf3"sv)
            .decode_pc(4)
            .at_exe(),
    },
    {
        "level_gen_entry"sv,
        // Put a bp on the virtual LevelInfo::spawn_level, start a new game, the caller is this function
        PatternCommandBuffer{}
            .find_inst("\xE8****\x41\x80\x7F**\x7C\x22"sv)
            .decode_call()
            .at_exe(),
    },
    {
        "level_gen_handle_tile_code"sv,
        // Put a conditional bp on spawn_entity with entity_type == to_id("ENT_TYPE_FLOOR_GENERIC")
        // The callstack should be handle_tile_code -> load_item -> spawn_entity
        PatternCommandBuffer{}
            .find_inst("\xE8****\x83\xC5\x01"sv)
            .decode_call()
            .at_exe(),
    },
    {
        "level_gen_setup_level_files"sv,
        // Search for string "ending.lvl", it is used in a call to this function
        PatternCommandBuffer{}
            .find_inst("\xE8****\x49\x8B\xB7****\x48\x8D\x4E\x48"sv)
            .decode_call()
            .at_exe(),
    },
    {
        "level_gen_load_level_file"sv,
        // Search for string "generic.lvl", it is used in a call to this function
        PatternCommandBuffer{}
            .find_inst("\x45\x84\xed\x74\x0f"sv)
            .find_next_inst("\x45\x84\xed\x74\x0f"sv)
            .find_inst("\xe8"sv)
            .decode_call()
            .at_exe(),
    },
    {
        "get_room_size_begin"sv,
        // Right after the big switch check for the first char to be `\\`
        PatternCommandBuffer{}
            .set_optional(true)
            .get_address("level_gen_load_level_file"sv)
            .find_inst_in_range("\x44\x8b\xbd\xe4\x05\x00\x00"sv, 0xa00)
            .at_exe(),
    },
    {
        "get_room_size_first_jump"sv,
        // First jump JMP after get_room_size_begin
        PatternCommandBuffer{}
            .set_optional(true)
            .get_address("get_room_size_begin"sv)
            .find_next_inst_in_range("\x74"sv, 0x20)
            .decode_pc(1, 0, 1)
            .at_exe(),
    },
    {
        "get_room_size_second_jump"sv,
        // Second jump JMP after get_room_size_begin
        PatternCommandBuffer{}
            .set_optional(true)
            .get_address("get_room_size_begin"sv)
            .find_next_inst_in_range("\x74"sv, 0x20)
            .find_next_inst_in_range("\xeb"sv, 0x20)
            .decode_pc(1, 0, 1)
            .at_exe(),
    },
    {
        "get_room_size_end"sv,
        // Right after the second jump
        PatternCommandBuffer{}
            .set_optional(true)
            .get_address("get_room_size_begin"sv)
            .find_next_inst_in_range("\x74"sv, 0x20)
            .find_after_inst_in_range("\xeb*"sv, 0x20)
            .at_exe(),
    },
    {
        "level_gen_do_extra_spawns"sv,
        // Put a conditional bp in `spawn_entity` on `entity_type == to_id("ENT_TYPE_ITEM_LOCKEDCHEST_KEY")`
        // Callstack should be `do_extra_spawns` -> `ThemeInfo::virtual_50` -> `load_item` -> `spawn_entity`
        // Note that there is no `0xcc` padding before this function so we can't use `function_start`, at
        // least for 1.25.0b and maybe later
        PatternCommandBuffer{}
            .find_inst("\xff\x90\x90\x01\x00\x00\x8b\x05****\x65"sv)
            .offset(-0xcc)
            .find_inst("\x41\x57\x41\x56\x41\x55\x41\x54"sv)
            .at_exe(),
    },
    {
        "level_gen_generate_room"sv,
        // One call up from generate_room_from_tile_codes
        PatternCommandBuffer{}
            .find_inst("\xE8****\x83\xC6\x01\x39\xF3"sv)
            .decode_call()
            .at_exe(),
    },
    {
        "level_gen_gather_room_data"sv,
        // First call in generate_room, it gets something from the unordered_map in `param_1 + 0x108`
        PatternCommandBuffer{}
            .find_inst("\xE8****\x44\x8A\x44\x24*\x45\x84\xC0"sv)
            .decode_call()
            .at_exe(),
    },
    {
        "level_gen_get_random_room_data"sv,
        // Call to this is the only thing happening in a loop along with checking a flag on the returned value
        PatternCommandBuffer{}
            .find_inst("\xE8****\x48\x8B\x58\x08"sv)
            .decode_call()
            .at_exe(),
    },
    {
        "level_gen_spawn_room_from_tile_codes"sv,
        // One of the few calls to handle_tile_code, does a `if (param != 0xec)` before the call
        PatternCommandBuffer{}
            .find_inst("\xE8****\x45\x89\xF8\x83\xC7\x01"sv)
            .decode_call()
            .at_exe(),
    },
    {
        "level_gen_test_spawn_chance"sv,
        // Called in the last virtual on ThemeInfo to determine whether a load_item should be done
        PatternCommandBuffer{}
            .find_inst("\xE8****\x84\xC0\x48\x8B\x6C\x24"sv)
            .decode_call()
            .at_exe(),
    },
    {
        "level_gen_emplace_chance"sv,
        // Called during init with a string of all the different monster and trap chances, e.g. "pushblock_chance"
        PatternCommandBuffer{}
            .find_inst("\x4c\x8d\x45\xe0\xe8****\x48\x8b\x45\xc8"sv)
            .offset(0x4)
            .decode_call()
            .at_exe(),
    },
    {
        "level_gen_emplace_level_chance"sv,
        // Called in load_level_file on LevelGenData::level_trap_chances and LevelGenData::level_trap_chances
        PatternCommandBuffer{}
            .find_inst("\x41\x89\xd8\xe8****\x48\x8b\x44\x24\x20"sv)
            .offset(0x3)
            .decode_call()
            .at_exe(),
    },
    {
        "online"sv,
        // Find online code in memory (reverse for endianness), look higher up and find __vftable, set read bp on __vftable
        PatternCommandBuffer{}
            .find_inst("\x66\x0F\x29\x85\xE0\x03\x00\x00"sv)
            .offset(0x8)
            .decode_pc()
            .at_exe(),
    },
    {
        "particle_emitter_db"sv,
        // Locate the particles init function (see pattern `particle_emitter_list`). At the start you will see the value 1
        // being written to a memory address. This is the start of the particle DB. Note that this function builds up the entire
        // DB so you have to step into it quite a bit to see the particle DB start forming in memory.
        // The size of a particle emitter is 0xA0
        PatternCommandBuffer{}
            .find_inst("\xFE\xFF\xFF\xFF\x66\xC7\x05"sv)
            .offset(0x4)
            .decode_pc(3, 2)
            .at_exe(),
    },
    {
        "particle_emitter_list"sv,
        // Find string reference PARTICLEEMITTER_TITLE_TORCHFLAME_SMOKE, used in particles init function, it processes all particle emitters
        // and inserts all into the unordered_map. Look at function call(s) shortly after string reference (insertion in map), it will contain a
        // reference to the start of the map, or the `size` which it increments. The unordered map insertion function can be recognized by a little
        // stub with some jumps to `Xlength_error` at the end.
        PatternCommandBuffer{}
            .find_inst("\xF3\x48\x0F\x2A\xC0\xF3\x0F\x58\xC0\xF3\x0F\x10\x0D"sv)
            .offset(0x9)
            .decode_pc(4)
            .at_exe(),
    },
    {
        "fetch_texture_begin"sv,
        // In spawn_entity right after the texture_id is assigned to a local (probably eax)
        // and then checked against -4
        PatternCommandBuffer{}
            .set_optional(true)
            .get_address("spawn_entity"sv)
            .find_inst_in_range("\x83\xf8\xfc"sv, 0x250)
            .at_exe(),
    },
    {
        "fetch_texture_end"sv,
        // In spawn_entity before assigning Entity::animation_frame (0x3c)
        PatternCommandBuffer{}
            .set_optional(true)
            .get_address("fetch_texture_begin"sv)
            .find_next_inst_in_range("\x66\x89\x46\x3c"sv, 0x250)
            .at_exe(),
    },
    {
        "declare_texture"sv,
        // Search for a string of any of the textures, e.g. just `.DDS`, take one with just one XREF and see the call that it is used in,
        // that call is load_texture
        PatternCommandBuffer{}
            .find_inst("\xe8****\xc7\x44\x24\x50"sv)
            .find_next_inst("\xe8****\xc7\x44\x24\x50"sv)
            .find_next_inst("\xe8****\xc7\x44\x24\x50"sv)
            .decode_call()
            .at_exe(),
    },
    {
        "render_layer"sv,
        // Put a bp into LevelInfo::get_backlayer_lut, step out to the caller
        // The next function that is called is this function
        PatternCommandBuffer{}
            .find_after_inst("\x49\x8b\x8f\x38\x13\x00\x00\x4d\x8b\x87\xa8\x13\x00\x00"sv)
            .find_inst("\xe8"sv)
            .decode_call()
            .at_exe(),
    },
    {
        "render_hud"sv,
        // Locate in memory 2B 00 24 00 25 00 64 00 "+$%d" (UTF16 str of the money gained text)
        // Find reference to this memory, it's only used in the HUD
        PatternCommandBuffer{}
            .find_inst("\xB8\x88\x14\x00\x00\xE8"sv)
            .at_exe()
            .function_start(),
    },
    {
        "prepare_text_for_rendering"sv,
        // Use `render_hud` to find the big function that renders the HUD. After every string preparation you will see two calls very
        // close to each other. The first is to prepare the text for rendering/calculate text dimensions/...
        // The second is to actually draw on screen
        PatternCommandBuffer{}
            .find_after_inst("\xB9\x0F\x00\x00\x00\x41\xB8\x02\x00\x00\x00\xE8"sv)
            .offset(-0x1)
            .decode_call()
            .at_exe(),
    },
    {
        "draw_text"sv,
        // See `prepare_text_for_rendering`
        PatternCommandBuffer{}
            .find_inst("\x41\x8B\x8F\xD0\x02\x00\x00\x81\xF9\xFF\x50\x25\x02"sv)
            .offset(-0x5)
            .decode_call()
            .at_exe(),
    },
    {
        "render_pause_menu"sv,
        // Put write bp on GameManager.pause_ui.scroll.y
        PatternCommandBuffer{}
            .find_inst("\xFE\xFF\xFF\xFF\x83\xB9"sv)
            .at_exe()
            .function_start(),
    },
    {
        "render_draw_depth"sv,
        // Break on draw_world_texture and go a couple functions higher in the call stack (4-5) until
        // you reach one that has rdx counting down from 0x35 to 0x01
        PatternCommandBuffer{}
            .find_inst("\x48\x81\xC6\x18\x3F\x06\x00"sv)
            .at_exe()
            .function_start(),
    },
    {
        "draw_screen_texture"sv,
        // Locate the function in `render_hud`. Towards the bottom you will find calls following a big stack buildup
        // and r8 will have the char* of the texture
        PatternCommandBuffer{}
            .find_inst("\x4C\x8D\x8D\xD8\x12\x00\x00\xB2\x29"sv)
            .offset(0x9)
            .decode_call()
            .at_exe(),
    },
    {
        "draw_world_texture"sv,
        // Look for cvttss2si instruction at the end of the exe (F3:0F2CC0)
        // Function has distinct look, one call at the top, for the rest a couple cvttss2si and
        // moving memory around at high offsets compared to register: [register+80XXX]
        PatternCommandBuffer{}
            .find_inst("\xC7\x44\x24\x28\x06\x00\x00\x00\xC7\x44\x24\x20\x04\x00\x00\x00"sv)
            .at_exe()
            .function_start(),
    },
    {
        "draw_world_texture_param_7"sv,
        // Look at the call site of `draw_world_texture`, there will be two hardcoded values loaded
        // One is the renderer, the other is the seventh param we need to pass to draw_world_texture
        PatternCommandBuffer{}
            .find_inst("\x48\x8D\x1D****\x48\x89\x5C\x24\x30"sv)
            .decode_pc()
            .at_exe(),
    },
    {
        "texture_db"sv,
        // Look up string reference to "Data/Textures/", at the beginning of this function
        // there will be a pointer to the start of TextureDB
        PatternCommandBuffer{}
            .find_inst("\x4C\x89\xC6\x41\x89\xCF\x8B\x1D"sv)
            .offset(0x6)
            .decode_pc(2)
            .at_exe(),
    },
    {
        "zoom_level"sv,
        // Go stand in a level next to a shop. In Cheat Engine, search for float 13.5
        // Go into the shop, search for 12.5, put a write bp on that address.
        // In 1.23.3 the default shop and level zoom levels aren't hardcoded in the exe, they are
        // in variables (loaded into xmm6)
        // Note that xmm6 (the new zoom level) gets written at a huge offset of rax. Rax is the
        // return value of the call just above, so look in that function at the bottom. There will
        // be a hardcoded value loaded in rax. At offset 0x10 in rax is another pointer that is the
        // base for the big offset.
        PatternCommandBuffer{}
            .find_inst("\x48\x8B\x05****\x48\x81\xC4\xF8\x08\x00\x00"sv)
            .decode_pc()
            .at_exe(),
    },
    {
        "zoom_level_offset"sv,
        // Follow the same logic as in `zoom_level` to get to the point where the zoom level is written.
        // That instruction contains the offset
        PatternCommandBuffer{}
            .find_inst("\xF3\x0F\x11\xB0****\x49"sv)
            .decode_imm(4),
    },
    {
        "default_zoom_level"sv,
        // Follow the same logic as in `zoom_level` to get to the point where the zoom level is written.
        // Above this instruction there are two memory locations being written into xmm6
        // The second is the default zoom level
        PatternCommandBuffer{}
            .find_inst("\xF3\x0F\x11\xB0****\x49"sv) // same pattern as zoom_level_offset
            .offset(-0x11)
            .decode_pc(4)
            .at_exe(),
    },
    {
        "default_zoom_level_shop"sv,
        // Follow the same logic as in `zoom_level` to get to the point where the zoom level is written.
        // Above this instruction there are two memory locations being written into xmm6
        // The first is the default shop zoom level
        PatternCommandBuffer{}
            .find_inst("\xF3\x0F\x11\xB0****\x49"sv) // same pattern as zoom_level_offset
            .offset(-0x1B)
            .decode_pc(4)
            .at_exe(),
    },
    {
        "default_zoom_level_telescope"sv,
        // Follow the same logic as in `zoom_level` to get to the point where the zoom level is written.
        // Put a write bp on this float with the condition not to break at the RIP where shop/in-game level is written.
        // Then look through the camp telescope, then stop looking
        PatternCommandBuffer{}
            .find_inst("\x48\x8B\x40\x10\xC7\x80"sv)
            .offset(0xA)
            .at_exe(),
    },
    {
        "default_zoom_level_camp"sv,
        // Follow the same logic as in `zoom_level` to get to the point where the zoom level is written.
        // Put a write bp on this float with the condition not to break at the RIP where shop/in-game level is written.
        // Then warp to camp
        PatternCommandBuffer{}
            .find_inst("\x48\x8B\x40\x10\xC7\x80"sv)
            .find_next_inst("\x48\x8B\x40\x10\xC7\x80"sv)
            .offset(0xA)
            .at_exe(),
    },
    {
        "coord_inside_active_shop_room"sv,
        // Same pattern as default_zoom_level_shop, check the condition before the jump that decides whether to activate
        // the shop zoom level or regular zoom level
        PatternCommandBuffer{}
            .find_inst("\xF3\x0F\x11\xB0****\x49"sv)
            .offset(-0x24)
            .decode_call()
            .at_exe(),
    },
    {
        "coord_inside_shop_zone"sv,
        // Can be found in same function as default_zoom_level_shop, check the condition higher up
        PatternCommandBuffer{}
            .find_inst("\x40\x8A\xBB\xA0\x00\x00\x00\x89\xFA\xE8"sv)
            .offset(0x9)
            .decode_call()
            .at_exe(),
    },
    {
        "coord_inside_shop_zone_rcx"sv,
        // See coord_inside_shop_zone, a little higher up rcx gets set to an on heap pointer
        PatternCommandBuffer{}
            .find_inst("\x0F\x84****\x48\x8B\x05****\x4A\x8D\x0C\x08"sv)
            .find_next_inst("\x0F\x84****\x48\x8B\x05****\x4A\x8D\x0C\x08"sv)
            .offset(0x6)
            .decode_pc()
            .at_exe(),
    },
    {
        "enforce_camp_camera_bounds"sv,
        // Go into basecamp, put a write bp on state.camera.bounds.top
        PatternCommandBuffer{}
            .find_inst("\xF3\x48\x0F\x2A\xF0\x45\x8B\x78\x4C"sv)
            .at_exe()
            .function_start(),
    },
    {
        "explosion_mask"sv,
        // Set a conditional bp on load_item for fx_explosion, throw a bomb and let it explode. When the debugger
        // breaks, continue execution, the pause menu will appear mid-explosion and you'll have a fully formed
        // fx_explosion entity. Put a read bp on its idle_counter and continue until you've breaked a couple of times.
        // The big function that breaks contains a call to the internal hitbox-overlap function for which the default
        // mask is put on the stack (0x18F)
        // it's virtual function 77 (process input)
        PatternCommandBuffer{}
            .find_inst("\x45\x0F\x57\xC0\x0F\x57\xDB\x4D\x89\xE8\xE8"sv)
            .offset(-0x15)
            .at_exe(),
    },
    {
        "attach_thrown_rope_to_background"sv,
        // Set a bp on load_item for ITEM_CLIMBABLE_ROPE and throw a rope
        // A little below that will 6 be written into the entity's segment_nr_inverse
        PatternCommandBuffer{}
            .find_inst("\xFF\x50\x30\xC7\x83\x30\x01\x00\x00*\x00\x00\x00"sv)
            .offset(0x09)
            .at_exe(),
    },
    {
        "process_ropes_one"sv,
        // Set a bp on load_item for ITEM_CLIMBABLE_ROPE and throw a rope, continue until all the segments are being made
        // At the beginning of this big function will be two comparisons to 6 and a comparison to 5
        PatternCommandBuffer{}
            .find_inst("\x83\xF9*\x75\x3B"sv)
            .offset(0x02)
            .at_exe(),
    },
    {
        "process_ropes_two"sv,
        // See process_ropes_one
        PatternCommandBuffer{}
            .get_address("process_ropes_one"sv)
            .find_next_inst("\x83\xF8*\x0F\x85"sv)
            .offset(0x02)
            .at_exe(),
    },
    {
        "process_ropes_three"sv,
        // See process_ropes_two
        PatternCommandBuffer{}
            .get_address("process_ropes_two"sv)
            .find_next_inst("\x83\xF8*\x0F\x87****\x41"sv)
            .offset(0x02)
            .at_exe(),
    },
    {
        "mount_carry"sv,
        // Set a bp on player's Entity::overlay, then jump on a turkey
        PatternCommandBuffer{}
            .find_inst("\xe8****\x66\xc7\x43\x04\x00\x00"sv)
            .decode_call()
            .at_exe(),
    },
    {
        "unequip"sv,
        // Put a bp on the player's item count when it's 1, and unequip a jetpack in game
        // Go one function up in the callstack (it breaks in the routine to update the vector)
        PatternCommandBuffer{}
            .find_inst("\x39\x71\x20\x0F\x92\xC2\x48\x0F\x43\xC1\x48"sv)
            .at_exe()
            .function_start(),
    },
    {
        "insta_gib"sv,
        // Put a write bp on player's Entity::flags, conditionally exclude the couple bp's it hits for just being in the level,
        // then place yourself in Quillback's path
        PatternCommandBuffer{}
            .find_inst("\x48\x81\xCA\x00\x00\x00\x10\x49\x89\x54\x24\x30"sv)
            .at_exe()
            .function_start(),
    },
    {
        "teleport"sv,
        // Put a bp on `load_item` for ENT_TYPE_FX_TELEPORTSHADOW, do a teleport, the calling function is the one
        PatternCommandBuffer{}
            .find_inst("\xB9\x7E\xFC\xFF\xFF\x03\x48\x14\x4C\x89\xE6"sv)
            .at_exe()
            .function_start(),
    },
    {
        "spawn_companion"sv,
        // Break on `load_item` with a condition of `rdx == 0xD7` (or whatever the id of a hired hand is).
        // Slap the coffin underneath Quillback
        PatternCommandBuffer{}
            .find_inst("\xBA\xD7\x00\x00\x00\x0F\x45\xD0"sv)
            .at_exe()
            .function_start(),
    },
    {
        "show_journal"sv,
        // Break on GameManager.journal_ui.state, open the journal
        PatternCommandBuffer{}
            .find_inst("\xC6\x87\x00\x02\x00\x00\x0F"sv)
            .at_exe()
            .function_start(),
    },
    {
        "generate_world_particles"sv,
        // Put read bp on State.particle_emitters, conditionally exclude the couple bp's it hits for just being in the level,
        // jump and when landing the floorpoof particle emitter id will be loaded into rdx. The subsequent call is the
        // generate_particles function.
        PatternCommandBuffer{}
            .find_inst("\x4D\x8D\x66\x08\x49\x8B\x5E\x08"sv)
            .at_exe()
            .function_start(),
    },
    {
        "generate_screen_particles"sv,
        // Put write bp on GameManager.screen_title.particle_whatever and go to the title screen
        PatternCommandBuffer{}
            .find_inst("\xE8****\x48\x89\x86\x40\x01\x00\x00\xF3\x0F\x10\x0D"sv)
            .decode_call()
            .at_exe(),
    },
    {
        "advance_screen_particles"sv,
        // See `generate_screen_particles`, a little bit below, the five pointers coming from the generate function are
        // passed to another function
        PatternCommandBuffer{}
            .find_inst("\xE8****\x48\x8B\x8E\x38\x01\x00\x00\xE8"sv)
            .decode_call()
            .at_exe(),
    },
    {
        "render_screen_particles"sv,
        // Go to the title screen, put a read bp on one of the particle emitter pointers and filter out the simulate call
        // Next break it hits is the render function (in the same function where the version string gets drawn on the screen)
        PatternCommandBuffer{}
            .find_inst("\xE8****\x48\x8B\x8E\x40\x01\x00\x00\x31\xD2"sv)
            .decode_call()
            .at_exe(),
    },
    {
        "free_particleemitterinfo"sv,
        // See `generate_screen_particles`, above that, the pointers to the particleemitters are checked, as well as fields inside
        // the particleemitter, and the same function is called if they are not null
        PatternCommandBuffer{}
            .find_inst("\xE8****\x48\x8B\xBE\x38\x01\x00\x00\x48\x85\xFF"sv)
            .decode_call()
            .at_exe(),
    },
    {
        "generate_illumination"sv,
        // Put a bp on load_item lamassu (or any other entity that has an internal Illumination*), follow into the first call of load_item
        // until the memory gets allocated, then put a write bp on the emmitted_light var inside the newly allocated memory.
        PatternCommandBuffer{}
            .find_inst("\xE8****\x48\x89\x86\x60\x01\x00\x00"sv)
            .decode_call()
            .at_exe(),
    },
    {
        "refresh_illumination_heap_offset"sv,
        // Put a bp on any Illumination.timer var, watch how it's written, the heap offset ptr is loaded a bit above
        PatternCommandBuffer{}
            .find_inst("\x48\x8B\x05****\x48\x85\xC0\x75\x16\xB9\x10\x00\x00\x00"sv)
            .decode_pc()
            .at_exe(),
    },
    {
        "ghost_spawn_time"sv,
        // 9000 frames / 60 fps = 2.5 minutes = 0x2328 ( 28 23 00 00 )
        // 10800 frames / 60 fps = 3 minutes = 0x2A30 ( 30 2A 00 00 )
        // Search for 0x2328 and 0x2A30 in very close proximity, it's in the ghost trigger logic perform virtual
        PatternCommandBuffer{}
            .get_virtual_function_address(VTABLE_OFFSET::LOGIC_GHOST_TRIGGER, VIRT_FUNC::LOGIC_PERFORM)
            .find_next_inst("\x74\x05\xB8"sv)
            .offset(0x3)
            .at_exe(),
    },
    {
        "ghost_spawn_time_cursed_player1"sv,
        // See `ghost_spawn_time` on how to search. New in 1.23.x is the fact that now all four players get checked
        // for curse, and they all have individual ghost trigger timings (all 0x2328 of course)
        PatternCommandBuffer{}
            .get_virtual_function_address(VTABLE_OFFSET::LOGIC_GHOST_TRIGGER, VIRT_FUNC::LOGIC_PERFORM)
            .find_next_inst("\x30\xB8"sv)
            .offset(0x2)
            .at_exe(),
    },
    {
        "ghost_spawn_time_cursed_player2"sv,
        PatternCommandBuffer{}
            .get_virtual_function_address(VTABLE_OFFSET::LOGIC_GHOST_TRIGGER, VIRT_FUNC::LOGIC_PERFORM)
            .find_next_inst("\x30\xB8"sv)
            .find_next_inst("\x30\xB8"sv)
            .offset(0x2)
            .at_exe(),
    },
    {
        "ghost_spawn_time_cursed_player3"sv,
        PatternCommandBuffer{}
            .get_virtual_function_address(VTABLE_OFFSET::LOGIC_GHOST_TRIGGER, VIRT_FUNC::LOGIC_PERFORM)
            .find_next_inst("\x30\xB8"sv)
            .find_next_inst("\x30\xB8"sv)
            .find_next_inst("\x30\xB8"sv)
            .offset(0x2)
            .at_exe(),
    },
    {
        "ghost_spawn_time_cursed_player4"sv,
        PatternCommandBuffer{}
            .get_virtual_function_address(VTABLE_OFFSET::LOGIC_GHOST_TRIGGER, VIRT_FUNC::LOGIC_PERFORM)
            .find_next_inst("\x30\xB8"sv)
            .find_next_inst("\x30\xB8"sv)
            .find_next_inst("\x30\xB8"sv)
            .find_next_inst("\x30\xB8"sv)
            .offset(0x2)
            .at_exe(),
    },
    {
        "olmec_transition_phase_1_y_level"sv,
        // Put a write bp on Olmec.attack_phase and trigger phase 1.
        // There will be a reference to the same float loaded in an xmm register twice,
        // once compared and once as an arg for a call. Both need to point to our custom float.
        // This address needs to point 1 after the first movss instruction because the relative
        // distance to our custom float needs to be calculated from this point.
        PatternCommandBuffer{}
            .find_inst("\xF3\x0F\x58\x48\x44\xF3\x0F\x10\x15****\x0F\x2E\xD1"sv)
            .offset(0xD)
            .at_exe(),
    },
    {
        "olmec_transition_phase_2_y_level"sv,
        // Look in the function determined by `olmec_transition_phase_1_y_level`
        // Search below the phase assignment to a similar pattern (two movss instructions, one
        // compared, one as arg for a call). The value should point at 83.0
        PatternCommandBuffer{}
            .find_inst("\xF3\x0F\x58\x40\x44\xF3\x0F\x10\x0D****\x0F\x2E\xC8\x48\x8B\x45"sv)
            .offset(0xD)
            .at_exe(),
    },
    {
        "olmec_transition_phase_1"sv,
        // Put write bp on olmec.attack_phase (when he's in phase 0)
        // Look for the condition that jumps over the little section that changes the phase to 1
        PatternCommandBuffer{}
            .find_inst("\x0F\x2E\xD1*\x2E\xF3\x0F\x10\x0D"sv)
            .offset(0x3)
            .at_exe(),
    },
    {
        "blood_multiplication"sv,
        // Put a read bp on Caveman(EntityDB):blood_content and kill one. If you look up a bit you will see
        // the value 2 get loaded into a register, this is the multiplication factor. From 1.23.x
        // onwards, the difference between wearing Vlad's cape and not is made by setting the default
        // at two, and subtracting 1 if you're not wearing the cape. This makes it hard to let the
        // modder set two distinct multiplication factors. When adjusting the hardcoded 2, you will
        // be setting Vlad's cape multiplier, and default will be n-1.
        // The pattern occurs twice with seemingly the same code logic, but don't know how to trigger.
        PatternCommandBuffer{}
            .find_inst("\x40\x0F\x94\xC7\xBD****\x29\xFD"sv)
            .offset(0x5)
            .at_exe(),
    },
    {
        "kapala_hud_icon"sv,
        // Put a read bp on KapalaPowerup:amount_of_blood
        PatternCommandBuffer{}
            .find_inst("\x0F\xB6\x89\x30\x01\x00\x00"sv)
            .at_exe(),
    },
    {
        "kapala_blood_threshold"sv,
        // Put a write bp on KapalaPowerup:amount_of_blood
        PatternCommandBuffer{}
            .find_inst("\x88\x88\x30\x01\x00\x00\x80\xF9"sv)
            .offset(0x8)
            .at_exe(),
    },
    {
        "sparktrap_angle_increment"sv,
        // Put a read bp on Spark:rotatnio_angle, the next instruction adds a hardcoded float from constant, we want address of that constant (not the whole instruction)
        PatternCommandBuffer{}
            .find_after_inst("\xF3\x0F\x10\x89\x58\x01\x00\x00\xF3\x0F\x58\x0D"sv)
            .at_exe(),
    },
    {
        "arrowtrap_projectile"sv,
        // Put a conditional bp on load_item (rdx = 0x173 (id of wooden arrow))
        // Trigger a trap
        PatternCommandBuffer{}
            .find_inst("\xBA****\x0F\x28\xD1\xE8****\x90"sv)
            .offset(0x1)
            .at_exe(),
    },
    {
        "poison_arrowtrap_projectile"sv,
        // See `arrowtrap_projectile`, but trigger a poison trap
        PatternCommandBuffer{}
            .find_inst("\xBA****\x0F\x28\xD1\xE8****\x48\x89\xC6\x48\x8B\x00"sv)
            .offset(0x1)
            .at_exe(),
    },
    {
        "give_powerup"sv,
        // Put a write bp on Player(PowerupCapable).powerups.size and give that player a powerup
        // Go up in the callstack until you find a function that takes the powerup ID in rdx
        PatternCommandBuffer{}
            .find_inst("\x8D\x86\xD4\xFD\xFF\xFF\x83\xF8\x03"sv)
            .at_exe()
            .function_start(),
    },
    {
        "remove_powerup"sv,
        // Give the player an ankh, put a write bp on Player(PowerupCapable).powerups.size, kill the player
        // Go up in the callstack until you find a function that takes the powerup ID in rdx
        // Careful: in 1.23.x the beginning of the function isn't clear! There aren't a bunch of 0xCC's to indicate where
        // it begins. That means we can't use function_start, because it looks for that 0xCC. Just use a manual offset.
        // Look for the typical pushing onto the stack to find the start of the function.
        PatternCommandBuffer{}
            .find_inst("\x48\xC7\x45\x00\xFE\xFF\xFF\xFF\x89\xD0"sv)
            .offset(-0xD)
            .at_exe(),
    },
    {
        "first_poison_tick_timer_default"sv,
        // Put a write bp on a non-poisoned Player(Movable):poison_tick_timer and poison the player
        // New in 1.21.x: This is only the first delay to a poison tick, from then on, see `subsequent_poison_timer_tick_default`
        // Note that there is a similar value of 1800 frames being written just above this location, no idea what triggers that
        PatternCommandBuffer{}
            .find_inst("\x66\xC7\x86\x20\x01\x00\x00**\xE9"sv)
            .find_next_inst("\x66\xC7\x86\x20\x01\x00\x00**\xE9"sv)
            .offset(0x7)
            .at_exe(),
    },
    {
        "subsequent_poison_tick_timer_default"sv,
        // Put a write bp on a poisoned Player(Movable):poison_tick_timer after the first poison tick has occurred
        // and filter out the timer countdown
        PatternCommandBuffer{}
            .find_inst("\x66\x41\xC7\x87\x20\x01\x00\x00**\x49"sv)
            .offset(0x8)
            .at_exe(),
    },
    {
        "cosmic_ocean_subtheme"sv,
        // Put a write bp on LevelGen.theme_cosmicocean.sub_theme and go to CO
        PatternCommandBuffer{}
            .find_inst("\x80\x42\x6B\x01\xC6\x42\x69\x04\xC6\x42\x75\x06\xC3"sv)
            .offset(0x40)
            .at_exe(),
    },
    {
        "toast"sv,
        // Put a write bp on State.toast
        PatternCommandBuffer{}
            .find_inst("\x48\x8D\x55\xD0\x41\xB8\x21\x00\x00\x00\xE8"sv)
            .at_exe()
            .function_start(),
    },
    {
        "say_context"sv,
        // Find the pattern for `say`, go one up higher in the callstack and look what writes to rcx
        PatternCommandBuffer{}
            .find_after_inst("\xC6\x44\x24\x20\x01\x48\x8D\x0D"sv)
            .offset(-0x3)
            .decode_pc()
            .at_exe(),
    },
    {
        "force_dark_level"sv,
        // Put a write bp on State.level_flags (3rd byte, containing dark level flag)
        // Filter out all breaks, then load levels until you get a dark one
        PatternCommandBuffer{}
            .find_inst("\x80\x79\x52\x02**\x4D\x85\xD2"sv)
            .offset(0x4)
            .at_exe(),
    },
    {
        "character_db"sv,
        PatternCommandBuffer{}
            .find_inst("\x48\x6B\xC3\x2C\x48\x8D\x15****\x48"sv)
            .decode_pc(7)
            .at_exe(),
    },
    {
        "string_table"sv,
        PatternCommandBuffer{}
            .find_inst("\x48\x8D\x15****\x4C\x8B\x0C\xCA"sv)
            .decode_pc()
            .at_exe(),
    },
    {
        "construct_soundposition_ptr"sv,
        // Put a write bp on ACTIVEFLOOR_DRILL sound_pos1 and release the drill
        PatternCommandBuffer{}
            .find_inst("\xE8****\x49\x89\x84\x24\x30\x01\x00\x00"sv)
            .decode_call()
            .at_exe(),
    },
    {
        // Open the journal on any page and find references to its vftable pointer (look at page in game_manager.journal_ui.pages)
        // scroll up to the top, find refrence to this address (each page is referenced two times, but the top of vtable is like 10 times)
        "vftable_JournalPages"sv,
        PatternCommandBuffer{}
            .find_inst("\x49\x89\x04\x24\x41\x89\x4C\x24\x50"sv)
            .offset(-0x7)
            .decode_pc()
            .at_exe(),
    },
    {
        // Put bp on state->basecamp_dialogue choose base/extra _dialogue, which ever the character you speak to responses ->line
        // Scroll down, you should see two calls, first one is to format text (the execution can jump around, call it multiple times)
        // The second call is this function
        // Or put write bp on state->speech_bubble(pointer), you will end up somewhere in the middle of the function
        "speech_bubble_fun"sv,
        PatternCommandBuffer{}
            .find_inst("\xE8\xFE\xFF\xFF\xFF\x48\x89\xD6\x48\x89\xCB"sv)
            .at_exe()
            .function_start(),
    },
    {
        // Put bp on state->liquid_physics->(which liquid you want)->liquidtile_liquid_amount and spawn this liquid
        // you will see something like "rcx+1", address for the "1" is what you want
        "spawn_liquid_amount"sv,
        PatternCommandBuffer{}
            .find_after_inst("\x8B\x8C\x01\xA0\x00\x00\x00\x8D\x79"sv)
            .at_exe(),
    },
    {
        // Next to `write_to_file` this is the only usage of `fopen`
        // Couldn't find any useful XREF in Ghidra so this pattern is exactly the function start
        "read_from_file"sv,
        PatternCommandBuffer{}
            .set_optional(true)
            .find_inst("\x41\x57\x41\x56\x56\x57\x53\x48\x81\xec\x20\x01\x00\x00\x4c\x89\xc3\x49\x89\xd7\x49\x89\xce"sv)
            .at_exe(),
    },
    {
        // Find a function being called as `save_to_file("input.bak", "input.cfg", data_ptr, data_size)`
        "write_to_file"sv,
        PatternCommandBuffer{}
            .find_inst("\x4d\x89\xf0\x4d\x89\xe1\xe8****\xe9"sv)
            .find_inst("\xe8")
            .decode_call()
            .at_exe(),
    },
    {
        // Find a string containing STEAMUSERSTATS, the enclosing function returns an `ISteamUserStats**` in `param_1`
        "get_steam_user_stats"sv,
        PatternCommandBuffer{}
            .find_after_inst("\xff\x90\xd0\x00\x00\x00\x48\x8d\xbd\xe0\x03\x00\x00"sv)
            .find_inst("\x48\x8d")
            .decode_pc()
            .at_exe(),
    },
    {
        // Set condition bp on spawn_entity (not load_item) for one of the entities spawned by this generator
        // execute to the return two times, you should see this array right above
        // It's pointer to array[4]: 0x000000F5 0x000000EB 0x000000FC 0x000000FA
        "sun_chalenge_generator_ent_types"sv,
        PatternCommandBuffer{}
            .find_after_inst("\x48\x89\x4A\x38\x48\xC1\xE8\x1C\x83\xE0"sv)
            .offset(0x4)
            .at_exe(),
    },
    {
        // Set bp on prize_dispenser->itemid_2 and roll a 7, you should see this array right above
        // array[25]
        "dice_shop_prizes"sv,
        PatternCommandBuffer{}
            .find_after_inst("\x41\x88\x8C\x24\x36\x01\x00\x00\x41\x0F\xB6\x84\x04\x30\x01\x00\x00"sv)
            .offset(0x3)
            .at_exe(),
    },
    {
        // Set condition bp on load_item for ITEM_DICE_PRIZE_DISPENSER, execute first call
        // go to the address in RAX (new entity) and set write bp on +0x130 (or execute till you see function that writes to this address)
        // we want address after (rol rsi,1B) - it should be 14 bytes that we want to change and then - (mov qword ptr ds:[rax+20],rdi | mov qword ptr ds:[rax+28],rsi)
        "dice_shop_prizes_id_roll"sv,
        PatternCommandBuffer{}
            .find_after_inst("\x49\x0F\xAF\xF8\x48\x29\xD6\x48\xC1\xC6\x1B"sv)
            .at_exe(),
    },
    {
        // Start local coop, kill one of the players, go to state->items->inventory of that player, set write bp on `time_of_death`
        // Spawn coffin, and set it's `respawn_player` to true, open the coffin, you should hit the bp right above the this function call
        "spawn_player"sv,
        PatternCommandBuffer{}
            .find_inst("\x4F\x8D\x0C\x7F\x42\x80\xBC\x89\xB8\x54\x00\x00\x00"sv)
            .at_exe()
            .function_start(),
    },
    {
        // Set conditional bp on load_item with vampire id, break altar
        // execute out of load_item, scroll up to find bunch of const addresses, on of which is array containing 5 id's (as of writing this comment, the address in not align to 8 bytes)
        "altar_break_ent_types"sv,
        PatternCommandBuffer{}
            .find_after_inst("\x45\x31\xFF\x4C\x8D\x25"sv)
            .at_exe(),
    },
    {
        // Set write bp on Movable.poison_tick_timer, get hit by cobra's acid spit
        "poison_entity"sv,
        PatternCommandBuffer{}
            .find_inst("\x48\x8B\x4E\x08\xF6\x41\x51\x04"sv)
            .at_exe()
            .function_start(),
    },
    {
        // Set conditional bp on load_item for KEY, execute til return, scroll up untill you find instruction writing const into r14
        "waddler_drop_array"sv,
        PatternCommandBuffer{}
            .find_after_inst("\x45\x0F\x57\xDB\x4C\x8D\x35"sv)
            .at_exe(),
    },
    {
        // Inside the same function as the above pattern, there should be: add r13, 1 | cmp r13, 3  (3 being the size)
        "waddler_drop_size"sv,
        PatternCommandBuffer{}
            .find_after_inst("\xF3\x0F\x11\x88\x0C\x01\x00\x00\x49\x83\xC5\x01"sv)
            .offset(3)
            .at_exe(),
    },
    {
        // Get ankh, die, when you get respawned on the door, but still not have health, set write bp on Movable.health, looking for (cmp eax,3)
        "ankh_health"sv,
        PatternCommandBuffer{}
            .find_after_inst("\x41\x0F\xB6\x87\x17\x01\x00\x00\x83\xF8"sv)
            .at_exe(),
    },
    {
        // Set write bp on State->shops->restricted_item_count, this structure is quite common so i chosen pattern before the call
        "add_shopitem"sv,
        PatternCommandBuffer{}
            .find_after_inst("\x4C\x8D\x84\x24\xD0\x00\x00\x00\xE8"sv)
            .offset(-0x1)
            .decode_call()
            .at_exe(),
    },
    {
        // Find a string "Basic systems initialized", right after it's usage (found via XREFS)
        // stuff gets emplaced to a map, it is this map
        "graphics_settings_map"sv,
        PatternCommandBuffer{}
            .find_after_inst("\x48\xb8\x77\x5f\x73\x63\x61\x6c\x65\x00"sv)
            .find_inst("\x4c\x8d"sv)
            .decode_pc()
            .at_exe(),
    },
    {
        // See graphics_settings_map, then go further down and another map is used, it is this map
        "settings_map"sv,
        PatternCommandBuffer{}
            .find_after_inst("\x48\xb8\x64\x61\x6d\x73\x65\x6c\x5f\x73"sv)
            .find_inst("\x48\x8d"sv)
            .decode_pc()
            .at_exe(),
    },
    {
        "grow_chain_and_blocks"sv,
        PatternCommandBuffer{}
            .find_inst("\x31\xC0\x45\x31\xED\x89\x4C\x24\x3C"sv)
            .at_exe()
            .function_start(),
    },
};
std::unordered_map<std::string_view, size_t> g_cached_addresses;

void preload_addresses()
{
    Memory mem = Memory::get();
    const char* exe = mem.exe();
    for (auto [address_name, rule] : g_address_rules)
    {
        if (auto address = rule(mem, exe, address_name))
        {
            g_cached_addresses[address_name] = address.value();
        }
    }
}
size_t load_address(std::string_view address_name)
{
    auto it = g_address_rules.find(address_name);
    if (it != g_address_rules.end())
    {
        Memory mem = Memory::get();
        if (auto address = it->second(mem, mem.exe(), address_name))
        {
            g_cached_addresses[address_name] = address.value();
            return address.value();
        }
    }
    const std::string message = fmt::format("Tried to get unknown address '{}'{}", address_name, get_error_information());
    MessageBox(NULL, message.c_str(), NULL, MB_OK);
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
