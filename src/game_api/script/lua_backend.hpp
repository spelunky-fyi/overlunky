#pragma once

#include "drops.hpp"
#include "entities_chars.hpp"
#include "entity.hpp"
#include "script.hpp"
#include "window_api.hpp"

#include <algorithm>
#include <deque>
#include <filesystem>
#include <functional>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

#include <imgui.h>
#include <sol/sol.hpp>

using CallbackId = int;
using Flags = std::uint32_t;
using SPAWN_TYPE = int;                  // NoAlias
using VANILLA_SOUND = std::string;       // NoAlias
using VANILLA_SOUND_CALLBACK_TYPE = int; // NoAlias
using BUTTONS = std::uint16_t;           // NoAlias

enum class ON
{
    LOGO = 0,
    INTRO = 1,
    PROLOGUE = 2,
    TITLE = 3,
    MENU = 4,
    OPTIONS = 5,
    LEADERBOARD = 7,
    SEED_INPUT = 8,
    CHARACTER_SELECT = 9,
    TEAM_SELECT = 10,
    CAMP = 11,
    LEVEL = 12,
    TRANSITION = 13,
    DEATH = 14,
    SPACESHIP = 15,
    WIN = 16,
    CREDITS = 17,
    SCORES = 18,
    CONSTELLATION = 19,
    RECAP = 20,
    ARENA_MENU = 21,
    ARENA_INTRO = 25,
    ARENA_MATCH = 26,
    ARENA_SCORE = 27,
    ONLINE_LOADING = 28,
    ONLINE_LOBBY = 29,

    // Custom events
    GUIFRAME = 100,
    FRAME,
    SCREEN,
    START,
    LOADING,
    RESET,
    SAVE,
    LOAD,
    GAMEFRAME,
    PRE_LEVEL_GENERATION,
    POST_ROOM_GENERATION,
    POST_LEVEL_GENERATION,
    SCRIPT_ENABLE,
    SCRIPT_DISABLE
};

struct IntOption
{
    int value;
    int min;
    int max;
};
struct FloatOption
{
    float value;
    float min;
    float max;
};
struct BoolOption
{
    bool value;
};
struct StringOption
{
    std::string value;
};
struct ComboOption
{
    int value;
    std::string options;
};
struct ButtonOption
{
    sol::function on_click;
};
struct ScriptOption
{
    std::string desc;
    std::string long_desc;
    std::variant<IntOption, FloatOption, BoolOption, StringOption, ComboOption, ButtonOption> option_impl;
};

struct IntervalCallback
{
    sol::function func;
    int interval;
    int lastRan;
};

struct TimeoutCallback
{
    sol::function func;
    int timeout;
};

struct ScreenCallback
{
    sol::function func;
    ON screen;
    int lastRan;
};

struct LevelGenCallback
{
    int id;
    std::string tile_code;
    sol::function func;
};

struct EntitySpawnCallback
{
    int id;
    int entity_mask;
    std::vector<uint32_t> entity_types;
    SPAWN_TYPE spawn_type_flags;
    sol::function func;
};

using TimerCallback = std::variant<IntervalCallback, TimeoutCallback>; // NoAlias

struct ScriptState
{
    Player* player;
    uint32_t screen;
    uint32_t time_level;
    uint32_t time_total;
    uint32_t time_global;
    uint32_t frame;
    uint32_t loading;
    uint32_t reset;
    uint32_t quest_flags;
};

struct SaveData;
struct StateMemory;
class SoundManager;
class LuaConsole;

class LuaBackend
{
  public:
    sol::environment lua;
    std::shared_ptr<sol::state> vm;
    std::unordered_set<std::string> loaded_modules;

    std::string result;
    ScriptState state = {nullptr, 0, 0, 0, 0, 0, 0, 0};

    int cbcount = 0;

    std::recursive_mutex gil;

    std::map<std::string, ScriptOption> options;
    std::deque<ScriptMessage> messages;
    std::unordered_map<int, TimerCallback> level_timers;
    std::unordered_map<int, TimerCallback> global_timers;
    std::unordered_map<int, ScreenCallback> callbacks;
    std::unordered_map<int, ScreenCallback> load_callbacks;
    std::vector<std::uint32_t> vanilla_sound_callbacks;
    std::vector<LevelGenCallback> pre_level_gen_callbacks;
    std::vector<LevelGenCallback> post_level_gen_callbacks;
    std::vector<EntitySpawnCallback> pre_entity_spawn_callbacks;
    std::vector<EntitySpawnCallback> post_entity_spawn_callbacks;
    std::vector<std::uint32_t> chance_callbacks;
    std::vector<int> clear_callbacks;
    std::vector<std::pair<int, std::uint32_t>> entity_hooks;
    std::vector<std::pair<int, std::uint32_t>> clear_entity_hooks;
    std::vector<std::pair<int, std::uint32_t>> entity_dtor_hooks;
    std::vector<std::string> required_scripts;
    std::unordered_map<int, ScriptInput*> script_input;
    std::unordered_set<std::string> windows;
    std::unordered_set<std::string> console_commands;

    ImDrawList* draw_list{nullptr};

    StateMemory* g_state = nullptr;

    SoundManager* sound_manager;
    LuaConsole* console;

    std::map<int, ScriptImage*> images;

    LuaBackend(SoundManager* sound_manager, LuaConsole* console);
    virtual ~LuaBackend();

    template <class... Args>
    bool handle_function(sol::function func, Args&&... args);
    template <class Ret, class... Args>
    std::optional<Ret> handle_function_with_return(sol::function func, Args&&... args);

    void clear();
    void clear_all_callbacks();

    virtual bool reset();
    virtual bool pre_draw()
    {
        return true;
    }
    virtual bool pre_update()
    {
        return true;
    };

    virtual void set_enabled(bool enabled) = 0;
    virtual bool get_enabled() const = 0;

    virtual bool get_unsafe() const = 0;
    virtual const char* get_name() const = 0;
    virtual const char* get_id() const = 0;
    virtual const char* get_path() const = 0;
    virtual const char* get_root() const = 0;
    virtual const std::filesystem::path& get_root_path() const = 0;

    bool update();
    void draw(ImDrawList* dl);
    void render_options();

    bool pre_level_gen_spawn(std::string_view tile_code, float x, float y, int layer);
    void post_level_gen_spawn(std::string_view tile_code, float x, float y, int layer);

    void pre_level_generation();
    void post_room_generation();
    void post_level_generation();

    Entity* pre_entity_spawn(std::uint32_t entity_type, float x, float y, int layer, Entity* overlay, int spawn_type_flags);
    void post_entity_spawn(Entity* entity, int spawn_type_flags);

    void hook_entity_dtor(Entity* entity);
    void pre_entity_destroyed(Entity* entity);

    static void for_each_backend(std::function<bool(LuaBackend&)> fun);
    static LuaBackend* get_backend(std::string_view id);
    static LuaBackend* get_calling_backend();
};

template <class... Args>
bool LuaBackend::handle_function(sol::function func, Args&&... args)
{
    return handle_function_with_return<std::monostate>(std::move(func), std::forward<Args>(args)...) != std::nullopt;
}
template <class Ret, class... Args>
std::optional<Ret> LuaBackend::handle_function_with_return(sol::function func, Args&&... args)
{
    auto lua_result = func(std::forward<Args>(args)...);
    if (!lua_result.valid())
    {
        sol::error e = lua_result;
        result = e.what();
#ifdef SPEL2_EXTRA_ANNOYING_SCRIPT_ERRORS
        std::istringstream errors(result);
        while (!errors.eof())
        {
            std::string eline;
            getline(errors, eline);
            messages.push_back({eline, std::chrono::system_clock::now(), ImVec4(1.0f, 0.2f, 0.2f, 1.0f)});
            DEBUG("{}", result);
            if (messages.size() > 30)
                messages.pop_front();
        }
#endif
        return std::nullopt;
    }
    if constexpr (std::is_same_v<Ret, std::monostate>)
    {
        return std::optional{std::monostate{}};
    }
    else
    {
        try
        {
            auto return_type = lua_result.get_type();
            return return_type == sol::type::none || return_type == sol::type::nil
                       ? std::optional<Ret>{}
                       : std::optional{static_cast<Ret>(lua_result)};
        }
        catch (...)
        {
            result = "Unexpected return type from function.";
        }
    }
    return std::nullopt;
}
