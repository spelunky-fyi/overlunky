#pragma once

#include <cstddef>         // for size_t
#include <filesystem>      // for path
#include <functional>      // for equal_to
#include <imgui.h>         // for ImVec2
#include <optional>        // for optional
#include <sol/forward.hpp> // for function
#include <string>          // for string, allocator, hash, basic_string
#include <string_view>     // for string_view
#include <type_traits>     // for move, declval
#include <unordered_map>   // for _Umap_traits<>::allocator_type, unordered...
#include <vector>          // for vector

#include "lua_backend.hpp" // for LuaBackend
#include "script.hpp"      // for ScriptMessage

class SoundManager;

struct ConsoleHistoryItem
{
    std::string command;
    std::vector<ScriptMessage> messages;
};

struct ConsoleResult
{
    std::string str;
    bool error;
};

class LuaConsole : public LockableLuaBackend<LuaConsole>
{
  public:
    LuaConsole(SoundManager* sound_manager);
    virtual ~LuaConsole() override
    {
    }

    std::unordered_map<std::string, LuaBackend*> console_commands_list;

    bool toggled{false};
    bool enabled{true};
    bool unsafe{true};
    bool set_focus{false};
    bool scroll_to_bottom{false};
    std::optional<size_t> set_scroll_to_history_item;
    std::optional<size_t> last_force_scroll;
    char console_input[8192]{};
    bool alt_keys{false};
    int prev_cursor_pos{0};

    bool has_new_history{false};
    size_t max_history{30};
    bool highlight_history{false};
    std::optional<std::size_t> history_pos;
    std::vector<ConsoleHistoryItem> history;

    std::unordered_map<std::string_view, std::string_view> entity_down_cast_map;

    std::string completion_options;
    std::string completion_error;

    ImVec2 pos{0, 0};
    ImVec2 size{0, 0};

    void on_history_request(struct ImGuiInputTextCallbackData* data);
    bool on_completion(struct ImGuiInputTextCallbackData* data);

    using LuaBackend::pre_update;
    using LuaBackend::reset;
    virtual bool pre_draw() override;

    virtual void set_enabled(bool enable) override
    {
        enabled = enable;
    }
    virtual bool get_enabled() const override
    {
        return enabled;
    }

    virtual bool get_unsafe() const override
    {
        return unsafe;
    }
    virtual const char* get_name() const override
    {
        return "lua_console";
    }
    virtual const char* get_id() const override
    {
        return "dev/lua_console";
    }
    virtual const char* get_version() const override
    {
        return "1.337";
    }
    virtual const char* get_path() const override
    {
        return "console_proxy.lua";
    }
    virtual const char* get_root() const override
    {
        return ".";
    }
    virtual const std::filesystem::path& get_root_path() const override;

    void register_command(LuaBackend* provider, std::string command_name, sol::function cmd);
    void unregister_command(LuaBackend* provider, std::string command_name);

    ConsoleResult execute(std::string str, bool raw = false);
    sol::protected_function_result execute_raw(std::string str);

    void toggle();

    void push_history(std::string history_item, std::vector<ScriptMessage> result_item);

    std::string dump_api();
    unsigned int get_input_lines();
    void set_geometry(float x, float y, float w, float h)
    {
        pos = ImVec2(x, y);
        size = ImVec2(w, h);
    }
    void set_alt_keys(bool enable)
    {
        alt_keys = enable;
    }
};
