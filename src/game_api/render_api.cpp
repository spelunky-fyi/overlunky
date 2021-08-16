#include "render_api.hpp"

#include <cstddef>
#include <detours.h>
#include <string>

#include "layer.hpp"
#include "memory.hpp"
#include "script/events.hpp"
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
        std::lock_guard lock{custom_textures_lock};
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

    std::lock_guard lock{custom_textures_lock};

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

using VanillaRenderHudFun = void(size_t, float, float, size_t);
VanillaRenderHudFun* g_render_hud_trampoline{nullptr};
void render_hud(size_t hud_data, float y, float opacity, size_t hud_data2)
{
    // hud_data and hud_data2 are the same pointer, but the second one is actually used (displays garbage if not passed)
    trigger_vanilla_render_callbacks(ON::RENDER_PRE_HUD);
    g_render_hud_trampoline(hud_data, y, opacity, hud_data2);
    trigger_vanilla_render_callbacks(ON::RENDER_POST_HUD);
}

using VanillaRenderPauseMenuFun = void(float*);
VanillaRenderPauseMenuFun* g_render_pause_menu_trampoline{nullptr};
void render_pause_menu(float* drawing_info)
{
    trigger_vanilla_render_callbacks(ON::RENDER_PRE_PAUSE_MENU);
    g_render_pause_menu_trampoline(drawing_info);
    trigger_vanilla_render_callbacks(ON::RENDER_POST_PAUSE_MENU);
}

using VanillaRenderDrawDepthFun = void(Layer*, uint8_t, float, float, float, float);
VanillaRenderDrawDepthFun* g_render_draw_depth_trampoline{nullptr};
void render_draw_depth(Layer* layer, uint8_t draw_depth, float bbox_left, float bbox_bottom, float bbox_right, float bbox_top)
{
    trigger_vanilla_render_draw_depth_callbacks(ON::RENDER_PRE_DRAW_DEPTH, draw_depth, {bbox_left, bbox_top, bbox_right, bbox_bottom});
    g_render_draw_depth_trampoline(layer, draw_depth, bbox_left, bbox_bottom, bbox_right, bbox_top);
}

static size_t text_rendering_context_offset = 0;
static size_t text_rendering_func1_offset = 0;
static size_t text_rendering_func2_offset = 0;
bool prepare_text_for_rendering(TextRenderingInfo* info, const std::string& text, float x, float y, float scale_x, float scale_y, uint32_t alignment, uint32_t fontstyle)
{
    auto& memory = Memory::get();
    auto exe = memory.exe();

    if (text_rendering_context_offset == 0)
    {
        std::string pattern = "\x48\x8D\x0D\x56\x05\x16\x00"s;
        size_t pattern_pos = find_inst(exe, pattern, memory.after_bundle);
        text_rendering_context_offset = memory.at_exe(decode_pc(exe, pattern_pos));

        pattern_pos += 7;
        text_rendering_func1_offset = memory.at_exe(decode_pc(exe, pattern_pos, 1));

        pattern_pos += 13;
        text_rendering_func2_offset = memory.at_exe(decode_pc(exe, pattern_pos, 1));
    }

    if (text_rendering_context_offset != 0)
    {
        auto convert_result = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()), nullptr, 0);
        if (convert_result <= 0)
        {
            return false;
        }
        std::wstring wide_text;
        wide_text.resize(convert_result + 10);
        convert_result = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()), &wide_text[0], static_cast<int>(wide_text.size()));

        typedef void func1(size_t context, uint32_t fontstyle, void* text_to_draw, uint32_t, float x, float y, TextRenderingInfo*, uint32_t, float scale_x, float scale_y, uint32_t alignment, uint32_t unknown_baseline_shift, int8_t);
        static func1* f1 = (func1*)(text_rendering_func1_offset);
        f1(text_rendering_context_offset, fontstyle, wide_text.data(), 2, x, y, info, 1, scale_x, scale_y, alignment, 2, 0);

        return true;
    }
    return false;
}

void RenderAPI::draw_text(const std::string& text, float x, float y, float scale_x, float scale_y, Color color, uint32_t alignment, uint32_t fontstyle)
{
    TextRenderingInfo tri = {0};
    if (!prepare_text_for_rendering(&tri, text, x, y, scale_x, scale_y, alignment, fontstyle))
    {
        return;
    }

    typedef void func2(TextRenderingInfo*, Color * color);
    static func2* f2 = (func2*)(text_rendering_func2_offset);
    f2(&tri, &color);
}

std::pair<float, float> RenderAPI::draw_text_size(const std::string& text, float scale_x, float scale_y, uint32_t fontstyle)
{
    TextRenderingInfo tri = {0};
    if (!prepare_text_for_rendering(&tri, text, 0, 0, scale_x, scale_y, 1 /*center*/, fontstyle))
    {
        return std::make_pair(0.0f, 0.0f);
    }
    return std::make_pair(tri.width, tri.height);
}

void RenderAPI::draw_screen_texture(uint32_t texture_id, uint8_t row, uint8_t column, float left, float top, float right, float bottom, Color color)
{
    static size_t offset = 0;

    auto& memory = Memory::get();
    auto exe = memory.exe();

    if (offset == 0)
    {
        std::string pattern = "\xB2\x29\xE8\xAE\x87\x04\x00"s;
        offset = memory.at_exe(decode_pc(exe, find_inst(exe, pattern, memory.after_bundle)));
    }

    if (offset != 0)
    {
        auto texture = RenderAPI::get().get_texture(texture_id);
        if (texture == nullptr)
        {
            return;
        }

        float width = right - left;
        float height = bottom - top;
        float half_width = width / 2.0f;
        float half_height = height / 2.0f;
        float center_x = left + half_width;
        float center_y = top + half_height;

        float uv_left = (texture->tile_width_fraction * column) + texture->offset_x_weird_math;
        float uv_right = uv_left + texture->tile_width_fraction;
        float uv_top = (texture->tile_height_fraction * row) + texture->offset_y_weird_math;
        float uv_bottom = uv_top + texture->tile_height_fraction;

        TextureRenderingInfo tri = {
            center_x,
            center_y,

            // DESTINATION
            // top left:
            -half_width,
            half_height,
            // top right:
            half_width,
            half_height,
            // bottom left:
            -half_width,
            -half_height,
            // bottom right:
            half_width,
            -half_height,

            // SOURCE
            // bottom left:
            uv_left,
            uv_bottom,
            // bottom right:
            uv_right,
            uv_bottom,
            // top left:
            uv_left,
            uv_top,
            // top right:
            uv_right,
            uv_top,
        };

        typedef void render_func(TextureRenderingInfo*, uint8_t shader, const char**, Color*);
        static render_func* rf = (render_func*)(offset);
        rf(&tri, 0x29, texture->name, &color);
        // < 0x27: invisible
        // 0x27: all white
        // 0x28: normal but a bit transparent
        // 0x29: normal but a bit transparent
        // 0x2a: normal but a lot transparent
        // 0x2b: normal but a bit transparent
        // 0x2c: all white
        // 0x2d: everything red and blurry
        // 0x2e: normal but a bit transparent
        // 0x2f: grayscale and a bit transparent
        // 0x30: everything red and blurry
        // 0x31: normal but a bit transparent
        // 0x32: grayscale and a lot transparent
        // 0x33: all white
        // 0x34: transparent parts become black
        // > 0x36: crash
    }
}

void RenderAPI::draw_world_texture(uint32_t texture_id, uint8_t row, uint8_t column, float left, float top, float right, float bottom, Color color)
{
    static size_t func_offset = 0;
    static size_t param_7 = 0;
    uint8_t shader = 0x7; // this comes from RenderInfo->shader (which might just be 8 bit instead of 32)

    auto& memory = Memory::get();
    auto exe = memory.exe();

    if (func_offset == 0)
    {
        std::string pattern = "\xC7\x44\x24\x28\x06\x00\x00\x00\x48\x8B\xD9"s;
        func_offset = function_start(memory.at_exe(find_inst(exe, pattern, memory.after_bundle)));

        auto offset = find_inst(exe, "\x4C\x8D\x0D\x59\xAF\x0F\x00"s, memory.after_bundle);
        param_7 = memory.at_exe(decode_pc(exe, offset));
    }

    if (func_offset != 0)
    {
        auto texture = RenderAPI::get().get_texture(texture_id);
        if (texture == nullptr)
        {
            return;
        }

        // destination and source float arrays are the same as in RenderInfo
        float unknown = 21;
        float destination[12] = {
            // bottom left:
            left,
            bottom,
            unknown,
            // bottom right:
            right,
            bottom,
            unknown,
            // top right:
            right,
            top,
            unknown,
            // top left:
            left,
            top,
            unknown};

        float uv_left = (texture->tile_width_fraction * column) + texture->offset_x_weird_math;
        float uv_right = uv_left + texture->tile_width_fraction;
        float uv_top = (texture->tile_height_fraction * row) + texture->offset_y_weird_math;
        float uv_bottom = uv_top + texture->tile_height_fraction;

        float source[8] = {
            // bottom left:
            uv_left,
            uv_bottom,
            // bottom right:
            uv_right,
            uv_bottom,
            // top right:
            uv_right,
            uv_top,
            // top left:
            uv_left,
            uv_top,
        };

        typedef void render_func(size_t, uint8_t, const char*** texture_name, bool render_as_non_liquid, float* destination, float* source, void*, void*, void*, void*, Color*, float*);
        static render_func* rf = (render_func*)(func_offset);
        size_t stack_filler = 0;
        rf(renderer(), shader, &texture->name, true, destination, source, (void*)stack_filler, (void*)stack_filler, (void*)param_7, (void*)stack_filler, &color, nullptr);
    }
}

using FetchTexture = const Texture*(class Entity*, uint32_t);
FetchTexture* g_fetch_texture_trampoline{nullptr};
const Texture* fetch_texture(class Entity* source_entity, uint32_t texture_id)
{
    auto* textures = get_textures();
    if (texture_id >= textures->texture_map.size())
    {
        const auto& render_api = RenderAPI::get();
        std::lock_guard lock{render_api.custom_textures_lock};
        const auto& custom_textures = render_api.custom_textures;
        auto it = custom_textures.find(texture_id);
        if (it != custom_textures.end())
        {
            return &it->second;
        }
    }

    return g_fetch_texture_trampoline(source_entity, texture_id);
}

void init_render_api_hooks()
{
    auto& memory = Memory::get();
    auto exe = memory.exe();
    auto after_bundle = memory.after_bundle;

    {
        auto fun_start = find_inst(exe, "\x48\x83\xec\x20\x48\x8b\x01\x48\x8b\xf9\x48\x8b\x58\x28"s, after_bundle);
        fun_start = find_inst(exe, "\xe8"s, fun_start);
        fun_start = Memory::decode_call(fun_start);
        g_fetch_texture_trampoline = (FetchTexture*)memory.at_exe(fun_start);
    }

    {
        auto fun_start = function_start(memory.at_exe(find_inst(exe, "\x48\x8D\x0D\x56\x05\x16\x00"s, after_bundle)));
        g_render_hud_trampoline = (VanillaRenderHudFun*)fun_start;
    }

    {
        auto fun_start = function_start(memory.at_exe(find_inst(exe, "\x48\x8B\x58\x10\x48\x8B\x83\xA8\x00\x00\x00"s, after_bundle)));
        g_render_pause_menu_trampoline = (VanillaRenderPauseMenuFun*)fun_start;
    }

    {
        auto fun_start = function_start(memory.at_exe(find_inst(exe, "\x44\x8B\xAC\xC1\x14\x3F\x06\x00"s, after_bundle)));
        g_render_draw_depth_trampoline = (VanillaRenderDrawDepthFun*)fun_start;
    }

    DetourRestoreAfterWith();

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    DetourAttach((void**)&g_fetch_texture_trampoline, &fetch_texture);
    DetourAttach((void**)&g_render_hud_trampoline, &render_hud);
    DetourAttach((void**)&g_render_pause_menu_trampoline, &render_pause_menu);
    DetourAttach((void**)&g_render_draw_depth_trampoline, &render_draw_depth);

    const LONG error = DetourTransactionCommit();
    if (error != NO_ERROR)
    {
        DEBUG("Failed hooking render_api: {}\n", error);
    }
}
