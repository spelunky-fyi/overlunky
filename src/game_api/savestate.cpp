#include "savestate.hpp"

#include "memory.hpp"        // for write_mem_prot, write_mem_recoverable
#include "online.hpp"        // for Online
#include "script/events.hpp" // for pre_load_state
#include "state.hpp"         // for State, get_state_ptr, enum_to_layer

void SaveState::backup_main(int slot_to)
{
    if (pre_save_state(slot_to, get_save_state(slot_to)))
        return;

    auto base_from = HeapBase::get_main();
    auto base_to = HeapBase::get(static_cast<uint8_t>(slot_to - 1));

    pre_copy_state_event(base_from, base_to);
    base_from.copy_to(base_to);
    post_save_state(slot_to, base_to.state());
}

void SaveState::restore_main(int slot_from)
{
    if (pre_load_state(slot_from, get_save_state(slot_from)))
        return;

    auto base_from = HeapBase::get(static_cast<uint8_t>(slot_from - 1));
    auto base_to = HeapBase::get_main();

    pre_copy_state_event(base_from, base_to);
    base_from.copy_to(base_to);
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

void SaveState::load()
{
    if (base.is_null())
        return;

    auto state = base.state();
    if (pre_load_state(-1, state))
        return;
    base.copy_to(HeapBase::get_main());
    post_load_state(-1, state);
}

void SaveState::save()
{
    if (base.is_null())
        return;

    auto state = base.state();
    if (pre_save_state(-1, state))
        return;
    HeapBase::get_main().copy_to(base);
    post_save_state(-1, state);
}
