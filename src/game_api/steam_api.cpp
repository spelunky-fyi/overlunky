#include "steam_api.hpp"

#include <array>     // for array, _Array_const_iterator
#include <detours.h> // for DetourAttach, DetourTransactionBegin

#include "memory.hpp" // for vtable_find
#include "script/events.hpp"
#include "search.hpp" // for get_address
#include "strings.hpp"
#include "vtable_hook.hpp" // for get_hook_function, register_hook_function

class ISteamUserStats;

class ISteamUserStats* get_steam_user_stats()
{
    using GetSteamUserStatsFun = void(ISteamUserStats**);
    static GetSteamUserStatsFun* get_steam_user_stats_impl = *(GetSteamUserStatsFun**)get_address("get_steam_user_stats");

    ISteamUserStats* steam_user_stats{nullptr};
    get_steam_user_stats_impl(&steam_user_stats);
    return steam_user_stats;
}

void enable_steam_achievements()
{
    ISteamUserStats* steam_user_stats = get_steam_user_stats();
    if (steam_user_stats != nullptr && get_hook_function((void***)steam_user_stats, 0x7))
    {
        unregister_hook_function((void***)steam_user_stats, 0x7);
    }
}

void disable_steam_achievements()
{
    ISteamUserStats* steam_user_stats = get_steam_user_stats();
    if (steam_user_stats != nullptr && !get_hook_function((void***)steam_user_stats, 0x7))
    {
        constexpr auto nop_set_achievement = [](ISteamUserStats*, const char*)
        {
            return true;
        };

        using ISteamUserStats_SetAchievement = bool(ISteamUserStats*, const char*);
        register_hook_function(
            (void***)steam_user_stats,
            0x7,
            (ISteamUserStats_SetAchievement*)nop_set_achievement);
    }
}

void reset_all_steam_achievements()
{
    ISteamUserStats* steam_user_stats = get_steam_user_stats();
    if (steam_user_stats != nullptr && !get_hook_function((void***)steam_user_stats, 0x7))
    {
        using ISteamUserStats_ResetAchievement = bool(ISteamUserStats*, const char*);
        ISteamUserStats_ResetAchievement* reset_achievement = *vtable_find<ISteamUserStats_ResetAchievement*>(steam_user_stats, 0x8);

        for (auto achievement_id : g_AllAchievements)
        {
            reset_achievement(steam_user_stats, achievement_id);
        }
    }
}

using GetFeatFun = bool(FEAT);
GetFeatFun* g_get_feat_trampoline{nullptr};
std::function<bool(FEAT)> g_get_feat{nullptr};

using SetFeatFun = void(FEAT);
SetFeatFun* g_set_feat_trampoline{nullptr};
std::function<void(FEAT)> g_set_feat{nullptr};

void set_feat_hidden(FEAT feat, bool hidden)
{
    if (--feat > 31)
        return;
    auto offset = get_address("get_feat_hidden"sv);
    auto mask = read_u32(offset);
    if (hidden)
        mask |= (1U << feat);
    else
        mask &= ~(1U << feat);
    write_mem_recoverable("hidden_feats", offset, mask, true);
}

bool get_feat_hidden(FEAT feat)
{
    if (--feat > 31)
        return false;
    auto offset = get_address("get_feat_hidden"sv);
    auto mask = read_u32(offset);
    return (mask & (1U << feat)) > 0;
}

bool feat_unlocked(uint8_t feat)
{
    auto pre = pre_get_feat(feat + 1);
    if (pre.has_value())
        return pre.value();
    else if (g_get_feat_trampoline)
        return g_get_feat_trampoline(feat);
    return false;
}

void unlock_feat(uint8_t feat)
{
    if (!pre_set_feat(feat + 1) && g_set_feat_trampoline)
        g_set_feat_trampoline(feat);
    on_feat(feat);
}

std::tuple<bool, bool, const char16_t*, const char16_t*> get_feat(FEAT feat)
{
    if (--feat > 31)
        return std::make_tuple(false, false, u"", u"");

    static const STRINGID first_feat = hash_to_stringid(0x335dbbd4); // The Full Spelunky
    auto data = std::make_tuple(feat_unlocked(feat), get_feat_hidden(feat + 1), get_string(first_feat + feat), get_string(first_feat + feat + 33));
    return data;
}

void change_feat(FEAT feat, bool hidden, std::u16string_view name, std::u16string_view description)
{
    if (--feat > 31)
        return;

    static const STRINGID first_feat = hash_to_stringid(0x335dbbd4); // The Full Spelunky
    change_string(first_feat + feat, name);
    change_string(first_feat + feat + 33, description);
    // Make up your mind! Is it feats, achievements or trophies?
    if (feat == 0)
        change_string(first_feat + 32, description);

    set_feat_hidden(feat + 1, hidden);
}

void init_achievement_hooks()
{
    static bool hooked = false;
    if (!hooked)
    {
        g_get_feat_trampoline = (GetFeatFun*)get_address("get_feat"sv);
        g_set_feat_trampoline = (SetFeatFun*)get_address("set_feat"sv);

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());

        DetourAttach((void**)&g_get_feat_trampoline, &feat_unlocked);
        DetourAttach((void**)&g_set_feat_trampoline, &unlock_feat);

        const LONG error = DetourTransactionCommit();
        if (error != NO_ERROR)
        {
            DEBUG("Failed hooking feats: {}\n", error);
        }

        hooked = true;
    }
}
