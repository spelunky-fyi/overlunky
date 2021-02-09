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
Say get_say() {
    ONCE(Say) {
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

    lua["players"] = std::vector<Movable *>(g_players.begin(), g_players.end());
    lua["message"] = [this](std::string message) {
        messages.push_back({message, std::chrono::system_clock::now()});
        if(messages.size() > 20)
            messages.pop_front();
    };
    lua["set_interval"] = [this](sol::function cb, int frames)
    {
        auto luaCb = IntervalCallback{cb, frames, -1};
        level_timers[cbcount] = luaCb;
        return cbcount++;
    };
    lua["set_timeout"] = [this](sol::function cb, int frames)
    {
        int now = g_state->time_level;
        auto luaCb = TimeoutCallback{cb, now + frames};
        level_timers[cbcount] = luaCb;
        return cbcount++;
    };
    lua["set_global_interval"] = [this](sol::function cb, int frames)
    {
        auto luaCb = IntervalCallback{cb, frames, -1};
        global_timers[cbcount] = luaCb;
        return cbcount++;
    };
    lua["set_global_timeout"] = [this](sol::function cb, int frames)
    {
        int now = get_frame_count();
        auto luaCb = TimeoutCallback{cb, now + frames};
        global_timers[cbcount] = luaCb;
        return cbcount++;
    };
    lua["set_callback"] = [this](sol::function cb, int screen) {
        auto luaCb = ScreenCallback{cb, screen, -1};
        callbacks[cbcount] = luaCb;
        return cbcount++;
    };
    lua["clear_callback"] = [this](int id) { clear_callbacks.push_back(id); };
    lua["meta"] = lua.create_named_table("meta");
    lua["options"] = lua.create_named_table("options");
    lua["toast"] = [this](std::wstring message) {
        auto toast = get_toast();
        toast(NULL, message.data());
    };
    lua["say"] = [this](uint32_t entity_id, std::wstring message, int unk_type, bool top) {
        auto say = get_say();
        auto entity = get_entity_ptr(entity_id);
        if(entity == nullptr)
            return;
        say(NULL, entity, message.data(), unk_type, top);
    };
    lua["register_option_int"] = [this](std::string name, std::string desc, int value, int min, int max) {
        options[name] = {desc, value, min, max};
        lua["options"][name] = value;
    };
    lua["register_option_bool"] = [this](std::string name, std::string desc, bool value) {
        options[name] = {desc, value, 0, 0};
        lua["options"][name] = value;
    };
    lua["spawn_entity"] = spawn_entity_abs;
    lua["spawn"] = spawn_entity_abs;
    lua["spawn_door"] = spawn_door_abs;
    lua["door"] = spawn_door_abs;
    lua["spawn_layer_door"] = spawn_backdoor_abs;
    lua["layer_door"] = spawn_backdoor_abs;
    lua["god"] = godmode;
    lua["force_dark_level"] = darkmode;
    lua["zoom"] = zoom;
    lua["pause"] = [this](bool p) {
        if (p)
            set_pause(0x20);
        else
            set_pause(0);
    };
    lua["move_entity"] = move_entity_abs;
    lua["set_door_target"] = set_door_target;
    lua["set_door"] = set_door_target;
    lua["set_contents"] = set_contents;
    lua["get_entity"] = get_entity;
    lua["get_type"] = get_type;
    lua["get_entities"] = get_entities;
    lua["get_entities_by"] = get_entities_by;
    lua["get_entities_by_type"] = [](sol::variadic_args va) {
        auto get_func = sol::resolve<std::vector<uint32_t>(std::vector<uint32_t>)>(get_entities_by_type);
        auto args = std::vector<uint32_t>(va.begin(), va.end());
        return get_func(args);
    };
    lua["get_entities_by_mask"] = get_entities_by_mask;
    lua["get_entities_by_layer"] = get_entities_by_layer;
    lua["get_entities_at"] = get_entities_at;
    lua["get_entity_flags"] = get_entity_flags;
    lua["set_entity_flags"] = set_entity_flags;
    lua["get_entity_flags2"] = get_entity_flags2;
    lua["set_entity_flags2"] = set_entity_flags2;
    lua["get_entity_ai_state"] = get_entity_ai_state;
    lua["get_hud_flags"] = get_hud_flags;
    lua["set_hud_flags"] = set_hud_flags;
    lua["get_entity_type"] = get_entity_type;
    lua["get_zoom_level"] = get_zoom_level;
    lua["screen_position"] = screen_position;
    lua["get_position"] = get_position;
    lua["entity_remove_item"] = entity_remove_item;
    lua["spawn_entity_over"] = spawn_entity_over;
    lua["entity_has_item_uid"] = entity_has_item_uid;
    lua["entity_has_item_type"] = entity_has_item_type;
    lua["lock_door_at"] = lock_door_at;
    lua["unlock_door_at"] = unlock_door_at;
    lua["get_frame"] = get_frame_count;
    lua["carry"] = carry;
    lua.new_usertype<Color>(
        "Color",
        "r",
        &Color::r,
        "g",
        &Color::g,
        "b",
        &Color::b,
        "a",
        &Color::a);
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
        "inside",
        &Movable::inside,
        "has_backpack",
        &Movable::has_backpack,
        "inventory",
        &Movable::inventory_ptr,
        "color",
        &Movable::color,
        "hitboxx",
        &Movable::hitboxx,
        "hitboxy",
        &Movable::hitboxy,
        "offsetx",
        &Movable::offsetx,
        "offsety",
        &Movable::offsety);
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
    lua["state"] = g_state;
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
}

bool Script::run()
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
        sol::optional<std::string> meta_name = lua["meta"]["name"];
        sol::optional<std::string> meta_version = lua["meta"]["version"];
        sol::optional<std::string> meta_description = lua["meta"]["description"];
        sol::optional<std::string> meta_author = lua["meta"]["author"];
        meta.name = meta_name.value_or(meta.file);
        meta.version = meta_version.value_or("");
        meta.description = meta_description.value_or("");
        meta.author = meta_author.value_or("Anonymous");

        sol::optional<sol::function> on_guiframe = lua["on_guiframe"];
        sol::optional<sol::function> on_frame = lua["on_frame"];
        sol::optional<sol::function> on_camp = lua["on_camp"];
        sol::optional<sol::function> on_level = lua["on_level"];
        sol::optional<sol::function> on_start = lua["on_start"];
        sol::optional<sol::function> on_transition = lua["on_transition"];
        sol::optional<sol::function> on_death = lua["on_death"];
        sol::optional<sol::function> on_win = lua["on_win"];
        sol::optional<sol::function> on_screen = lua["on_screen"];
        g_players = get_players();
        lua["players"] = std::vector<Movable *>(g_players.begin(), g_players.end());
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
        if (on_frame && get_frame_count() != state.frame)
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
            if(g_state->level_count == 0)
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
                else if (cb->screen == 12 && g_state->screen == 12 && !g_players.empty() && state.player != g_players.at(0)) // run ON.LEVEL on instant restart too
                {
                    cb->func();
                    cb->lastRan = now;
                }
                else if (cb->screen == 100) // ON.GUIFRAME
                {
                    cb->func();
                    cb->lastRan = now;
                }
                else if (cb->screen == 101 && get_frame_count() != state.frame) // ON.FRAME
                {
                    cb->func();
                    cb->lastRan = now;
                }
                else if (cb->screen == 102 && g_state->screen != state.screen) // ON.SCREEN
                {
                    cb->func();
                    cb->lastRan = now;
                }
                else if (cb->screen == 103 && g_state->screen == 12 && g_state->level_count == 0 && !g_players.empty() && state.player != g_players.at(0)) // ON.START
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
