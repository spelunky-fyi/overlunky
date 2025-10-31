#include "lua_require.hpp"

#include <algorithm>     // for replace, mismatch
#include <exception>     // for exception
#include <filesystem>    // for path, operator==, exists, operator/, _Pat...
#include <fmt/format.h>  // for check_format_string, format, vformat
#include <fstream>       // for filesystem
#include <lauxlib.h>     // for luaL_loadfilex
#include <lua.h>         // for lua_setupvalue, lua_State, LUA_OK
#include <new>           // for operator new
#include <optional>      // for optional, nullopt
#include <sol/sol.hpp>   // for proxy_key_t, table_proxy, state, protecte...
#include <string_view>   // for string_view
#include <tuple>         // for get
#include <type_traits>   // for move, declval, conditional_t, forward
#include <unordered_set> // for unordered_set
#include <utility>       // for min, max, pair, tuple_element<>::type

#include "lua_backend.hpp" // for LuaBackend
#include "lua_vm.hpp"      // for get_lua_vm

void register_custom_require(sol::state& lua)
{
    lua.clear_package_loaders();
    lua.add_package_loader(custom_loader);

    lua["__require"] = lua["require"];
    lua["__loadlib"] = lua["package"]["loadlib"];

    /// Custom implementation to trick Lua into allowing to `require 'lib.module'` more than once given it was called from a different source
    lua["require"] = custom_require;

    lua["package"]["loadlib"] = custom_loadlib;
}
sol::object custom_require(std::string path)
{
    static sol::state& lua = get_lua_vm();

    if (path == "io" || path == "os" || path == "math" || path == "string" || path == "table" || path == "coroutine" || path == "package")
        return lua[path];

    // Turn module into a real path
    {
        if (path.ends_with(".lua") || path.ends_with(".dll"))
        {
            path = path.substr(0, path.size() - 4);
        }
        std::replace(path.begin(), path.end(), '.', '/');
        std::replace(path.begin(), path.end(), ':', '.');
    }

    // Could be preloaded by some unsafe script, which can only be fetched by unsafe scripts
    auto backend = LuaBackend::get_calling_backend();
    const bool unsafe = backend->get_unsafe();
    if (unsafe)
    {
        auto preload = lua["package"]["preload"][path];
        if (preload != sol::nil)
        {
            return preload;
        }
    }

    // Walk up the stack until we find an _ENV that is not global, then grab the source from that stack index
    auto [short_source, source] = []() -> std::pair<std::string_view, std::string_view>
    {
        return lua.safe_script(R"(
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

local info = debug.getinfo(up)
return info.short_src, info.source
)");
    }();

    if (short_source.starts_with("[string"))
    {
        source = backend->get_root();
    }
    else
    {
        auto last_slash = source.find_last_of("/\\");
        if (last_slash != std::string::npos)
        {
            source = source.substr(0, last_slash);
        }
        if (source.starts_with('@'))
        {
            source = source.substr(1);
        }
    }

    namespace fs = std::filesystem;
    static auto is_sub_path = [](const std::filesystem::path& base, const std::filesystem::path& _path)
    {
        const auto first_mismatch = std::mismatch(_path.begin(), _path.end(), base.begin(), base.end());
        return first_mismatch.second == base.end();
    };

    const fs::path& backend_root = backend->get_root_path();

    auto require = [&](std::string _path)
    {
        if (_path.ends_with(".lua") || _path.ends_with(".dll"))
        {
            _path = _path.substr(0, _path.size() - 4);
        }
        std::replace(_path.begin(), _path.end(), '.', ':'); // Need to be able to recover periods in folder names, curses garebear
        std::replace(_path.begin(), _path.end(), '/', '.');
        std::replace(_path.begin(), _path.end(), '\\', '.');
        backend->loaded_modules.insert(_path);
        return lua["__require"](_path);
    };
    auto require_if_exists = [&](fs::path _path) -> std::optional<sol::object>
    {
        if (!unsafe && !is_sub_path(backend_root, _path))
        {
            return std::nullopt;
        }

        if (fs::exists(_path.replace_extension(".lua")))
        {
            return require(_path.string());
        }
        else if (unsafe && fs::exists(_path.replace_extension(".dll")))
        {
            return require(_path.string());
        }
        else
        {
            _path.replace_extension() /= "init";
            if (fs::exists(_path.replace_extension(".lua")))
            {
                return require(_path.string());
            }
        }
        return std::nullopt;
    };

    /// Valid safe options:
    // path/to/calling/script/path/to/require.lua
    // path/to/calling/script/path/to/require/init.lua
    // path/to/calling/mod/path/to/require.lua
    // path/to/calling/mod/path/to/require/init.lua
    // path/to/require.lua
    // path/to/require/init.lua

    /// Valid unsafe options:
    // path/to/calling/script/path/to/require.dll
    // path/to/calling/mod/path/to/require.dll
    // path/to/require.dll

    /// Only unsafe backends can require files that are not relative to their root

    auto res = require_if_exists(std::filesystem::path(source) / path);
    if (!res && source != backend->get_root())
    {
        res = require_if_exists(std::filesystem::path(backend->get_root()) / path);
    }
    if (!res)
    {
        res = require_if_exists(std::filesystem::path(path));
    }

    return std::move(res).value_or(sol::nil);
}
sol::object custom_loadlib(std::string path, std::string func)
{
    static sol::state& lua = get_lua_vm();
    auto backend = LuaBackend::get_calling_backend();
    if (!backend->get_unsafe())
        return sol::nil;

    // Walk up the stack until we find an _ENV that is not global, then grab the source from that stack index
    auto [short_source, source] = []() -> std::pair<std::string_view, std::string_view>
    {
        return lua.safe_script(R"(
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

local info = debug.getinfo(up)
return info.short_src, info.source
)");
    }();

    if (short_source.starts_with("[string"))
    {
        source = backend->get_root();
    }
    else
    {
        auto last_slash = source.find_last_of("/\\");
        if (last_slash != std::string::npos)
        {
            source = source.substr(0, last_slash);
        }
        if (source.starts_with('@'))
        {
            source = source.substr(1);
        }
    }

    namespace fs = std::filesystem;

    auto loadlib_if_exists = [](fs::path _path, std::string _func) -> std::optional<sol::object>
    {
        if (std::filesystem::exists(_path))
            return lua["__loadlib"](_path.string(), _func);
        return std::nullopt;
    };

    /// Valid unsafe options:
    // path/to/calling/script/path/to/lib.dll
    // path/to/calling/mod/path/to/lib.dll
    // path/to/lib.dll

    auto res = loadlib_if_exists(std::filesystem::path(source) / path, func);

    if (!res && source != backend->get_root())
    {
        res = loadlib_if_exists(std::filesystem::path(backend->get_root()) / path, func);
    }
    if (!res)
    {
        res = loadlib_if_exists(std::filesystem::path(path), func);
    }

    return std::move(res).value_or(sol::nil);
}
int custom_loader(lua_State* L)
{
    std::string path = sol::stack::get<std::string>(L, 1);
    std::replace(path.begin(), path.end(), '.', '/');
    std::replace(path.begin(), path.end(), ':', '.');
    auto backend = LuaBackend::get_calling_backend();

    auto try_load = [&](std::string _path, std::string_view ext)
    {
        if (ext == ".dll")
        {
            auto root = std::string(backend->get_root());
            auto module = _path.substr(root.size() + 1);
            auto func = "luaopen_" + module;
            std::replace(func.begin(), func.end(), '/', '_');
            std::replace(func.begin(), func.end(), '.', '_');
            sol::stack::push(L, backend->lua["__loadlib"](_path, func));
            return true;
        }
        _path += ext;
        const auto res = luaL_loadfile(L, _path.c_str());
        if (res == LUA_OK)
        {
            backend->lua.push();
            //  The first up-value is always the _ENV of the chunk
            lua_setupvalue(L, -2, 1);
            return true;
        }
        return false;
    };

    if (try_load(path, ".lua"))
    {
        return 1;
    }

    if (backend->get_unsafe() && try_load(path, ".dll"))
    {
        return 1;
    }

    sol::stack::push(L, fmt::format("Could not not find file '{}'...", path));
    return 1;
}
