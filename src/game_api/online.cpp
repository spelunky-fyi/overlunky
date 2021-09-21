#include "online.hpp"
#include "memory.hpp"
#include "search.hpp"

Online* get_online()
{
    // Rev.Eng.: find online code in memory (reverse for endianness), look higher up and find __vftable, set read bp on __vftable
    ONCE(Online*)
    {
        auto mem = Memory::get();
        auto tmp = find_inst(mem.exe(), "\x66\x0F\x29\x85\xE0\x03\x00\x00"s, mem.after_bundle);
        tmp = mem.at_exe(decode_pc(mem.exe(), tmp + 8));
        return res = *(Online**)tmp;
    }
}

std::string OnlineLobby::get_code()
{
    return fmt::format("{:X}", code);
}
