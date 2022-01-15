#pragma once

#include "game_allocator.hpp"
#include "level_api_types.hpp"
#include "spawn_api.hpp"
#include "state.hpp"
#include "rpc.hpp"

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>

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
    None = 0,
    Entrance = 1,
    Exit = 2,
    Shop = 3,
    MachineRoom = 4,
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

    virtual ~ThemeInfo()
    {
    }
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

    // Note: Inserted somewhere between initialize_flags and populate_level
    virtual bool unknown_v12() = 0;

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
    // strangely, it's only applied to an odd block here and there (so floorspreading?)
    virtual uint32_t random_block_floorstyle() = 0;

    // similar to random_block_floorstyle(), except now the default = 103 (ENT_TYPE_FLOORSTYLED_STONE)
    virtual uint32_t random_block_floorstyle_2() = 0;

    // all return false, except olmec, temple, neobab, cog, duat
    virtual bool unknown_v30() = 0;

    // determines the types of FLOOR_TUNNEL_NEXT/CURRENT (depending on where you are transitioning from/to) for this theme
    // returns 85 by default, except for: olmec: 15, cog: 23
    virtual uint32_t transition_tunnel_block_modifier() = 0;

    virtual uint32_t unknown_v32() = 0;

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

    // a value between 0.0 (default) and 1.0 used to illuminate (backlayer) locations
    // depending on camera level (thus player y-level), the brightness is increased gradually
    // used in black market, vlad's castle, ice caves backlayer, pleasure palace
    // for tiamat: the value is always 1.0 for full backlayer global illumination (ship)
    virtual float backlayer_global_illumination_level() = 0;

    // this is used for CO (checks that player is in the level, not in the transition)
    // if enabled in another theme, it adds an extra border and if you zoom out, you see the level loop if you move to the side
    virtual bool enable_camera_loop() = 0;

    // not 100% sure, this is used in a random calculation that determines whether a vault spawns
    // looks to be the highest level a vault can spawn; it's mostly 3 or 4, but for neobab it's 1, which makes sense
    virtual uint8_t max_level_for_vault() = 0;

    // index == 0 ? return unknown1 : return unknown2
    virtual bool get_unknown_1_or_2(uint8_t index) = 0;

    // e.g. for dwelling:
    // texture_id == -4 -> returns 122 BG_CAVE_0
    // texture_id == -5 -> returns 115 FLOOR_CAVE_0
    // texture_id == -6 -> returns 117 FLOOR_CAVE_2
    // texture_id == -7 -> returns 118 FLOOR_CAVE_3
    // texture_id == -8 -> returns 120 DECO_CAVE_0
    // texture_id == -10 -> returns 369 COFFINS_0
    // ...
    // the texture_id parameter comes from the entitydb.texture field, for some entities the texture is not a valid texture ID but a negative number
    // that number is passed here and mapped into this dynamic per-theme list (see entitydb[4].texture)
    virtual uint32_t get_dynamic_floor_texture_id(int32_t texture_id) = 0;

    // manipulates state.level_next, world_next and theme_next; triggers when exiting a level
    // for dwelling, it just increments level_next because the world/theme choice is made by which door you pick
    // for jungle/volcana, it checks whether it's on the fourth level, if so, sets theme_next (4), world_next (3) and level_next (1) correctly for olmec
    // for CO it checks whether the next level is 99, and set state.win_state to 3 to finish the game
    virtual void set_next_world_level_theme() = 0;

    // default = return state.h - 1
    // for special levels (black market, vlad, ...) fixed heights are returned
    virtual uint32_t get_zero_based_level_height() = 0;

    // returns a value that appears to affect room generation and is based on current world,level
    virtual uint32_t unknown_v47() = 0;

    // used e.g. in Vlad's castle to insert the big banner in the center with the two demon statues
    // also implemented for neobab (i think in the zoos)
    // might do other things as well
    virtual void post_process_decoration1() = 0;

    // dwelling: adds the decal above the udjat chest
    // jungle: adds the colorful jungle flowers on top of the blocks
    // does lots of other things as well, not sure about difference between this and post_process_decoration1
    virtual void post_process_decoration2() = 0;

    // dwelling udjat level: adds the key in random place
    // vlad's castle: adds decorative banners
    // tidepool: adds hanging seaweed, red lanterns, ...
    // temple: adds temple sand deco, ...
    // neobab: adds neon signs, hanging wires, ...
    virtual void populate_extra_random_entities() = 0;

    virtual void do_procedural_spawn() = 0;
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
    DoorCoords* exit_doors_locations;
    void* unknown37;
    void* unknown38;
    uint8_t flags;
    uint8_t unknown39;
    uint8_t unknown40;
    uint8_t shop_type;
    uint8_t unknown42;
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

    std::pair<int, int> get_room_index(float x, float y);
    std::pair<float, float> get_room_pos(uint32_t x, uint32_t y);
    std::optional<uint16_t> get_room_template(uint32_t x, uint32_t y, uint8_t l);
    bool set_room_template(uint32_t x, uint32_t y, int l, uint16_t room_template);

    bool is_room_flipped(uint32_t x, uint32_t y);
    bool mark_as_machine_room_origin(uint32_t x, uint32_t y, uint8_t l);
    bool mark_as_set_room(uint32_t x, uint32_t y, uint8_t l, bool is_set_room);

    std::string_view get_room_template_name(uint16_t room_template);

    uint32_t get_procedural_spawn_chance(uint32_t chance_id);
    bool set_procedural_spawn_chance(uint32_t chance_id, uint32_t inverse_chance);
};

bool default_spawn_is_valid(float x, float y, uint8_t layer);

void override_next_levels(std::vector<std::string> next_levels);
void add_next_levels(std::vector<std::string> next_levels);

int8_t get_co_subtheme();
void force_co_subtheme(int8_t subtheme);

class CustomTheme : public ThemeInfo
{
  public:
    std::string level_file = "";
    uint8_t theme = 100;
    uint8_t base_theme = 0;
    uint32_t spreading_floor = UINT32_MAX;
    uint32_t spreading_floorstyled = UINT32_MAX;
    uint32_t border_floor = UINT32_MAX;
    uint8_t border_type = 0; //enum
    uint32_t texture_floor = 0;
    uint32_t texture_bg = 0;
    uint32_t texture_door = 0;
    uint32_t texture_backdoor = 0;

    float gravity = -1.0f;
    float back_light = 0.0f;

    bool player_damage = true;
    bool loop = false;
    bool procedural_spawn = false;
    bool procedural_level_gen = false;
    bool vault = false;
    bool coffin = false;
    bool populate = false;
    bool spawn_players = true;
    bool transition = true;

    ~CustomTheme(){}
    bool get_unknown1()
    {
        return State::get().ptr_local()->level_gen->themes[base_theme]->get_unknown1();
    }
    void initialize_flags()
    {
    }
    void initialize_levelgen()
    {
        if (procedural_level_gen)
            State::get().ptr_local()->level_gen->themes[base_theme]->initialize_levelgen();
    }
    void unknown_v4()
    {
        State::get().ptr_local()->level_gen->themes[base_theme]->unknown_v4();
    }
    void unknown_v5()
    {
        State::get().ptr_local()->level_gen->themes[base_theme]->unknown_v5();
    }
    void handle_level_specialities()
    {
        State::get().ptr_local()->level_gen->themes[base_theme]->handle_level_specialities();
    }
    void unknown_v7()
    {
        State::get().ptr_local()->level_gen->themes[base_theme]->unknown_v7();
    }
    void unknown_v8()
    {
        State::get().ptr_local()->level_gen->themes[base_theme]->unknown_v8();
    }
    void insert_shopkeeper_vault()
    {
        if (vault)
            State::get().ptr_local()->level_gen->themes[base_theme]->insert_shopkeeper_vault();
    }
    void insert_coffin()
    {
        if (coffin)
            State::get().ptr_local()->level_gen->themes[base_theme]->insert_coffin();
    }
    void set_theme_specific_level_feeling()
    {
        State::get().ptr_local()->level_gen->themes[base_theme]->set_theme_specific_level_feeling();
    }
    bool unknown_v12()
    {
        return State::get().ptr_local()->level_gen->themes[base_theme]->unknown_v12();
    }
    void populate_level()
    {
        if (populate)
            State::get().ptr_local()->level_gen->themes[base_theme]->populate_level();
    }
    void add_level_bordertiles()
    {
        // get borders from dwelling so they don't leak for testing
        switch(border_type)
        {
            // full border, from dwelling
            case 0:
            State::get().ptr_local()->level_gen->themes[0]->add_level_bordertiles();
            break;

            // bottomless, from ice caves
            case 1:
            State::get().ptr_local()->level_gen->themes[6]->add_level_bordertiles();
            break;

            // no border
            default:
            break;
        }
    }
    void post_process_level()
    {
        State::get().ptr_local()->level_gen->themes[base_theme]->post_process_level();
    }
    void post_process_exitdoors()
    {
        State::get().ptr_local()->level_gen->themes[base_theme]->post_process_exitdoors();
    }
    void post_process_entities()
    {
        State::get().ptr_local()->level_gen->themes[base_theme]->post_process_entities();
    }
    void populate_background()
    {
        State::get().ptr_local()->level_gen->themes[base_theme]->populate_background();
    }
    void populate_background_beautification()
    {
        State::get().ptr_local()->level_gen->themes[base_theme]->populate_background_beautification();
    }
    void populate_extra_lighting()
    {
        State::get().ptr_local()->level_gen->themes[base_theme]->populate_extra_lighting();
    }
    void populate_level_transition()
    {
        if (transition)
            State::get().ptr_local()->level_gen->themes[base_theme]->populate_level_transition();
    }
    void on_level_transition()
    {
        auto state = State::get().ptr_local();
        state->quest_flags &= ~(1U);
        state->screen_next = 12;
        state->ingame = true;
        state->loading = 1;
    }
    void populate_players()
    {
        if (spawn_players)
            State::get().ptr_local()->level_gen->themes[base_theme]->populate_players();
    }
    void handle_multiplayer()
    {
        State::get().ptr_local()->level_gen->themes[base_theme]->handle_multiplayer();
    }
    const char* level_file_to_load()
    {
        if (level_file != "")
            return level_file.c_str();
        return State::get().ptr_local()->level_gen->themes[base_theme]->level_file_to_load();
    }
    uint8_t theme_id()
    {
        return theme;
    }
    uint8_t theme_base_id()
    {
        return base_theme;
    }
    uint32_t random_block_floorstyle()
    {
        if (spreading_floor != UINT32_MAX)
            return spreading_floor;
        return State::get().ptr_local()->level_gen->themes[base_theme]->random_block_floorstyle();
    }
    uint32_t random_block_floorstyle_2()
    {
        if (spreading_floorstyled != UINT32_MAX)
            return spreading_floorstyled;
        return State::get().ptr_local()->level_gen->themes[base_theme]->random_block_floorstyle_2();
    }
    bool unknown_v30()
    {
        return State::get().ptr_local()->level_gen->themes[base_theme]->unknown_v30();
    }
    uint32_t transition_tunnel_block_modifier()
    {
        return State::get().ptr_local()->level_gen->themes[base_theme]->transition_tunnel_block_modifier();
    }
    uint32_t unknown_v32()
    {
        return State::get().ptr_local()->level_gen->themes[base_theme]->unknown_v32();
    }
    uint32_t backwall_entity_id()
    {
        return to_id("ENT_TYPE_BG_LEVEL_BACKWALL");
    }
    uint32_t bordertile_entity_id()
    {
        if (border_floor != UINT32_MAX)
            return border_floor;
        return State::get().ptr_local()->level_gen->themes[base_theme]->bordertile_entity_id();
    }
    uint32_t transition_tunnel_critter_entity_id()
    {
        return State::get().ptr_local()->level_gen->themes[base_theme]->transition_tunnel_critter_entity_id();
    }
    float liquid_gravity_direction()
    {
        return gravity;
    }
    bool are_players_vulnerable()
    {
        return player_damage;
    }
    bool unknown_37()
    {
        return State::get().ptr_local()->level_gen->themes[base_theme]->unknown_37();
    }
    uint32_t backlayer_lut()
    {
        return 395;
    }
    float backlayer_global_illumination_level()
    {
        return back_light;
    }
    bool enable_camera_loop()
    {
        return loop;
    }
    uint8_t max_level_for_vault()
    {
        return 255;
    }
    bool get_unknown_1_or_2(uint8_t index)
    {
        return State::get().ptr_local()->level_gen->themes[base_theme]->get_unknown_1_or_2(index);
    }
    // texture_id == -4 -> returns 122 BG_CAVE_0
    // texture_id == -5 -> returns 115 FLOOR_CAVE_0
    // texture_id == -6 -> returns 117 FLOOR_CAVE_2
    // texture_id == -7 -> returns 118 FLOOR_CAVE_3
    // texture_id == -8 -> returns 120 DECO_CAVE_0
    // texture_id == -10 -> returns 369 COFFINS_0
    uint32_t get_dynamic_floor_texture_id(int32_t texture_id)
    {
        switch (texture_id)
        {
        case -4:
            if (texture_bg > 0)
                return texture_bg;
            break;
        case -5:
            if (texture_floor > 0)
                return texture_floor;
            break;
        case -6:
            if (texture_door > 0)
                return texture_door;
            break;
        case -7:
            if (texture_backdoor > 0)
                return texture_backdoor;
            break;
        default:
            break;
        }
        return State::get().ptr_local()->level_gen->themes[base_theme]->get_dynamic_floor_texture_id(texture_id);
    }
    void set_next_world_level_theme()
    {
        auto state = State::get().ptr_local();
        state->world_next = 1;
        state->level_next = 1;
        state->theme_next = 1;
    }
    uint32_t get_zero_based_level_height()
    {
        //return State::get().ptr()->h - 1;
        return State::get().ptr_local()->level_gen->themes[base_theme]->get_zero_based_level_height();
    }
    uint32_t unknown_v47()
    {
        return State::get().ptr_local()->level_gen->themes[base_theme]->unknown_v47();
    }
    void post_process_decoration1()
    {
        return State::get().ptr_local()->level_gen->themes[base_theme]->post_process_decoration1();
    }
    void post_process_decoration2()
    {
        return State::get().ptr_local()->level_gen->themes[base_theme]->post_process_decoration2();
    }
    void populate_extra_random_entities()
    {
        if (procedural_spawn)
            State::get().ptr_local()->level_gen->themes[base_theme]->populate_extra_random_entities();
    }
    void do_procedural_spawn()
    {
        if (procedural_spawn)
            State::get().ptr_local()->level_gen->themes[base_theme]->do_procedural_spawn();
    }
};
