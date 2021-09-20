#include "game_allocator.hpp"

#include "memory.hpp"

using MallocFun = decltype(game_malloc);
using FreeFun = decltype(game_free);

MallocFun* get_malloc()
{
    static MallocFun* spel2_malloc = []()
    {
        auto memory = Memory::get();
        auto exe = memory.exe();
        auto after_bundle = memory.after_bundle;

        auto off = find_inst(exe, "\x45\x84\xe4\x0f\x84"s, after_bundle) - 0x10;
        auto fun_start = decode_pc(exe, find_inst(exe, "\xff\x15"s, off), 2);

        return *(MallocFun**)memory.at_exe(fun_start);
    }();
    return spel2_malloc;
}
FreeFun* get_free()
{
    static FreeFun* spel2_free = []()
    {
        auto memory = Memory::get();
        auto exe = memory.exe();
        auto after_bundle = memory.after_bundle;

        auto off = find_inst(exe, "\x48\x83\x7e\x18\x00"s, after_bundle) - 0x10;
        auto fun_start = decode_pc(exe, find_inst(exe, "\xff\x15"s, off), 2);

        return *(FreeFun**)memory.at_exe(fun_start);
    }();
    return spel2_free;
}

void* game_malloc(std::size_t size)
{
    return get_malloc()(size);
}
void game_free(void* mem)
{
    get_free()(mem);
}
