#include "savestate.hpp"

#include "memory.hpp"        // for write_mem_prot, write_mem_recoverable
#include "online.hpp"        // for Online
#include "script/events.hpp" // for pre_load_state
#include "state.hpp"         // for State, get_state_ptr, enum_to_layer

void copy_state(HeapBase from, HeapBase to)
{
    if (from.is_null() || to.is_null())
        return;

    auto fromBaseState = from.address();
    auto toBaseState = to.address();
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

// void copy_save_slot(uint8_t from, uint8_t to)
//{
//     if ((from == 5 && pre_save_state(to, get_save_state(to))) ||
//         (to == 5 && pre_load_state(from, get_save_state(from))))
//         return;
//
//     auto base_from = HeapBase::get(from - 1);
//     auto base_to = HeapBase::get(to - 1);
//
//     pre_copy_state_event(get_save_state_raw(from), get_save_state_raw(to));
//     copy_state(HeapBase::get(from - 1), HeapBase::get(to - 1));
//     if (from == 5)
//         post_save_state(to, get_save_state(to));
//     else if (to == 5)
//         post_load_state(from, get_save_state(from));
// };

void save_main_heap(int slot_to)
{
    if (pre_save_state(slot_to, get_save_state(slot_to)))
        return;

    auto base_from = HeapBase::get_main();
    auto base_to = HeapBase::get(static_cast<uint8_t>(slot_to - 1));

    pre_copy_state_event(base_from.state(), base_to.state());
    copy_state(base_from, base_to);
    post_save_state(slot_to, base_to.state());
}

void load_main_heap(int slot_from)
{
    if (pre_load_state(slot_from, get_save_state(slot_from)))
        return;

    auto base_from = HeapBase::get(static_cast<uint8_t>(slot_from - 1));
    auto base_to = HeapBase::get_main();

    pre_copy_state_event(base_from.state(), base_to.state());
    copy_state(base_from, base_to);
    post_load_state(slot_from, base_from.state());
}

StateMemory* get_save_state(int slot)
{
    auto state = HeapBase::get(static_cast<uint8_t>(slot - 1)).state();
    if (state->screen)
        return state;
    return nullptr;
}

void invalidate_save_slots()
{
    if (get_online()->is_active())
        return;
    for (int i = 1; i <= 4; ++i)
    {
        auto state = get_save_state(i);
        if (state)
            state->screen = 0;
    }
}

SaveState::SaveState()
    : base(reinterpret_cast<uintptr_t>(malloc(8ull * 0x400000)))
{
    save();
}

StateMemory* SaveState::get_state() const
{
    if (base.is_null())
        return nullptr;
    return base.state();
}

void SaveState::load()
{
    if (base.is_null())
        return;

    auto state = base.state();
    if (pre_load_state(-1, state))
        return;
    copy_state(base, HeapBase::get_main());
    post_load_state(-1, state);
}

void SaveState::save()
{
    if (base.is_null())
        return;

    auto state = base.state();
    if (pre_save_state(-1, state))
        return;
    copy_state(HeapBase::get_main(), base);
    post_save_state(-1, state);
}
