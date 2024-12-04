#include "lua_vm.hpp"

#include <Windows.h>     // for IsBadReadPtr, Get...
#include <algorithm>     // for max, transform
#include <cctype>        // for toupper
#include <chrono>        // for milliseconds, sys...
#include <cmath>         // for pow, sqrt
#include <compare>       // for operator<
#include <csignal>       // for raise
#include <cstddef>       // for size_t, NULL
#include <cstdint>       // for uint32_t, int8_t
#include <deque>         // for deque
#include <exception>     // for exception
#include <fmt/chrono.h>  // for format_time
#include <fmt/format.h>  // for format_error
#include <functional>    // for _Func_impl_no_all...
#include <imgui.h>       // for GetIO, ImVec4
#include <lauxlib.h>     // for luaL_error
#include <list>          // for _List_const_iterator
#include <lua.h>         // for lua_Debug, lua_State
#include <map>           // for map, map<>::mappe...
#include <new>           // for operator new
#include <optional>      // for nullopt, optional
#include <sol/sol.hpp>   // for global_table, pro...
#include <string>        // for string, char_traits
#include <tuple>         // for get, tuple, make_...
#include <type_traits>   // for move, declval
#include <unordered_map> // for unordered_map
#include <unordered_set> // for unordered_set
#include <utility>       // for min, max, pair, get
#include <vector>        // for vector, _Vector_i...

#include "aliases.hpp"                             // for CallbackId, ENT_TYPE
#include "color.hpp"                               // for Color
#include "entities_chars.hpp"                      // for Player
#include "entities_items.hpp"                      // for Container, Player...
#include "entity.hpp"                              // for get_entity_ptr
#include "entity_lookup.hpp"                       //
#include "game_api.hpp"                            //
#include "game_manager.hpp"                        // for get_game_manager
#include "handle_lua_function.hpp"                 // for handle_function
#include "illumination.hpp"                        //
#include "items.hpp"                               // for Inventory
#include "layer.hpp"                               // for g_level_max_x
#include "lua_backend.hpp"                         // for LuaBackend, ON
#include "lua_console.hpp"                         // for LuaConsole
#include "lua_libs/lua_libs.hpp"                   // for require_format_lua
#include "lua_require.hpp"                         // for register_custom_r...
#include "math.hpp"                                // for AABB
#include "memory.hpp"                              // for Memory
#include "movable.hpp"                             // for Movable
#include "online.hpp"                              // for get_online
#include "overloaded.hpp"                          // for overloaded
#include "rpc.hpp"                                 // for get_entities_by
#include "safe_cb.hpp"                             // for make_safe_clearable_cb
#include "savedata.hpp"                            // IWYU pragma: keep
#include "screen.hpp"                              // for get_screen_ptr
#include "script.hpp"                              // for ScriptMessage
#include "script_util.hpp"                         // for sanitize, get_say
#include "search.hpp"                              // for get_address
#include "settings_api.hpp"                        // for get_settings_name...
#include "spawn_api.hpp"                           // for spawn_roomowner
#include "state.hpp"                               // for State, StateMemory
#include "strings.hpp"                             // for change_string
#include "thread_utils.hpp"                        // for OnHeapPointer
#include "usertypes/behavior_lua.hpp"              // for register_usertypes
#include "usertypes/bucket_lua.hpp"                // for register_usertypes
#include "usertypes/char_state_lua.hpp"            // for register_usertypes
#include "usertypes/color_lua.hpp"                 // for register_usertypes
#include "usertypes/drops_lua.hpp"                 // for register_usertypes
#include "usertypes/entities_activefloors_lua.hpp" // for register_usertypes
#include "usertypes/entities_backgrounds_lua.hpp"  // for register_usertypes
#include "usertypes/entities_chars_lua.hpp"        // for register_usertypes
#include "usertypes/entities_decorations_lua.hpp"  // for register_usertypes
#include "usertypes/entities_floors_lua.hpp"       // for register_usertypes
#include "usertypes/entities_fx_lua.hpp"           // for register_usertypes
#include "usertypes/entities_items_lua.hpp"        // for register_usertypes
#include "usertypes/entities_liquids_lua.hpp"      // for register_usertypes
#include "usertypes/entities_logical_lua.hpp"      // for register_usertypes
#include "usertypes/entities_monsters_lua.hpp"     // for register_usertypes
#include "usertypes/entities_mounts_lua.hpp"       // for register_usertypes
#include "usertypes/entity_casting_lua.hpp"        // for register_usertypes
#include "usertypes/entity_lua.hpp"                // for register_usertypes
#include "usertypes/flags_lua.hpp"                 // for register_usertypes
#include "usertypes/game_manager_lua.hpp"          // for register_usertypes
#include "usertypes/gui_lua.hpp"                   // for register_usertypes
#include "usertypes/hitbox_lua.hpp"                // for register_usertypes
#include "usertypes/level_lua.hpp"                 // for register_usertypes
#include "usertypes/logic_lua.hpp"                 // for register_usertypes
#include "usertypes/particles_lua.hpp"             // for register_usertypes
#include "usertypes/player_lua.hpp"                // for register_usertypes
#include "usertypes/prng_lua.hpp"                  // for register_usertypes
#include "usertypes/save_context.hpp"              // for register_usertypes
#include "usertypes/screen_arena_lua.hpp"          // for register_usertypes
#include "usertypes/screen_lua.hpp"                // for register_usertypes
#include "usertypes/socket_lua.hpp"                // for register_usertypes
#include "usertypes/sound_lua.hpp"                 // for register_usertypes
#include "usertypes/state_lua.hpp"                 // for register_usertypes
#include "usertypes/steam_lua.hpp"                 // for register_usertypes
#include "usertypes/texture_lua.hpp"               // for register_usertypes
#include "usertypes/vanilla_render_lua.hpp"        // for VanillaRenderContext
#include "usertypes/vtables_lua.hpp"               // for register_usertypes
#include "virtual_table.hpp"                       //

struct Illumination;

struct Players
{
    // This is probably over complicating
    // but i couldn't find better solution for the global players to be always correct
    // (not return reference to non existing entity when in between screens etc. like in draw callback)

    using value_type = Player*;
    using iterator = std::vector<Player*>::iterator;

    Players()
    {
        update();
    }
    size_t size()
    {
        update();
        return p.size();
    }
    Player* at(const int index)
    {
        update();
        if (index < 0 || index >= p.size())
            return nullptr;

        return p[index];
    }
    auto begin()
    {
        return p.begin();
    }
    auto end()
    {
        return p.end();
    }

  private:
    std::vector<Player*> p;

    void update()
    {
        StateMemory* local_state = State::get().ptr_local();
        if (local_state == nullptr)
        {
            StateMemory* main_state = State::get().ptr_main();
            p = get_players(main_state);
        }
        else
        {
            p = get_players(local_state);
        }
    }
    struct lua_iterator_state
    {
        typedef std::vector<Player*>::iterator it_t;
        it_t begin;
        it_t it;
        it_t last;

        lua_iterator_state(Players& mt)
            : begin(mt.begin()), it(mt.begin()), last(mt.end())
        {
        }
    };
    static std::tuple<sol::object, sol::object> my_next(sol::user<lua_iterator_state&> user_it_state, sol::this_state l)
    {
        // this gets called
        // to start the first iteration, and every
        // iteration there after

        lua_iterator_state& it_state = user_it_state;
        auto& it = it_state.it;
        if (it == it_state.last)
        {
            // return nil to signify that there's nothing more to work with.
            return std::make_tuple(sol::object(sol::lua_nil), sol::object(sol::lua_nil));
        }
        // 2 values are returned (pushed onto the stack):
        // the key and the value
        // the state is left alone
        auto r = std::make_tuple(
            sol::object(l, sol::in_place, it - it_state.begin + 1),
            sol::object(l, sol::in_place, *it));
        // the iterator must be moved forward one before we return
        std::advance(it, 1);
        return r;
    }

  public:
    static auto my_pairs(Players& mt)
    {
        mt.update();
        // pairs expects 3 returns:
        // the "next" function on how to advance,
        // the "table" itself or some state,
        // and an initial key value (can be nil)

        // prepare our state
        lua_iterator_state it_state(mt);
        // sol::user is a space/time optimization over regular
        // usertypes, it's incompatible with regular usertypes and
        // stores the type T directly in lua without any pretty
        // setup saves space allocation and a single dereference
        return std::make_tuple(&my_next, sol::user<lua_iterator_state>(std::move(it_state)), sol::lua_nil);
    }
};

void load_libraries(sol::state& lua)
{
    lua.open_libraries(sol::lib::math, sol::lib::base, sol::lib::string, sol::lib::table, sol::lib::coroutine, sol::lib::package);
    require_json_lua(lua);
    require_inspect_lua(lua);
    require_format_lua(lua);

    register_custom_require(lua);
}
void load_unsafe_libraries(sol::state& lua)
{
    lua.open_libraries(sol::lib::io, sol::lib::os, sol::lib::ffi, sol::lib::debug);
    require_serpent_lua(lua);
    NSocket::register_usertypes(lua);
}
void populate_lua_state(sol::state& lua, SoundManager* sound_manager)
{
    auto infinite_loop = [](lua_State* argst, [[maybe_unused]] lua_Debug* argdb)
    {
        static size_t last_frame = 0;
        auto backend = LuaBackend::get_calling_backend();
        if (last_frame == backend->frame_counter && backend->infinite_loop_detection)
            luaL_error(argst, "Hit Infinite Loop Detection of 420 million instructions");
        last_frame = backend->frame_counter;
    };

    lua_sethook(lua.lua_state(), NULL, 0, 0);
    lua_sethook(lua.lua_state(), infinite_loop, LUA_MASKCOUNT, 420000000);

    lua.safe_script(R"(
-- This function walks up the stack until it finds an _ENV that is not _G
-- That _ENV has to be the environment of a script where we can look up the scripts id
get_script_id = function()
    -- Not available in Lua 5.2+
    local getfenv = getfenv or function(f)
        f = (type(f) == 'function' and f or debug.getinfo(f + 1, 'f').func)
        local name, val
        local up = 0
        repeat
            up = up + 1
            name, val = debug.getupvalue(f, up)
        until name == '_ENV' or name == nil
        return val
    end

    local env
    local up = 1
    repeat
        up = up + 1
        env = getfenv(up)
    until env ~= _G and env ~= nil
    return env.__script_id
end
)");

    NHitbox::register_usertypes(lua);
    NSound::register_usertypes(lua, sound_manager);
    NLevel::register_usertypes(lua);
    NGui::register_usertypes(lua);
    NVanillaRender::register_usertypes(lua);
    NTexture::register_usertypes(lua);
    NEntity::register_usertypes(lua);
    NEntitiesChars::register_usertypes(lua);
    NEntitiesFloors::register_usertypes(lua);
    NEntitiesActiveFloors::register_usertypes(lua);
    NEntitiesBG::register_usertypes(lua);
    NEntitiesDecorations::register_usertypes(lua);
    NEntitiesLogical::register_usertypes(lua);
    NEntitiesMounts::register_usertypes(lua);
    NEntitiesMonsters::register_usertypes(lua);
    NEntitiesItems::register_usertypes(lua);
    NEntitiesFX::register_usertypes(lua);
    NEntitiesLiquids::register_usertypes(lua);
    NParticles::register_usertypes(lua);
    NSaveContext::register_usertypes(lua);
    NGM::register_usertypes(lua);
    NState::register_usertypes(lua);
    NPRNG::register_usertypes(lua);
    NScreen::register_usertypes(lua);
    NScreenArena::register_usertypes(lua);
    NPlayer::register_usertypes(lua);
    NDrops::register_usertypes(lua);
    NCharacterState::register_usertypes(lua);
    NEntityFlags::register_usertypes(lua);
    NEntityCasting::register_usertypes(lua);
    NBehavior::register_usertypes(lua);
    NSteam::register_usertypes(lua);
    NVTables::register_usertypes(lua);
    NLogic::register_usertypes(lua);
    NBucket::register_usertypes(lua);
    NColor::register_usertypes(lua);

    StateMemory* main_state = State::get().ptr_main();

    /// NoDoc
    lua.new_usertype<Players>(
        "Players", sol::no_constructor, sol::meta_function::index, [](Players* p, const int index)
        { return p->at(index - 1); },
        sol::meta_function::pairs,
        Players::my_pairs);
    Players players;

    /// A bunch of [game state](#StateMemory) variables. Your ticket to almost anything that is not an Entity.
    lua["state"] = main_state;
    /// The GameManager gives access to a couple of Screens as well as the pause and journal UI elements
    lua["game_manager"] = get_game_manager();
    /// The Online object has information about the online lobby and its players
    lua["online"] = get_online();
    /// An array of [Player](#Player) of the current players. This is just a list of existing Player entities in order, i.e., `players[1]` is not guaranteed to be P1 if they have been gibbed for example. See [get_player](#get_player).
    lua["players"] = players;

    auto get_player = sol::overload(
        [&lua](int8_t slot) -> sol::object // -> Player
        {
            for (auto player : get_players(State::get().ptr()))
            {
                if (player->inventory_ptr->player_slot == slot - 1)
                    return sol::make_object_userdata(lua, player);
            }
            return sol::nil;
        },
        [&lua](int8_t slot, bool or_ghost) -> sol::object
        {
            for (auto player : get_players(State::get().ptr()))
            {
                if (player->inventory_ptr->player_slot == slot - 1)
                    return sol::make_object_userdata(lua, player);
            }
            if (or_ghost)
            {
                for (auto uid : get_entities_by(to_id("ENT_TYPE_ITEM_PLAYERGHOST"), 0x8u, LAYER::BOTH))
                {
                    auto player = get_entity_ptr(uid)->as<PlayerGhost>();
                    if (player->inventory->player_slot == slot - 1)
                        return sol::make_object_userdata(lua, player);
                }
            }
            return sol::nil;
        });

    /// Returns Player (or PlayerGhost if `get_player(1, true)`) with this player slot
    // lua["get_player"] = [](int8_t slot, bool or_ghost = false) -> Player
    lua["get_player"] = get_player;

    /// Returns PlayerGhost with this player slot 1..4
    lua["get_playerghost"] = [](int8_t slot) -> PlayerGhost*
    {
        for (auto uid : get_entities_by(to_id("ENT_TYPE_ITEM_PLAYERGHOST"), 0x8u, LAYER::BOTH))
        {
            auto player = get_entity_ptr(uid)->as<PlayerGhost>();
            if (player->inventory->player_slot == slot - 1)
                return player;
        }
        return nullptr;
    };
    /// Provides access to the save data, updated as soon as something changes (i.e. before it's written to savegame.sav.) Use [save_progress](#save_progress) to save to savegame.sav.
    lua["savegame"] = State::get().savedata();

    /// Standard lua print function, prints directly to the terminal but not to the game
    lua["lua_print"] = lua["print"];

    /// Print a log message on screen.
    lua["print"] = [](std::string message) -> void
    {
        auto backend = LuaBackend::get_calling_backend();
        bool is_console = !strcmp(backend->get_id(), "dev/lua_console");
        backend->messages.push_back({message, std::chrono::system_clock::now(), ImVec4(1.0f, 1.0f, 1.0f, 1.0f)});
        if (backend->messages.size() > 40 && !is_console)
            backend->messages.pop_front();
        backend->lua["lua_print"](message);
    };

    /// Print a log message to in-game console with a comment identifying the script that sent it.
    lua["console_print"] = [&lua](std::string message) -> void
    {
        auto backend = LuaBackend::get_calling_backend();
        bool is_console = !strcmp(backend->get_id(), "dev/lua_console");
        if (is_console)
        {
            lua["print"](std::move(message));
            return;
        }
        auto in_time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::tm time_buf;
        localtime_s(&time_buf, &in_time_t);
        std::vector<ScriptMessage> messages{{message, std::chrono::system_clock::now(), ImVec4(1.0f, 1.0f, 1.0f, 1.0f)}};
        backend->console->push_history(fmt::format("--- [{}] at {:%Y-%m-%d %X}", backend->get_id(), time_buf), std::move(messages));
    };

    /// Prinspect to in-game console.
    lua["console_prinspect"] = [&lua](sol::variadic_args objects) -> void
    {
        if (objects.size() > 0)
        {
            std::string message;
            for (const auto& obj : objects)
            {
                message += lua["inspect"](obj);
                message += ", ";
            }
            message.pop_back();
            message.pop_back();

            lua["console_print"](std::move(message));
        }
    };

    /// Same as `print`
    lua["message"] = [&lua](std::string message) -> void
    { lua["print"](message); };

    /// Prints any type of object by first funneling it through `inspect`, no need for a manual `tostring` or `inspect`.
    lua["prinspect"] = [&lua](sol::variadic_args objects) -> void
    {
        if (objects.size() > 0)
        {
            std::string message;
            for (const auto& obj : objects)
            {
                message += lua["inspect"](obj);
                message += ", ";
            }
            message.pop_back();
            message.pop_back();

            lua["print"](std::move(message));
        }
    };

    /// Same as `prinspect`
    lua["messpect"] = [&lua](sol::variadic_args objects) -> void
    { lua["prinspect"](objects); };

    /// Dump the object (table, container, class) as a recursive table, for pretty printing in console. Don't use this for anything except debug printing. Unsafe.
    // lua["dump"] = [](object object, optional<int> depth) -> table

    /// Adds a command that can be used in the console.
    lua["register_console_command"] = [](std::string name, sol::function cmd)
    {
        auto backend = LuaBackend::get_calling_backend();
        if (backend->console)
        {
            backend->console_commands.insert(name);
            backend->console->register_command(backend.get(), std::move(name), std::move(cmd));
        }
    };

    /// Returns unique id for the callback to be used in [clear_callback](#clear_callback). You can also return `false` from your function to clear the callback.
    /// Add per level callback function to be called every `frames` engine frames
    /// Ex. frames = 100 - will call the function on 100th frame from this point. This might differ in the exact timing of first frame depending as in what part of the frame you call this function
    /// or even be one frame off if called right before the time_level variable is updated
    /// If you require precise timing, choose the start of your interval in one of those safe callbacks:
    /// The SCREEN callbacks: from ON.LOGO to ON.ONLINE_LOBBY or custom callbacks ON.FRAME, ON.SCREEN, ON.START, ON.LOADING, ON.RESET, ON.POST_UPDATE
    /// Timer is paused on pause and cleared on level transition.
    lua["set_interval"] = [](sol::function cb, int frames) -> CallbackId
    {
        auto backend = LuaBackend::get_calling_backend();
        auto state = State::get().ptr_main();
        auto luaCb = IntervalCallback{cb, frames, (int)state->time_level};
        backend->level_timers[backend->cbcount] = luaCb;
        return backend->cbcount++;
    };
    /// Returns unique id for the callback to be used in [clear_callback](#clear_callback).
    /// Add per level callback function to be called after `frames` engine frames. Timer is paused on pause and cleared on level transition.
    lua["set_timeout"] = [](sol::function cb, int frames) -> CallbackId
    {
        auto backend = LuaBackend::get_calling_backend();
        int now = backend->g_state->time_level;
        auto luaCb = TimeoutCallback{cb, now + frames};
        backend->level_timers[backend->cbcount] = luaCb;
        return backend->cbcount++;
    };
    /// Returns unique id for the callback to be used in [clear_callback](#clear_callback). You can also return `false` from your function to clear the callback.
    /// Add global callback function to be called every `frames` engine frames. This timer is never paused or cleared.
    lua["set_global_interval"] = [](sol::function cb, int frames) -> CallbackId
    {
        auto backend = LuaBackend::get_calling_backend();
        auto luaCb = IntervalCallback{cb, frames, -1};
        backend->global_timers[backend->cbcount] = luaCb;
        return backend->cbcount++;
    };
    /// Returns unique id for the callback to be used in [clear_callback](#clear_callback).
    /// Add global callback function to be called after `frames` engine frames. This timer is never paused or cleared.
    lua["set_global_timeout"] = [](sol::function cb, int frames) -> CallbackId
    {
        auto backend = LuaBackend::get_calling_backend();
        int now = get_frame_count();
        auto luaCb = TimeoutCallback{cb, now + frames};
        backend->global_timers[backend->cbcount] = luaCb;
        return backend->cbcount++;
    };
    /// Returns unique id for the callback to be used in [clear_callback](#clear_callback).
    /// Add global callback function to be called on an [event](#Events).
    lua["set_callback"] = [](sol::function cb, ON event) -> CallbackId
    {
        auto backend = LuaBackend::get_calling_backend();
        auto luaCb = ScreenCallback{cb, event, -1};
        if (luaCb.screen == ON::LOAD)
            backend->load_callbacks[backend->cbcount] = luaCb; // Make sure load always runs before other callbacks
        else if (luaCb.screen == ON::SAVE)
            backend->save_callbacks[backend->cbcount] = luaCb; // Make sure save always runs after other callbacks
        else
            backend->callbacks[backend->cbcount] = luaCb;
        return backend->cbcount++;
    };
    /// Clear previously added callback `id` or call without arguments inside any callback to clear that callback after it returns.
    // lua["clear_callback"] = [](sol::optional<CallbackId> id) -> void {};
    lua["clear_callback"] = sol::overload(
        [](CallbackId id)
        {
            auto backend = LuaBackend::get_calling_backend();
            backend->clear_callbacks.push_back(id);
        },
        []()
        {
            auto backend = LuaBackend::get_calling_backend();
            auto caller = backend->get_current_callback();
            switch (caller.type)
            {
            case CallbackType::Normal:
            case CallbackType::HotKey:
                backend->clear_callbacks.push_back(caller.id);
                break;
            case CallbackType::Entity:
                backend->HookHandler<Entity, CallbackType::Entity>::clear_hook(caller.id, caller.aux_id);
                break;
            case CallbackType::Screen:
                backend->clear_screen_hooks.push_back({caller.aux_id, caller.id});
                break;
            case CallbackType::Theme:
                backend->HookHandler<ThemeInfo, CallbackType::Theme>::clear_hook(caller.id, caller.aux_id);
                break;
            case CallbackType::None:
                // DEBUG("No callback to clear");
            default:
                break;
            }
        });

    lua.create_named_table("HOTKEY_TYPE", "NORMAL", HOTKEY_TYPE::NORMAL, "GLOBAL", HOTKEY_TYPE::GLOBAL, "INPUT", HOTKEY_TYPE::INPUT);
    /* HOTKEY_TYPE
    // NORMAL
    // Suppressed when the game window is inactive or inputting text in this tool instance (get_io().wantkeyboard == true). Can't detect if OL is in a text input and script is running in PL though. Use ImGuiIO if you need to do that.
    // GLOBAL
    // Enabled even when the game window is inactive and will capture keys even from other programs.
    // INPUT
    // Enabled even when inputting text and will override normal text input keys.
    */

    /// Table of options set in the UI, added with the [register_option_functions](#Option-functions), but `nil` before any options are registered. You can also write your own options in here or override values defined in the register functions/UI before or after they are registered. Check the examples for many different use cases and saving options to disk.
    // lua["options"] = lua.create_named_table("options");

    /// Load another script by id "author/name" and import its `exports` table. Returns:
    ///
    /// - `table` if the script has exports
    /// - `nil` if the script was found but has no exports
    /// - `false` if the script was not found but optional is set to true
    /// - an error if the script was not found and the optional argument was not set
    // lua["import"] = [](string id, optional<string> version, optional<bool> optional) -> table
    lua["import"] = sol::overload(
        [&lua](std::string id)
        {
            auto backend = LuaBackend::get_calling_backend();
            backend->required_scripts.push_back(sanitize(id));
            auto import_backend_opt = LuaBackend::get_backend_by_id_safe(std::string_view(sanitize(id)));
            if (!import_backend_opt.has_value())
            {
                luaL_error(lua, "Imported script not found");
                return sol::make_object(lua, sol::lua_nil);
            }
            auto& import_backend = import_backend_opt.value();
            if (!import_backend->get_enabled())
            {
                import_backend->set_enabled(true);
                import_backend->update();
            }
            return sol::make_object(lua, import_backend->lua["exports"]);
        },
        [&lua](std::string id, std::string version)
        {
            auto backend = LuaBackend::get_calling_backend();
            backend->required_scripts.push_back(sanitize(id));
            auto import_backend_opt = LuaBackend::get_backend_by_id_safe(std::string_view(sanitize(id)), std::string_view(version));
            if (!import_backend_opt.has_value())
            {
                luaL_error(lua, "Imported script not found");
                return sol::make_object(lua, sol::lua_nil);
            }
            auto& import_backend = import_backend_opt.value();
            if (!import_backend->get_enabled())
            {
                import_backend->set_enabled(true);
                import_backend->update();
            }
            return sol::make_object(lua, import_backend->lua["exports"]);
        },
        [&lua](std::string id, std::string version, bool optional)
        {
            auto backend = LuaBackend::get_calling_backend();
            backend->required_scripts.push_back(sanitize(id));
            auto import_backend_opt = LuaBackend::get_backend_by_id_safe(std::string_view(sanitize(id)), std::string_view(version));
            if (!import_backend_opt.has_value())
            {
                if (!optional)
                    luaL_error(lua, "Imported script not found");
                return sol::make_object(lua, false);
            }
            auto& import_backend = import_backend_opt.value();
            if (!import_backend->get_enabled())
            {
                import_backend->set_enabled(true);
                import_backend->update();
            }
            return sol::make_object(lua, import_backend->lua["exports"]);
        },
        [&lua](std::string id, bool optional)
        {
            auto backend = LuaBackend::get_calling_backend();
            backend->required_scripts.push_back(sanitize(id));
            auto import_backend_opt = LuaBackend::get_backend_by_id_safe(std::string_view(sanitize(id)));
            if (!import_backend_opt.has_value())
            {
                if (!optional)
                    luaL_error(lua, "Imported script not found");
                return sol::make_object(lua, false);
            }
            auto& import_backend = import_backend_opt.value();
            if (!import_backend->get_enabled())
            {
                import_backend->set_enabled(true);
                import_backend->update();
            }
            return sol::make_object(lua, import_backend->lua["exports"]);
        });

    /// Deprecated
    /// Same as import().
    lua["load_script"] = lua["import"];

    /// Check if another script is enabled by id "author/name". You should probably check this after all the other scripts have had a chance to load.
    // lua["script_enabled"] = [](string id, string version = "") -> bool
    lua["script_enabled"] = sol::overload(
        [](std::string id)
        {
            auto import_backend_opt = LuaBackend::get_backend_by_id_safe(std::string_view(sanitize(id)));
            if (!import_backend_opt.has_value())
                return false;
            auto& import_backend = import_backend_opt.value();
            return import_backend->get_enabled();
        },
        [](std::string id, std::string version)
        {
            auto import_backend_opt = LuaBackend::get_backend_by_id_safe(std::string_view(sanitize(id)), std::string_view(version));
            if (!import_backend_opt.has_value())
                return false;
            auto& import_backend = import_backend_opt.value();
            return import_backend->get_enabled();
        });

    /// Some random hash function
    lua["lowbias32"] = lowbias32;

    /// Reverse of some random hash function
    lua["lowbias32_r"] = lowbias32_r;

    /// Get your sanitized script id to be used in import.
    lua["get_id"] = []() -> std::string
    {
        auto backend = LuaBackend::get_calling_backend();
        return backend->get_id();
    };

    /// Deprecated
    /// Read the game prng state. Use [prng](#PRNG):get_pair() instead.
    lua["read_prng"] = []() -> std::vector<int64_t>
    { return read_prng(); };

    using Toast = void(const char16_t*);
    using Say = void(HudData*, Entity*, const char16_t*, int, bool);

    /// Show a message that looks like a level feeling.
    lua["toast"] = [](std::u16string message)
    {
        static auto toast_fun = (Toast*)get_address("toast");
        toast_fun(message.c_str());
    };
    /// Show a message coming from an entity
    lua["say"] = [](uint32_t entity_uid, std::u16string message, int sound_type, bool top)
    {
        static auto say = (Say*)get_address("speech_bubble_fun");
        const auto hud = get_hud();

        auto entity = get_entity_ptr(entity_uid);

        if (entity == nullptr)
            return;

        say(hud, entity, message.c_str(), sound_type, top);
    };
    /// Add an integer option that the user can change in the UI. Read with `options.name`, `value` is the default. Keep in mind these are just soft
    /// limits, you can override them in the UI with double click.
    // lua["register_option_int"] = [](std::string name, std::string desc, std::string long_desc, int value, int min, int max)
    lua["register_option_int"] = sol::overload(
        [](std::string name, std::string desc, std::string long_desc, int value, int min, int max)
        {
            auto backend = LuaBackend::get_calling_backend();
            backend->options[name] = {desc, long_desc, IntOption{value, min, max}};
            if (backend->lua["options"][name] == sol::nil)
                backend->lua[sol::create_if_nil]["options"][name] = value;
        },
        [](std::string name, std::string desc, int value, int min, int max)
        {
            auto backend = LuaBackend::get_calling_backend();
            backend->options[name] = {desc, "", IntOption{value, min, max}};
            if (backend->lua["options"][name] == sol::nil)
                backend->lua[sol::create_if_nil]["options"][name] = value;
        });
    /// Add a float option that the user can change in the UI. Read with `options.name`, `value` is the default. Keep in mind these are just soft
    /// limits, you can override them in the UI with double click.
    // lua["register_option_float"] = [](std::string name, std::string desc, std::string long_desc, float value, float min, float max)
    lua["register_option_float"] = sol::overload(
        [](std::string name, std::string desc, std::string long_desc, float value, float min, float max)
        {
            auto backend = LuaBackend::get_calling_backend();
            backend->options[name] = {desc, long_desc, FloatOption{value, min, max}};
            if (backend->lua["options"][name] == sol::nil)
                backend->lua[sol::create_if_nil]["options"][name] = value;
        },
        [](std::string name, std::string desc, float value, float min, float max)
        {
            auto backend = LuaBackend::get_calling_backend();
            backend->options[name] = {desc, "", FloatOption{value, min, max}};
            if (backend->lua["options"][name] == sol::nil)
                backend->lua[sol::create_if_nil]["options"][name] = value;
        });
    /// Add a boolean option that the user can change in the UI. Read with `options.name`, `value` is the default.
    // lua["register_option_bool"] = [](std::string name, std::string desc, std::string long_desc, bool value)
    lua["register_option_bool"] = sol::overload(
        [](std::string name, std::string desc, std::string long_desc, bool value)
        {
            auto backend = LuaBackend::get_calling_backend();
            backend->options[name] = {desc, long_desc, BoolOption{value}};
            if (backend->lua["options"][name] == sol::nil)
                backend->lua[sol::create_if_nil]["options"][name] = value;
        },
        [](std::string name, std::string desc, bool value)
        {
            auto backend = LuaBackend::get_calling_backend();
            backend->options[name] = {desc, "", BoolOption{value}};
            if (backend->lua["options"][name] == sol::nil)
                backend->lua[sol::create_if_nil]["options"][name] = value;
        });
    /// Add a string option that the user can change in the UI. Read with `options.name`, `value` is the default.
    // lua["register_option_string"] = [](std::string name, std::string desc, std::string long_desc, std::string value)
    lua["register_option_string"] = sol::overload(
        [](std::string name, std::string desc, std::string long_desc, std::string value)
        {
            auto backend = LuaBackend::get_calling_backend();
            backend->options[name] = {desc, long_desc, StringOption{value}};
            if (backend->lua["options"][name] == sol::nil)
                backend->lua[sol::create_if_nil]["options"][name] = value;
        },
        [](std::string name, std::string desc, std::string value)
        {
            auto backend = LuaBackend::get_calling_backend();
            backend->options[name] = {desc, "", StringOption{value}};
            if (backend->lua["options"][name] == sol::nil)
                backend->lua[sol::create_if_nil]["options"][name] = value;
        });
    /// Add a combobox option that the user can change in the UI. Read the int index of the selection with `options.name`. Separate `opts` with `\0`,
    /// with a double `\0\0` at the end. `value` is the default index 1..n.
    // lua["register_option_combo"] = [](std::string name, std::string desc, std::string long_desc, std::string opts, int value)
    lua["register_option_combo"] = sol::overload(
        [](std::string name, std::string desc, std::string long_desc, std::string opts, int value)
        {
            auto backend = LuaBackend::get_calling_backend();
            backend->options[name] = {desc, long_desc, ComboOption{value - 1, opts}};
            if (backend->lua["options"][name] == sol::nil)
                backend->lua[sol::create_if_nil]["options"][name] = value;
        },
        [](std::string name, std::string desc, std::string long_desc, std::string opts)
        {
            auto backend = LuaBackend::get_calling_backend();
            backend->options[name] = {desc, long_desc, ComboOption{0, opts}};
            if (backend->lua["options"][name] == sol::nil)
                backend->lua[sol::create_if_nil]["options"][name] = 1;
        },
        [](std::string name, std::string desc, std::string opts, int value)
        {
            auto backend = LuaBackend::get_calling_backend();
            backend->options[name] = {desc, "", ComboOption{value - 1, opts}};
            if (backend->lua["options"][name] == sol::nil)
                backend->lua[sol::create_if_nil]["options"][name] = value;
        },
        [](std::string name, std::string desc, std::string opts)
        {
            auto backend = LuaBackend::get_calling_backend();
            backend->options[name] = {desc, "", ComboOption{0, opts}};
            if (backend->lua["options"][name] == sol::nil)
                backend->lua[sol::create_if_nil]["options"][name] = 1;
        });
    /// Add a button that the user can click in the UI. Sets the timestamp of last click on value and runs the callback function.
    // lua["register_option_button"] = [](std::string name, std::string desc, std::string long_desc, sol::function on_click)
    lua["register_option_button"] = sol::overload(
        [](std::string name, std::string desc, std::string long_desc, sol::function callback)
        {
            auto backend = LuaBackend::get_calling_backend();
            backend->options[name] = {desc, long_desc, ButtonOption{callback}};
            backend->lua[sol::create_if_nil]["options"][name] = -1;
        },
        [](std::string name, std::string desc, sol::function callback)
        {
            auto backend = LuaBackend::get_calling_backend();
            backend->options[name] = {desc, "", ButtonOption{callback}};
            backend->lua[sol::create_if_nil]["options"][name] = -1;
        });
    /// Add custom options using the window drawing functions. Everything drawn in the callback will be rendered in the options window and the return value saved to `options[name]` or overwriting the whole `options` table if using and empty name.
    /// `value` is the default value, and pretty important because anything defined in the callback function will only be defined after the options are rendered. See the example for details.
    /// <br/>The callback signature is optional<any> on_render(GuiDrawContext draw_ctx)
    lua["register_option_callback"] = [](std::string name, sol::object value, sol::function on_render)
    {
        auto backend = LuaBackend::get_calling_backend();
        backend->options[name] = {"", "", CustomOption{on_render}};
        if (backend->lua["options"][name] == sol::nil)
        {
            if (name != "")
                backend->lua[sol::create_if_nil]["options"][name] = value;
            else
                backend->lua[sol::create_if_nil]["options"] = value;
        }
    };

    /// Removes an option by name. To make complicated conditionally visible options you should probably just use register_option_callback though.
    lua["unregister_option"] = [](std::string name)
    {
        auto backend = LuaBackend::get_calling_backend();
        backend->options.erase(name);
        backend->lua["options"][name] = sol::nil;
    };

    auto spawn_liquid = sol::overload(
        static_cast<void (*)(ENT_TYPE, float, float)>(::spawn_liquid),
        static_cast<void (*)(ENT_TYPE, float, float, float, float, uint32_t, uint32_t)>(::spawn_liquid_ex),
        static_cast<void (*)(ENT_TYPE, float, float, float, float, uint32_t, uint32_t, float)>(::spawn_liquid));
    /// Spawn liquids, always spawns in the front layer, will have fun effects if `entity_type` is not a liquid (only the short version, without velocity etc.).
    /// Don't overuse this, you are still restricted by the liquid pool sizes and thus might crash the game.
    /// `liquid_flags` - not much known about, 2 - will probably crash the game, 3 - pause_physics, 6-12 is probably agitation, surface_tension etc. set to 0 to ignore
    /// `amount` - it will spawn amount x amount (so 1 = 1, 2 = 4, 3 = 6 etc.), `blobs_separation` is optional
    lua["spawn_liquid"] = spawn_liquid;
    /// Spawn an entity in position with some velocity and return the uid of spawned entity.
    /// Uses level coordinates with [LAYER.FRONT](#LAYER) and LAYER.BACK, but player-relative coordinates with LAYER.PLAYER(n), where (n) is a player number (1-4).
    lua["spawn_entity"] = spawn_entity_abs;
    /// Short for [spawn_entity](#spawn_entity).
    lua["spawn"] = spawn_entity_abs;
    /// Spawns an entity directly on the floor below the tile at the given position.
    /// Use this to avoid the little fall that some entities do when spawned during level gen callbacks.
    lua["spawn_entity_snapped_to_floor"] = spawn_entity_snap_to_floor;
    /// Short for [spawn_entity_snapped_to_floor](#spawn_entity_snapped_to_floor).
    lua["spawn_on_floor"] = spawn_entity_snap_to_floor;
    /// Spawn a grid entity, such as floor or traps, that snaps to the grid.
    lua["spawn_grid_entity"] = spawn_entity_snap_to_grid;
    /// Same as `spawn_entity` but does not trigger any pre-entity-spawn callbacks, so it will not be replaced by another script
    lua["spawn_entity_nonreplaceable"] = spawn_entity_abs_nonreplaceable;
    /// Short for [spawn_entity_nonreplaceable](#spawn_entity_nonreplaceable).
    lua["spawn_critical"] = spawn_entity_abs_nonreplaceable;
    /// Spawn a door to another world, level and theme and return the uid of spawned entity.
    /// Uses level coordinates with LAYER.FRONT and LAYER.BACK, but player-relative coordinates with LAYER.PLAYERn
    lua["spawn_door"] = spawn_door_abs;
    /// Short for [spawn_door](#spawn_door).
    lua["door"] = spawn_door_abs;
    /// Spawn a door to backlayer.
    lua["spawn_layer_door"] = spawn_backdoor_abs;
    /// Short for [spawn_layer_door](#spawn_layer_door).
    lua["layer_door"] = spawn_backdoor_abs;
    /// Spawns apep with the choice if it going left or right, if you want the game to choose use regular spawn functions with `ENT_TYPE.MONS_APEP_HEAD`
    lua["spawn_apep"] = spawn_apep;

    auto spawn_tree = sol::overload(
        static_cast<int32_t (*)(float, float, LAYER)>(::spawn_tree),
        static_cast<int32_t (*)(float, float, LAYER, uint16_t)>(::spawn_tree));
    /// Spawns and grows a tree
    lua["spawn_tree"] = spawn_tree;

    auto spawn_mushroom = sol::overload(
        static_cast<int32_t (*)(float, float, LAYER)>(::spawn_mushroom),
        static_cast<int32_t (*)(float, float, LAYER, uint16_t)>(::spawn_mushroom));
    /// Spawns and grows mushroom, height relates to the trunk, without it, it will roll the game default 3-5 height
    /// Regardless, if there is not enough space, it will spawn shorter one or if there is no space even for the smallest one, it will just not spawn at all
    /// Returns uid of the base or -1 if it wasn't able to spawn
    lua["spawn_mushroom"] = spawn_mushroom;

    auto spawn_unrolled_player_rope = sol::overload(
        static_cast<int32_t (*)(float, float, LAYER, TEXTURE)>(::spawn_unrolled_player_rope),
        static_cast<int32_t (*)(float, float, LAYER, TEXTURE, uint16_t)>(::spawn_unrolled_player_rope));

    /// Spawns an already unrolled rope as if created by player
    lua["spawn_unrolled_player_rope"] = spawn_unrolled_player_rope;

    /// NoDoc
    /// Spawns an impostor lake, `top_threshold` determines how much space on top is rendered as liquid but does not have liquid physics, fill that space with real liquid
    /// There needs to be other liquid in the level for the impostor lake to be visible, there can only be one impostor lake in the level
    lua["spawn_impostor_lake"] = spawn_impostor_lake;
    /// NoDoc
    /// Fixes the bounds of impostor lakes in the liquid physics engine to match the bounds of the impostor lake entities.
    lua["fix_impostor_lake_positions"] = fix_impostor_lake_positions;
    /// Spawn a player in given location, if player of that slot already exist it will spawn clone, the game may crash as this is very unexpected situation
    /// If you want to respawn a player that is a ghost, set in his Inventory `health` to above 0, and `time_of_death` to 0 and call this function, the ghost entity will be removed automatically
    lua["spawn_player"] = spawn_player;
    /// Spawn the PlayerGhost entity, it will not move and not be connected to any player, you can then use [steal_input](#steal_input) and send_input to control it
    /// or change it's `player_inputs` to the `input` of real player so he can control it directly
    lua["spawn_playerghost"] = spawn_playerghost;
    /// Add a callback for a spawn of specific entity types or mask. Set `mask` to `MASK.ANY` to ignore that.
    /// This is run before the entity is spawned, spawn your own entity and return its uid to replace the intended spawn.
    /// In many cases replacing the intended entity won't have the intended effect or will even break the game, so use only if you really know what you're doing.
    /// <br/>The callback signature is optional<int> pre_entity_spawn(ENT_TYPE entity_type, float x, float y, int layer, Entity overlay_entity, SPAWN_TYPE spawn_flags)
    lua["set_pre_entity_spawn"] = [](sol::function cb, SPAWN_TYPE flags, int mask, sol::variadic_args entity_types) -> CallbackId
    {
        std::vector<ENT_TYPE> types;
        sol::type va_type = entity_types.get_type();
        if (va_type == sol::type::number)
        {
            types = std::vector<uint32_t>(entity_types.begin(), entity_types.end());
        }
        else if (va_type == sol::type::table)
        {
            types = entity_types.get<std::vector<uint32_t>>(0);
        }
        std::vector<ENT_TYPE> proper_types = get_proper_types(std::move(types));

        auto backend = LuaBackend::get_calling_backend();
        backend->pre_entity_spawn_callbacks.push_back(EntitySpawnCallback{backend->cbcount, mask, std::move(proper_types), flags, std::move(cb)});
        return backend->cbcount++;
    };
    /// Add a callback for a spawn of specific entity types or mask. Set `mask` to `MASK.ANY` to ignore that.
    /// This is run right after the entity is spawned but before and particular properties are changed, e.g. owner or velocity.
    /// <br/>The callback signature is nil post_entity_spawn(Entity ent, SPAWN_TYPE spawn_flags)
    lua["set_post_entity_spawn"] = [](sol::function cb, SPAWN_TYPE flags, int mask, sol::variadic_args entity_types) -> CallbackId
    {
        std::vector<ENT_TYPE> types;
        sol::type va_type = entity_types.get_type();
        if (va_type == sol::type::number)
        {
            types = std::vector<uint32_t>(entity_types.begin(), entity_types.end());
        }
        else if (va_type == sol::type::table)
        {
            types = entity_types.get<std::vector<uint32_t>>(0);
        }
        std::vector<ENT_TYPE> proper_types = get_proper_types(std::move(types));

        auto backend = LuaBackend::get_calling_backend();
        backend->post_entity_spawn_callbacks.push_back(EntitySpawnCallback{backend->cbcount, mask, std::move(proper_types), flags, std::move(cb)});
        return backend->cbcount++;
    };

    /// Warp to a level immediately.
    lua["warp"] = warp;
    /// Set seed and reset run.
    lua["set_seed"] = set_seed;
    /// Enable/disable godmode for players.
    lua["god"] = [](bool g)
    { State::get().godmode(g); };
    /// Enable/disable godmode for companions.
    lua["god_companions"] = [](bool g)
    { State::get().godmode_companions(g); };
    /// Deprecated
    /// Set level flag 18 on post room generation instead, to properly force every level to dark
    lua["force_dark_level"] = [](bool g)
    { State::get().darkmode(g); };
    /// Set the zoom level used in levels and shops. 13.5 is the default, or 12.5 for shops. See zoom_reset.
    lua["zoom"] = [](float level)
    { State::get().zoom(level); };
    /// Reset the default zoom levels for all areas and sets current zoom level to 13.5.
    lua["zoom_reset"] = []()
    { State::get().zoom_reset(); };
    auto move_entity_abs = sol::overload(
        static_cast<void (*)(uint32_t, float, float, float, float)>(::move_entity_abs),
        static_cast<void (*)(uint32_t, float, float, float, float, LAYER)>(::move_entity_abs));
    /// Teleport entity to coordinates with optional velocity
    lua["move_entity"] = move_entity_abs;
    /// Teleport grid entity, the destination should be whole number, this ensures that the collisions will work properly
    lua["move_grid_entity"] = move_grid_entity;
    auto destroy_grid = sol::overload(
        static_cast<void (*)(int32_t uid)>(::destroy_grid),
        static_cast<void (*)(float x, float y, LAYER layer)>(::destroy_grid));
    /// Destroy the grid entity (by uid or position), and its item entities, removing them from the grid without dropping particles or gold.
    /// Will also destroy monsters or items that are standing on a linked activefloor or chain, though excludes MASK.PLAYER to prevent crashes
    lua["destroy_grid"] = destroy_grid;
    /// Make an ENT_TYPE.FLOOR_DOOR_EXIT go to world `w`, level `l`, theme `t`
    lua["set_door_target"] = set_door_target;
    /// Short for [set_door_target](#set_door_target).
    lua["set_door"] = set_door_target;
    /// Get door target `world`, `level`, `theme`
    lua["get_door_target"] = get_door_target;
    /// Set the contents of [Coffin](#Coffin), [Present](#Present), [Pot](#Pot), [Container](#Container)
    /// Check the [entity hierarchy list](https://github.com/spelunky-fyi/overlunky/blob/main/docs/entities-hierarchy.md) for what the exact ENT_TYPE's can this function affect
    lua["set_contents"] = set_contents;
    /// Get the Entity behind an uid, converted to the correct type. To see what type you will get, consult the [entity hierarchy list](https://github.com/spelunky-fyi/overlunky/blob/main/docs/entities-hierarchy.md)
    // lua["get_entity"] = [](uint32_t uid) -> Entity*{};
    /// NoDoc
    /// Get the [Entity](#Entity) behind an uid, without converting to the correct type (do not use, use `get_entity` instead)
    lua["get_entity_raw"] = get_entity_ptr;
    lua.script(R"##(
        function cast_entity(entity_raw)
            if entity_raw == nil then
                return nil
            end

            local cast_fun = TYPE_MAP[entity_raw.type.id]
            if cast_fun ~= nil then
                return cast_fun(entity_raw)
            else
                return entity_raw
            end
        end
        function get_entity(ent_uid)
            if ent_uid == nil then
                return nil
            end

            local entity_raw = get_entity_raw(ent_uid)
            if entity_raw == nil then
                return nil
            end

            return cast_entity(entity_raw)
        end
        )##");
    /// Get the [EntityDB](#EntityDB) behind an ENT_TYPE...
    lua["get_type"] = get_type;
    /// Gets a grid entity, such as floor or spikes, at the given position and layer.
    lua["get_grid_entity_at"] = get_grid_entity_at;
    /// Get uids of static entities overlapping this grid position (decorations, backgrounds etc.)
    lua["get_entities_overlapping_grid"] = get_entities_overlapping_grid;
    /// Deprecated
    /// Use `get_entities_by(0, MASK.ANY, LAYER.BOTH)` instead
    lua["get_entities"] = get_entities;
    /// Returns a list of all uids in `entities` for which `predicate(get_entity(uid))` returns true
    lua["filter_entities"] = [&lua](std::vector<uint32_t> entities, sol::function predicate) -> std::vector<uint32_t>
    {
        return filter_entities(std::move(entities), [&lua, pred = std::move(predicate)](Entity* entity) -> bool
                               { return pred(lua["cast_entity"](entity)); });
    };

    auto get_entities_by = sol::overload(
        static_cast<std::vector<uint32_t> (*)(ENT_TYPE, uint32_t, LAYER)>(::get_entities_by),
        static_cast<std::vector<uint32_t> (*)(std::vector<ENT_TYPE>, uint32_t, LAYER)>(::get_entities_by));
    /// Get uids of entities by some conditions ([ENT_TYPE](#ENT_TYPE), [MASK](#MASK)). Set `entity_type` or `mask` to `0` to ignore that, can also use table of entity_types.
    /// Recommended to always set the mask, even if you look for one entity type
    lua["get_entities_by"] = get_entities_by;
    /// Get uids of entities matching id. This function is variadic, meaning it accepts any number of id's.
    /// You can even pass a table!
    /// This function can be slower than the [get_entities_by](#get_entities_by) with the mask parameter filled
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
    /// Deprecated
    /// Use `get_entities_by(0, mask, LAYER.BOTH)` instead
    lua["get_entities_by_mask"] = get_entities_by_mask;
    /// Deprecated
    /// Use `get_entities_by(0, MASK.ANY, layer)` instead
    lua["get_entities_by_layer"] = get_entities_by_layer;

    auto get_entities_at = sol::overload(
        static_cast<std::vector<uint32_t> (*)(ENT_TYPE, uint32_t, float, float, LAYER, float)>(::get_entities_at),
        static_cast<std::vector<uint32_t> (*)(std::vector<ENT_TYPE>, uint32_t, float, float, LAYER, float)>(::get_entities_at));
    /// Get uids of matching entities inside some radius ([ENT_TYPE](#ENT_TYPE), [MASK](#MASK)). Set `entity_type` or `mask` to `0` to ignore that, can also use table of entity_types
    /// Recommended to always set the mask, even if you look for one entity type
    lua["get_entities_at"] = get_entities_at;

    auto get_entities_overlapping = sol::overload(
        static_cast<std::vector<uint32_t> (*)(ENT_TYPE, uint32_t, float, float, float, float, LAYER)>(::get_entities_overlapping),
        static_cast<std::vector<uint32_t> (*)(std::vector<ENT_TYPE>, uint32_t, float, float, float, float, LAYER)>(::get_entities_overlapping));
    /// Deprecated
    /// Use `get_entities_overlapping_hitbox` instead
    lua["get_entities_overlapping"] = get_entities_overlapping;

    auto get_entities_overlapping_hitbox = sol::overload(
        static_cast<std::vector<uint32_t> (*)(ENT_TYPE, uint32_t, AABB, LAYER)>(::get_entities_overlapping_hitbox),
        static_cast<std::vector<uint32_t> (*)(std::vector<ENT_TYPE>, uint32_t, AABB, LAYER)>(::get_entities_overlapping_hitbox));
    /// Get uids of matching entities overlapping with the given hitbox. Set `entity_type` or `mask` to `0` to ignore that, can also use table of entity_types
    lua["get_entities_overlapping_hitbox"] = get_entities_overlapping_hitbox;
    /// Attaches `attachee` to `overlay`, similar to setting `get_entity(attachee).overlay = get_entity(overlay)`.
    /// However this function offsets `attachee` (so you don't have to) and inserts it into `overlay`'s inventory.
    lua["attach_entity"] = attach_entity_by_uid;
    /// Get the `flags` field from entity by uid
    lua["get_entity_flags"] = get_entity_flags;
    /// Set the `flags` field from entity by uid
    lua["set_entity_flags"] = set_entity_flags;
    /// Get the `more_flags` field from entity by uid
    lua["get_entity_flags2"] = get_entity_flags2;
    /// Set the `more_flags` field from entity by uid
    lua["set_entity_flags2"] = set_entity_flags2;
    /// Deprecated
    /// As the name is misleading. use Movable.`move_state` field instead
    lua["get_entity_ai_state"] = get_entity_ai_state;
    /// Get `state.level_flags`
    lua["get_level_flags"] = get_level_flags;
    /// Set `state.level_flags`
    lua["set_level_flags"] = set_level_flags;
    /// Get the ENT_TYPE... of the entity by uid
    lua["get_entity_type"] = get_entity_type;
    /// Get the current set zoom level
    lua["get_zoom_level"] = []() -> float
    {
        auto game_api = GameAPI::get();
        return game_api->get_current_zoom();
    };
    /// Get the game coordinates at the screen position (`x`, `y`)
    lua["game_position"] = [](float x, float y) -> std::pair<float, float>
    { return State::click_position(x, y); };
    /// Translate an entity position to screen position to be used in drawing functions
    lua["screen_position"] = [](float x, float y) -> std::pair<float, float>
    { return State::screen_position(x, y); };
    /// Translate a distance of `x` tiles to screen distance to be be used in drawing functions
    lua["screen_distance"] = screen_distance;
    /// Get position `x, y, layer` of entity by uid. Use this, don't use `Entity.x/y` because those are sometimes just the offset to the entity
    /// you're standing on, not real level coordinates.
    lua["get_position"] = get_position;
    /// Get interpolated render position `x, y, layer` of entity by uid. This gives smooth hitboxes for 144Hz master race etc...
    lua["get_render_position"] = get_render_position;
    /// Get velocity `vx, vy` of an entity by uid. Use this, don't use `Entity.velocityx/velocityy` because those are relative to `Entity.overlay`.
    lua["get_velocity"] = get_velocity;
    /// Remove item by uid from entity
    lua["entity_remove_item"] = entity_remove_item;
    /// Spawns and attaches ball and chain to `uid`, the initial position of the ball is at the entity position plus `off_x`, `off_y`
    lua["attach_ball_and_chain"] = attach_ball_and_chain;
    /// Spawn an entity of `entity_type` attached to some other entity `over_uid`, in offset `x`, `y`
    lua["spawn_entity_over"] = spawn_entity_over;
    /// Short for [spawn_entity_over](#spawn_entity_over)
    lua["spawn_over"] = spawn_entity_over;
    /// Check if the entity `uid` has some specific `item_uid` by uid in their inventory
    lua["entity_has_item_uid"] = entity_has_item_uid;

    auto entity_has_item_type = sol::overload(
        static_cast<bool (*)(uint32_t, ENT_TYPE)>(::entity_has_item_type),
        static_cast<bool (*)(uint32_t, std::vector<ENT_TYPE>)>(::entity_has_item_type));
    /// Check if the entity `uid` has some ENT_TYPE `entity_type` in their inventory, can also use table of entity_types
    lua["entity_has_item_type"] = entity_has_item_type;

    auto entity_get_items_by = sol::overload(
        static_cast<std::vector<uint32_t> (*)(uint32_t, ENT_TYPE, uint32_t)>(::entity_get_items_by),
        static_cast<std::vector<uint32_t> (*)(uint32_t, std::vector<ENT_TYPE>, uint32_t)>(::entity_get_items_by));
    /// Gets uids of entities attached to given entity uid. Use `entity_type` and `mask` ([MASK](#MASK)) to filter, set them to 0 to return all attached entities.
    lua["entity_get_items_by"] = entity_get_items_by;
    /// Kills an entity by uid. `destroy_corpse` defaults to `true`, if you are killing for example a caveman and want the corpse to stay make sure to pass `false`.
    lua["kill_entity"] = kill_entity;
    /// Pick up another entity by uid. Make sure you're not already holding something, or weird stuff will happen.
    lua["pick_up"] = pick_up;
    /// Drop an entity by uid
    lua["drop"] = drop;
    /// Unequips the currently worn backitem
    lua["unequip_backitem"] = unequip_backitem;
    /// Returns the uid of the currently worn backitem, or -1 if wearing nothing
    lua["worn_backitem"] = worn_backitem;
    /// Apply changes made in [get_type](#get_type)() to entity instance by uid.
    lua["apply_entity_db"] = apply_entity_db;
    /// Try to lock the exit at coordinates
    lua["lock_door_at"] = lock_door_at;
    /// Try to unlock the exit at coordinates
    lua["unlock_door_at"] = unlock_door_at;
    /// Get the current frame count since the game was started. You can use this to make some timers yourself, the engine runs at 60fps. This counter is paused if you block PRE_UPDATE from running, and also doesn't increment during some loading screens, even though state update still runs.
    lua["get_frame"] = get_frame_count;
    /// Get the current global frame count since the game was started. You can use this to make some timers yourself, the engine runs at 60fps. This counter keeps incrementing when state is updated, even during loading screens.
    lua["get_global_frame"] = get_global_frame_count;
    /// Get the current timestamp in milliseconds since the Unix Epoch.
    lua["get_ms"] = []()
    { return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(); };
    /// Make `mount_uid` carry `rider_uid` on their back. Only use this with actual mounts and living things.
    lua["carry"] = carry;
    /// Deprecated
    /// Use [replace_drop](#replace_drop)(DROP.ARROWTRAP_WOODENARROW, new_arrow_type) and [replace_drop](#replace_drop)(DROP.POISONEDARROWTRAP_WOODENARROW, new_arrow_type) instead
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
    /// Activate custom variables for speed and distance in the `ITEM_SPARK`
    /// note: because those the variables are custom and game does not initiate them, you need to do it yourself for each spark, recommending `set_post_entity_spawn`
    /// default game values are: speed = -0.015, distance = 3.0
    lua["activate_sparktraps_hack"] = activate_sparktraps_hack;
    /// Set layer to search for storage items on
    lua["set_storage_layer"] = set_storage_layer;
    /// Deprecated
    /// This function never worked properly as too many places in the game individually check for vlads cape and calculate the blood multiplication
    /// `default_multiplier` doesn't do anything due to some changes in last game updates, `vladscape_multiplier` only changes the multiplier to some entities death's blood spit
    lua["set_blood_multiplication"] = set_blood_multiplication;
    /// Flip entity around by uid. All new entities face right by default.
    lua["flip_entity"] = flip_entity;
    /// Sets the Y-level at which Olmec changes phases
    lua["set_olmec_phase_y_level"] = set_olmec_phase_y_level;
    /// Forces Olmec to stay on phase 0 (stomping)
    lua["force_olmec_phase_0"] = force_olmec_phase_0;
    /// Determines when the ghost appears, either when the player is cursed or not
    lua["set_ghost_spawn_times"] = set_ghost_spawn_times;
    /// Determines whether the ghost appears when breaking the ghost pot
    lua["set_cursepot_ghost_enabled"] = set_cursepot_ghost_enabled;
    /// Determines whether the time ghost appears, including the showing of the ghost toast
    lua["set_time_ghost_enabled"] = set_time_ghost_enabled;
    /// Determines whether the time jelly appears in cosmic ocean
    lua["set_time_jelly_enabled"] = set_time_jelly_enabled;
    /// Enables or disables the journal
    lua["set_journal_enabled"] = set_journal_enabled;
    /// Enables or disables the default position based camp camera bounds, to set them manually yourself
    lua["set_camp_camera_bounds_enabled"] = set_camp_camera_bounds_enabled;
    /// Sets which entities are affected by a bomb explosion. Default = MASK.PLAYER | MASK.MOUNT | MASK.MONSTER | MASK.ITEM | MASK.ACTIVEFLOOR | MASK.FLOOR
    lua["set_explosion_mask"] = set_explosion_mask;
    /// Sets the maximum length of a thrown rope (anchor segment not included). Unfortunately, setting this higher than default (6) creates visual glitches in the rope, even though it is fully functional.
    lua["set_max_rope_length"] = set_max_rope_length;
    /// Checks whether a coordinate is inside a room containing an active shop. This function checks whether the shopkeeper is still alive.
    lua["is_inside_active_shop_room"] = is_inside_active_shop_room;
    /// Checks whether a coordinate is inside a shop zone, the rectangle where the camera zooms in a bit. Does not check if the shop is still active!
    lua["is_inside_shop_zone"] = is_inside_shop_zone;
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
    /// Spawn a companion (hired hand, player character, eggplant child)
    lua["spawn_companion"] = spawn_companion;

    /// Calculate the tile distance of two entities by uid
    lua["distance"] = [](uint32_t uid_a, uint32_t uid_b) -> float
    {
        Entity* ea = get_entity_ptr(uid_a);
        Entity* eb = get_entity_ptr(uid_b);
        if (ea == nullptr || eb == nullptr)
            return -1.0f;
        else
            return (float)std::sqrt(std::pow(ea->position().x - eb->position().x, 2) + std::pow(ea->position().y - eb->position().y, 2));
    };
    /// Basically gets the absolute coordinates of the area inside the unbreakable bedrock walls, from wall to wall. Every solid entity should be
    /// inside these boundaries. The order is: left x, top y, right x, bottom y
    lua["get_bounds"] = []() -> std::tuple<float, float, float, float>
    {
        auto state = State::get().ptr();
        return std::make_tuple(2.5f, 122.5f, state->w * 10.0f + 2.5f, 122.5f - state->h * 8.0f);
    };
    /// Same as [get_bounds](#get_bounds) but returns AABB struct instead of loose floats
    lua["get_aabb_bounds"] = []() -> AABB
    {
        auto state = State::get().ptr();
        return {2.5f, 122.5f, state->w * 10.0f + 2.5f, 122.5f - state->h * 8.0f};
    };
    /// Gets the current camera position in the level
    lua["get_camera_position"] = []() -> std::pair<float, float>
    {
        return State::get_camera_position();
    };
    /// Sets the absolute current camera position without rubberbanding animation. Ignores camera bounds or currently focused uid, but doesn't clear them. Best used in ON.RENDER_PRE_GAME or similar. See Camera for proper camera handling with bounds and rubberbanding.
    lua["set_camera_position"] = set_camera_position;
    /// Updates the camera focus according to the params set in Camera, i.e. to apply normal camera movement when paused etc.
    lua["update_camera_position"] = update_camera_position;

    /// Set the nth bit in a number. This doesn't actually change the variable you pass, it just returns the new value you can use.
    lua["set_flag"] = [](Flags flags, int bit) -> Flags
    { return flags | (1U << (bit - 1)); };
    /// Deprecated
    lua["setflag"] = lua["set_flag"];
    /// Clears the nth bit in a number. This doesn't actually change the variable you pass, it just returns the new value you can use.
    lua["clr_flag"] = [](Flags flags, int bit) -> Flags
    { return flags & ~(1U << (bit - 1)); };
    /// Deprecated
    lua["clrflag"] = lua["clr_flag"];
    /// Flips the nth bit in a number. This doesn't actually change the variable you pass, it just returns the new value you can use.
    lua["flip_flag"] = [](Flags flags, int bit) -> Flags
    { return flags ^ (1U << (bit - 1)); };
    /// Returns true if the nth bit is set in the number.
    lua["test_flag"] = [](Flags flags, int bit) -> bool
    { return (flags & (1U << (bit - 1))) > 0; };
    /// Deprecated
    lua["testflag"] = lua["test_flag"];

    /// Set a bitmask in a number. This doesn't actually change the variable you pass, it just returns the new value you can use.
    lua["set_mask"] = [](Flags flags, Flags mask) -> Flags
    { return (flags | mask); };
    /// Clears a bitmask in a number. This doesn't actually change the variable you pass, it just returns the new value you can use.
    lua["clr_mask"] = [](Flags flags, Flags mask) -> Flags
    { return (flags & ~mask); };
    /// Flips the nth bit in a number. This doesn't actually change the variable you pass, it just returns the new value you can use.
    lua["flip_mask"] = [](Flags flags, Flags mask) -> Flags
    { return (flags ^ mask); };
    /// Returns true if a bitmask is set in the number.
    lua["test_mask"] = [](Flags flags, Flags mask) -> bool
    { return (flags & mask) > 0; };

    /// Gets the resolution (width and height) of the screen
    lua["get_window_size"] = []() -> std::tuple<int, int>
    { return {(int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y}; };

    /// Deprecated
    /// Deprecated because it's a weird old hack that crashes the game. You can modify inputs in many other ways, like editing `state.player_inputs.player_slot_1.buttons_gameplay` in PRE_UPDATE or a `set_pre_process_input` hook. Steal input from a Player, HiredHand or PlayerGhost.
    lua["steal_input"] = [](int uid)
    {
        static const auto player_ghost = to_id("ENT_TYPE_ITEM_PLAYERGHOST");
        static const auto ana = to_id("ENT_TYPE_CHAR_ANA_SPELUNKY");
        static const auto egg_child = to_id("ENT_TYPE_CHAR_EGGPLANT_CHILD");

        auto backend = LuaBackend::get_calling_backend();
        if (backend->script_input.find(uid) != backend->script_input.end())
            return;
        Player* player = get_entity_ptr(uid)->as<Player>();
        if (player == nullptr)
            return;

        if (player->type->id == player_ghost)
        {
            auto player_gh = player->as<PlayerGhost>();
            ScriptInput* newinput = new ScriptInput();
            newinput->gameplay = 0;
            newinput->all = 0;
            newinput->orig_input = player_gh->player_inputs;
            player_gh->player_inputs = reinterpret_cast<PlayerSlot*>(newinput);
            backend->script_input[uid] = newinput;
        }
        else
        {
            if (player->type->id < ana || player->type->id > egg_child)
                return;

            ScriptInput* newinput = new ScriptInput();
            newinput->gameplay = 0;
            newinput->all = 0;
            newinput->orig_input = player->input_ptr;
            newinput->orig_ai = player->ai;
            player->input_ptr = reinterpret_cast<PlayerSlot*>(newinput);
            player->ai = nullptr;
            backend->script_input[uid] = newinput;
        }
    };
    /// Deprecated
    /// Return input previously stolen with [steal_input](#steal_input)
    lua["return_input"] = [](int uid)
    {
        static const auto player_ghost = to_id("ENT_TYPE_ITEM_PLAYERGHOST");

        auto backend = LuaBackend::get_calling_backend();
        if (backend->script_input.find(uid) == backend->script_input.end())
            return;
        Player* player = get_entity_ptr(uid)->as<Player>();
        if (player == nullptr)
            return;

        if (player->type->id == player_ghost)
        {
            auto player_gh = player->as<PlayerGhost>();
            player_gh->player_inputs = backend->script_input[uid]->orig_input;
        }
        else
        {
            player->input_ptr = backend->script_input[uid]->orig_input;
            player->ai = backend->script_input[uid]->orig_ai;
        }
        backend->script_input.erase(uid);
    };
    /// Deprecated
    /// Send input to entity, has to be previously stolen with [steal_input](#steal_input)
    lua["send_input"] = [](int uid, INPUTS buttons)
    {
        auto backend = LuaBackend::get_calling_backend();
        auto it = backend->script_input.find(uid);
        if (it != backend->script_input.end())
        {
            it->second->all = buttons;
            it->second->gameplay = buttons;
        }
    };
    /// Deprecated
    /// Use `players[1].input.buttons_gameplay` for only the inputs during the game, or `.buttons` for all the inputs, even during the pause menu
    /// Of course, you can get the Player by other mean, it doesn't need to be the `players` table
    /// You can only read inputs from actual players, HH don't have any inputs
    lua["read_input"] = [](int uid) -> INPUTS
    {
        Player* player = get_entity_ptr(uid)->as<Player>();
        if (player == nullptr)
            return (INPUTS)0;

        if (!IsBadReadPtr(player->input_ptr, 20))
        {
            return player->input_ptr->buttons_gameplay;
        }
        return (INPUTS)0;
    };
    /// Deprecated
    /// Read input that has been previously stolen with [steal_input](#steal_input)
    /// Use `state.player_inputs.player_slots[player_slot].buttons_gameplay` for only the inputs during the game, or `.buttons` for all the inputs, even during the pause menu
    lua["read_stolen_input"] = [](int uid) -> INPUTS
    {
        auto backend = LuaBackend::get_calling_backend();
        if (backend->script_input.find(uid) == backend->script_input.end())
        {
            // this means that the input is attacked to the real input and not stolen so return early
            return (INPUTS)0;
        }
        Player* player = get_entity_ptr(uid)->as<Player>();
        if (player == nullptr)
            return (INPUTS)0;
        ScriptInput* readinput = reinterpret_cast<ScriptInput*>(player->input_ptr);
        if (!IsBadReadPtr(readinput, 20))
        {
            readinput = reinterpret_cast<ScriptInput*>(readinput->orig_input);
            if (!IsBadReadPtr(readinput, 20))
            {
                return readinput->gameplay;
            }
        }
        return (INPUTS)0;
    };

    /// Clears a callback that is specific to a screen.
    lua["clear_screen_callback"] = [](int screen_id, CallbackId cb_id)
    {
        auto backend = LuaBackend::get_calling_backend();
        backend->clear_screen_hooks.push_back({screen_id, cb_id});
    };

    /// Returns unique id for the callback to be used in [clear_screen_callback](#clear_screen_callback) or `nil` if screen_id is not valid.
    /// Sets a callback that is called right before the screen is drawn, return `true` to skip the default rendering.
    /// <br/>The callback signature is bool render_screen(Screen self, VanillaRenderContext render_ctx)
    lua["set_pre_render_screen"] = [](int screen_id, sol::function fun) -> sol::optional<CallbackId>
    {
        if (Screen* screen = get_screen_ptr(screen_id))
        {
            std::uint32_t id = screen->reserve_callback_id();
            screen->set_pre_render(
                id,
                make_safe_clearable_cb<bool(Screen*), CallbackType::Screen>(
                    std::move(fun),
                    screen_id,
                    id,
                    FrontBinder{},
                    BackBinder{[]()
                               { return VanillaRenderContext{}; }}));

            auto backend = LuaBackend::get_calling_backend();
            backend->screen_hooks.push_back({screen_id, id});
            return id;
        }
        return sol::nullopt;
    };
    /// Returns unique id for the callback to be used in [clear_screen_callback](#clear_screen_callback) or `nil` if screen_id is not valid.
    /// Sets a callback that is called right after the screen is drawn.
    /// <br/>The callback signature is nil render_screen(Screen self, VanillaRenderContext render_ctx)
    lua["set_post_render_screen"] = [](int screen_id, sol::function fun) -> sol::optional<CallbackId>
    {
        if (Screen* screen = get_screen_ptr(screen_id))
        {
            std::uint32_t id = screen->reserve_callback_id();
            screen->set_post_render(
                id,
                make_safe_clearable_cb<void(Screen*), CallbackType::Screen>(
                    std::move(fun),
                    screen_id,
                    id,
                    FrontBinder{},
                    BackBinder{[]()
                               { return VanillaRenderContext{}; }}));

            auto backend = LuaBackend::get_calling_backend();
            backend->screen_hooks.push_back({screen_id, id});
            return id;
        }
        return sol::nullopt;
    };

    /// Deprecated
    /// Use `entity.clear_virtual` instead.
    /// Clears a callback that is specific to an entity.
    lua["clear_entity_callback"] = [](int uid, CallbackId cb_id)
    {
        auto backend = LuaBackend::get_calling_backend();
        backend->HookHandler<Entity, CallbackType::Entity>::clear_hook(cb_id, uid);
    };
    /// Deprecated
    /// Use `entity:set_pre_update_state_machine` instead.
    /// Returns unique id for the callback to be used in [clear_entity_callback](#clear_entity_callback) or `nil` if uid is not valid.
    /// `uid` has to be the uid of a `Movable` or else stuff will break.
    /// Sets a callback that is called right before the statemachine, return `true` to skip the statemachine update.
    /// Use this only when no other approach works, this call can be expensive if overused.
    /// Check [here](https://github.com/spelunky-fyi/overlunky/blob/main/docs/virtual-availability.md) to see whether you can use this callback on the entity type you intend to.
    /// <br/>The callback signature is bool statemachine(Entity self)
    lua["set_pre_statemachine"] = [&lua](int uid, sol::function fun) -> sol::optional<CallbackId>
    {
        if (Entity* ent = get_entity_ptr(uid))
        {
            return lua["Entity"]["set_pre_update_state_machine"](ent, std::move(fun));
        }
        return sol::nullopt;
    };
    /// Deprecated
    /// Use `entity:set_post_update_state_machine` instead.
    /// Returns unique id for the callback to be used in [clear_entity_callback](#clear_entity_callback) or `nil` if uid is not valid.
    /// `uid` has to be the uid of a `Movable` or else stuff will break.
    /// Sets a callback that is called right after the statemachine, so you can override any values the satemachine might have set (e.g. `animation_frame`).
    /// Use this only when no other approach works, this call can be expensive if overused.
    /// Check [here](https://github.com/spelunky-fyi/overlunky/blob/main/docs/virtual-availability.md) to see whether you can use this callback on the entity type you intend to.
    /// <br/>The callback signature is nil statemachine(Entity self)
    lua["set_post_statemachine"] = [&lua](int uid, sol::function fun) -> sol::optional<CallbackId>
    {
        if (Entity* ent = get_entity_ptr(uid))
        {
            return lua["Entity"]["set_post_update_state_machine"](ent, std::move(fun));
        }
        return sol::nullopt;
    };
    /// Deprecated
    /// Use `entity:set_pre_destroy` instead.
    /// Returns unique id for the callback to be used in [clear_entity_callback](#clear_entity_callback) or `nil` if uid is not valid.
    /// Sets a callback that is called right when an entity is destroyed, e.g. as if by `Entity.destroy()` before the game applies any side effects.
    /// Use this only when no other approach works, this call can be expensive if overused.
    /// <br/>The callback signature is nil on_destroy(Entity self)
    lua["set_on_destroy"] = [&lua](int uid, sol::function fun) -> sol::optional<CallbackId>
    {
        if (Entity* ent = get_entity_ptr(uid))
        {
            return lua["Entity"]["set_pre_destroy"](ent, std::move(fun));
        }
        return sol::nullopt;
    };
    /// Deprecated
    /// Use `entity:set_pre_kill` instead.
    /// Returns unique id for the callback to be used in [clear_entity_callback](#clear_entity_callback) or `nil` if uid is not valid.
    /// Sets a callback that is called right when an entity is eradicated, before the game applies any side effects.
    /// Use this only when no other approach works, this call can be expensive if overused.
    /// <br/>The callback signature is nil on_kill(Entity self, Entity killer)
    lua["set_on_kill"] = [&lua](int uid, sol::function fun) -> sol::optional<CallbackId>
    {
        if (Entity* ent = get_entity_ptr(uid))
        {
            return lua["Entity"]["set_pre_kill"](ent, std::move(fun));
        }
        return sol::nullopt;
    };
    /// Returns unique id for the callback to be used in [clear_callback](#clear_callback) or `nil` if uid is not valid.
    /// Sets a callback that is called right when an player/hired hand is crushed/insta-gibbed, return `true` to skip the game's crush handling.
    /// The game's instagib function will be forcibly executed (regardless of whatever you return in the callback) when the entity's health is zero.
    /// This is so that when the entity dies (from other causes), the death screen still gets shown.
    /// Use this only when no other approach works, this call can be expensive if overused.
    /// <br/>The callback signature is bool on_player_instagib(Entity self)
    lua["set_on_player_instagib"] = [](int uid, sol::function fun) -> sol::optional<CallbackId>
    {
        if (Entity* ent = get_entity_ptr(uid))
        {
            auto backend = LuaBackend::get_calling_backend();
            backend->pre_entity_instagib_callbacks.push_back(EntityInstagibCallback{backend->cbcount, uid, std::move(fun)});
            return backend->cbcount++;
        }
        return sol::nullopt;
    };
    /// Deprecated
    /// Use `entity:set_pre_damage` instead.
    /// Returns unique id for the callback to be used in [clear_entity_callback](#clear_entity_callback) or `nil` if uid is not valid.
    /// Sets a callback that is called right before an entity is damaged, return `true` to skip the game's damage handling.
    /// Note that damage_dealer can be nil ! (long fall, ...)
    /// DO NOT CALL `self:damage()` in the callback !
    /// Use this only when no other approach works, this call can be expensive if overused.
    /// The entity has to be of a [Movable](#Movable) type.
    /// <br/>The callback signature is bool on_damage(Entity self, Entity damage_dealer, int damage_amount, float vel_x, float vel_y, int stun_amount, int iframes)
    lua["set_on_damage"] = [&lua](int uid, sol::function fun) -> sol::optional<CallbackId>
    {
        if (Entity* ent = get_entity_ptr(uid); ent != nullptr && ent->is_movable())
        {
            return lua["Movable"]["set_pre_damage"](ent, std::move(fun));
        }
        return sol::nullopt;
    };
    /// Deprecated
    /// Use `entity:set_pre_floor_update` instead.
    /// Returns unique id for the callback to be used in [clear_entity_callback](#clear_entity_callback) or `nil` if uid is not valid.
    /// Sets a callback that is called right before a floor is updated (by killed neighbor), return `true` to skip the game's neighbor update handling.
    /// Use this only when no other approach works, this call can be expensive if overused.
    /// <br/>The callback signature is bool pre_floor_update(Entity self)
    lua["set_pre_floor_update"] = [&lua](int uid, sol::function fun) -> sol::optional<CallbackId>
    {
        if (Entity* ent = get_entity_ptr(uid)) // TODO: Requires ent->is_floor
        {
            return lua["Floor"]["set_pre_floor_update"](ent, std::move(fun));
        }
        return sol::nullopt;
    };
    /// Deprecated
    /// Use `entity:set_post_floor_update` instead.
    /// Returns unique id for the callback to be used in [clear_entity_callback](#clear_entity_callback) or `nil` if uid is not valid.
    /// Sets a callback that is called right after a floor is updated (by killed neighbor).
    /// Use this only when no other approach works, this call can be expensive if overused.
    /// <br/>The callback signature is nil post_floor_update(Entity self)
    lua["set_post_floor_update"] = [&lua](int uid, sol::function fun) -> sol::optional<CallbackId>
    {
        if (Entity* ent = get_entity_ptr(uid)) // TODO: Requires ent->is_floor
        {
            return lua["Floor"]["set_post_floor_update"](ent, std::move(fun));
        }
        return sol::nullopt;
    };
    /// Deprecated
    /// Use `entity:set_pre_trigger_action` instead.
    /// Returns unique id for the callback to be used in [clear_entity_callback](#clear_entity_callback) or `nil` if uid is not valid.
    /// Sets a callback that is called right when a container is opened by the player (up+whip)
    /// Use this only when no other approach works, this call can be expensive if overused.
    /// Check [here](https://github.com/spelunky-fyi/overlunky/blob/main/docs/virtual-availability.md) to see whether you can use this callback on the entity type you intend to.
    /// <br/>The callback signature is nil on_open(Entity entity_self, Entity opener)
    lua["set_on_open"] = [&lua](int uid, sol::function fun) -> sol::optional<CallbackId>
    {
        if (Entity* ent = get_entity_ptr(uid)) // TODO: Requires ent->is_container
        {
            return lua["Entity"]["set_pre_trigger_action"](
                ent,
                [fun = std::move(fun)](Entity* usee, Entity* user)
                {
                    if (user->is_movable() && user->as<Movable>()->movey > 0)
                    {
                        auto backend = LuaBackend::get_calling_backend();
                        handle_function<void>(backend.get(), fun, usee, user);
                    }
                });
        }
        return sol::nullopt;
    };
    /// Deprecated
    /// Use `entity:set_pre_collision1` instead.
    /// Returns unique id for the callback to be used in [clear_entity_callback](#clear_entity_callback) or `nil` if uid is not valid.
    /// Sets a callback that is called right before the collision 1 event, return `true` to skip the game's collision handling.
    /// Use this only when no other approach works, this call can be expensive if overused.
    /// Check [here](https://github.com/spelunky-fyi/overlunky/blob/main/docs/virtual-availability.md) to see whether you can use this callback on the entity type you intend to.
    /// <br/>The callback signature is bool pre_collision1(Entity entity_self, Entity collision_entity)
    lua["set_pre_collision1"] = [&lua](int uid, sol::function fun) -> sol::optional<CallbackId>
    {
        if (Entity* ent = get_entity_ptr(uid))
        {
            return lua["Entity"]["set_pre_on_collision1"](ent, std::move(fun));
        }
        return sol::nullopt;
    };
    /// Deprecated
    /// Use `entity:set_pre_collision2` instead.
    /// Returns unique id for the callback to be used in [clear_entity_callback](#clear_entity_callback) or `nil` if uid is not valid.
    /// Sets a callback that is called right before the collision 2 event, return `true` to skip the game's collision handling.
    /// Use this only when no other approach works, this call can be expensive if overused.
    /// Check [here](https://github.com/spelunky-fyi/overlunky/blob/main/docs/virtual-availability.md) to see whether you can use this callback on the entity type you intend to.
    /// <br/>The callback signature is bool pre_collision12(Entity self, Entity collision_entity)
    lua["set_pre_collision2"] = [&lua](int uid, sol::function fun) -> sol::optional<CallbackId>
    {
        if (Entity* ent = get_entity_ptr(uid))
        {
            return lua["Entity"]["set_pre_on_collision2"](ent, std::move(fun));
        }
        return sol::nullopt;
    };
    /// Deprecated
    /// Use `entity.rendering_info:set_pre_render` in combination with `render_info:get_entity` instead.
    /// Returns unique id for the callback to be used in [clear_entity_callback](#clear_entity_callback) or `nil` if uid is not valid.
    /// Sets a callback that is called right after the entity is rendered.
    /// Return `true` to skip the original rendering function and all later pre_render callbacks.
    /// Use this only when no other approach works, this call can be expensive if overused.
    /// <br/>The callback signature is bool render(VanillaRenderContext render_ctx, Entity self)
    lua["set_pre_render"] = [&lua](int uid, sol::function fun) -> sol::optional<CallbackId>
    {
        if (Entity* ent = get_entity_ptr(uid))
        {
            auto backend_id = LuaBackend::get_calling_backend_id();
            return lua["RenderInfo"]["set_pre_render"](
                ent->rendering_info,
                [backend_id, fun = std::move(fun)](RenderInfo* ri, float*, VanillaRenderContext render_ctx)
                {
                    auto backend = LuaBackend::get_backend(backend_id);
                    return handle_function<bool>(
                        backend.get(),
                        fun,
                        render_ctx,
                        ri->get_entity());
                });
        }
        return sol::nullopt;
    };
    /// Deprecated
    /// Use `entity.rendering_info:set_post_render` in combination with `render_info:get_entity` instead.
    /// Returns unique id for the callback to be used in [clear_entity_callback](#clear_entity_callback) or `nil` if uid is not valid.
    /// Sets a callback that is called right after the entity is rendered.
    /// Use this only when no other approach works, this call can be expensive if overused.
    /// <br/>The callback signature is nil post_render(VanillaRenderContext render_ctx, Entity self)
    lua["set_post_render"] = [&lua](int uid, sol::function fun) -> sol::optional<CallbackId>
    {
        if (Entity* ent = get_entity_ptr(uid))
        {
            auto backend_id = LuaBackend::get_calling_backend_id();
            return lua["RenderInfo"]["set_post_render"](
                ent->rendering_info,
                [backend_id, fun = std::move(fun)](RenderInfo* ri, float*, VanillaRenderContext render_ctx)
                {
                    auto backend = LuaBackend::get_backend(backend_id);
                    return handle_function<bool>(
                        backend.get(),
                        fun,
                        render_ctx,
                        ri->get_entity());
                });
        }
        return sol::nullopt;
    };

    /// Raise a signal and probably crash the game
    lua["raise"] = std::raise;

    /// Convert the hash to stringid
    /// Check [strings00_hashed.str](https://github.com/spelunky-fyi/overlunky/blob/main/docs/game_data/strings00_hashed.str) for the hash values, or extract assets with modlunky and check those.
    lua["hash_to_stringid"] = hash_to_stringid;

    /// Get string behind STRINGID, don't use stringid directly for vanilla string, use [hash_to_stringid](#hash_to_stringid) first
    /// Will return the string of currently choosen language
    lua["get_string"] = get_string;

    /// Change string at the given id (don't use stringid diretcly for vanilla string, use `hash_to_stringid` first)
    /// This edits custom string and in game strings but changing the language in settings will reset game strings
    lua["change_string"] = [](STRINGID id, std::u16string str)
    {
        return change_string(id, str);
    };

    /// Add custom string, currently can only be used for names of shop items (EntityDB->description)
    /// Returns STRINGID of the new string
    lua["add_string"] = add_string;

    /// Get localized name of an entity from the journal, pass `fallback_strategy` as `true` to fall back to the `ENT_TYPE.*` enum name
    /// if the entity has no localized name
    lua["get_entity_name"] = [](ENT_TYPE type, sol::optional<bool> fallback_strategy) -> std::u16string
    {
        return get_entity_name(type, fallback_strategy.value_or(false));
    };

    /// Adds custom name to the item by uid used in the shops
    /// This is better alternative to `add_string` but instead of changing the name for entity type, it changes it for this particular entity
    lua["add_custom_name"] = add_custom_name;

    /// Clears the name set with [add_custom_name](#add_custom_name)
    lua["clear_custom_name"] = clear_custom_name;

    /// Calls the enter door function, position doesn't matter, can also enter closed doors (like COG, EW) without unlocking them
    lua["enter_door"] = enter_door;

    /// Change ENT_TYPE's spawned by `FLOOR_SUNCHALLENGE_GENERATOR`, by default there are 4:<br/>
    /// {MONS_WITCHDOCTOR, MONS_VAMPIRE, MONS_SORCERESS, MONS_NECROMANCER}<br/>
    /// Use empty table as argument to reset to the game default
    lua["change_sunchallenge_spawns"] = change_sunchallenge_spawns;

    /// Change ENT_TYPE's spawned in dice shops (Madame Tusk as well), by default there are 25:<br/>
    /// {ITEM_PICKUP_BOMBBAG, ITEM_PICKUP_BOMBBOX, ITEM_PICKUP_ROPEPILE, ITEM_PICKUP_COMPASS, ITEM_PICKUP_PASTE, ITEM_PICKUP_PARACHUTE, ITEM_PURCHASABLE_CAPE, ITEM_PICKUP_SPECTACLES, ITEM_PICKUP_CLIMBINGGLOVES, ITEM_PICKUP_PITCHERSMITT,
    /// ENT_TYPE_ITEM_PICKUP_SPIKESHOES, ENT_TYPE_ITEM_PICKUP_SPRINGSHOES, ITEM_MACHETE, ITEM_BOOMERANG, ITEM_CROSSBOW, ITEM_SHOTGUN, ITEM_FREEZERAY, ITEM_WEBGUN, ITEM_CAMERA, ITEM_MATTOCK, ITEM_PURCHASABLE_JETPACK, ITEM_PURCHASABLE_HOVERPACK,
    /// ITEM_TELEPORTER, ITEM_PURCHASABLE_TELEPORTER_BACKPACK, ITEM_PURCHASABLE_POWERPACK}<br/>
    /// Min 6, Max 255, if you want less then 6 you need to write some of them more then once (they will have higher "spawn chance").
    /// If you use this function in the level with diceshop in it, you have to update `item_ids` in the [ITEM_DICE_PRIZE_DISPENSER](#PrizeDispenser).
    /// Use empty table as argument to reset to the game default
    lua["change_diceshop_prizes"] = change_diceshop_prizes;

    /// Change ENT_TYPE's spawned when you damage the altar, by default there are 6:<br/>
    /// {MONS_BAT, MONS_BEE, MONS_SPIDER, MONS_JIANGSHI, MONS_FEMALE_JIANGSHI, MONS_VAMPIRE}<br/>
    /// Max 255 types.
    /// Use empty table as argument to reset to the game default
    lua["change_altar_damage_spawns"] = change_altar_damage_spawns;

    /// Change ENT_TYPE's spawned when Waddler dies, by default there are 3:<br/>
    /// {ITEM_PICKUP_COMPASS, ITEM_CHEST, ITEM_KEY}<br/>
    /// Max 255 types.
    /// Use empty table as argument to reset to the game default
    lua["change_waddler_drop"] = change_waddler_drop;

    /// Poisons entity, to cure poison set [Movable](#Movable).`poison_tick_timer` to -1
    lua["poison_entity"] = poison_entity;

    /// Change how much health the ankh gives you after death, with every beat (the heart beat effect) it will add `beat_add_health` to your health,
    /// `beat_add_health` has to be divisor of `health` and can't be 0, otherwise the function does nothing. Set `health` to 0 to return to the game defaults
    /// If you set `health` above the game max health it will be forced down to the game max
    lua["modify_ankh_health_gain"] = modify_ankh_health_gain;

    /// Adds entity as shop item, has to be of [Purchasable](#Purchasable) type, check the [entity hierarchy list](https://github.com/spelunky-fyi/overlunky/blob/main/docs/entities-hierarchy.md) to find all the Purchasable entity types.
    /// Adding other entities will result in not obtainable items or game crash
    lua["add_item_to_shop"] = add_item_to_shop;

    /// Change the amount of frames after the damage from poison is applied
    lua["change_poison_timer"] = change_poison_timer;

    auto create_illumination = sol::overload(
        static_cast<Illumination* (*)(Color, float, float, float)>(::create_illumination),
        static_cast<Illumination* (*)(Color, float, int32_t)>(::create_illumination),
        static_cast<Illumination* (*)(Vec2, Color, LIGHT_TYPE, float, uint8_t, int32_t, LAYER)>(::create_illumination));
    /// Creates a new Illumination. Don't forget to continuously call [refresh_illumination](#refresh_illumination), otherwise your light emitter fades out! Check out the [illumination.lua](https://github.com/spelunky-fyi/overlunky/blob/main/examples/illumination.lua) script for an example.
    lua["create_illumination"] = create_illumination;
    /// Refreshes an Illumination, keeps it from fading out (updates the timer, keeping it in sync with the game render)
    lua["refresh_illumination"] = refresh_illumination;

    /// Removes all liquid that is about to go out of bounds, this would normally crash the game, but playlunky/overlunky patch this bug.
    /// The patch however does not destroy the liquids that fall pass the level bounds,
    /// so you may still want to use this function if you spawn a lot of liquid that may fall out of the level
    lua["fix_liquid_out_of_bounds"] = fix_liquid_out_of_bounds;

    /// Return the name of the first matching number in an enum table
    // lua["enum_get_name"] = [](table enum, int value) -> string
    lua["enum_get_name"] = lua.safe_script(R"(
        return function(enum, value)
            for k,v in pairs(enum) do
                if v == value then return k end
            end
        end
    )");

    /// Return all the names of a number in an enum table
    // lua["enum_get_names"] = [](table enum, int value) -> table<string>
    lua["enum_get_names"] = lua.safe_script(R"(
        return function(enum, value)
            local list = {}
            for k,v in pairs(enum) do
                if v == value then list[#list+1] = k end
            end
            return list
        end
    )");

    /// Return the matching names for a bitmask in an enum table of masks
    // lua["enum_get_mask_names"] = [](table enum, int value) -> table<string>
    lua["enum_get_mask_names"] = lua.safe_script(R"(
        return function(enum, mask)
            local list = {}
            for k,v in pairs(enum) do
                if test_mask(mask, v) then list[#list+1] = k end
            end
            return list
        end
    )");

    /// Paramater to set_setting
    lua.create_named_table("SAFE_SETTING", "PET_STYLE", 20, "SCREEN_SHAKE", 21, "HUD_STYLE", 23, "HUD_SIZE", 24, "LEVEL_TIMER", 25, "TIMER_DETAIL", 26, "LEVEL_NUMBER", 27, "ANGRY_SHOPKEEPER", 28, "BUTTON_PROMPTS", 30, "FEAT_POPUPS", 32, "TEXTBOX_SIZE", 33, "TEXTBOX_DURATION", 34, "TEXTBOX_OPACITY", 35, "LEVEL_FEELINGS", 36, "DIALOG_TEXT", 37, "KALI_TEXT", 38, "GHOST_TEXT", 39);

    /// Gets the specified setting, values might need to be interpreted differently per setting
    lua["get_setting"] = get_setting;

    /// Sets the specified setting temporarily. These values are not saved and might reset to the users real settings if they visit the options menu. (Check example.) All settings are available in unsafe mode and only a smaller subset SAFE_SETTING by default for Hud and other visuals. Returns false, if setting failed.
    // lua["set_setting"] = set_setting;
    /// NoDoc
    lua["set_setting"] = [](GAME_SETTING setting, std::uint32_t value)
    {
        auto backend = LuaBackend::get_calling_backend();
        bool is_safe = std::find(std::begin(safe_settings), std::end(safe_settings), setting) != std::end(safe_settings);

        if (backend->get_unsafe() || is_safe)
        {
            save_original_setting(setting);
            return set_setting(setting, value);
        }
        return false;
    };

    /// Short for print(string.format(...))
    lua["printf"] = lua.safe_script(R"(
        return function(...)
            local out = string.format(...)
            print(out)
            return out
        end
    )");

    /// Spawn a Shopkeeper in the coordinates and make the room their shop. Returns the Shopkeeper uid. Also see [spawn_roomowner](#spawn_roomowner).
    // lua["spawn_shopkeeper"] = [](float x, float y, LAYER layer, ROOM_TEMPLATE room_template = ROOM_TEMPLATE.SHOP) -> uint32_t
    lua["spawn_shopkeeper"] = sol::overload(
        [](float x, float y, LAYER layer)
        {
            return spawn_shopkeeper(x, y, layer);
        },
        [](float x, float y, LAYER layer, ROOM_TEMPLATE room_template)
        {
            return spawn_shopkeeper(x, y, layer, room_template);
        });

    /// Spawn a RoomOwner (or a few other like [CavemanShopkeeper](#CavemanShopkeeper)) in the coordinates and make them own the room, optionally changing the room template. Returns the RoomOwner uid.
    // lua["spawn_roomowner"] = [](ENT_TYPE owner_type, float x, float y, LAYER layer, ROOM_TEMPLATE room_template = -1) -> uint32_t
    lua["spawn_roomowner"] = sol::overload(
        [](ENT_TYPE owner_type, float x, float y, LAYER layer)
        {
            return spawn_roomowner(owner_type, x, y, layer);
        },
        [](ENT_TYPE owner_type, float x, float y, LAYER layer, int16_t room_template)
        {
            return spawn_roomowner(owner_type, x, y, layer, room_template);
        });

    /// Get the current adventure seed pair, or optionally what it was at the start of this run, because it changes every level.
    lua["get_adventure_seed"] = get_adventure_seed;
    /// Set the current adventure seed pair. Use just before resetting a run to recreate an adventure run.
    lua["set_adventure_seed"] = set_adventure_seed;
    /// Updates the floor collisions used by the liquids, set add to false to remove tile of collision, set to true to add one
    /// optional `layer` parameter to be used when liquid was moved to back layer using [set_liquid_layer](#set_liquid_layer)
    lua["update_liquid_collision_at"] = update_liquid_collision_at;

    /// Disable all crust item spawns, returns whether they were already disabled before the call
    lua["disable_floor_embeds"] = disable_floor_embeds;

    /// Get the rva for a pattern name, used for debugging.
    lua["get_rva"] = [](std::string_view address_name) -> std::string
    {
        return fmt::format("{:X}", get_address(address_name) - Memory::get().at_exe(0));
    };

    /// Get the rva for a vtable offset and index, used for debugging.
    lua["get_virtual_rva"] = [](VTABLE_OFFSET offset, uint32_t index) -> std::string
    {
        return fmt::format("{:X}", get_virtual_function_address(offset, index));
    };

    /// Get memory address from a lua object
    lua["get_address"] = [&lua]([[maybe_unused]] sol::object o)
    {
        return fmt::format("{:X}", *(size_t*)lua_touserdata(lua, 1));
    };

    /// Log to spelunky.log
    lua["log_print"] = game_log;

    /// Immediately ends the run with the death screen, also calls the [save_progress](#save_progress)
    lua["load_death_screen"] = load_death_screen;

    /// Saves the game to savegame.sav, unless game saves are blocked in the settings. Also runs the ON.SAVE callback. Fails and returns false, if you're trying to save too often (2s).
    lua["save_progress"] = []() -> bool
    {
        auto backend = LuaBackend::get_calling_backend();
        if (backend->last_save <= State::get().ptr()->time_startup - 120)
        {
            backend->manual_save = true;
            save_progress();
            return true;
        }
        return false;
    };

    /// Runs the ON.SAVE callback. Fails and returns false, if you're trying to save too often (2s).
    lua["save_script"] = []() -> bool
    {
        auto backend = LuaBackend::get_calling_backend();
        if (backend->last_save <= get_frame_count() - 120)
        {
            backend->manual_save = true;
            return true;
        }
        return false;
    };

    /// Set the level number shown in the hud and journal to any string. This is reset to the default "%d-%d" automatically just before PRE_LOAD_SCREEN to a level or main menu, so use in PRE_LOAD_SCREEN, POST_LEVEL_GENERATION or similar for each level.
    /// Use "%d-%d" to reset to default manually. Does not affect the "...COMPLETED!" message in transitions or lines in "Dear Journal", you need to edit them separately with [change_string](#change_string).
    lua["set_level_string"] = [](std::u16string str)
    {
        return set_level_string(str);
    };

    /// Force the character unlocked in either ending to ENT_TYPE. Set to 0 to reset to the default guys. Does not affect the texture of the actual savior. (See example)
    lua["set_ending_unlock"] = set_ending_unlock;

    /// Get the thread-local version of state
    lua["get_local_state"] = []() -> StateMemory*
    {
        return State::get().ptr_local();
    };

    /// Get the thread-local version of players
    lua["get_local_players"] = []() -> std::vector<Player*>
    {
        return get_players(State::get().ptr_local());
    };

    /// List files in directory relative to the script root. Returns table of file/directory names or nil if not found.
    lua["list_dir"] = [&lua](std::optional<std::string> dir)
    {
        std::vector<std::string> files;
        auto backend = LuaBackend::get_calling_backend();
        const auto& base = backend->get_root_path();
        auto path = base / std::filesystem::path(dir.value_or("."));
        if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path))
            return sol::make_object(lua, sol::lua_nil);
        auto base_check = std::filesystem::relative(path, base).string();
        if (base_check.starts_with("..") && !backend->get_unsafe())
        {
            luaL_error(lua, "Tried to list parent directory without unsafe mode.");
            return sol::make_object(lua, sol::lua_nil);
        }
        for (const auto& file : std::filesystem::directory_iterator(path))
        {
            auto str = std::filesystem::relative(file.path(), base).string();
            std::replace(str.begin(), str.end(), '\\', '/');
            if (std::filesystem::is_directory(file.path()))
                str = str + "/";
            files.push_back(str);
        }
        return sol::make_object(lua, sol::as_table(files));
    };

    /// List files in directory relative to the mods data directory (Mods/Data/...). Returns table of file/directory names or nil if not found.
    lua["list_data_dir"] = [&lua](std::optional<std::string> dir)
    {
        std::vector<std::string> files;
        auto backend = LuaBackend::get_calling_backend();
        auto is_pack = check_safe_io_path(backend->get_path(), "Mods/Packs");
        auto is_safe = !backend->get_unsafe();
        std::string moddir = backend->get_root_path().filename().string();
        std::string luafile = std::filesystem::path(backend->get_path()).filename().string();
        std::string datadir = "Mods/Data/" + (is_pack ? moddir : luafile);
        auto base = std::filesystem::path(datadir);
        std::string fullpath = datadir + "/" + dir.value_or(".");
        std::string dirpath = std::filesystem::path(fullpath).parent_path().string();
        auto is_based = check_safe_io_path(fullpath, datadir);
        DEBUG("list_data_dir: safe:{} pack:{} based:{} {}", is_safe, is_pack, is_based, fullpath);
        if (is_safe && !is_based)
        {
            luaL_error(lua, "Tried to list files outside data directory");
            return sol::make_object(lua, sol::lua_nil);
        }
        auto path = std::filesystem::path(fullpath);
        if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path))
            return sol::make_object(lua, sol::lua_nil);
        for (const auto& file : std::filesystem::directory_iterator(path))
        {
            auto str = std::filesystem::relative(file.path(), base).string();
            std::replace(str.begin(), str.end(), '\\', '/');
            if (std::filesystem::is_directory(file.path()))
                str = str + "/";
            files.push_back(str);
        }
        return sol::make_object(lua, sol::as_table(files));
    };

    /// List all char_*.png files recursively from Mods/Packs. Returns table of file paths.
    lua["list_char_mods"] = [&lua]()
    {
        std::vector<std::string> files;
        auto backend = LuaBackend::get_calling_backend();
        auto path = std::filesystem::path("Mods/Packs");
        auto base = std::filesystem::path(".");
        if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path))
            return sol::make_object(lua, sol::lua_nil);
        for (const auto& file : std::filesystem::recursive_directory_iterator(path))
        {
            auto str = std::filesystem::relative(file.path(), base).string();
            std::replace(str.begin(), str.end(), '\\', '/');
            if (str.find("/.db") != std::string::npos || str.find("char_") == std::string::npos || !str.ends_with(".png") || str.ends_with("_col.png") || str.ends_with("_lumin.png"))
                continue;
            str = "/" + str;
            files.push_back(str);
        }
        return sol::make_object(lua, sol::as_table(files));
    };

    /// Approximate bounding box of the player hud element for player index 1..4 based on user settings and player count
    lua["get_hud_position"] = [](int index) -> AABB
    {
        index--;
        float ax = -0.98f;
        float f = 1.0f;
        uint32_t hs = get_setting(GAME_SETTING::HUD_SIZE).value_or(0);
        if (hs == 0 || State::get().ptr()->items->player_count > 3)
            f = 1.0f;
        else if (hs == 1 || State::get().ptr()->items->player_count > 2)
            f = 1.15f;
        else
            f = 1.3f;
        float w = 0.32f * f;

        float ay = 0.94f - (1.0f - f) * 0.1f;
        float h = 0.2f * f;

        return AABB(ax + index * w + 0.02f * f, ay, ax + index * w + w - 0.02f * f, ay - h);
    };

    /// Olmec cutscene moves Olmec and destroys the four floor tiles, so those things never happen if the cutscene is disabled, and Olmec will spawn on even ground. More useful for level gen mods, where the cutscene doesn't make sense. You can also set olmec_cutscene.timer to the last frame (809) to skip to the end, with Olmec in the hole.
    lua["set_olmec_cutscene_enabled"] = set_olmec_cutscene_enabled;

    /// Tiamat cutscene is also responsible for locking the exit door, so you may need to close it yourself if you still want Tiamat kill to be required
    lua["set_tiamat_cutscene_enabled"] = set_tiamat_cutscene_enabled;

    /// Activate custom variables for position used for detecting the player (normally hardcoded)
    /// note: because those variables are custom and game does not initiate them, you need to do it yourself for each Tiamat entity, recommending set_post_entity_spawn
    /// default game values are: attack_x = 17.5 attack_y = 62.5
    lua["activate_tiamat_position_hack"] = activate_tiamat_position_hack;

    /// Activate custom variables for speed and y coordinate limit for crushing elevator
    /// note: because those variables are custom and game does not initiate them, you need to do it yourself for each CrushElevator entity, recommending set_post_entity_spawn
    /// default game values are: speed = 0.0125, y_limit = 98.5
    lua["activate_crush_elevator_hack"] = activate_crush_elevator_hack;

    /// Activate custom variables for y coordinate limit for hundun and spawn of it's heads
    /// note: because those variables are custom and game does not initiate them, you need to do it yourself for each Hundun entity, recommending set_post_entity_spawn
    /// default game value are: y_limit = 98.5, rising_speed_x = 0, rising_speed_y = 0.0125, bird_head_spawn_y = 55, snake_head_spawn_y = 71
    lua["activate_hundun_hack"] = activate_hundun_hack;

    /// Allows you to disable the control over the door for Hundun and Tiamat
    /// This will also prevent game crashing when there is no exit door when they are in level
    lua["set_boss_door_control_enabled"] = set_boss_door_control_enabled;

    /// Run state update manually, i.e. simulate one logic frame. Use in e.g. POST_UPDATE, but be mindful of infinite loops, this will cause another POST_UPDATE. Can even be called thousands of times to simulate minutes of gameplay in a few seconds.
    lua["update_state"] = update_state;

    /// Set engine target frametime (1/framerate, default 1/60). Always capped by your GPU max FPS / VSync. To run the engine faster than rendered FPS, try update_state. Set to 0 to go as fast as possible. Call without arguments to reset. Also see set_speedhack
    lua["set_frametime"] = set_frametime;

    /// Get engine target frametime (1/framerate, default 1/60).
    lua["get_frametime"] = get_frametime;

    /// Set engine target frametime when game is unfocused (1/framerate, default 1/33). Always capped by the engine frametime. Set to 0 to go as fast as possible. Call without arguments to reset.
    lua["set_frametime_unfocused"] = set_frametime_inactive;

    /// Get engine target frametime when game is unfocused (1/framerate, default 1/33).
    lua["get_frametime_unfocused"] = get_frametime_inactive;

    auto add_custom_type = sol::overload(
        static_cast<ENT_TYPE (*)(std::vector<ENT_TYPE>)>(::add_custom_type),
        static_cast<ENT_TYPE (*)()>(::add_custom_type));

    /// Adds new custom type (group of ENT_TYPE) that can be later used in functions like get_entities_by or set_(pre/post)_entity_spawn
    /// Use empty array or no parameter to get new unique ENT_TYPE that can be used for custom EntityDB
    lua["add_custom_type"] = add_custom_type;

    auto get_entities_by_draw_depth = sol::overload(
        static_cast<std::vector<uint32_t> (*)(uint8_t, LAYER)>(::get_entities_by_draw_depth),
        static_cast<std::vector<uint32_t> (*)(std::vector<uint8_t>, LAYER)>(::get_entities_by_draw_depth));

    /// Get uids of entities by draw_depth. Can also use table of draw_depths.
    /// You can later use [filter_entities](#filter_entities) if you want specific entity
    lua["get_entities_by_draw_depth"] = get_entities_by_draw_depth;

    /// Just convenient way of getting the current amount of money
    /// short for state->money_shop_total + loop[inventory.money + inventory.collected_money_total]
    lua["get_current_money"] = get_current_money;

    /// Adds money to the state.money_shop_total and displays the effect on the HUD for money change
    /// Can be negative, default display_time = 60 (about 2s). Returns the current money after the transaction
    lua["add_money"] = add_money;

    /// Adds money to the state.items.player_inventory[player_slot].money and displays the effect on the HUD for money change
    /// Can be negative, default display_time = 60 (about 2s). Returns the current money after the transaction
    lua["add_money_slot"] = add_money_slot;

    /// Destroys all layers and all entities in the level. Usually a bad idea, unless you also call create_level and spawn the player back in.
    lua["destroy_level"] = destroy_level;

    /// Destroys a layer and all entities in it.
    lua["destroy_layer"] = destroy_layer;

    /// Initializes an empty front and back layer that don't currently exist. Does nothing(?) if layers already exist.
    lua["create_level"] = create_level;

    /// Initializes an empty layer that doesn't currently exist.
    lua["create_layer"] = create_layer;

    /// Setting to false disables all player logic in SCREEN.LEVEL, mainly the death screen from popping up if all players are dead or missing, but also shop camera zoom and some other small things.
    lua["set_level_logic_enabled"] = set_level_logic_enabled;

    /// Setting to true will stop the state update from unpausing after a screen load, leaving you with state.pause == PAUSE.FADE on the first frame to do what you want.
    lua["set_start_level_paused"] = set_start_level_paused;

    /// Returns true if the level pause hack is enabled
    lua["get_start_level_paused"] = get_start_level_paused;

    /// Converts INPUTS to (x, y, BUTTON)
    lua["inputs_to_buttons"] = [](INPUTS inputs) -> std::tuple<float, float, BUTTON>
    {
        float x = 0;
        float y = 0;
        if (inputs & 0x100)
            x = -1;
        else if (inputs & 0x200)
            x = 1;
        if (inputs & 0x400)
            y = 1;
        else if (inputs & 0x800)
            y = -1;
        BUTTON buttons = (BUTTON)(inputs & 0x3f);
        return std::make_tuple(x, y, buttons);
    };

    /// Converts (x, y, BUTTON) to INPUTS
    lua["buttons_to_inputs"] = [](float x, float y, BUTTON buttons) -> INPUTS
    {
        INPUTS inputs = buttons;
        if (x < 0)
            inputs |= 0x100;
        else if (x > 0)
            inputs |= 0x200;
        if (y > 0)
            inputs |= 0x400;
        else if (y < 0)
            inputs |= 0x800;
        return inputs;
    };

    /// Disable the Infinite Loop Detection of 420 million instructions per frame, if you know what you're doing and need to perform some serious calculations that hang the game updates for several seconds.
    lua["set_infinite_loop_detection_enabled"] = [](bool enable)
    {
        auto backend = LuaBackend::get_calling_backend();
        backend->infinite_loop_detection = enable;
    };

    /// This disables the `state.camera_layer` to be forced to the `(leader player).layer` and setting of the `state.layer_transition_timer` & `state.transition_to_layer` when player enters layer door.
    /// Letting you control those manually.
    /// Look at the example on how to mimic game layer switching behavior
    lua["set_camera_layer_control_enabled"] = set_camera_layer_control_enabled;

    /// Set multiplier (default 1.0) for a QueryPerformanceCounter hook based speedhack, similar to the one in Cheat Engine. Call without arguments to reset. Also see set_frametime
    lua["set_speedhack"] = set_speedhack;

    /// Get the current speedhack multiplier
    lua["get_speedhack"] = get_speedhack;

    /// Retrieves the current value of the performance counter, which is a high resolution (<1us) time stamp that can be used for time-interval measurements.
    lua["get_performance_counter"] = []() -> int64_t
    {
        LARGE_INTEGER ret;
        if (QueryPerformanceCounter(&ret))
            return ret.QuadPart;
        return 0;
    };

    /// Retrieves the frequency of the performance counter. The frequency of the performance counter is fixed at system boot and is consistent across all processors. Therefore, the frequency need only be queried upon application initialization, and the result can be cached.
    lua["get_performance_frequency"] = []() -> int64_t
    {
        LARGE_INTEGER ret;
        if (QueryPerformanceFrequency(&ret))
            return ret.QuadPart;
        return 0;
    };

    /// Initializes some adventure run related values and loads the character select screen, as if starting a new adventure run from the Play menu. Character select can be skipped by changing `state.screen_next` right after calling this function, maybe with `warp()`. If player isn't already selected, make sure to set `state.items.player_select` and `state.items.player_count` appropriately too.
    lua["play_adventure"] = init_adventure;

    /// Initializes some seedeed run related values and loads the character select screen, as if starting a new seeded run after entering the seed.
    lua["play_seeded"] = init_seeded;

    /// Change layer at which the liquid spawns in, THIS FUNCTION NEEDS TO BE CALLED BEFORE THE LEVEL IS BUILD, otherwise collisions and other stuff will be wrong for the newly spawned liquid
    /// This sadly also makes lavamanders extinct, since the logic for their spawn is hardcoded to front layer with bunch of other unrelated stuff (you can still spawn them with script or place them directly in level files)
    /// Everything should be working more or less correctly (report on community discord if you find something unusual)
    lua["set_liquid_layer"] = set_liquid_layer;

    /// Get the current layer that the liquid is spawn in. Related function [set_liquid_layer](#set_liquid_layer)
    lua["get_liquid_layer"] = get_liquid_layer;

    /// Attach liquid collision to entity by uid (this is what the push blocks use)
    /// Collision is based on the entity's hitbox, collision is removed when the entity is destroyed (bodies of killed entities will still have the collision)
    /// Use only for entities that can move around, (for static prefer [update_liquid_collision_at](#update_liquid_collision_at) )
    /// If entity is in back layer and liquid in the front, there will be no collision created, also collision is not destroyed when entity changes layers, so you have to handle that yourself
    lua["add_entity_to_liquid_collision"] = add_entity_to_liquid_collision;

    lua.create_named_table("INPUTS", "NONE", 0x0, "JUMP", 0x1, "WHIP", 0x2, "BOMB", 0x4, "ROPE", 0x8, "RUN", 0x10, "DOOR", 0x20, "MENU", 0x40, "JOURNAL", 0x80, "LEFT", 0x100, "RIGHT", 0x200, "UP", 0x400, "DOWN", 0x800);

    lua.create_named_table("MENU_INPUT", "NONE", 0x0, "SELECT", 0x1, "BACK", 0x2, "DELETE", 0x4, "RANDOM", 0x8, "JOURNAL", 0x10, "LEFT", 0x20, "RIGHT", 0x40, "UP", 0x80, "DOWN", 0x100);

    lua.create_named_table(
        "ON",
        "LOGO",
        ON::LOGO,
        "INTRO",
        ON::INTRO,
        "PROLOGUE",
        ON::PROLOGUE,
        "TITLE",
        ON::TITLE,
        "MENU",
        ON::MENU,
        "OPTIONS",
        ON::OPTIONS,
        "PLAYER_PROFILE",
        ON::PLAYER_PROFILE,
        "LEADERBOARD",
        ON::LEADERBOARD,
        "SEED_INPUT",
        ON::SEED_INPUT,
        "CHARACTER_SELECT",
        ON::CHARACTER_SELECT,
        "TEAM_SELECT",
        ON::TEAM_SELECT,
        "CAMP",
        ON::CAMP,
        "LEVEL",
        ON::LEVEL,
        "TRANSITION",
        ON::TRANSITION,
        "DEATH",
        ON::DEATH,
        "SPACESHIP",
        ON::SPACESHIP,
        "WIN",
        ON::WIN,
        "CREDITS",
        ON::CREDITS,
        "SCORES",
        ON::SCORES,
        "CONSTELLATION",
        ON::CONSTELLATION,
        "RECAP",
        ON::RECAP,
        "ARENA_MENU",
        ON::ARENA_MENU,
        "ARENA_STAGES",
        ON::ARENA_STAGES,
        "ARENA_ITEMS",
        ON::ARENA_ITEMS,
        "ARENA_SELECT",
        ON::ARENA_SELECT,
        "ARENA_INTRO",
        ON::ARENA_INTRO,
        "ARENA_MATCH",
        ON::ARENA_MATCH,
        "ARENA_SCORE",
        ON::ARENA_SCORE,
        "ONLINE_LOADING",
        ON::ONLINE_LOADING,
        "ONLINE_LOBBY",
        ON::ONLINE_LOBBY,

        "GUIFRAME",
        ON::GUIFRAME,
        "FRAME",
        ON::FRAME,
        "GAMEFRAME",
        ON::GAMEFRAME,
        "SCREEN",
        ON::SCREEN,
        "START",
        ON::START,
        "LOADING",
        ON::LOADING,
        "RESET",
        ON::RESET,
        "SAVE",
        ON::SAVE,
        "LOAD",
        ON::LOAD,
        "PRE_LOAD_LEVEL_FILES",
        ON::PRE_LOAD_LEVEL_FILES,
        "PRE_LEVEL_GENERATION",
        ON::PRE_LEVEL_GENERATION,
        "PRE_LOAD_SCREEN",
        ON::PRE_LOAD_SCREEN,
        "POST_ROOM_GENERATION",
        ON::POST_ROOM_GENERATION,
        "POST_LEVEL_GENERATION",
        ON::POST_LEVEL_GENERATION,
        "POST_LOAD_SCREEN",
        ON::POST_LOAD_SCREEN,
        "PRE_GET_RANDOM_ROOM",
        ON::PRE_GET_RANDOM_ROOM,
        "PRE_HANDLE_ROOM_TILES",
        ON::PRE_HANDLE_ROOM_TILES,
        "SCRIPT_ENABLE",
        ON::SCRIPT_ENABLE,
        "SCRIPT_DISABLE",
        ON::SCRIPT_DISABLE,
        "RENDER_PRE_HUD",
        ON::RENDER_PRE_HUD,
        "RENDER_POST_HUD",
        ON::RENDER_POST_HUD,
        "RENDER_PRE_PAUSE_MENU",
        ON::RENDER_PRE_PAUSE_MENU,
        "RENDER_POST_PAUSE_MENU",
        ON::RENDER_POST_PAUSE_MENU,
        "RENDER_PRE_BLURRED_BACKGROUND",
        ON::RENDER_PRE_BLURRED_BACKGROUND,
        "RENDER_POST_BLURRED_BACKGROUND",
        ON::RENDER_POST_BLURRED_BACKGROUND,
        "RENDER_PRE_DRAW_DEPTH",
        ON::RENDER_PRE_DRAW_DEPTH,
        "RENDER_POST_DRAW_DEPTH",
        ON::RENDER_POST_DRAW_DEPTH,
        "RENDER_PRE_JOURNAL_PAGE",
        ON::RENDER_PRE_JOURNAL_PAGE,
        "RENDER_POST_JOURNAL_PAGE",
        ON::RENDER_POST_JOURNAL_PAGE,
        "RENDER_PRE_LAYER",
        ON::RENDER_PRE_LAYER,
        "RENDER_POST_LAYER",
        ON::RENDER_POST_LAYER,
        "RENDER_PRE_LEVEL",
        ON::RENDER_PRE_LEVEL,
        "RENDER_POST_LEVEL",
        ON::RENDER_POST_LEVEL,
        "RENDER_PRE_GAME",
        ON::RENDER_PRE_GAME,
        "RENDER_POST_GAME",
        ON::RENDER_POST_GAME,
        "SPEECH_BUBBLE",
        ON::SPEECH_BUBBLE,
        "TOAST",
        ON::TOAST,
        "DEATH_MESSAGE",
        ON::DEATH_MESSAGE,
        "PRE_LOAD_JOURNAL_CHAPTER",
        ON::PRE_LOAD_JOURNAL_CHAPTER,
        "POST_LOAD_JOURNAL_CHAPTER",
        ON::POST_LOAD_JOURNAL_CHAPTER,
        "PRE_GET_FEAT",
        ON::PRE_GET_FEAT,
        "PRE_SET_FEAT",
        ON::PRE_SET_FEAT,
        "PRE_UPDATE",
        ON::PRE_UPDATE,
        "POST_UPDATE",
        ON::POST_UPDATE,
        "USER_DATA",
        ON::USER_DATA,
        "PRE_LEVEL_CREATION",
        ON::PRE_LEVEL_CREATION,
        "POST_LEVEL_CREATION",
        ON::POST_LEVEL_CREATION,
        "PRE_LAYER_CREATION",
        ON::PRE_LAYER_CREATION,
        "POST_LAYER_CREATION",
        ON::POST_LAYER_CREATION,
        "PRE_LEVEL_DESTRUCTION",
        ON::PRE_LEVEL_DESTRUCTION,
        "POST_LEVEL_DESTRUCTION",
        ON::POST_LEVEL_DESTRUCTION,
        "PRE_LAYER_DESTRUCTION",
        ON::PRE_LAYER_DESTRUCTION,
        "POST_LAYER_DESTRUCTION",
        ON::POST_LAYER_DESTRUCTION,
        "PRE_PROCESS_INPUT",
        ON::PRE_PROCESS_INPUT,
        "POST_PROCESS_INPUT",
        ON::POST_PROCESS_INPUT,
        "PRE_GAME_LOOP",
        ON::PRE_GAME_LOOP,
        "POST_GAME_LOOP",
        ON::POST_GAME_LOOP,
        "PRE_SAVE_STATE",
        ON::PRE_SAVE_STATE,
        "POST_SAVE_STATE",
        ON::POST_SAVE_STATE,
        "PRE_LOAD_STATE",
        ON::PRE_LOAD_STATE,
        "POST_LOAD_STATE",
        ON::POST_LOAD_STATE,
        "BLOCKED_UPDATE",
        ON::BLOCKED_UPDATE,
        "BLOCKED_GAME_LOOP",
        ON::BLOCKED_GAME_LOOP,
        "BLOCKED_PROCESS_INPUT",
        ON::BLOCKED_PROCESS_INPUT);

    /* ON
    // LOGO
    // Runs when entering the the mossmouth logo screen.
    // INTRO
    // Runs when entering the intro cutscene.
    // PROLOGUE
    // Runs when entering the prologue / poem.
    // TITLE
    // Runs when entering the title screen.
    // MENU
    // Runs when entering the main menu.
    // OPTIONS
    // Runs when entering the options menu.
    // PLAYER_PROFILE
    // Runs when entering the player profile screen.
    // LEADERBOARD
    // Runs when entering the leaderboard screen.
    // SEED_INPUT
    // Runs when entering the seed input screen of a seeded run.
    // CHARACTER_SELECT
    // Runs when entering the character select screen.
    // TEAM_SELECT
    // Runs when entering the team select screen of arena mode.
    // CAMP
    // Runs when entering the camp, after all entities have spawned, on the first level frame.
    // LEVEL
    // Runs when entering any level, after all entities have spawned, on the first level frame.
    // TRANSITION
    // Runs when entering the transition screen, after all entities have spawned.
    // DEATH
    // Runs when entering the death screen.
    // SPACESHIP
    // Runs when entering the olmecship cutscene after Tiamat.
    // WIN
    // Runs when entering any winning cutscene, including the constellation.
    // CREDITS
    // Runs when entering the credits screen.
    // SCORES
    // Runs when entering the final score celebration screen of a normal or hard ending.
    // CONSTELLATION
    // Runs when entering the turning into constellation cutscene after cosmic ocean.
    // RECAP
    // Runs when entering the Dear Journal screen after final scores.
    // ARENA_MENU
    // Runs when entering the main arena rules menu screen.
    // ARENA_STAGES
    // Runs when entering the arena stage selection screen.
    // ARENA_ITEMS
    // Runs when entering the arena item config screen.
    // ARENA_INTRO
    // Runs when entering the arena VS intro screen.
    // ARENA_MATCH
    // Runs when entering the arena level screen, after all entities have spawned, on the first level frame, before the get ready go scene.
    // ARENA_SCORE
    // Runs when entering the arena scores screen.
    // ONLINE_LOADING
    // Runs when entering the online loading screen.
    // ONLINE_LOBBY
    // Runs when entering the online lobby screen.
    // GUIFRAME
    // Params: GuiDrawContext draw_ctx
    // Runs every frame the game is rendered, thus runs at selected framerate. Drawing functions are only available during this callback through a GuiDrawContext
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
    // PRE_LOAD_LEVEL_FILES
    // Params: PreLoadLevelFilesContext load_level_ctx
    // Runs right before level files would be loaded
    // PRE_LEVEL_GENERATION
    // Runs before any level generation, no entities exist at this point. Runs in most screens that have entities. Return true to block normal level generation, i.e. stop any entities from being spawned by ThemeInfo functions. Does not block other ThemeInfo functions, like spawn_effects though. POST_LEVEL_GENERATION will still run if this callback is blocked.
    // POST_ROOM_GENERATION
    // Params: PostRoomGenerationContext room_gen_ctx
    // Runs right after all rooms are generated before entities are spawned
    // POST_LEVEL_GENERATION
    // Runs right after level generation is done, i.e. after all level gen entities are spawned, before any entities are updated. You can spawn your own entities here, like extra enemies, give items to players etc.
    // LOADING
    // Runs whenever state.loading changes and is > 0. Prefer PRE/POST_LOAD_SCREEN instead though.
    // PRE_LOAD_SCREEN
    // Runs right before loading a new screen based on screen_next. Return true from callback to block the screen from loading.
    // POST_LOAD_SCREEN
    // Runs right after a screen is loaded, before rendering anything
    // PRE_GET_RANDOM_ROOM
    // Params: int x, int y, LAYER layer, ROOM_TEMPLATE room_template
    // Return: `string room_data`
    // Called when the game wants to get a random room for a given template. Return a string that represents a room template to make the game use that.
    // If the size of the string returned does not match with the room templates expected size the room is discarded.
    // White spaces at the beginning and end of the string are stripped, not at the beginning and end of each line.
    // PRE_HANDLE_ROOM_TILES
    // Params: int x, int y, ROOM_TEMPLATE room_template, PreHandleRoomTilesContext room_ctx
    // Return: `bool last_callback` to determine whether callbacks of the same type should be executed after this
    // Runs after a random room was selected and right before it would spawn entities for each tile code
    // Allows you to modify the rooms content in the front and back layer as well as add a backlayer if not yet existant
    // SAVE
    // Params: SaveContext save_ctx
    // Runs at the same times as ON.SCREEN, but receives the save_ctx
    // LOAD
    // Params: LoadContext load_ctx
    // Runs as soon as your script is loaded, including reloads, then never again
    // RENDER_PRE_GAME
    // Params: VanillaRenderContext render_ctx
    // Runs before the ingame part of the game is rendered. Return `true` to skip rendering.
    // RENDER_POST_GAME
    // Params: VanillaRenderContext render_ctx
    // Runs after the level and HUD are rendered, before pause menus and blur effects
    // RENDER_PRE_LEVEL
    // Params: VanillaRenderContext render_ctx, int camera_layer
    // Runs before the level is rendered. Return `true` to skip rendering.
    // RENDER_POST_LEVEL
    // Params: VanillaRenderContext render_ctx, int camera_layer
    // Runs after the level is rendered, before hud
    // RENDER_PRE_LAYER
    // Params: VanillaRenderContext render_ctx, int rendered_layer
    // Runs before a layer is rendered, runs for both layers during layer door transitions. Return `true` to skip rendering.
    // RENDER_POST_LAYER
    // Params: VanillaRenderContext render_ctx, int rendered_layer
    // Runs after a layer is rendered, runs for both layers during layer door transitions. Things drawn here will be part of the layer transition animation
    // RENDER_PRE_HUD
    // Params: VanillaRenderContext render_ctx, Hud hud
    // Runs before the HUD is drawn on screen. In this event, you can draw textures with the `draw_screen_texture` function of the render_ctx or edit the Hud values. Return `true` to skip rendering.
    // RENDER_POST_HUD
    // Params: VanillaRenderContext render_ctx, Hud hud
    // Runs after the HUD is drawn on screen. In this event, you can draw textures with the `draw_screen_texture` function of the render_ctx
    // RENDER_PRE_PAUSE_MENU
    // Params: VanillaRenderContext render_ctx
    // Runs before the pause menu is drawn on screen. In this event, you can't really draw textures, because the blurred background is drawn on top of them. Return `true` to skip rendering.
    // RENDER_POST_PAUSE_MENU
    // Params: VanillaRenderContext render_ctx
    // Runs after the pause menu is drawn on screen. In this event, you can draw textures with the `draw_screen_texture` function of the render_ctx
    // RENDER_PRE_BLURRED_BACKGROUND
    // Params: VanillaRenderContext render_ctx, float blur
    // Runs before the blurred background is drawn on screen, behind pause menu or journal book. In this event, you can't really draw textures, because the blurred background is drawn on top of them. Return `true` to skip rendering.
    // RENDER_POST_BLURRED_BACKGROUND
    // Params: VanillaRenderContext render_ctx, float blur
    // Runs after the blurred background is drawn on screen, behind pause menu or journal book. In this event, you can draw textures with the `draw_screen_texture` function of the render_ctx. (blur amount is probably the same as journal opacity)
    // RENDER_PRE_DRAW_DEPTH
    // Params: VanillaRenderContext render_ctx, int draw_depth
    // Runs before the entities of the specified draw_depth are drawn on screen. In this event, you can draw textures with the `draw_world_texture` function of the render_ctx. Return `true` to skip rendering.
    // RENDER_POST_DRAW_DEPTH
    // Params: VanillaRenderContext render_ctx, int draw_depth
    // Runs right after the entities of the specified draw_depth are drawn on screen. In this event, you can draw textures with the `draw_world_texture` function of the render_ctx
    // RENDER_PRE_JOURNAL_PAGE
    // Params: VanillaRenderContext render_ctx, JOURNAL_PAGE_TYPE page_type, JournalPage page
    // Runs before the journal page is drawn on screen. Return `true` to skip rendering.
    // RENDER_POST_JOURNAL_PAGE
    // Params: VanillaRenderContext render_ctx, JOURNAL_PAGE_TYPE page_type, JournalPage page
    // Runs after the journal page is drawn on screen. In this event, you can draw textures with the draw_screen_texture function of the VanillaRenderContext
    // The JournalPage parameter gives you access to the specific fields of the page. Be sure to cast it to the correct type, the following functions are available to do that:
    // `page:as_journal_page_progress()`
    // `page:as_journal_page_journalmenu()`
    // `page:as_journal_page_places()`
    // `page:as_journal_page_people()`
    // `page:as_journal_page_bestiary()`
    // `page:as_journal_page_items()`
    // `page:as_journal_page_traps()`
    // `page:as_journal_page_feats()`
    // `page:as_journal_page_deathcause()`
    // `page:as_journal_page_deathmenu()`
    // `page:as_journal_page_playerprofile()`
    // `page:as_journal_page_lastgameplayed()`
    // SPEECH_BUBBLE
    // Params: Entity speaking_entity, string text
    // Runs before any speech bubble is created, even the one using [say](#say) function
    // Return: if you don't return anything it will execute the speech bubble function normally with default message
    // if you return empty string, it will not create the speech bubble at all, if you return string, it will use that instead of the original
    // The first script to return string (empty or not) will take priority, the rest will receive callback call but the return behavior won't matter
    // TOAST
    // Params: string text
    // Runs before any toast is created, even the one using [toast](#toast) function
    // Return: if you don't return anything it will execute the toast function normally with default message
    // if you return empty string, it will not create the toast at all, if you return string, it will use that instead of the original message
    // The first script to return string (empty or not) will take priority, the rest will receive callback call but the return behavior won't matter
    // DEATH_MESSAGE
    // Params: STRINGID id
    // Runs once after death when the death message journal page is shown. The parameter is the STRINGID of the title, like 1221 for BLOWN UP.
    // PRE_LOAD_JOURNAL_CHAPTER
    // Params: JOURNALUI_PAGE_SHOWN chapter
    // Runs before the journal or any of it's chapter is opened
    // Return: return true to not load the chapter (or journal as a whole)
    // POST_LOAD_JOURNAL_CHAPTER
    // Params: JOURNALUI_PAGE_SHOWN chapter, array:int pages
    // Runs after the pages for the journal are prepared, but not yet displayed, `pages` is a list of page numbers that the game loaded, if you want to change it, do the changes (remove pages, add new ones, change order) and return it
    // All new pages will be created as [JournalPageStory](#JournalPageStory), any custom with page number above 9 will be empty, I recommend using above 99 to be sure not to get the game page, you can later use this to recognise and render your own stuff on that page in the RENDER_POST_JOURNAL_PAGE
    // Return: return new page array to modify the journal, returning empty array or not returning anything will load the journal normally, any page number that was already loaded will result in the standard game page
    // When changing the order of game pages make sure that the page that normally is rendered on the left side is on the left in the new order, otherwise you get some messed up result, custom pages don't have this problem. The order is: left, right, left, right ...
    // PRE_GET_FEAT
    // Runs before getting performed status for a FEAT when rendering the Feats page in journal.
    // Return: true to override the vanilla feat with your own. Defaults to Steam GetAchievement.
    // PRE_SET_FEAT
    // Runs before the game sets a vanilla feat performed.
    // Return: true to block the default behaviour of calling Steam SetAchievement.
    // PRE_UPDATE
    // Runs before the State is updated, runs always (menu, settings, camp, game, arena, online etc.) with the game engine, typically 60FPS
    // Return behavior: return true to stop further PRE_UPDATE callbacks from executing and don't update the state (this will essentially freeze the game engine)
    // POST_UPDATE
    // Runs right after the State is updated, runs always (menu, settings, camp, game, arena, online etc.) with the game engine, typically 60FPS
    // SCRIPT_ENABLE
    // Runs when the script is enabled from the UI or when imported by another script while disabled, but not on load.
    // SCRIPT_DISABLE
    // Runs when the script is disabled from the UI and also right before unloading/reloading.
    // USER_DATA
    // Params: Entity ent
    // Runs on all changes to Entity.user_data, including after loading saved user_data in the next level and transition. Also runs the first time user_data is set back to nil, but nil won't be saved to bother you on future levels.
    // PRE_LEVEL_CREATION
    // Runs right before the front layer is created. Runs in all screens that usually have entities, or when creating a layer manually.
    // POST_LEVEL_CREATION
    // Runs right after the back layer has been created and you can start spawning entities in it. Runs in all screens that usually have entities, or when creating a layer manually.
    // PRE_LAYER_CREATION
    // Params: LAYER layer
    // Runs right before a layer is created. Runs in all screens that usually have entities, or when creating a layer manually.
    // POST_LAYER_CREATION
    // Params: LAYER layer
    // Runs right after a layer has been created and you can start spawning entities in it. Runs in all screens that usually have entities, or when creating a layer manually.
    // PRE_LEVEL_DESTRUCTION
    // Runs right before the current level is unloaded and any entities destroyed. Runs in pretty much all screens, even ones without entities. The screen has already changed at this point, meaning the screen being destroyed is in state.screen_last.
    // POST_LEVEL_DESTRUCTION
    // Runs right after the current level has been unloaded and all entities destroyed. Runs in pretty much all screens, even ones without entities. The screen has already changed at this point, meaning the screen being destroyed is in state.screen_last.
    // PRE_LAYER_DESTRUCTION
    // Params: LAYER layer
    // Runs right before a layer is unloaded and any entities there destroyed. Runs in pretty much all screens, even ones without entities. The screen has already changed at this point, meaning the screen being destroyed is in state.screen_last.
    // POST_LAYER_DESTRUCTION
    // Params: LAYER layer
    // Runs right after a layer has been unloaded and any entities there destroyed. Runs in pretty much all screens, even ones without entities. The screen has already changed at this point, meaning the screen being destroyed is in state.screen_last.
    // PRE_PROCESS_INPUT
    // Runs right before the game gets input from various devices and writes to a bunch of buttons-variables. Return true to disable all game input completely.
    // POST_PROCESS_INPUT
    // Runs right after the game gets input from various devices and writes to a bunch of buttons-variables. Probably the first chance you have to capture or edit buttons_gameplay or buttons_menu sort of things.
    // PRE_GAME_LOOP
    // Runs right before the main engine loop. Return true to block state updates and menu updates, i.e. to pause inside menus.
    // POST_GAME_LOOP
    // Runs right after the main engine loop.
    // PRE_SAVE_STATE
    // Runs right before the main StateMemory is manually saved to a slot or a custom SaveState. Slot is 1..4 or -1 on custom SaveState. Return true to block save.
    // Params: int slot, StateMemory saved
    // POST_SAVE_STATE
    // Runs right after the main StateMemory is manually saved to a slot or a custom SaveState. Slot is 1..4 or -1 on custom SaveState.
    // Params: int slot, StateMemory saved
    // PRE_LOAD_STATE
    // Runs right before the main StateMemory is manually loaded from a slot or a custom SaveState. Slot is 1..4 or -1 on custom SaveState. Return true to block load.
    // Params: int slot, StateMemory loaded
    // POST_LOAD_STATE
    // Runs right after the main StateMemory is manually loaded from a slot or a custom SaveState. Slot is 1..4 or -1 on custom SaveState.
    // Params: int slot, StateMemory loaded
    // BLOCKED_UPDATE
    // Runs instead of POST_UPDATE when anything blocks a PRE_UPDATE. Even runs in Playlunky when Overlunky blocks a PRE_UPDATE.
    // BLOCKED_GAME_LOOP
    // Runs instead of POST_GAME_LOOP when anything blocks a PRE_GAME_LOOP. Even runs in Playlunky when Overlunky blocks a PRE_GAME_LOOP.
    // BLOCKED_PROCESS_INPUT
    // Runs instead of POST_PROCESS_INPUT when anything blocks a PRE_PROCESS_INPUT. Even runs in Playlunky when Overlunky blocks a PRE_PROCESS_INPUT.
    */

    lua.create_named_table(
        "SPAWN_TYPE",
        "LEVEL_GEN",
        SPAWN_TYPE_LEVEL_GEN,
        "LEVEL_GEN_TILE_CODE",
        SPAWN_TYPE_LEVEL_GEN_TILE_CODE,
        "LEVEL_GEN_PROCEDURAL",
        SPAWN_TYPE_LEVEL_GEN_PROCEDURAL,
        "LEVEL_GEN_FLOOR_SPREADING",
        SPAWN_TYPE_LEVEL_GEN_FLOOR_SPREADING,
        "LEVEL_GEN_GENERAL",
        SPAWN_TYPE_LEVEL_GEN_GENERAL,
        "SCRIPT",
        SPAWN_TYPE_SCRIPT,
        "SYSTEMIC",
        SPAWN_TYPE_SYSTEMIC,
        "ANY",
        SPAWN_TYPE_ANY);
    /* SPAWN_TYPE
    // LEVEL_GEN
    // For any spawn happening during level generation, even if the call happened from the Lua API during a tile code callback.
    // LEVEL_GEN_TILE_CODE
    // Similar to LEVEL_GEN but only triggers on tile code spawns.
    // LEVEL_GEN_PROCEDURAL
    // Similar to LEVEL_GEN but only triggers on random level spawns, like snakes or bats.
    // LEVEL_GEN_FLOOR_SPREADING
    // Includes solid floor type spreading (i.e. floorstyled bleeding to existing generic floor) but also corner filling of empty tiles.
    // LEVEL_GEN_GENERAL
    // Covers all spawns during level gen that are not covered by the other two.
    // SCRIPT
    // Runs for any spawn happening through a call from the Lua API, also during level generation.
    // SYSTEMIC
    // Covers all other spawns, such as items from crates or the player throwing bombs.
    // ANY
    // Covers all of the above.
    */
    /// Some arbitrary constants of the engine
    lua.create_named_table("CONST", "ENGINE_FPS", 60, "ROOM_WIDTH", 10, "ROOM_HEIGHT", 8, "MAX_TILES_VERT", g_level_max_y, "MAX_TILES_HORIZ", g_level_max_x, "NOF_DRAW_DEPTHS", 53, "MAX_PLAYERS", 4);
    /* CONST
    // ENGINE_FPS
    // The framerate at which the engine updates, e.g. at which `ON.GAMEFRAME` and similar are called.
    // Independent of rendering framerate, so it does not correlate with the call rate of `ON.GUIFRAME` and similar.
    // ROOM_WIDTH
    // Width of a 1x1 room, both in world coordinates and in tiles.
    // ROOM_HEIGHT
    // Height of a 1x1 room, both in world coordinates and in tiles.
    // MAX_TILES_VERT
    // Maximum number of working floor tiles in vertical axis, 126 (0-125 coordinates)
    // Floors spawned above or below will not have any collision
    // MAX_TILES_HORIZ
    // Maximum number of working floor tiles in horizontal axis, 86 (0-85 coordinates)
    // Floors spawned above or below will not have any collision
    // NOF_DRAW_DEPTHS
    // Number of draw_depths, 53 (0-52)
    // MAX_PLAYERS
    // Just the max number of players in multiplayer
    */
    /// After setting the WIN_STATE, the exit door on the current level will lead to the chosen ending
    lua.create_named_table(
        "WIN_STATE",
        "NO_WIN",
        0,
        "TIAMAT_WIN",
        1,
        "HUNDUN_WIN",
        2,
        "COSMIC_OCEAN_WIN",
        3);

    /// Used in the `render_ctx:draw_text` and `render_ctx:draw_text_size` functions of the ON.RENDER_PRE/POST_xxx event
    lua.create_named_table(
        "VANILLA_TEXT_ALIGNMENT",
        "LEFT",
        0,
        "CENTER",
        1,
        "RIGHT",
        2);

    /// Used in the `render_ctx:draw_text` and `render_ctx:draw_text_size` functions of the ON.RENDER_PRE/POST_xxx event
    /// There are more styles, we just didn't name them all
    lua.create_named_table(
        "VANILLA_FONT_STYLE",
        "NORMAL",
        0,
        "ITALIC",
        1,
        "BOLD",
        2);

    /// Paramater to get_setting (and set_setting in unsafe mode)
    lua.create_named_table("GAME_SETTING"
                           //, "WINDOW_SCALE", 0
                           //, "", ...check__[game_settings.txt]\[game_data/game_settings.txt\]...
                           //, "CROSSPROGRESS_AUTOSYNC", 47
    );
    for (auto& [setting_name_view, setting_index] : get_settings_names_and_indices())
    {
        std::string setting_name{setting_name_view};
        std::transform(setting_name.begin(), setting_name.end(), setting_name.begin(), [](unsigned char c)
                       { return (unsigned char)std::toupper(c); });
        lua["GAME_SETTING"][std::move(setting_name)] = setting_index;
    }

    /// 8bit bitmask used in state.pause
    lua.create_named_table("PAUSE", "MENU", 0x01, "FADE", 0x02, "CUTSCENE", 0x04, "FLAG4", 0x08, "FLAG5", 0x10, "ANKH", 0x20);
    /* PAUSE
    // MENU
    // Menu: Pauses the level timer and engine. Can't set, controller by the menu.
    // FADE
    // Fade/Loading: Pauses all timers and engine.
    // CUTSCENE
    // Cutscene: Pauses total/level time but not engine. Used by boss cutscenes.
    // FLAG4
    // Unknown purpose: Pauses total/level time and engine. Does not pause the global counter so [set_global_interval](#set_global_interval) timers still run. Might change this later!
    // FLAG5
    // Unknown purpose: Pauses total/level time and engine. Does not pause the global counter so [set_global_interval](#set_global_interval) timers still run. Might change this later!
    // ANKH
    // Ankh: Pauses all timers, physics and music, but not camera. Used by the ankh cutscene.
    */
}

std::recursive_mutex global_lua_lock;

std::vector<std::string> safe_fields{};
std::vector<std::string> unsafe_fields{};

std::shared_ptr<sol::state> acquire_lua_vm(class SoundManager* sound_manager)
{
    static std::shared_ptr<sol::state> global_vm = [sound_manager]()
    {
        std::unique_lock lock{global_lua_lock};
        std::shared_ptr<sol::state> global_vms = std::make_shared<sol::state>();
        sol::state& lua_vm = *global_vms;
        load_libraries(lua_vm);
        populate_lua_state(lua_vm, sound_manager);

        for (auto& [k, v] : lua_vm["_G"].get<sol::table>())
        {
            if (k.get_type() == sol::type::string)
            {
                std::string_view key = k.as<std::string_view>();
                if (key != "debug" && key != "package")
                {
                    safe_fields.push_back(std::string{key});
                }
            }
        }

        safe_fields.push_back("serpent");
        load_unsafe_libraries(lua_vm);

        for (auto& [k, v] : lua_vm["_G"].get<sol::table>())
        {
            if (k.get_type() == sol::type::string)
            {
                std::string_view key = k.as<std::string_view>();
                auto it = std::find(safe_fields.begin(), safe_fields.end(), key);
                if (it == safe_fields.end())
                {
                    unsafe_fields.push_back(std::string{key});
                }
            }
        }

        return global_vms;
    }();
    return global_vm;
}
sol::state& get_lua_vm(SoundManager* sound_manager)
{
    static sol::state& global_vm = *acquire_lua_vm(sound_manager);
    return global_vm;
}

sol::protected_function_result execute_lua(sol::environment& env, std::string_view code, bool pass)
{
    static sol::state& global_vm = get_lua_vm();
    return global_vm.safe_script(code, env, pass ? &sol::script_pass_on_error : &sol::script_default_on_error);
}

bool check_safe_io_path(const std::string& filepath, const std::string& basepath)
{
    if (basepath.empty())
        return false;

    auto base = std::filesystem::absolute(basepath).lexically_normal();
    auto path = std::filesystem::absolute(filepath).lexically_normal();

    auto [rootEnd, nothing] =
        std::mismatch(base.begin(), base.end(), path.begin());

    return rootEnd == base.end();
}

void populate_lua_env(sol::environment& env)
{
    static const sol::state& global_vm = get_lua_vm();
    for (auto& field : safe_fields)
    {
        env[field] = global_vm["_G"][field];
    }
    env["_G"] = env;
    add_partial_safe_libraries(env);
}
void add_partial_safe_libraries(sol::environment& env)
{
    static const sol::state& global_vm = get_lua_vm();

    auto open_data = [](std::string filename, std::optional<std::string> mode) -> sol::object
    {
        auto backend = LuaBackend::get_calling_backend();
        auto is_pack = check_safe_io_path(backend->get_path(), "Mods/Packs");
        auto is_safe = !backend->get_unsafe();
        std::string moddir = backend->get_root_path().filename().string();
        std::string luafile = std::filesystem::path(backend->get_path()).filename().string();
        std::string datadir = "Mods/Data/" + (is_pack ? moddir : luafile);
        std::string fullpath = datadir + "/" + filename;
        std::string dirpath = std::filesystem::path(fullpath).parent_path().string();
        auto is_based = check_safe_io_path(fullpath, datadir);
        if (is_safe && !is_based)
        {
            luaL_error(global_vm, "Attempted to open data file outside data directory");
            return sol::nil;
        }
        if (mode.value_or("r")[0] != 'r')
            std::filesystem::create_directories(dirpath);
        return global_vm["io"]["open"](fullpath, mode.value_or("r"));
    };

    auto open_mod = [](std::string filename, std::optional<std::string> mode) -> sol::object
    {
        auto backend = LuaBackend::get_calling_backend();
        auto is_pack = check_safe_io_path(backend->get_path(), "Mods/Packs");
        auto is_safe = !backend->get_unsafe();
        std::string fullpath = std::string(backend->get_root()) + "/" + filename;
        auto is_based = check_safe_io_path(fullpath, backend->get_root());
        std::string dirpath = std::filesystem::path(fullpath).parent_path().string();
        if (is_safe)
        {
            if (!is_based)
            {
                luaL_error(global_vm, "Attempted to open mod file outside mod directory");
                return sol::nil;
            }
            if (!is_pack && mode.value_or("r")[0] != 'r')
            {
                luaL_error(global_vm, "Attempted to write mod file outside Packs directory");
                return sol::nil;
            }
        }
        if (mode.value_or("r")[0] != 'r')
            std::filesystem::create_directories(dirpath);
        return global_vm["io"]["open"](fullpath, mode);
    };

    auto remove_data = [](std::string filename) -> sol::object
    {
        auto backend = LuaBackend::get_calling_backend();
        auto is_pack = check_safe_io_path(backend->get_path(), "Mods/Packs");
        auto is_safe = !backend->get_unsafe();
        std::string moddir = backend->get_root_path().filename().string();
        std::string luafile = std::filesystem::path(backend->get_path()).filename().string();
        std::string datadir = "Mods/Data/" + (is_pack ? moddir : luafile);
        std::string fullpath = datadir + "/" + filename;
        std::string dirpath = std::filesystem::path(fullpath).parent_path().string();
        auto is_based = check_safe_io_path(fullpath, datadir);
        if (is_safe && !is_based)
        {
            luaL_error(global_vm, "Attempted to remove data file outside data directory");
            return sol::nil;
        }
        return global_vm["os"]["remove"](fullpath);
    };

    auto remove_mod = [](std::string filename) -> sol::object
    {
        auto backend = LuaBackend::get_calling_backend();
        auto is_pack = check_safe_io_path(backend->get_path(), "Mods/Packs");
        auto is_safe = !backend->get_unsafe();
        std::string fullpath = std::string(backend->get_root()) + "/" + filename;
        auto is_based = check_safe_io_path(fullpath, backend->get_root());
        std::string dirpath = std::filesystem::path(fullpath).parent_path().string();
        if (is_safe)
        {
            if (!is_based)
            {
                luaL_error(global_vm, "Attempted to remove mod file outside mod directory");
                return sol::nil;
            }
            if (!is_pack)
            {
                luaL_error(global_vm, "Attempted to remove mod file outside Packs directory");
                return sol::nil;
            }
        }
        return global_vm["os"]["remove"](fullpath);
    };

    if (env["os"] == sol::nil)
    {
        sol::table os(global_vm, sol::create);
        os["clock"] = global_vm["os"]["clock"];
        os["date"] = global_vm["os"]["date"];
        os["difftime"] = global_vm["os"]["difftime"];
        os["time"] = global_vm["os"]["time"];
        os["remove_data"] = remove_data;
        os["remove_mod"] = remove_mod;
        env["os"] = os;
    }
    else if (env["os"].get_type() == sol::type::table)
    {
        env["os"]["remove_data"] = remove_data;
        env["os"]["remove_mod"] = remove_mod;
    }

    if (env["io"] == sol::nil)
    {
        sol::table io(global_vm, sol::create);
        io["type"] = global_vm["io"]["type"];
        io["open_data"] = open_data;
        io["open_mod"] = open_mod;
        env["io"] = io;
    }
    else if (env["io"].get_type() == sol::type::table)
    {
        env["io"]["open_data"] = open_data;
        env["io"]["open_mod"] = open_mod;
    }
}
void hide_unsafe_libraries(sol::environment& env)
{
    for (auto& field : unsafe_fields)
    {
        env[field] = sol::nil;
    }
    add_partial_safe_libraries(env);
}
void expose_unsafe_libraries(sol::environment& env)
{
    static const sol::state& global_vm = get_lua_vm();
    for (auto& field : unsafe_fields)
    {
        env[field] = global_vm["_G"][field];
    }
    add_partial_safe_libraries(env);
}
