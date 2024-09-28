#include "screen.hpp"

#include <algorithm>   // for transform
#include <cctype>      // for toupper
#include <functional>  // for function, _Func_impl_no_alloc<>::_Mybase
#include <new>         // for operator new
#include <type_traits> // for move
#include <utility>     // for find_if, min
#include <vector>      // for vector, _Vector_iterator, allocator, era...

#include "containers/game_allocator.hpp" //
#include "entity_hooks_info.hpp"         // for HookWithId
#include "game_manager.hpp"              // for GameManager, get_game_manager
#include "logger.h"                      // for DEBUG
#include "memory.hpp"
#include "render_api.hpp"
#include "screen_arena.hpp" // for ScreenArenaIntro, ScreenArenaItems, Scre...
#include "search.hpp"       //
#include "state.hpp"        // for StateMemory, get_state_ptr
#include "vtable_hook.hpp"  // for hook_vtable_no_dtor

struct ScreenHooksInfo
{
    Screen* screen;
    std::uint32_t cbcount;
    std::vector<HookWithId<bool(Screen*)>> pre_render;
    std::vector<HookWithId<void(Screen*)>> post_render;
};

std::vector<ScreenHooksInfo> g_screen_hooks;

ScreenHooksInfo& Screen::get_hooks()
{
    auto it = std::find_if(g_screen_hooks.begin(), g_screen_hooks.end(), [this](auto& hook)
                           { return hook.screen == this; });
    if (it == g_screen_hooks.end())
    {
        g_screen_hooks.push_back({this});
        return g_screen_hooks.back();
    }
    return *it;
}

void hook_screen_render(Screen* self)
{
    hook_vtable_no_dtor<void(Screen*), 0x3>(
        self,
        [](Screen* lmbd_self, void (*original)(Screen*))
        {
            ScreenHooksInfo& hook_info = lmbd_self->get_hooks();

            bool skip_orig = false;
            for (auto& [id, pre] : hook_info.pre_render)
            {
                if (pre(lmbd_self))
                {
                    skip_orig = true;
                }
            }

            if (!skip_orig)
            {
                original(lmbd_self);
            }

            for (auto& [id, post] : hook_info.post_render)
            {
                post(lmbd_self);
            }
        });
}

std::uint32_t Screen::reserve_callback_id()
{
    ScreenHooksInfo& hook_info = get_hooks();
    return hook_info.cbcount++;
}

void Screen::set_pre_render(std::uint32_t reserved_callback_id, std::function<bool(Screen*)> pre_render)
{
    ScreenHooksInfo& hook_info = get_hooks();
    if (hook_info.pre_render.empty() && hook_info.post_render.empty())
    {
        hook_screen_render(this);
    }
    hook_info.pre_render.push_back({reserved_callback_id, std::move(pre_render)});
}

void Screen::set_post_render(std::uint32_t reserved_callback_id, std::function<void(Screen*)> post_render)
{
    ScreenHooksInfo& hook_info = get_hooks();
    if (hook_info.pre_render.empty() && hook_info.post_render.empty())
    {
        hook_screen_render(this);
    }
    hook_info.post_render.push_back({reserved_callback_id, std::move(post_render)});
}

void Screen::unhook(std::uint32_t id)
{
    auto it = std::find_if(g_screen_hooks.begin(), g_screen_hooks.end(), [this](auto& hook)
                           { return hook.screen == this; });
    if (it != g_screen_hooks.end())
    {
        std::erase_if(it->pre_render, [id](auto& hook)
                      { return hook.id == id; });
        std::erase_if(it->post_render, [id](auto& hook)
                      { return hook.id == id; });
    }
}

Screen* get_screen_ptr(uint32_t screen_id)
{
    auto game_manager = get_game_manager();
    auto state = get_state_ptr();
    switch (screen_id)
    {
    case 0:
    {
        return game_manager->screen_logo;
    }
    case 1:
    {
        return game_manager->screen_intro;
    }
    case 2:
    {
        return game_manager->screen_prologue;
    }
    case 3:
    {
        return game_manager->screen_title;
    }
    case 4:
    {
        return game_manager->screen_menu;
    }
    case 5:
    {
        return game_manager->screen_options;
    }
    case 6:
    {
        return game_manager->screen_player_profile;
    }
    case 7:
    {
        return game_manager->screen_leaderboards;
    }
    case 8:
    {
        return game_manager->screen_seed_input;
    }
    case 9:
    {
        return state->screen_character_select;
    }
    case 10:
    {
        return state->screen_team_select;
    }
    case 11:
    {
        // a screen_camp also exists in State, but its buttons flags do not work, so we use GameManager's one
        return game_manager->screen_camp;
    }
    case 12:
    {
        // a screen_level also exists in State
        return game_manager->screen_level;
    }
    case 13:
    {
        return state->screen_transition;
    }
    case 14:
    {
        return state->screen_death;
    }
    // 15 = spaceship -> the spots in State and GameManager where this should be are both nullptr -> has no UI
    case 16:
    {
        return state->screen_win;
    }
    case 17:
    {
        return state->screen_credits;
    }
    case 18:
    {
        return state->screen_scores;
    }
    case 19:
    {
        return state->screen_constellation;
    }
    case 20:
    {
        return state->screen_recap;
    }
    case 21:
    {
        return state->screen_arena_menu;
    }
    case 22:
    case 24:
    {
        return state->screen_arena_stages_select1;
    }
    case 23:
    {
        return state->screen_arena_items;
    }
    case 25:
    {
        return state->screen_arena_intro;
    }
    case 26:
    {
        // the one in GameManager has no UI entries
        return state->screen_arena_level;
    }
    case 27:
    {
        return state->screen_arena_score;
    }
    case 28:
    {
        return game_manager->screen_online_loading;
    }
    case 29:
    {
        return game_manager->screen_online_lobby;
    }
    }
    DEBUG("Screen pointer requested for unknown screen ID: {}", screen_id);
    return nullptr;
}

JournalPageStory* JournalPageStory::construct(bool right_side, uint32_t pn)
{
    static auto journal_storypage_vtable = get_address("vftable_JournalPages") + JOURNAL_VFTABLE::STORY;

    size_t* mem = (size_t*)game_malloc(0x58);
    *mem = journal_storypage_vtable;
    JournalPageStory* page = (JournalPageStory*)mem;
    page->page_number = pn;

    page->background.y = 0;
    page->background.dest_set_quad(Quad(AABB(-0.775f, 0.888888f, 0.775f, -0.888888f)));

    if (right_side == false)
    {
        page->background.x = 0.225f;
        page->background.source_set_quad(Quad(AABB(0.1125f, 0, 0.5f, 1.0f)));
    }
    else
    {
        page->background.x = -0.225f;
        page->background.source_set_quad(Quad(AABB(0.5f, 0, 0.8875f, 1.0f)));
    }
    return page;
}

void JournalPage::set_page_background_side(bool right)
{
    if (right)
    {
        if (is_right_side_page())
            return;

        this->background.x *= -1;
        this->background.source_set_quad(Quad(AABB(0.5f, 0, 0.8875f, 1.0f)));
    }
    else
    {
        if (!is_right_side_page())
            return;

        this->background.x *= -1;
        this->background.source_set_quad(Quad(AABB(0.1125f, 0, 0.5f, 1.0f)));
    }
}
JOURNAL_PAGE_TYPE JournalPage::get_type()
{
    static const size_t journal_vftable = get_address("vftable_JournalPages"sv);
    switch (*(size_t*)this - journal_vftable)
    {
    case JOURNAL_VFTABLE::PROGRESS:
        return JournalPageType::Progress;
    case JOURNAL_VFTABLE::MENU:
        return JournalPageType::JournalMenu;
    case JOURNAL_VFTABLE::PLACES:
        return JournalPageType::Places;
    case JOURNAL_VFTABLE::PEOPLE:
        return JournalPageType::People;
    case JOURNAL_VFTABLE::BESTIARY:
        return JournalPageType::Bestiary;
    case JOURNAL_VFTABLE::ITEMS:
        return JournalPageType::Items;
    case JOURNAL_VFTABLE::TRAPS:
        return JournalPageType::Traps;
    case JOURNAL_VFTABLE::STORY:
        return JournalPageType::Story;
    case JOURNAL_VFTABLE::FEATS:
        return JournalPageType::Feats;
    case JOURNAL_VFTABLE::DEATH_CAUSE:
        return JournalPageType::DeathCause;
    case JOURNAL_VFTABLE::DEATH_MENU:
        return JournalPageType::DeathMenu;
    case JOURNAL_VFTABLE::RECAP:
        return JournalPageType::Recap;
    case JOURNAL_VFTABLE::PLAYER_PROFILE:
        return JournalPageType::PlayerProfile;
    case JOURNAL_VFTABLE::LAST_GAME_PLAYED:
        return JournalPageType::LastGamePlayed;
    default:
        return (JournalPageType)(-1);
    }
}

void force_journal(uint32_t chapter, uint32_t entry)
{
    auto gm = get_game_manager();
    static const auto journal_popup_open = get_address("journal_popup_open"sv);
    if (chapter > 2)
    {
        gm->save_related->journal_popup_ui.chapter_to_show = chapter;
        gm->save_related->journal_popup_ui.entry_to_show = entry;
        gm->save_related->journal_popup_ui.timer = 0;
        gm->save_related->journal_popup_ui.slide_position = 0;
        // NOP the check for JournalPopupUI being visible in toggle_journal to always open the given entry
        write_mem_recoverable("journal_popup_hack", journal_popup_open, "\x90\x90"sv, true);
    }
    else
    {
        recover_mem("journal_popup_hack");
    }
}

void toggle_journal()
{
    auto gm = get_game_manager();
    typedef void show_journal_func(JournalUI*, size_t);
    static show_journal_func* show = (show_journal_func*)(get_address("toggle_journal"sv));
    show(gm->journal_ui, heap_base());
}

void show_journal(JOURNALUI_PAGE_SHOWN chapter, uint32_t page)
{
    auto gm = get_game_manager();
    if (chapter > 2 && chapter < 10 && gm->journal_ui->state == 0)
    {
        on_open_journal_chapter(gm->journal_ui, 2, false, true);
        if (gm->journal_ui->state == 1 && gm->journal_ui->chapter_shown == 2)
            on_open_journal_chapter(gm->journal_ui, chapter, false, false);
    }
    else
    {
        on_open_journal_chapter(gm->journal_ui, chapter, false, true);
    }
    if (chapter > 2 && chapter < 10 && gm->journal_ui->chapter_shown == chapter && page > 0)
    {
        gm->journal_ui->current_page = page;
        gm->journal_ui->flipping_to_page = page;
    }
}

std::optional<uint32_t> ScreenCodeInput::get_seed()
{
    if (code_length == 0)
        return std::nullopt;
    std::wstringstream ss;
    std::wstring seed_str;
    for (uint8_t i = 0; i < code_length; ++i)
        seed_str.push_back((wchar_t)(code_chars[i]));
    ss << std::hex << seed_str;
    uint32_t seed{0};
    ss >> seed;
    return seed;
}

void ScreenCodeInput::set_seed(std::optional<uint32_t> seed, std::optional<uint8_t> length)
{
    uint8_t len = length.value_or(8);
    if (len > 8)
        len = 8;
    if (seed.has_value())
    {
        std::wstringstream ss;
        ss << std::uppercase << std::hex << std::setw(len) << std::setfill(L'0') << seed.value();
        memcpy(code_chars, ss.str().c_str(), len * 2);
        code_length = len;
    }
    else
    {
        code_length = 0;
    }
}
