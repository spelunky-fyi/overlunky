#include "render_api.hpp"

#include <cstddef>
#include <detours.h>
#include <string>

#include "memory.hpp"
#include "script/lua_backend.hpp"
#include "texture.hpp"

size_t* find_api(Memory memory)
{
    ONCE(size_t*)
    {
        auto exe = memory.exe();
        auto after_bundle = memory.after_bundle;
        auto off = find_inst(exe, "\x48\x8B\x50\x10\x48\x89"s, after_bundle) - 5;
        off = off + (*(int32_t*)(&exe[off + 1])) + 5;

        return res = (size_t*)memory.at_exe(decode_pc(exe, off + 6));
    }
}

size_t get_load_texture()
{
    ONCE(size_t)
    {
        auto memory = Memory::get();
        auto exe = memory.exe();
        auto after_bundle = memory.after_bundle;
        auto off = find_inst(exe, "\x75\xf7\x48\x8d\x54\x24\x40\x48"s, after_bundle);
        off = find_inst(exe, "\xe8"s, off);
        off = find_inst(exe, "\xe8"s, off + 1);
        return res = memory.at_exe(decode_pc(exe, off, 1));
    }
}

RenderAPI& RenderAPI::get()
{
    static RenderAPI render_api = []()
    {
        auto memory = Memory::get();
        auto api = (find_api(memory));
        auto off = decode_imm(memory.exe(), find_inst(memory.exe(), "\xBA\xF0\xFF\xFF\xFF\x41\xB8\x00\x00\x00\x90"s, memory.after_bundle) + 17);

        return RenderAPI{api, off};
    }();
    return render_api;
}

TextureDefinition RenderAPI::get_texture_definition(std::uint32_t texture_id)
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

Texture* RenderAPI::get_texture(std::uint64_t texture_id)
{
    auto* textures = get_textures();
    if (texture_id >= textures->texture_map.size())
    {
        return &custom_textures[texture_id];
    }
    return textures->texture_map[texture_id];
}

std::uint64_t RenderAPI::define_texture(TextureDefinition data)
{

    if (data.sub_image_width == 0 || data.sub_image_height == 0)
    {
        data.sub_image_width = data.width;
        data.sub_image_height = data.height;
    }

    auto* textures = get_textures();
    Texture new_texture{
        textures->texture_map.size() + custom_textures.size() + 1,
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

    for (auto& [id, texture] : custom_textures)
    {
        if (*texture.name == data.texture_path && is_same(texture, new_texture))
        {
            return texture.id;
        }
    }

    for (auto& texture : textures->textures)
    {
        if (texture.name != nullptr && *texture.name == data.texture_path && is_same(texture, new_texture))
        {
            return texture.id;
        }
    }

    new_texture.name = load_texture(std::move(data.texture_path));
    custom_textures[new_texture.id] = new_texture;

    return new_texture.id;
}

using LoadTextureFunT = const char**(void*, std::string*, std::uint8_t);
const char** RenderAPI::load_texture(std::string file_name)
{
    void* render_api = (void*)renderer();
    auto load_texture = (LoadTextureFunT*)get_load_texture();
    return load_texture(render_api, &file_name, 1);
}

using VanillaRenderHudFun = void(void*, float, float);
VanillaRenderHudFun* g_render_hud_trampoline{nullptr};
void render_hud(void* hud_data, float y, float opacity)
{
    g_render_hud_trampoline(hud_data, y, opacity);

    LuaBackend::for_each_backend(
        [&](LuaBackend& backend)
        {
            backend.vanilla_render();
            return true;
        });
}

void init_render_hud_hook()
{
    static size_t func_render_hud = 0;

    auto& memory = Memory::get();
    auto exe = memory.exe();

    if (func_render_hud == 0)
    {
        std::string pattern = "\x48\x8D\x0D\x56\x05\x16\x00"s;
        size_t pattern_pos = find_inst(exe, pattern, memory.after_bundle);
        func_render_hud = function_start(memory.at_exe(pattern_pos));
    }

    if (func_render_hud != 0)
    {
        DetourRestoreAfterWith();

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());

        g_render_hud_trampoline = (VanillaRenderHudFun*)func_render_hud;
        DetourAttach((void**)&g_render_hud_trampoline, (VanillaRenderHudFun*)render_hud);

        const LONG error = DetourTransactionCommit();
        if (error != NO_ERROR)
        {
            DEBUG("Failed hooking render_hud: {}\n", error);
        }
    }
}
