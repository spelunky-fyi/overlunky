#pragma once

#include <array>       // for array
#include <cstddef>     // for byte
#include <cstdint>     // for uint32_t, uint8_t, uint...
#include <functional>  // for function
#include <new>         // for operator new
#include <optional>    // for optional
#include <span>        // for span
#include <string>      // for string, allocator
#include <string_view> // for string_view
#include <type_traits> // for move
#include <utility>     // for pair
#include <vector>      // for vector

#include "aliases.hpp"                       // for LAYER
#include "containers/custom_vector.hpp"      // for custom_vector
#include "containers/game_string.hpp"        // for game_string
#include "containers/game_unordered_map.hpp" // for game_unordered_map
#include "containers/game_vector.hpp"        // for game_vector
#include "level_api_types.hpp"               // for ShortTileCodeDef
#include "math.hpp"                          // for AABB (ptr only), Vec2

class Entity;

struct TileCodeDef
{
    std::uint32_t id;
};
struct RoomTemplateDef
{
    std::uint16_t id;
};
struct RoomData
{
    bool flag0 : 1; // ???
    bool flag1 : 1; // ???
    bool flag2 : 1; // ???
    bool flipped : 1;
    bool dual : 1;
    // 3-bit padding
    uint8_t room_width;
    uint8_t room_height;
    // padding
    const char* room_data;
};
struct RoomTemplateData
{
    game_vector<RoomData> datas;
};
struct ChanceDef
{
    std::uint32_t id;
};
struct LevelChanceDef
{
    game_vector<uint32_t> chances;
};

struct SpawnLogicProvider
{
    std::function<bool(float, float, uint8_t)> is_valid;
    std::function<void(float, float, uint8_t)> do_spawn;
};

enum class RoomTemplateType
{
    VanillaMachineRoom = -1,
    None = 0,
    Entrance = 1,
    Exit = 2,
    Shop = 3,
    MachineRoom = 4,
};

enum class POS_TYPE
{
    FLOOR = 0x1,
    CEILING = 0x2,
    AIR = 0x4,
    WALL = 0x8,
    ALCOVE = 0x10,
    PIT = 0x20,
    HOLE = 0x40,
    WATER = 0x80,
    LAVA = 0x100,
    SAFE = 0x200,
    EMPTY = 0x400,
    SOLID = 0x800,
    DEFAULT = 0x1000,
    WALL_LEFT = 0x2000,
    WALL_RIGHT = 0x4000,
};

struct LevelGenData
{
    void init();

    std::optional<std::uint32_t> get_tile_code(const std::string& tile_code) const
    {
        auto it = tile_codes.find((game_string&)tile_code);
        if (it != tile_codes.end())
        {
            return it->second.id;
        }
        return {};
    }
    std::uint32_t define_tile_code(std::string tile_code);

    std::optional<uint8_t> get_short_tile_code(ShortTileCodeDef short_tile_code_def) const
    {
        for (auto& [i, def] : short_tile_codes)
        {
            if (def == short_tile_code_def)
            {
                return i;
            }
        }
        return {};
    }
    std::optional<ShortTileCodeDef> get_short_tile_code_def(uint8_t short_tile_code) const
    {
        auto it = short_tile_codes.find(short_tile_code);
        if (it != short_tile_codes.end())
        {
            return it->second;
        }
        return {};
    }
    void change_short_tile_code(uint8_t short_tile_code, ShortTileCodeDef short_tile_code_def)
    {
        short_tile_codes[short_tile_code] = short_tile_code_def;
    }
    std::optional<uint8_t> define_short_tile_code(ShortTileCodeDef short_tile_code_def);

    std::optional<std::uint32_t> get_chance(const std::string& chance) const
    {
        if (auto it = monster_chances.find((game_string&)chance); it != monster_chances.end())
        {
            return it->second.id;
        }
        if (auto it = trap_chances.find((game_string&)chance); it != trap_chances.end())
        {
            return it->second.id;
        }
        return {};
    }
    std::uint32_t define_chance(std::string chance);

    std::uint32_t register_chance_logic_provider(std::uint32_t chance_id, SpawnLogicProvider provider);
    void unregister_chance_logic_provider(std::uint32_t provider_id);

    std::uint32_t define_extra_spawn(std::uint32_t num_spawns_front_layer, std::uint32_t num_spawns_back_layer, SpawnLogicProvider provider);
    void set_num_extra_spawns(std::uint32_t extra_spawn_id, std::uint32_t num_spawns_front_layer, std::uint32_t num_spawns_back_layer);
    std::pair<std::uint32_t, std::uint32_t> get_missing_extra_spawns(std::uint32_t extra_spawn_id);
    void undefine_extra_spawn(std::uint32_t extra_spawn_id);

    std::optional<std::uint16_t> get_room_template(const std::string& room_template) const
    {
        auto it = room_templates.find(reinterpret_cast<const game_string&>(room_template));
        if (it != room_templates.end())
            return it->second.id;

        return {};
    }
    std::uint16_t define_room_template(std::string room_template, RoomTemplateType type);
    bool set_room_template_size(std::uint16_t room_template, uint16_t width, uint16_t height);
    RoomTemplateType get_room_template_type(std::uint16_t room_template) const;
    uint16_t get_pretend_room_template(std::uint16_t room_template) const;
    static std::span<const std::pair<std::string_view, uint16_t>> get_missing_room_templates();

    union
    {
        std::array<uint32_t, 17> level_config;
        struct
        {
            uint32_t back_room_chance;
            uint32_t back_room_interconnection_chance;
            uint32_t back_room_hidden_door_chance;
            uint32_t back_room_hidden_door_cache_chance;
            uint32_t mount_chance;
            uint32_t altar_room_chance;
            uint32_t idol_room_chance;
            uint32_t floor_side_spread_chance;
            uint32_t floor_bottom_spread_chance;
            uint32_t background_chance;
            uint32_t ground_background_chance;
            uint32_t machine_bigroom_chance;
            uint32_t machine_wideroom_chance;
            uint32_t machine_tallroom_chance;
            uint32_t machine_rewardroom_chance;
            uint32_t max_liquid_particles;
            uint32_t flagged_liquid_rooms;
            uint32_t unknown_config; // padding
        };
    };

    game_unordered_map<std::uint8_t, ShortTileCodeDef> short_tile_codes;
    game_unordered_map<game_string, TileCodeDef> tile_codes;
    game_unordered_map<game_string, RoomTemplateDef> room_templates;

    game_unordered_map<std::uint16_t, RoomTemplateData> room_template_datas;
    std::array<uint32_t, 429> unknown; // index is tile code id from TileCodeDef
    // uint32_t padding;
    std::array<RoomTemplateData, 8 * 15> set_room_datas;

    game_unordered_map<game_string, ChanceDef> monster_chances;
    game_unordered_map<std::uint32_t, LevelChanceDef> level_monster_chances;

    game_unordered_map<game_string, ChanceDef> trap_chances;
    game_unordered_map<std::uint32_t, LevelChanceDef> level_trap_chances;

    size_t unknown2;
};
static_assert(sizeof(LevelGenData) == 0x1448);

struct SpawnInfo
{
    ROOM_TEMPLATE room_template;
    // probably padding here

    /// Grid entity at this position, will only try to spawn procedural if this is nil
    Entity* grid_entity;
    float x;
    float y;
};

enum class DYNAMIC_TEXTURE : int32_t
{
    INVISIBLE = -2,
    BACKGROUND = -4,
    FLOOR = -5,
    DOOR = -6,
    DOOR_LAYER = -7,
    BACKGROUND_DECORATION = -8,
    KALI_STATUE = -9,
    COFFIN = -10
};

class ThemeInfo
{
  public:
    bool allow_beehive;
    bool allow_leprechaun;
    uint8_t padding1;
    uint8_t padding2;
    uint32_t padding3;
    ThemeInfo* sub_theme;

    virtual ~ThemeInfo(){};

    /// Sets the beehive and leprechaun flags
    virtual void reset_theme_flags() = 0;

    // dwelling,tidepool: unset levelgen.flags.flag12
    // jungle,volcana.olmec,icecaves,neobab,cog,duat,abzu,tiamat,eggplant,hundun,basecamp,arena: nop
    // temple: unset levelgen.flags.flag10 + calculate chance of grasshopper critter spawn
    // sunken: unset levelgen.flags.flag9, 10, 17, 18 + if state.level == 1 -> unset flag 11
    // cosmic: calls same virtual on its sub_theme
    /// Initializes some flags in the LevelGenSystem, also dark level flag in state.level_flags.
    virtual void init_flags() = 0;

    /// Adds the entrance room and sets spawn_room_x/y. Sets the level size and toast for echoes feeling. Sets some other level_flags, shop related flags and shop_type.
    virtual void init_level() = 0;

    // most themes call the same function, some check whether they are in CO
    virtual void init_rooms() = 0;

    /// Generates and adds the path rooms and exit room
    /// Params: reset to start over from the beginning if other rooms didn't fit
    virtual void generate_path(bool reset) = 0;

    /// Adds rooms related to udjat, black market, castle etc
    virtual void add_special_rooms() = 0;

    /// Adds a player revival coffin room
    virtual void add_player_coffin() = 0;

    /// Adds a Dirk coffin room
    virtual void add_dirk_coffin() = 0;

    /// Adds an idol room
    virtual void add_idol() = 0;

    /// Adds a vault room
    virtual void add_vault() = 0;

    /// Adds a character unlock coffin room
    virtual void add_coffin() = 0;

    /// Adds the metal clanking or oppression rooms
    virtual void add_special_feeling() = 0;

    /// Calls many other theme functions to spawn the floor, enemies, items etc, but not background and players. (Disable this to only spawn background and players.)
    virtual void spawn_level() = 0;

    /// Spawns the border entities (some floor or teleportingborder)
    virtual void spawn_border() = 0;

    /// Theme specific specialties like randomizing ushabti and Coco coffin location, spawns impostor lakes
    virtual void post_process_level() = 0;

    /// Spawns theme specific random traps, pushblocks and critters. Sets special exit doors.
    virtual void spawn_traps() = 0;

    /// Fixes textures on pleasure palace ladders, adds some decorations
    virtual void post_process_entities() = 0;

    /// Adds legs under platforms, random pots, goldbars, monsters, compass indicator, initialises quests, random shadows...
    virtual void spawn_procedural() = 0;

    /// Adds the main level background , e.g. CO stars / Duat moon / Plain backwall for other themes
    virtual void spawn_background() = 0;

    /// Adds room lights to udjat chest room or black market
    virtual void spawn_lights() = 0;

    /// Spawns the transition tunnel and players in it
    virtual void spawn_transition() = 0;

    /// Handles loading the next level screen from a transition screen
    virtual void post_transition() = 0;

    /// Spawns the players with inventory at `state.level_gen.spawn_x/y`. Also shop and kali background and probably other stuff for some stupid reason.
    virtual void spawn_players() = 0;

    /// Sets the camera bounds and position. Spawns jelly and orbs and the flag in coop. Sets timers/conditions for more jellies and ghosts. Enables the special fog/ember/ice etc particle effects. Spawns beg and handles it's quest flags
    virtual void spawn_effects() = 0;

    /// Returns: The .lvl file to load (e.g. dwelling = dwellingarea.lvl except when level == 4 (cavebossarea.lvl))
    virtual const char* get_level_file() = 0;

    /// Returns: THEME, or subtheme in CO
    virtual uint8_t get_theme_id() = 0;

    /// Returns: THEME, or logical base THEME for special levels (Abzu->Tide Pool etc)
    virtual uint8_t get_base_id() = 0;

    // all themes return 4 (ENT_TYPE_FLOOR_GENERIC), except:
    // temple: 104 (ENT_TYPE_FLOORSTYLED_TEMPLE)
    // neobab: 106 (ENT_TYPE_FLOORSTYLED_BABYLON)
    // sunken: 107 (ENT_TYPE_FLOORSTYLED_SUNKEN)
    // cog: 110 (ENT_TYPE_FLOORSTYLED_COG)
    // duat: 112 (ENT_TYPE_FLOORSTYLED_DUAT)
    // hundun: 107 (ENT_TYPE_FLOORSTYLED_SUNKEN)
    /// Returns: ENT_TYPE used for floor spreading (generic or one of the styled floors)
    virtual ENT_TYPE get_floor_spreading_type() = 0;

    // similar to get_floor_spreading_type(), except now the default = 103 (ENT_TYPE_FLOORSTYLED_STONE)
    /// Returns: ENT_TYPE used for floor spreading (stone or one of the styled floors)
    virtual ENT_TYPE get_floor_spreading_type2() = 0;

    /// Returns: true if transition should use styled floor
    virtual bool get_transition_styled_floor() = 0;

    /// Determines the types of FLOOR_TUNNEL_NEXT/CURRENT (depending on where you are transitioning from/to)
    /// Returns: 85 by default, except for: olmec: 15, cog: 23
    virtual uint32_t get_transition_floor_modifier() = 0;

    /// Returns: ENT_TYPE used for the transition floor
    virtual ENT_TYPE get_transition_styled_floor_type() = 0;

    /// Returns: ENT_TYPE used for the backwall (BG_LEVEL_BACKWALL by default)
    virtual ENT_TYPE get_backwall_type() = 0;

    /// Returns: ENT_TYPE to use for the border tiles
    virtual ENT_TYPE get_border_type() = 0;

    /// Returns: ENT_TYPE for theme specific critter
    virtual ENT_TYPE get_critter_type() = 0;

    /// Returns: gravity used to initialize liquid pools (-1..1)
    virtual float get_liquid_gravity() = 0;

    /// Returns: false to disable most player damage and the usage of bombs and ropes. Enabled in parts of base camp.
    virtual bool get_player_damage() = 0;

    /// Returns: true if explosions should spawn background soot
    virtual bool get_explosion_soot() = 0;

    /// Returns: TEXTURE for the LUT to be applied to the special back layer, e.g. vlad's castle
    virtual uint32_t get_backlayer_lut() = 0;

    /// Returns: dynamic backlayer light level (0..1)
    virtual float get_backlayer_light_level() = 0;

    /// Returns: true if the loop rendering should be enabled (Combine with the right get_border_type)
    virtual bool get_loop() = 0;

    /// Returns: highest y-level a vault can spawn
    virtual uint8_t get_vault_level() = 0;

    /// Returns: allow_beehive or allow_leprechaun flag
    /// Params: index: 0 or 1
    virtual bool get_theme_flag(uint8_t index) = 0;

    /// Returns: TEXTURE based on texture_id
    /// Params: DYNAMIC_TEXTURE texture_id
    virtual TEXTURE get_dynamic_texture(DYNAMIC_TEXTURE texture_id) = 0;

    /// Sets state.level_next, world_next and theme_next (or state.win_state) based on level number. Runs when exiting a level.
    virtual void pre_transition() = 0;

    /// Returns: usually state.height - 1. For special levels fixed heights are returned.
    virtual uint32_t get_exit_room_y_level() = 0;

    /// Returns: inverse shop chance
    virtual uint32_t get_shop_chance() = 0;

    /// Spawns some specific decoration, e.g. Vlad's big banner
    virtual void spawn_decoration() = 0;

    /// Spawns some other specific decorations, e.g. grass, flowers, udjat room decal
    virtual void spawn_decoration2() = 0;

    /// Spawns specific extra entities and decorations, like gold key, seaweed, lanterns, banners, signs, wires...
    virtual void spawn_extra() = 0;

    /// Spawns a single procedural entity, used in spawn_procedural (mostly monsters, scarab in dark levels etc.)
    virtual void do_procedural_spawn(SpawnInfo* info) = 0;

    uint32_t get_aux_id() const;
};
static_assert(sizeof(ThemeInfo) == 0x18);

struct LevelGenRooms
{
    std::array<uint16_t, 8 * 15> rooms;
};
static_assert(sizeof(LevelGenRooms) == 0xF0);

struct LevelGenRoomsMeta
{
    std::array<bool, 8 * 15> rooms;
};
static_assert(sizeof(LevelGenRoomsMeta) == 0x78);

class SpecialLevelGeneration
{
  public:
    virtual ~SpecialLevelGeneration(){};

    // For bees, sets rooms to be beehive rooms.
    virtual void set_rooms() = 0;

    // For bees, spawns the hive background and midbgs.
    virtual void spawn_backgrounds() = 0;

    // For bees, spawns bees and honey.
    // For leprechauns, spawns leprechaun, pot of gold, and rainbow.
    virtual void procedural_spawns() = 0;
};
static_assert(sizeof(SpecialLevelGeneration) == 0x8);

enum class SHOP_TYPE : uint8_t
{
    General,
    Clothing,
    Weapon,
    Specialty,
    HiredHand,
    Pet,
    Dice,
    Unkown07,
    Hedjet,
    Curio,
    Caveman,
    Turkey,
    Ghist,
    Tusk,
};

struct LevelGenSystem
{
    void init();

    template <class HookImplT>
    void hook_themes(HookImplT hook_impl)
    {
        for (ThemeInfo* theme : themes)
        {
            hook_impl.template hook<PopulateLevelFun, 0xd>(theme, &populate_level_hook);
            hook_impl.template hook<DoProceduralSpawnFun, 0x33>(theme, &do_procedural_spawn_hook);
            // this didn't work right
            // hook_impl.template hook<PopulateTransitionFun, 0x15>(theme, &populate_transition_hook);
        }
    }

    using PopulateLevelFun = void(ThemeInfo*, uint64_t, uint64_t, uint64_t);
    static void populate_level_hook(ThemeInfo*, uint64_t, uint64_t, uint64_t, PopulateLevelFun*);

    // using PopulateTransitionFun = void(ThemeInfo*);
    // static void populate_transition_hook(ThemeInfo*, PopulateTransitionFun*);

    using DoProceduralSpawnFun = void(ThemeInfo*, SpawnInfo*);
    static void do_procedural_spawn_hook(ThemeInfo*, SpawnInfo*, DoProceduralSpawnFun*);

    LevelGenData* data;
    uint64_t unknown2;
    union
    {
        std::array<ThemeInfo*, 18> themes;
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
    SpecialLevelGeneration* bee_gen;
    SpecialLevelGeneration* leprechaun_gen;
    union
    {
        LevelGenRooms* rooms[2];
        struct
        {
            LevelGenRooms* rooms_frontlayer;
            LevelGenRooms* rooms_backlayer;
        };
    };
    LevelGenRoomsMeta* flipped_rooms;
    LevelGenRoomsMeta* rooms_meta_27;
    LevelGenRoomsMeta* set_room_front_layer;
    LevelGenRoomsMeta* set_room_back_layer;
    LevelGenRoomsMeta* backlayer_room_exists;
    LevelGenRoomsMeta* machine_room_origin;
    LevelGenRoomsMeta* dual_room;
    LevelGenRoomsMeta* rooms_meta_33;
    LevelGenRoomsMeta* rooms_meta_34;
    std::uint32_t spawn_room_x;
    std::uint32_t spawn_room_y;
    float spawn_x;
    float spawn_y;
    custom_vector<Vec2> exit_doors;
    uint8_t flags;
    uint8_t flags2;
    uint8_t flags3;
    union
    {
        SHOP_TYPE shop_types[2];
        struct
        {
            SHOP_TYPE shop_type;
            SHOP_TYPE backlayer_shop_type;
        };
    };
    uint8_t frontlayer_shop_music;
    uint8_t backlayer_shop_music;
    uint8_t unknown47; // set to 0 at the start of level gen
    bool unknown48;

    /*uint8_t unknown47; // probably all just padding
    uint8_t unknown48;
    uint8_t unknown49;
    uint32_t unknown50;*/

    static std::pair<int, int> get_room_index(float x, float y);
    static Vec2 get_room_pos(uint32_t x, uint32_t y);
    std::optional<uint16_t> get_room_template(uint32_t x, uint32_t y, uint8_t l) const;
    bool set_room_template(uint32_t x, uint32_t y, int l, uint16_t room_template);

    bool is_room_flipped(uint32_t x, uint32_t y) const;
    bool is_machine_room_origin(uint32_t x, uint32_t y) const;
    bool mark_as_machine_room_origin(uint32_t x, uint32_t y, uint8_t l);
    bool mark_as_set_room(uint32_t x, uint32_t y, uint8_t l, bool is_set_room);

    static bool set_shop_type(uint32_t x, uint32_t y, uint8_t l, SHOP_TYPE shop_type);

    std::string_view get_room_template_name(uint16_t room_template) const;
    static std::optional<std::string_view> get_procedural_spawn_chance_name(uint32_t chance_id);
    uint32_t get_procedural_spawn_chance(uint32_t chance_id) const;
    bool set_procedural_spawn_chance(uint32_t chance_id, uint32_t inverse_chance);

    ~LevelGenSystem() = delete; // cuz it was complaining
};
static_assert(sizeof(LevelGenSystem) == 0x148);

bool default_spawn_is_valid(float x, float y, LAYER layer);
bool position_is_valid(float x, float y, LAYER layer, POS_TYPE flags);

void override_next_levels(std::vector<std::string> next_levels);
void add_next_levels(std::vector<std::string> next_levels);

using COSUBTHEME = int8_t; // NoAlias
COSUBTHEME get_co_subtheme();
void force_co_subtheme(COSUBTHEME subtheme);

void grow_vines(LAYER l, uint32_t max_length, AABB area, bool destroy_broken);
inline void grow_vines(LAYER l, uint32_t max_length)
{
    grow_vines(l, max_length, {0, 0, 0, 0}, false);
}

void grow_poles(LAYER l, uint32_t max_length, AABB area, bool destroy_broken);
inline void grow_poles(LAYER l, uint32_t max_length)
{
    grow_poles(l, max_length, {0, 0, 0, 0}, false);
}

bool grow_chain_and_blocks();
bool grow_chain_and_blocks(uint32_t x, uint32_t y);

void do_load_screen();

enum class THEME_OVERRIDE : uint8_t
{
    BASE,
    RESET_THEME_FLAGS,
    INIT_FLAGS,
    INIT_LEVEL,
    INIT_ROOMS,
    GENERATE_PATH,
    SPECIAL_ROOMS,
    PLAYER_COFFIN,
    DIRK_COFFIN,
    IDOL,
    VAULT,
    COFFIN,
    FEELING,
    SPAWN_LEVEL,
    SPAWN_BORDER,
    POST_PROCESS_LEVEL,
    SPAWN_TRAPS,
    POST_PROCESS_ENTITIES,
    SPAWN_PROCEDURAL,
    SPAWN_BACKGROUND,
    SPAWN_LIGHTS,
    SPAWN_TRANSITION,
    POST_TRANSITION,
    SPAWN_PLAYERS,
    SPAWN_EFFECTS,
    LVL_FILE,
    THEME_ID,
    BASE_ID,
    ENT_FLOOR_SPREADING,
    ENT_FLOOR_SPREADING2,
    TRANSITION_STYLED_FLOOR,
    TRANSITION_MODIFIER,
    ENT_TRANSITION_STYLED_FLOOR,
    ENT_BACKWALL,
    ENT_BORDER,
    ENT_CRITTER,
    GRAVITY,
    PLAYER_DAMAGE,
    SOOT,
    TEXTURE_BACKLAYER_LUT,
    BACKLAYER_LIGHT_LEVEL,
    LOOP,
    VAULT_LEVEL,
    THEME_FLAG,
    TEXTURE_DYNAMIC,
    PRE_TRANSITION,
    EXIT_ROOM_Y_LEVEL,
    SHOP_CHANCE,
    SPAWN_DECORATION,
    SPAWN_DECORATION2,
    SPAWN_EXTRA,
    DO_PROCEDURAL_SPAWN
};
