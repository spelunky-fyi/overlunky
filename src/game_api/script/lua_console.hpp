#pragma once

#include "lua_backend.hpp"

#include <optional>

struct ConsoleHistoryItem
{
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

    std::unordered_map<std::string, LuaBackend*> console_commands;

    bool enabled{false};
    bool set_focus{false};
    bool scroll_to_bottom{false};
    std::optional<size_t> set_scroll_to_history_item;
    char console_input[2048]{};

    size_t max_history{30};
    std::optional<std::size_t> history_pos;
    std::vector<ConsoleHistoryItem> history;

    std::unordered_map<std::string_view, std::string_view> entity_down_cast_map;

    std::string completion_options;
    std::string completion_error;

    void on_history_request(struct ImGuiInputTextCallbackData* data);
    void on_completion(struct ImGuiInputTextCallbackData* data);

    using LuaBackend::pre_update;
    using LuaBackend::reset;
    virtual bool pre_draw() override;

    virtual void set_enabled(bool enabled) override;
    virtual bool get_enabled() const override;

    virtual bool get_unsafe() const override;
    virtual const char* get_name() const override;
    virtual const char* get_id() const override;
    virtual const char* get_root() const override;
    virtual const std::filesystem::path& get_root_path() const override;

    void register_command(LuaBackend* provider, std::string command_name, sol::function cmd);
    void unregister_command(LuaBackend* provider, std::string command_name);

    std::string execute(std::string code);
    std::string execute_raw(std::string code);

    void toggle();

    std::string dump_api();
};
