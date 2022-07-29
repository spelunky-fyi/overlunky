#include "game_allocator.hpp"

#include <string> // for operator""sv

#include "search.hpp" // for get_address

using GameMallocFun = decltype(game_malloc);
using GameFreeFun = decltype(game_free);

void* game_malloc(std::size_t size)
{
    static GameMallocFun* _malloc = *(GameMallocFun**)get_address("game_malloc"sv);
    return _malloc(size);
}
void game_free(void* mem)
{
    static GameFreeFun* _free = *(GameFreeFun**)get_address("game_free"sv);
    _free(mem);
}
