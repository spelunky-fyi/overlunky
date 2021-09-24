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
        "state_location"sv,
        PatternCommandBuffer{}
            .find_inst("\x49\x0F\x44\xC0"sv)
            .offset(0x1)
            .find_inst("\x49\x0F\x44\xC0"sv)
            .offset(-0x19)
            .find_inst("\x48\x8B"sv)
            .decode_pc()
            .at_exe(),
    },
    {
        "write_load_opt"sv,
        PatternCommandBuffer{}
            .find_inst("\xFF\xD3\xB9\xFA\x00\x00\x00"sv)
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
        "spawn_entity"sv,
        // First call in LoadItem is to this function
        PatternCommandBuffer{}
            .find_inst("\x44\x88\xb8\xa0\x00\x00\x00\xf3\x0f\x11\x78\x40"sv)
            .at_exe()
            .function_start(),
    },
    {
        "virtual_functions_table"sv,
        // Look at any entity in memory, dereference the __vftable to see the big table of pointers
        // scroll up to the first one, and find a reference to that
        PatternCommandBuffer{}
            .find_inst("\x48\x8D\x0D\x03\x79\x51\x00"sv)
            .decode_pc()
            .at_exe(),
    },
    {
        "fmod_studio"sv,
        // Break at startup on FMOD::Studio::System::initialize, the first parameter passed is the system-pointer-pointer
        PatternCommandBuffer{}
            .find_inst("\xBA\x05\x01\x02\x00"sv)
            .offset(-0x7)
            .decode_pc()
            .at_exe(),
    },
    {
        "fmod_event_properties"sv,
        // Find a call to FMOD::Studio::EventDescription::getParameterDescriptionByName, the second parameter is the name of the event
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
        // Find a call to FMOD::Studio::System::getEvent (should be before the call to FMOD::Studio::EventDescription::getParameterDescriptionByName)
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
        // Search for string "generic.lvl", it is used in a call to this function
        PatternCommandBuffer{}
            .find_inst("\x45\x84\xed\x74\x0f"sv)
            .offset(0x1)
            .find_inst("\x45\x84\xed\x74\x0f"sv)
            .find_inst("\xe8"sv)
            .decode_call()
            .at_exe(),
    },
    {
        "level_gen_handle_tile_code"sv,
        PatternCommandBuffer{}
            .find_inst("\xE8****\x83\xC5\x01"sv)
            .decode_call()
            .at_exe(),
    },
    {
        "level_generate_room"sv,
        // One call up from generate_room_from_tile_codes
        PatternCommandBuffer{}
            .find_inst("\xE8****\x83\xC6\x01\x39\xF3"sv)
            .decode_call()
            .at_exe(),
    },
    {
        "level_gather_room_data"sv,
        // First call in generate_room, it gets something from the unordered_map in `param_1 + 0x108`
        PatternCommandBuffer{}
            .find_inst("\xE8****\x44\x8A\x44\x24*\x45\x84\xC0"sv)
            .decode_call()
            .at_exe(),
    },
    {
        "level_get_random_room_data"sv,
        // Call to this is the only thing happening in a loop along with checking a flag on the returned value
        PatternCommandBuffer{}
            .find_inst("\xE8****\x48\x8B\x58\x08"sv)
            .decode_call()
            .at_exe(),
    },
    {
        "level_gen_generate_room_from_tile_codes"sv,
        // One of the few calls to handle_tile_code, does a `if (param != 0xec)` before the call
        PatternCommandBuffer{}
            .find_inst("\xE8****\x45\x89\xF8"sv)
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
        "render_hud"sv,
        // Locate in memory 2B 00 24 00 25 00 64 00 "+$%d" (UTF16 str of the money gained text)
        // Find reference to this memory, it's only used in the HUD
        PatternCommandBuffer{}
            .find_inst("\xB8\x88\x14\x00\x00\xE8"sv)
            .at_exe()
            .function_start(),
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
            .find_inst("\x48\x8B\x05\xCF\x4F\x15\x00"sv)
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
        "default_zoom_level_camp"sv,
        // Follow the same logic as in `zoom_level` to get to the point where the zoom level is written.
        // Put a write bp on this float with the condition not to break at the RIP where shop/in-game level is written.
        // Then warp to camp
        PatternCommandBuffer{}
            .find_inst("\xC7\x80****\x00\x00\x58\x41"sv)
            .offset(0x1)
            .find_inst("\xC7\x80****\x00\x00\x58\x41"sv)
            .offset(0x6)
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
        "insta_gib"sv,
        // Put a write bp on player's Entity::flags, conditionally exclude the couple bp's it hits for just being in the level,
        // then place yourself in Quillback's path
        PatternCommandBuffer{}
            .find_inst("\x48\x81\xCA\x00\x00\x00\x10\x49\x89\x54\x24\x30"sv)
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
        "generate_particles"sv,
        // Put read bp on State.particle_emitters, conditionally exclude the couple bp's it hits for just being in the level,
        // jump and when landing the floorpoof particle emitter id will be loaded into rdx. The subsequent call is the
        // generate_particles function.
        PatternCommandBuffer{}
            .find_inst("\x4D\x8D\x66\x08\x49\x8B\x5E\x08"sv)
            .at_exe()
            .function_start(),
    },
    {
        "ghost_spawn_time"sv,
        // 9000 frames / 60 fps = 2.5 minutes = 0x2328 ( 28 23 00 00 )
        // 10800 frames / 60 fps = 3 minutes = 0x2A30 ( 30 2A 00 00 )
        // Search for 0x2328 and 0x2A30 in very close proximity
        PatternCommandBuffer{}
            .find_inst("\xB8\x28\x23\x00\x00\x4C\x39\xCA\x74\x05"sv)
            .offset(0xB)
            .at_exe(),
    },
    {
        "ghost_spawn_time_cursed_player1"sv,
        // See `ghost_spawn_time` on how to search. New in 1.23.x is the fact that now all four players get checked
        // for curse, and they all have individual ghost trigger timings (all 0x2328 of course)
        PatternCommandBuffer{}
            .find_inst("\xB8\x28\x23\x00\x00\x4C\x39\xCA\x74\x05"sv)
            .offset(-0x59)
            .at_exe(),
    },
    {
        "ghost_spawn_time_cursed_player2"sv,
        PatternCommandBuffer{}
            .find_inst("\xB8\x28\x23\x00\x00\x4C\x39\xCA\x74\x05"sv)
            .offset(-0x3B)
            .at_exe(),
    },
    {
        "ghost_spawn_time_cursed_player3"sv,
        PatternCommandBuffer{}
            .find_inst("\xB8\x28\x23\x00\x00\x4C\x39\xCA\x74\x05"sv)
            .offset(-0x1D)
            .at_exe(),
    },
    {
        "ghost_spawn_time_cursed_player4"sv,
        PatternCommandBuffer{}
            .find_inst("\xB8\x28\x23\x00\x00\x4C\x39\xCA\x74\x05"sv)
            .offset(0x1)
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
