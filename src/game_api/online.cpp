#include "online.hpp"
#include "memory.hpp"
#include "search.hpp"

Online* get_online()
{
    ONCE(Online*)
    {
        auto mem = Memory::get();
        auto tmp = find_inst(mem.exe(), "\xF3\x0F\x11\x4C\x24\x10\x56\x57"s, mem.after_bundle);
        tmp = mem.at_exe(decode_pc(mem.exe(), tmp + 24));
        return res = *(Online**)tmp;
    }
}

std::string OnlineLobby::get_code()
{
    return fmt::format("{:X}", code);
}
