#pragma once

#include <cstdint>

struct SaveData
{
    bool places[16];
    bool bestiary[78];
    bool people[38];
    bool items[54];
    bool traps[24];
    int8_t bd2;
    int8_t bd3;
    int32_t id4;
    char last_daily[8];
    int8_t be1;
    uint32_t characters;
    int8_t be8;
    uint8_t shortcuts;
    int8_t bea;
    int8_t beb;
    int32_t bestiary_killed[78];
    int32_t bestiary_killed_by[78];
    int32_t people_killed[38];
    int32_t people_killed_by[38];
    int32_t plays;
    int32_t deaths;
    int32_t wins_normal;
    int32_t wins_hard;
    int32_t wins_special;
    int64_t score_total;
    int32_t score_top;
    uint8_t deepest_area;
    uint8_t deepest_level;
};
