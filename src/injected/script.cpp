#include "script.hpp"
#include "entity.hpp"
#include "logger.h"
#include "rpc.hpp"
#include "state.hpp"

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

using Toast = void (*)(void *, wchar_t *);
Toast get_toast()
{
    ONCE(Toast)
    {
        auto memory = Memory::get();
        auto off = find_inst(memory.exe(), "\x49\x8B\x0C\x3F\xB8\x60\x01\x00\x00"s, memory.after_bundle);
        off = function_start(memory.at_exe(off));
        return res = (Toast)off;
    }
}

using Say = void (*)(void *, Entity *, wchar_t *, int unk_type /* 0, 2, 3 */, bool top /* top or bottom */);
Say get_say()
{
    ONCE(Say)
    {
        auto memory = Memory::get();
        auto off = find_inst(memory.exe(), "\x4A\x8B\x0C\x2F\xB8\x60\x01\x00\x00"s, memory.after_bundle);
        off = function_start(memory.at_exe(off));
        return res = (Say)off;
    }
}

Script::Script(std::string script, std::string file)
{
    strcpy(code, script.data());
    meta.file = file;

    g_state = (struct StateMemory *)get_state_ptr();
    g_items = list_entities();
    g_players = get_players();

    lua.open_libraries(sol::lib::math, sol::lib::base, sol::lib::string, sol::lib::table);

    /// A bunch of [game state](#statememory) variables
    /// Example:
    /// ```
    /// if state.time_level > 300 and state.theme == THEME.DWELLING then
    ///     toast("Congratilations for lasting 10 seconds in Dwelling")
    /// end
    /// ```
    lua["state"] = g_state;
    /// An array of [Movables](#movable) of the current players. Pro tip: You need `players[1].uid` in most entity functions.
    lua["players"] = std::vector<Movable *>(g_players.begin(), g_players.end());
    /// Print a log message on screen.
    lua["message"] = [this](std::string message) {
        messages.push_back({message, std::chrono::system_clock::now()});
        if (messages.size() > 20)
            messages.pop_front();
    };
    /// Returns: `int` unique id for the callback to be used in [clear_callback](#clear_callback).
    /// Add per level callback function to be called every `frames` game frames. Timer is paused on pause and cleared on level transition.
    lua["set_interval"] = [this](sol::function cb, int frames) {
        auto luaCb = IntervalCallback{cb, frames, -1};
        level_timers[cbcount] = luaCb;
        return cbcount++;
    };
    /// Returns: `int` unique id for the callback to be used in [clear_callback](#clear_callback).
    /// Add per level callback function to be called after `frames` frames. Timer is paused on pause and cleared on level transition.
    lua["set_timeout"] = [this](sol::function cb, int frames) {
        int now = g_state->time_level;
        auto luaCb = TimeoutCallback{cb, now + frames};
        level_timers[cbcount] = luaCb;
        return cbcount++;
    };
    /// Returns: `int` unique id for the callback to be used in [clear_callback](#clear_callback).
    /// Add global callback function to be called every `frames` frames. This timer is never paused or cleared.
    lua["set_global_interval"] = [this](sol::function cb, int frames) {
        auto luaCb = IntervalCallback{cb, frames, -1};
        global_timers[cbcount] = luaCb;
        return cbcount++;
    };
    /// Returns: `int` unique id for the callback to be used in [clear_callback](#clear_callback).
    /// Add global callback function to be called after `frames` frames. This timer is never paused or cleared.
    lua["set_global_timeout"] = [this](sol::function cb, int frames) {
        int now = get_frame_count();
        auto luaCb = TimeoutCallback{cb, now + frames};
        global_timers[cbcount] = luaCb;
        return cbcount++;
    };
    /// Returns: `int` unique id for the callback to be used in [clear_callback](#clear_callback).
    /// Add global callback function to be called on an [event](#on).
    lua["set_callback"] = [this](sol::function cb, int screen) {
        auto luaCb = ScreenCallback{cb, screen, -1};
        callbacks[cbcount] = luaCb;
        return cbcount++;
    };
    /// Clear previously added callback `id`
    lua["clear_callback"] = [this](int id) { clear_callbacks.push_back(id); };
    /// Table of strings where you should set some script metadata shown in the UI.
    /// - `meta.name` Script name
    /// - `meta.version` Version
    /// - `meta.description` Short description of the script
    /// - `meta.author` Your name
    lua["meta"] = lua.create_named_table("meta");
    /// Table of options set in the UI, added with the [register_option_functions](#register_option_int).
    lua["options"] = lua.create_named_table("options");
    /// Show a message that looks like a level feeling.
    lua["toast"] = [this](std::wstring message) {
        auto toast = get_toast();
        toast(NULL, message.data());
    };
    /// Show a message coming from an entity
    lua["say"] = [this](uint32_t entity_id, std::wstring message, int unk_type, bool top) {
        auto say = get_say();
        auto entity = get_entity_ptr(entity_id);
        if (entity == nullptr)
            return;
        say(NULL, entity, message.data(), unk_type, top);
    };
    /// Add an integer option that the user can change in the UI. Read with `options.name`, `value` is the default.
    lua["register_option_int"] = [this](std::string name, std::string desc, int value, int min, int max) {
        options[name] = {desc, value, min, max};
        lua["options"][name] = value;
    };
    /// Add a boolean option that the user can change in the UI. Read with `options.name`, `value` is the default.
    lua["register_option_bool"] = [this](std::string name, std::string desc, bool value) {
        options[name] = {desc, value, 0, 0};
        lua["options"][name] = value;
    };
    /// Spawn an entity in position with some velocity and return the uid of spawned entity.
    lua["spawn_entity"] = spawn_entity_abs;
    /// Short for [spawn_entity](#spawn_entity).
    lua["spawn"] = spawn_entity_abs;
    /// Spawn a door to another world, level and theme and return the uid of spawned entity.
    lua["spawn_door"] = spawn_door_abs;
    /// Short for [spawn_door](#spawn_door).
    lua["door"] = spawn_door_abs;
    /// Spawn a door to backlayer
    lua["spawn_layer_door"] = spawn_backdoor_abs;
    /// Short for [spawn_layer_door](#spawn_layer_door).
    lua["layer_door"] = spawn_backdoor_abs;
    /// Enable/disable godmode
    lua["god"] = godmode;
    /// Try to force next levels to be dark
    lua["force_dark_level"] = darkmode;
    /// Set the zoom level used in levels and shops. 13.5 is the default.
    lua["zoom"] = zoom;
    /// Enable/disable game engine pause
    lua["pause"] = [this](bool p) {
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
    /// Set the contents of ENT_TYPE.ITEM_POT, ENT_TYPE.ITEM_CRATE or ENT_TYPE.ITEM_COFFIN `id` to ENT_TYPE... `item`
    lua["set_contents"] = set_contents;
    /// Get the [Movable](#movable) entity behind an uid
    lua["get_entity"] = get_entity;
    /// Get the [EntityDB](#entitydb) behind an uid. This is kinda read only, the changes don't really show up in game. Use the `type` field in
    /// [Movable](#movable) to actually edit these.
    lua["get_type"] = get_type;
    /// Get uids of all entities currently loaded
    lua["get_entities"] = get_entities;
    /// Get uids of entities by some conditions. Set `type` or `mask` to `0` to ignore that.
    lua["get_entities_by"] = get_entities_by;
    /// Returns: `array<int>`
    /// Get uids of entities matching id. This function is variadic, meaning it accepts any number of id's.
    /// You can even pass a table! Example:
    /// ```
    /// types = {ENT_TYPE.MONS_SNAKE, ENT_TYPE.MONS_BAT}
    /// function on_level()
    ///     uids = get_entities_by_type(ENT_TYPE.MONS_SNAKE, ENT_TYPE.MONS_BAT)
    ///     -- is not the same thing as this, but also works
    ///     uids2 = get_entities_by_type(types)
    ///     message(tostring(#uids).." == "..tostring(#uids2))
    /// end
    /// ```
    lua["get_entities_by_type"] = [](sol::variadic_args va) {
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
    /// Get uids of matching entities inside some radius. Set `type` or `mask` to `0` to ignore that.
    lua["get_entities_at"] = get_entities_at;
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
    /// Get `state.flags`
    lua["get_hud_flags"] = get_hud_flags;
    /// Set `state.flags`
    lua["set_hud_flags"] = set_hud_flags;
    /// Get the ENT_TYPE... for entity by uid
    lua["get_entity_type"] = get_entity_type;
    /// Get the current set zoom level
    lua["get_zoom_level"] = get_zoom_level;
    /// Translate an entity position to screen position to be used in drawing functions
    lua["screen_position"] = screen_position;
    /// Translate a distance of `x` tiles to screen distance to be be used in drawing functions
    lua["screen_distance"] = screen_distance;
    /// Get position `x, y, layer` of entity by uid. Use this, don't use `Movable.x/y` because those are sometimes just the offset to the entity
    /// you're standing on.
    lua["get_position"] = get_position;
    /// Remove item by uid from entity
    lua["entity_remove_item"] = entity_remove_item;
    /// Spawn an entity by `id` attached to some other entity `over`, in offset `x`, `y`
    lua["spawn_entity_over"] = spawn_entity_over;
    /// Check if the entity `id` has some specific `item` by uid in their inventory
    lua["entity_has_item_uid"] = entity_has_item_uid;
    /// Check if the entity `id` has some ENT_TYPE `type` in their inventory
    lua["entity_has_item_type"] = entity_has_item_type;
    /// Kills an entity by uid.
    lua["kill_entity"] = kill_entity;
    /// Apply changes made in [get_type](#get_type)() to entity instance by uid.
    lua["apply_entity_db"] = apply_entity_db;
    /// Try to lock the exit at coordinates
    lua["lock_door_at"] = lock_door_at;
    /// Try to unlock the exit at coordinates
    lua["unlock_door_at"] = unlock_door_at;
    /// Get the current global frame count since the game was started. You can use this to make some timers yourself, the engine runs at 60fps.
    lua["get_frame"] = get_frame_count;
    /// Make `mount` carry `rider` on their back. Only use this with actual mounts and living things.
    lua["carry"] = carry;
    /// Calculate the tile distance of two entities by uid
    lua["distance"] = [this](uint32_t a, uint32_t b) {
        Entity *ea = get_entity_ptr(a);
        Entity *eb = get_entity_ptr(b);
        if (ea == nullptr || eb == nullptr)
            return -1.0f;
        else
            return (float)sqrt(pow(ea->position().first - eb->position().first, 2) + pow(ea->position().second - eb->position().second, 2));
    };
    /// Set a bit in a number. This doesn't actually change the bit in the entity you pass it, it just returns the new value you can use.
    lua["set_flag"] = [](uint32_t flags, int bit) { return flags | (1U << (bit - 1)); };
    lua["setflag"] = lua["set_flag"];
    /// Clears a bit in a number. This doesn't actually change the bit in the entity you pass it, it just returns the new value you can use.
    lua["clr_flag"] = [](uint32_t flags, int bit) { return flags & ~(1U << (bit - 1)); };
    lua["clrflag"] = lua["clr_flag"];
    /// Returns true if a bit is set in the flags
    lua["test_flag"] = [](uint32_t flags, int bit) { return (flags & (1U << (bit - 1))) > 0; };
    lua["testflag"] = lua["test_flag"];

    /// Converts a color to int to be used in drawing functions. Use values from `0..255`.
    lua["rgba"] = [](int r, int g, int b, int a) { return (unsigned int)(a << 24) + (b << 16) + (g << 8) + (r); };
    /// Draws a line on screen
    lua["draw_line"] = [this](float x1, float y1, float x2, float y2, float thickness, ImU32 color) {
        ImVec2 a = screenify({x1, y1});
        ImVec2 b = screenify({x2, y2});
        drawlist->AddLine(a, b, color, thickness);
    };
    /// Draws rectangle on screen from top-left to bottom-right.
    lua["draw_rect"] = [this](float x1, float y1, float x2, float y2, float thickness, float rounding, ImU32 color) {
        ImVec2 a = screenify({x1, y1});
        ImVec2 b = screenify({x2, y2});
        drawlist->AddRect(a, b, color, rounding, 15, thickness);
    };
    /// Draws a circle on screen
    lua["draw_circle"] = [this](float x, float y, float radius, float thickness, ImU32 color) {
        ImVec2 a = screenify({x, y});
        drawlist->AddCircle(a, screenify(radius), color, 0, thickness);
    };
    /// Draws text on screen
    lua["draw_text"] = [this](float x, float y, std::string text, ImU32 color) {
        ImVec2 a = screenify({x, y});
        drawlist->AddText(a, color, text.data());
    };

    lua.new_usertype<Color>("Color", "r", &Color::r, "g", &Color::g, "b", &Color::b, "a", &Color::a);
    lua.new_usertype<Inventory>(
        "Inventory",
        "money",
        &Inventory::money,
        "bombs",
        &Inventory::bombs,
        "ropes",
        &Inventory::ropes,
        "kills_level",
        &Inventory::kills_level,
        "kills_total",
        &Inventory::kills_total);
    lua.new_usertype<EntityDB>(
        "EntityDB",
        "id",
        &EntityDB::id,
        "search_flags",
        &EntityDB::search_flags,
        "width",
        &EntityDB::width,
        "height",
        &EntityDB::height,
        "friction",
        &EntityDB::friction,
        "elasticity",
        &EntityDB::elasticity,
        "weight",
        &EntityDB::weight,
        "acceleration",
        &EntityDB::acceleration,
        "max_speed",
        &EntityDB::max_speed,
        "sprint_factor",
        &EntityDB::sprint_factor,
        "jump",
        &EntityDB::jump,
        "damage",
        &EntityDB::damage,
        "life",
        &EntityDB::life);
    lua.new_usertype<Movable>(
        "Movable",
        "type",
        &Entity::type,
        "overlay",
        &Entity::overlay,
        "flags",
        &Entity::flags,
        "more_flags",
        &Entity::more_flags,
        "uid",
        &Entity::uid,
        "animation",
        &Entity::animation,
        "x",
        &Entity::x,
        "y",
        &Entity::y,
        "width",
        &Entity::w,
        "height",
        &Entity::h,
        "topmost",
        &Entity::topmost,
        "topmost_mount",
        &Entity::topmost_mount,
        "movex",
        &Movable::movex,
        "movey",
        &Movable::movey,
        "buttons",
        &Movable::buttons,
        "stand_counter",
        &Movable::stand_counter,
        "owner_uid",
        &Movable::owner_uid,
        "last_owner_uid",
        &Movable::last_owner_uid,
        "idle_counter",
        &Movable::idle_counter,
        "standing_on_uid",
        &Movable::standing_on_uid,
        "velocityx",
        &Movable::velocityx,
        "velocityy",
        &Movable::velocityy,
        "holding_uid",
        &Movable::holding_uid,
        "state",
        &Movable::state,
        "last_state",
        &Movable::last_state,
        "move_state",
        &Movable::move_state,
        "health",
        &Movable::health,
        "some_state",
        &Movable::some_state,
        "color",
        &Movable::color,
        "hitboxx",
        &Movable::hitboxx,
        "hitboxy",
        &Movable::hitboxy,
        "offsetx",
        &Movable::offsetx,
        "offsety",
        &Movable::offsety,
        "airtime",
        &Movable::airtime);
    lua.new_usertype<Player>("Player", "inventory", &Player::inventory_ptr, sol::base_classes, sol::bases<Movable>());
    lua.new_usertype<Container>("Container", "inside", &Container::inside, sol::base_classes, sol::bases<Movable>());
    lua.new_usertype<StateMemory>(
        "StateMemory",
        "screen_last",
        &StateMemory::screen_last,
        "screen",
        &StateMemory::screen,
        "screen_next",
        &StateMemory::screen_next,
        "ingame",
        &StateMemory::ingame,
        "playing",
        &StateMemory::playing,
        "pause",
        &StateMemory::pause,
        "width",
        &StateMemory::w,
        "height",
        &StateMemory::h,
        "kali_favor",
        &StateMemory::kali_favor,
        "kali_status",
        &StateMemory::kali_status,
        "kali_altars_destroyed",
        &StateMemory::kali_altars_destroyed,
        "time_total",
        &StateMemory::time_total,
        "world",
        &StateMemory::world,
        "world_next",
        &StateMemory::world_next,
        "level",
        &StateMemory::level,
        "level_next",
        &StateMemory::level_next,
        "theme",
        &StateMemory::theme,
        "theme_next",
        &StateMemory::theme_next,
        "shoppie_aggro",
        &StateMemory::shoppie_aggro,
        "shoppie_aggro_next",
        &StateMemory::shoppie_aggro_levels,
        "merchant_aggro",
        &StateMemory::merchant_aggro,
        "kills_npc",
        &StateMemory::kills_npc,
        "level_count",
        &StateMemory::level_count,
        "journal_flags",
        &StateMemory::journal_flags,
        "time_last_level",
        &StateMemory::time_last_level,
        "time_level",
        &StateMemory::time_level,
        "hud_flags",
        &StateMemory::hud_flags);
    lua.create_named_table("ENT_TYPE");
    for (int i = 0; i < g_items.size(); i++)
    {
        auto name = g_items[i].name.substr(9, g_items[i].name.size());
        lua["ENT_TYPE"][name] = g_items[i].id;
    }
    lua.new_enum(
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
        17);
    lua.new_enum(
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
        "GUIFRAME",
        100,
        "FRAME",
        101,
        "SCREEN",
        102,
        "START",
        103);
    lua.new_enum("LAYER", "FRONT", 0, "BACK", 1, "CURRENT", -1);
}

bool Script::run(ImDrawList *dl)
{
    if (!enabled)
        return true;
    if (changed)
    {
        changed = false;
        // Compile & Evaluate the script if the script is changed
        try
        {
            // Clear all callbacks on script reload to avoid running them
            // multiple times.
            level_timers.clear();
            global_timers.clear();
            callbacks.clear();
            options.clear();
            lua["on_guiframe"] = sol::lua_nil;
            lua["on_frame"] = sol::lua_nil;
            lua["on_camp"] = sol::lua_nil;
            lua["on_start"] = sol::lua_nil;
            lua["on_level"] = sol::lua_nil;
            lua["on_transition"] = sol::lua_nil;
            lua["on_death"] = sol::lua_nil;
            lua["on_win"] = sol::lua_nil;
            lua["on_screen"] = sol::lua_nil;
            auto lua_result = lua.safe_script(code);
            result = "OK";
        }
        catch (const sol::error &e)
        {
            result = e.what();
            return false;
        }
    }
    try
    {
        drawlist = dl;
        sol::optional<std::string> meta_name = lua["meta"]["name"];
        sol::optional<std::string> meta_version = lua["meta"]["version"];
        sol::optional<std::string> meta_description = lua["meta"]["description"];
        sol::optional<std::string> meta_author = lua["meta"]["author"];
        meta.name = meta_name.value_or(meta.file);
        meta.version = meta_version.value_or("");
        meta.description = meta_description.value_or("");
        meta.author = meta_author.value_or("Anonymous");

        /// Runs on every screen frame. You need this to use draw functions.
        sol::optional<sol::function> on_guiframe = lua["on_guiframe"];
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
        lua["players"] = std::vector<Player *>(g_players.begin(), g_players.end());
        if (g_state->screen != state.screen || (!g_players.empty() && state.player != g_players.at(0)))
        {
            level_timers.clear();
            if (on_screen)
                on_screen.value()();
        }
        if (on_guiframe)
        {
            on_guiframe.value()();
        }
        if (on_frame && g_state->time_level != state.time_level)
        {
            on_frame.value()();
        }
        if (g_state->screen == 11 && state.screen != 11)
        {
            if (on_camp)
                on_camp.value()();
        }
        if (g_state->screen == 12 && !g_players.empty() && state.player != g_players.at(0))
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
            auto it = level_timers.find(id);
            if (it != level_timers.end())
                level_timers.erase(id);

            auto it2 = global_timers.find(id);
            if (it2 != global_timers.end())
                global_timers.erase(id);

            auto it3 = callbacks.find(id);
            if (it3 != callbacks.end())
                callbacks.erase(id);
        }
        clear_callbacks.clear();

        for (auto it = global_timers.begin(); it != global_timers.end();)
        {
            auto now = get_frame_count();
            if (auto cb = std::get_if<IntervalCallback>(&it->second))
            {
                if (now >= cb->lastRan + cb->interval)
                {
                    cb->func();
                    cb->lastRan = now;
                }
                ++it;
            }
            else if (auto cb = std::get_if<TimeoutCallback>(&it->second))
            {
                if (now >= cb->timeout)
                {
                    cb->func();
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

        for (auto it = callbacks.begin(); it != callbacks.end();)
        {
            auto now = get_frame_count();
            if (auto cb = std::get_if<ScreenCallback>(&it->second))
            {
                if (g_state->screen == cb->screen && g_state->screen != state.screen) // game screens
                {
                    cb->func();
                    cb->lastRan = now;
                }
                else if (cb->screen == 12 && g_state->screen == 12 && !g_players.empty() && state.player != g_players.at(0)) // run ON.LEVEL on
                                                                                                                             // instant restart too
                {
                    cb->func();
                    cb->lastRan = now;
                }
                else if (cb->screen == 100) // ON.GUIFRAME
                {
                    cb->func();
                    cb->lastRan = now;
                }
                else if (cb->screen == 101 && g_state->time_level != state.time_level) // ON.FRAME
                {
                    cb->func();
                    cb->lastRan = now;
                }
                else if (cb->screen == 102 && g_state->screen != state.screen) // ON.SCREEN
                {
                    cb->func();
                    cb->lastRan = now;
                }
                else if (
                    cb->screen == 103 && g_state->screen == 12 && g_state->level_count == 0 && !g_players.empty() &&
                    state.player != g_players.at(0)) // ON.START
                {
                    cb->func();
                    cb->lastRan = now;
                }
                ++it;
            }
            else
            {
                ++it;
            }
        }

        for (auto it = level_timers.begin(); it != level_timers.end();)
        {
            auto now = g_state->time_level;
            if (auto cb = std::get_if<IntervalCallback>(&it->second))
            {
                if (now >= cb->lastRan + cb->interval)
                {
                    cb->func();
                    cb->lastRan = now;
                }
                ++it;
            }
            else if (auto cb = std::get_if<TimeoutCallback>(&it->second))
            {
                if (now >= cb->timeout)
                {
                    cb->func();
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
        state.screen = g_state->screen;
        state.time_level = g_state->time_level;
        state.time_total = g_state->time_total;
        state.frame = get_frame_count();
    }
    catch (const sol::error &e)
    {
        result = e.what();
        return false;
    }
    return true;
}

Movable *get_entity(uint32_t id)
{
    return (Movable *)get_entity_ptr(id);
}

std::tuple<float, float, int> get_position(uint32_t id)
{
    Entity *ent = get_entity_ptr(id);
    if (ent)
        return std::make_tuple(ent->position().first, ent->position().second, ent->layer());
    return {0.0f, 0.0f, 0};
}

float screenify(float dis)
{
    ImGuiIO &io = ImGui::GetIO();
    ImVec2 res = io.DisplaySize;
    return dis / (1.0 / (res.x / 2));
}

ImVec2 screenify(ImVec2 pos)
{
    ImGuiIO &io = ImGui::GetIO();
    ImVec2 res = io.DisplaySize;
    ImVec2 bar = {0.0, 0.0};
    if (res.x / res.y > 1.78)
    {
        bar.x = (res.x - res.y / 9 * 16) / 2;
        res.x = res.y / 9 * 16;
    }
    else if (res.x / res.y < 1.77)
    {
        bar.y = (res.y - res.x / 16 * 9) / 2;
        res.y = res.x / 16 * 9;
    }
    ImVec2 screened = ImVec2(pos.x / (1.0 / (res.x / 2)) + res.x / 2 + bar.x, res.y - (pos.y / (1.0 / (res.y / 2)) + res.y / 2 + bar.y));
    return screened;
}
