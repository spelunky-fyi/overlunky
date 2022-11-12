#include "render_api.hpp"

#include <Windows.h>    // for MultiByteToWideChar, GetCurrentThread
#include <array>        // for array
#include <cstddef>      // for size_t
#include <detours.h>    // for DetourAttach, DetourTransactionBegin
#include <fmt/format.h> // for check_format_string, format, vformat
#include <list>         // for _List_iterator, _List_const_iterator
#include <optional>     // for optional, nullopt
#include <string>       // for operator""sv, string, wstring, all...
#include <string_view>  // for string_view
#include <vector>       // for vector

#include "entity.hpp"             // for Entity, EntityDB
#include "level_api.hpp"          // for ThemeInfo
#include "logger.h"               // for DEBUG
#include "memory.hpp"             // for read_u64, to_le_bytes, write_mem_prot
#include "screen.hpp"             //
#include "script/events.hpp"      // for trigger_vanilla_render_journal_pag...
#include "script/lua_backend.hpp" // for ON, ON::RENDER_POST_JOURNAL_PAGE
#include "search.hpp"             // for get_address
#include "state.hpp"              // for State, StateMemory
#include "strings.hpp"            //
#include "texture.hpp"            // for Texture, get_textures, get_texture

class JournalPage;
struct Camera;
struct Illumination;
struct Layer;

RenderAPI& RenderAPI::get()
{
    static RenderAPI render_api = []()
    {
        return RenderAPI{(size_t*)get_address("render_api_callback"sv), get_address("render_api_offset"sv)};
    }();
    return render_api;
}

size_t RenderAPI::renderer() const
{
    return read_u64(*api + 0x10);
}

size_t RenderAPI::swap_chain() const
{
    return read_u64(renderer() + swap_chain_off);
}

void (*g_post_render_game)(){nullptr};

using VanillaRenderLoadingFun = void(size_t);
VanillaRenderLoadingFun* g_render_loading_trampoline{nullptr};
void render_loading(size_t param_1)
{
    g_render_loading_trampoline(param_1);

    // Piggy-backing off the last draw call
    if (g_post_render_game)
    {
        g_post_render_game();
    }
}

std::optional<TEXTURE> g_forced_lut_textures[2]{};

using RenderLayer = void(const std::vector<Illumination*>&, uint8_t, const Camera&, const char**, const char**);
RenderLayer* g_render_layer_trampoline{nullptr};
void render_layer(const std::vector<Illumination*>& lightsources, uint8_t layer, const Camera& camera, const char** lut_lhs, const char** lut_rhs)
{
    // The lhs and rhs LUTs are blended in the shader, but we don't know where that value is CPU side so we can only override
    // with a single LUT for now
    if (g_forced_lut_textures[layer])
    {
        if (Texture* lut = get_texture(g_forced_lut_textures[layer].value()))
        {
            g_render_layer_trampoline(lightsources, layer, camera, lut->name, lut->name);
            return;
        }
    }
    g_render_layer_trampoline(lightsources, layer, camera, lut_lhs, lut_rhs);
}

void RenderAPI::set_lut(TEXTURE texture_id, uint8_t layer)
{
    g_forced_lut_textures[layer] = texture_id;
}
void RenderAPI::reset_lut(uint8_t layer)
{
    g_forced_lut_textures[layer] = std::nullopt;
}

bool g_advanced_hud{false};

using VanillaRenderHudFun = void(size_t, float, float, size_t);
VanillaRenderHudFun* g_render_hud_trampoline{nullptr};
void render_hud(size_t hud_data, float y, float opacity, size_t hud_data2)
{
    // hud_data and hud_data2 are the same pointer, but the second one is actually used (displays garbage if not passed)
    trigger_vanilla_render_callbacks(ON::RENDER_PRE_HUD);
    g_render_hud_trampoline(hud_data, y - g_advanced_hud * 0.004f, opacity, hud_data2);
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
    trigger_vanilla_render_draw_depth_callbacks(ON::RENDER_POST_DRAW_DEPTH, draw_depth, {bbox_left, bbox_top, bbox_right, bbox_bottom});
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

using OnSelectFromJournalMenu = void(void*, uint8_t);
OnSelectFromJournalMenu* g_on_select_from_journal_menu_trampoline{nullptr};
bool g_selecting_from_menu_scope = false;
void on_select_from_journal(void* unknown, uint8_t index)
{
    g_selecting_from_menu_scope = true;
    if (pre_load_journal_chapter(index + 3))
    {
        return;
    }
    g_on_select_from_journal_menu_trampoline(unknown, index);
    g_selecting_from_menu_scope = false;
}

static bool g_journal_enabled = true;
using OnShowJournalFun = void(JournalUI*, uint8_t, bool, bool);
OnShowJournalFun* g_on_show_journal_trampoline{nullptr};
void on_open_journal_chapter(JournalUI* journal_ui, uint8_t chapter, bool instant, bool play_sound)
{
    if (!g_journal_enabled && chapter == 2)
    {
        return;
    }

    if (!g_selecting_from_menu_scope && pre_load_journal_chapter(chapter))
    {
        // journal_ui->pages.clear(); // bug fix, probably not needed anymore, left just in case
        return;
    }

    g_on_show_journal_trampoline(journal_ui, chapter, instant, play_sound);

    std::vector<uint32_t> pages;
    pages.reserve(journal_ui->pages_tmp.size());
    for (auto p : journal_ui->pages_tmp)
    {
        pages.push_back(p->page_number);
    }

    auto return_pages = post_load_journal_chapter(chapter, pages);

    if (!return_pages.empty())
    {
        std::map<JournalPage*, bool> pages_copy;
        for (auto p : journal_ui->pages_tmp)
        {
            pages_copy.insert({p, true});
        }

        auto find_page = [](std::map<JournalPage*, bool>& x, uint32_t find) -> JournalPage*
        {
            for (auto& it : x)
            {
                if (it.first->page_number == find)
                {
                    if (it.second == false)
                        return nullptr;

                    it.second = false;
                    return it.first;
                }
            }
            return nullptr;
        };

        journal_ui->pages_tmp.clear();
        uint8_t side = 0;
        journal_ui->max_page_count = 0x7FFFFFFF;
        for (auto page_id : return_pages)
        {
            auto p = find_page(pages_copy, page_id);
            if (p)
            {
                journal_ui->pages_tmp.push_back(p);
                // p->set_page_background_side(side); // only changes background, all the text and graphics are still rendered with the old offset
            }
            else
            {
                journal_ui->pages_tmp.push_back(JournalPageStory::construct(side, page_id));
            }
            side ^= 0x1;
        }
        for (auto p : pages_copy) // free unused screens
        {
            if (p.second)
            {
                p.first->~JournalPage();
            }
        }
    }
}

bool& get_journal_enabled()
{
    return g_journal_enabled;
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
void free_text_post_rendering(TextRenderingInfo* info)
{
    if (info->unknown4 != nullptr)
    {
        game_free(info->unknown4);
    }
    if (info->letter_textures != nullptr)
    {
        game_free(info->letter_textures);
    }
    if (info->unknown6 != nullptr)
    {
        game_free(info->unknown6);
    }
}

void RenderAPI::draw_text(const std::string& text, float x, float y, float scale_x, float scale_y, Color color, uint32_t alignment, uint32_t fontstyle)
{
    TextRenderingInfo tri{};
    ON_SCOPE_EXIT(free_text_post_rendering(&tri));

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
    TextRenderingInfo tri{};
    ON_SCOPE_EXIT(free_text_post_rendering(&tri));

    if (!prepare_text_for_rendering(&tri, text, 0, 0, scale_x, scale_y, 1 /*center*/, fontstyle))
    {
        return std::make_pair(0.0f, 0.0f);
    }
    return std::make_pair(tri.width, tri.height);
}

void RenderAPI::draw_screen_texture(Texture* texture, Quad source, Quad dest, Color color)
{
    static size_t offset = get_address("draw_screen_texture");
    constexpr uint8_t shader = 0x29;

    if (offset != 0)
    {
        TextureRenderingInfo tri = {
            0,
            0,
            // DESTINATION
            dest.bottom_left_x,
            dest.bottom_left_y,
            dest.bottom_right_x,
            dest.bottom_right_y,
            dest.top_left_x,
            dest.top_left_y,
            dest.top_right_x,
            dest.top_right_y,

            // SOURCE
            source.bottom_left_x,
            source.bottom_left_y,
            source.bottom_right_x,
            source.bottom_right_y,
            source.top_left_x,
            source.top_left_y,
            source.top_right_x,
            source.top_right_y,
        };

        typedef void render_func(TextureRenderingInfo*, uint8_t, const char**, Color*);
        static render_func* rf = (render_func*)(offset);
        rf(&tri, shader, texture->name, &color);
    }
}

void RenderAPI::draw_world_texture(Texture* texture, Quad source, Quad dest, Color color, WorldShader shader)
{
    static size_t func_offset = 0;
    static size_t param_7 = 0;
    if (func_offset == 0)
    {
        func_offset = get_address("draw_world_texture"sv);
        param_7 = get_address("draw_world_texture_param_7"sv);
    }

    if (func_offset != 0)
    {
        // destination and source float arrays are the same as in RenderInfo
        const float unknown = 21;
        // this is also Quad, but some special one
        float destination[12] = {
            // bottom left:
            dest.bottom_left_x,
            dest.bottom_left_y,
            unknown,
            // bottom right:
            dest.bottom_right_x,
            dest.bottom_right_y,
            unknown,
            // top right:
            dest.top_right_x,
            dest.top_right_y,
            unknown,
            // top left:
            dest.top_left_x,
            dest.top_left_y,
            unknown};

        typedef void render_func(size_t, WorldShader, const char*** texture_name, uint32_t render_as_non_liquid, float* destination, Quad* source, void*, Color*, float*);
        static render_func* rf = (render_func*)(func_offset);
        auto texture_name = texture->name;
        rf(renderer(), shader, &texture_name, 1, destination, &source, (void*)param_7, &color, nullptr);
    }
}

void RenderAPI::set_post_render_game(void (*post_render_game)())
{
    g_post_render_game = post_render_game;
}
void RenderAPI::set_advanced_hud()
{
    g_advanced_hud = true;
}

void RenderAPI::reload_shaders()
{
    using ReloadShadersFun = void(size_t);
    ReloadShadersFun* reload_shaders_impl = (ReloadShadersFun*)get_address("reload_shaders"sv);
    reload_shaders_impl(renderer());
}

void fetch_texture(Entity* entity, int32_t texture_id)
{
    entity->texture = nullptr;

    auto* textures = get_textures();
    if (texture_id >= static_cast<int64_t>(textures->texture_map.size()))
    {
        auto& render_api = RenderAPI::get();
        std::lock_guard lock{render_api.custom_textures_lock};
        auto& custom_textures = render_api.custom_textures;
        auto it = custom_textures.find(texture_id);
        if (it != custom_textures.end())
        {
            entity->texture = &it->second;
        }
    }

    if (entity->texture == nullptr)
    {
        if (texture_id < -3)
        {
            texture_id = State::get().ptr_local()->current_theme->get_dynamic_texture(texture_id);
        }
        entity->texture = get_textures()->texture_map[texture_id];
    }

    if (entity->texture != nullptr)
    {
        entity->animation_frame = static_cast<uint16_t>(entity->type->tile_y * entity->texture->num_tiles_width + entity->type->tile_x);
    }
    else
    {
        entity->animation_frame = 0;
    }
}

using PrepareTextFun = void(uint32_t fontstyle, const wchar_t* text, uint32_t a3, float x, float y, TextRenderingInfo* a6, float scale_x, float scale_y, uint32_t alignment, uint32_t unknown_baseline_shift, int8_t a11);
PrepareTextFun* g_prepare_text_trampoline{nullptr};
void prepare_text(uint32_t fontstyle, const wchar_t* text, uint32_t a3, float x, float y, TextRenderingInfo* a6, float scale_x, float scale_y, uint32_t alignment, uint32_t unknown_baseline_shift, int8_t a11)
{
    static const STRINGID first_death = hash_to_stringid(0x5a52a061);
    static const STRINGID last_death = hash_to_stringid(0x5c9b2332);
    if (fontstyle == 1)
    {
        STRINGID id = pointer_to_stringid((size_t)text);
        if (id >= first_death && id <= last_death)
        {
            on_death_message(id);
        }
    }
    g_prepare_text_trampoline(fontstyle, text, a3, x, y, a6, scale_x, scale_y, alignment, unknown_baseline_shift, a11);
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
            "\x48\x89\xf1"    // mov    rcx, rax
            "\x48\x89\xc2"    // mov    rdx, rsi
            "\x48\xb8{}"      // mov    rax, 0x0
            "\xff\xd0"        // call   rax
            "\x48\x31\xc0"sv, // xor    rax, rax
            to_le_bytes(fetch_texture_addr));

        // Fill with nop, code is not performance-critical either way
        const size_t original_code_size = fetch_texture_end - fetch_texture_begin;
        code.resize(original_code_size, '\x90');

        write_mem_prot(fetch_texture_begin, code, true);
    }

    g_render_loading_trampoline = (VanillaRenderLoadingFun*)get_address("render_loading"sv);
    g_render_layer_trampoline = (RenderLayer*)get_address("render_layer"sv);
    g_render_hud_trampoline = (VanillaRenderHudFun*)get_address("render_hud"sv);
    g_render_pause_menu_trampoline = (VanillaRenderPauseMenuFun*)get_address("render_pause_menu"sv);
    g_render_draw_depth_trampoline = (VanillaRenderDrawDepthFun*)get_address("render_draw_depth"sv);
    g_on_show_journal_trampoline = (OnShowJournalFun*)get_address("show_journal");
    g_on_select_from_journal_menu_trampoline = (OnSelectFromJournalMenu*)get_address("journal_menu_select");

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

    g_prepare_text_trampoline = (PrepareTextFun*)get_address("prepare_text_for_rendering");

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    DetourAttach((void**)&g_render_loading_trampoline, render_loading);
    DetourAttach((void**)&g_render_layer_trampoline, render_layer);
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
    DetourAttach((void**)&g_on_show_journal_trampoline, &on_open_journal_chapter);
    DetourAttach((void**)&g_on_select_from_journal_menu_trampoline, &on_select_from_journal);

    DetourAttach((void**)&g_prepare_text_trampoline, prepare_text);

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

Quad TextureRenderingInfo::dest_get_quad()
{
    return Quad{destination_bottom_left_x, destination_bottom_left_y, destination_bottom_right_x, destination_bottom_right_y, destination_top_right_x, destination_top_right_y, destination_top_left_x, destination_top_left_y};
}
void TextureRenderingInfo::dest_set_quad(const Quad& quad)
{
    destination_bottom_left_x = quad.bottom_left_x;
    destination_bottom_left_y = quad.bottom_left_y;
    destination_bottom_right_x = quad.bottom_right_x;
    destination_bottom_right_y = quad.bottom_right_y;
    destination_top_right_x = quad.top_right_x;
    destination_top_right_y = quad.top_right_y;
    destination_top_left_x = quad.top_left_x;
    destination_top_left_y = quad.top_left_y;
}
Quad TextureRenderingInfo::source_get_quad()
{
    return Quad{source_bottom_left_x, source_bottom_left_y, source_bottom_right_x, source_bottom_right_y, source_top_right_x, source_top_right_y, source_top_left_x, source_top_left_y};
}
void TextureRenderingInfo::source_set_quad(const Quad& quad)
{
    source_bottom_left_x = quad.bottom_left_x;
    source_bottom_left_y = quad.bottom_left_y;
    source_bottom_right_x = quad.bottom_right_x;
    source_bottom_right_y = quad.bottom_right_y;
    source_top_right_x = quad.top_right_x;
    source_top_right_y = quad.top_right_y;
    source_top_left_x = quad.top_left_x;
    source_top_left_y = quad.top_left_y;
}
