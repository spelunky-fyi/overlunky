#include "texture_lua.hpp"

#include <algorithm>   // for max, replace, transform
#include <array>       // for _Array_iterator, array, _Array_con...
#include <cctype>      // for toupper
#include <cstddef>     // for size_t
#include <cstdint>     // for uint32_t
#include <functional>  // for equal_to
#include <new>         // for operator new
#include <optional>    // for optional
#include <sol/sol.hpp> // for global_table, proxy_key_t, data_t
#include <string>      // for string, allocator, hash, operator==
#include <string_view>
#include <tuple>         // for get
#include <type_traits>   // for move, declval
#include <unordered_map> // for _Umap_traits<>::allocator_type
#include <utility>       // for min, max

#include "aliases.hpp"            // for TEXTURE
#include "file_api.hpp"           // for get_image_file_path
#include "script/lua_backend.hpp" // for LuaBackend
#include "texture.hpp"            // for TextureDefinition, get_texture

namespace NTexture
{
void resolve_path(std::string& path)
{
    auto backend = LuaBackend::get_calling_backend();
    static const std::string prefix = "Data/Textures/../../";
    if (path.size() > prefix.size() && path.substr(0, prefix.size()) == prefix)
    {
        path = path.substr(prefix.size());
    }
    if (path.starts_with("/"))
    {
        path = path.substr(1);
        path = get_image_file_path(std::filesystem::absolute(".").string().c_str(), std::move(path));
    }
    else if (path[1] != ':')
    {
        path = get_image_file_path(backend->get_root(), std::move(path));
    }
}

void register_usertypes(sol::state& lua)
{
    /// Gets a `TextureDefinition` for equivalent to the one used to define the texture with `id`
    lua["get_texture_definition"] = [](TEXTURE texture_id) -> TextureDefinition
    {
        return get_texture_definition(texture_id);
    };
    /// Defines a new texture that can be used in Entity::set_texture
    /// If a texture with the same definition already exists the texture will be reloaded from disk.
    lua["define_texture"] = [](TextureDefinition texture_data) -> TEXTURE
    {
        resolve_path(texture_data.texture_path);
        return define_texture(std::move(texture_data));
    };
    /// Gets a texture with the same definition as the given, if none exists returns `nil`
    lua["get_texture"] = [](TextureDefinition texture_data) -> std::optional<TEXTURE>
    {
        resolve_path(texture_data.texture_path);
        return get_texture(std::move(texture_data));
    };
    /// Gets the first texture with the matching path, if none exists returns `nil`
    lua["get_texture"] = [](std::string texture_path) -> std::optional<TEXTURE>
    {
        resolve_path(texture_path);
        return get_texture(texture_path);
    };
    /// Reloads a texture from disk, use this only as a development tool for example in the console
    /// Note that [define_texture](#define_texture) will also reload the texture if it already exists
    lua["reload_texture"] = [](std::string texture_path)
    {
        resolve_path(texture_path);
        return reload_texture(texture_path.c_str());
    };
    /// Replace a vanilla texture definition with a custom texture definition and reload the texture.
    lua["replace_texture"] = replace_texture;
    /// Reset a replaced vanilla texture to the original and reload the texture.
    lua["reset_texture"] = reset_texture;
    /// Replace a vanilla texture definition with a custom texture definition and reload the texture. Set corresponding character heart color to the pixel in the center of the player indicator arrow in that texture. (448,1472)
    lua["replace_texture_and_heart_color"] = replace_texture_and_heart_color;
    /// Clear cache for a file path or the whole directory
    lua["clear_cache"] = sol::overload(
        []()
        {
            auto backend = LuaBackend::get_calling_backend();
            if (backend->get_unsafe())
                clear_cache();
        },
        [](std::string path)
        {
            if (path == "")
                return;
            resolve_path(path);
            clear_cache(path);
        });

    /// Use `TextureDefinition.new()` to get a new instance to this and pass it to [define_texture](#define_texture).
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
