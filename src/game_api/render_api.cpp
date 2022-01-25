#include "render_api.hpp"

#include <cstddef>
#include <detours.h>
#include <string>

#include "layer.hpp"
#include "level_api.hpp"
#include "memory.hpp"
#include "script/events.hpp"
#include "script/lua_backend.hpp"
#include "state.hpp"
#include "texture.hpp"

RenderAPI& RenderAPI::get()
{
    static RenderAPI render_api = []()
    {
        return RenderAPI{(size_t*)get_address("render_api_callback"sv), get_address("render_api_offset"sv)};
    }();
    return render_api;
}

TextureDefinition RenderAPI::get_texture_definition(TEXTURE texture_id)
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

Texture* RenderAPI::get_texture(TEXTURE texture_id)
{
    auto* textures = get_textures();
    if (texture_id >= static_cast<int64_t>(textures->texture_map.size()))
    {
        std::lock_guard lock{custom_textures_lock};
        return &custom_textures[texture_id];
    }
    return textures->texture_map[texture_id];
}

TEXTURE RenderAPI::define_texture(TextureDefinition data)
{

    if (data.sub_image_width == 0 || data.sub_image_height == 0)
    {
        data.sub_image_width = data.width;
        data.sub_image_height = data.height;
    }

    auto* textures = get_textures();

    std::lock_guard lock{custom_textures_lock};

    Texture new_texture{
        static_cast<int64_t>(textures->texture_map.size() + custom_textures.size() + 1),
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
        std::string_view existing_name{*texture.name};
        existing_name.remove_prefix(sizeof("Data/Textures/../../") - 1);
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

    auto* new_texture_target = textures->texture_map[0];

    const auto backup_num_textures = textures->num_textures;
    const auto backup_texture = *new_texture_target;
    textures->num_textures = 0;
    data.texture_path = "../../" + data.texture_path;

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
    custom_textures[new_texture.id] = new_texture;

    return new_texture.id;
}

void RenderAPI::reload_texture(const char* texture_name)
{
    {
        std::lock_guard lock{custom_textures_lock};
        for (auto& [id, texture] : custom_textures)
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
void RenderAPI::reload_texture(const char** texture_name)
{
    class Renderer;
    using LoadTextureFunT = void(Renderer*, const char**);

    auto renderer_ptr = (Renderer*)renderer();
    auto load_texture = *vtable_find<LoadTextureFunT*>(renderer_ptr, 0x2D);
    load_texture(renderer_ptr, texture_name);
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

using VanillaRenderJournalPageFun = void(JournalPage*);
VanillaRenderJournalPageFun* g_render_journal_page_journalmenu_trampoline{nullptr};
VanillaRenderJournalPageFun* g_render_journal_page_progress_trampoline{nullptr};
VanillaRenderJournalPageFun* g_render_journal_page_place_trampoline{nullptr};
VanillaRenderJournalPageFun* g_render_journal_page_people_trampoline{nullptr};
VanillaRenderJournalPageFun* g_render_journal_page_bestiary_trampoline{nullptr};
VanillaRenderJournalPageFun* g_render_journal_page_items_trampoline{nullptr};
VanillaRenderJournalPageFun* g_render_journal_page_traps_trampoline{nullptr};
VanillaRenderJournalPageFun* g_render_journal_page_story_trampoline{nullptr};
VanillaRenderJournalPageFun* g_render_journal_page_feats_trampoline{nullptr};
VanillaRenderJournalPageFun* g_render_journal_page_deathcause_trampoline{nullptr};
VanillaRenderJournalPageFun* g_render_journal_page_deathmenu_trampoline{nullptr};
VanillaRenderJournalPageFun* g_render_journal_page_recap_trampoline{nullptr};
VanillaRenderJournalPageFun* g_render_journal_page_player_profile_trampoline{nullptr};
VanillaRenderJournalPageFun* g_render_journal_page_last_game_played_trampoline{nullptr};
void render_journal_page_journalmenu(JournalPage* page)
{
    g_render_journal_page_journalmenu_trampoline(page);
    trigger_vanilla_render_journal_page_callbacks(ON::RENDER_POST_JOURNAL_PAGE, JournalPageType::JournalMenu, page);
}
void render_journal_page_progress(JournalPage* page)
{
    g_render_journal_page_progress_trampoline(page);
    trigger_vanilla_render_journal_page_callbacks(ON::RENDER_POST_JOURNAL_PAGE, JournalPageType::Progress, page);
}
void render_journal_page_place(JournalPage* page)
{
    g_render_journal_page_place_trampoline(page);
    trigger_vanilla_render_journal_page_callbacks(ON::RENDER_POST_JOURNAL_PAGE, JournalPageType::Places, page);
}
void render_journal_page_people(JournalPage* page)
{
    g_render_journal_page_people_trampoline(page);
    trigger_vanilla_render_journal_page_callbacks(ON::RENDER_POST_JOURNAL_PAGE, JournalPageType::People, page);
}
void render_journal_page_bestiary(JournalPage* page)
{
    g_render_journal_page_bestiary_trampoline(page);
    trigger_vanilla_render_journal_page_callbacks(ON::RENDER_POST_JOURNAL_PAGE, JournalPageType::Bestiary, page);
}
void render_journal_page_items(JournalPage* page)
{
    g_render_journal_page_items_trampoline(page);
    trigger_vanilla_render_journal_page_callbacks(ON::RENDER_POST_JOURNAL_PAGE, JournalPageType::Items, page);
}
void render_journal_page_traps(JournalPage* page)
{
    g_render_journal_page_traps_trampoline(page);
    trigger_vanilla_render_journal_page_callbacks(ON::RENDER_POST_JOURNAL_PAGE, JournalPageType::Traps, page);
}
void render_journal_page_story(JournalPage* page)
{
    g_render_journal_page_story_trampoline(page);
    trigger_vanilla_render_journal_page_callbacks(ON::RENDER_POST_JOURNAL_PAGE, JournalPageType::Story, page);
}
void render_journal_page_feats(JournalPage* page)
{
    g_render_journal_page_feats_trampoline(page);
    trigger_vanilla_render_journal_page_callbacks(ON::RENDER_POST_JOURNAL_PAGE, JournalPageType::Feats, page);
}
void render_journal_page_deathcause(JournalPage* page)
{
    g_render_journal_page_deathcause_trampoline(page);
    trigger_vanilla_render_journal_page_callbacks(ON::RENDER_POST_JOURNAL_PAGE, JournalPageType::DeathCause, page);
}
void render_journal_page_deathmenu(JournalPage* page)
{
    g_render_journal_page_deathmenu_trampoline(page);
    trigger_vanilla_render_journal_page_callbacks(ON::RENDER_POST_JOURNAL_PAGE, JournalPageType::DeathMenu, page);
}
void render_journal_page_recap(JournalPage* page)
{
    g_render_journal_page_recap_trampoline(page);
    trigger_vanilla_render_journal_page_callbacks(ON::RENDER_POST_JOURNAL_PAGE, JournalPageType::Recap, page);
}
void render_journal_page_player_profile(JournalPage* page)
{
    g_render_journal_page_player_profile_trampoline(page);
    trigger_vanilla_render_journal_page_callbacks(ON::RENDER_POST_JOURNAL_PAGE, JournalPageType::PlayerProfile, page);
}
void render_journal_page_last_game_played(JournalPage* page)
{
    g_render_journal_page_last_game_played_trampoline(page);
    trigger_vanilla_render_journal_page_callbacks(ON::RENDER_POST_JOURNAL_PAGE, JournalPageType::LastGamePlayed, page);
}

bool prepare_text_for_rendering(TextRenderingInfo* info, const std::string& text, float x, float y, float scale_x, float scale_y, uint32_t alignment, uint32_t fontstyle)
{
    static size_t text_rendering_func1_offset = 0;
    if (text_rendering_func1_offset == 0)
    {
        text_rendering_func1_offset = get_address("prepare_text_for_rendering"sv);
    }

    if (text_rendering_func1_offset != 0)
    {
        auto convert_result = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()), nullptr, 0);
        if (convert_result <= 0)
        {
            return false;
        }
        std::wstring wide_text;
        wide_text.resize(convert_result + 10);
        convert_result = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()), &wide_text[0], static_cast<int>(wide_text.size()));

        typedef void func1(uint32_t fontstyle, void* text_to_draw, uint32_t, float x, float y, TextRenderingInfo*, float scale_x, float scale_y, uint32_t alignment, uint32_t unknown_baseline_shift, int8_t);
        static func1* f1 = (func1*)(text_rendering_func1_offset);
        f1(fontstyle, wide_text.data(), 2, x, y, info, scale_x, scale_y, alignment, 2, 0);
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

    static size_t text_rendering_func2_offset = 0;
    if (text_rendering_func2_offset == 0)
    {
        text_rendering_func2_offset = get_address("draw_text"sv);
    }

    if (text_rendering_func2_offset != 0)
    {
        typedef void func2(TextRenderingInfo*, Color * color);
        static func2* f2 = (func2*)(text_rendering_func2_offset);
        f2(&tri, &color);
    }
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

void RenderAPI::draw_screen_texture(TEXTURE texture_id, uint8_t row, uint8_t column, float left, float top, float right, float bottom, Color color)
{
    static size_t offset = 0;

    if (offset == 0)
    {
        offset = get_address("draw_screen_texture");
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
        float uv_right = uv_left + texture->tile_width_fraction - texture->one_over_width;
        float uv_top = (texture->tile_height_fraction * row) + texture->offset_y_weird_math;
        float uv_bottom = uv_top + texture->tile_height_fraction - texture->one_over_height;

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
    }
}

void RenderAPI::draw_world_texture(TEXTURE texture_id, uint8_t row, uint8_t column, float left, float top, float right, float bottom, Color color)
{
    static size_t func_offset = 0;
    static size_t param_7 = 0;
    uint8_t shader = 0x7; // this comes from RenderInfo->shader

    if (func_offset == 0)
    {
        func_offset = get_address("draw_world_texture"sv);
        param_7 = get_address("draw_world_texture_param_7"sv);
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
        float uv_right = uv_left + texture->tile_width_fraction - texture->one_over_width;
        float uv_top = (texture->tile_height_fraction * row) + texture->offset_y_weird_math;
        float uv_bottom = uv_top + texture->tile_height_fraction - texture->one_over_height;

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

        typedef void render_func(size_t, uint8_t, const char*** texture_name, uint32_t render_as_non_liquid, float* destination, float* source, void*, Color*, float*);
        static render_func* rf = (render_func*)(func_offset);
        auto texture_name = texture->name;
        rf(renderer(), shader, &texture_name, 1, destination, source, (void*)param_7, &color, nullptr);
    }
}

const Texture* fetch_texture(int32_t texture_id)
{
    auto* textures = get_textures();
    if (texture_id >= static_cast<int64_t>(textures->texture_map.size()))
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

    if (texture_id < -3)
    {
        texture_id = State::get().ptr_local()->current_theme->get_dynamic_floor_texture_id(texture_id);
    }
    return get_textures()->texture_map[texture_id];
}

void init_render_api_hooks()
{
    // Fix the texture fetching in spawn_entity
    if (const size_t fetch_texture_begin = get_address("fetch_texture_begin"))
    {
        const size_t fetch_texture_end = get_address("fetch_texture_end");

        const size_t fetch_texture_addr = (size_t)&fetch_texture;

        // Manually assembled code, let's hope it won't have to change ever
        std::string code = fmt::format(
            "\x48\x89\xc1"                         //mov    rcx, rax
            "\x48\xba{}"                           //mov    rdx, 0x0
            "\xff\xd2"                             //call   rdx
            "\x48\x89\x86\x90\x00\x00\x00"         //mov    QWORD PTR[rsi + 0x90], rax
            "\x48\x85\xc0"                         //test   rax, rax
            "\x74\x17"                             //jz     0x17
            "\x48\x0f\xb7\x40\x18"                 //movzx  rax, WORD PTR[rax + 0x18]
            "\x66\x0f\xaf\x84\x1f\x90\x00\x00\x00" //imul   ax, WORD PTR[rdi + rbx * 1 + 0x90]
            "\x66\x03\x84\x1f\x8c\x00\x00\x00"     //add    ax, WORD PTR[rdi + rbx * 1 + 0x8c]
            "\xeb\x02"                             //jmp    0x2
            "\x31\xc0"sv,                          //xor    rax, rax
            to_le_bytes(fetch_texture_addr));

        // Fill with nop, code is not performance-critical either way
        const size_t original_code_size = fetch_texture_end - fetch_texture_begin;
        code.resize(original_code_size, '\x90');

        write_mem_prot(fetch_texture_begin, code, true);
    }

    g_render_hud_trampoline = (VanillaRenderHudFun*)get_address("render_hud"sv);
    g_render_pause_menu_trampoline = (VanillaRenderPauseMenuFun*)get_address("render_pause_menu"sv);
    g_render_draw_depth_trampoline = (VanillaRenderDrawDepthFun*)get_address("render_draw_depth"sv);

    const size_t fourth_virt = 4 * sizeof(size_t);
    const size_t journal_vftable = get_address("vftable_JournalPages"sv);

    size_t* render_virt = (size_t*)(journal_vftable + JOURNAL_VFTABLE::MENU + fourth_virt);
    g_render_journal_page_journalmenu_trampoline = (VanillaRenderJournalPageFun*)(*render_virt);
    render_virt = (size_t*)(journal_vftable + JOURNAL_VFTABLE::PROGRESS + fourth_virt);
    g_render_journal_page_progress_trampoline = (VanillaRenderJournalPageFun*)(*render_virt);
    render_virt = (size_t*)(journal_vftable + JOURNAL_VFTABLE::PLACES + fourth_virt);
    g_render_journal_page_place_trampoline = (VanillaRenderJournalPageFun*)(*render_virt);
    render_virt = (size_t*)(journal_vftable + JOURNAL_VFTABLE::PEOPLE + fourth_virt);
    g_render_journal_page_people_trampoline = (VanillaRenderJournalPageFun*)(*render_virt);
    render_virt = (size_t*)(journal_vftable + JOURNAL_VFTABLE::BESTIARY + fourth_virt);
    g_render_journal_page_bestiary_trampoline = (VanillaRenderJournalPageFun*)(*render_virt);
    render_virt = (size_t*)(journal_vftable + JOURNAL_VFTABLE::ITEMS + fourth_virt);
    g_render_journal_page_items_trampoline = (VanillaRenderJournalPageFun*)(*render_virt);
    render_virt = (size_t*)(journal_vftable + JOURNAL_VFTABLE::TRAPS + fourth_virt);
    g_render_journal_page_traps_trampoline = (VanillaRenderJournalPageFun*)(*render_virt);
    render_virt = (size_t*)(journal_vftable + JOURNAL_VFTABLE::STORY + fourth_virt);
    g_render_journal_page_story_trampoline = (VanillaRenderJournalPageFun*)(*render_virt);
    render_virt = (size_t*)(journal_vftable + JOURNAL_VFTABLE::FEATS + fourth_virt);
    g_render_journal_page_feats_trampoline = (VanillaRenderJournalPageFun*)(*render_virt);
    render_virt = (size_t*)(journal_vftable + JOURNAL_VFTABLE::DEATH_CAUSE + fourth_virt);
    g_render_journal_page_deathcause_trampoline = (VanillaRenderJournalPageFun*)(*render_virt);
    render_virt = (size_t*)(journal_vftable + JOURNAL_VFTABLE::DEATH_MENU + fourth_virt);
    g_render_journal_page_deathmenu_trampoline = (VanillaRenderJournalPageFun*)(*render_virt);
    render_virt = (size_t*)(journal_vftable + JOURNAL_VFTABLE::RECAP + fourth_virt);
    g_render_journal_page_recap_trampoline = (VanillaRenderJournalPageFun*)(*render_virt);
    render_virt = (size_t*)(journal_vftable + JOURNAL_VFTABLE::PLAYER_PROFILE + fourth_virt);
    g_render_journal_page_player_profile_trampoline = (VanillaRenderJournalPageFun*)(*render_virt);
    render_virt = (size_t*)(journal_vftable + JOURNAL_VFTABLE::LAST_GAME_PLAYED + fourth_virt);
    g_render_journal_page_last_game_played_trampoline = (VanillaRenderJournalPageFun*)(*render_virt);

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    DetourAttach((void**)&g_render_hud_trampoline, &render_hud);
    DetourAttach((void**)&g_render_pause_menu_trampoline, &render_pause_menu);
    DetourAttach((void**)&g_render_draw_depth_trampoline, &render_draw_depth);
    DetourAttach((void**)&g_render_journal_page_journalmenu_trampoline, &render_journal_page_journalmenu);
    DetourAttach((void**)&g_render_journal_page_progress_trampoline, &render_journal_page_progress);
    DetourAttach((void**)&g_render_journal_page_place_trampoline, &render_journal_page_place);
    DetourAttach((void**)&g_render_journal_page_people_trampoline, &render_journal_page_people);
    DetourAttach((void**)&g_render_journal_page_bestiary_trampoline, &render_journal_page_bestiary);
    DetourAttach((void**)&g_render_journal_page_items_trampoline, &render_journal_page_items);
    DetourAttach((void**)&g_render_journal_page_traps_trampoline, &render_journal_page_traps);
    DetourAttach((void**)&g_render_journal_page_story_trampoline, &render_journal_page_story);
    DetourAttach((void**)&g_render_journal_page_feats_trampoline, &render_journal_page_feats);
    DetourAttach((void**)&g_render_journal_page_deathcause_trampoline, &render_journal_page_deathcause);
    DetourAttach((void**)&g_render_journal_page_deathmenu_trampoline, &render_journal_page_deathmenu);
    DetourAttach((void**)&g_render_journal_page_recap_trampoline, &render_journal_page_recap);
    DetourAttach((void**)&g_render_journal_page_player_profile_trampoline, &render_journal_page_player_profile);
    DetourAttach((void**)&g_render_journal_page_last_game_played_trampoline, &render_journal_page_last_game_played);

    const LONG error = DetourTransactionCommit();
    if (error != NO_ERROR)
    {
        DEBUG("Failed hooking render_api: {}\n", error);
    }
}

void TextureRenderingInfo::set_destination(const AABB& bbox)
{
    auto w = bbox.width();
    auto h = bbox.bottom - bbox.top;
    auto half_w = w / 2.0f;
    auto half_h = h / 2.0f;

    x = bbox.left + half_w;
    y = bbox.top + half_h;

    destination_top_left_x = -half_w;
    destination_top_left_y = half_h;
    destination_top_right_x = half_w;
    destination_top_right_y = half_h;
    destination_bottom_left_x = -half_w;
    destination_bottom_left_y = -half_h;
    destination_bottom_right_x = half_w;
    destination_bottom_right_y = -half_h;
}
