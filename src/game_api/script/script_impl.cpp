#include "script_impl.hpp"

#include "lua_vm.hpp"
#include "rpc.hpp"
#include "script_util.hpp"

#include <sol/sol.hpp>

ScriptImpl::ScriptImpl(std::string script, std::string file, SoundManager* sound_mgr, LuaConsole* con, bool enable)
    : LuaBackend(sound_mgr, con)
{
    // Needs to be populated for reliable cleanup later
    name = get_name();

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

    script_folder = std::filesystem::path(meta.path);

    enabled = enable;

    /// Table of strings where you should set some script metadata shown in the UI.
    /// - `meta.name` Script name
    /// - `meta.version` Version
    /// - `meta.description` Short description of the script
    /// - `meta.author` Your name
    lua["meta"] = get_lua_vm().create_named_table("meta");

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
        auto lua_result = execute_lua(lua, metacode);
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
        lua["__script_id"] = meta.id;

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
        std::lock_guard gil_guard{gil};
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
        lua["__script_id"] = meta.id;

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
bool ScriptImpl::pre_update()
{
    if (changed)
    {
        result = "";
        changed = false;
        if (!reset())
        {
            return false;
        }
    }
    return true;
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
bool ScriptImpl::get_enabled() const
{
    return enabled;
}

bool ScriptImpl::get_unsafe() const
{
    return meta.unsafe;
}
const char* ScriptImpl::get_name() const
{
    return meta.stem.c_str();
}
const char* ScriptImpl::get_id() const
{
    return meta.id.c_str();
}
const char* ScriptImpl::get_root() const
{
    return meta.path.c_str();
}
const std::filesystem::path& ScriptImpl::get_root_path() const
{
    return script_folder;
}
