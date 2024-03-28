#include "savestate.hpp"

#include "memory.hpp"        // for write_mem_prot, write_mem_recoverable
#include "online.hpp"        // for Online
#include "script/events.hpp" // for pre_load_state
#include "state.hpp"         // for State, get_state_ptr, enum_to_layer

void copy_save_slot(int from, int to)
{
    if ((from == 5 && pre_save_state(to, get_save_state(to))) ||
        (to == 5 && pre_load_state(from, get_save_state(from))))
        return;
    size_t arr = get_address("save_states");
    State* fromBaseState = memory_read<State*>(arr + (from - 1) * 8);
    State* toBaseState = memory_read<State*>(arr + (to - 1) * 8);
    copy_state(fromBaseState, toBaseState);
    if (from == 5)
        post_save_state(to, get_save_state(to));
    else if (to == 5)
        post_load_state(from, get_save_state(from));
};

void copy_state(State* fromBaseStatePtr, State* toBaseStatePtr)
{
    size_t fromBaseState = reinterpret_cast<size_t>(fromBaseStatePtr);
    size_t toBaseState = reinterpret_cast<size_t>(toBaseStatePtr);
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

State* get_save_state(int slot)
{
    size_t arr = get_address("save_states");
    State* state = memory_read<State*>(arr + (slot - 1) * 8);
    if (state->state.screen)
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
            state->state.screen = 0;
    }
}

SaveState::SaveState()
{
    addr = static_cast<State*>(malloc(8 * 0x400000));
    save();
}

SaveState::~SaveState()
{
    clear();
}

StateMemory* SaveState::get_state()
{
    return &addr->state;
}

void SaveState::load()
{
    if (!addr)
        return;
    auto to = State::get_main();
    auto state = addr;
    if (pre_load_state(-1, state))
        return;
    copy_state(addr, to);
    post_load_state(-1, state);
}

void SaveState::save()
{
    if (!addr)
        return;
    auto from = State::get_main();
    auto state = addr;
    if (pre_save_state(-1, state))
        return;
    copy_state(from, addr);
    post_save_state(-1, state);
}

void SaveState::clear()
{
    if (!addr)
        return;
    free((void*)addr);
    addr = 0;
}
