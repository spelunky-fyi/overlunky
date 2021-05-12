#include "game_manager.hpp"
#include "memory.hpp"
#include "search.hpp"

GameManager* get_game_manager()
{
    ONCE(GameManager*)
    {
        auto mem = Memory::get();
        auto tmp = find_inst(mem.exe(), "\xC6\x80\x39\x01\x00\x00\x00\x48"s, mem.after_bundle);
        tmp = mem.at_exe(decode_pc(mem.exe(), tmp + 7));
        return res = *(GameManager**)tmp;
    }
}