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

void clear_cutscene_behavior()
{
    StateMemory* state = State::get().ptr_local();
    if (state == nullptr)
        state = State::get().ptr_main();

    // loop thru entities mask 7
    const auto entities_map = &state->layers[0]->entities_by_mask;
    for (uint8_t mask = 1; mask < 7; mask <<= 1)
    {
        auto it = entities_map->find(mask);
        if (it == entities_map->end())
            continue;
        for (auto entity : it->second.entities())
        {
            auto mov = entity->as<Movable>();
            if (mov->ic8 != nullptr)
            {
                mov->ic8->~CutsceneBehavior();
                mov->ic8 = nullptr;
            }
        }
    }
    // fix the camera bound
    state->camera->bounds_bottom = 66.75;

    // some bs, don't worry about it
    state->entity_lookup->unknown3 = state->entity_lookup->unknown4;
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
        // find the jump out of olmec lookup loop
        auto jump_out_lookup = find_inst(memory.exe(), "\x48\x03\x58\x28"sv, function_offset, function_offset + 0x51C, "patch_olmec_kill_crash");
        if (jump_out_lookup == 0)
            return;

        // find first jump (skips the whole funciton)
        auto end_function_jump = find_inst(memory.exe(), "\x0F\x84"sv, function_offset, function_offset + 0xC3, "patch_olmec_kill_crash");
        if (end_function_jump == 0)
            return;

        auto end_loop_jump = memory.at_exe(jump_out_lookup + 9); // +9 to skip the pattern and some other stuff
        auto jump_addr = memory.at_exe(end_function_jump);
        auto addr_to_jump_to = jump_addr + 6 + memory_read<int32_t>(jump_addr + 2);
        std::string clear_ic8_code = fmt::format(
            "\x48\xb8{}"  // movabs RAX, &clear_cutscene_behavior
            "\xff\xd0"sv, // call   RAX
            to_le_bytes((size_t)&clear_cutscene_behavior));

        /* The idea:
         * replace end of the loop jump with jump to the new code
         * call our own function to clear all the cutscene behaviors
         * jump to the end of the function
         * hopefully there isn't something important that we're skipping
         */

        patch_and_redirect(end_loop_jump, 5, clear_ic8_code, true, addr_to_jump_to);
    }

    /* The idea:
     * if it's not the end of the array it's looking for olmec, jump back to the oryginal code via jump in `new_code`
     * if it's end of the array (no olmec found) jump to return_addr
     * the place for return_addr was kind of choosen by feel, as the code is complicated
     * the whole point of the patched code is to find olmec and check it's faze, maybe for the music?
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

void patch_liquid_OOB()
{
    /*
     * The idea:
     * there is a loop thru all liquid entities
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
        "\x0f\x8C\x00\x00\x00\x00"sv}; //   jl     (same offset as before)

    auto new_code_addr = patch_and_redirect(offset, code_to_move, new_code);
    new_code_addr += code_to_move;

    int32_t rel = static_cast<int32_t>(continue_addr - (new_code_addr + 10)); // +10 after the jump
    write_mem_prot(new_code_addr + 6, rel, true);
    write_mem_prot(new_code_addr + 16, rel - 10, true);

    once = true;
}
