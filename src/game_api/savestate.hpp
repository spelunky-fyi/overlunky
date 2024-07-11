#pragma once

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
    void clear();

  private:
    size_t addr;
};

void copy_save_slot(int from, int to);
void copy_state(size_t fromBaseState, size_t toBaseState);
StateMemory* get_save_state(int slot);
void invalidate_save_slots();
