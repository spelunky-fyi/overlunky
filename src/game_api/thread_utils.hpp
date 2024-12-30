#pragma once

#include <cstddef>  // for size_t
#include <cstdint>  // for int64_t
#include <stdlib.h> // for free

struct PRNG;
struct StateMemory;
struct LevelGenSystem;
struct LiquidPhysics;

struct HeapBase
{
    static HeapBase get(uint8_t slot);
    static HeapBase get_main();
    // can be NULL
    static HeapBase get_local();
    // fallback to main if can't get local
    static HeapBase get_local_safe()
    {
        auto local = get_local();
        return local.is_null() ? get_main() : local;
    }

    bool is_null() const noexcept
    {
        return ptr == NULL;
    }
    uintptr_t address() const noexcept
    {
        return ptr;
    }
    StateMemory* state() const noexcept
    {
        if (is_null())
            return nullptr;

        return reinterpret_cast<StateMemory*>(ptr + GAME_OFFSET::STATE);
    }
    size_t frame() const noexcept
    {
        if (is_null())
            return NULL;

        return *reinterpret_cast<size_t*>(ptr + GAME_OFFSET::FRAME_COUNTER);
    }
    PRNG* prng() const noexcept
    {
        if (is_null())
            return nullptr;

        return reinterpret_cast<PRNG*>(ptr + GAME_OFFSET::_PRNG);
    }
    LevelGenSystem* level_gen() const noexcept
    {
        if (is_null())
            return nullptr;

        return reinterpret_cast<LevelGenSystem*>(ptr + GAME_OFFSET::LEVEL_GEN);
    }
    LiquidPhysics* liquid_physics() const noexcept
    {
        if (is_null())
            return nullptr;

        return reinterpret_cast<LiquidPhysics*>(ptr + GAME_OFFSET::LIQUID_ENGINE);
    }

  protected:
    HeapBase(uintptr_t addr) noexcept
        : ptr(addr){};

    void free()
    {
        if (ptr != NULL)
            ::free(reinterpret_cast<void*>(ptr));

        ptr = NULL;
    }

  private:
    uintptr_t ptr{NULL};

    enum GAME_OFFSET : size_t
    {
        UNKNOWN1 = 0x8,          // - ?
        MALLOC = 0x20,           // - custom malloc base
        FRAME_COUNTER = 0x3D0,   // - FRAME_COUNTER
        _PRNG = 0x3F0,           // - PRNG
        STATE = 0x4A0,           // - State Memory
        LEVEL_GEN = 0xD7B30,     // - level gen
        LIQUID_ENGINE = 0xD8650, // - liquid physics
        UNKNOWN3 = 0x108420,     // - some vector?
    };
    static const uint8_t MAX_SAVE_SLOTS = 5;
    friend class SaveState;
};

// Used for objects that are allocated with the game's custom allocator
template <typename T>
class OnHeapPointer
{
  public:
    explicit OnHeapPointer(uint64_t ptr)
        : ptr_(ptr){};

    T* decode() const
    {
        return reinterpret_cast<T*>(ptr_ + HeapBase::get_main().address());
    }

    T* decode_local() const
    {
        auto lhb = HeapBase::get_local();
        if (lhb.is_null())
            return nullptr;

        return reinterpret_cast<T*>(ptr_ + lhb.address());
    }

    T* decode_local_safe() const
    {
        auto lhb = HeapBase::get_local();
        if (lhb.is_null())
            lhb = HeapBase::get_main();

        return reinterpret_cast<T*>(ptr_ + lhb.address());
    }

  private:
    uint64_t ptr_;
};
