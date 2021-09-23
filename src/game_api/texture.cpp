#include "texture.hpp"

#include "memory.hpp"

Textures* get_textures()
{
    static Textures* textures_ptr = (Textures*)get_address("texture_db");
    return textures_ptr;
}
