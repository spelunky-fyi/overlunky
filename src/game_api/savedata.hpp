#pragma once

#include <array>
#include <cstdint>

#pragma pack(push, 1)

struct SaveData
{
    std::array<bool, 16> places;
    std::array<bool, 78> bestiary;
    std::array<bool, 38> people;
    std::array<bool, 54> items;
    std::array<bool, 24> traps;
    int8_t skip1[2];
    int32_t time_tutorial;
    char last_daily[8];
    uint8_t show_longer_journal_popup_count; // the next n times the 'Journal entry added' popup is shown, it's shown for 300 frames instead of 180
    int8_t skip2[3];
    uint32_t characters;
    int8_t tutorial_state;
    uint8_t shortcuts;
    int8_t skip3[2];
    std::array<int32_t, 78> bestiary_killed;
    std::array<int32_t, 78> bestiary_killed_by;
    std::array<int32_t, 38> people_killed;
    std::array<int32_t, 38> people_killed_by;
    int32_t plays;
    int32_t deaths;
    int32_t wins_normal;
    int32_t wins_hard;
    int32_t wins_special;
    int64_t score_total;
    int32_t score_top;
    uint8_t deepest_area;
    uint8_t deepest_level;
    int8_t skip4[1022];                                            //TODO
    std::array<std::array<uint32_t, 255>, 8> deathcount_per_level; // 8 themes, 255 uint32_t's for each level
    int64_t time_total;
    int32_t time_best;
    std::array<int32_t, 20> character_deaths;
    std::array<uint8_t, 3> pets_rescued;

    // 1-based theme id into this array (0 = invalid theme, 19 = unknown)
    // bool is set during transition with a theme change, except for CO, basecamp, arena
    // due to tiamat and hundun, bool for neobab/sunkencity is only set when finishing these as 7-98
    std::array<bool, 20> completed_themes;

    bool completed_normal;
    bool completed_ironman;
    bool completed_hard;
    bool profile_seen;
    bool seeded_unlocked;
    uint8_t world_last;
    uint8_t level_last;
    uint8_t theme_last;
    int8_t skip7;
    uint32_t score_last;
    uint32_t time_last;
    std::array<int32_t, 20> stickers;
    int8_t skip8[40];                     // first dword is a mask(?) that determines horizontal spacing between stickers
    std::array<float, 20> sticker_angles; // rotation angle for each sticker
    int8_t skip9[40];
    std::array<float, 20> sticker_vert_offsets; // vertical offset for each sticker
    int8_t skip10[40];
    std::array<uint8_t, 4> players;
};

#pragma pack(pop)
