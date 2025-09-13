#include "lua_vm.hpp"

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
#include "heap_base.hpp"                           // for OnHeapPointer, HeapBase
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
#include "state.hpp"                               // for StateMemory
#include "strings.hpp"                             // for change_string
#include "usertypes/behavior_lua.hpp"              // for register_usertypes
#include "usertypes/bucket_lua.hpp"                // for register_usertypes
#include "usertypes/char_state_lua.hpp"            // for register_usertypes
#include "usertypes/color_lua.hpp"                 // for register_usertypes
#include "usertypes/deprecated_func.hpp"           // for register_usertypes
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
#include "usertypes/game_patches_lua.hpp"          // for register_usertypes
#include "usertypes/global_players_lua.hpp"        // for register_usertypes
#include "usertypes/gui_lua.hpp"                   // for register_usertypes
#include "usertypes/hitbox_lua.hpp"                // for register_usertypes
#include "usertypes/level_lua.hpp"                 // for register_usertypes
#include "usertypes/logic_lua.hpp"                 // for register_usertypes
#include "usertypes/options_lua.hpp"               // for register_usertypes
#include "usertypes/particles_lua.hpp"             // for register_usertypes
#include "usertypes/player_lua.hpp"                // for register_usertypes
#include "usertypes/prng_lua.hpp"                  // for register_usertypes
#include "usertypes/save_context.hpp"              // for register_usertypes
#include "usertypes/screen_arena_lua.hpp"          // for register_usertypes
#include "usertypes/screen_lua.hpp"                // for register_usertypes
#include "usertypes/socket_lua.hpp"                // for register_usertypes
#include "usertypes/sound_lua.hpp"                 // for register_usertypes
#include "usertypes/spawn_lua.hpp"                 // for register_usertypes
#include "usertypes/state_lua.hpp"                 // for register_usertypes
#include "usertypes/steam_lua.hpp"                 // for register_usertypes
#include "usertypes/texture_lua.hpp"               // for register_usertypes
#include "usertypes/vanilla_render_lua.hpp"        // for VanillaRenderContext
#include "usertypes/vtables_lua.hpp"               // for register_usertypes
#include "virtual_table.hpp"                       //

struct Illumination;

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
    NDeprecated::register_usertypes(lua);
    NGPlayers::register_usertypes(lua);
    NSpawn::register_usertypes(lua);
    NGamePatches::register_usertypes(lua);
    NOptions::register_usertypes(lua);

    /// A bunch of [game state](#StateMemory) variables. Your ticket to almost anything that is not an Entity.
    lua["state"] = HeapBase::get_main().state();
    /// The GameManager gives access to a couple of Screens as well as the pause and journal UI elements
    lua["game_manager"] = get_game_manager();
    /// The Online object has information about the online lobby and its players
    lua["online"] = get_online();
    /// An array of [Player](#Player) of the current players. This is just a list of existing Player entities in order, i.e., `players[1]` is not guaranteed to be P1 if they have been gibbed for example. See [get_player](#get_player).
    // lua["players"] = get_players();

    /// Returns Player (or PlayerGhost if `get_player(1, true)`) with this player slot
    // lua["get_player"] = [&lua](int8_t slot, std::optional<bool> or_ghost) -> Player|PlayerGhost
    lua["get_player"] = [&lua](int8_t slot, std::optional<bool> or_ghost) -> sol::object
    {
        auto state = HeapBase::get().state();
        if (state && state->items)
        {
            auto player = state->items->player(slot - 1);
            if (player)
                return sol::make_object_userdata(lua, player);
        }
        if (or_ghost.value_or(false))
        {
            for (auto uid : get_entities_by(to_id("ENT_TYPE_ITEM_PLAYERGHOST"), ENTITY_MASK::ITEM, LAYER::BOTH))
            {
                auto player = get_entity_ptr(uid)->as<PlayerGhost>();
                if (player->inventory && player->inventory->player_slot == slot - 1)
                    return sol::make_object_userdata(lua, player);
            }
        }
        return sol::nil;
    };

    /// Returns PlayerGhost with this player slot 1..4
    lua["get_playerghost"] = [](int8_t slot) -> PlayerGhost*
    {
        for (auto uid : get_entities_by(to_id("ENT_TYPE_ITEM_PLAYERGHOST"), ENTITY_MASK::ITEM, LAYER::BOTH))
        {
            auto player = get_entity_ptr(uid)->as<PlayerGhost>();
            if (player->inventory && player->inventory->player_slot == slot - 1)
                return player;
        }
        return nullptr;
    };
    /// Provides access to the save data, updated as soon as something changes (i.e. before it's written to savegame.sav.) Use [save_progress](#save_progress) to save to savegame.sav.
    lua["savegame"] = get_game_manager()->save_related->savedata.decode_main();

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
    // lua["dump"] = [](sol::object object, optional<int> depth) -> table

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
        int now = HeapBase::get().state()->time_level;
        auto luaCb = IntervalCallback{cb, frames, now};
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
        int now = HeapBase::get().frame_count();
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

    /// Enable/disable godmode for players.
    lua["god"] = [](bool g)
    { API::godmode(g); };
    /// Enable/disable godmode for companions.
    lua["god_companions"] = [](bool g)
    { API::godmode_companions(g); };
    /// Set the zoom level used in levels and shops. 13.5 is the default, or 12.5 for shops. See zoom_reset.
    lua["zoom"] = [](float level)
    { API::zoom(level); };
    /// Reset the default zoom levels for all areas and sets current zoom level to 13.5.
    lua["zoom_reset"] = []()
    { API::zoom_reset(); };

    /// Set the contents of [Coffin](#Coffin), [Present](#Present), [Pot](#Pot), [Container](#Container)
    /// Check the [entity hierarchy list](https://github.com/spelunky-fyi/overlunky/blob/main/docs/entities-hierarchy.md) for what the exact ENT_TYPE's can this function affect
    lua["set_contents"] = set_contents;

    /// Gets a grid entity, such as floor or spikes, at the given position and layer.
    lua["get_grid_entity_at"] = get_grid_entity_at;
    /// Get uids of static entities overlapping this grid position (decorations, backgrounds etc.)
    lua["get_entities_overlapping_grid"] = get_entities_overlapping_grid;
    /// Returns a list of all uids in `entities` for which `predicate(get_entity(uid))` returns true
    lua["filter_entities"] = [&lua](std::vector<uint32_t> entities, sol::function predicate) -> std::vector<uint32_t>
    {
        return filter_entities(std::move(entities), [&lua, pred = std::move(predicate)](Entity* entity) -> bool
                               { return pred(lua["cast_entity"](entity)); });
    };

    auto get_entities_by = sol::overload(
        static_cast<std::vector<uint32_t> (*)(ENT_TYPE, ENTITY_MASK, LAYER)>(::get_entities_by),
        static_cast<std::vector<uint32_t> (*)(std::vector<ENT_TYPE>, ENTITY_MASK, LAYER)>(::get_entities_by));
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

    auto get_entities_at = sol::overload(
        static_cast<std::vector<uint32_t> (*)(ENT_TYPE, ENTITY_MASK, float, float, LAYER, float)>(::get_entities_at),
        static_cast<std::vector<uint32_t> (*)(std::vector<ENT_TYPE>, ENTITY_MASK, float, float, LAYER, float)>(::get_entities_at));
    /// Get uids of matching entities inside some radius ([ENT_TYPE](#ENT_TYPE), [MASK](#MASK)). Set `entity_type` or `mask` to `0` to ignore that, can also use table of entity_types
    /// Recommended to always set the mask, even if you look for one entity type
    lua["get_entities_at"] = get_entities_at;

    auto get_entities_overlapping_hitbox = sol::overload(
        static_cast<std::vector<uint32_t> (*)(ENT_TYPE, ENTITY_MASK, AABB, LAYER)>(::get_entities_overlapping_hitbox),
        static_cast<std::vector<uint32_t> (*)(std::vector<ENT_TYPE>, ENTITY_MASK, AABB, LAYER)>(::get_entities_overlapping_hitbox));
    /// Get uids of matching entities overlapping with the given hitbox. Set `entity_type` or `mask` to `0` to ignore that, can also use table of entity_types
    lua["get_entities_overlapping_hitbox"] = get_entities_overlapping_hitbox;

    /// Get the current set zoom level
    lua["get_zoom_level"] = []() -> float
    {
        auto game_api = GameAPI::get();
        return game_api->get_current_zoom();
    };
    /// Get the game coordinates at the screen position (`x`, `y`)
    lua["game_position"] = [](float x, float y) -> std::pair<float, float>
    { return API::click_position(x, y); };
    /// Translate an entity position to screen position to be used in drawing functions
    lua["screen_position"] = [](float x, float y) -> std::pair<float, float>
    { return API::screen_position(x, y); };
    /// Translate a distance of `x` tiles to screen distance to be be used in drawing functions
    lua["screen_distance"] = screen_distance;
    /// Get the current frame count since the game was started*. You can use this to make some timers yourself, the engine runs at 60fps. This counter is paused if the pause is set with flags PAUSE.FADE or PAUSE.ANKH.
    lua["get_frame"] = []() -> uint32_t
    { return HeapBase::get().frame_count(); };
    /// Get the current global frame count since the game was started. You can use this to make some timers yourself, the engine runs at 60fps. This counter keeps incrementing with game loop. Never stops.
    // lua["get_global_frame"] = []() -> int
    lua["get_global_frame"] = API::get_global_frame_count;
    /// Get the current timestamp in milliseconds since the Unix Epoch.
    lua["get_ms"] = []()
    { return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(); };
    /// Enables or disables the journal
    lua["set_journal_enabled"] = [](bool b)
    { get_journal_enabled() = b; };
    /// Checks whether a coordinate is inside a room containing an active shop. This function checks whether the shopkeeper is still alive.
    lua["is_inside_active_shop_room"] = is_inside_active_shop_room;
    /// Checks whether a coordinate is inside a shop zone, the rectangle where the camera zooms in a bit. Does not check if the shop is still active!
    lua["is_inside_shop_zone"] = is_inside_shop_zone;
    /// Basically gets the absolute coordinates of the area inside the unbreakable bedrock walls, from wall to wall. Every solid entity should be
    /// inside these boundaries. The order is: left x, top y, right x, bottom y
    lua["get_bounds"] = []() -> std::tuple<float, float, float, float>
    {
        auto state = HeapBase::get().state();
        return std::make_tuple(2.5f, 122.5f, state->w * 10.0f + 2.5f, 122.5f - state->h * 8.0f);
    };
    /// Same as [get_bounds](#get_bounds) but returns AABB struct instead of loose floats
    lua["get_aabb_bounds"] = []() -> AABB
    {
        auto state = HeapBase::get().state();
        return {2.5f, 122.5f, state->w * 10.0f + 2.5f, 122.5f - state->h * 8.0f};
    };
    /// Gets the current camera position in the level
    lua["get_camera_position"] = []() -> std::pair<float, float>
    { return Camera::get_position(); };
    /// Sets the absolute current camera position without rubberbanding animation. Ignores camera bounds or currently focused uid, but doesn't clear them. Best used in ON.RENDER_PRE_GAME or similar. See Camera for proper camera handling with bounds and rubberbanding.
    lua["set_camera_position"] = [](float cx, float cy)
    { HeapBase::get().state()->camera->set_position(cx, cy); };
    /// Updates the camera focus according to the params set in Camera, i.e. to apply normal camera movement when paused etc.
    lua["update_camera_position"] = []()
    { HeapBase::get().state()->camera->update_position(); };

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
                    id,
                    screen_id,
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
                    id,
                    screen_id,
                    FrontBinder{},
                    BackBinder{[]()
                               { return VanillaRenderContext{}; }}));

            auto backend = LuaBackend::get_calling_backend();
            backend->screen_hooks.push_back({screen_id, id});
            return id;
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

    /// Raise a signal and probably crash the game
    lua["raise"] = std::raise;

    /// Convert the hash to stringid
    /// Check [strings00_hashed.str](https://github.com/spelunky-fyi/overlunky/blob/main/docs/game_data/strings00_hashed.str) for the hash values, or extract assets with modlunky and check those.
    lua["hash_to_stringid"] = hash_to_stringid;
    /// Get string behind STRINGID, **don't use stringid directly for vanilla string**, use [hash_to_stringid](#hash_to_stringid) first
    /// Will return the string of currently choosen language
    lua["get_string"] = get_string;
    /// Change string at the given id (**don't use stringid directly for vanilla string**, use [hash_to_stringid](#hash_to_stringid) first)
    /// This edits custom string and in game strings but changing the language in settings will reset game strings
    lua["change_string"] = [](STRINGID id, std::u16string str)
    { return change_string(id, str); };
    /// Add custom string, currently can only be used for names of shop items (EntityDB->description)
    /// Returns STRINGID of the new string
    lua["add_string"] = add_string;
    /// Adds custom name to the item by uid used in the shops
    /// This is better alternative to `add_string` but instead of changing the name for entity type, it changes it for this particular entity
    lua["add_custom_name"] = add_custom_name;
    /// Clears the name set with [add_custom_name](#add_custom_name)
    lua["clear_custom_name"] = clear_custom_name;

    /// Adds entity as shop item, has to be of [Purchasable](#Purchasable) type, check the [entity hierarchy list](https://github.com/spelunky-fyi/overlunky/blob/main/docs/entities-hierarchy.md) to find all the Purchasable entity types.
    /// Adding other entities will result in not obtainable items or game crash, if item already is in StateMemory.room_owners.owned_items then it will just re-parent it
    lua["add_item_to_shop"] = add_item_to_shop;

    auto create_illumination = sol::overload(
        static_cast<Illumination* (*)(Color, float, float, float)>(::create_illumination),
        static_cast<Illumination* (*)(Color, float, int32_t)>(::create_illumination),
        static_cast<Illumination* (*)(Vec2, Color, LIGHT_TYPE, float, uint8_t, int32_t, LAYER)>(::create_illumination));
    /// Creates a new Illumination. Don't forget to continuously call [refresh_illumination](#refresh_illumination), otherwise your light emitter fades out! Check out the [illumination.lua](https://github.com/spelunky-fyi/overlunky/blob/main/examples/illumination.lua) script for an example.
    /// Warning: this is only valid for current level!
    lua["create_illumination"] = create_illumination;
    /// Refreshes an Illumination, keeps it from fading out, short for `illumination.timer = get_frame()`
    lua["refresh_illumination"] = refresh_illumination;

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
    lua["set_setting"] = [](GAME_SETTING setting, std::uint32_t value) -> bool
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

    /// Get the current adventure seed pair, or optionally what it was at the start of this run, because it changes every level.
    lua["get_adventure_seed"] = get_adventure_seed;
    /// Set the current adventure seed pair. Use just before resetting a run to recreate an adventure run.
    lua["set_adventure_seed"] = set_adventure_seed;
    /// Updates the floor collisions used by the liquids, set add to false to remove tile of collision, set to true to add one
    /// optional `layer` parameter to be used when liquid was moved to back layer using [set_liquid_layer](#set_liquid_layer)
    lua["update_liquid_collision_at"] = update_liquid_collision_at;

    /// Optimized function to check for the amount of liquids at a certain position, by accessing a 2d array of liquids by third of a tile. Try the `liquids.lua` example to know better how it works.
    /// Returns a pair of water and lava, in that order.
    /// Water blobs increase the number by 2 on the grid, while lava blobs increase it by 3. The maximum is usually 6.
    /// Coarse water increase the number by 3, coarse and stagnant lava by 6. Combinations of both normal and coarse can make the number higher than 6.
    lua["get_liquids_at"] = get_liquids_at;

    /// Get the rva for a pattern name, used for debugging.
    lua["get_rva"] = [](std::string_view address_name) -> std::string
    { return fmt::format("{:X}", get_address(address_name) - Memory::get().at_exe(0)); };

    /// Get the rva for a vtable offset and index, used for debugging.
    lua["get_virtual_rva"] = [](VTABLE_OFFSET offset, uint32_t index) -> std::string
    { return fmt::format("{:X}", get_virtual_function_address(offset, index)); };

    /// Get memory address from a lua object
    lua["get_address"] = [&lua]([[maybe_unused]] sol::object o)
    { return fmt::format("{:X}", *(size_t*)lua_touserdata(lua, 1)); };

    /// Log to spelunky.log
    lua["log_print"] = game_log;

    /// Immediately ends the run with the death screen, also calls the [save_progress](#save_progress)
    lua["load_death_screen"] = load_death_screen;

    /// Saves the game to savegame.sav, unless game saves are blocked in the settings. Also runs the ON.SAVE callback. Fails and returns false, if you're trying to save too often (2s).
    lua["save_progress"] = []() -> bool
    {
        auto backend = LuaBackend::get_calling_backend();
        if (backend->last_save <= API::get_global_frame_count() - 120)
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
        if (backend->last_save <= API::get_global_frame_count() - 120)
        {
            backend->manual_save = true;
            return true;
        }
        return false;
    };

    /// Set the level number shown in the hud and journal to any string. This is reset to the default "%d-%d" automatically just before PRE_LOAD_SCREEN to a level or main menu, so use in PRE_LOAD_SCREEN, POST_LEVEL_GENERATION or similar for each level.
    /// Use "%d-%d" to reset to default manually. Does not affect the "...COMPLETED!" message in transitions or lines in "Dear Journal", you need to edit them separately with [change_string](#change_string).
    lua["set_level_string"] = [](std::u16string str)
    { return set_level_string(str); };

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
        auto state = HeapBase::get().state();
        if (hs == 0 || state->items->player_count > 3)
            f = 1.0f;
        else if (hs == 1 || state->items->player_count > 2)
            f = 1.15f;
        else
            f = 1.3f;
        float w = 0.32f * f;

        float ay = 0.94f - (1.0f - f) * 0.1f;
        float h = 0.2f * f;

        return AABB(ax + index * w + 0.02f * f, ay, ax + index * w + w - 0.02f * f, ay - h);
    };

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

    /// Set multiplier (default 1.0) for a QueryPerformanceCounter hook based speedhack, similar to the one in Cheat Engine. Call without arguments to reset. Also see [set_frametime](#set_frametime)
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

    /// Initializes some seeded run related values and loads the character select screen, as if starting a new seeded run after entering the seed.
    lua["play_seeded"] = init_seeded;

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
    // Allows you to modify the rooms content in the front and back layer as well as add a back layer if not yet existant
    // SAVE
    // Params: SaveContext save_ctx
    // Runs at the same times as ON.SCREEN, but receives the save_ctx
    // LOAD
    // Params: LoadContext load_ctx
    // Runs as soon as your script is loaded, including reloads, then never again
    // RENDER_PRE_GAME
    // Params: VanillaRenderContext render_ctx
    // Runs before the in-game part of the game is rendered. Return `true` to skip rendering.
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
