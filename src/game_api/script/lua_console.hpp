#pragma once

#include "lua_backend.hpp"

#include <optional>

struct ConsoleHistoryItem {
    std::string command;
    std::vector<ScriptMessage> messages;
};

class LuaConsole : public LuaBackend
{
public:
    LuaConsole(SoundManager* sound_manager);
    virtual ~LuaConsole() override
    {
    }

    std::unordered_map<std::string, std::string> console_commands;

    bool enabled{ false };
    bool set_focus{ false };
    std::optional<float> set_scroll;
    char console_input[2048]{};
    std::optional<std::size_t> history_pos;
    std::vector<ConsoleHistoryItem> history;

    using LuaBackend::reset;
    using LuaBackend::pre_update;
    virtual bool pre_draw() override;

    virtual void set_enabled(bool enabled) override;
    virtual bool get_enabled() const override;

    virtual bool get_unsafe() const override;
    virtual const char* get_name() const override;
    virtual const char* get_id() const override;
    virtual const char* get_root() const override;
    virtual const std::filesystem::path& get_root_path() const override;

    void register_command(std::string provider_name, std::string command_name, sol::function cmd);
    void unregister_command(std::string provider_name, std::string command_name);

    std::string execute(std::string code);
    std::string execute_raw(std::string code);

    void toggle();

    std::string dump_api();
};
