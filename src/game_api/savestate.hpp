#pragma once

#include <cstdint> // for uint32_t, int8_t

#include "heap_base.hpp" // for HeapBase

struct StateMemory;
struct PRNG;

class SaveState
{
  public:
    /// Create a new temporary SaveState/clone of the main level state. Unlike save_state slots that are preallocated by the game anyway, these will use 32MiB a pop and aren't freed automatically, so make sure to clear them or reuse the same one to save memory. The garbage collector will eventually clear the SaveStates you don't have a handle to any more though.
    SaveState()
        : base(reinterpret_cast<uintptr_t>(malloc(8ull * 0x400000)))
    {
        save();
    }
    /// NoDoc
    SaveState(uint8_t index)
        : base(HeapBase::get(index)){};
    ~SaveState()
    {
        clear();
    }
    /// Get the pre-allocated by the game save slot 1-4. Call as `SaveState.get(slot)`
    static SaveState get(int save_slot)
    {
        int8_t index = static_cast<int8_t>(save_slot - 1);
        SaveState save_from_slot(index);
        save_from_slot.slot = index;
        return save_from_slot;
    }

    /// Access the StateMemory inside a SaveState
    StateMemory* get_state() const
    {
        return base.state();
    }
    /// Get the current frame from the SaveState, equivelent to the [get_frame](#Get_frame) global function that returns the frame from the "loaded in state"
    uint32_t get_frame() const
    {
        return base.frame_count();
    }
    /// Access the PRNG inside a SaveState
    PRNG* get_prng() const
    {
        return base.prng();
    }

    /// Load a SaveState
    void load();

    /// Save over a previously allocated SaveState
    void save();

    /// Delete the SaveState and free the memory. The SaveState can't be used after this.
    void clear()
    {
        if (slot != -1)
            return;

        base.free();
    }
    static void backup_main(int slot_to);
    static void restore_main(int from_slot);

  private:
    HeapBase base;
    int8_t slot{-1};
};

StateMemory* get_save_state(int slot);
void invalidate_save_slots();
