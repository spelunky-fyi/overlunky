#include "steam_api.hpp"

#include <array> // for array, _Array_const_iterator

#include "memory.hpp"      // for vtable_find
#include "search.hpp"      // for get_address
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

        constexpr std::array c_AllAchievements = {
            "6E0C60E83AC07309",
            "B03193B1D35645AC",
            "81B12D01899DD911",
            "C356BD3F920AA007",
            "CD2247AB88095173",
            "3080F6BD3EFFD04E",
            "0D471B6E7F899BC9",
            "381FE256BB6A3D93",
            "7039963F98FB58A5",
            "5C97977A1C41E1D8",
            "12BB5BD07F56194C",
            "EA488CC02AD233FD",
            "1BFD11B72624F4C6",
            "887316D012E74D3B",
            "5B7F2E4EAEC18E51",
            "BB2966DD89D2C3E9",
            "31F3186C15C42794",
            "E93DBDD33881A338",
            "3DF7CAAF05559953",
            "B8604E694E6449F3",
            "84D574F017DC65B9",
            "7D7B995A1ED5E7A7",
            "468F80D65DD09F9E",
            "37801BFF5481B550",
            "7EFF7F7E6B9D813F",
            "ECBEF23A87A0737A",
            "B7EFFD56C8457082",
            "061E03E6CA94CA71",
            "4F080C487BB27C26",
            "112E2F91AC19A57A",
            "710891CB8FE6D822",
            "C999E58F1EF15759",
        };
        for (const char* achievement_id : c_AllAchievements)
        {
            reset_achievement(steam_user_stats, achievement_id);
        }
    }
}
