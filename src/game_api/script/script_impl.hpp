#pragma once

#include "lua_backend.hpp"

class ScriptImpl : public LuaBackend
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
    virtual bool pre_update() override;

    virtual void set_enabled(bool enabled) override;
    virtual bool get_enabled() const override;

    virtual bool get_unsafe() const override;
    virtual const char* get_name() const override;
    virtual const char* get_id() const override;
    virtual const char* get_version() const override;
    virtual const char* get_path() const override;
    virtual const char* get_root() const override;
    virtual const std::filesystem::path& get_root_path() const override;
};
