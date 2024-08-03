#pragma once

#include <array>
#include <cstdint>

#include "state_structs.hpp"

#pragma pack(push, 1)

struct SaveGameArenaRuleset
{
    uint8_t unknown1;
    uint8_t unknown12;
    uint8_t timer;
    uint8_t timer_ending;
    uint8_t wins;
    uint8_t lives;
    uint8_t unknown7;
    uint8_t unknown8;
    std::array<uint16_t, 4> unused_player_idolheld_countdown; // struct is similar to state.arenas so they just copied it, but this is not useful to store in the savegame
    uint8_t health;
    uint8_t bombs;
    uint8_t ropes;
    uint8_t stun_time;
    uint8_t mount;
    uint8_t arena_select;
    ArenaConfigArenas arenas;
    uint8_t dark_level_chance;
    uint8_t crate_frequency;
    ArenaConfigItems items_enabled;
    ArenaConfigItems items_in_crate;
    int8_t held_item;
    int8_t equipped_backitem;
    ArenaConfigEquippedItems equipped_items;
    uint8_t whip_damage;
    bool final_ghost;
    uint8_t breath_cooldown;
    bool punish_ball;
    uint8_t padding[2];
};

struct ConstellationStar
{
    uint32_t type;
    float x;
    float y;
    float size;
    float red;
    float green;
    float blue;
    float alpha;
    float halo_red;
    float halo_green;
    float halo_blue;
    float halo_alpha;
    bool canis_ring;
    bool fidelis_ring;
    uint8_t padding[2];
    uint32_t unknown14; // might have something to do with how they are laid out on the path, having/being offshoots etc
};

struct ConstellationLine
{
    uint8_t from; // zero based star index into Constellation.stars
    uint8_t to;
};

struct Constellation
{
    uint8_t star_count;
    uint8_t unknown[3]; // possibly something?
    std::array<ConstellationStar, 45> stars;
    float scale;
    uint8_t line_count;
    std::array<ConstellationLine, 90> lines; // You'd only need 44 lines if you have 45 stars, but there is room for 91. Could be to draw two lines on top of each other to make it brighter.
    uint8_t padding[3];
    float line_red_intensity; // 0 = normal white, npc_kills 8 - 16 = 0.5 - 1.0 (pink to deep red for criminalis)
};

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
    /// 20bit bitmask of unlocked characters
    uint32_t characters;
    /// Tutorial state 0..4. Changes the camp layout, camera and lighting. (0=nothing, 1=journal got, 2=key spawned, 3=door unlocked, 4=complete)
    int8_t tutorial_state;
    /// Terra quest state 0..10 (0=not met ... 10=complete)
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
    int8_t skip4[1022];                                            // TODO
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
    std::array<ENT_TYPE, 20> stickers;
    int8_t skip8[40];                     // first dword is a mask(?) that determines horizontal spacing between stickers
    std::array<float, 20> sticker_angles; // rotation angle for each sticker
    int8_t skip9[40];
    std::array<float, 20> sticker_vert_offsets; // vertical offset for each sticker
    int8_t skip10[40];
    std::array<uint8_t, 4> players;
    SaveGameArenaRuleset arena_favorite_ruleset;
    Constellation constellation;
};

#pragma pack(pop)
