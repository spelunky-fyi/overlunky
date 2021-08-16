#pragma once

#include <array>
#include <cstdint>

struct SaveData
{
    std::array<bool, 16> places;
    std::array<bool, 78> bestiary;
    std::array<bool, 38> people;
    std::array<bool, 54> items;
    std::array<bool, 24> traps;
    int8_t bd2;
    int8_t bd3;
    int32_t best_time_speedrun;
    char last_daily[8];
    int8_t be1;
    uint32_t characters;
    int8_t tutorial_state;
    uint8_t shortcuts;
    int8_t bea;
    int8_t beb;
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
    uint8_t skip1[9674]; //TODO: there's still a lot of stuff here, but I just wanted to get to the player selections
    std::array<uint8_t, 4> players;
};
