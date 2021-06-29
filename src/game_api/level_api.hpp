#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>

struct ShortTileCodeDef
{
    std::uint32_t id;
    std::uint8_t chance;
    std::uint32_t alt_id;
};
struct TileCodeDef
{
    std::uint32_t id;
};
struct TemplateDef
{
    std::uint32_t id;
};
struct ChanceDef
{
    std::uint32_t id;
};
struct LevelChanceDef
{
    std::uint32_t id;
    std::uint8_t chance;
};

struct LevelGenData
{
    void init();

    std::optional<std::uint32_t> get_tile_code(const std::string& tile_code);
    std::uint32_t define_tile_code(std::string tile_code);

    std::optional<std::uint32_t> get_chance(const std::string& chance);
    std::uint32_t define_chance(std::string chance);

    // TODO: Get offsets from binary instead of hardcoding them
    const std::unordered_map<std::uint8_t, ShortTileCodeDef>& short_tile_codes() const
    {
        return *(const std::unordered_map<std::uint8_t, ShortTileCodeDef>*)((size_t)this + 0x48);
    }
    const std::unordered_map<std::string, TileCodeDef>& tile_codes() const
    {
        return *(const std::unordered_map<std::string, TileCodeDef>*)((size_t)this + 0x88);
    }
    const std::unordered_map<std::string, TemplateDef>& templates() const
    {
        return *(const std::unordered_map<std::string, TemplateDef>*)((size_t)this + 0xC8);
    }
    const std::unordered_map<std::string, ChanceDef>& chances() const
    {
        return *(const std::unordered_map<std::string, ChanceDef>*)((size_t)this + 0x1330);
    }
    const std::unordered_map<std::uint32_t, LevelChanceDef>& level_chances() const
    {
        return *(const std::unordered_map<std::uint32_t, LevelChanceDef>*)((size_t)this + 0x1370);
    }
};

struct DoorCoords
{
    float door1_x;
    float door1_y;
    float door2_x; // door2 only valid when there are two in the level, like Volcana drill, Olmec, ..."
    float door2_y;
};

struct ThemeInfo
{
    void** __vftable;
    bool unknown1; // gets set to false for the jungle and temple theme in the cosmic ocean
    bool unknown2;
    uint8_t padding1;
    uint8_t padding2;
    uint32_t padding3;
    ThemeInfo* sub_theme;
    uint32_t unknown3;
    uint32_t unknown4;
};
static_assert(sizeof(ThemeInfo) == 0x20);

struct LevelGenRooms
{
    std::array<uint16_t, 8 * 16> rooms;
};

struct LevelGenRoomsMeta
{
    std::array<bool, 8 * 16> rooms;
};

struct LevelGenSystem
{
    void init();

    LevelGenData* data;
    uint64_t unknown2;
    union
    {
        ThemeInfo* themes[18];
        struct
        {
            ThemeInfo* theme_dwelling;
            ThemeInfo* theme_jungle;
            ThemeInfo* theme_volcana;
            ThemeInfo* theme_olmec;
            ThemeInfo* theme_tidepool;
            ThemeInfo* theme_temple;
            ThemeInfo* theme_icecaves;
            ThemeInfo* theme_neobabylon;
            ThemeInfo* theme_sunkencity;
            ThemeInfo* theme_cosmicocean;
            ThemeInfo* theme_city_of_gold;
            ThemeInfo* theme_duat;
            ThemeInfo* theme_abzu;
            ThemeInfo* theme_tiamat;
            ThemeInfo* theme_eggplantworld;
            ThemeInfo* theme_hundun;
            ThemeInfo* theme_basecamp;
            ThemeInfo* theme_arena;
        };
    };
    uint64_t unknown21;
    void* unknown22; // MysteryPointer1
    void* unknown23; // MysteryPointer1
    LevelGenRooms* rooms_frontlayer;
    LevelGenRooms* rooms_backlayer;
    LevelGenRoomsMeta* rooms_meta_26;
    LevelGenRoomsMeta* rooms_meta_27;
    LevelGenRoomsMeta* rooms_meta_28;
    LevelGenRoomsMeta* backlayer_room_exists;
    LevelGenRoomsMeta* rooms_meta_29;
    LevelGenRoomsMeta* rooms_meta_31;
    LevelGenRoomsMeta* rooms_meta_32;
    LevelGenRoomsMeta* rooms_meta_33;
    LevelGenRoomsMeta* rooms_meta_34;
    std::uint32_t spawn_room_x;
    std::uint32_t spawn_room_y;
    float spawn_x;
    float spawn_y;
    DoorCoords* exit_doors_locations;
    void* unknown37;
    void* unknown38;
    uint16_t flags;
    int8_t unknown40;
    uint8_t unknown41;
    uint8_t unknown42;
    uint8_t unknown43;
    uint8_t unknown44;
    uint8_t unknown45;
    uint8_t unknown46;
    uint8_t unknown47;
    uint8_t unknown48;
    uint32_t unknown49;
    uint32_t unknown50;
    uint32_t unknown51;
};
