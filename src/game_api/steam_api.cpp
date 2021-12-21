#include "steam_api.hpp"

#include "search.hpp"
#include "vtable_hook.hpp"

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
