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
    int8_t skip2[4];
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
    int8_t skip4[9182]; //TODO
    int64_t time_total;
    int32_t time_best;
    std::array<int32_t, 20> character_deaths;
    std::array<uint8_t, 3> pets_rescued;
    int8_t skip6[20];
    bool completed_normal;
    bool completed_ironman;
    bool completed_hard;
    bool profile_seen;
    bool seeded_unlocked;
    uint8_t world_last;
    uint8_t level_last;
    int8_t skip7[2];
    uint32_t score_last;
    uint32_t time_last;
    std::array<int32_t, 9> stickers;
    int8_t skip8[324];
    std::array<uint8_t, 4> players;
};

#pragma pack(pop)
