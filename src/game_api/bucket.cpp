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
    new_bucket->pause_api = new PauseAPI();
    return new_bucket;
}

PAUSE_TYPE PauseAPI::get_pause()
{
    pause = (PAUSE_TYPE)(State::get().ptr()->pause | ((uint32_t)pause & ~0x3f));
    return pause;
}

void PauseAPI::set_pause(PAUSE_TYPE flags)
{
    auto state = State::get().ptr();
    pause = flags;
    state->pause = (uint8_t)(((uint32_t)flags) & 0x3f);
}

bool PauseAPI::paused()
{
    return get_pause() != PAUSE_TYPE::NONE && (get_pause() & pause_type) != PAUSE_TYPE::NONE;
}

bool PauseAPI::set_paused(bool enable)
{
    if (enable)
        set_pause(get_pause() | pause_type);
    else
        set_pause(get_pause() & (~pause_type));
    return paused();
}

bool PauseAPI::toggle()
{
    if (paused())
        set_paused(false);
    else
        set_paused(true);
    return paused();
}

void PauseAPI::frame_advance()
{
    skip = true;
}

void PauseAPI::apply()
{
    set_pause(pause);
}
