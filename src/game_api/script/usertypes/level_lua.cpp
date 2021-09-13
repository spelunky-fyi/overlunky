#include "level_lua.hpp"

#include "level_api.hpp"
#include "savedata.hpp"
#include "script/lua_backend.hpp"
#include "state.hpp"

#include <sol/sol.hpp>

void PreLoadLevelFilesContext::override_level_files(std::vector<std::string> levels)
{
    override_next_levels(std::move(levels));
}
void PreLoadLevelFilesContext::add_level_files(std::vector<std::string> levels)
{
    add_next_levels(std::move(levels));
}

bool PostRoomGenerationContext::set_room_template(int x, int y, int l, ROOM_TEMPLATE room_template)
{
    return State::get().ptr_local()->level_gen->set_room_template(x, y, l, room_template);
}

bool PostRoomGenerationContext::set_procedural_spawn_chance(PROCEDURAL_CHANCE chance_id, uint32_t inverse_chance)
{
    return State::get().ptr_local()->level_gen->set_procedural_spawn_chance(chance_id, inverse_chance);
}

void PostRoomGenerationContext::set_num_extra_spawns(std::uint32_t extra_spawn_id, std::uint32_t num_spawns_front_layer, std::uint32_t num_spawns_back_layer)
{
    State::get().ptr_local()->level_gen->data->set_num_extra_spawns(extra_spawn_id, num_spawns_front_layer, num_spawns_back_layer);
}

namespace NLevel
{
void register_usertypes(sol::state& lua)
{
    /// Add a callback for a specific tile code that is called before the game handles the tile code.
    /// The callback signature is `bool pre_tile_code(x, y, layer, room_template)`
    /// Return true in order to stop the game or scripts loaded after this script from handling this tile code.
    /// For example, when returning true in this callback set for `"floor"` then no floor will spawn in the game (unless you spawn it yourself)
    lua["set_pre_tile_code_callback"] = [](sol::function cb, std::string tile_code) -> CallbackId
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        backend->pre_tile_code_callbacks.push_back(LevelGenCallback{backend->cbcount, std::move(tile_code), std::move(cb)});
        return backend->cbcount++;
    };
    /// Add a callback for a specific tile code that is called after the game handles the tile code.
    /// The callback signature is `nil post_tile_code(x, y, layer, room_template)`
    /// Use this to affect what the game or other scripts spawned in this position.
    /// This is received even if a previous pre-tile-code-callback has returned true
    lua["set_post_tile_code_callback"] = [](sol::function cb, std::string tile_code) -> CallbackId
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        backend->post_tile_code_callbacks.push_back(LevelGenCallback{backend->cbcount, std::move(tile_code), std::move(cb)});
        return backend->cbcount++;
    };
    /// Define a new tile code, to make this tile code do anything you have to use either `set_pre_tile_code_callback` or `set_post_tile_code_callback`.
    /// If a user disables your script but still uses your level mod nothing will be spawned in place of your tile code.
    lua["define_tile_code"] = [](std::string tile_code)
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        backend->g_state->level_gen->data->define_tile_code(std::move(tile_code));
    };

    /// Define a new procedural spawn, the function `nil do_spawn(x, y, layer)` contains your code to spawn the thing, whatever it is.
    /// The function `bool is_valid(x, y, layer)` determines whether the spawn is legal in the given position and layer.
    /// Use for example when you can spawn only on the ceiling, under water or inside a shop.
    /// Set `is_valid` to `nil` in order to use the default rule (aka. on top of floor and not obstructed).
    /// If a user disables your script but still uses your level mod nothing will be spawned in place of your procedural spawn.
    lua["define_procedural_spawn"] = [](std::string procedural_spawn, sol::function do_spawn, sol::function is_valid) -> PROCEDURAL_CHANCE
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        LevelGenData* data = backend->g_state->level_gen->data;
        uint32_t chance = data->define_chance(std::move(procedural_spawn));
        std::function<bool(float, float, int)> is_valid_call{nullptr};
        if (is_valid)
        {
            is_valid_call = [backend, is_valid_lua = std::move(is_valid)](float x, float y, int layer)
            {
                std::lock_guard lock{backend->gil};
                return backend->handle_function_with_return<bool>(is_valid_lua, x, y, layer).value_or(false);
            };
        }
        std::function<void(float, float, int)> do_spawn_call = [backend, do_spawn_lua = std::move(do_spawn)](float x, float y, int layer)
        {
            std::lock_guard lock{backend->gil};
            return backend->handle_function_with_return<bool>(do_spawn_lua, x, y, layer).value_or(false);
        };
        std::uint32_t id = data->register_chance_logic_provider(chance, SpawnLogicProvider{std::move(is_valid_call), std::move(do_spawn_call)});
        backend->chance_callbacks.push_back(id);
        return chance;
    };

    /// Define a new extra spawn, these are semi-guaranteed level gen spawns with a fixed upper bound.
    /// The function `nil do_spawn(x, y, layer)` contains your code to spawn the thing, whatever it is.
    /// The function `bool is_valid(x, y, layer)` determines whether the spawn is legal in the given position and layer.
    /// Use for example when you can spawn only on the ceiling, under water or inside a shop.
    /// Set `is_valid` to `nil` in order to use the default rule (aka. on top of floor and not obstructed).
    /// To change the number of spawns use `PostRoomGenerationContext::set_num_extra_spawns` during `ON.POST_ROOM_GENERATION`
    /// No name is attached to the extra spawn since it is not modified from level files, instead every call to this function will return a new uniqe id.
    lua["define_extra_spawn"] = [](sol::function do_spawn, sol::function is_valid, std::uint32_t num_spawns_frontlayer, std::uint32_t num_spawns_backlayer) -> std::uint32_t
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        std::function<bool(float, float, int)> is_valid_call{nullptr};
        if (is_valid)
        {
            is_valid_call = [backend, is_valid_lua = std::move(is_valid)](float x, float y, int layer)
            {
                std::lock_guard lock{backend->gil};
                return backend->handle_function_with_return<bool>(is_valid_lua, x, y, layer).value_or(false);
            };
        }
        std::function<void(float, float, int)> do_spawn_call = [backend, do_spawn_lua = std::move(do_spawn)](float x, float y, int layer)
        {
            std::lock_guard lock{backend->gil};
            return backend->handle_function_with_return<bool>(do_spawn_lua, x, y, layer).value_or(false);
        };
        LevelGenData* data = backend->g_state->level_gen->data;
        std::uint32_t extra_spawn_id = data->define_extra_spawn(num_spawns_frontlayer, num_spawns_backlayer, SpawnLogicProvider{std::move(is_valid_call), std::move(do_spawn_call)});
        backend->extra_spawn_callbacks.push_back(extra_spawn_id);
        return extra_spawn_id;
    };
    /// Use to query whether any of the requested spawns could not be made, usually because there were not enough valid spaces in the level.
    /// Returns missing spawns in the front layer and missing spawns in the back layer in that order.
    /// The value only makes sense after level generation is complete, aka after `ON.POST_LEVEL_GENERATION` has run.
    lua["get_missing_extra_spawns"] = [](std::uint32_t extra_spawn_chance_id) -> std::pair<std::uint32_t, std::uint32_t> {
        return State::get().ptr()->level_gen->data->get_missing_extra_spawns(extra_spawn_chance_id);
    };

    /// Transform a position to a room index to be used in `get_room_template` and `PostRoomGenerationContext.set_room_template`
    lua["get_room_index"] = [](float x, float y) -> std::pair<int, int> {
        return State::get().ptr_local()->level_gen->get_room_index(x, y);
    };
    /// Transform a room index into the top left corner position in the room
    lua["get_room_pos"] = [](int x, int y) -> std::pair<float, float> {
        return State::get().ptr_local()->level_gen->get_room_pos(x, y);
    };
    /// Get the room template given a certain index
    lua["get_room_template"] = [](int x, int y, LAYER l) -> std::optional<uint16_t> {
        return State::get().ptr_local()->level_gen->get_room_template(x, y, l);
    };
    /// For debugging only, get the name of a room template
    lua["get_room_template_name"] = [](int16_t room_template) -> std::string_view
    {
        return State::get().ptr_local()->level_gen->get_room_template_name(room_template);
    };

    /// Define a new room remplate to use with `set_room_template`
    lua["define_room_template"] = [](std::string room_template, ROOM_TEMPLATE_TYPE type) -> uint16_t
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        return backend->g_state->level_gen->data->define_room_template(std::move(room_template), static_cast<RoomTemplateType>(type));
    };

    /// Get the inverse chance of a procedural spawn for the current level.
    /// A return value of 0 does not mean the chance is infinite, it means the chance is zero.
    lua["get_procedural_spawn_chance"] = [](PROCEDURAL_CHANCE chance_id) -> uint32_t
    {
        return State::get().ptr_local()->level_gen->get_procedural_spawn_chance(chance_id);
    };

    /// Gets the sub theme of the current cosmic ocean level, returns `COSUBTHEME.NONE` if the current level is not a CO level.
    lua["get_co_subtheme"] = get_co_subtheme;
    /// Forces the theme of the next cosmic ocean level(s) (use e.g. `force_co_subtheme(COSUBTHEME.JUNGLE)`. Use `COSUBTHEME.RESET` to reset to default random behaviour)
    lua["force_co_subtheme"] = force_co_subtheme;

    // Context received in ON.PRE_LOAD_LEVEL_FILES, used for forcing specific `.lvl` files to load.
    lua.new_usertype<PreLoadLevelFilesContext>(
        "PreLoadLevelFilesContext",
        sol::no_constructor,
        "override_level_files",
        &PreLoadLevelFilesContext::override_level_files,
        "add_level_files",
        &PreLoadLevelFilesContext::add_level_files);

    lua.new_usertype<DoorCoords>("DoorCoords", sol::no_constructor, "door1_x", &DoorCoords::door1_x, "door1_y", &DoorCoords::door1_y, "door2_x", &DoorCoords::door2_x, "door2_y", &DoorCoords::door2_y);

    lua.new_usertype<LevelGenSystem>("LevelGenSystem", sol::no_constructor, "shop_type", &LevelGenSystem::shop_type, "spawn_x", &LevelGenSystem::spawn_x, "spawn_y", &LevelGenSystem::spawn_y, "spawn_room_x", &LevelGenSystem::spawn_room_x, "spawn_room_y", &LevelGenSystem::spawn_room_y, "exits", &LevelGenSystem::exit_doors_locations);

    // Context received in ON.POST_ROOM_GENERATION.
    // Used to change the room templates in the level and other shenanigans that affect level gen.
    lua.new_usertype<PostRoomGenerationContext>(
        "PostRoomGenerationContext",
        sol::no_constructor,
        "set_room_template",
        &PostRoomGenerationContext::set_room_template,
        "set_procedural_spawn_chance",
        &PostRoomGenerationContext::set_procedural_spawn_chance,
        "set_num_extra_spawns",
        &PostRoomGenerationContext::set_num_extra_spawns);

    lua.new_usertype<QuestsInfo>(
        "QuestsInfo",
        "yang_state",
        &QuestsInfo::yang_state,
        "jungle_sisters_flags",
        &QuestsInfo::jungle_sisters_flags,
        "van_horsing_state",
        &QuestsInfo::van_horsing_state,
        "sparrow_state",
        &QuestsInfo::sparrow_state,
        "madame_tusk_state",
        &QuestsInfo::madame_tusk_state,
        "beg_state",
        &QuestsInfo::beg_state);

    lua.new_usertype<SaveData>(
        "SaveData",
        "places",
        sol::readonly(&SaveData::places),
        "bestiary",
        sol::readonly(&SaveData::bestiary),
        "people",
        sol::readonly(&SaveData::people),
        "items",
        sol::readonly(&SaveData::items),
        "traps",
        sol::readonly(&SaveData::traps),
        "last_daily",
        sol::readonly(&SaveData::last_daily),
        "characters",
        sol::readonly(&SaveData::characters),
        "shortcuts",
        sol::readonly(&SaveData::shortcuts),
        "bestiary_killed",
        sol::readonly(&SaveData::bestiary_killed),
        "bestiary_killed_by",
        sol::readonly(&SaveData::bestiary_killed_by),
        "people_killed",
        sol::readonly(&SaveData::people_killed),
        "people_killed_by",
        sol::readonly(&SaveData::people_killed_by),
        "plays",
        sol::readonly(&SaveData::plays),
        "deaths",
        sol::readonly(&SaveData::deaths),
        "wins_normal",
        sol::readonly(&SaveData::wins_normal),
        "wins_hard",
        sol::readonly(&SaveData::wins_hard),
        "wins_special",
        sol::readonly(&SaveData::wins_special),
        "score_total",
        sol::readonly(&SaveData::score_total),
        "score_top",
        sol::readonly(&SaveData::score_top),
        "deepest_area",
        sol::readonly(&SaveData::deepest_area),
        "deepest_level",
        sol::readonly(&SaveData::deepest_level),
        "time_best",
        sol::readonly(&SaveData::time_best),
        "time_total",
        sol::readonly(&SaveData::time_total),
        "time_tutorial",
        sol::readonly(&SaveData::time_tutorial),
        "character_deaths",
        sol::readonly(&SaveData::character_deaths),
        "pets_rescued",
        sol::readonly(&SaveData::pets_rescued),
        "completed_normal",
        sol::readonly(&SaveData::completed_normal),
        "completed_ironman",
        sol::readonly(&SaveData::completed_ironman),
        "completed_hard",
        sol::readonly(&SaveData::completed_hard),
        "profile_seen",
        sol::readonly(&SaveData::profile_seen),
        "seeded_unlocked",
        sol::readonly(&SaveData::seeded_unlocked),
        "world_last",
        sol::readonly(&SaveData::world_last),
        "level_last",
        sol::readonly(&SaveData::level_last),
        "score_last",
        sol::readonly(&SaveData::score_last),
        "time_last",
        sol::readonly(&SaveData::time_last),
        "stickers",
        sol::readonly(&SaveData::stickers),
        "players",
        sol::readonly(&SaveData::players));

    lua.create_named_table("LAYER", "FRONT", 0, "BACK", 1, "PLAYER", -1, "PLAYER1", -1, "PLAYER2", -2, "PLAYER3", -3, "PLAYER4", -4, "BOTH", -128);
    lua.create_named_table(
        "THEME",
        "DWELLING",
        1,
        "JUNGLE",
        2,
        "VOLCANA",
        3,
        "OLMEC",
        4,
        "TIDE_POOL",
        5,
        "TEMPLE",
        6,
        "ICE_CAVES",
        7,
        "NEO_BABYLON",
        8,
        "SUNKEN_CITY",
        9,
        "COSMIC_OCEAN",
        10,
        "CITY_OF_GOLD",
        11,
        "DUAT",
        12,
        "ABZU",
        13,
        "TIAMAT",
        14,
        "EGGPLANT_WORLD",
        15,
        "HUNDUN",
        16,
        "BASE_CAMP",
        17,
        "ARENA",
        18);

    /// Parameter to force_co_subtheme
    lua.create_named_table("COSUBTHEME", "NONE", -1, "RESET", -1, "DWELLING", 0, "JUNGLE", 1, "VOLCANA", 2, "TIDE_POOL", 3, "TEMPLE", 4, "ICE_CAVES", 5, "NEO_BABYLON", 6, "SUNKEN_CITY", 7);

    /// Yang quest states
    lua.create_named_table("YANG", "ANGRY", -1, "QUEST_NOT_STARTED", 0, "TURKEY_PEN_SPAWNED", 2, "BOTH_TURKEYS_DELIVERED", 3, "TURKEY_SHOP_SPAWNED", 4, "ONE_TURKEY_BOUGHT", 5, "TWO_TURKEYS_BOUGHT", 6, "THREE_TURKEYS_BOUGHT", 7);

    /// Jungle sister quest flags (angry = -1)
    lua.create_named_table("JUNGLESISTERS", "PARSLEY_RESCUED", 1, "PARSNIP_RESCUED", 2, "PARMESAN_RESCUED", 3, "WARNING_ONE_WAY_DOOR", 4, "GREAT_PARTY_HUH", 5, "I_WISH_BROUGHT_A_JACKET", 6);

    /// Van Horsing quest states
    lua.create_named_table("VANHORSING", "QUEST_NOT_STARTED", 0, "JAILCELL_SPAWNED", 1, "FIRST_ENCOUNTER_DIAMOND_THROWN", 2, "SPAWNED_IN_VLADS_CASTLE", 3, "SHOT_VLAD", 4, "TEMPLE_HIDEOUT_SPAWNED", 5, "SECOND_ENCOUNTER_COMPASS_THROWN", 6, "TUSK_CELLAR", 7);

    /// Sparrow quest states
    lua.create_named_table("SPARROW", "QUEST_NOT_STARTED", 0, "THIEF_STATUS", 1, "FINISHED_LEVEL_WITH_THIEF_STATUS", 2, "FIRST_HIDEOUT_SPAWNED_ROPE_THROW", 3, "FIRST_ENCOUNTER_ROPES_THROWN", 4, "TUSK_IDOL_STOLEN", 5, "SECOND_HIDEOUT_SPAWNED_NEOBAB", 6, "SECOND_ENCOUNTER_INTERACTED", 7, "MEETING_AT_TUSK_BASEMENT", 8);

    /// Madame Tusk quest states
    lua.create_named_table("TUSK", "ANGRY", -2, "DEAD", -1, "QUEST_NOT_STARTED", 0, "DICE_HOUSE_SPAWNED", 1, "HIGH_ROLLER_STATUS", 2, "PALACE_WELCOME_MESSAGE", 3);

    /// Beg quest states
    lua.create_named_table("BEG", "QUEST_NOT_STARTED", 0, "ALTAR_DESTROYED", 1, "SPAWNED_WITH_BOMBBAG", 2, "BOMBBAG_THROWN", 3, "SPAWNED_WITH_TRUECROWN", 4, "TRUECROWN_THROWN", 5);

    /// Use in `define_room_template` to declare whether a room template has any special behavior
    lua.create_named_table("ROOM_TEMPLATE_TYPE", "NONE", 0, "ENTRANCE", 1, "EXIT", 2, "SHOP", 3);

    /// Determines which kind of shop spawns in the level, if any
    lua.create_named_table("SHOP_TYPE", "GENERAL_STORE", 0, "CLOTHING_SHOP", 1, "WEAPON_SHOP", 2, "SPECIALTY_SHOP", 3, "HIRED_HAND_SHOP", 4, "PET_SHOP", 5, "DICE_SHOP", 6, "TUSK_DICE_SHOP", 13);

    lua.create_named_table("ROOM_TEMPLATE"
                           //, "SIDE", 0
                           //, "", ...check__[room_templates.txt]\[game_data/room_templates.txt\]...
    );
    for (const auto& [room_name, room_template] : State::get().ptr()->level_gen->data->room_templates())
    {
        std::string clean_room_name = room_name;
        std::transform(
            clean_room_name.begin(), clean_room_name.end(), clean_room_name.begin(), [](unsigned char c)
            { return (unsigned char)std::toupper(c); });
        std::replace(clean_room_name.begin(), clean_room_name.end(), '-', '_');
        lua["ROOM_TEMPLATE"][std::move(clean_room_name)] = room_template.id;
    };

    lua.create_named_table("PROCEDURAL_CHANCE"
                           //, "ARROWTRAP_CHANCE", 0
                           //, "", ...check__[spawn_chances.txt]\[game_data/spawn_chances.txt\]...
    );
    auto* state = State::get().ptr();
    for (auto* chances : {&state->level_gen->data->monster_chances(), &state->level_gen->data->trap_chances()})
    {
        for (const auto& [chance_name, chance] : *chances)
        {
            std::string clean_chance_name = chance_name;
            std::transform(
                clean_chance_name.begin(), clean_chance_name.end(), clean_chance_name.begin(), [](unsigned char c)
                { return (unsigned char)std::toupper(c); });
            std::replace(clean_chance_name.begin(), clean_chance_name.end(), '-', '_');
            lua["PROCEDURAL_CHANCE"][std::move(clean_chance_name)] = chance.id;
        }
    }
}
}; // namespace NLevel
