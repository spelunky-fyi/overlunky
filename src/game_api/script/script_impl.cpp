#include "script_impl.hpp"

#include "file_api.hpp"
#include "level_api.hpp"
#include "lua_libs/lua_libs.hpp"
#include "overloaded.hpp"
#include "particles.hpp"
#include "render_api.hpp"
#include "rpc.hpp"
#include "script_util.hpp"
#include "sound_manager.hpp"
#include "state.hpp"

#include "usertypes/char_state.hpp"
#include "usertypes/drops_lua.hpp"
#include "usertypes/entity_lua.hpp"
#include "usertypes/flags_lua.hpp"
#include "usertypes/gui_lua.hpp"
#include "usertypes/level_lua.hpp"
#include "usertypes/particles_lua.hpp"
#include "usertypes/player_lua.hpp"
#include "usertypes/save_context.hpp"
#include "usertypes/sound_lua.hpp"
#include "usertypes/state_lua.hpp"
#include "usertypes/texture_lua.hpp"

#include <sol/sol.hpp>

void infinite_loop(lua_State* argst, lua_Debug* argdb)
{
    luaL_error(argst, "Hit Infinite Loop Detection of 1bln instructions");
};

ScriptImpl::ScriptImpl(std::string script, std::string file, SoundManager* sound_mgr, bool enable)
{
    sound_manager = sound_mgr;

#ifdef SPEL2_EDITABLE_SCRIPTS
    strcpy(code, script.c_str());
#else
    code_storage = std::move(script);
    code = code_storage.c_str();
#endif
    meta.file = std::move(file);
    meta.path = std::filesystem::path(meta.file).parent_path().string();
    meta.filename = std::filesystem::path(meta.file).filename().string();
    meta.stem = std::filesystem::path(meta.file).stem().string();

    script_folder = std::filesystem::path(meta.file).parent_path();

    enabled = enable;

    g_state = get_state_ptr();
    g_items = list_entities();
    g_players = get_players();
    g_save = savedata();

    if (!g_players.empty())
        state.player = g_players.at(0);
    else
        state.player = nullptr;
    state.screen = g_state->screen;
    state.time_level = g_state->time_level;
    state.time_total = g_state->time_total;
    state.time_global = get_frame_count();
    state.frame = get_frame_count();
    state.loading = g_state->loading;
    state.reset = (g_state->quest_flags & 1);
    state.quest_flags = g_state->quest_flags;

    lua.open_libraries(sol::lib::math, sol::lib::base, sol::lib::string, sol::lib::table, sol::lib::coroutine, sol::lib::package, sol::lib::debug);
    require_json_lua(lua);
    require_inspect_lua(lua);
    require_format_lua(lua);

    /// Table of strings where you should set some script metadata shown in the UI.
    /// - `meta.name` Script name
    /// - `meta.version` Version
    /// - `meta.description` Short description of the script
    /// - `meta.author` Your name
    lua["meta"] = lua.create_named_table("meta");

    try
    {
        std::string metacode = "";
        std::stringstream metass(code);
        std::regex reg("(^\\s*meta\\.[a-z]+\\s*=)");
        std::regex regstart("(^\\s*meta\\s*=)");
        std::regex regend("(\\})");
        std::regex multistart("\\[\\[|\\.\\.\\s*($|--)|\\bmeta\\.[a-z]+\\s*=\\s*($|--)");
        std::regex multiend("\\]\\]\\s*($|--)|[\"']\\s*($|--)");
        bool getmeta = false;
        bool getmulti = false;
        for (std::string line; std::getline(metass, line);)
        {
            if (std::regex_search(line, regstart))
            {
                getmeta = true;
            }
            if (std::regex_search(line, reg) && std::regex_search(line, multistart))
            {
                getmulti = true;
            }
            if (std::regex_search(line, reg) || getmeta || getmulti)
            {
                metacode += line + "\n";
            }
            if (std::regex_search(line, multiend))
            {
                getmulti = false;
            }
            if (std::regex_search(line, regend))
            {
                getmeta = false;
            }
        }
        auto lua_result = lua.safe_script(metacode.data());
        sol::optional<std::string> meta_name = lua["meta"]["name"];
        sol::optional<std::string> meta_version = lua["meta"]["version"];
        sol::optional<std::string> meta_description = lua["meta"]["description"];
        sol::optional<std::string> meta_author = lua["meta"]["author"];
        sol::optional<bool> meta_unsafe = lua["meta"]["unsafe"];
        meta.name = meta_name.value_or(meta.filename);
        meta.version = meta_version.value_or("");
        meta.description = meta_description.value_or("");
        meta.author = meta_author.value_or("Anonymous");
        meta.unsafe = meta_unsafe.value_or(false);
        meta.id = script_id();
        result = "Got metadata";
    }
    catch (const sol::error& e)
    {
        result = e.what();
#ifdef SPEL2_EXTRA_ANNOYING_SCRIPT_ERRORS
        messages.push_back({result, std::chrono::system_clock::now(), ImVec4(1.0f, 0.2f, 0.2f, 1.0f)});
        DEBUG("{}", result);
        if (messages.size() > 20)
            messages.pop_front();
#endif
    }

    /// A bunch of [game state](#statememory) variables
    /// Example:
    /// ```lua
    /// if state.time_level > 300 and state.theme == THEME.DWELLING then
    ///     toast("Congratulations for lasting 5 seconds in Dwelling")
    /// end
    /// ```
    lua["state"] = g_state;
    /// An array of [Player](#player) of the current players. Pro tip: You need `players[1].uid` in most entity functions.
    lua["players"] = std::vector<Movable*>(g_players.begin(), g_players.end());
    /// Provides a read-only access to the save data, updated as soon as something changes (i.e. before it's written to savegame.sav.)
    lua["savegame"] = g_save;

    /// Print a log message on screen.
    lua["message"] = [this](std::string message) -> void
    {
        messages.push_back({message, std::chrono::system_clock::now(), ImVec4(1.0f, 1.0f, 1.0f, 1.0f)});
        if (messages.size() > 20)
            messages.pop_front();
    };
    /// Returns unique id for the callback to be used in [clear_callback](#clear_callback).
    /// Add per level callback function to be called every `frames` engine frames. Timer is paused on pause and cleared on level transition.
    lua["set_interval"] = [this](sol::function cb, int frames) -> CallbackId
    {
        auto luaCb = IntervalCallback{cb, frames, -1};
        level_timers[cbcount] = luaCb;
        return cbcount++;
    };
    /// Returns unique id for the callback to be used in [clear_callback](#clear_callback).
    /// Add per level callback function to be called after `frames` engine frames. Timer is paused on pause and cleared on level transition.
    lua["set_timeout"] = [this](sol::function cb, int frames) -> CallbackId
    {
        int now = g_state->time_level;
        auto luaCb = TimeoutCallback{cb, now + frames};
        level_timers[cbcount] = luaCb;
        return cbcount++;
    };
    /// Returns unique id for the callback to be used in [clear_callback](#clear_callback).
    /// Add global callback function to be called every `frames` engine frames. This timer is never paused or cleared.
    lua["set_global_interval"] = [this](sol::function cb, int frames) -> CallbackId
    {
        auto luaCb = IntervalCallback{cb, frames, -1};
        global_timers[cbcount] = luaCb;
        return cbcount++;
    };
    /// Returns unique id for the callback to be used in [clear_callback](#clear_callback).
    /// Add global callback function to be called after `frames` engine frames. This timer is never paused or cleared.
    lua["set_global_timeout"] = [this](sol::function cb, int frames) -> CallbackId
    {
        int now = get_frame_count();
        auto luaCb = TimeoutCallback{cb, now + frames};
        global_timers[cbcount] = luaCb;
        return cbcount++;
    };
    /// Returns unique id for the callback to be used in [clear_callback](#clear_callback).
    /// Add global callback function to be called on an [event](#on).
    lua["set_callback"] = [this](sol::function cb, int screen) -> CallbackId
    {
        auto luaCb = ScreenCallback{cb, (ON)screen, -1};
        if (luaCb.screen == ON::LOAD)
            load_callbacks[cbcount] = luaCb; // Make sure load always runs before other callbacks
        else
            callbacks[cbcount] = luaCb;
        return cbcount++;
    };
    /// Clear previously added callback `id`
    lua["clear_callback"] = [this](CallbackId id)
    { clear_callbacks.push_back(id); };

    /// Table of options set in the UI, added with the [register_option_functions](#register_option_int).
    lua["options"] = lua.create_named_table("options");
    /// Load another script by id "author/name"
    lua["load_script"] = [this](std::string id)
    { required_scripts.push_back(sanitize(id)); };
    /// Seed the game prng.
    lua["seed_prng"] = [this](int64_t seed)
    {
        auto seed_prng = get_seed_prng();
        seed_prng(seed);
    };
    /// Read the game prng state. Maybe you can use these and math.randomseed() to make deterministic things, like online scripts :shrug:. Example:
    /// ```lua
    /// -- this should always print the same table D877...E555
    /// set_callback(function()
    ///   seed_prng(42069)
    ///   local prng = read_prng()
    ///   for i,v in ipairs(prng) do
    ///     message(string.format("%08X", v))
    ///   end
    /// end, ON.LEVEL)
    /// ```
    lua["read_prng"] = [this]() -> std::vector<int64_t> { return read_prng(); };
    /// Show a message that looks like a level feeling.
    lua["toast"] = [this](std::wstring message)
    {
        auto toast = get_toast();
        toast(NULL, message.data());
    };
    /// Show a message coming from an entity
    lua["say"] = [this](uint32_t entity_uid, std::wstring message, int unk_type, bool top)
    {
        auto say = get_say();
        auto entity = get_entity_ptr(entity_uid);
        if (entity == nullptr)
            return;
        say(NULL, entity, message.data(), unk_type, top);
    };
    /// Add an integer option that the user can change in the UI. Read with `options.name`, `value` is the default. Keep in mind these are just soft
    /// limits, you can override them in the UI with double click.
    // lua["register_option_int"] = [this](std::string name, std::string desc, std::string long_desc, int value, int min, int max)
    lua["register_option_int"] = sol::overload(
        [this](std::string name, std::string desc, std::string long_desc, int value, int min, int max)
        {
            options[name] = {desc, long_desc, IntOption{value, min, max}};
            lua["options"][name] = value;
        },
        [this](std::string name, std::string desc, int value, int min, int max)
        {
            options[name] = {desc, "", IntOption{value, min, max}};
            lua["options"][name] = value;
        });
    /// Add a float option that the user can change in the UI. Read with `options.name`, `value` is the default. Keep in mind these are just soft
    /// limits, you can override them in the UI with double click.
    // lua["register_option_float"] = [this](std::string name, std::string desc, std::string long_desc, float value, float min, float max)
    lua["register_option_float"] = sol::overload(
        [this](std::string name, std::string desc, std::string long_desc, float value, float min, float max)
        {
            options[name] = {desc, long_desc, FloatOption{value, min, max}};
            lua["options"][name] = value;
        },
        [this](std::string name, std::string desc, float value, float min, float max)
        {
            options[name] = {desc, "", FloatOption{value, min, max}};
            lua["options"][name] = value;
        });
    /// Add a boolean option that the user can change in the UI. Read with `options.name`, `value` is the default.
    // lua["register_option_bool"] = [this](std::string name, std::string desc, std::string long_desc, bool value)
    lua["register_option_bool"] = sol::overload(
        [this](std::string name, std::string desc, std::string long_desc, bool value)
        {
            options[name] = {desc, long_desc, BoolOption{value}};
            lua["options"][name] = value;
        },
        [this](std::string name, std::string desc, bool value)
        {
            options[name] = {desc, "", BoolOption{value}};
            lua["options"][name] = value;
        });
    /// Add a string option that the user can change in the UI. Read with `options.name`, `value` is the default.
    // lua["register_option_string"] = [this](std::string name, std::string desc, std::string long_desc, std::string value)
    lua["register_option_string"] = sol::overload(
        [this](std::string name, std::string desc, std::string long_desc, std::string value)
        {
            options[name] = {desc, long_desc, StringOption{value}};
            lua["options"][name] = value;
        },
        [this](std::string name, std::string desc, std::string value)
        {
            options[name] = {desc, "", StringOption{value}};
            lua["options"][name] = value;
        });
    /// Add a combobox option that the user can change in the UI. Read the int index of the selection with `options.name`. Separate `opts` with `\0`,
    /// with a double `\0\0` at the end.
    // lua["register_option_combo"] = [this](std::string name, std::string desc, std::string long_desc, std::string opts)
    lua["register_option_combo"] = sol::overload(
        [this](std::string name, std::string desc, std::string long_desc, std::string opts)
        {
            options[name] = {desc, long_desc, ComboOption{0, opts}};
            lua["options"][name] = 1;
        },
        [this](std::string name, std::string desc, std::string opts)
        {
            options[name] = {desc, "", ComboOption{0, opts}};
            lua["options"][name] = 1;
        });
    /// Add a button that the user can click in the UI. Sets the timestamp of last click on value and runs the callback function.
    // lua["register_option_combo"] = [this](std::string name, std::string desc, std::string long_desc, sol::function on_click)
    lua["register_option_button"] = sol::overload(
        [this](std::string name, std::string desc, std::string long_desc, sol::function callback)
        {
            options[name] = {desc, long_desc, ButtonOption{callback}};
            lua["options"][name] = -1;
        },
        [this](std::string name, std::string desc, sol::function callback)
        {
            options[name] = {desc, "", ButtonOption{callback}};
            lua["options"][name] = -1;
        });

    /// Spawn an entity in position with some velocity and return the uid of spawned entity.
    /// Uses level coordinates with [LAYER.FRONT](#layer) and LAYER.BACK, but player-relative coordinates with LAYER.PLAYERn.
    /// Example:
    /// ```lua
    /// -- spawn megajelly using absolute coordinates
    /// set_callback(function()
    ///     x, y, layer = get_position(players[1].uid)
    ///     spawn_entity(ENT_TYPE.MONS_MEGAJELLYFISH, x, y+3, layer, 0, 0)
    /// end, ON.LEVEL)
    /// -- spawn clover using player-relative coordinates
    /// set_callback(function()
    ///     spawn(ENT_TYPE.ITEM_PICKUP_CLOVER, 0, 1, LAYER.PLAYER1, 0, 0)
    /// end, ON.LEVEL)
    /// ```
    lua["spawn_entity"] = spawn_entity_abs;
    /// Short for [spawn_entity](#spawn_entity).
    lua["spawn"] = spawn_entity_abs;
    /// Spawn a door to another world, level and theme and return the uid of spawned entity.
    /// Uses level coordinates with LAYER.FRONT and LAYER.BACK, but player-relative coordinates with LAYER.PLAYERn
    lua["spawn_door"] = spawn_door_abs;
    /// Short for [spawn_door](#spawn_door).
    lua["door"] = spawn_door_abs;
    /// Spawn a door to backlayer.
    lua["spawn_layer_door"] = spawn_backdoor_abs;
    /// Short for [spawn_layer_door](#spawn_layer_door).
    lua["layer_door"] = spawn_backdoor_abs;
    /// Warp to a level immediately.
    lua["warp"] = warp;
    /// Set seed and reset run.
    lua["set_seed"] = set_seed;
    /// Enable/disable godmode.
    lua["god"] = godmode;
    /// Try to force next levels to be dark.
    lua["force_dark_level"] = darkmode;
    /// Set the zoom level used in levels and shops. 13.5 is the default.
    lua["zoom"] = zoom;
    /// Enable/disable game engine pause.
    lua["pause"] = [this](bool p)
    {
        if (p)
            set_pause(0x20);
        else
            set_pause(0);
    };
    /// Teleport entity to coordinates with optional velocity
    lua["move_entity"] = move_entity_abs;
    /// Make an ENT_TYPE.FLOOR_DOOR_EXIT go to world `w`, level `l`, theme `t`
    lua["set_door_target"] = set_door_target;
    /// Short for [set_door_target](#set_door_target).
    lua["set_door"] = set_door_target;
    /// Get door target `world`, `level`, `theme`
    lua["get_door_target"] = get_door_target;
    /// Set the contents of ENT_TYPE.ITEM_POT, ENT_TYPE.ITEM_CRATE or ENT_TYPE.ITEM_COFFIN `uid` to ENT_TYPE... `item_entity_type`
    lua["set_contents"] = set_contents;
    /// Get the [Entity](#entity) behind an uid
    lua["get_entity"] = get_entity_ptr;
    /// Get the [EntityDB](#entitydb) behind an ENT_TYPE...
    lua["get_type"] = get_type;
    /// Get uids of all entities currently loaded
    lua["get_entities"] = get_entities;
    /// Get uids of entities by some conditions. Set `entity_type` or `mask` to `0` to ignore that.
    lua["get_entities_by"] = get_entities_by;
    /// Get uids of entities matching id. This function is variadic, meaning it accepts any number of id's.
    /// You can even pass a table! Example:
    /// ```lua
    /// types = {ENT_TYPE.MONS_SNAKE, ENT_TYPE.MONS_BAT}
    /// function on_level()
    ///     uids = get_entities_by_type(ENT_TYPE.MONS_SNAKE, ENT_TYPE.MONS_BAT)
    ///     -- is not the same thing as this, but also works
    ///     uids2 = get_entities_by_type(entity_types)
    ///     message(tostring(#uids).." == "..tostring(#uids2))
    /// end
    /// ```
    lua["get_entities_by_type"] = [](sol::variadic_args va) -> std::vector<uint32_t>
    {
        sol::type type = va.get_type();
        if (type == sol::type::number)
        {
            auto args = std::vector<uint32_t>(va.begin(), va.end());
            auto get_func = sol::resolve<std::vector<uint32_t>(std::vector<uint32_t>)>(get_entities_by_type);
            return get_func(args);
        }
        else if (type == sol::type::table)
        {
            auto args = va.get<std::vector<uint32_t>>(0);
            auto get_func = sol::resolve<std::vector<uint32_t>(std::vector<uint32_t>)>(get_entities_by_type);
            return get_func(args);
        }
        return std::vector<uint32_t>({});
    };
    /// Get uids of entities by some search_flags
    lua["get_entities_by_mask"] = get_entities_by_mask;
    /// Get uids of entities by layer. `0` for main level, `1` for backlayer, `-1` for layer of the player.
    lua["get_entities_by_layer"] = get_entities_by_layer;
    /// Get uids of matching entities inside some radius. Set `entity_type` or `mask` to `0` to ignore that.
    lua["get_entities_at"] = get_entities_at;
    /// Get uids of matching entities overlapping with the given rect. Set `entity_type` or `mask` to `0` to ignore that.
    lua["get_entities_overlapping"] = get_entities_overlapping;
    /// Get the `flags` field from entity by uid
    lua["get_entity_flags"] = get_entity_flags;
    /// Set the `flags` field from entity by uid
    lua["set_entity_flags"] = set_entity_flags;
    /// Get the `more_flags` field from entity by uid
    lua["get_entity_flags2"] = get_entity_flags2;
    /// Set the `more_flags` field from entity by uid
    lua["set_entity_flags2"] = set_entity_flags2;
    /// Get the `move_state` field from entity by uid
    lua["get_entity_ai_state"] = get_entity_ai_state;
    /// Get `state.level_flags`
    lua["get_level_flags"] = get_hud_flags;
    /// Set `state.level_flags`
    lua["set_level_flags"] = set_hud_flags;
    /// Get the ENT_TYPE... for entity by uid
    lua["get_entity_type"] = get_entity_type;
    /// Get the current set zoom level
    lua["get_zoom_level"] = get_zoom_level;
    /// Get the game coordinates at the screen position (`x`, `y`)
    lua["game_position"] = click_position;
    /// Translate an entity position to screen position to be used in drawing functions
    lua["screen_position"] = screen_position;
    /// Translate a distance of `x` tiles to screen distance to be be used in drawing functions
    lua["screen_distance"] = screen_distance;
    /// Get position `x, y, layer` of entity by uid. Use this, don't use `Entity.x/y` because those are sometimes just the offset to the entity
    /// you're standing on, not real level coordinates.
    lua["get_position"] = get_position;
    /// Get interpolated render position `x, y, layer` of entity by uid. This gives smooth hitboxes for 144Hz master race etc...
    lua["get_render_position"] = get_render_position;
    /// Remove item by uid from entity
    lua["entity_remove_item"] = entity_remove_item;
    /// Spawn an entity of `entity_type` attached to some other entity `over_uid`, in offset `x`, `y`
    lua["spawn_entity_over"] = spawn_entity_over;
    /// Check if the entity `uid` has some specific `item_uid` by uid in their inventory
    lua["entity_has_item_uid"] = entity_has_item_uid;
    /// Check if the entity `uid` has some ENT_TYPE `entity_type` in their inventory
    lua["entity_has_item_type"] = entity_has_item_type;
    /// Gets all items of `entity_type` and `mask` from an entity's inventory. Set `entity_type` and `mask` to 0 to return all inventory items.
    lua["entity_get_items_by"] = entity_get_items_by;
    /// Kills an entity by uid.
    lua["kill_entity"] = kill_entity;
    /// Pick up another entity by uid. Make sure you're not already holding something, or weird stuff will happen. Example:
    /// ```lua
    /// -- spawn and equip a jetpack
    /// pick_up(players[1].uid, spawn(ENT_TYPE.ITEM_JETPACK, 0, 0, LAYER.PLAYER, 0, 0))
    /// ```
    lua["pick_up"] = pick_up;
    /// Apply changes made in [get_type](#get_type)() to entity instance by uid.
    lua["apply_entity_db"] = apply_entity_db;
    /// Try to lock the exit at coordinates
    lua["lock_door_at"] = lock_door_at;
    /// Try to unlock the exit at coordinates
    lua["unlock_door_at"] = unlock_door_at;
    /// Get the current global frame count since the game was started. You can use this to make some timers yourself, the engine runs at 60fps.
    lua["get_frame"] = get_frame_count;
    /// Get the current timestamp in milliseconds since the Unix Epoch.
    lua["get_ms"] = []()
    { return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(); };
    /// Make `mount_uid` carry `rider_uid` on their back. Only use this with actual mounts and living things.
    lua["carry"] = carry;
    /// Sets the arrow type (wooden, metal, light) that is shot from a regular arrow trap and a poison arrow trap.
    lua["set_arrowtrap_projectile"] = set_arrowtrap_projectile;
    /// Sets the amount of blood drops in the Kapala needed to trigger a health increase (default = 7).
    lua["set_kapala_blood_threshold"] = set_kapala_blood_threshold;
    /// Sets the hud icon for the Kapala (0-6 ; -1 for default behaviour).
    /// If you set a Kapala treshold greater than 7, make sure to set the hud icon in the range 0-6, or other icons will appear in the hud!
    lua["set_kapala_hud_icon"] = set_kapala_hud_icon;
    /// Changes characteristics of (all) sparktraps: speed, rotation direction and distance from center
    /// Speed: expressed as the amount that should be added to the angle every frame (use a negative number to go in the other direction)
    /// Distance from center: if you go above 3.0 the game might crash because a spark may go out of bounds!
    lua["modify_sparktraps"] = modify_sparktraps;
    /// Sets the multiplication factor for blood droplets (default/no Vlad's cape = 1, with Vlad's cape = 2)
    lua["set_blood_multiplication"] = set_blood_multiplication;
    /// Flip entity around by uid. All new entities face right by default.
    lua["flip_entity"] = flip_entity;
    /// Sets the Y-level at which Olmec changes phases
    lua["set_olmec_phase_y_level"] = set_olmec_phase_y_level;
    /// Determines when the ghost appears, either when the player is cursed or not
    lua["set_ghost_spawn_times"] = set_ghost_spawn_times;
    /// Forces the theme of the next cosmic ocean level(s) (use e.g. force_co_subtheme(COSUBTHEME.JUNGLE)  Use COSUBTHEME.RESET to reset to default random behaviour)
    lua["force_co_subtheme"] = force_co_subtheme;
    /// Enables or disables the journal
    lua["set_journal_enabled"] = set_journal_enabled;
    /// Returns how many of a specific entity type Waddler has stored
    lua["waddler_count_entity"] = waddler_count_entity;
    /// Store an entity type in Waddler's storage. Returns the slot number the item was stored in or -1 when storage is full and the item couldn't be stored.
    lua["waddler_store_entity"] = waddler_store_entity;
    /// Removes an entity type from Waddler's storage. Second param determines how many of the item to remove (default = remove all)
    lua["waddler_remove_entity"] = waddler_remove_entity;
    /// Gets the 16-bit meta-value associated with the entity type in the associated slot
    lua["waddler_get_entity_meta"] = waddler_get_entity_meta;
    /// Sets the 16-bit meta-value associated with the entity type in the associated slot
    lua["waddler_set_entity_meta"] = waddler_set_entity_meta;
    /// Gets the entity type of the item in the provided slot
    lua["waddler_entity_type_in_slot"] = waddler_entity_type_in_slot;

    /// Calculate the tile distance of two entities by uid
    lua["distance"] = [this](uint32_t uid_a, uint32_t uid_b) -> float
    {
        Entity* ea = get_entity_ptr(uid_a);
        Entity* eb = get_entity_ptr(uid_b);
        if (ea == nullptr || eb == nullptr)
            return -1.0f;
        else
            return (float)sqrt(pow(ea->position().first - eb->position().first, 2) + pow(ea->position().second - eb->position().second, 2));
    };
    /// Basically gets the absolute coordinates of the area inside the unbreakable bedrock walls, from wall to wall. Every solid entity should be
    /// inside these boundaries. The order is: top left x, top left y, bottom right x, bottom right y Example:
    /// ```lua
    /// -- Draw the level boundaries
    /// set_callback(function()
    ///     xmin, ymin, xmax, ymax = get_bounds()
    ///     sx, sy = screen_position(xmin, ymin) -- top left
    ///     sx2, sy2 = screen_position(xmax, ymax) -- bottom right
    ///     draw_rect(sx, sy, sx2, sy2, 4, 0, rgba(255, 255, 255, 255))
    /// end, ON.GUIFRAME)
    /// ```
    lua["get_bounds"] = [this]() -> std::tuple<float, float, float, float> { return std::make_tuple(2.5f, 122.5f, g_state->w * 10.0f + 2.5f, 122.5f - g_state->h * 8.0f); };
    /// Gets the current camera position in the level
    lua["get_camera_position"] = get_camera_position;
    /// Sets the current camera position in the level.
    /// Note: The camera will still try to follow the player and this doesn't actually work at all.
    lua["set_camera_position"] = set_camera_position;

    /// Set a bit in a number. This doesn't actually change the bit in the entity you pass it, it just returns the new value you can use.
    lua["set_flag"] = [](Flags flags, int bit) -> Flags
    { return flags | (1U << (bit - 1)); };
    lua["setflag"] = lua["set_flag"];
    /// Clears a bit in a number. This doesn't actually change the bit in the entity you pass it, it just returns the new value you can use.
    lua["clr_flag"] = [](Flags flags, int bit) -> Flags
    { return flags & ~(1U << (bit - 1)); };
    lua["clrflag"] = lua["clr_flag"];
    /// Returns true if a bit is set in the flags
    lua["test_flag"] = [](Flags flags, int bit) -> bool
    { return (flags & (1U << (bit - 1))) > 0; };
    lua["testflag"] = lua["test_flag"];

    /// Gets the resolution (width and height) of the screen
    lua["get_window_size"] = [this]() -> std::tuple<int, int> { return {(int)ImGui::GetWindowWidth(), (int)ImGui::GetWindowHeight()}; };

    /// Steal input from a Player or HH.
    lua["steal_input"] = [this](int uid)
    {
        if (script_input.find(uid) != script_input.end())
            return;
        Player* player = get_entity_ptr(uid)->as<Player>();
        if (player == nullptr)
            return;
        ScriptInput* newinput = new ScriptInput();
        newinput->next = 0;
        newinput->current = 0;
        newinput->orig_input = player->input_ptr;
        newinput->orig_ai = player->ai_func;
        player->input_ptr = reinterpret_cast<size_t>(newinput);
        player->ai_func = 0;
        script_input[uid] = newinput;
        // DEBUG("Steal input: {:x} -> {:x}", newinput->orig_input, player->input_ptr);
    };
    /// Return input
    lua["return_input"] = [this](int uid)
    {
        if (script_input.find(uid) == script_input.end())
            return;
        Player* player = get_entity_ptr(uid)->as<Player>();
        if (player == nullptr)
            return;
        // DEBUG("Return input: {:x} -> {:x}", player->input_ptr, script_input[uid]->orig_input);
        player->input_ptr = script_input[uid]->orig_input;
        player->ai_func = script_input[uid]->orig_ai;
        script_input.erase(uid);
    };
    /// Send input
    lua["send_input"] = [this](int uid, BUTTONS buttons)
    {
        if (script_input.find(uid) != script_input.end())
        {
            script_input[uid]->current = buttons;
            script_input[uid]->next = buttons;
        }
    };
    /// Read input
    lua["read_input"] = [this](int uid) -> BUTTONS
    {
        Player* player = get_entity_ptr(uid)->as<Player>();
        if (player == nullptr)
            return (uint16_t)0;
        ScriptInput* readinput = reinterpret_cast<ScriptInput*>(player->input_ptr);
        if (!IsBadReadPtr(readinput, 20))
        {
            return readinput->next;
        }
        return (uint16_t)0;
    };
    /// Read input that has been previously stolen with steal_input
    lua["read_stolen_input"] = [this](int uid)
    {
        if (script_input.find(uid) == script_input.end())
        {
            // this means that the input is attacked to the real input and not stolen so return early
            return (uint16_t)0;
        }
        Player* player = get_entity_ptr(uid)->as<Player>();
        if (player == nullptr)
            return (uint16_t)0;
        ScriptInput* readinput = reinterpret_cast<ScriptInput*>(player->input_ptr);
        if (!IsBadReadPtr(readinput, 20))
        {
            readinput = reinterpret_cast<ScriptInput*>(readinput->orig_input);
            if (!IsBadReadPtr(readinput, 20))
            {
                return readinput->next;
            }
        }
        return (uint16_t)0;
    };

    NSound::register_usertypes(lua, this);
    NLevel::register_usertypes(lua, this);
    NGui::register_usertypes(lua, this);
    NTexture::register_usertypes(lua, this);
    NEntity::register_usertypes(lua, this);
    NParticles::register_usertypes(lua);
    NSaveContext::register_usertypes(lua);
    NState::register_usertypes(lua);
    NPlayer::register_usertypes(lua);
    NDrops::register_usertypes(lua);
    NCharacterState::register_usertypes(lua);
    NEntityFlags::register_usertypes(lua);

    lua.create_named_table(
        "ON",
        "LOGO",
        0,
        "INTRO",
        1,
        "PROLOGUE",
        2,
        "TITLE",
        3,
        "MENU",
        4,
        "OPTIONS",
        5,
        "LEADERBOARD",
        7,
        "SEED_INPUT",
        8,
        "CHARACTER_SELECT",
        9,
        "TEAM_SELECT",
        10,
        "CAMP",
        11,
        "LEVEL",
        12,
        "TRANSITION",
        13,
        "DEATH",
        14,
        "SPACESHIP",
        15,
        "WIN",
        16,
        "CREDITS",
        17,
        "SCORES",
        18,
        "CONSTELLATION",
        19,
        "RECAP",
        20,
        "ARENA_MENU",
        21,
        "ARENA_INTRO",
        25,
        "ARENA_MATCH",
        26,
        "ARENA_SCORE",
        27,
        "ONLINE_LOADING",
        28,
        "ONLINE_LOBBY",
        29,
        "GUIFRAME",
        100,
        "FRAME",
        101,
        "GAMEFRAME",
        108,
        "SCREEN",
        102,
        "START",
        103,
        "LOADING",
        104,
        "RESET",
        105,
        "SAVE",
        106,
        "LOAD",
        107,
        "SCRIPT_ENABLE",
        109,
        "SCRIPT_DISABLE",
        110);
    /* ON
    // GUIFRAME
    // Runs every frame the game is rendered, thus runs at selected framerate. Drawing functions are only available during this callback
    // FRAME
    // Runs while playing the game while the player is controllable, not in the base camp or the arena mode
    // GAMEFRAME
    // Runs whenever the game engine is actively running. This includes base camp, arena, level transition and death screen
    // SCREEN
    // Runs whenever state.screen changes
    // START
    // Runs on the first ON.SCREEN of a run
    // RESET
    // Runs when resetting a run
    // SAVE
    // Params: `SaveContext save_ctx`
    // Runs at the same times as ON.SCREEN, but receives the save_ctx
    // LOAD
    // Params: `LoadContext load_ctx`
    // Runs as soon as your script is loaded, including reloads, then never again
    */

    lua.create_named_table("CONST", "ENGINE_FPS", 60);
    /// After setting the WIN_STATE, the exit door on the current level will lead to the chosen ending
    lua.create_named_table("WIN_STATE", "NO_WIN", 0, "TIAMAT_WIN", 1, "HUNDUN_WIN", 2, "COSMIC_OCEAN_WIN", 3);
}

void ScriptImpl::clear()
{
    std::lock_guard gil_guard{gil};

    // Clear all callbacks on script reload to avoid running them
    // multiple times.
    level_timers.clear();
    global_timers.clear();
    callbacks.clear();
    for (auto id : vanilla_sound_callbacks)
    {
        sound_manager->clear_callback(id);
    }
    vanilla_sound_callbacks.clear();
    options.clear();
    required_scripts.clear();
    lua["on_guiframe"] = sol::lua_nil;
    lua["on_frame"] = sol::lua_nil;
    lua["on_camp"] = sol::lua_nil;
    lua["on_start"] = sol::lua_nil;
    lua["on_level"] = sol::lua_nil;
    lua["on_transition"] = sol::lua_nil;
    lua["on_death"] = sol::lua_nil;
    lua["on_win"] = sol::lua_nil;
    lua["on_screen"] = sol::lua_nil;
    if (meta.unsafe)
    {
        lua["package"]["path"] = meta.path + "/?.lua;" + meta.path + "/?/init.lua";
        lua["package"]["cpath"] = meta.path + "/?.dll;" + meta.path + "/?/init.dll";
        lua.open_libraries(sol::lib::io, sol::lib::os, sol::lib::ffi);
    }
    else
    {
        lua["package"]["path"] = meta.path + "/?.lua;" + meta.path + "/?/init.lua";
        lua["package"]["cpath"] = "";
        lua["package"]["loadlib"] = sol::lua_nil;
    }
}

bool ScriptImpl::reset()
{
    clear();

    // Compile & Evaluate the script if the script is changed
    try
    {
        std::lock_guard gil_guard{gil};
        auto lua_result = lua.safe_script(code);
        result = "OK";
        return true;
    }
    catch (const sol::error& e)
    {
        result = e.what();
#ifdef SPEL2_EXTRA_ANNOYING_SCRIPT_ERRORS
        messages.push_back({result, std::chrono::system_clock::now(), ImVec4(1.0f, 0.2f, 0.2f, 1.0f)});
        DEBUG("{}", result);
        if (messages.size() > 20)
            messages.pop_front();
#endif
        return false;
    }
}

void ScriptImpl::set_enabled(bool enabled)
{
    if (enabled != this->enabled)
    {
        auto cb_type = enabled ? ON::SCRIPT_ENABLE : ON::SCRIPT_DISABLE;
        auto now = get_frame_count();
        for (auto& [id, callback] : callbacks)
        {
            if (callback.screen == cb_type)
            {
                handle_function(callback.func);
                callback.lastRan = now;
            }
        }
    }
    this->enabled = enabled;
}

bool ScriptImpl::run()
{
    if (!enabled)
        return true;
    if (changed)
    {
        result = "";
        changed = false;
        if (!reset())
        {
            return false;
        }
    }
    try
    {
        std::lock_guard gil_guard{gil};

        lua_sethook(lua.lua_state(), NULL, 0, 0);
        lua_sethook(lua.lua_state(), &infinite_loop, LUA_MASKCOUNT, 1000000000);
        sol::optional<std::string> meta_name = lua["meta"]["name"];
        sol::optional<std::string> meta_version = lua["meta"]["version"];
        sol::optional<std::string> meta_description = lua["meta"]["description"];
        sol::optional<std::string> meta_author = lua["meta"]["author"];
        sol::optional<std::string> meta_id = lua["meta"]["id"];
        meta.name = meta_name.value_or(meta.file);
        meta.version = meta_version.value_or("");
        meta.description = meta_description.value_or("");
        meta.author = meta_author.value_or("Anonymous");
        meta.id = script_id();

        /// Runs on every game engine frame.
        sol::optional<sol::function> on_frame = lua["on_frame"];
        /// Runs on entering the camp.
        sol::optional<sol::function> on_camp = lua["on_camp"];
        /// Runs on the start of every level.
        sol::optional<sol::function> on_level = lua["on_level"];
        /// Runs on the start of first level.
        sol::optional<sol::function> on_start = lua["on_start"];
        /// Runs on the start of level transition.
        sol::optional<sol::function> on_transition = lua["on_transition"];
        /// Runs on the death screen.
        sol::optional<sol::function> on_death = lua["on_death"];
        /// Runs on any ending cutscene.
        sol::optional<sol::function> on_win = lua["on_win"];
        /// Runs on any [screen change](#on).
        sol::optional<sol::function> on_screen = lua["on_screen"];
        g_players = get_players();
        lua["players"] = std::vector<Player*>(g_players.begin(), g_players.end());
        if (g_state->screen != state.screen && g_state->screen_last != 5)
        {
            level_timers.clear();
            script_input.clear();
            if (on_screen)
                on_screen.value()();
        }
        if (on_frame && g_state->time_level != state.time_level && g_state->screen == 12)
        {
            on_frame.value()();
        }
        if (g_state->screen == 11 && state.screen != 11)
        {
            if (on_camp)
                on_camp.value()();
        }
        if (g_state->screen == 12 && g_state->screen_last != 5 && !g_players.empty() &&
            (state.player != g_players.at(0) || ((g_state->quest_flags & 1) == 0 && state.reset > 0)))
        {
            if (g_state->level_count == 0)
            {
                if (on_start)
                    on_start.value()();
            }
            if (on_level)
                on_level.value()();
        }
        if (g_state->screen == 13 && state.screen != 13)
        {
            if (on_transition)
                on_transition.value()();
        }
        if (g_state->screen == 14 && state.screen != 14)
        {
            if (on_death)
                on_death.value()();
        }
        if ((g_state->screen == 16 && state.screen != 16) || (g_state->screen == 19 && state.screen != 19))
        {
            if (on_win)
                on_win.value()();
        }

        for (auto id : clear_callbacks)
        {
            level_timers.erase(id);
            global_timers.erase(id);
            callbacks.erase(id);
            load_callbacks.erase(id);

            {
                auto it = std::find_if(pre_level_gen_callbacks.begin(), pre_level_gen_callbacks.end(), [id](auto& cb)
                                       { return cb.id == id; });
                if (it != pre_level_gen_callbacks.end())
                    pre_level_gen_callbacks.erase(it);
            }

            {
                auto it = std::find_if(post_level_gen_callbacks.begin(), post_level_gen_callbacks.end(), [id](auto& cb)
                                       { return cb.id == id; });
                if (it != post_level_gen_callbacks.end())
                    post_level_gen_callbacks.erase(it);
            }
        }
        clear_callbacks.clear();

        for (auto it = global_timers.begin(); it != global_timers.end();)
        {
            auto now = get_frame_count();
            if (auto cb = std::get_if<IntervalCallback>(&it->second))
            {
                if (now >= cb->lastRan + cb->interval)
                {
                    handle_function(cb->func);
                    cb->lastRan = now;
                }
                ++it;
            }
            else if (auto cb = std::get_if<TimeoutCallback>(&it->second))
            {
                if (now >= cb->timeout)
                {
                    handle_function(cb->func);
                    it = global_timers.erase(it);
                }
                else
                {
                    ++it;
                }
            }
            else
            {
                ++it;
            }
        }

        auto now = get_frame_count();
        for (auto& [id, callback] : load_callbacks)
        {
            if (callback.lastRan < 0)
            {
                handle_function(callback.func, LoadContext{meta.path, meta.stem});
                callback.lastRan = now;
            }
            break;
        }

        for (auto& [id, callback] : callbacks)
        {
            if ((ON)g_state->screen == callback.screen && g_state->screen != state.screen && g_state->screen_last != 5) // game screens
            {
                handle_function(callback.func);
                callback.lastRan = now;
            }
            else if (callback.screen == ON::LEVEL && g_state->screen == (int)ON::LEVEL && g_state->screen_last != (int)ON::OPTIONS && !g_players.empty() && (state.player != g_players.at(0) || ((g_state->quest_flags & 1) == 0 && state.reset > 0)))
            {
                handle_function(callback.func);
                callback.lastRan = now;
            }
            else
            {
                switch (callback.screen)
                {
                case ON::FRAME:
                {
                    if (g_state->time_level != state.time_level && g_state->screen == (int)ON::LEVEL)
                    {
                        handle_function(callback.func);
                        callback.lastRan = now;
                    }
                    break;
                }
                case ON::GAMEFRAME:
                {
                    if (!g_state->pause && get_frame_count() != state.time_global &&
                        ((g_state->screen >= (int)ON::CAMP && g_state->screen <= (int)ON::DEATH) || g_state->screen == (int)ON::ARENA_MATCH))
                    {
                        handle_function(callback.func);
                        callback.lastRan = now;
                    }
                    break;
                }
                case ON::SCREEN:
                {
                    if (g_state->screen != state.screen)
                    {
                        handle_function(callback.func);
                        callback.lastRan = now;
                    }
                    break;
                }
                case ON::START:
                {
                    if (g_state->screen == (int)ON::LEVEL && g_state->level_count == 0 && !g_players.empty() &&
                        state.player != g_players.at(0))
                    {
                        handle_function(callback.func);
                        callback.lastRan = now;
                    }
                    break;
                }
                case ON::LOADING:
                {
                    if (g_state->loading > 0 && g_state->loading != state.loading)
                    {
                        handle_function(callback.func);
                        callback.lastRan = now;
                    }
                    break;
                }
                case ON::RESET:
                {
                    if ((g_state->quest_flags & 1) > 0 && (g_state->quest_flags & 1) != state.reset)
                    {
                        handle_function(callback.func);
                        callback.lastRan = now;
                    }
                    break;
                }
                case ON::SAVE:
                {
                    if (g_state->screen != state.screen && g_state->screen != (int)ON::OPTIONS && g_state->screen_last != (int)ON::OPTIONS)
                    {
                        handle_function(callback.func, SaveContext{meta.path, meta.stem});
                        callback.lastRan = now;
                    }
                    break;
                }
                }
            }
        }

        for (auto it = level_timers.begin(); it != level_timers.end();)
        {
            auto now = g_state->time_level;
            if (auto cb = std::get_if<IntervalCallback>(&it->second))
            {
                if (now >= cb->lastRan + cb->interval)
                {
                    handle_function(cb->func);
                    cb->lastRan = now;
                }
                ++it;
            }
            else if (auto cb = std::get_if<TimeoutCallback>(&it->second))
            {
                if (now >= cb->timeout)
                {
                    handle_function(cb->func);
                    it = level_timers.erase(it);
                }
                else
                {
                    ++it;
                }
            }
            else
            {
                ++it;
            }
        }

        if (!g_players.empty())
            state.player = g_players.at(0);
        else
            state.player = nullptr;
        state.screen = g_state->screen;
        state.time_level = g_state->time_level;
        state.time_total = g_state->time_total;
        state.time_global = get_frame_count();
        state.frame = get_frame_count();
        state.loading = g_state->loading;
        state.reset = (g_state->quest_flags & 1);
        state.quest_flags = g_state->quest_flags;
    }
    catch (const sol::error& e)
    {
        result = e.what();
        return false;
    }
    return true;
}

void ScriptImpl::draw(ImDrawList* dl)
{
    if (!enabled)
        return;
    draw_list = dl;
    try
    {
        std::lock_guard gil_guard{gil};

        /// Runs on every screen frame. You need this to use draw functions.
        sol::optional<sol::function> on_guiframe = lua["on_guiframe"];

        if (on_guiframe)
        {
            on_guiframe.value()();
        }

        for (auto& [id, callback] : callbacks)
        {
            auto now = get_frame_count();
            if (callback.screen == ON::GUIFRAME)
            {
                handle_function(callback.func);
                callback.lastRan = now;
            }
        }
    }
    catch (const sol::error& e)
    {
        result = e.what();
    }
    draw_list = nullptr;
}

std::string ScriptImpl::script_id()
{
    std::string newid = sanitize(meta.author) + "/" + sanitize(meta.name);
    return newid;
}

void ScriptImpl::render_options()
{
    ImGui::PushID(meta.id.data());
    for (auto& name_option_pair : options)
    {
        std::visit(
            overloaded{
                [&](IntOption& option)
                {
                    if (ImGui::DragInt(name_option_pair.second.desc.c_str(), &option.value, 0.5f, option.min, option.max))
                    {
                        auto& name = name_option_pair.first;
                        lua["options"][name] = option.value;
                    }
                },
                [&](FloatOption& option)
                {
                    if (ImGui::DragFloat(name_option_pair.second.desc.c_str(), &option.value, 0.5f, option.min, option.max))
                    {
                        auto& name = name_option_pair.first;
                        lua["options"][name] = option.value;
                    }
                },
                [&](BoolOption& option)
                {
                    if (ImGui::Checkbox(name_option_pair.second.desc.c_str(), &option.value))
                    {
                        auto& name = name_option_pair.first;
                        lua["options"][name] = option.value;
                    }
                },
                [&](StringOption& option)
                {
                    if (InputString(name_option_pair.second.desc.c_str(), &option.value, 0, nullptr, nullptr))
                    {
                        auto& name = name_option_pair.first;
                        lua["options"][name] = option.value;
                    }
                },
                [&](ComboOption& option)
                {
                    if (ImGui::Combo(name_option_pair.second.desc.c_str(), &option.value, option.options.c_str()))
                    {
                        auto& name = name_option_pair.first;
                        lua["options"][name] = option.value + 1;
                    }
                },
                [&](ButtonOption& option)
                {
                    if (ImGui::Button(name_option_pair.second.desc.c_str()))
                    {
                        uint64_t now =
                            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                        auto& name = name_option_pair.first;
                        lua["options"][name] = now;
                        handle_function(option.on_click);
                    }
                },
            },
            name_option_pair.second.option_impl);
        if (!name_option_pair.second.long_desc.empty())
        {
            ImGui::TextWrapped("%s", name_option_pair.second.long_desc.c_str());
        }
    }
    ImGui::PopID();
}

bool ScriptImpl::pre_level_gen_spawn(std::string_view tile_code, float x, float y, int layer)
{
    if (!enabled)
        return false;

    for (auto& callback : pre_level_gen_callbacks)
    {
        if (callback.tile_code == tile_code)
        {
            if (handle_function_with_return<bool>(callback.func, x, y, layer).value_or(false))
            {
                return true;
            }
        }
    }
    return false;
}
void ScriptImpl::post_level_gen_spawn(std::string_view tile_code, float x, float y, int layer)
{
    if (!enabled)
        return;

    for (auto& callback : post_level_gen_callbacks)
    {
        if (callback.tile_code == tile_code)
        {
            handle_function(callback.func, x, y, layer);
        }
    }
}

std::string ScriptImpl::dump_api()
{
    std::set<std::string> excluded_keys{"meta"};

    sol::state dummy_state;
    dummy_state.open_libraries(sol::lib::math, sol::lib::base, sol::lib::string, sol::lib::table, sol::lib::coroutine, sol::lib::package, sol::lib::debug);

    for (auto& [key, value] : lua["_G"].get<sol::table>())
    {
        std::string key_str = key.as<std::string>();
        if (key_str.starts_with("sol."))
        {
            excluded_keys.insert(std::move(key_str));
        }
    }

    for (auto& [key, value] : dummy_state["_G"].get<sol::table>())
    {
        std::string key_str = key.as<std::string>();
        excluded_keys.insert(std::move(key_str));
    }

    require_serpent_lua(dummy_state);
    sol::table opts = dummy_state.create_table();
    opts["comment"] = false;
    sol::function serpent = dummy_state["serpent"]["block"];

    std::map<std::string, std::string> sorted_output;
    for (auto& [key, value] : lua["_G"].get<sol::table>())
    {
        std::string key_str = key.as<std::string>();
        if (!excluded_keys.contains(key_str))
        {
            std::string value_str = serpent(value, opts).get<std::string>();
            if (value_str.starts_with("\"function"))
            {
                value_str = "function(...) end";
            }
            else if (value_str.starts_with("\"userdata"))
            {
                value_str = {};
            }
            sorted_output[std::move(key_str)] = std::move(value_str);
        }
    }

    std::string api;
    for (auto& [key, value] : sorted_output)
        api += fmt::format("{} = {}\n", key, value);
    return api;
}
