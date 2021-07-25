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
struct RoomTemplateDef
{
    std::uint16_t id;
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

struct ChanceLogicProvider
{
    std::function<bool(float, float, int)> is_valid;
    std::function<void(float, float, int)> do_spawn;
};

struct LevelGenData
{
    void init();

    std::optional<std::uint32_t> get_tile_code(const std::string& tile_code);
    std::uint32_t define_tile_code(std::string tile_code);

    std::optional<std::uint32_t> get_chance(const std::string& chance);
    std::uint32_t define_chance(std::string chance);

    std::uint32_t register_chance_logic_provider(std::uint32_t chance_id, ChanceLogicProvider provider);
    void unregister_chance_logic_provider(std::uint32_t provider_id);

    // TODO: Get offsets from binary instead of hardcoding them
    const std::unordered_map<std::uint8_t, ShortTileCodeDef>& short_tile_codes() const
    {
        return *(const std::unordered_map<std::uint8_t, ShortTileCodeDef>*)((size_t)this + 0x48);
    }
    const std::unordered_map<std::string, TileCodeDef>& tile_codes() const
    {
        return *(const std::unordered_map<std::string, TileCodeDef>*)((size_t)this + 0x88);
    }
    const std::unordered_map<std::string, RoomTemplateDef>& room_templates() const
    {
        return *(const std::unordered_map<std::string, RoomTemplateDef>*)((size_t)this + 0xC8);
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

class ThemeInfo
{
  public:
    bool unknown1; // gets set to false for the jungle and temple theme in the cosmic ocean
    bool unknown2;
    uint8_t padding1;
    uint8_t padding2;
    uint32_t padding3;
    ThemeInfo* sub_theme;
    uint32_t unknown3;
    uint32_t unknown4;

    virtual ~ThemeInfo();
    virtual bool get_unknown1() = 0;

    // dwelling,tidepool: unset levelgen.flags.flag12
    // jungle,volcana.olmec,icecaves,neobab,cog,duat,abzu,tiamat,eggplant,hundun,basecamp,arena: nop
    // temple: unset levelgen.flags.flag10 + calculate chance of grasshopper critter spawn
    // sunken: unset levelgen.flags.flag9, 10, 17, 18 + if state.level == 1 -> unset flag 11
    // cosmic: calls same virtual on its sub_theme
    virtual void initialize_flags() = 0;

    // does random calculations and calls function to determine the start room in most themes
    virtual void initialize_levelgen() = 0;

    // most themes call the same function, some check whether they are in CO
    virtual void unknown_v4() = 0;

    virtual void unknown_v5() = 0;

    // dwelling: does stuff when level == 4 or udjat present
    // jungle: when black market present
    // volcana: when drill present
    // touches the rooms and sometimes the meta info about the rooms
    virtual void handle_level_specialities() = 0;

    // can't trigger, dwelling (quillback) and abzu do something special (arena just returns)
    virtual void unknown_v7() = 0;

    // does something depending on levelgen.data.unknown7
    virtual void unknown_v8() = 0;

    virtual void insert_shopkeeper_vault() = 0;

    virtual void insert_coffin() = 0;

    // metal clanking and air of oppression
    virtual void set_theme_specific_level_feeling() = 0;

    // disable this and only the player is spawned in the level
    virtual void populate_level() = 0;

    virtual void add_level_bordertiles() = 0;

    // volcana: checks if state.coffin_contents = cocovondiamonds -> chooses one of the four coffins in vlad's castle at random to put her in
    // tidepool: spawns impostor lake, some door and a litwalltorch, ...
    // neobab: assigns the correct animation_frame to all the ushabtis
    // co, arena: forwards to the same virtual of the sub_theme
    // abzu, tiamat: spawns impostor lake
    virtual void post_process_level() = 0;

    // quillback: adds butterflies and snails + configures the door to go to the correct theme/level
    // olmec: same thing but crabs and grasshoppers
    // temple: spawns COG door
    // co, arena: forwards to the same virtual of the sub_theme
    // other themes also do stuff, but nothing seemingly important
    virtual void post_process_exitdoors() = 0;

    // pleasure palace: applies correct texture to ladders and ladder platforms
    // sunken city: randomly adds ENT_TYPE_DECORATION_SUNKEN_BRIDGE between entities (the slimy bridges)
    // hundun: calls sunken city virtual, so has slimy bridges
    // co: does nothing, so no slimy bridges in CO sunken city levels! (forgotten/bug?)
    virtual void post_process_entities() = 0;

    // adds the background, but also the compass indicator at the door
    virtual void populate_background() = 0;

    // adds random beautification to the background, e.g. the chalk drawings of the three people in dwelling
    virtual void populate_background_beautification() = 0;

    // adds extra light where needed, e.g. in the udjat chest room, or the top layer of the black market: spawns ENT_TYPE_LOGICAL_ROOM_LIGHT
    virtual void populate_extra_lighting() = 0;

    virtual void populate_level_transition() = 0;

    // unsets flag 1 (Reset) of state.quest_flags
    // sets the correct state.screen (0xC)
    // sets state.ingame to true, adjust fade values and starts loading
    virtual void on_level_transition() = 0;

    // spawns the player(s) in the world, along with what they were holding
    virtual void populate_players() = 0;

    // when disabled, during multiplayer the camera is not focused; also responsible for spawning the leader flag; not looked at in detail
    virtual void handle_multiplayer() = 0;

    // the .lvl file to load (e.g. dwelling = dwellingarea.lvl except when level == 4 (cavebossarea.lvl))
    virtual const char* level_file_to_load() = 0;

    // for co: returns sub_theme->theme_id()
    virtual uint8_t theme_id() = 0;

    // whereas theme_id() returns a unique id for all ThemeInfo's, this function returns the id of the theme it logically belongs to
    // e.g. theme_abzu->theme_base_id() = 5 (tide_pool) as opposed to theme_abzu->theme_id() = 13 (abzu)
    virtual uint8_t theme_base_id() = 0;

    // all themes return 4 (ENT_TYPE_FLOOR_GENERIC), except:
    // temple: 104 (ENT_TYPE_FLOORSTYLED_TEMPLE)
    // neobab: 106 (ENT_TYPE_FLOORSTYLED_BABYLON)
    // sunken: 107 (ENT_TYPE_FLOORSTYLED_SUNKEN)
    // cog: 110 (ENT_TYPE_FLOORSTYLED_COG)
    // duat: 112 (ENT_TYPE_FLOORSTYLED_DUAT)
    // hundun: 107 (ENT_TYPE_FLOORSTYLED_SUNKEN)
    // strangely, it's only applied to an odd block here and there
    virtual uint32_t random_block_floorstyle() = 0;

    // similar to random_block_floorstyle(), except now the default = 103 (ENT_TYPE_FLOORSTYLED_STONE)
    virtual uint32_t random_block_floorstyle_2() = 0;

    // all return false, except olmec, temple, neobab, cog, duat
    virtual bool unknown_v29() = 0;

    // determines the types of FLOOR_TUNNEL_NEXT/CURRENT (depending on where you are transitioning from/to) for this theme
    // returns 85 by default, except for: olmec: 15, cog: 23
    virtual uint32_t transition_tunnel_block_modifier() = 0;

    virtual uint32_t unknown_v31() = 0;

    // always returns 778 ENT_TYPE_BG_LEVEL_BACKWALL
    virtual uint32_t backwall_entity_id() = 0;

    // returns ENT_TYPE_FLOOR_BORDERTILE by default, except:
    // neobab ENT_TYPE_FLOOR_BORDERTILE_METAL
    // sunken ENT_TYPE_FLOOR_BORDERTILE_OCTOPUS
    // duat ENT_TYPE_FLOOR_DUSTWALL
    // tiamat ENT_TYPE_FLOOR_BORDERTILE_METAL
    // hundun ENT_TYPE_FLOOR_BORDERTILE_OCTOPUS
    virtual uint32_t bordertile_entity_id() = 0;

    virtual uint32_t transition_tunnel_critter_entity_id() = 0;

    // returns -1.0 (downwards) by default, except for sunken/hundun 1.0 (upwards); applies to both lava and water
    virtual float liquid_gravity_direction() = 0;

    // used to make the player invincible in basecamp (but does an OOB check)
    // if you return false in other themes you are invincible except for crushing deaths, and you do still experience knockback and stun
    virtual bool are_players_vulnerable() = 0;

    // returns true by default, except CO, duat
    virtual bool unknown_37() = 0;

    // returns the texture ID for the LUT to be applied to the special back layer, e.g. vlad's castle for the volcana theme
    virtual uint32_t backlayer_lut() = 0;
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
    union
    {
        LevelGenRooms* rooms[2];
        struct
        {
            LevelGenRooms* rooms_frontlayer;
            LevelGenRooms* rooms_backlayer;
        };
    };
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
    uint32_t flags;
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

    std::pair<int, int> get_room_index(float x, float y);
    std::pair<float, float> get_room_pos(int x, int y);
    std::optional<uint16_t> get_room_template(int x, int y, int l);
    bool set_room_template(int x, int y, int l, uint16_t room_template);

    std::string_view get_room_template_name(uint16_t room_template);
};
