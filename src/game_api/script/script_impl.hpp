#pragma once

#include <filesystem> // for path
#include <string>     // for string

#include "lua_backend.hpp" // for LuaBackend
#include "script.hpp"      // for ScriptMeta

class LuaConsole;
class SoundManager;

class ScriptImpl : public LockableLuaBackend<ScriptImpl>
{
  public:
#ifdef SPEL2_EDITABLE_SCRIPTS
    std::string code;
#else
    std::string code;
#endif

    bool changed = true;
    bool enabled = true;
    ScriptMeta meta = {"", "", "", "", "", "", "", "", "", false};
    std::filesystem::path script_folder;

    ScriptImpl(std::string script, std::string file, SoundManager* sound_manager, LuaConsole* con, bool enable = true);
    virtual ~ScriptImpl() override
    {
        set_enabled(false);
    }

    std::string script_id();

    virtual bool reset() override;
    virtual bool pre_update() override
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

    virtual void set_enabled(bool enabled) override;
    virtual bool get_enabled() const override
    {
        return enabled;
    }
    virtual bool get_unsafe() const override
    {
        return meta.unsafe;
    }
    virtual const char* get_name() const override
    {
        return meta.stem.c_str();
    }
    virtual const char* get_id() const override
    {
        return meta.id.c_str();
    }
    virtual const char* get_version() const override
    {
        return meta.version.c_str();
    }
    virtual const char* get_path() const override
    {
        return meta.file.c_str();
    }
    virtual const char* get_root() const override
    {
        return meta.path.c_str();
    }
    virtual const std::filesystem::path& get_root_path() const override
    {
        return script_folder;
    }

    std::string execute(std::string str, bool raw = false);
    sol::protected_function_result execute_raw(std::string str);
};
