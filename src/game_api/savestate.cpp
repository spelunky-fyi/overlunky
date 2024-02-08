#include "savestate.hpp"

#include "memory.hpp" // for write_mem_prot, write_mem_recoverable
#include "online.hpp" // for Online
#include "state.hpp"  // for State, get_state_ptr, enum_to_layer

void copy_save_slot(int from, int to)
{
    size_t arr = get_address("save_states");
    size_t fromBaseState = memory_read<size_t>(arr + (from - 1) * 8);
    size_t toBaseState = memory_read<size_t>(arr + (to - 1) * 8);
    copy_state(fromBaseState, toBaseState);
};

void copy_state(size_t fromBaseState, size_t toBaseState)
{
    size_t iterIdx = 1;
    do
    {
        size_t copyContent = *(size_t*)((fromBaseState - 8) + iterIdx * 8);
        // variable used to fix pointers that point somewhere in the same Thread
        size_t diff = toBaseState - fromBaseState;
        if (copyContent >= fromBaseState + 0x2000000 || copyContent <= fromBaseState)
        {
            diff = 0;
        }
        *(size_t*)(toBaseState + iterIdx * 8 + -8) = diff + copyContent;

        // Almost same code as before, but on the next value, idk why
        copyContent = *(size_t*)(fromBaseState + iterIdx * 8);
        diff = toBaseState - fromBaseState;
        if (copyContent >= fromBaseState + 0x2000000 || copyContent <= fromBaseState)
        {
            diff = 0;
        }
        *(size_t*)(toBaseState + iterIdx * 8) = diff + copyContent;

        iterIdx = iterIdx + 2;
    } while (iterIdx != 0x400001);
};

StateMemory* get_save_state(int slot)
{
    size_t arr = get_address("save_states");
    size_t base = memory_read<size_t>(arr + (slot - 1) * 8);
    auto state = reinterpret_cast<StateMemory*>(base + 0x4a0);
    if (state->screen)
        return state;
    return nullptr;
}

void invalidate_save_slots()
{
    auto online = get_online();
    if (online->lobby.code != 0)
        return;
    for (int i = 1; i <= 4; ++i)
    {
        auto state = get_save_state(i);
        if (state)
            state->screen = 0;
    }
}

SaveState::SaveState()
{
    size_t from = (size_t)(State::get().ptr_main()) - 0x4a0;
    addr = (size_t)malloc(8 * 0x400000);
    if (addr)
        copy_state(from, addr);
}

SaveState::~SaveState()
{
    clear();
}

StateMemory* SaveState::get_state()
{
    if (!addr)
        return nullptr;
    return reinterpret_cast<StateMemory*>(addr + 0x4a0);
}

void SaveState::load()
{
    if (!addr)
        return;
    size_t to = (size_t)(State::get().ptr_main()) - 0x4a0;
    copy_state(addr, to);
}

void SaveState::save()
{
    if (!addr)
        return;
    size_t from = (size_t)(State::get().ptr_main()) - 0x4a0;
    copy_state(from, addr);
}

void SaveState::clear()
{
    if (!addr)
        return;
    free((void*)addr);
    addr = 0;
}
