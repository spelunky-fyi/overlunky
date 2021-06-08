#include "drops.hpp"
#include "entity.hpp"
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

using CallbackId = int;
using Flags = std::uint32_t;
using uColor = ImU32;
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
    GUIFRAME = 100,
    FRAME = 101,
    SCREEN = 102,
    START = 103,
    LOADING = 104,
    RESET = 105,
    SAVE = 106,
    LOAD = 107,
    GAMEFRAME = 108,
    SCRIPT_ENABLE = 109,
    SCRIPT_DISABLE = 110
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

class ScriptImpl
{
  public:
    sol::state lua;

#ifdef SPEL2_EDITABLE_SCRIPTS
    char code[204800];
#else
    std::string code_storage;
    const char* code;
#endif
    std::string result = "";
    ScriptState state = {nullptr, 0, 0, 0, 0, 0, 0, 0};
    bool changed = true;
    bool enabled = true;
    ScriptMeta meta = {"", "", "", "", "", "", "", "", "", false};
    std::filesystem::path script_folder;
    int cbcount = 0;

    std::recursive_mutex gil;

    std::map<std::string, ScriptOption> options;
    std::deque<ScriptMessage> messages;
    std::map<int, TimerCallback> level_timers;
    std::map<int, TimerCallback> global_timers;
    std::map<int, ScreenCallback> callbacks;
    std::map<int, ScreenCallback> load_callbacks;
    std::vector<std::uint32_t> vanilla_sound_callbacks;
    std::vector<LevelGenCallback> pre_level_gen_callbacks;
    std::vector<LevelGenCallback> post_level_gen_callbacks;
    std::vector<int> clear_callbacks;
    std::vector<std::string> required_scripts;
    std::map<int, ScriptInput*> script_input;
    std::set<std::string> windows;

    ImDrawList* draw_list{nullptr};

    StateMemory* g_state = nullptr;
    std::vector<EntityItem> g_items;
    std::vector<Player*> g_players;
    SaveData* g_save = nullptr;

    SoundManager* sound_manager;

    std::map<int, ScriptImage*> images;

    ScriptImpl(std::string script, std::string file, SoundManager* sound_manager, bool enable = true);
    ~ScriptImpl()
    {
        set_enabled(false);
        clear();
    }

    std::string script_id();
    template <class... Args>
    bool handle_function(sol::function func, Args&&... args);
    template <class Ret, class... Args>
    std::optional<Ret> handle_function_with_return(sol::function func, Args&&... args);

    void clear();
    bool reset();
    void set_enabled(bool enabled);

    bool run();
    void draw(ImDrawList* dl);
    void render_options();

    bool pre_level_gen_spawn(std::string_view tile_code, float x, float y, int layer);
    void post_level_gen_spawn(std::string_view tile_code, float x, float y, int layer);

    std::string dump_api();
};

template <class... Args>
bool ScriptImpl::handle_function(sol::function func, Args&&... args)
{
    return handle_function_with_return<std::monostate>(std::move(func), std::forward<Args>(args)...) != std::nullopt;
}
template <class Ret, class... Args>
std::optional<Ret> ScriptImpl::handle_function_with_return(sol::function func, Args&&... args)
{
    auto lua_result = func(std::forward<Args>(args)...);
    if (!lua_result.valid())
    {
        sol::error e = lua_result;
        result = e.what();
#ifdef SPEL2_EXTRA_ANNOYING_SCRIPT_ERRORS
        messages.push_back({result, std::chrono::system_clock::now(), ImVec4(1.0f, 0.2f, 0.2f, 1.0f)});
        DEBUG("{}", result);
        if (messages.size() > 20)
            messages.pop_front();
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
