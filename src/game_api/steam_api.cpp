#include "steam_api.hpp"

#include "search.hpp"
#include "vtable_hook.hpp"

void disable_steam_achievements()
{
    class ISteamUserStats;
    using GetSteamUserStatsFun = void(ISteamUserStats**);
    GetSteamUserStatsFun* get_steam_user_stats = *(GetSteamUserStatsFun**)get_address("get_steam_user_stats");

    ISteamUserStats* steam_user_stats{nullptr};
    get_steam_user_stats(&steam_user_stats);
    if (steam_user_stats != nullptr)
    {
        using ISteamUserStats_SetAchievement = bool(ISteamUserStats*, const char*);
        hook_vtable_no_dtor<ISteamUserStats_SetAchievement>(
            steam_user_stats,
            [](ISteamUserStats*, const char*, ISteamUserStats_SetAchievement*)
            {
                return true;
            },
            0x7);
    }
}
