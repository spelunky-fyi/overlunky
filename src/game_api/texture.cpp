#include "texture.hpp"

#include "memory.hpp"

size_t textures_ptr()
{
    static size_t cached_textures_ptr;
    ONCE(size_t)
    {
        auto mem = Memory::get();
        res = mem.at_exe(decode_pc(
                  mem.exe(), find_inst(mem.exe(), "\x48\x8d"s, find_inst(mem.exe(), "\x75\x06\x41\x8b\xec\x41\x8b\xf7"s, mem.after_bundle)))) -
              8;
        return res;
    }
}

Textures *get_textures()
{
    return (Textures *)textures_ptr();
}
