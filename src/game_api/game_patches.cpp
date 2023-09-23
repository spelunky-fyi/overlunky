#include "game_patches.hpp"

#include <array>
#include <cstdint>
#include <fmt/format.h>
#include <map>
#include <string>
#include <string_view>
#include <utility>

#include "entity.hpp"
#include "layer.hpp"
#include "memory.hpp"
#include "movable.hpp"
#include "search.hpp"
#include "state.hpp"
#include "state_structs.hpp"
#include "virtual_table.hpp"

void patch_orbs_limit()
{
    /*
     * The idea: we nuke jump instruction and some nop's after it
     * in new code we check if the number of orbs is greater than 3
     * if yes we set it as 3 (2 becouse of 0 - 2 range)
     * we jump back to the place of oryginal jump
     */
    static bool once = false;
    if (once)
        return;

    auto memory = Memory::get();
    const auto function_offset = get_virtual_function_address(VTABLE_OFFSET::ITEM_FLOATING_ORB, (uint32_t)VIRT_FUNC::ENTITY_KILL);
    // finding exit of the loop that counts orbs, after jump there is unused code so we have enogh space for a our patch
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
    StateMemory* state = State::get().ptr_local();
    if (state == nullptr)
        state = State::get().ptr_main();

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

void patch_olmec_kill_crash()
{
    /*
     * The idea: do what's necessary xd
     */
    static bool once = false;
    if (once)
        return;

    const auto offset = get_address("olmec_lookup_crash");
    constexpr auto code_to_move = 7;
    auto memory = Memory::get();
    size_t return_addr;
    {
        // find address to escape to

        auto rva = offset - memory.exe_ptr;
        // there are two jump that performe long jump, at the end, of it, the is 'mov rax,qword ptr ds:[rdi]', then find jump that's jumps over that code and create sound meta call
        // this is actually unique pattern
        auto jump_out_lookup = find_inst(memory.exe(), "\x48\x8B\x45\x50\x48\x83\x78\x60\x00"sv, rva, std::nullopt, "patch_olmec_kill_crash");
        if (jump_out_lookup == 0)
            return;

        // could probably just make static offset from this stuff
        auto jump_offset_offset = memory.at_exe(jump_out_lookup + 10); // 4 (lookup instruction size) + 1 (jump instruction)
        auto jump_offset = memory_read<int8_t>(jump_offset_offset);
        return_addr = jump_offset_offset + 1 + jump_offset; // +1 to get address after the jump
    }
    {
        // patch the cutscene

        const auto function_offset = get_virtual_function_address(VTABLE_OFFSET::THEME_OLMEC, 24); // spawn_effects
        const auto jump_out_lookup = get_address("olmec_lookup_in_theme");
        if (jump_out_lookup == 0)
            return;

        // find first jump (skips the whole funciton)
        auto end_function_jump = find_inst(memory.exe(), "\x0F\x84"sv, function_offset, function_offset + 0xC3, "patch_olmec_kill_crash");
        if (end_function_jump == 0)
            return;

        auto jump_addr = memory.at_exe(end_function_jump);
        auto addr_to_jump_to = jump_addr + 6 + memory_read<int32_t>(jump_addr + 2);
        std::string clear_ic8_code = fmt::format(
            "\x48\xb8{}" // movabs RAX, &clear_cutscene_behavior
            "\xff\xd0"sv // call   RAX
        );

        /* The idea:
         * replace end of the loop jump with jump to the new code
         * call our own function to clear all the cutscene behaviors
         * jump to the end of the function
         * hopefully there isn't something important that we're skipping
         */

        patch_and_redirect(jump_out_lookup, 5, clear_ic8_code, true, addr_to_jump_to);
    }

    /* The idea:
     * if it's not the end of the array it's looking for olmec, jump back to the oryginal code via jump in `new_code`
     * if it's end of the array (no olmec found) jump to return_addr
     * the place for return_addr was kind of choosen by feel, as the code is complicated
     * the whole point of the patched code is to find olmec and check it's faze, maybe for the music? no idea
     */

    std::string_view new_code{
        "\x0f\x85\x00\x00\x00\x00"sv}; //   jne (offset needs to be updated after we know the address)

    auto new_code_addr = patch_and_redirect(offset, code_to_move, new_code, false, return_addr);
    if (new_code_addr == 0)
        return;

    new_code_addr += code_to_move;
    int32_t rel = static_cast<int32_t>(offset + code_to_move - (new_code_addr + 6)); // +6 after the jump
    write_mem_prot(new_code_addr + 2, rel, true);
    once = true;
}

size_t g_tiamat_patch_size;
size_t g_tiamat_patch_addr;

void patch_tiamat_kill_crash()
{
    static bool once = false;
    if (once)
        return;

    auto memory = Memory::get();
    const auto patch_addr = get_address("tiamat_lookup_in_theme");
    if (patch_addr == 0)
        return;

    size_t return_to_addr;
    {
        // find end of the function that sets the camera and stuff
        auto rva = patch_addr - memory.exe_ptr;
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
        auto memory = Memory::get();
        auto rva = offset - memory.exe_ptr;
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
    new_code_addr += code_to_move;

    int32_t rel = static_cast<int32_t>(continue_addr - (new_code_addr + 10)); // +10 after the jump
    write_mem_prot(new_code_addr + 6, rel, true);
    write_mem_prot(new_code_addr + 16, rel - 10, true);

    once = true;
}

void set_skip_olmec_cutscene(bool skip)
{
    static const auto jump_out_lookup = get_address("olmec_lookup_in_theme");
    if (jump_out_lookup == 0)
        return;

    if (skip)
        write_mem_recoverable("set_skip_olmec_cutscene", jump_out_lookup - 2, "\x90\x90"sv, true);
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
