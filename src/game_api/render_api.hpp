#include "memory.hpp"

struct RenderAPI
{
    const size_t* api;
    size_t swap_chain_off;

    static RenderAPI get();

    size_t renderer() const
    {
        return read_u64(*api + 0x10);
    }

    size_t swap_chain() const
    {
        return read_u64(renderer() + swap_chain_off);
    }
};
