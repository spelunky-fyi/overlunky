#pragma once

#include "thread_utils.hpp"

struct StateMemory;

class SaveState
{
  public:
    /// Create a new temporary SaveState/clone of the main level state. Unlike save_state slots that are preallocated by the game anyway, these will use 32MiB a pop and aren't freed automatically, so make sure to clear them or reuse the same one to save memory. The garbage collector will eventually clear the SaveStates you don't have a handle to any more though.
    SaveState();
    ~SaveState()
    {
        clear();
    }

    /// Access the StateMemory inside a SaveState
    StateMemory* get_state() const;

    /// Load a SaveState
    void load();

    /// Save over a previously allocated SaveState
    void save();

    /// Delete the SaveState and free the memory. The SaveState can't be used after this.
    void clear()
    {
        base.free();
    }

  private:
    HeapBase base;
};

void save_main_heap(int slot_to);
void load_main_heap(int slot_from);
StateMemory* get_save_state(int slot);
void invalidate_save_slots();
