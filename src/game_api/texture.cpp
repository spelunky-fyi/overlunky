#include "texture.hpp"

#include <cstddef> // IWYU pragma: keep
#include <cstring>
#include <functional>
#include <list>
#include <mutex>
#include <new>
#include <type_traits>
#include <unordered_map>
#include <utility>

#include "character_def.hpp"
#include "game_api.hpp"
#include "memory.hpp"
#include "render_api.hpp"
#include "search.hpp"

Textures* get_textures()
{
    static Textures* textures_ptr = (Textures*)get_address("texture_db");
    return textures_ptr;
}

TextureDefinition get_texture_definition(TEXTURE texture_id)
{
    if (Texture* tex = get_texture(texture_id))
    {
        return TextureDefinition{
            tex->default_texture->name,
            tex->width,
            tex->height,
            static_cast<std::uint32_t>(tex->tile_width_fraction * tex->width),
            static_cast<std::uint32_t>(tex->tile_height_fraction * tex->height),
            static_cast<std::uint32_t>(tex->offset_x_weird_math * tex->width - 0.5f),
            static_cast<std::uint32_t>(tex->offset_y_weird_math * tex->height - 0.5f),
            static_cast<std::uint32_t>(tex->tile_width_fraction * tex->width * tex->num_tiles_width),
            static_cast<std::uint32_t>(tex->tile_height_fraction * tex->height * tex->num_tiles_height)};
    }
    return {};
}

Texture* get_texture(TEXTURE texture_id)
{
    if (texture_id < 0)
        return nullptr;

    auto* textures = get_textures();
    auto& render = RenderAPI::get();

    if (texture_id >= static_cast<int64_t>(textures->texture_map.size()))
    {
        std::lock_guard lock{render.custom_textures_lock};
        return &render.custom_textures[texture_id];
    }
    else if (texture_id < 0x192)
    {
        return textures->texture_map[texture_id];
    }
    return nullptr;
}

TEXTURE define_texture(TextureDefinition data)
{
    auto& render = RenderAPI::get();

    if (const std::optional<TEXTURE> existing = get_texture(data))
    {
        return existing.value();
    }

    if (data.sub_image_width == 0 || data.sub_image_height == 0)
    {
        data.sub_image_width = data.width;
        data.sub_image_height = data.height;
    }

    auto* textures = get_textures();

    Texture new_texture{
        static_cast<TEXTURE>(textures->texture_map.size() + render.custom_textures.size() + 1),
        0,
        nullptr,
        data.width,
        data.height,
        data.sub_image_width / data.tile_width,
        data.sub_image_height / data.tile_height,
        (data.sub_image_offset_x + 0.5f) / data.width,
        (data.sub_image_offset_y + 0.5f) / data.height,
        (float)data.tile_width / data.width,
        (float)data.tile_height / data.height,
        (data.tile_width - 1.0f) / data.width,
        (data.tile_height - 1.0f) / data.height,
        1.0f / data.width,
        1.0f / data.height,
    };

    std::lock_guard lock{render.custom_textures_lock};

    auto* new_texture_target = textures->texture_map[0];

    const auto backup_num_textures = textures->num_textures;
    const auto backup_texture = *new_texture_target;
    textures->num_textures = 0;
    data.texture_path = "../../" + data.texture_path;

    {
        std::string_view path{data.texture_path};
        constexpr char c_VanillaTexturePath[]{"../../Data/Textures/"};
        if (path.starts_with(c_VanillaTexturePath))
        {
            path.remove_prefix(sizeof(c_VanillaTexturePath) - 1);
            data.texture_path = path.data();
        }
    }

    // clang-format off
    using DeclareTextureFunT = void(
        uint8_t, uint32_t, const char*,
        uint32_t, uint32_t, uint32_t, uint32_t,
        uint32_t, uint32_t, uint32_t, uint32_t);
    static auto declare_texture_fun = (DeclareTextureFunT*)get_address("declare_texture"sv);
    declare_texture_fun(
        1, 0x0, data.texture_path.c_str(),
        data.width, data.height, data.tile_width, data.tile_height,
        data.sub_image_offset_x, data.sub_image_offset_y, data.sub_image_width, data.sub_image_height);
    // clang-format on

    new_texture.default_texture = new_texture_target->default_texture;
    textures->num_textures = backup_num_textures;
    *new_texture_target = backup_texture;

    render.custom_textures[new_texture.id] = new_texture;

    return new_texture.id;
}
std::optional<TEXTURE> get_texture(TextureDefinition data)
{
    auto& render = RenderAPI::get();

    if (data.sub_image_width == 0 || data.sub_image_height == 0)
    {
        data.sub_image_width = data.width;
        data.sub_image_height = data.height;
    }

    auto* textures = get_textures();

    Texture new_texture{
        static_cast<TEXTURE>(textures->texture_map.size() + render.custom_textures.size() + 1),
        0,
        nullptr,
        data.width,
        data.height,
        data.sub_image_width / data.tile_width,
        data.sub_image_height / data.tile_height,
        (data.sub_image_offset_x + 0.5f) / data.width,
        (data.sub_image_offset_y + 0.5f) / data.height,
        (float)data.tile_width / data.width,
        (float)data.tile_height / data.height,
        (data.tile_width - 1.0f) / data.width,
        (data.tile_height - 1.0f) / data.height,
        1.0f / data.width,
        1.0f / data.height,
    };
    constexpr auto compare_offset = offsetof(Texture, width);
    constexpr auto compare_size = sizeof(Texture) - offsetof(Texture, width);
    auto is_same = [](const Texture& lhs, const Texture& rhs)
    {
        // Note, even bits for floats should be the same here since all calculations are matched 1-to-1 from the games code
        return memcmp((char*)&lhs + compare_offset, (char*)&rhs + compare_offset, compare_size) == 0;
    };

    std::lock_guard lock{render.custom_textures_lock};

    for (auto& [id, texture] : render.custom_textures)
    {
        std::string_view existing_name{texture.default_texture->name};
        constexpr char c_VanillaTexturePath[]{"Data/Textures/../../"};
        if (existing_name.starts_with(c_VanillaTexturePath))
        {
            existing_name.remove_prefix(sizeof(c_VanillaTexturePath) - 1);
        }
        if (existing_name == data.texture_path && is_same(texture, new_texture))
        {
            reload_texture(texture.default_texture);
            return texture.id;
        }
    }

    for (auto& texture : textures->textures)
    {
        if (texture.default_texture != nullptr && texture.default_texture->name == data.texture_path && is_same(texture, new_texture))
        {
            reload_texture(texture.default_texture);
            return texture.id;
        }
    }

    return std::nullopt;
}
std::optional<TEXTURE> get_texture(std::string_view texture_name)
{
    // TODO: shouldn't this return multiple? since multiple "Textures" share the same image resource, which this search relays on
    auto* textures = get_textures();
    auto& render = RenderAPI::get();

    std::lock_guard lock{render.custom_textures_lock};

    for (auto& [id, texture] : render.custom_textures)
    {
        std::string_view existing_name{texture.default_texture->name};
        constexpr char c_VanillaTexturePath[]{"Data/Textures/../../"};
        if (existing_name.starts_with(c_VanillaTexturePath))
        {
            existing_name.remove_prefix(sizeof(c_VanillaTexturePath) - 1);
        }
        if (existing_name == texture_name)
        {
            reload_texture(texture.default_texture);
            return texture.id;
        }
    }

    for (auto& texture : textures->textures)
    {
        if (texture.default_texture != nullptr && texture.default_texture->name == texture_name)
        {
            reload_texture(texture.default_texture);
            return texture.id;
        }
    }

    return std::nullopt;
}

void reload_texture(const char* texture_name)
{
    {
        auto& render = RenderAPI::get();

        std::lock_guard lock{render.custom_textures_lock};
        for (auto& [id, texture] : render.custom_textures)
        {
            std::string_view existing_name{texture.default_texture->name};
            existing_name.remove_prefix(sizeof("Data/Textures/../../") - 1);
            if (existing_name == texture_name)
            {
                reload_texture(texture.default_texture);
                return;
            }
        }
    }

    std::string_view name_view{texture_name};
    auto* textures = get_textures();
    for (auto& texture : textures->textures)
    {
        if (texture.default_texture && texture.default_texture->name == name_view)
        {
            reload_texture(texture.default_texture);
            return;
        }
    }
}
void reload_texture(Resource* texture_name)
{
    using LoadTextureFunT = void(Renderer*, Resource*);

    // Find this by first finding a function that is called with "loading.DDS" as a first param
    // That function contains a single virtual call (it's done on the render) which is a call
    // to the wanted function
    static constexpr size_t c_LoadTextureVirtualIndex = 0x2E;

    auto renderer_ptr = GameAPI::get()->renderer;
    auto load_texture = *vtable_find<LoadTextureFunT*>(renderer_ptr, c_LoadTextureVirtualIndex);

    if (texture_name->dx_resource)
    {
        texture_name->dx_resource->release(); // should be safe since apparently the objects have reference counter, essentially working like shared pointer
        game_free(texture_name->dx_resource);
    }
    load_texture(renderer_ptr, texture_name);
}

void set_heart_color_from_texture(TEXTURE vanilla_id, TEXTURE custom_id)
{
    uint32_t char_index = (uint32_t)vanilla_id - 285;
    if (char_index > 19)
        return;

    auto def = get_texture_definition(vanilla_id);
    auto& render = RenderAPI::get();
    if (render.texture_colors.contains(def.texture_path))
    {
        if (!render.original_colors.contains(vanilla_id))
        {
            render.original_colors[vanilla_id] = NCharacterDB::get_character_heart_color(char_index);
        }
        NCharacterDB::set_character_heart_color(char_index, render.texture_colors[def.texture_path]);
    }
    else if (render.original_colors.contains(vanilla_id) && vanilla_id == custom_id)
    {
        NCharacterDB::set_character_heart_color(char_index, render.original_colors[vanilla_id]);
    }
    else if (custom_id >= 285 && custom_id <= 304)
    {
        uint32_t custom_index = (uint32_t)custom_id - 285;
        auto color = NCharacterDB::get_character_heart_color(custom_index);
        NCharacterDB::set_character_heart_color(char_index, color);
    }
}

bool replace_texture(TEXTURE vanilla_id, TEXTURE custom_id)
{
    auto* textures = get_textures();
    auto& render = RenderAPI::get();

    std::lock_guard lock{render.custom_textures_lock};

    if (vanilla_id >= 0 && vanilla_id < 0x192)
    {
        if (vanilla_id != custom_id && !render.original_textures.contains(vanilla_id))
        {
            render.original_textures[vanilla_id] = textures->textures[vanilla_id];
        }
        if (vanilla_id == custom_id && render.original_textures.contains(vanilla_id))
        {
            textures->textures[vanilla_id] = render.original_textures[custom_id];
            reload_texture(textures->textures[vanilla_id].default_texture);
            return true;
        }
        else if (render.custom_textures.contains(custom_id))
        {
            textures->textures[vanilla_id] = render.custom_textures[custom_id];
            textures->textures[vanilla_id].id = vanilla_id;
            reload_texture(textures->textures[vanilla_id].default_texture);
            return true;
        }
        else if (custom_id >= 0 && custom_id < 0x192)
        {
            textures->textures[vanilla_id] = textures->textures[custom_id];
            textures->textures[vanilla_id].id = vanilla_id;
            reload_texture(textures->textures[vanilla_id].default_texture);
            return true;
        }
    }

    return false;
}

bool replace_texture_and_heart_color(TEXTURE vanilla_id, TEXTURE custom_id)
{
    auto ret = replace_texture(vanilla_id, custom_id);
    if (ret)
        set_heart_color_from_texture(vanilla_id, custom_id);
    return ret;
}

void reset_texture(TEXTURE vanilla_id)
{
    replace_texture(vanilla_id, vanilla_id);
}
