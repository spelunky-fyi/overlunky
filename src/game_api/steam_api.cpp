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

using UnlockedFeatFun = bool(FEAT);
bool get_feat(FEAT feat)
{
    if (--feat > 31)
        return false;
    auto memory = Memory::get();
    auto uff = (UnlockedFeatFun*)memory.at_exe(0x22cc4c70);
    return uff(feat);
}

uint32_t g_hidden_feat_mask{0};
void change_feat(FEAT feat, std::u16string_view name, std::u16string_view description, bool hidden)
{
    if (--feat > 31)
        return;

    static const STRINGID first_feat = hash_to_stringid(0x335dbbd4); // The Full Spelunky
    change_string(first_feat + feat, name);
    change_string(first_feat + feat + 33, description);
    // Make up your mind! Is it feats, achievements or trophies?
    if (feat == 0)
        change_string(first_feat + 32, description);

    auto memory = Memory::get();
    auto offset = memory.at_exe(0x22c720d7); // TODO: It's the 32bit hardcoded mask for hidden feats (0xfc007e18U)
    g_hidden_feat_mask = read_u32(offset);
    if (hidden)
        g_hidden_feat_mask |= (1U << feat);
    else
        g_hidden_feat_mask &= ~(1U << feat);
    write_mem_recoverable("hidden_feats", offset, g_hidden_feat_mask, true);
}

using GetFeatFun = bool(FEAT);
GetFeatFun* g_get_feat_trampoline{nullptr};
std::function<bool(FEAT)> g_get_feat{nullptr};

using SetFeatFun = void(FEAT);
SetFeatFun* g_set_feat_trampoline{nullptr};
std::function<void(FEAT)> g_set_feat{nullptr};

bool feat_unlocked(uint8_t feat)
{
    if (g_get_feat)
        return g_get_feat(feat + 1);
    else if (g_get_feat_trampoline)
        return g_get_feat_trampoline(feat);
    return false;
}

void unlock_feat(uint8_t feat)
{
    on_feat(feat);
    if (g_set_feat)
        g_set_feat(feat + 1);
    else if (g_set_feat_trampoline)
        g_set_feat_trampoline(feat);
}

void init_achievement_hooks()
{
    static bool hooked = false;
    if (!hooked)
    {
        auto memory = Memory::get();
        g_get_feat_trampoline = (GetFeatFun*)memory.at_exe(0x22cc4c70); // TODO: It's the function that calls steamapi getachievement
        g_set_feat_trampoline = (SetFeatFun*)memory.at_exe(0x22cc45b0); // TODO: It's the function that calls steamapi setachievement

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

void hook_get_feat(std::function<bool(FEAT)> func = nullptr)
{
    g_get_feat = func;
}

void hook_set_feat(std::function<void(FEAT)> func = nullptr)
{
    g_set_feat = func;
}
