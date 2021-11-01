#include "texture_lua.hpp"

#include "file_api.hpp"
#include "render_api.hpp"
#include "script/lua_backend.hpp"
#include "texture.hpp"

#include <sol/sol.hpp>

namespace NTexture
{
void register_usertypes(sol::state& lua)
{
    /// Gets a `TextureDefinition` for equivalent to the one used to define the texture with `id`
    lua["get_texture_definition"] = [](TEXTURE texture_id) -> TextureDefinition
    {
        return RenderAPI::get().get_texture_definition(texture_id);
    };
    /// Defines a new texture that can be used in Entity::set_texture
    lua["define_texture"] = [](TextureDefinition texture_data) -> TEXTURE
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        texture_data.texture_path = get_image_file_path(backend->get_root(), std::move(texture_data.texture_path));
        return RenderAPI::get().define_texture(std::move(texture_data));
    };

    /// Use `TextureDefinition.new()` to get a new instance to this and pass it to define_entity_texture.
    /// `width` and `height` always have to be the size of the image file. They should be divisible by `tile_width` and `tile_height` respectively.
    /// `tile_width` and `tile_height` define the size of a single tile, the image will automatically be divided into these tiles.
    /// Tiles are labeled in sequence starting at the top left, going right and down at the end of the image (you know, like sentences work in the English language). Use those numbers in `Entity::animation_frame`.
    /// `sub_image_offset_x`, `sub_image_offset_y`, `sub_image_width` and `sub_image_height` can be used if only a part of the image should be used. Leave them at zero to ignore this.
    lua.new_usertype<TextureDefinition>(
        "TextureDefinition",
        "texture_path",
        &TextureDefinition::texture_path,
        "width",
        &TextureDefinition::width,
        "height",
        &TextureDefinition::height,
        "tile_width",
        &TextureDefinition::tile_width,
        "tile_height",
        &TextureDefinition::tile_height,
        "sub_image_offset_x",
        &TextureDefinition::sub_image_offset_x,
        "sub_image_offset_y",
        &TextureDefinition::sub_image_offset_y,
        "sub_image_width",
        &TextureDefinition::sub_image_width,
        "sub_image_height",
        &TextureDefinition::sub_image_height);

    lua.create_named_table("TEXTURE"
                           //, "DATA_TEXTURES_PLACEHOLDER_0", 0
                           //, "", ...check__[textures.txt]\[game_data/textures.txt\]...
                           //, "DATA_TEXTURES_SHINE_0", 388
                           //, "DATA_TEXTURES_OLDTEXTURES_AI_0", 389
    );
    {
        std::unordered_map<std::string, uint32_t> counts;
        for (const auto* tex : get_textures()->texture_map)
        {
            if (tex != nullptr && tex->name != nullptr)
            {
                std::string clean_tex_name = *tex->name;
                std::transform(
                    clean_tex_name.begin(), clean_tex_name.end(), clean_tex_name.begin(), [](unsigned char c)
                    { return (unsigned char)std::toupper(c); });
                std::replace(clean_tex_name.begin(), clean_tex_name.end(), '/', '_');
                size_t index = clean_tex_name.find(".DDS", 0);
                if (index != std::string::npos)
                {
                    clean_tex_name.erase(index, 4);
                }
                clean_tex_name += '_' + std::to_string(counts[clean_tex_name]++);
                lua["TEXTURE"][clean_tex_name] = tex->id;
            }
        }
    }
}
}; // namespace NTexture
