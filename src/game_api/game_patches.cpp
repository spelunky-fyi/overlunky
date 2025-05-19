#include "game_patches.hpp"

#include <array>
#include <cstdint>
#include <fmt/format.h>
#include <map>
#include <string>
#include <string_view>
#include <utility>

#include "entity.hpp"        // for Entity
#include "entity_db.hpp"     // for to_id
#include "layer.hpp"         // for Layer, EntityList
#include "memory.hpp"        // for Memory, get_address, memory_read ...
#include "search.hpp"        // for get_address
#include "state.hpp"         // for StateMemory
#include "virtual_table.hpp" // for get_virtual_function_address

void patch_orbs_limit()
{
    /*
     * The idea: we nuke jump instruction and some nop's after it
     * in new code we check if the number of orbs is greater than 3
     * if yes we set it as 3 (2 because of 0 - 2 range)
     * we jump back to the place of original jump
     */
    static bool once = false;
    if (once)
        return;

    auto& memory = Memory::get();
    const auto function_offset = get_virtual_function_address(VTABLE_OFFSET::ITEM_FLOATING_ORB, (uint32_t)VIRT_FUNC::ENTITY_KILL);
    // finding exit of the loop that counts orbs, after jump there is unused code so we have enough space for a our patch
    auto instance = find_inst(memory.exe(), "\xF3\x75"sv, function_offset, function_offset + 0x622, "patch_orbs_limit");
    auto offset = memory.at_exe(instance) + 3;

    if (instance == 0) // not found, don't crash the game (ex. PL + OV calling this)
        return;

    auto loop_exit_jump = memory_read<int8_t>(offset + 1);

    // set r8b to 2 if it's more than 2
    std::string_view new_code{
        "\x41\x80\xf8\x02"sv //   cmp    r8b,0x2
        "\x72\x03"sv         //   jb     <end>
        "\x41\xb0\x02"sv};   //   mov    r8b,0x2

    patch_and_redirect(offset, 8, new_code, true, offset + 2 + loop_exit_jump);
    once = true;
}

bool check_if_ent_type_exists(ENT_TYPE type, int mask)
{
    StateMemory* state = get_state_ptr();

    const auto entities_map = &state->layers[0]->entities_by_mask; // game code only cares about the front layer, so we do the same
    auto it = entities_map->find(mask);
    if (it == entities_map->end())
        return false;

    for (auto entity : it->second.entities())
    {
        if (entity->type->id == type)
            return true;
    }
    return false;
}

size_t g_olmec_patch_size;
size_t g_olmec_patch_addr;
void patch_olmec_kill_crash()
{
    static bool once = false;
    if (once)
        return;

    // [patch ... whatever this code does]

    const auto offset = get_address("olmec_lookup_crash");
    constexpr auto code_to_move = 7;
    auto& memory = Memory::get();
    size_t return_addr;
    {
        // find address to escape to
        size_t rva = offset - memory.exe_address();
        // below the patched code there are two jumps that performs long jump, at the end of it there is 'mov rax,qword ptr ds:[rdi]',
        // from this point find jump that's jumps over sond meta and fmod stuff, the jump ends up on code `mov eax,dword ptr ss:[rbp+10]`, that's our target for return_addr
        auto jump_out_lookup = find_inst(memory.exe(), "\x48\xC7\x40\x60\x00\x00\x00\x00"sv, rva, rva + 0x69D, "patch_olmec_kill_crash");
        if (jump_out_lookup == 0)
            return;

        return_addr = memory.at_exe(jump_out_lookup + 8); // 8 - pattern size
    }

    /* The idea:
     * if it's not the end of the array it's looking for olmec, jump back to the original code via jump in `new_code`
     * if it's end of the array (no olmec found) jump to return_addr
     * the place for return_addr was kind of choosen by feel, as the code is complicated
     * the whole point of the patched code is to find olmec and check it's faze, maybe for the music? no idea, it's a big function
     */

    std::string_view new_code{
        "\x0f\x85\x00\x00\x00\x00"sv}; // jne   (offset needs to be updated after we know the address)

    auto new_code_addr = patch_and_redirect(offset, code_to_move, new_code, false, return_addr);
    if (new_code_addr == 0)
        return;

    new_code_addr += code_to_move;
    int32_t rel = static_cast<int32_t>(offset + code_to_move - (new_code_addr + 6)); // +6 after the jump
    write_mem_prot(new_code_addr + 2, rel, true);

    // [patch the cutscene]

    const auto patch_addr = get_address("olmec_lookup_in_theme");
    if (patch_addr == 0)
        return;

    size_t addr_to_jump_to;
    {
        // find end of the function that sets the camera and stuff
        size_t rva = patch_addr - memory.exe_address();
        size_t rva_jumpout_to = find_inst(memory.exe(), "\x48\x8B\x06"sv, rva, rva + 0xA50, "patch_olmec_kill_crash");
        if (rva_jumpout_to == 0)
            return;

        addr_to_jump_to = memory.at_exe(rva_jumpout_to + 14); // +14 just offset to get what we want
    }

    std::string cutscene_new_code = fmt::format(
        "\xb9{}"sv                    // mov      ecx, ENT_TYPE
        "\xba\x80\x00\x00\x00"sv      // mov      edx, 0x80 (MASK::ACTIVEFLOOR)
        "\x48\xb8{}"sv                // movabs   RAX, &check_if_ent_type_exists
        "\xff\xd0"sv                  // call     RAX
        "\x84\xc0"sv                  // test     al, al
        "\x0f\x85\x00\x00\x00\x00"sv, // jnz      (offset needs to be updated after we know the address)
        to_le_bytes(to_id("ENT_TYPE_ACTIVEFLOOR_OLMEC")),
        to_le_bytes((size_t)&check_if_ent_type_exists));

    /* The idea:
     * same as tiamat
     */
    constexpr auto cutscene_code_to_move = 7;
    g_olmec_patch_size = cutscene_new_code.size() + cutscene_code_to_move; // without the final jump added by patch_and_redirect

    g_olmec_patch_addr = patch_and_redirect(patch_addr, cutscene_code_to_move, cutscene_new_code, false, addr_to_jump_to);
    if (g_olmec_patch_addr == 0)
        return;

    auto jump_addr = g_olmec_patch_addr + g_olmec_patch_size;
    int32_t rel_off = static_cast<int32_t>(patch_addr + cutscene_code_to_move - jump_addr);
    write_mem_prot(jump_addr - 4, rel_off, true); // update the jump offset

    once = true;
}

size_t g_tiamat_patch_size;
size_t g_tiamat_patch_addr;
void patch_tiamat_kill_crash()
{
    static bool once = false;
    if (once)
        return;

    auto& memory = Memory::get();
    const auto patch_addr = get_address("tiamat_lookup_in_theme");
    if (patch_addr == 0)
        return;

    size_t return_to_addr;
    {
        // find end of the function that sets the camera and stuff
        auto rva = patch_addr - memory.exe_address();
        auto rva_jumpout_to = find_inst(memory.exe(), "\x49\x89\x0C\xC6"sv, rva, rva + 0x5C7, "patch_tiamat_kill_crash");
        if (rva_jumpout_to == 0)
            return;

        return_to_addr = memory.at_exe(rva_jumpout_to + 4); // +4 - after pattern
    }

    std::string new_code = fmt::format(
        "\xb9{}"sv                    // mov      ecx, ENT_TYPE
        "\xba\x04\x00\x00\x00"sv      // mov      edx, 0x4 (MASK::MONSTER)
        "\x48\xb8{}"sv                // movabs   RAX, &check_if_ent_type_exists
        "\xff\xd0"sv                  // call     RAX
        "\x84\xc0"sv                  // test     al, al
        "\x0f\x85\x00\x00\x00\x00"sv, // jnz      (offset needs to be updated after we know the address)
        to_le_bytes(to_id("ENT_TYPE_MONS_TIAMAT")),
        to_le_bytes((size_t)&check_if_ent_type_exists));

    /* The idea:
     * After state.screen == screen::level check, we plug our code
     * it calls simple function to check if entity exists (tiamat in this case)
     * if tiamat is found, we jump back to the original code, if not, jump to return_to_addr
     * we can kind of ignore the replaced game code as it sets R14 register
     * so we let patch_and_redirect just copy before our new code
     */
    constexpr int copy_over_code_size = 7;
    g_tiamat_patch_size = new_code.size() + copy_over_code_size; // without the final jump added by patch_and_redirect

    g_tiamat_patch_addr = patch_and_redirect(patch_addr, copy_over_code_size, new_code, false, return_to_addr);
    if (g_tiamat_patch_addr == 0)
        return;

    auto jump_addr = g_tiamat_patch_addr + g_tiamat_patch_size;
    int32_t rel = static_cast<int32_t>(patch_addr + copy_over_code_size - jump_addr);
    write_mem_prot(jump_addr - 4, rel, true); // update the jump offset

    once = true;
}

void patch_liquid_OOB()
{
    /*
     * The idea:
     * there is a loop thru all liquid entities (probably for the collision stuff)
     * if liquid is out of bounds (coordinate below 0) we essentially simulate `continue;` behavior
     */

    static bool once = false;
    if (once)
        return;

    const auto offset = get_address("liquid_OOB_crash");
    size_t continue_addr;
    {
        // find address to continue the loop
        auto& memory = Memory::get();
        auto rva = offset - memory.exe_address();
        // first `ja` loop
        auto jump_out_lookup = find_inst(memory.exe(), "\x0F\x87"sv, rva, rva + 0x7D, "patch_liquid_OOB");
        if (jump_out_lookup == 0)
            return;

        auto jump_out_addr = memory.at_exe(jump_out_lookup);
        auto jump_offset = memory_read<int32_t>(jump_out_addr + 2); // should be negative
        continue_addr = jump_out_addr + 6 + jump_offset;            // 6 size of the jump instruction
    }

    constexpr auto code_to_move = 5;

    std::string_view new_code{
        "\x48\x83\xFD\x00"sv           //   cmp    ebp,0x0 (ebp = y)
        "\x0f\x8C\x00\x00\x00\x00"sv   //   jl     (offset needs to be updated after we know the address)
        "\x48\x83\xfa\x00"             //   cmp    rdx,0x0 (rdx = x)
        "\x0f\x8C\x00\x00\x00\x00"sv}; //   jl     (same jump as before)

    auto new_code_addr = patch_and_redirect(offset, code_to_move, new_code);
    if (new_code_addr == 0)
        return;

    new_code_addr += code_to_move;

    int32_t rel = static_cast<int32_t>(continue_addr - (new_code_addr + 10)); // +10 after the jump
    write_mem_prot(new_code_addr + 6, rel, true);
    write_mem_prot(new_code_addr + 16, rel - 10, true);

    // replace "Ran out of liquids pool!" with jmp out of the main loop, which effectively
    // fixes the problem of spawning too much liquid, simply removing some old ones when new is spawned
    {
        const size_t message_addr = offset - 0x10f;
        // replace call MessageBoxA with
        // jmp whatever (after of the main while do-while != 5 loop)
        // nop
        write_mem_prot(message_addr, "\xE9\x72\x01\x00\x00\x90"sv, true);
    }

    once = true;
}

void set_skip_olmec_cutscene(bool skip)
{
    patch_olmec_kill_crash(); // just in case

    // simple jump over the olmec check, nop here just so there is no funny business
    static const std::string code = fmt::format("\xEB{}\x90"sv, to_le_bytes(static_cast<int8_t>(g_olmec_patch_size - 2)));
    if (skip)
        write_mem_recoverable("set_skip_olmec_cutscene", g_olmec_patch_addr, code, true);
    else
        recover_mem("set_skip_olmec_cutscene");
}

void set_skip_tiamat_cutscene(bool skip)
{
    patch_tiamat_kill_crash(); // just in case

    // simple jump over the tiamat check, nop here just so there is no funny business
    static const std::string code = fmt::format("\xEB{}\x90"sv, to_le_bytes(static_cast<int8_t>(g_tiamat_patch_size - 2)));
    if (skip)
        write_mem_recoverable("set_skip_tiamat_cutscene", g_tiamat_patch_addr, code, true);
    else
        recover_mem("set_skip_tiamat_cutscene");
}

void patch_ushabti_error()
{
    // nop's MessageBoxA("Number of generated Ushabti statues isn't 100!")
    static bool once = false;
    if (once)
        return;
    const auto offset = get_address("ushabti_error");
    write_mem_prot(offset, "\x90\x90\x90\x90\x90\x90"sv, true);
    once = true;
}

void patch_entering_closed_door_crash()
{
    static bool once = false;
    if (once)
        return;

    size_t addr = get_address("enter_closed_door_crash");
    size_t return_addr;
    {
        auto& memory = Memory::get();
        auto rva = find_inst(memory.exe(), "\x49\x39\xD4", addr - memory.exe_address(), addr - memory.exe_address() + 0x3F5, "patch_entering_closed_door_crash");
        if (rva == 0)
            return;
        size_t jump_addr = memory.at_exe(rva + 3);
        int32_t offset = memory_read<int32_t>(jump_addr + 2);
        return_addr = jump_addr + 6 + offset;
    }
    std::string_view new_code{
        "\x48\x85\xC0"sv         // test   rax,rax
        "\x74\x0D"sv             // je     <end>
        "\x48\x8B\x48\x08"sv     // mov    rcx,QWORD PTR [rax+0x8]   // game code
        "\x41\x8B\x47\x28"sv     // mov    eax,DWORD PTR [r15+0x28]  // game code
        "\xE9\x00\x00\x00\x00"sv // jmp    (offset needs to be updated after we know the address)
    };

    auto new_code_addr = patch_and_redirect(addr, 8, new_code, true, return_addr);
    if (new_code_addr == 0)
        return;

    int32_t rel = static_cast<int32_t>((addr + 8) - (new_code_addr + 18));
    write_mem_prot(new_code_addr + 14, rel, true);
    once = true;
}

float* g_sparktrap_parameters{nullptr};
void modify_sparktraps(float angle_increment, float distance)
{
    if (g_sparktrap_parameters == nullptr)
    {
        static const auto offset = get_address("sparktrap_angle_increment") + 4;

        if (memory_read<uint8_t>(offset - 1) == 0x89) // check if sparktraps_hack is active
            return;

        const int32_t distance_offset = 0xF1;
        g_sparktrap_parameters = (float*)alloc_mem_rel32(offset + 4, sizeof(float) * 2);
        if (!g_sparktrap_parameters)
            return;

        int32_t rel = static_cast<int32_t>((size_t)g_sparktrap_parameters - (offset + 4));
        write_mem_prot(offset, rel, true);
        write_mem_prot(offset + distance_offset, (int32_t)(rel - distance_offset + sizeof(float)), true);
    }
    *g_sparktrap_parameters = angle_increment;
    *(g_sparktrap_parameters + 1) = distance;
}

float* get_sparktraps_parameters_ptr() // only for the UI
{
    return g_sparktrap_parameters;
}

void activate_sparktraps_hack(bool activate)
{
    if (activate)
    {
        static const auto offset = get_address("sparktrap_angle_increment");
        const int32_t distance_offset = 0xF1;

        write_mem_recoverable("sparktraps_hack", offset, "\xF3\x0F\x58\x89\x6C\x01\x00\x00"sv, true);
        write_mem_recoverable("sparktraps_hack", offset + distance_offset, "\xF3\x0F\x10\xB9\x70\x01\x00\x00"sv, true);
    }
    else
    {
        recover_mem("sparktraps_hack");
    }
}

void set_storage_layer(LAYER layer)
{
    static const auto storage_layer = get_address("storage_layer");
    if (layer == LAYER::FRONT || layer == LAYER::BACK)
        write_mem_prot(storage_layer, 0x1300 + 8 * (uint8_t)layer, true);
}

void set_kapala_blood_threshold(uint8_t threshold)
{
    static const auto kapala_blood_threshold = get_address("kapala_blood_threshold");
    write_mem_prot(kapala_blood_threshold, threshold, true);
}

void set_kapala_hud_icon(int8_t icon_index)
{
    static const size_t instruction_offset = get_address("kapala_hud_icon");
    static const size_t icon_index_offset = instruction_offset + 0x12;
    static const uint32_t distance = static_cast<uint32_t>(icon_index_offset - (instruction_offset + 7));

    if (icon_index < 0) // reset to original
    {
        write_mem_prot(instruction_offset + 2, 0x00013089, true);
    }
    else
    {
        // Instead of loading the value from KapalaPowerup:amount_of_blood (the instruction pointed at by instruction_offset)
        // we overwrite this with an instruction that loads a byte located a bit after the current function.
        // So you need to assemble `movzx  <relevant register>,BYTE PTR [rip+<distance>]`
        write_mem_prot(instruction_offset + 2, {0x0d}, true);
        write_mem_prot(instruction_offset + 3, distance, true);
        if (icon_index > 6)
        {
            icon_index = 6;
        }
        write_mem_prot(icon_index_offset, icon_index, true);
    }
}

void set_olmec_phase_y_level(uint8_t phase, float y)
{
    // Sets the Y-level Olmec changes phases. The defaults are :
    // - phase 1 (bombs) = 100
    // - phase 2 (ufos) = 83
    // Olmecs checks phases in order! The means if you want ufo's from the start
    // you have to put both phase 1 and 2 at e.g. level 199
    // If you want to make Olmec stay in phase 0 (stomping) all the time, you can just set
    // the phase 1 y level to 70. Don't set it too low, from 1.25.0 onwards, Olmec's stomp
    // activation distance seems to be related to the y-level trigger point.
    static size_t phase1_offset;
    if (phase1_offset == 0)
    {
        // from 1.23.x onwards, there are now two instructions per phase that reference the y-level float
        const size_t phase_1_instruction_a = get_address("olmec_transition_phase_1_y_level");
        const size_t phase_1_instruction_b = phase_1_instruction_a + 0xd;

        const size_t phase_2_instruction_a = get_address("olmec_transition_phase_2_y_level");
        const size_t phase_2_instruction_b = phase_2_instruction_a + 0x11;
        phase1_offset = (size_t)alloc_mem_rel32(phase_2_instruction_b + 4, sizeof(float) * 2);
        if (!phase1_offset)
            return;

        auto phase2_offset = phase1_offset + 0x4;

        // write the default values to our new floats
        write_mem_prot(phase1_offset, 100.0f, true);
        write_mem_prot(phase2_offset, 83.0f, true);

        // calculate the distances between our floats and the movss instructions
        auto distance_1_a = static_cast<int32_t>(phase1_offset - phase_1_instruction_a);
        auto distance_1_b = static_cast<int32_t>(phase1_offset - phase_1_instruction_b);
        auto distance_2_a = static_cast<int32_t>(phase2_offset - phase_2_instruction_a);
        auto distance_2_b = static_cast<int32_t>(phase2_offset - phase_2_instruction_b);

        // overwrite the movss instructions to load our floats
        write_mem_prot(phase_1_instruction_a - 4, distance_1_a, true);
        write_mem_prot(phase_1_instruction_b - 4, distance_1_b, true);
        write_mem_prot(phase_2_instruction_a - 4, distance_2_a, true);
        write_mem_prot(phase_2_instruction_b - 4, distance_2_b, true);
    }

    if (phase == 1)
    {
        *(float*)phase1_offset = y;
    }
    else if (phase == 2)
    {
        *(float*)(phase1_offset + sizeof(float)) = y;
    }
}

void force_olmec_phase_0(bool b)
{
    static const size_t offset = get_address("olmec_transition_phase_1");

    if (b)
        write_mem_recoverable("force_olmec_phase_0", offset, "\xEB\x2E"sv, true); // jbe -> jmp
    else
        recover_mem("force_olmec_phase_0");
}

void set_ghost_spawn_times(uint32_t normal, uint32_t cursed)
{
    static const auto ghost_spawn_time = get_address("ghost_spawn_time");
    static const auto ghost_spawn_time_cursed_p1 = get_address("ghost_spawn_time_cursed_player1");
    static const auto ghost_spawn_time_cursed_p2 = get_address("ghost_spawn_time_cursed_player2");
    static const auto ghost_spawn_time_cursed_p3 = get_address("ghost_spawn_time_cursed_player3");
    static const auto ghost_spawn_time_cursed_p4 = get_address("ghost_spawn_time_cursed_player4");

    write_mem_prot(ghost_spawn_time, normal, true);
    write_mem_prot(ghost_spawn_time_cursed_p1, cursed, true);
    write_mem_prot(ghost_spawn_time_cursed_p2, cursed, true);
    write_mem_prot(ghost_spawn_time_cursed_p3, cursed, true);
    write_mem_prot(ghost_spawn_time_cursed_p4, cursed, true);
}

void set_time_ghost_enabled(bool b)
{
    static size_t offset_trigger = 0;
    static size_t offset_toast_trigger = 0;
    if (offset_trigger == 0)
    {
        auto& memory = Memory::get();
        offset_trigger = memory.at_exe(get_virtual_function_address(VTABLE_OFFSET::LOGIC_GHOST_TRIGGER, static_cast<uint32_t>(VIRT_FUNC::LOGIC_PERFORM)));
        offset_toast_trigger = memory.at_exe(get_virtual_function_address(VTABLE_OFFSET::LOGIC_GHOST_TOAST_TRIGGER, static_cast<uint32_t>(VIRT_FUNC::LOGIC_PERFORM)));
    }
    if (b)
    {
        recover_mem("set_time_ghost_enabled");
    }
    else
    {
        write_mem_recoverable("set_time_ghost_enabled", offset_trigger, "\xC3\x90\x90\x90"sv, true);
        write_mem_recoverable("set_time_ghost_enabled", offset_toast_trigger, "\xC3\x90\x90\x90"sv, true);
    }
}

void set_time_jelly_enabled(bool b)
{
    auto& memory = Memory::get();
    static const size_t offset = memory.at_exe(get_virtual_function_address(VTABLE_OFFSET::LOGIC_COSMIC_OCEAN, static_cast<uint32_t>(VIRT_FUNC::LOGIC_PERFORM)));
    if (b)
        recover_mem("set_time_jelly_enabled");
    else
        write_mem_recoverable("set_time_jelly_enabled", offset, "\xC3\x90\x90\x90"sv, true);
}

void set_camp_camera_bounds_enabled(bool b)
{
    static const size_t offset = get_address("enforce_camp_camera_bounds");
    if (b)
        recover_mem("camp_camera_bounds");
    else
        write_mem_recoverable("camp_camera_bounds", offset, "\xC3\x90\x90"sv, true);
}

void set_explosion_mask(int32_t mask)
{
    static const size_t addr = get_address("explosion_mask");
    if (mask == -1)
        recover_mem("explosion_mask");
    else
        write_mem_recoverable("explosion_mask", addr, mask, true);
}

void set_max_rope_length(uint8_t length)
{
    uint32_t length_32 = length;
    static const auto attach_thrown_rope = get_address("attach_thrown_rope_to_background");
    static const auto process_ropes_one = get_address("process_ropes_one");
    static const auto process_ropes_two = get_address("process_ropes_two");
    static const auto process_ropes_three = get_address("process_ropes_three");

    // there's four instances where the max (default=6) is used

    // 1) When throwing a rope and it attaches to the background, the initial entity is
    // given a start value in its segment_nr_inverse variable
    write_mem_prot(attach_thrown_rope, length_32, true);

    // 2) and 3) at the top of the rope processing function are two comparisons to the max
    write_mem_prot(process_ropes_one, length, true);
    write_mem_prot(process_ropes_two, length, true);

    // 4) in the same function at the end of the little loop of process_ropes_two is a comparison to n-1
    uint8_t length_minus_one_8 = length - 1;
    write_mem_prot(process_ropes_three, length_minus_one_8, true);
}

void change_sunchallenge_spawns(std::vector<ENT_TYPE> ent_types)
{
    // [Known_Issue]: as all the functions that base some functionality on static, this can break if used in PL and OV simultaneously
    static uintptr_t offset;
    static uintptr_t new_code_address;
    if (offset == 0)
    {
        offset = get_address("sun_challenge_generator_ent_types");

        // just so we can recover the oryginal later
        save_mem_recoverable("sunchallenge_spawn", offset, 14, true);
    }
    const size_t table_offset = offset + 10; // offset to the offset of ent_type table
    ENT_TYPE* old_types_array = (ENT_TYPE*)(memory_read<int32_t>(table_offset) + table_offset + 4);
    bool was_edited_before = mem_written("sunchallenge_spawn");
    if (ent_types.size() == 0)
    {
        recover_mem("sunchallenge_spawn");
        if (was_edited_before)
            VirtualFree(old_types_array, 0, MEM_RELEASE);

        // just free it since it's just easier to put the code again
        if (new_code_address != 0)
        {
            VirtualFree(reinterpret_cast<LPVOID>(new_code_address), 0, MEM_RELEASE);
            new_code_address = 0;
        }
        return;
    }
    const auto data_size = ent_types.size() * sizeof(ENT_TYPE);
    ENT_TYPE* new_array = (ENT_TYPE*)alloc_mem_rel32(table_offset + 4, data_size);
    if (new_array)
    {
        std::memcpy(new_array, ent_types.data(), data_size);
        int32_t rel = static_cast<int32_t>((size_t)new_array - (table_offset + 4));
        write_mem_prot(table_offset, rel, true);

        if (new_code_address == 0)
        {
            std::string new_code = fmt::format("\x31\xD2\xB9{}\xF7\xF1\x67\x8D\x04\x95\x00\x00\x00\x00"sv, to_le_bytes(static_cast<uint32_t>(ent_types.size())));
            // xor edx, edx                 ; dividend high half = 0.
            // mov ecx, ent_types.size()    ; dividend low half
            // div ecx                      ; division, (divisor already in rax)
            //                              ; edx - remainder
            // lea eax,[edx * 4 + 0]        ; multiply by 4 (sizeof ENT_TYPE) and put result in rax

            new_code_address = patch_and_redirect(offset, 7, new_code, true);
        }
        else // update the size since the code is in place
            write_mem_prot(new_code_address + 3, to_le_bytes(static_cast<uint32_t>(ent_types.size())), true);

        if (was_edited_before)
            VirtualFree(old_types_array, 0, MEM_RELEASE);
    }
}

void change_diceshop_prizes(std::vector<ENT_TYPE> ent_types)
{
    static const auto offset = get_address("dice_shop_prizes_id_roll");
    static const auto array_offset = get_address("dice_shop_prizes");
    ENT_TYPE* old_types_array = (ENT_TYPE*)(memory_read<int32_t>(array_offset) + array_offset + 4);
    bool original_instr = (memory_read<uint8_t>(offset) == 0x89);

    if (ent_types.size() > 255 || ent_types.size() < 6) // has to be min 6 as the game needs 6 uniqe item ids for prize_dispenser
    {
        if (!ent_types.size())
        {
            if (!original_instr)
                VirtualFree(old_types_array, 0, MEM_RELEASE);

            recover_mem("diceshop_prizes");
        }
        return;
    }

    if ((original_instr && ent_types.size() == 25) ||                              // if it's the unchanged instruction and we set the same number of ent_type's
        (!original_instr && memory_read<uint8_t>(offset + 5) == ent_types.size())) // or new instruction but the same size
    {
        for (unsigned int i = 0; i < ent_types.size(); ++i)
            write_mem_recoverable("diceshop_prizes", (size_t)&old_types_array[i], ent_types[i], true);

        return;
    }

    const auto data_size = ent_types.size() * sizeof(ENT_TYPE);
    ENT_TYPE* new_array = (ENT_TYPE*)alloc_mem_rel32(array_offset + 4, data_size);

    if (new_array)
    {
        if (!original_instr)
            VirtualFree(old_types_array, 0, MEM_RELEASE);

        memcpy(new_array, ent_types.data(), data_size);
        int32_t rel = static_cast<int32_t>((size_t)new_array - (array_offset + 4));
        write_mem_recoverable("diceshop_prizes", array_offset, rel, true);

        if (original_instr)
        {
            std::string new_code = fmt::format("\x50\x31\xC0\x41\xB3{}\x88\xD0\x41\xF6\xF3\x88\xE2\x58"sv, to_le_bytes((uint8_t)ent_types.size()));
            // push rax
            // xor eax, eax
            // mov r11b, (size)
            // mov al, dl
            // divb r11b
            // mov dl, ah
            // pop rax
            write_mem_recoverable("diceshop_prizes", offset, new_code, true);
        }
        else
        {
            write_mem_recoverable("diceshop_prizes", offset + 5, (uint8_t)ent_types.size(), true);
        }
    }
}

void change_altar_damage_spawns(std::vector<ENT_TYPE> ent_types)
{
    if (ent_types.size() > 255)
        return;

    static const auto array_offset = get_address("altar_break_ent_types");
    ENT_TYPE* old_types_array = (ENT_TYPE*)(memory_read<int32_t>(array_offset) + array_offset + 4);
    const auto code_offset = array_offset + 0xDD;
    const auto instruction_shr = array_offset + 0x13D;
    const auto instruction_to_modifiy = array_offset + 0x204;
    const auto original_instr = (memory_read<uint8_t>(instruction_shr) == 0x41);
    if (ent_types.empty())
    {
        if (!original_instr)
            VirtualFree(old_types_array, 0, MEM_RELEASE);

        recover_mem("altar_damage_spawn");
        return;
    }
    if (!original_instr && memory_read<uint8_t>(code_offset + 2) == ent_types.size())
    {
        // original array is used for something else as well, so i never edit that content
        for (uint32_t i = 0; i < ent_types.size(); ++i)
            write_mem_recoverable("altar_damage_spawn", (size_t)&old_types_array[i], ent_types[i], true);

        return;
    }
    const auto data_size = ent_types.size() * sizeof(ENT_TYPE);
    ENT_TYPE* new_array = (ENT_TYPE*)alloc_mem_rel32(array_offset + 4, data_size);
    if (new_array)
    {
        if (!original_instr)
            VirtualFree(old_types_array, 0, MEM_RELEASE);

        memcpy(new_array, ent_types.data(), data_size);
        int32_t rel = static_cast<int32_t>((size_t)new_array - (array_offset + 4));
        write_mem_recoverable("altar_damage_spawn", array_offset, rel, true);

        if (original_instr)
        {
            std::string new_code = fmt::format("\x41\xB1{}\x48\xC1\xE8\x38\x41\xF6\xF1\x49\x89\xC1"sv, to_le_bytes((uint8_t)ent_types.size()));
            // mov R9b, (size)
            // shr RAX, 0x38
            // divb R9b
            // mov R9, RAX
            write_mem_recoverable("altar_damage_spawn", code_offset, new_code, true);
            write_mem_recoverable("altar_damage_spawn", instruction_shr, "\x49\xC1\xE9\x08"sv, true); // shr r9,0x8
            write_mem_recoverable("altar_damage_spawn", instruction_to_modifiy, (uint8_t)0x8C, true); // r9+r12 => r12+r9*4
        }
        else
        {
            write_mem_recoverable("altar_damage_spawn", code_offset + 2, (uint8_t)ent_types.size(), true);
        }
    }
}

void change_waddler_drop(std::vector<ENT_TYPE> ent_types)
{
    static bool modified = false;

    static const auto offset = get_address("waddler_drop_size");
    static const auto array_offset = get_address("waddler_drop_array");
    ENT_TYPE* old_types_array = (ENT_TYPE*)(memory_read<int32_t>(array_offset) + array_offset + 4);

    if (ent_types.size() > 255 || ent_types.size() < 1)
    {
        if (!ent_types.size())
        {
            if (modified)
                VirtualFree(old_types_array, 0, MEM_RELEASE);

            recover_mem("waddler_drop");
            modified = false;
        }
        return;
    }

    if ((!modified && ent_types.size() == 3) ||                         // if it's the unchanged instruction and we set the same number of ent_type's
        (modified && memory_read<uint8_t>(offset) == ent_types.size())) // or new instruction but the same size
    {
        for (unsigned int i = 0; i < ent_types.size(); ++i)
            write_mem_recoverable("waddler_drop", (size_t)&old_types_array[i], ent_types[i], true);

        return;
    }

    const auto data_size = ent_types.size() * sizeof(ENT_TYPE);
    ENT_TYPE* new_array = (ENT_TYPE*)alloc_mem_rel32(array_offset + 4, data_size);

    if (new_array)
    {
        if (modified)
            VirtualFree(old_types_array, 0, MEM_RELEASE);

        memcpy(new_array, ent_types.data(), data_size);
        int32_t rel = static_cast<int32_t>((size_t)new_array - (array_offset + 4));
        write_mem_recoverable("waddler_drop", array_offset, rel, true);
        write_mem_recoverable("waddler_drop", offset, (uint8_t)ent_types.size(), true);
        modified = true;
    }
}

void modify_ankh_health_gain(uint8_t health, uint8_t beat_add_health)
{
    static size_t offsets[4];
    static const auto size_minus_one = get_address("ankh_health");
    if (!health)
    {
        recover_mem("ankh_health");
        return;
    }
    if (size_minus_one && beat_add_health)
    {
        if (!offsets[0])
        {
            auto& memory = Memory::get();
            size_t offset = size_minus_one - memory.exe_address();
            const auto limit_size = offset + 0x200;

            offsets[0] = find_inst(memory.exe(), "\x41\x80\xBF\x17\x01\x00\x00"sv, offset, limit_size, "ankh_health_gain_1");
            offsets[1] = find_inst(memory.exe(), "\x41\x80\xBF\x17\x01\x00\x00"sv, offsets[0] + 7, limit_size, "ankh_health_gain_2");
            offsets[2] = find_inst(memory.exe(), "\x0F\x42\xCA\x83\xC0"sv, offset, limit_size, "ankh_health_gain_3");
            offsets[3] = find_inst(memory.exe(), "\x8A\x83\x17\x01\x00\x00\x3C"sv, offset, std::nullopt, "ankh_health_gain_4"); // this is some bs
            if (!offsets[0] || !offsets[1] || !offsets[2] || !offsets[3])
            {
                offsets[0] = 0;
                return;
            }
            offsets[0] = memory.at_exe(offsets[0] + 7); // add pattern size
            offsets[1] = memory.at_exe(offsets[1] + 7);
            offsets[2] = memory.at_exe(offsets[2] + 5);
            offsets[3] = memory.at_exe(offsets[3] + 7);
        }
        const uint8_t game_maxhp = memory_read<uint8_t>(offsets[2] - 14);
        if (health > game_maxhp)
            health = game_maxhp;

        if (health % beat_add_health == 0)
        {
            write_mem_recoverable("ankh_health", size_minus_one, (uint8_t)(health - 1), true);
            write_mem_recoverable("ankh_health", offsets[0], health, true);
            write_mem_recoverable("ankh_health", offsets[1], health, true);
            write_mem_recoverable("ankh_health", offsets[2], beat_add_health, true);
            if (health < 4)
            {
                write_mem_recoverable("ankh_health", offsets[3], (uint8_t)0, true);
            }
            else
            {
                if (memory_read<uint8_t>(offsets[3]) != 3)
                    recover_mem("ankh_health", offsets[3]);
            }
        }
    }
}

void change_poison_timer(int16_t frames)
{
    static const size_t offset_first = get_address("first_poison_tick_timer_default");
    static const size_t offset_subsequent = get_address("subsequent_poison_tick_timer_default");

    if (frames == -1)
    {
        recover_mem("change_poison_timer");
    }
    else
    {
        write_mem_recoverable("change_poison_timer", offset_first, frames, true);
        write_mem_recoverable("change_poison_timer", offset_subsequent, frames, true);
    }
}

bool disable_floor_embeds(bool disable)
{
    static const auto address = get_address("spawn_floor_embeds");
    const bool current_value = memory_read<uint8_t>(address) == 0xc3;
    if (disable)
        write_mem_recoverable("disable_floor_embeds", address, "\xC3"sv, true);
    else
        recover_mem("disable_floor_embeds");
    return current_value;
}

void set_cursepot_ghost_enabled(bool enable)
{
    static const auto address = get_address("ghost_jar_ghost_spawn");
    if (!enable)
        write_mem_recoverable("ghost_jar_ghost_spawn", address, "\x90\x90\x90\x90\x90"sv, true);
    else
        recover_mem("ghost_jar_ghost_spawn");
}

void set_ending_unlock(ENT_TYPE type)
{
    static const ENT_TYPE first = to_id("ENT_TYPE_CHAR_ANA_SPELUNKY");
    static const ENT_TYPE last = to_id("ENT_TYPE_CHAR_CLASSIC_GUY");
    if (type >= first && type <= last)
    {
        static const auto offset = get_address("ending_unlock");
        const int32_t char_offset = 10;

        write_mem_recoverable("ending_unlock", offset, "\x90\x90\x90\x90\x90\x90\x90\x90"sv, true);
        write_mem_recoverable("ending_unlock", offset + char_offset, type, true);
    }
    else
    {
        recover_mem("ending_unlock");
    }
}

void activate_tiamat_position_hack(bool activate)
{
    static const auto code_addr = get_address("tiamat_attack_position");

    static const std::string_view code{"\xF3\x0F\x5C\xBE\x78\x01\x00\x00"sv   // subss  xmm7,DWORD PTR [rsi+0x178]
                                       "\xF3\x0F\x5C\xB6\x7C\x01\x00\x00"sv}; // subss  xmm6,DWORD PTR [rsi+0x17C]

    if (activate)
        write_mem_recoverable("activate_tiamat_position_hack", code_addr, code, true);
    else
        recover_mem("activate_tiamat_position_hack");
}

void activate_crush_elevator_hack(bool activate)
{
    auto& memory = Memory::get();
    static size_t offsets[3];
    if (offsets[0] == 0)
    {
        auto func_offset = get_virtual_function_address(VTABLE_OFFSET::ACTIVEFLOOR_CRUSHING_ELEVATOR, 78);

        offsets[0] = find_inst(memory.exe(), "\xF3\x0F\x58\xD0"sv, func_offset, func_offset + 0x80, "activate_crush_elevator_hack");
        if (offsets[0] == 0)
            return;

        offsets[0] += 4; // pattern size
        offsets[1] = find_inst(memory.exe(), "\xEB*\x0F\x57\xD2"sv, offsets[0], offsets[0] + 0xF0, "activate_crush_elevator_hack");
        if (offsets[1] == 0)
            return;

        offsets[1] += 5; // pattern size
        offsets[2] = find_inst(memory.exe(), "\xF3\x0F\x58\xC1"sv, offsets[1], offsets[1] + 0x40, "activate_crush_elevator_hack");
        if (offsets[2] == 0)
            return;

        offsets[2] += 4; // pattern size
    }

    if (activate)
    {
        write_mem_recoverable("activate_crush_elevator_hack", memory.at_exe(offsets[0]), "\x0f\x2e\x90\x30\x01\x00\x00"sv, true); // ucomiss xmm2,DWORD PTR [rax+0x130] // limit
        write_mem_recoverable("activate_crush_elevator_hack", memory.at_exe(offsets[1]), "\xf3\x0f\x10\x9b\x30\x01\x00"sv, true); // movss  xmm3,DWORD PTR [rbx+0x130]  // limit
        write_mem_recoverable("activate_crush_elevator_hack", memory.at_exe(offsets[2]), "\xf3\x0f\x58\x83\x34\x01\x00"sv, true); // addss  xmm0,DWORD PTR [rbx+0x134]  // speed
    }
    else
        recover_mem("activate_crush_elevator_hack");
}

void activate_hundun_hack(bool activate)
{
    /*
     * Pointer to Hundun entity is stored in r13 register. which means we need 8 bytes for ucomiss instruction
     * but we have 7 available, that's why we jump out to new code with the instruction and back
     */
    static size_t offsets[6]; // y_limit, y_limit, bird_head, sneak_head, speed, speed
    static char new_code[3][8];

    if (offsets[0] == 0)
    {
        auto& memory = Memory::get();
        auto func_offset = get_virtual_function_address(VTABLE_OFFSET::MONS_HUNDUN, 78);
        offsets[0] = find_inst(memory.exe(), "\x41\xF6\x85\x61\x01\x00\x00\x08"sv, func_offset, func_offset + 0x1420, "activate_hundun_hack");
        if (offsets[0] == 0)
            return;

        offsets[0] -= 13; // offset, no good pattern above
        offsets[1] = find_inst(memory.exe(), "\x41\x80\x8D\x61\x01\x00\x00\x04"sv, offsets[0], offsets[0] + 0xF40, "activate_hundun_hack");
        if (offsets[1] == 0)
        {
            offsets[0] = 0;
            return;
        }
        offsets[1] += 8; // pattern size

        offsets[2] = find_inst(memory.exe(), "\xF3\x41\x0F\x58\x45\x7C"sv, offsets[0], offsets[1], "activate_hundun_hack");
        if (offsets[2] == 0)
        {
            offsets[0] = 0;
            return;
        }
        offsets[2] += 6; // pattern size

        offsets[3] = find_inst(memory.exe(), "\xF3\x41\x0F\x58\x45\x7C"sv, offsets[2], offsets[1], "activate_hundun_hack");
        if (offsets[3] == 0)
        {
            offsets[0] = 0;
            return;
        }
        offsets[3] += 6; // pattern size

        offsets[4] = find_inst(memory.exe(), "\x83\x7A\x0C\x0E"sv, offsets[1], offsets[1] + 0xC0, "activate_hundun_hack");
        if (offsets[4] == 0)
        {
            offsets[0] = 0;
            return;
        }
        offsets[4] += 6; // pattern size plus jump

        offsets[5] = find_inst(memory.exe(), "\xF3\x41\x0F"sv, offsets[4], offsets[4] + 0x58, "activate_hundun_hack");
        if (offsets[5] == 0)
        {
            offsets[0] = 0;
            return;
        }
        offsets[5] += 9; // instruction size (didn't include the whole thing in pattern, very short distance from previous pattern)

        offsets[0] = memory.at_exe(offsets[0]);
        offsets[1] = memory.at_exe(offsets[1]);
        offsets[2] = memory.at_exe(offsets[2]);
        offsets[3] = memory.at_exe(offsets[3]);
        offsets[4] = memory.at_exe(offsets[4]);
        offsets[5] = memory.at_exe(offsets[5]);

        char old_code[3][8];

        std::memcpy(old_code[0], (void*)offsets[0], 7);
        std::memcpy(old_code[1], (void*)offsets[1], 7);
        std::memcpy(old_code[2], (void*)offsets[5], 8);

        const std::string_view patch_code{"\x41\x0F\x2E\xBD\x64\x01\x00\x00"sv};      // ucomiss xmm7,DWORD PTR [r13+0x164]
        const std::string_view speed_patch{"\xF3\x41\x0F\x58\x85\x6C\x01\x00\x00"sv}; // addss  xmm0,DWORD PTR [r13+0x16C]

        patch_and_redirect(offsets[0], 7, patch_code, true);
        patch_and_redirect(offsets[1], 7, patch_code, true);
        patch_and_redirect(offsets[5], 8, speed_patch, true);

        std::memcpy(new_code[0], (void*)offsets[0], 7);
        std::memcpy(new_code[1], (void*)offsets[1], 7);
        std::memcpy(new_code[2], (void*)offsets[5], 8);

        // writing back the old code so we can just use write_mem_recoverable for going from vanilla to the patch
        write_mem_prot(offsets[0], std::string_view{&old_code[0][0], &old_code[0][7]}, true);
        write_mem_prot(offsets[1], std::string_view{&old_code[1][0], &old_code[1][7]}, true);
        write_mem_prot(offsets[5], std::string_view{&old_code[2][0], &old_code[2][8]}, true);
    }

    if (activate)
    {
        static const std::string_view speed_code{"\x49\x8D\x95\x68\x01\x00\x00"sv                           // lea    rdx,[r13+0x168]
                                                 "\x66\x2E\x0F\x1F\x84\x00\x00\x00\x00\x00\x90\x90\x90"sv}; //  spoiled with space, all nop

        write_mem_recoverable("activate_hundun_hack", offsets[0], std::string_view{&new_code[0][0], &new_code[0][7]}, true); // limit
        write_mem_recoverable("activate_hundun_hack", offsets[1], std::string_view{&new_code[1][0], &new_code[1][7]}, true); // limit
        write_mem_recoverable("activate_hundun_hack", offsets[5], std::string_view{&new_code[2][0], &new_code[2][8]}, true); // speed for adding to the y_limit

        write_mem_recoverable("activate_hundun_hack", offsets[4], speed_code, true); // speed (for adding to the x position)

        write_mem_recoverable("activate_hundun_hack", offsets[2], "\x0F\x2E\xB8\x70\x01\x00\x00"sv, true); // ucomiss xmm7,DWORD PTR [rax+0x170] // bird_head
        write_mem_recoverable("activate_hundun_hack", offsets[3], "\x0F\x2E\xB8\x74\x01\x00\x00"sv, true); // ucomiss xmm7,DWORD PTR [rax+0x174] // snake head
    }
    else
        recover_mem("activate_hundun_hack");
}

void set_boss_door_control_enabled(bool enable)
{
    static size_t offsets[2];
    if (offsets[0] == 0)
    {
        auto& memory = Memory::get();
        offsets[0] = get_address("hundun_door_control");
        if (offsets[0] == 0)
            return;
        // find tiamat door control (the same pattern)
        offsets[1] = find_inst(memory.exe(), "\x4A\x8B\xB4\xC8\x80\xF4\x00\x00"sv, offsets[0] - memory.exe_address() + 0x777, std::nullopt, "set_boss_door_control_enabled");
        if (offsets[1] == 0)
        {
            offsets[0] = 0;
            return;
        }
        offsets[1] = function_start(memory.at_exe(offsets[1]));
    }
    if (!enable)
    {
        write_mem_recoverable("set_boss_door_control_enabled", offsets[0], "\xC3\x90"sv, true);
        write_mem_recoverable("set_boss_door_control_enabled", offsets[1], "\xC3\x90"sv, true);
    }
    else
        recover_mem("set_boss_door_control_enabled");
}

void set_level_logic_enabled(bool enable)
{
    auto state = HeapBase::get().state();
    static const size_t offset = get_virtual_function_address(state->screen_level, 1);

    if (!enable)
        write_mem_recoverable("set_level_logic_enabled", offset, "\xC3\x90"sv, true);
    else
        recover_mem("set_level_logic_enabled");
}

void set_camera_layer_control_enabled(bool enable)
{
    static const size_t offset = get_address("camera_layer_control");
    static const size_t offset2 = get_address("player_behavior_layer_switch");

    if (enable)
    {
        recover_mem("set_camera_layer_control");
    }
    else
    {
        write_mem_recoverable("set_camera_layer_control", offset, get_nop(7), true);
        write_mem_recoverable("set_camera_layer_control", offset2, get_nop(18), true);
    }
}

void set_start_level_paused(bool enable)
{
    static const size_t offset = get_address("unpause_level");
    if (enable)
        write_mem_recoverable("start_level_paused", offset, get_nop(3), true);
    else
        recover_mem("start_level_paused");
}

void set_liquid_layer(LAYER l)
{
    static std::array<uintptr_t, 7> jumps;          // jne (0F85) -> je (0F84)
    static std::array<uintptr_t, 20> layer_offsets; // 0x1300 -> 0x1308
    static std::array<uintptr_t, 6> layer_byte;
    static uintptr_t jump2;
    static uintptr_t jump3;
    if (jumps[0] == 0)
    {
        layer_byte[0] = get_address("check_if_collides_with_liquid_layer");
        layer_byte[1] = get_address("check_if_collides_with_liquid_layer2");
        layer_byte[2] = get_address("lavamander_spewing_lava");
        layer_byte[3] = get_address("movement_calculations_layer_check");
        layer_byte[4] = get_address("jump_calculations_layer_check");
        // i don't actually know what this bit does, probably bool param, it's not just liquid relates as it's for all the entities with collision mask
        // and it's not layer as well since other collision occur in back layer even with this set to 0 and vice versa
        layer_byte[5] = get_address("collision_mask_check_param");

        for (auto addr : layer_byte)
            if (addr == 0)
                return;

        auto& mem = Memory::get();
        layer_offsets[0] = get_address("spawn_liquid_layer");

        {
            auto sound_stuff = get_address("liquid_stream_spawner");
            if (sound_stuff == 0)
                return;

            auto last_offset = sound_stuff - mem.exe_address();
            bool skip = true;
            for (uint8_t idx = 0; idx < 6; ++idx)
            {
                last_offset = find_inst(mem.exe(), "\x48\x8B\x8A"sv, last_offset, last_offset + 0x170, "set_liquid_layer-sound stuff");
                if (idx == 5 && skip) // skip one, same instruction but not layer related
                {
                    idx = 4;
                    skip = false;
                    last_offset += 7;
                    continue;
                }
                layer_offsets[idx + 1] = mem.at_exe(last_offset);
                last_offset += 7;
            }
        }
        layer_offsets[7] = get_address("tidepool_impostor_spawn");
        layer_offsets[8] = get_address("tiamat_impostor_spawn");
        layer_offsets[9] = get_address("olmec_impostor_spawn");
        layer_offsets[10] = get_address("abzu_impostor_spawn");

        {
            auto logic_magman_spawn = get_virtual_function_address(VTABLE_OFFSET::LOGIC_VOLCANA_RELATED, 1);
            if (logic_magman_spawn == 0)
                return;

            auto lookup_patterns = {
                // in order
                "\x48\x8B\x8D*\x13\x00\x00"sv,
                "\x48\x03\xB7*\x13\x00\x00"sv,
                "\x48\x8B\x89*\x13\x00\x00"sv,
                "\x48\x03\x95*\x13\x00\x00"sv,
                "\x48\x03\x95*\x13\x00\x00"sv,
                "\x48\x03\x8D*\x13\x00\x00"sv,
                "\x48\x8B\x8A*\x13\x00\x00"sv,
            };
            auto current_offset = logic_magman_spawn;
            uint8_t idx = 11; // next free index
            for (auto& pattern : lookup_patterns)
            {
                current_offset = find_inst(mem.exe(), pattern, current_offset + 7, logic_magman_spawn + 0x764, "set_liquid_layer-volcana");
                if (current_offset == 0)
                    return;

                layer_offsets[idx++] = mem.at_exe(current_offset);
            }
        }
        layer_offsets[18] = get_address("logic_volcana_gather_magman_spawn_locations");
        layer_offsets[19] = get_address("logic_volcana_gather_magman_spawn_locations2");

        for (auto addr : layer_offsets)
            if (addr == 0)
                return;

        jump2 = get_address("robot_layer_check");
        jump3 = get_address("logic_underwater_bubbles_loop_check");
        if (jump2 == 0 || jump3 == 0)
            return;

        jumps[0] = get_address("layer_check_in_add_liquid_collision");
        jumps[1] = get_address("layer_check_in_remove_liquid_collision");
        jumps[2] = get_address("is_entity_in_liquid_check"); // TODO there is also layer offset nearby, test if it's related
        jumps[3] = get_address("liquid_render_layer");
        jumps[4] = get_address("entity_in_liquid_detection1");
        jumps[5] = get_address("entity_in_liquid_detection2");
        jumps[6] = get_address("layer_check_in_add_movable_liquid_collision");

        for (auto addr : jumps)
            if (addr == 0)
            {
                jumps[0] = 0;
                return;
            }
    }
    auto actual_layer = enum_to_layer(l);
    uint8_t offset_ending = actual_layer == 0 ? 0 : 8;
    uint8_t jump_oppcode = actual_layer == 0 ? 0x85 : 0x84;
    uint8_t jump_oppcode2 = actual_layer == 0 ? 0x75 : 0x74;
    uint8_t jump_oppcode2_inverse = actual_layer == 0 ? 0x74 : 0x75;

    for (auto addr : jumps)
        write_mem_prot(addr + 1, jump_oppcode, true);

    for (auto addr : layer_offsets)
        write_mem_prot(addr + 3, offset_ending, true);

    for (auto addr : layer_byte)
        write_mem_prot(addr, actual_layer, true);

    write_mem_prot(jump2, jump_oppcode2, true);
    write_mem_prot(jump3, jump_oppcode2_inverse, true);
}
