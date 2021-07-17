#pragma once

#include "drops.hpp"
#include "entity.hpp"
#include "lua_backend.hpp"
#include "script.hpp"
#include "window_api.hpp"

#include <algorithm>
#include <deque>
#include <filesystem>
#include <map>
#include <mutex>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include <imgui.h>
#include <sol/sol.hpp>

class ScriptImpl : public LuaBackend
{
  public:
#ifdef SPEL2_EDITABLE_SCRIPTS
    char code[204800];
#else
    std::string code_storage;
    const char* code;
#endif

    bool changed = true;
    bool enabled = true;
    ScriptMeta meta = {"", "", "", "", "", "", "", "", "", false};
    std::filesystem::path script_folder;

    ScriptImpl(std::string script, std::string file, SoundManager* sound_manager, bool enable = true);
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
    virtual const char* get_root() const override;
    virtual const std::filesystem::path& get_root_path() const override;
};
