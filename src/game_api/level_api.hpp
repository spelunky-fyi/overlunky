#pragma once

#include <array>       // for array
#include <cstddef>     // for byte
#include <cstdint>     // for uint32_t, uint8_t, uint...
#include <functional>  // for function
#include <new>         // for operator new
#include <optional>    // for optional
#include <string>      // for string, allocator
#include <string_view> // for string_view
#include <type_traits> // for move
#include <utility>     // for pair
#include <vector>      // for vector

#include "aliases.hpp"                       // for LAYER
#include "containers/game_string.hpp"        // for game_string
#include "containers/game_unordered_map.hpp" // for game_unordered_map
#include "containers/game_vector.hpp"        // for game_vector
#include "level_api_types.hpp"               // for ShortTileCodeDef
#include "math.hpp"                          // for AABB (ptr only), Vec2

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

    std::optional<std::uint32_t> get_tile_code(const std::string& tile_code);
    std::uint32_t define_tile_code(std::string tile_code);

    std::optional<uint8_t> get_short_tile_code(ShortTileCodeDef short_tile_code_def);
    std::optional<ShortTileCodeDef> get_short_tile_code_def(uint8_t short_tile_code);
    void change_short_tile_code(uint8_t short_tile_code, ShortTileCodeDef short_tile_code_def);
    std::optional<uint8_t> define_short_tile_code(ShortTileCodeDef short_tile_code_def);

    std::optional<std::uint32_t> get_chance(const std::string& chance);
    std::uint32_t define_chance(std::string chance);

    std::uint32_t register_chance_logic_provider(std::uint32_t chance_id, SpawnLogicProvider provider);
    void unregister_chance_logic_provider(std::uint32_t provider_id);

    std::uint32_t define_extra_spawn(std::uint32_t num_spawns_front_layer, std::uint32_t num_spawns_back_layer, SpawnLogicProvider provider);
    void set_num_extra_spawns(std::uint32_t extra_spawn_id, std::uint32_t num_spawns_front_layer, std::uint32_t num_spawns_back_layer);
    std::pair<std::uint32_t, std::uint32_t> get_missing_extra_spawns(std::uint32_t extra_spawn_id);
    void undefine_extra_spawn(std::uint32_t extra_spawn_id);

    std::optional<std::uint16_t> get_room_template(const std::string& room_template);
    std::uint16_t define_room_template(std::string room_template, RoomTemplateType type);
    bool set_room_template_size(std::uint16_t room_template, uint16_t width, uint16_t height);
    RoomTemplateType get_room_template_type(std::uint16_t room_template);
    uint16_t get_pretend_room_template(std::uint16_t room_template);

    union
    {
        uint32_t level_config[18];
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
            uint32_t unknown_config;
        };
    };

    game_unordered_map<std::uint8_t, ShortTileCodeDef> short_tile_codes;
    game_unordered_map<game_string, TileCodeDef> tile_codes;
    game_unordered_map<game_string, RoomTemplateDef> room_templates;

    game_unordered_map<std::uint16_t, RoomTemplateData> room_template_datas;
    std::byte padding1[0x6b8];
    std::array<RoomTemplateData, 8 * 15> set_room_datas;

    game_unordered_map<game_string, ChanceDef> monster_chances;
    game_unordered_map<std::uint32_t, LevelChanceDef> level_monster_chances;

    game_unordered_map<game_string, ChanceDef> trap_chances;
    game_unordered_map<std::uint32_t, LevelChanceDef> level_trap_chances;
};

struct DoorCoords
{
    float door1_x;
    float door1_y;
    /// door2 only valid when there are two in the level, like Volcana drill, Olmec, ...
    float door2_x;
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

    std::uint32_t reserve_callback_id();
    void unhook(std::uint32_t id);
    struct ThemeHooksInfo& get_hooks();

    void set_pre_init_flags(std::uint32_t reserved_callback_id, std::function<bool(ThemeInfo*)> pre_init_flags);
    void set_post_init_flags(std::uint32_t reserved_callback_id, std::function<void(ThemeInfo*)> post_init_flags);

    void set_pre_spawn_effects(std::uint32_t reserved_callback_id, std::function<bool(ThemeInfo*)> pre_spawn_effects);
    void set_post_spawn_effects(std::uint32_t reserved_callback_id, std::function<void(ThemeInfo*)> post_spawn_effects);

    virtual ~ThemeInfo()
    {
    }

    virtual bool get_unknown1() = 0;

    /// dwelling,tidepool: unset levelgen.flags.flag12
    /// jungle,volcana.olmec,icecaves,neobab,cog,duat,abzu,tiamat,eggplant,hundun,basecamp,arena: nop
    /// temple: unset levelgen.flags.flag10 + calculate chance of grasshopper critter spawn
    /// sunken: unset levelgen.flags.flag9, 10, 17, 18 + if state.level == 1 -> unset flag 11
    /// cosmic: calls same virtual on its sub_theme
    // ???
    virtual void init_flags() = 0;

    /// does random calculations and calls function to determine the start room in most themes
    // ???
    virtual void init_level() = 0;

    /// most themes call the same function, some check whether they are in CO
    virtual void unknown_v4() = 0;

    virtual void unknown_v5() = 0;

    /// dwelling: does stuff when level == 4 or udjat present
    /// jungle: when black market present
    /// volcana: when drill present
    /// touches the rooms and sometimes the meta info about the rooms
    // ???
    virtual void add_special_rooms() = 0;

    /// can't trigger, dwelling (quillback) and abzu do something special (arena just returns)
    virtual void unknown_v7() = 0;

    /// does something depending on levelgen.data.unknown7
    virtual void unknown_v8() = 0;

    // ???
    virtual void add_vault() = 0;

    // ???
    virtual void add_coffin() = 0;

    /// metal clanking and air of oppression
    virtual void add_special_feeling() = 0;

    // Note: Inserted somewhere between init_flags and spawn_level
    virtual bool unknown_v12() = 0;

    /// spawns all floor etc tiles based on the room layout
    /// disable this and only the player is spawned in the level
    virtual void spawn_level() = 0;

    /// spawns CO: teleportingborder / Duat: dust / theme specific border tiles around the level
    virtual void spawn_border() = 0;

    /// volcana: checks if state.coffin_contents = cocovondiamonds -> chooses one of the four coffins in vlad's castle at random to put her in
    /// tidepool: spawns impostor lake, some door and a litwalltorch, ...
    /// neobab: assigns the correct animation_frame to all the ushabtis
    /// co, arena: forwards to the same virtual of the sub_theme
    /// abzu, tiamat: spawns impostor lake
    // ???
    virtual void post_process_level() = 0;

    /// adds theme specific random traps and pushblocks
    // quillback: adds butterflies and snails + configures the door to go to the correct theme/level
    // olmec: same thing but crabs and grasshoppers
    // temple: spawns COG door
    // co, arena: forwards to the same virtual of the sub_theme
    // other themes also do stuff, but nothing seemingly important
    virtual void spawn_traps() = 0;

    /// pleasure palace: applies correct texture to ladders and ladder platforms
    /// sunken city: randomly adds ENT_TYPE_DECORATION_SUNKEN_BRIDGE between entities (the slimy bridges)
    /// hundun: calls sunken city virtual, so has slimy bridges
    // co: does nothing, so no slimy bridges in CO sunken city levels! (forgotten/bug?)
    virtual void post_process_entities() = 0;

    /// adds legs under platforms, random pots, goldbars, procedural spawns, compass indicator...
    virtual void spawn_procedural() = 0;

    /// adds a background, e.g. CO stars / Duat moon / Plain backwall for other themes
    virtual void spawn_background() = 0;

    /// adds extra light where needed, e.g. in the udjat chest room, or the top layer of the black market: spawns ENT_TYPE_LOGICAL_ROOM_LIGHT
    virtual void spawn_lights() = 0;

    /// spawns a transition tunnel to the level
    virtual void spawn_transition() = 0;

    /// unsets flag 1 (Reset) of state.quest_flags
    /// sets the correct state.screen (0xC)
    /// sets state.ingame to true, adjust fade values and starts loading
    virtual void post_transition() = 0;

    /// spawns the player(s) in the world, along with what they were holding
    virtual void spawn_players() = 0;

    /// when disabled, during multiplayer the camera is not focused; also responsible for spawning the leader flag; not looked at in detail
    /// this actually also sets the camera bounds and various theme specific special effects
    /// also spawns / sets the conditions for osiris, jelly, orbs...
    /// also makes curse pots spawn the ghost
    virtual void spawn_effects() = 0;

    /// the .lvl file to load (e.g. dwelling = dwellingarea.lvl except when level == 4 (cavebossarea.lvl))
    virtual const char* get_level_file() = 0;

    /// for co: returns sub_theme->get_theme_id()
    virtual uint8_t get_theme_id() = 0;

    /// whereas get_theme_id() returns a unique id for all ThemeInfo's, this function returns the id of the theme it logically belongs to
    /// e.g. theme_abzu->get_base_id() = 5 (tide_pool) as opposed to theme_abzu->get_theme_id() = 13 (abzu)
    virtual uint8_t get_base_id() = 0;

    /// all themes return 4 (ENT_TYPE_FLOOR_GENERIC), except:
    /// temple: 104 (ENT_TYPE_FLOORSTYLED_TEMPLE)
    /// neobab: 106 (ENT_TYPE_FLOORSTYLED_BABYLON)
    /// sunken: 107 (ENT_TYPE_FLOORSTYLED_SUNKEN)
    /// cog: 110 (ENT_TYPE_FLOORSTYLED_COG)
    /// duat: 112 (ENT_TYPE_FLOORSTYLED_DUAT)
    /// hundun: 107 (ENT_TYPE_FLOORSTYLED_SUNKEN)
    virtual uint32_t get_floor_spreading_type() = 0;

    /// similar to get_floor_spreading_type(), except now the default = 103 (ENT_TYPE_FLOORSTYLED_STONE)
    virtual uint32_t get_floor_spreading_type2() = 0;

    /// all return false, except olmec, temple, neobab, cog, duat
    virtual bool unknown_v30() = 0;

    /// determines the types of FLOOR_TUNNEL_NEXT/CURRENT (depending on where you are transitioning from/to) for this theme
    /// returns 85 by default, except for: olmec: 15, cog: 23
    virtual uint32_t get_transition_block_modifier() = 0;

    virtual uint32_t unknown_v32() = 0;

    /// always returns 778 ENT_TYPE_BG_LEVEL_BACKWALL
    virtual uint32_t get_backwall_type() = 0;

    /// returns ENT_TYPE_FLOOR_BORDERTILE by default, except:
    /// neobab ENT_TYPE_FLOOR_BORDERTILE_METAL
    /// sunken ENT_TYPE_FLOOR_BORDERTILE_OCTOPUS
    /// duat ENT_TYPE_FLOOR_DUSTWALL
    /// tiamat ENT_TYPE_FLOOR_BORDERTILE_METAL
    /// hundun ENT_TYPE_FLOOR_BORDERTILE_OCTOPUS
    virtual uint32_t get_border_type() = 0;

    virtual uint32_t get_critter_type() = 0;

    /// returns -1.0 (downwards) by default, except for sunken/hundun 1.0 (upwards); applies to both lava and water
    virtual float get_liquid_gravity() = 0;

    /// used to make the player invincible in basecamp (but does an OOB check)
    /// if you return false in other themes you are invincible except for crushing deaths, and you do still experience knockback and stun
    virtual bool get_player_damage() = 0;

    /// returns true by default, except CO, duat (these also have no bg, but don't know if related)
    virtual bool unknown_v38() = 0;

    /// returns the texture ID for the LUT to be applied to the special back layer, e.g. vlad's castle for the volcana theme
    virtual uint32_t get_backlayer_lut() = 0;

    /// a value between 0.0 (default) and 1.0 used to illuminate (backlayer) locations
    /// depending on camera level (thus player y-level), the brightness is increased gradually
    /// used in black market, vlad's castle, ice caves backlayer, pleasure palace
    /// for tiamat: the value is always 1.0 for full backlayer global illumination (ship)
    virtual float get_backlayer_light_level() = 0;

    /// this is used for CO (checks that player is in the level, not in the transition)
    /// if enabled in another theme, it adds an extra border and if you zoom out, you see the level loop if you move to the side
    virtual bool get_loop() = 0;

    /// not 100% sure, this is used in a random calculation that determines whether a vault spawns
    /// looks to be the highest level a vault can spawn; it's mostly 3 or 4, but for neobab it's 1, which makes sense
    virtual uint8_t get_vault_level() = 0;

    /// index == 0 ? return unknown1 : return unknown2
    virtual bool get_unknown_1_or_2(uint8_t index) = 0;

    // e.g. for dwelling:
    // texture_id == -4 -> returns 122 BG_CAVE_0
    // texture_id == -5 -> returns 115 FLOOR_CAVE_0
    // texture_id == -6 -> returns 117 FLOOR_CAVE_2
    // texture_id == -7 -> returns 118 FLOOR_CAVE_3
    // texture_id == -8 -> returns 120 DECO_CAVE_0
    // texture_id == -10 -> returns 369 COFFINS_0
    // ...
    /// the texture_id parameter comes from the entitydb.texture field, for some entities the texture is not a valid texture ID but a negative number
    /// that number is passed here and mapped into this dynamic per-theme list (see entitydb[4].texture)
    virtual uint32_t get_dynamic_texture(int32_t texture_id) = 0;

    /// manipulates state.level_next, world_next and theme_next; triggers when exiting a level
    /// for dwelling, it just increments level_next because the world/theme choice is made by which door you pick
    /// for jungle/volcana, it checks whether it's on the fourth level, if so, sets theme_next (4), world_next (3) and level_next (1) correctly for olmec
    /// for CO it checks whether the next level is 99, and set state.win_state to 3 to finish the game
    virtual void pre_transition() = 0;

    /// default = return state.h - 1
    /// for special levels (black market, vlad, ...) fixed heights are returned
    virtual uint32_t get_level_height() = 0;

    /// returns a value that appears to affect room generation and is based on current world,level
    virtual uint32_t unknown_v47() = 0;

    /// used e.g. in Vlad's castle to insert the big banner in the center with the two demon statues
    /// also implemented for neobab (i think in the zoos)
    /// might do other things as well
    virtual void spawn_decoration() = 0;

    /// dwelling: adds the decal above the udjat chest
    /// jungle: adds the colorful jungle flowers on top of the blocks
    /// does lots of other things as well, not sure about difference between this and spawn_decoration
    virtual void spawn_decoration2() = 0;

    /// dwelling udjat level: adds the key in random place
    /// vlad's castle: adds decorative banners
    /// tidepool: adds hanging seaweed, red lanterns, ...
    /// temple: adds temple sand deco, ...
    /// neobab: adds neon signs, hanging wires, ...
    virtual void spawn_extra() = 0;

    virtual void unknown_v51() = 0;
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

enum class ShopType : uint8_t
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
    union
    {
        std::vector<Vec2> exit_doors;
        struct
        {
            /// NoDoc
            DoorCoords* exit_doors_locations;
            void* unknown37;
            void* unknown38;
        };
    };
    uint8_t flags;
    uint8_t unknown39; // also flags?
    uint8_t unknown40; // also flags?
    union
    {
        ShopType shop_types[2];
        struct
        {
            ShopType shop_type;
            ShopType backlayer_shop_type;
        };
    };
    uint8_t unknown43;
    uint8_t unknown44;
    uint8_t unknown45;
    uint8_t unknown46;
    uint8_t unknown47;
    uint8_t unknown48;
    uint8_t unknown49;
    uint32_t unknown50;
    uint32_t unknown51;
    uint32_t unknown52;

    static std::pair<int, int> get_room_index(float x, float y);
    static std::pair<float, float> get_room_pos(uint32_t x, uint32_t y);
    std::optional<uint16_t> get_room_template(uint32_t x, uint32_t y, uint8_t l);
    bool set_room_template(uint32_t x, uint32_t y, int l, uint16_t room_template);

    bool is_room_flipped(uint32_t x, uint32_t y);
    bool is_machine_room_origin(uint32_t x, uint32_t y);
    bool mark_as_machine_room_origin(uint32_t x, uint32_t y, uint8_t l);
    bool mark_as_set_room(uint32_t x, uint32_t y, uint8_t l, bool is_set_room);

    bool set_shop_type(uint32_t x, uint32_t y, uint8_t l, ShopType shop_type);

    std::string_view get_room_template_name(uint16_t room_template);

    uint32_t get_procedural_spawn_chance(uint32_t chance_id);
    bool set_procedural_spawn_chance(uint32_t chance_id, uint32_t inverse_chance);

    ~LevelGenSystem() = delete; // cuz it was complaining
};

bool default_spawn_is_valid(float x, float y, LAYER layer);
bool position_is_valid(float x, float y, LAYER layer, POS_TYPE flags);

void override_next_levels(std::vector<std::string> next_levels);
void add_next_levels(std::vector<std::string> next_levels);

int8_t get_co_subtheme();
void force_co_subtheme(int8_t subtheme);

void grow_vines(LAYER l, uint32_t max_lengh);
void grow_vines(LAYER l, uint32_t max_lengh, AABB area, bool destroy_broken);

void grow_poles(LAYER l, uint32_t max_lengh);
void grow_poles(LAYER l, uint32_t max_lengh, AABB area, bool destroy_broken);

bool grow_chain_and_blocks();
bool grow_chain_and_blocks(uint32_t x, uint32_t y);

void do_load_screen();

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

enum class THEME_OVERRIDE : uint8_t
{
    BASE,
    UNKNOWN_V1,
    INIT_FLAGS,
    INIT_LEVEL,
    UNKNOWN_V4,
    UNKNOWN_V5,
    SPECIAL_ROOMS,
    UNKNOWN_V7,
    UNKNOWN_V8,
    VAULT,
    COFFIN,
    FEELING,
    UNKNOWN_V12,
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
    UNKNOWN_V30,
    TRANSITION_MODIFIER,
    UNKNOWN_V32,
    ENT_BACKWALL,
    ENT_BORDER,
    ENT_CRITTER,
    GRAVITY,
    PLAYER_DAMAGE,
    UNKNOWN_V38,
    TEXTURE_BACKLAYER_LUT,
    BACKLAYER_LIGHT_LEVEL,
    LOOP,
    VAULT_LEVEL,
    GET_UNKNOWN1_OR_2,
    TEXTURE_DYNAMIC,
    PRE_TRANSITION,
    LEVEL_HEIGHT,
    UNKNOWN_V47,
    SPAWN_DECORATION,
    SPAWN_DECORATION2,
    SPAWN_EXTRA,
    UNKNOWN_V51
};
