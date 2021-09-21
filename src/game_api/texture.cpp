#include "texture.hpp"

#include "memory.hpp"

Textures* get_textures()
{
    static Textures* textures_ptr = (Textures*)[]()
    {
        // Rev.Eng.: Look up string reference to "Data/Textures/", at the beginning of this function
        // there will be a pointer to the TextureDB start
        auto mem = Memory::get();
        size_t off = find_inst(mem.exe(), "\x4C\x89\xC6\x41\x89\xCF\x8B\x1D"s, mem.after_bundle);
        // Add 8 because this particular pattern points to the size qword preceding the actual table
        off = mem.at_exe(decode_pc(mem.exe(), off + 6, 2)) + 8;
        return off;
    }
    ();
    return textures_ptr;
}
