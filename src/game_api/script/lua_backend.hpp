#pragma once

#include <algorithm>     // for max
#include <chrono>        // for system_clock
#include <cstdint>       // for uint32_t, uint16_t, uint8_t, int32_t
#include <deque>         // for deque
#include <filesystem>    // for path
#include <functional>    // for equal_to, function, less
#include <imgui.h>       // for ImDrawList (ptr only), ImVec4
#include <locale>        // for num_get, num_put
#include <map>           // for map
#include <memory>        // for allocator, shared_ptr
#include <mutex>         // for recursive_mutex
#include <new>           // for operator new
#include <optional>      // for operator==, optional, nullopt
#include <sol/sol.hpp>   // for object, basic_object, basic_protected_function
#include <sstream>       // for basic_istringstream, istringstream, basic_s...
#include <string>        // for string, hash, getline, u16string, basic_string
#include <string_view>   // for string_view
#include <type_traits>   // for move, hash, declval, forward
#include <unordered_map> // for _Umap_traits<>::allocator_type, unordered_map
#include <unordered_set> // for _Uset_traits<>::allocator_type, _Uset_trait...
#include <utility>       // for max, min, pair, monostate
#include <variant>       // for variant
#include <vector>        // for vector

#include "aliases.hpp"   // for IMAGE, JournalPageType, SPAWN_TYPE
#include "level_api.hpp" // IWYU pragma: keep
#include "logger.h"      // for DEBUG
#include "script.hpp"    // for ScriptMessage, ScriptImage (ptr only), Scri...

class Player;
class JournalPage;
class Entity;
struct LevelGenRoomData;
struct AABB;

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
    PRE_LOAD_LEVEL_FILES,
    PRE_LEVEL_GENERATION,
    POST_ROOM_GENERATION,
    POST_LEVEL_GENERATION,
    PRE_GET_RANDOM_ROOM,
    PRE_HANDLE_ROOM_TILES,
    SCRIPT_ENABLE,
    SCRIPT_DISABLE,
    RENDER_PRE_HUD,
    RENDER_POST_HUD,
    RENDER_PRE_PAUSE_MENU,
    RENDER_POST_PAUSE_MENU,
    RENDER_PRE_DRAW_DEPTH,
    RENDER_POST_DRAW_DEPTH,
    RENDER_POST_JOURNAL_PAGE,
    SPEECH_BUBBLE,
    TOAST,
    PRE_LOAD_SCREEN,
    POST_LOAD_SCREEN,
    DEATH_MESSAGE,
    EVERYFRAME,
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

enum class CallbackType
{
    None,
    Normal,
    Entity,
    Screen
};

struct CurrentCallback
{
    int uid;
    int id;
    CallbackType type;
};

struct VanillaMovableBehavior;
struct CustomMovableBehavior;

struct CustomMovableBehaviorStorage
{
    std::string name;
    // Stored as std::shared_ptr<void> since MovableBehavior has no virtual dtor but
    // std::shared_ptr<void> takes care of the type erasure to correctly destroy it
    std::shared_ptr<void> behavior;
};

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

struct UserData
{
    sol::object data;
    std::uint32_t hook_id;
};

struct SavedUserData
{
    sol::optional<sol::object> self;
    sol::optional<sol::object> held;
    sol::optional<sol::object> mount;
    std::unordered_map<uint32_t, sol::object> powerups;
};

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
    ScriptState state = {nullptr, 0, 0, 0, 0, 0, 0, 0, 0};

    int cbcount = 0;
    CurrentCallback current_cb = {-1, 0, CallbackType::None};

    std::recursive_mutex gil;

    std::map<std::string, ScriptOption> options;
    std::deque<ScriptMessage> messages;
    std::unordered_map<int, TimerCallback> level_timers;
    std::unordered_map<int, TimerCallback> global_timers;
    std::unordered_map<int, ScreenCallback> callbacks;
    std::unordered_map<int, ScreenCallback> load_callbacks;
    std::vector<std::uint32_t> vanilla_sound_callbacks;
    std::vector<LevelGenCallback> pre_tile_code_callbacks;
    std::vector<LevelGenCallback> post_tile_code_callbacks;
    std::vector<EntitySpawnCallback> pre_entity_spawn_callbacks;
    std::vector<EntitySpawnCallback> post_entity_spawn_callbacks;
    std::vector<std::uint32_t> chance_callbacks;
    std::vector<std::uint32_t> extra_spawn_callbacks;
    std::vector<int> clear_callbacks;
    std::vector<std::pair<int, std::uint32_t>> entity_hooks;
    std::vector<std::pair<int, std::uint32_t>> clear_entity_hooks;
    std::vector<std::pair<int, std::uint32_t>> entity_dtor_hooks;
    std::vector<std::pair<int, std::uint32_t>> screen_hooks;
    std::vector<std::pair<int, std::uint32_t>> clear_screen_hooks;
    std::vector<CustomMovableBehaviorStorage> custom_movable_behaviors;
    std::unordered_map<std::uint32_t, UserData> user_datas;
    std::unordered_map<int, SavedUserData> saved_user_datas;
    std::vector<std::string> required_scripts;
    std::unordered_map<int, ScriptInput*> script_input;
    std::unordered_set<std::string> windows;
    std::unordered_set<std::string> console_commands;

    ImDrawList* draw_list{nullptr};

    StateMemory* g_state = nullptr;

    SoundManager* sound_manager;
    LuaConsole* console;

    std::map<IMAGE, ScriptImage*> images;

    LuaBackend(SoundManager* sound_manager, LuaConsole* console);
    virtual ~LuaBackend();

    template <class... Args>
    bool handle_function(sol::function func, Args&&... args);
    template <class Ret, class... Args>
    std::optional<Ret> handle_function_with_return(sol::function func, Args&&... args);

    void clear();
    void clear_all_callbacks();
    bool frame_update(StateMemory* state_mem);

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
    virtual const char* get_version() const = 0;
    virtual const char* get_path() const = 0;
    virtual const char* get_root() const = 0;
    virtual const std::filesystem::path& get_root_path() const = 0;

    CustomMovableBehavior* get_custom_movable_behavior(std::string_view name);
    CustomMovableBehavior* make_custom_movable_behavior(std::string_view name, uint8_t state_id, VanillaMovableBehavior* base_behavior);

    sol::object get_user_data(uint32_t uid);
    sol::object get_user_data(Entity& entity);
    void set_user_data(uint32_t uid, sol::object user_data);
    void set_user_data(Entity& entity, sol::object user_data);

    bool update();
    void draw(ImDrawList* dl);
    void render_options();

    bool is_callback_cleared(int32_t callback_id);
    bool is_entity_callback_cleared(std::pair<int, uint32_t> callback_id);
    bool is_screen_callback_cleared(std::pair<int, uint32_t> callback_id);

    bool pre_tile_code(std::string_view tile_code, float x, float y, int layer, uint16_t room_template);
    void post_tile_code(std::string_view tile_code, float x, float y, int layer, uint16_t room_template);

    void pre_load_level_files();
    void pre_level_generation();
    bool pre_load_screen();
    void post_room_generation();
    void post_level_generation();
    void post_load_screen();
    void on_death_message(STRINGID stringid);

    std::string pre_get_random_room(int x, int y, uint8_t layer, uint16_t room_template);
    struct PreHandleRoomTilesResult
    {
        bool stop_callback;
        std::optional<LevelGenRoomData> modded_room_data;
    };
    PreHandleRoomTilesResult pre_handle_room_tiles(LevelGenRoomData room_data, int x, int y, uint16_t room_template);

    Entity* pre_entity_spawn(std::uint32_t entity_type, float x, float y, int layer, Entity* overlay, int spawn_type_flags);
    void post_entity_spawn(Entity* entity, int spawn_type_flags);

    void hook_entity_dtor(Entity* entity);
    void pre_entity_destroyed(Entity* entity);

    void process_vanilla_render_callbacks(ON event);
    void process_vanilla_render_draw_depth_callbacks(ON event, uint8_t draw_depth, const AABB& bbox);
    void process_vanilla_render_journal_page_callbacks(ON event, JournalPageType page_type, JournalPage* page);

    std::u16string pre_speach_bubble(Entity* entity, char16_t* buffer);
    std::u16string pre_toast(char16_t* buffer);

    static void for_each_backend(std::function<bool(LuaBackend&)> fun);
    static LuaBackend* get_backend(std::string_view id);
    static LuaBackend* get_backend_by_id(std::string_view id, std::string_view ver = "");
    static LuaBackend* get_calling_backend();

    CurrentCallback get_current_callback();
    void set_current_callback(int uid, int id, CallbackType type);
    void clear_current_callback();
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
            if (return_type == sol::type::none || return_type == sol::type::nil)
            {
                return std::optional<Ret>{};
            }
            Ret return_value = lua_result;
            return return_value;
        }
        catch (...)
        {
            result = "Unexpected return type from function.";
            return std::nullopt;
        }
    }
}
