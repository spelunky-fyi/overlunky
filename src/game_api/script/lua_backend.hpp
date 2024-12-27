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

#include "aliases.hpp"      // for IMAGE, JournalPageType, SPAWN_TYPE
#include "hook_handler.hpp" // for HookHandler
#include "level_api.hpp"    // IWYU pragma: keep
#include "logger.h"         // for DEBUG
#include "script.hpp"       // for ScriptMessage, ScriptImage (ptr only), Scri...
#include "util.hpp"         // for GlobalMutexProtectedResource, ON_SCOPE_EXIT

extern std::recursive_mutex global_lua_lock;

class Player;
class JournalPage;
class Entity;
struct LevelGenRoomData;
struct AABB;
struct HudData;
struct Hud;

enum class ON
{
    LOGO = 0,
    INTRO = 1,
    PROLOGUE = 2,
    TITLE = 3,
    MENU = 4,
    OPTIONS = 5,
    PLAYER_PROFILE = 6,
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
    ARENA_STAGES = 22, // selecting available stages for arena from the menu
    ARENA_ITEMS = 23,
    ARENA_SELECT = 24, // select arena before fight
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
    RENDER_PRE_BLURRED_BACKGROUND,
    RENDER_POST_BLURRED_BACKGROUND,
    RENDER_PRE_DRAW_DEPTH,
    RENDER_POST_DRAW_DEPTH,
    RENDER_PRE_JOURNAL_PAGE,
    RENDER_POST_JOURNAL_PAGE,
    RENDER_PRE_LAYER,
    RENDER_POST_LAYER,
    RENDER_PRE_LEVEL,
    RENDER_POST_LEVEL,
    RENDER_PRE_GAME,
    RENDER_POST_GAME,
    SPEECH_BUBBLE,
    TOAST,
    PRE_LOAD_SCREEN,
    POST_LOAD_SCREEN,
    DEATH_MESSAGE,
    PRE_LOAD_JOURNAL_CHAPTER,
    POST_LOAD_JOURNAL_CHAPTER,
    PRE_GET_FEAT,
    PRE_SET_FEAT,
    PRE_UPDATE,
    POST_UPDATE,
    USER_DATA,
    PRE_LEVEL_CREATION,
    POST_LEVEL_CREATION,
    PRE_LAYER_CREATION,
    POST_LAYER_CREATION,
    PRE_LEVEL_DESTRUCTION,
    POST_LEVEL_DESTRUCTION,
    PRE_LAYER_DESTRUCTION,
    POST_LAYER_DESTRUCTION,
    PRE_PROCESS_INPUT,
    POST_PROCESS_INPUT,
    PRE_GAME_LOOP,
    POST_GAME_LOOP,
    PRE_SAVE_STATE,
    POST_SAVE_STATE,
    PRE_LOAD_STATE,
    POST_LOAD_STATE,
    BLOCKED_UPDATE,
    BLOCKED_GAME_LOOP,
    BLOCKED_PROCESS_INPUT,
    PRE_SET_RANDOM_BACKLAYER_ROOMS,
    POST_SET_RANDOM_BACKLAYER_ROOMS,
    PRE_SPAWN_BACKLAYER_ROOMS,
    POST_SPAWN_BACKLAYER_ROOMS,
    // PRE_COPY_STATE,
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
struct CustomOption
{
    sol::function func;
};
struct ScriptOption
{
    std::string desc;
    std::string long_desc;
    std::variant<IntOption, FloatOption, BoolOption, StringOption, ComboOption, ButtonOption, CustomOption> option_impl;
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

struct EntityInstagibCallback
{
    int id;
    int uid;
    sol::function func;
};

using TimerCallback = std::variant<IntervalCallback, TimeoutCallback>; // NoAlias

struct CurrentCallback
{
    int32_t aux_id;
    int32_t id;
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
    std::uint32_t dtor_hook_id;
};

struct SavedUserData
{
    sol::optional<sol::object> self;
    sol::optional<sol::object> held;
    sol::optional<sol::object> mount;
    std::unordered_map<uint32_t, sol::object> powerups;
};

struct HotKeyCallback
{
    sol::function func;
    KEY key;
    int lastRan;
    int queue;
    int hotkeyid;
};

struct StateMemory;
class SoundManager;
class LuaConsole;
struct RenderInfo;

struct LocalStateData
{
    sol::object user_data;
    ScriptState state = {0, 0, 0, 0, 0, 0, 0, 0};
};

class LuaBackend
    : public HookHandler<Entity, CallbackType::Entity>,
      public HookHandler<RenderInfo, CallbackType::Entity>,
      public HookHandler<ThemeInfo, CallbackType::Theme>
{
  public:
    using ProtectedBackend = GlobalMutexProtectedResource<LuaBackend*, &global_lua_lock>;
    using LockedBackend = ProtectedBackend::LockedResource<LuaBackend>;

    ProtectedBackend* self;

    sol::environment lua;
    std::shared_ptr<sol::state> vm;
    std::unordered_set<std::string> loaded_modules;

    std::string result;

    int cbcount = 0;
    CurrentCallback current_cb = {0, 0, CallbackType::None};

    std::map<std::string, ScriptOption> options;
    std::deque<ScriptMessage> messages;
    std::unordered_map<int, TimerCallback> level_timers;
    std::unordered_map<int, TimerCallback> global_timers;
    std::unordered_map<int, ScreenCallback> callbacks;
    std::unordered_map<int, ScreenCallback> load_callbacks;
    std::unordered_map<int, ScreenCallback> save_callbacks;
    std::unordered_map<int, HotKeyCallback> hotkey_callbacks;
    std::vector<std::uint32_t> vanilla_sound_callbacks;
    std::vector<LevelGenCallback> pre_tile_code_callbacks;
    std::vector<LevelGenCallback> post_tile_code_callbacks;
    std::vector<EntitySpawnCallback> pre_entity_spawn_callbacks;
    std::vector<EntitySpawnCallback> post_entity_spawn_callbacks;
    std::vector<EntityInstagibCallback> pre_entity_instagib_callbacks;
    std::vector<std::uint32_t> chance_callbacks;
    std::vector<std::uint32_t> extra_spawn_callbacks;
    std::vector<int> clear_callbacks;
    std::vector<std::pair<int, std::uint32_t>> screen_hooks;
    std::vector<std::pair<int, std::uint32_t>> clear_screen_hooks;
    std::vector<CustomMovableBehaviorStorage> custom_movable_behaviors;
    std::unordered_map<std::uint32_t, UserData> user_datas;
    std::unordered_map<int, SavedUserData> saved_user_datas;
    std::vector<std::string> required_scripts;
    std::unordered_map<int, ScriptInput*> script_input;
    std::unordered_set<std::string> windows;
    std::unordered_set<std::string> console_commands;
    std::unordered_map<StateMemory*, LocalStateData> local_state_datas;
    bool manual_save{false};
    uint32_t last_save{0};

    ImDrawList* draw_list{nullptr};

    StateMemory* g_state = nullptr;

    SoundManager* sound_manager;
    LuaConsole* console;

    std::map<IMAGE, ScriptImage*> images;

    size_t frame_counter{0};
    bool infinite_loop_detection{true};

    LuaBackend(SoundManager* sound_manager, LuaConsole* console);
    virtual ~LuaBackend();

    LocalStateData& get_locals();
    void copy_locals(StateMemory* from, StateMemory* to);
    void clear();
    void clear_all_callbacks();
    bool update();

    virtual bool reset()
    {
        clear();
        return true;
    }
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

    void draw(ImDrawList* dl);
    void render_options();

    bool is_callback_cleared(int32_t callback_id) const;
    bool is_screen_callback_cleared(std::pair<int32_t, uint32_t> callback_id) const;

    bool pre_tile_code(std::string_view tile_code, float x, float y, int layer, uint16_t room_template);
    void post_tile_code(std::string_view tile_code, float x, float y, int layer, uint16_t room_template);

    void pre_load_level_files();
    bool pre_load_screen();
    bool pre_init_level();
    bool pre_init_layer(LAYER layer);
    bool pre_unload_level();
    bool pre_unload_layer(LAYER layer);
    bool pre_save_state(int slot, StateMemory* saved);
    bool pre_load_state(int slot, StateMemory* loaded);

    void post_room_generation();
    void post_level_generation();
    void post_load_screen();
    void post_init_layer(LAYER layer);
    void post_unload_layer(LAYER layer);
    void post_save_state(int slot, StateMemory* saved);
    void post_load_state(int slot, StateMemory* loaded);

    void on_death_message(STRINGID stringid);
    std::optional<bool> pre_get_feat(FEAT feat);
    bool pre_set_feat(FEAT feat);

    std::string pre_get_random_room(int x, int y, uint8_t layer, uint16_t room_template);
    struct PreHandleRoomTilesResult
    {
        bool stop_callback;
        std::optional<LevelGenRoomData> modded_room_data;
    };
    PreHandleRoomTilesResult pre_handle_room_tiles(LevelGenRoomData room_data, int x, int y, uint16_t room_template);

    Entity* pre_entity_spawn(std::uint32_t entity_type, float x, float y, int layer, Entity* overlay, int spawn_type_flags);
    void post_entity_spawn(Entity* entity, int spawn_type_flags);

    bool pre_entity_instagib(Entity* victim);

    bool process_vanilla_render_callbacks(ON event);
    bool process_vanilla_render_blur_callbacks(ON event, float blur_amount);
    bool process_vanilla_render_hud_callbacks(ON event, Hud* hud);
    bool process_vanilla_render_layer_callbacks(ON event, uint8_t layer);
    bool process_vanilla_render_draw_depth_callbacks(ON event, uint8_t draw_depth, const AABB& bbox);
    bool process_vanilla_render_journal_page_callbacks(ON event, JournalPageType page_type, JournalPage* page);

    std::u16string pre_speach_bubble(Entity* entity, char16_t* buffer);
    std::u16string pre_toast(char16_t* buffer);

    bool pre_load_journal_chapter(uint8_t chapter);
    std::vector<uint32_t> post_load_journal_chapter(uint8_t chapter, const std::vector<uint32_t>& pages);

    CurrentCallback get_current_callback() const;
    void set_current_callback(int32_t aux_id, int32_t id, CallbackType type);
    void clear_current_callback();

    void set_error(std::string err);

    static void for_each_backend(std::function<bool(LockedBackend)> fun, bool stop_propagation = true);
    static LockedBackend get_backend(std::string_view id);
    static std::optional<LockedBackend> get_backend_safe(std::string_view id);
    static LockedBackend get_backend_by_id(std::string_view id, std::string_view ver = "");
    static std::optional<LockedBackend> get_backend_by_id_safe(std::string_view id, std::string_view ver = "");

    static LockedBackend get_calling_backend();
    static std::string get_calling_backend_id();
    static void push_calling_backend(LuaBackend*);
    static void pop_calling_backend(LuaBackend*);
    void on_set_user_data(Entity* ent);
    void load_user_data();
    void pre_copy_state(StateMemory* from, StateMemory* to);
    bool pre_spawn_backlayer_rooms(uint32_t start_x, uint32_t start_y, uint32_t limit_width, uint32_t limit_height);
    void post_spawn_backlayer_rooms(uint32_t start_x, uint32_t start_y, uint32_t limit_width, uint32_t limit_height);

    void hotkey_callback(int cb);
    int register_hotkey(HotKeyCallback cb, HOTKEY_TYPE flags);
    static void wm_activate(bool active);
    static void wm_hotkey(int keyid);
};

template <class Inheriting>
class LockableLuaBackend : public LuaBackend
{
  public:
    using LuaBackend::LuaBackend;

    using LockableInherited = ProtectedBackend::LockedResource<Inheriting>;
    LockableInherited Lock()
    {
        return self->LockAs<Inheriting>();
    }
};

struct HotKey
{
    int mod;
    int key;
    LuaBackend* backend;
    int cb;
    bool active;
    HOTKEY_TYPE flags;
    HOTKEY_TYPE suppressflags;
};
