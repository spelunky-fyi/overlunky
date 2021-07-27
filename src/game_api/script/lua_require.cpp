#include "lua_require.hpp"

#include "lua_backend.hpp"
#include "lua_vm.hpp"

#include <sol/sol.hpp>

sol::object custom_require(std::string path)
{
    if (path.ends_with(".lua"))
    {
        path = path.substr(0, path.size() - 4);
    }

    static sol::state& lua = get_lua_vm();

    // Walk up the stack until we find an _ENV that is not global, then grab the source from that stack index
    std::pair<std::string_view, std::string_view> sources = lua.safe_script(R"(
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
    auto [short_source, source] = sources;

    LuaBackend* backend = LuaBackend::get_calling_backend();
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

    std::string resolved_path = (std::filesystem::path(source) / path).string();
    std::replace(resolved_path.begin(), resolved_path.end(), '/', '.');
    std::replace(resolved_path.begin(), resolved_path.end(), '\\', '.');
    sol::stack::push(lua.lua_state(), resolved_path);
    return lua["__require"](resolved_path);
}
int custom_loader(lua_State* L)
{
    std::string path = sol::stack::get<std::string>(L, 1);
    if (!path.ends_with(".lua"))
    {
        std::replace(path.begin(), path.end(), '.', '/');
        path += ".lua";
    }

    LuaBackend* backend = LuaBackend::get_calling_backend();

    const auto res = luaL_loadfilex(L, path.c_str(), "bt");
    if (res == LUA_OK)
    {
        backend->lua.push();
        // The first up-value is always the _ENV of the function
        lua_setupvalue(L, -2, 1);
        return 1;
    }

    sol::stack::push(L, fmt::format("Could not not find file '{}'...", path));
    return 1;
}
