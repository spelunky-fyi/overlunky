#include "lua_console.hpp"

#include "lua_libs/lua_libs.hpp"
#include "rpc.hpp"
#include "script_util.hpp"

#include <sol/sol.hpp>

LuaConsole::LuaConsole(SoundManager* sound_manager)
    : LuaBackend(sound_manager, this)
{
    require_serpent_lua(lua);
}

void LuaConsole::set_enabled(bool enabled)
{}
bool LuaConsole::get_enabled() const
{
    return true;
}

bool LuaConsole::get_unsafe() const
{
    return true;
}
const char* LuaConsole::get_name() const
{
    return "lua_console";
}
const char* LuaConsole::get_id() const
{
    return "lua_console";
}
const char* LuaConsole::get_root() const
{
    return ".";
}
const std::filesystem::path& LuaConsole::get_root_path() const
{
    static std::filesystem::path root_path{ "." };
    return root_path;
}

void LuaConsole::register_command(std::string provider_name, std::string command_name, sol::function cmd)
{
    lua[command_name] = std::move(cmd);
    console_commands[std::move(command_name)] = std::move(provider_name);
}
void LuaConsole::unregister_command(std::string provider_name, std::string command_name)
{
    if (console_commands[command_name] == provider_name)
    {
        lua[command_name] = sol::nil;
        console_commands.erase(std::move(command_name));
    }
}

std::string LuaConsole::execute(std::string code)
{
    try {
        if (!code.starts_with("return"))
        {
            try {
                return execute_raw("return " + code);
            }
            catch (const sol::error& e)
            {
                return execute_raw(std::move(code));
            }
        }
        else
        {
            return execute_raw(std::move(code));
        }
    }
    catch (const sol::error& e)
    {
        return e.what();
    }
}
std::string LuaConsole::execute_raw(std::string code)
{
    auto ret = lua.safe_script(code);
    if (ret.get_type() == sol::type::nil || ret.get_type() == sol::type::none)
    {
        return "";
    }
    else
    {
        sol::function serpent = lua["serpent"]["block"];
        return serpent(ret);
    }
}

std::string LuaConsole::dump_api()
{
    std::set<std::string> excluded_keys{ "meta" };

    sol::state dummy_state;
    dummy_state.open_libraries(sol::lib::math, sol::lib::base, sol::lib::string, sol::lib::table, sol::lib::coroutine, sol::lib::package, sol::lib::debug);
    require_serpent_lua(dummy_state);

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

    const static std::regex reg(R"("function:\s[0-9A-F]+")");
    api = std::regex_replace(api, reg, R"("function:")");

    return api;
}
