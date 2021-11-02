#include "screen.hpp"

#include <algorithm>
#include <functional>
#include <vector>

#include "vtable_hook.hpp"

template <class FunT>
struct HookWithId
{
    std::uint32_t id;
    std::function<FunT> fun;
};

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
    hook_vtable_no_dtor<void(Screen*)>(
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
        },
        0x3);
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

void ScreenOnlineLobby::set_code(const std::string& code)
{
    if (code.length() != 8)
    {
        characters_entered_count = 0;
        return;
    }
    std::string code_upper = code;
    std::transform(code_upper.begin(), code_upper.end(), code_upper.begin(), [](unsigned char c)
                   { return (unsigned char)std::toupper(c); });

    for (size_t x = 0; x < 8; ++x)
    {
        const unsigned char c = code.at(x);
        auto valid = (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F');
        if (!valid)
        {
            characters_entered_count = 0;
            return;
        }
        code_chars[x] = c;
    }
    characters_entered_count = 8;
}

JournalPage* JournalUI::get_journal_page(size_t page_number)
{
    if (page_number < pages.size())
    {
        return pages.at(page_number);
    }
    return nullptr;
}
