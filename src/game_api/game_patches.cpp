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

    // simple jump over the tiamat check, nop here just so there is no funny business
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
