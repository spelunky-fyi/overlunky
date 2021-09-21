#include "online.hpp"
#include "memory.hpp"
#include "search.hpp"

Online* get_online()
{
    // Rev.Eng.: find online code in memory, set read bp, look at register containing base, see where register is written
    ONCE(Online*)
    {
        auto mem = Memory::get();
        auto tmp = find_inst(mem.exe(), "\x4C\x8B\x35\xFA\x61\x35\x00"s, mem.after_bundle);
        tmp = mem.at_exe(decode_pc(mem.exe(), tmp));
        return res = *(Online**)tmp;
    }
}

std::string OnlineLobby::get_code()
{
    return fmt::format("{:X}", code);
}
