#include "script_impl.hpp"

#include <chrono>        // for system_clock
#include <cstring>       // for memchr
#include <deque>         // for deque
#include <exception>     // for exception
#include <fmt/format.h>  // for format_error
#include <imgui.h>       // for ImVec4
#include <list>          // for _List_iterator, _List_const_iterator
#include <locale>        // for num_get, num_put
#include <mutex>         // for lock_guard
#include <new>           // for operator new
#include <regex>         // for regex_search, regex
#include <sol/sol.hpp>   // for table_proxy, optional, basic_envir...
#include <sstream>       // for basic_istringstream, istringstream
#include <tuple>         // for get
#include <type_traits>   // for move, conditional_t
#include <unordered_map> // for unordered_map
#include <utility>       // for max, min

#include "logger.h"                       // for DEBUG
#include "lua_vm.hpp"                     // for execute_lua, get_lua_vm
#include "script/handle_lua_function.hpp" // for handle_function
#include "script/lua_backend.hpp"         // for LuaBackend, ON, ON::SCRIPT_DISABLE
#include "script_util.hpp"                // for sanitize
#include "thread_utils.hpp"               // for HeapBase

class LuaConsole;
class SoundManager;

ScriptImpl::ScriptImpl(std::string script, std::string file, SoundManager* sound_mgr, LuaConsole* con, bool enable)
    : LockableLuaBackend<ScriptImpl>(sound_mgr, con)
{
#ifdef SPEL2_EDITABLE_SCRIPTS
    code = script;
#else
    code = std::move(script);
#endif

    meta.file = std::move(file);
    meta.path = std::filesystem::path(meta.file).parent_path().string();
    meta.filename = std::filesystem::path(meta.file).filename().string();
    meta.stem = std::filesystem::path(meta.file).stem().string();

    script_folder = std::filesystem::path(meta.path);

    enabled = enable;

    /// Table of strings where you should set some script metadata shown in the UI and used by other scripts to find your script.
    lua["meta"] = get_lua_vm().create_named_table("meta");

    try
    {
        std::string metacode = "";
        std::stringstream metass(code);
        std::regex reg("(^\\s*meta\\.[a-zA-Z_][a-zA-Z0-9_]*\\s*=)");
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
        auto lua_result = execute_lua(lua, metacode);
        sol::optional<std::string> meta_name = lua["meta"]["name"];
        sol::optional<std::string> meta_version = lua["meta"]["version"];
        sol::optional<std::string> meta_description = lua["meta"]["description"];
        sol::optional<std::string> meta_author = lua["meta"]["author"];
        sol::optional<bool> meta_unsafe = lua["meta"]["unsafe"];
        sol::optional<bool> meta_online_safe = lua["meta"]["online_safe"];
        meta.name = meta_name.value_or(meta.filename);
        meta.version = meta_version.value_or("");
        meta.description = meta_description.value_or("");
        meta.author = meta_author.value_or("Anonymous");
        meta.unsafe = meta_unsafe.value_or(false);
        meta.online_safe = meta_online_safe.value_or(false);
        meta.id = script_id();
        lua["__script_id"] = meta.file;

        result = "Got metadata";
    }
    catch (const sol::error& e)
    {
        set_error(e.what());
    }
}

std::string ScriptImpl::script_id()
{
    std::string newid = sanitize(meta.author) + "/" + sanitize(meta.name);
    return newid;
}

bool ScriptImpl::reset()
{
    LuaBackend::reset();

    // Compile & Evaluate the script if the script is changed
    try
    {
        auto lua_result = execute_lua(lua, code);

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
        lua["__script_id"] = meta.file;

        result = "OK";
        return true;
    }
    catch (const sol::error& e)
    {
        set_error(e.what());
        return false;
    }
}

void ScriptImpl::set_enabled(bool enbl)
{
    if (enbl != enabled)
    {
        auto cb_type = enbl ? ON::SCRIPT_ENABLE : ON::SCRIPT_DISABLE;
        auto now = HeapBase::get().frame_count();
        for (auto& [id, callback] : callbacks)
        {
            if (callback.screen == cb_type)
            {
                handle_function<void>(this, callback.func);
                callback.lastRan = now;
            }
        }
    }
    enabled = enbl;
}

std::string ScriptImpl::execute(std::string str, bool raw)
{
    sol::protected_function_result res;
    if (!str.starts_with("return") && !raw)
    {
        res = execute_raw("return " + str);
        if (!res.valid())
            res = execute_raw(std::move(str));
    }
    else
    {
        res = execute_raw(std::move(str));
    }
    if (!res.valid())
    {
        sol::error err = res;
        return err.what();
    }
    if (res.get_type() == sol::type::nil || res.get_type() == sol::type::none)
        return "";
    sol::function serpent = lua["serpent"]["block"];
    return serpent(res);
}

sol::protected_function_result ScriptImpl::execute_raw(std::string str)
{
    return execute_lua(lua, str, true);
}
