#include "bucket.hpp"

#include "containers/game_allocator.hpp"
#include "memory.hpp"
#include "state.hpp"

Bucket* Bucket::get()
{
    static auto bucket_offset = get_address("hundun_door_control") - 12; // another random CCCC with some space for a pointer
    auto bucket = reinterpret_cast<Bucket*>(memory_read<size_t>(bucket_offset));
    if ((size_t)bucket != 0xCCCCCCCCCCCCCCCC)
        return bucket;
    auto new_bucket = new Bucket();
    write_mem_prot(bucket_offset, new_bucket, true);
    return new_bucket;
}

PAUSE_TYPE PauseAPI::get_pause()
{
    return (PAUSE_TYPE)(State::get().ptr()->pause | ((uint32_t)pause & ~0x3f));
}

void PauseAPI::set_pause(PAUSE_TYPE flags)
{
    auto state = State::get().ptr();
    pause = flags;
    state->pause = (uint8_t)(((uint32_t)flags) & 0x3f);
}
