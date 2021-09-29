#include "game_allocator.hpp"

#include "memory.hpp"

using MallocFun = decltype(game_malloc);
using FreeFun = decltype(game_free);

void* game_malloc(std::size_t size)
{
    static MallocFun* _malloc = (MallocFun*)get_address("game_malloc"sv);
    return _malloc(size);
}
void game_free(void* mem)
{
    static FreeFun* _free = (FreeFun*)get_address("game_free");
    _free(mem);
}
