#pragma once

using FEAT = uint8_t;

constexpr std::array g_AchievementNames = {
    "THE_FULL_SPELUNKY",
    "YOU_GOT_THIS",
    "FEELS_GOOD",
    "SKILLS_IMPROVING",
    "PERSISTENT",
    "JOURNEYMAN",
    "IRONMAN",
    "SPEEDLUNKY",
    "LOW_SCORER",
    "PILGRIM",
    "MASTER",
    "AWAKENED",
    "EXCAVATOR",
    "TORCHBEARER",
    "SURVIVOR",
    "MILLIONAIRE",
    "SEEN_A_LOT",
    "SEEN_IT_ALL",
    "MAMAS_LITTLE_HELPER",
    "MAMAS_BIG_HELPER",
    "TRACK_STAR",
    "ARENA_CHAMPION",
    "TURKEY_WHISPERER",
    "SUPPORT_A_LOCAL_BUSINESS",
    "VIP",
    "SHADOW_SHOPPER",
    "LEGENDARY",
    "HER_FAVORITE",
    "DIVINE_RIGHT",
    "A_SECOND_CHANCE",
    "CHOSEN_ONE",
    "PARENTHOOD",
};

constexpr std::array g_AllAchievements = {
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

class ISteamUserStats* get_steam_user_stats();
void enable_steam_achievements();
void disable_steam_achievements();
void reset_all_steam_achievements();
void change_feat(FEAT feat, bool hidden, std::u16string_view name, std::u16string_view description);
std::tuple<bool, bool, const char16_t*, const char16_t*> get_feat(FEAT feat);
bool get_feat_hidden(FEAT feat);
void set_feat_hidden(FEAT feat, bool hidden);
void init_achievement_hooks();
