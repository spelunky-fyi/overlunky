#include "texture.hpp"

#include "memory.hpp"
#include "render_api.hpp"

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
            *tex->name,
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
    auto* textures = get_textures();
    auto& render = RenderAPI::get();
    if (texture_id >= static_cast<int64_t>(textures->texture_map.size()))
    {
        std::lock_guard lock{render.custom_textures_lock};
        return &render.custom_textures[texture_id];
    }
    return textures->texture_map[texture_id];
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
        static_cast<int64_t>(textures->texture_map.size() + render.custom_textures.size() + 1),
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

    new_texture.name = new_texture_target->name;
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
        static_cast<int64_t>(textures->texture_map.size() + render.custom_textures.size() + 1),
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
        std::string_view existing_name{*texture.name};
        constexpr char c_VanillaTexturePath[]{"Data/Textures/../../"};
        if (existing_name.starts_with(c_VanillaTexturePath))
        {
            existing_name.remove_prefix(sizeof(c_VanillaTexturePath) - 1);
        }
        if (existing_name == data.texture_path && is_same(texture, new_texture))
        {
            reload_texture(texture.name);
            return texture.id;
        }
    }

    for (auto& texture : textures->textures)
    {
        if (texture.name != nullptr && *texture.name == data.texture_path && is_same(texture, new_texture))
        {
            reload_texture(texture.name);
            return texture.id;
        }
    }

    return std::nullopt;
}
std::optional<TEXTURE> get_texture(std::string_view texture_name)
{
    auto* textures = get_textures();
    auto& render = RenderAPI::get();

    std::lock_guard lock{render.custom_textures_lock};

    for (auto& [id, texture] : render.custom_textures)
    {
        std::string_view existing_name{*texture.name};
        constexpr char c_VanillaTexturePath[]{"Data/Textures/../../"};
        if (existing_name.starts_with(c_VanillaTexturePath))
        {
            existing_name.remove_prefix(sizeof(c_VanillaTexturePath) - 1);
        }
        if (existing_name == texture_name)
        {
            reload_texture(texture.name);
            return texture.id;
        }
    }

    for (auto& texture : textures->textures)
    {
        if (texture.name != nullptr && *texture.name == texture_name)
        {
            reload_texture(texture.name);
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
            std::string_view existing_name{*texture.name};
            existing_name.remove_prefix(sizeof("Data/Textures/../../") - 1);
            if (existing_name == texture_name)
            {
                reload_texture(texture.name);
                return;
            }
        }
    }

    std::string_view name_view{texture_name};
    auto* textures = get_textures();
    for (auto& texture : textures->textures)
    {
        if (texture.name && *texture.name == name_view)
        {
            reload_texture(texture.name);
            return;
        }
    }
}
void reload_texture(const char** texture_name)
{
    class Renderer;
    using LoadTextureFunT = void(Renderer*, const char**);

    auto& render = RenderAPI::get();

    // Find this by first finding a function that is called with "loading.DDS" as a first param
    // That function contains a single virtual call (it's done on the render) which is a call
    // to the wanted function
    static constexpr size_t c_LoadTextureVirtualIndex = 0x2E;

    auto renderer_ptr = (Renderer*)render.renderer();
    auto load_texture = *vtable_find<LoadTextureFunT*>(renderer_ptr, c_LoadTextureVirtualIndex);
    load_texture(renderer_ptr, texture_name);
}