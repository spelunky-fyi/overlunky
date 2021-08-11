#include "texture.hpp"

#include "memory.hpp"

Textures* get_textures()
{
    static Textures* textures_ptr = (Textures*)[]()
    {
        auto mem = Memory::get();
        size_t off = find_inst(mem.exe(), "\x75\x06\x41\x8b\xec\x41\x8b\xf7"s, mem.after_bundle);
        off = find_inst(mem.exe(), "\x48\x8d"s, off);
        off = mem.at_exe(decode_pc(mem.exe(), off) - 8);
        return off;
    }
    ();
    return textures_ptr;
}
