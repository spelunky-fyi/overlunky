#include "script.hpp"
#include "entity.hpp"
#include "logger.h"
#include "rpc.hpp"
#include "state.hpp"
#include "sound_manager.hpp"
#include "savedata.hpp"
#include "window_api.hpp"

#include <regex>
#include <algorithm>
#include <array>
#include <codecvt>
#include <fstream>
#include <iomanip>
#include <locale>
#include <set>
#include <map>
#include <filesystem>

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

// helper type for visitor pattern
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

struct IntOption {
    int value;
    int min;
    int max;
};
struct FloatOption {
    float value;
    float min;
    float max;
};
struct BoolOption {
    bool value;
};
struct StringOption {
    std::string value;
};
struct ComboOption {
    int value;
    std::string options;
};
struct ButtonOption {
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
    int screen;
    int lastRan;
};

struct ScriptState
{
    Player* player;
    uint32_t screen;
    uint32_t time_level;
    uint32_t time_total;
    uint32_t frame;
    uint32_t loading;
    uint32_t reset;
    uint32_t quest_flags;
};

using Callback = std::variant<IntervalCallback, TimeoutCallback, ScreenCallback>;

using Toast = void (*)(void *, wchar_t *);
Toast get_toast()
{
    ONCE(Toast)
    {
        auto memory = Memory::get();
        auto off = find_inst(memory.exe(), "\x49\x8B\x0C\x3F\xB8\x60\x01\x00\x00"s, memory.after_bundle);
        off = function_start(memory.at_exe(off));
        return res = (Toast)off;
    }
}

using Say = void (*)(void *, Entity *, wchar_t *, int unk_type /* 0, 2, 3 */, bool top /* top or bottom */);
Say get_say()
{
    ONCE(Say)
    {
        auto memory = Memory::get();
        auto off = find_inst(memory.exe(), "\x4A\x8B\x0C\x2F\xB8\x60\x01\x00\x00"s, memory.after_bundle);
        off = function_start(memory.at_exe(off));
        return res = (Say)off;
    }
}

using Prng = void (*)(int64_t seed);
Prng get_seed_prng()
{
    ONCE(Prng)
    {
        auto memory = Memory::get();
        auto off = find_inst(memory.exe(), "\x48\x89\x5C\x24\x08\x48\x89\x74\x24\x10\x57\x48\x83\xEC\x10\x8B\xC1\x33\xFF\x48\x85\xC0\x41\xB9\x30\x01\x00\x00\x48\xBB\x99\x9A\x6A\x67\xD0\x63\x6C\x9E"s, memory.after_bundle);
        off = function_start(memory.at_exe(off));
        return res = (Prng)off;
    }
}

void infinite_loop(lua_State* argst, lua_Debug * argdb) {
    luaL_error(argst, "Hit Infinite Loop Detection of 1bln instructions");
};

Movable* get_entity(uint32_t id)
{
    return (Movable*)get_entity_ptr(id);
}

std::tuple<float, float, int> get_position(uint32_t id)
{
    Entity* ent = get_entity_ptr(id);
    if (ent)
        return std::make_tuple(ent->position().first, ent->position().second, ent->layer());
    return { 0.0f, 0.0f, 0 };
}

float screenify(float dis)
{
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 res = io.DisplaySize;
    return dis / (1.0 / (res.x / 2));
}

ImVec2 screenify(ImVec2 pos)
{
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 res = io.DisplaySize;
    ImVec2 bar = { 0.0, 0.0 };
    if (res.x / res.y > 1.78)
    {
        bar.x = (res.x - res.y / 9 * 16) / 2;
        res.x = res.y / 9 * 16;
    }
    else if (res.x / res.y < 1.77)
    {
        bar.y = (res.y - res.x / 16 * 9) / 2;
        res.y = res.x / 16 * 9;
    }
    ImVec2 screened = ImVec2(pos.x / (1.0 / (res.x / 2)) + res.x / 2 + bar.x, res.y - (res.y / 2 * pos.y) - res.y / 2 + bar.y);
    return screened;
}

ImVec2 normalize(ImVec2 pos)
{
    ImGuiIO &io = ImGui::GetIO();
    ImVec2 res = io.DisplaySize;
    if (res.x / res.y > 1.78)
    {
        pos.x -= (res.x - res.y / 9 * 16) / 2;
        res.x = res.y / 9 * 16;
    }
    else if (res.x / res.y < 1.77)
    {
        pos.y -= (res.y - res.x / 16 * 9) / 2;
        res.y = res.x / 16 * 9;
    }
    ImVec2 normal = ImVec2((pos.x - res.x / 2) * (1.0 / (res.x / 2)), -(pos.y - res.y / 2) * (1.0 / (res.y / 2)));
    return normal;
}

std::string sanitize(std::string data)
{
    std::transform(data.begin(), data.end(), data.begin(), [](unsigned char c) { return std::tolower(c); });
    std::regex reg("[^a-z/]*");
    data = std::regex_replace(data, reg, "");
    return data;
}

struct InputTextCallback_UserData
{
    std::string*            Str;
    ImGuiInputTextCallback  ChainCallback;
    void*                   ChainCallbackUserData;
};

static int InputTextCallback(ImGuiInputTextCallbackData* data)
{
    InputTextCallback_UserData* user_data = (InputTextCallback_UserData*)data->UserData;
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
    {
        std::string* str = user_data->Str;
        IM_ASSERT(data->Buf == str->c_str());
        str->resize(data->BufTextLen);
        data->Buf = (char*)str->c_str();
    }
    else if (user_data->ChainCallback)
    {
        data->UserData = user_data->ChainCallbackUserData;
        return user_data->ChainCallback(data);
    }
    return 0;
}

bool InputString(const char* label, std::string* str, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
    flags |= ImGuiInputTextFlags_CallbackResize;

    InputTextCallback_UserData cb_user_data;
    cb_user_data.Str = str;
    cb_user_data.ChainCallback = callback;
    cb_user_data.ChainCallbackUserData = user_data;
    return ImGui::InputText(label, (char*)str->c_str(), str->capacity() + 1, flags, InputTextCallback, &cb_user_data);
}

class SpelunkyScript::ScriptImpl
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
    ScriptState state = { nullptr, 0, 0, 0, 0, 0, 0, 0 };
    bool changed = true;
    bool enabled = true;
    ScriptMeta meta = { "", "", "", "", "", "", "", "", false };
    std::filesystem::path script_folder;
    int cbcount = 0;

    std::map<std::string, ScriptOption> options;
    std::deque<ScriptMessage> messages;
    std::map<int, Callback> level_timers;
    std::map<int, Callback> global_timers;
    std::map<int, Callback> callbacks;
    std::vector<int> clear_callbacks;
    std::vector<std::string> required_scripts;
    std::map<int, ScriptInput *> script_input;
    std::set<std::string> windows;

    ImDrawList* draw_list{ nullptr };

    StateMemory* g_state = nullptr;
    std::vector<EntityItem> g_items;
    std::vector<Player*> g_players;
    SaveData* g_save = nullptr;

    SoundManager* sound_manager;

    std::map<int, ScriptImage *> images;

    ScriptImpl(std::string script, std::string file, SoundManager* sound_manager, bool enable = true);
    ~ScriptImpl() = default;

    std::string script_id();
    bool handle_function(sol::function func);

    bool run();
    void draw(ImDrawList* dl);
    void render_options();
};

SpelunkyScript::ScriptImpl::ScriptImpl(std::string script, std::string file, SoundManager* sound_mgr, bool enable)
{
    sound_manager = sound_mgr;

#ifdef SPEL2_EDITABLE_SCRIPTS
    strcpy(code, script.c_str());
#else
    code_storage = std::move(script);
    code = code_storage.c_str();
#endif
    meta.file = std::move(file);
    meta.path = std::filesystem::path(meta.file).parent_path().string();
    meta.filename = std::filesystem::path(meta.file).filename().string();

    script_folder = std::filesystem::path(meta.file).parent_path();

    enabled = enable;

    g_state = get_state_ptr();
    g_items = list_entities();
    g_players = get_players();
    g_save = savedata();

    if (!g_players.empty())
        state.player = g_players.at(0);
    else
        state.player = nullptr;
    state.screen = g_state->screen;
    state.time_level = g_state->time_level;
    state.time_total = g_state->time_total;
    state.frame = get_frame_count();
    state.loading = g_state->loading;
    state.reset = (g_state->quest_flags & 1);
    state.quest_flags = g_state->quest_flags;

    lua.open_libraries(sol::lib::math, sol::lib::base, sol::lib::string, sol::lib::table, sol::lib::coroutine, sol::lib::package);

    /// Table of strings where you should set some script metadata shown in the UI.
    /// - `meta.name` Script name
    /// - `meta.version` Version
    /// - `meta.description` Short description of the script
    /// - `meta.author` Your name
    lua["meta"] = lua.create_named_table("meta");

    try
    {
        std::string metacode = "";
        std::stringstream metass(code);
        std::regex reg("(^\\s*meta\\.[a-z]+\\s*=)");
        std::regex regstart("(^\\s*meta\\s*=)");
        std::regex regend("(\\})");
        std::regex multistart("\\[\\[|\\.\\.\\s*($|--)|\\bmeta\\.[a-z]+\\s*=\\s*($|--)");
        std::regex multiend("\\]\\]\\s*($|--)|[\"']\\s*($|--)");
        bool getmeta = false;
        bool getmulti = false;
        for (std::string line; std::getline(metass, line);)
        {
            if (std::regex_search(line, regstart))
            {
                getmeta = true;
            }
            if (std::regex_search(line, reg) && std::regex_search(line, multistart))
            {
                getmulti = true;
            }
            if (std::regex_search(line, reg) || getmeta || getmulti)
            {
                metacode += line + "\n";
            }
            if (std::regex_search(line, multiend))
            {
                getmulti = false;
            }
            if (std::regex_search(line, regend))
            {
                getmeta = false;
            }
        }
        auto lua_result = lua.safe_script(metacode.data());
        sol::optional<std::string> meta_name = lua["meta"]["name"];
        sol::optional<std::string> meta_version = lua["meta"]["version"];
        sol::optional<std::string> meta_description = lua["meta"]["description"];
        sol::optional<std::string> meta_author = lua["meta"]["author"];
        sol::optional<bool> meta_unsafe = lua["meta"]["unsafe"];
        meta.name = meta_name.value_or(meta.filename);
        meta.version = meta_version.value_or("");
        meta.description = meta_description.value_or("");
        meta.author = meta_author.value_or("Anonymous");
        meta.unsafe = meta_unsafe.value_or(false);
        meta.id = script_id();
        result = "Got metadata";
    }
    catch (const sol::error& e)
    {
        result = e.what();
        messages.push_back({ result, std::chrono::system_clock::now(), ImVec4(1.0f, 0.2f, 0.2f, 1.0f) });
        DEBUG("{}", result);
        if (messages.size() > 20)
            messages.pop_front();
    }

    /// A bunch of [game state](#statememory) variables
    /// Example:
    /// ```lua
    /// if state.time_level > 300 and state.theme == THEME.DWELLING then
    ///     toast("Congratulations for lasting 5 seconds in Dwelling")
    /// end
    /// ```
    lua["state"] = g_state;
    /// An array of [Player](#player) of the current players. Pro tip: You need `players[1].uid` in most entity functions.
    lua["players"] = std::vector<Movable*>(g_players.begin(), g_players.end());
    /// Provides a read-only access to the save data, updated as soon as something changes (i.e. before it's written to savegame.sav.)
    lua["savegame"] = g_save;

    /// Print a log message on screen.
    lua["message"] = [this](std::string message) {
        messages.push_back({ message, std::chrono::system_clock::now(), ImVec4(1.0f, 1.0f, 1.0f, 1.0f) });
        if (messages.size() > 20)
            messages.pop_front();
    };
    /// Returns: `int` unique id for the callback to be used in [clear_callback](#clear_callback).
    /// Add per level callback function to be called every `frames` engine frames. Timer is paused on pause and cleared on level transition.
    lua["set_interval"] = [this](sol::function cb, int frames) {
        auto luaCb = IntervalCallback{ cb, frames, -1 };
        level_timers[cbcount] = luaCb;
        return cbcount++;
    };
    /// Returns: `int` unique id for the callback to be used in [clear_callback](#clear_callback).
    /// Add per level callback function to be called after `frames` engine frames. Timer is paused on pause and cleared on level transition.
    lua["set_timeout"] = [this](sol::function cb, int frames) {
        int now = g_state->time_level;
        auto luaCb = TimeoutCallback{ cb, now + frames };
        level_timers[cbcount] = luaCb;
        return cbcount++;
    };
    /// Returns: `int` unique id for the callback to be used in [clear_callback](#clear_callback).
    /// Add global callback function to be called every `frames` engine frames. This timer is never paused or cleared.
    lua["set_global_interval"] = [this](sol::function cb, int frames) {
        auto luaCb = IntervalCallback{ cb, frames, -1 };
        global_timers[cbcount] = luaCb;
        return cbcount++;
    };
    /// Returns: `int` unique id for the callback to be used in [clear_callback](#clear_callback).
    /// Add global callback function to be called after `frames` engine frames. This timer is never paused or cleared.
    lua["set_global_timeout"] = [this](sol::function cb, int frames) {
        int now = get_frame_count();
        auto luaCb = TimeoutCallback{ cb, now + frames };
        global_timers[cbcount] = luaCb;
        return cbcount++;
    };
    /// Returns: `int` unique id for the callback to be used in [clear_callback](#clear_callback).
    /// Add global callback function to be called on an [event](#on).
    lua["set_callback"] = [this](sol::function cb, int screen) {
        auto luaCb = ScreenCallback{ cb, screen, -1 };
        callbacks[cbcount] = luaCb;
        return cbcount++;
    };
    /// Clear previously added callback `id`
    lua["clear_callback"] = [this](int id) { clear_callbacks.push_back(id); };
    /// Table of options set in the UI, added with the [register_option_functions](#register_option_int).
    lua["options"] = lua.create_named_table("options");
    /// Load another script by id "author/name"
    lua["load_script"] = [this](std::string id) { required_scripts.push_back(sanitize(id)); };
    /// Seed the game prng.
    lua["seed_prng"] = [this](int64_t seed) {
        auto seed_prng = get_seed_prng();
        seed_prng(seed);
    };
    /// Returns: `int[20]`
    /// Read the game prng state. Maybe you can use these and math.randomseed() to make deterministic things, like online scripts :shrug:. Example:
    /// ```lua
    /// -- this should always print the same table D877...E555
    /// set_callback(function()
    ///   seed_prng(42069)
    ///   local prng = read_prng()
    ///   for i,v in ipairs(prng) do
    ///     message(string.format("%08X", v))
    ///   end
    /// end, ON.LEVEL)
    /// ```
    lua["read_prng"] = [this]() { return read_prng(); };
    /// Show a message that looks like a level feeling.
    lua["toast"] = [this](std::wstring message) {
        auto toast = get_toast();
        toast(NULL, message.data());
    };
    /// Show a message coming from an entity
    lua["say"] = [this](uint32_t entity_id, std::wstring message, int unk_type, bool top) {
        auto say = get_say();
        auto entity = get_entity_ptr(entity_id);
        if (entity == nullptr)
            return;
        say(NULL, entity, message.data(), unk_type, top);
    };
    /// Add an integer option that the user can change in the UI. Read with `options.name`, `value` is the default. Keep in mind these are just soft limits, you can override them in the UI with double click.
    // lua["register_option_int"] = [this](std::string name, std::string desc, std::string long_desc, int value, int min, int max)
    lua["register_option_int"] = sol::overload(
        [this](std::string name, std::string desc, std::string long_desc, int value, int min, int max) {
            options[name] = { desc, long_desc, IntOption{ value, min, max } };
            lua["options"][name] = value;
        },
        [this](std::string name, std::string desc, int value, int min, int max) {
            options[name] = { desc, "", IntOption{ value, min, max } };
            lua["options"][name] = value;
        }
    );
    /// Add a float option that the user can change in the UI. Read with `options.name`, `value` is the default. Keep in mind these are just soft limits, you can override them in the UI with double click.
    // lua["register_option_float"] = [this](std::string name, std::string desc, std::string long_desc, float value, float min, float max)
    lua["register_option_float"] = sol::overload(
        [this](std::string name, std::string desc, std::string long_desc, float value, float min, float max) {
            options[name] = { desc, long_desc, FloatOption{ value, min, max } };
            lua["options"][name] = value;
        },
        [this](std::string name, std::string desc, float value, float min, float max) {
            options[name] = { desc, "", FloatOption{ value, min, max } };
            lua["options"][name] = value;
        }
    );
    /// Add a boolean option that the user can change in the UI. Read with `options.name`, `value` is the default.
    // lua["register_option_bool"] = [this](std::string name, std::string desc, std::string long_desc, bool value)
    lua["register_option_bool"] = sol::overload(
        [this](std::string name, std::string desc, std::string long_desc, bool value) {
            options[name] = { desc, long_desc, BoolOption{ value } };
            lua["options"][name] = value;
        },
        [this](std::string name, std::string desc, bool value) {
            options[name] = { desc, "", BoolOption{ value } };
            lua["options"][name] = value;
        }
    );
    /// Add a string option that the user can change in the UI. Read with `options.name`, `value` is the default.
    // lua["register_option_string"] = [this](std::string name, std::string desc, std::string long_desc, std::string value)
    lua["register_option_string"] = sol::overload(
        [this](std::string name, std::string desc, std::string long_desc, std::string value) {
            options[name] = { desc, long_desc, StringOption{ value } };
            lua["options"][name] = value;
        },
        [this](std::string name, std::string desc, std::string value) {
            options[name] = { desc, "", StringOption{ value } };
            lua["options"][name] = value;
        }
    );
    /// Add a combobox option that the user can change in the UI. Read the int index of the selection with `options.name`. Separate `opts` with `\0`, with a double `\0\0` at the end.
    // lua["register_option_combo"] = [this](std::string name, std::string desc, std::string long_desc, std::string opts)
    lua["register_option_combo"] = sol::overload(
        [this](std::string name, std::string desc, std::string long_desc, std::string opts) {
            options[name] = { desc, long_desc, ComboOption{ 0, opts } };
            lua["options"][name] = 1;
        },
        [this](std::string name, std::string desc, std::string opts) {
            options[name] = { desc, "", ComboOption{ 0, opts } };
            lua["options"][name] = 1;
        }
    );
    /// Add a button that the user can click in the UI. Sets the timestamp of last click on value and runs the callback function.
    // lua["register_option_combo"] = [this](std::string name, std::string desc, std::string long_desc, sol::function on_click)
    lua["register_option_button"] = sol::overload(
        [this](std::string name, std::string desc, std::string long_desc, sol::function callback) {
            options[name] = { desc, long_desc, ButtonOption{ callback } };
            lua["options"][name] = -1;
        },
        [this](std::string name, std::string desc, sol::function callback) {
            options[name] = { desc, "", ButtonOption{ callback } };
            lua["options"][name] = -1;
        }
    );
    /// Spawn an entity in position with some velocity and return the uid of spawned entity.
    /// Uses level coordinates with [LAYER.FRONT](#layer) and LAYER.BACK, but player-relative coordinates with LAYER.PLAYERn.
    /// Example:
    /// ```lua
    /// -- spawn megajelly using absolute coordinates
    /// set_callback(function()
    ///     x, y, layer = get_position(players[1].uid)
    ///     spawn_entity(ENT_TYPE.MONS_MEGAJELLYFISH, x, y+3, layer, 0, 0)
    /// end, ON.LEVEL)
    /// -- spawn clover using player-relative coordinates
    /// set_callback(function()
    ///     spawn(ENT_TYPE.ITEM_PICKUP_CLOVER, 0, 1, LAYER.PLAYER1, 0, 0)
    /// end, ON.LEVEL)
    /// ```
    lua["spawn_entity"] = spawn_entity_abs;
    /// Short for [spawn_entity](#spawn_entity).
    lua["spawn"] = spawn_entity_abs;
    /// Spawn a door to another world, level and theme and return the uid of spawned entity.
    /// Uses level coordinates with LAYER.FRONT and LAYER.BACK, but player-relative coordinates with LAYER.PLAYERn
    lua["spawn_door"] = spawn_door_abs;
    /// Short for [spawn_door](#spawn_door).
    lua["door"] = spawn_door_abs;
    /// Spawn a door to backlayer.
    lua["spawn_layer_door"] = spawn_backdoor_abs;
    /// Short for [spawn_layer_door](#spawn_layer_door).
    lua["layer_door"] = spawn_backdoor_abs;
    /// Warp to a level immediately.
    lua["warp"] = warp;
    /// Set seed and reset run.
    lua["set_seed"] = set_seed;
    /// Enable/disable godmode.
    lua["god"] = godmode;
    /// Try to force next levels to be dark.
    lua["force_dark_level"] = darkmode;
    /// Set the zoom level used in levels and shops. 13.5 is the default.
    lua["zoom"] = zoom;
    /// Enable/disable game engine pause.
    lua["pause"] = [this](bool p) {
        if (p)
            set_pause(0x20);
        else
            set_pause(0);
    };
    /// Teleport entity to coordinates with optional velocity
    lua["move_entity"] = move_entity_abs;
    /// Make an ENT_TYPE.FLOOR_DOOR_EXIT go to world `w`, level `l`, theme `t`
    lua["set_door_target"] = set_door_target;
    /// Short for [set_door_target](#set_door_target).
    lua["set_door"] = set_door_target;
    /// Get door target `world`, `level`, `theme`
    lua["get_door_target"] = get_door_target;
    /// Set the contents of ENT_TYPE.ITEM_POT, ENT_TYPE.ITEM_CRATE or ENT_TYPE.ITEM_COFFIN `id` to ENT_TYPE... `item`
    lua["set_contents"] = set_contents;
    /// Get the [Entity](#entity) behind an uid
    lua["get_entity"] = get_entity_ptr;
    /// Get the [EntityDB](#entitydb) behind an ENT_TYPE...
    lua["get_type"] = get_type;
    /// Get uids of all entities currently loaded
    lua["get_entities"] = get_entities;
    /// Get uids of entities by some conditions. Set `type` or `mask` to `0` to ignore that.
    lua["get_entities_by"] = get_entities_by;
    /// Returns: `array<int>`
    /// Get uids of entities matching id. This function is variadic, meaning it accepts any number of id's.
    /// You can even pass a table! Example:
    /// ```lua
    /// types = {ENT_TYPE.MONS_SNAKE, ENT_TYPE.MONS_BAT}
    /// function on_level()
    ///     uids = get_entities_by_type(ENT_TYPE.MONS_SNAKE, ENT_TYPE.MONS_BAT)
    ///     -- is not the same thing as this, but also works
    ///     uids2 = get_entities_by_type(types)
    ///     message(tostring(#uids).." == "..tostring(#uids2))
    /// end
    /// ```
    lua["get_entities_by_type"] = [](sol::variadic_args va) {
        sol::type type = va.get_type();
        if (type == sol::type::number)
        {
            auto args = std::vector<uint32_t>(va.begin(), va.end());
            auto get_func = sol::resolve<std::vector<uint32_t>(std::vector<uint32_t>)>(get_entities_by_type);
            return get_func(args);
        }
        else if (type == sol::type::table)
        {
            auto args = va.get<std::vector<uint32_t>>(0);
            auto get_func = sol::resolve<std::vector<uint32_t>(std::vector<uint32_t>)>(get_entities_by_type);
            return get_func(args);
        }
        return std::vector<uint32_t>({});
    };
    /// Get uids of entities by some search_flags
    lua["get_entities_by_mask"] = get_entities_by_mask;
    /// Get uids of entities by layer. `0` for main level, `1` for backlayer, `-1` for layer of the player.
    lua["get_entities_by_layer"] = get_entities_by_layer;
    /// Get uids of matching entities inside some radius. Set `type` or `mask` to `0` to ignore that.
    lua["get_entities_at"] = get_entities_at;
    /// Get the `flags` field from entity by uid
    lua["get_entity_flags"] = get_entity_flags;
    /// Set the `flags` field from entity by uid
    lua["set_entity_flags"] = set_entity_flags;
    /// Get the `more_flags` field from entity by uid
    lua["get_entity_flags2"] = get_entity_flags2;
    /// Set the `more_flags` field from entity by uid
    lua["set_entity_flags2"] = set_entity_flags2;
    /// Get the `move_state` field from entity by uid
    lua["get_entity_ai_state"] = get_entity_ai_state;
    /// Get `state.level_flags`
    lua["get_level_flags"] = get_hud_flags;
    /// Set `state.level_flags`
    lua["set_level_flags"] = set_hud_flags;
    /// Get the ENT_TYPE... for entity by uid
    lua["get_entity_type"] = get_entity_type;
    /// Get the current set zoom level
    lua["get_zoom_level"] = get_zoom_level;
    /// Translate an entity position to screen position to be used in drawing functions
    lua["screen_position"] = screen_position;
    /// Translate a distance of `x` tiles to screen distance to be be used in drawing functions
    lua["screen_distance"] = screen_distance;
    /// Get position `x, y, layer` of entity by uid. Use this, don't use `Entity.x/y` because those are sometimes just the offset to the entity
    /// you're standing on, not real level coordinates.
    lua["get_position"] = get_position;
    /// Remove item by uid from entity
    lua["entity_remove_item"] = entity_remove_item;
    /// Spawn an entity by `id` attached to some other entity `over`, in offset `x`, `y`
    lua["spawn_entity_over"] = spawn_entity_over;
    /// Check if the entity `id` has some specific `item` by uid in their inventory
    lua["entity_has_item_uid"] = entity_has_item_uid;
    /// Check if the entity `id` has some ENT_TYPE `type` in their inventory
    lua["entity_has_item_type"] = entity_has_item_type;
    /// Kills an entity by uid.
    lua["kill_entity"] = kill_entity;
    /// Pick up another entity by uid. Make sure you're not already holding something, or weird stuff will happen. Example:
    /// ```lua
    /// -- spawn and equip a jetpack
    /// pick_up(players[1].uid, spawn(ENT_TYPE.ITEM_JETPACK, 0, 0, LAYER.PLAYER, 0, 0))
    /// ```
    lua["pick_up"] = pick_up;
    /// Apply changes made in [get_type](#get_type)() to entity instance by uid.
    lua["apply_entity_db"] = apply_entity_db;
    /// Try to lock the exit at coordinates
    lua["lock_door_at"] = lock_door_at;
    /// Try to unlock the exit at coordinates
    lua["unlock_door_at"] = unlock_door_at;
    /// Get the current global frame count since the game was started. You can use this to make some timers yourself, the engine runs at 60fps.
    lua["get_frame"] = get_frame_count;
    /// Get the current timestamp in milliseconds since the Unix Epoch.
    lua["get_ms"] = []() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    };
    /// Make `mount` carry `rider` on their back. Only use this with actual mounts and living things.
    lua["carry"] = carry;
    /// Sets the arrow type (wooden, metal, light) that is shot from a regular arrow trap and a poison arrow trap.
    lua["set_arrowtrap_projectile"] = set_arrowtrap_projectile;
    /// Sets the amount of blood drops in the Kapala needed to trigger a health increase (default = 7).
    lua["set_kapala_blood_threshold"] = set_kapala_blood_threshold;
    /// Sets the hud icon for the Kapala (0-6 ; -1 for default behaviour). 
    /// If you set a Kapala treshold greater than 7, make sure to set the hud icon in the range 0-6, or other icons will appear in the hud!
    lua["set_kapala_hud_icon"] = set_kapala_hud_icon;
    /// Changes characteristics of (all) sparktraps: speed, rotation direction and distance from center
    /// Speed: expressed as the amount that should be added to the angle every frame (use a negative number to go in the other direction)
    /// Distance from center: if you go above 3.0 the game might crash because a spark may go out of bounds!
    lua["modify_sparktraps"] = modify_sparktraps;
    /// Sets the multiplication factor for blood droplets (default/no Vlad's cape = 1, with Vlad's cape = 2)
    lua["set_blood_multiplication"] = set_blood_multiplication;
    /// Flip entity around by uid. All new entities face right by default.
    lua["flip_entity"] = flip_entity;
    /// Sets the Y-level at which Olmec changes phases
    lua["set_olmec_phase_y_level"] = set_olmec_phase_y_level;
    /// Determines when the ghost appears, either when the player is cursed or not
    lua["set_ghost_spawn_times"] = set_ghost_spawn_times;

    /// Calculate the tile distance of two entities by uid
    lua["distance"] = [this](uint32_t a, uint32_t b) {
        Entity* ea = get_entity_ptr(a);
        Entity* eb = get_entity_ptr(b);
        if (ea == nullptr || eb == nullptr)
            return -1.0f;
        else
            return (float)sqrt(pow(ea->position().first - eb->position().first, 2) + pow(ea->position().second - eb->position().second, 2));
    };
    /// Returns: `float`, `float`, `float`, `float`
    /// Basically gets the absolute coordinates of the area inside the unbreakable bedrock walls, from wall to wall. Every solid entity should be
    /// inside these boundaries. The order is: top left x, top left y, bottom right x, bottom right y Example:
    /// ```lua
    /// -- Draw the level boundaries
    /// set_callback(function()
    ///     xmin, ymin, xmax, ymax = get_bounds()
    ///     sx, sy = screen_position(xmin, ymin) -- top left
    ///     sx2, sy2 = screen_position(xmax, ymax) -- bottom right
    ///     draw_rect(sx, sy, sx2, sy2, 4, 0, rgba(255, 255, 255, 255))
    /// end, ON.GUIFRAME)
    /// ```
    lua["get_bounds"] = [this]() { return std::make_tuple(2.5f, 122.5f, g_state->w * 10.0f + 2.5f, 122.5f - g_state->h * 8.0f); };
    /// Gets the current camera position in the level
    lua["get_camera_position"] = get_camera_position;
    /// Sets the current camera position in the level.
    /// Note: The camera will still try to follow the player and this doesn't actually work at all.
    lua["set_camera_position"] = set_camera_position;

    /// Set a bit in a number. This doesn't actually change the bit in the entity you pass it, it just returns the new value you can use.
    lua["set_flag"] = [](uint32_t flags, int bit) { return flags | (1U << (bit - 1)); };
    lua["setflag"] = lua["set_flag"];
    /// Clears a bit in a number. This doesn't actually change the bit in the entity you pass it, it just returns the new value you can use.
    lua["clr_flag"] = [](uint32_t flags, int bit) { return flags & ~(1U << (bit - 1)); };
    lua["clrflag"] = lua["clr_flag"];
    /// Returns true if a bit is set in the flags
    lua["test_flag"] = [](uint32_t flags, int bit) { return (flags & (1U << (bit - 1))) > 0; };
    lua["testflag"] = lua["test_flag"];

    /// Converts a color to int to be used in drawing functions. Use values from `0..255`.
    lua["rgba"] = [](int r, int g, int b, int a) { return (unsigned int)(a << 24) + (b << 16) + (g << 8) + (r); };
    /// Draws a line on screen
    lua["draw_line"] = [this](float x1, float y1, float x2, float y2, float thickness, ImU32 color) {
        ImVec2 a = screenify({ x1, y1 });
        ImVec2 b = screenify({ x2, y2 });
        draw_list->AddLine(a, b, color, thickness);
    };
    /// Draws a rectangle on screen from top-left to bottom-right.
    lua["draw_rect"] = [this](float x1, float y1, float x2, float y2, float thickness, float rounding, ImU32 color) {
        ImVec2 a = screenify({ x1, y1 });
        ImVec2 b = screenify({ x2, y2 });
        draw_list->AddRect(a, b, color, rounding, ImDrawCornerFlags_All, thickness);
    };
    /// Draws a filled rectangle on screen from top-left to bottom-right.
    lua["draw_rect_filled"] = [this](float x1, float y1, float x2, float y2, float rounding, ImU32 color) {
        ImVec2 a = screenify({ x1, y1 });
        ImVec2 b = screenify({ x2, y2 });
        draw_list->AddRectFilled(a, b, color, rounding, ImDrawCornerFlags_All);
    };
    /// Draws a circle on screen
    lua["draw_circle"] = [this](float x, float y, float radius, float thickness, ImU32 color) {
        ImVec2 a = screenify({ x, y });
        float r = screenify(radius);
        draw_list->AddCircle(a, r, color, 0, thickness);
    };
    /// Draws a filled circle on screen
    lua["draw_circle_filled"] = [this](float x, float y, float radius, ImU32 color) {
        ImVec2 a = screenify({ x, y });
        float r = screenify(radius);
        draw_list->AddCircleFilled(a, r, color, 0);
    };
    /// Draws text in screen coordinates `x`, `y`, anchored top-left. Text size 0 uses the default 18.
    lua["draw_text"] = [this](float x, float y, float size, std::string text, ImU32 color) {
        ImVec2 a = screenify({ x, y });
        ImGuiIO& io = ImGui::GetIO();
        ImFont* font = io.Fonts->Fonts.back();
        for (auto pickfont : io.Fonts->Fonts)
        {
            if (floor(size) <= floor(pickfont->FontSize))
            {
                font = pickfont;
                break;
            }
        }
        draw_list->AddText(font, size, a, color, text.c_str());
    };
    /// Returns: `w`, `h` in screen distance.
    /// Calculate the bounding box of text, so you can center it etc.
    /// Example:
    /// ```lua
    /// function on_guiframe()
    ///     -- get a random color
    ///     color = math.random(0, 0xffffffff)
    ///     -- zoom the font size based on frame
    ///     size = (get_frame() % 199)+1
    ///     text = 'Awesome!'
    ///     -- calculate size of text
    ///     w, h = draw_text_size(size, text)
    ///     -- draw to the center of screen
    ///     draw_text(0-w/2, 0-h/2, size, text, color)
    /// end
    /// ```
    lua["draw_text_size"] = [this](float size, std::string text) {
        ImGuiIO &io = ImGui::GetIO();
        ImFont *font = io.Fonts->Fonts.back();
        for (auto pickfont : io.Fonts->Fonts)
        {
            if (floor(size) <= floor(pickfont->FontSize))
            {
                font = pickfont;
                break;
            }
        }
        ImVec2 textsize = font->CalcTextSizeA(size, 9999.0, 9999.0, text.c_str());
        auto a = normalize(ImVec2(0, 0));
        auto b = normalize(textsize);
        return std::make_pair(b.x - a.x, b.y - a.y);
    };
    /// Returns: `id, width, height`
    /// Create image from file.
    lua["create_image"] = [this](std::string path) -> std::tuple<int, int, int> {
        ScriptImage *image = new ScriptImage;
        image->width = 0;
        image->height = 0;
        image->texture = NULL;
        if (LoadTextureFromFile((script_folder / path).string().data(), &image->texture, &image->width, &image->height))
        {
            int id = images.size();
            images[id] = image;
            return std::make_tuple(id, image->width, image->height);
        }
        return std::make_tuple(-1, -1, -1);
    };
    /// Draws an image on screen from top-left to bottom-right. Use UV coordinates `0, 0, 1, 1` to just draw the whole image.
    lua["draw_image"] = [this](int image, float x1, float y1, float x2, float y2, float uvx1, float uvy1, float uvx2, float uvy2, ImU32 color) {
        if (images.find(image) == images.end())
            return;
        ImVec2 a = screenify({ x1, y1 });
        ImVec2 b = screenify({ x2, y2 });
        ImVec2 uva = ImVec2(uvx1, uvy1);
        ImVec2 uvb = ImVec2(uvx2, uvy2);
        draw_list->AddImage(images[image]->texture, a, b, uva, uvb, color);
    };

    /// Loads a sound from disk relative to this script, ownership might be shared with other code that loads the same file. Returns nil if file can't be found
    lua["create_sound"] = [this](std::string path) -> sol::optional<CustomSound> {
        CustomSound sound = sound_manager->get_sound((script_folder / path).string());
        if (sound)
        {
            return sound;
        }
        return sol::nullopt;
    };

    /// Steal input from a Player or HH.
    lua["steal_input"] = [this](int uid) {
        if (script_input.find(uid) != script_input.end())
            return;
        Player *player = get_entity_ptr(uid)->as<Player>();
        if (player == nullptr)
            return;
        ScriptInput *newinput = new ScriptInput();
        newinput->next = 0;
        newinput->current = 0;
        newinput->orig_input = player->input_ptr;
        newinput->orig_ai = player->ai_func;
        player->input_ptr = reinterpret_cast<size_t>(newinput);
        player->ai_func = 0;
        script_input[uid] = newinput;
        //DEBUG("Steal input: {:x} -> {:x}", newinput->orig_input, player->input_ptr);
    };
    /// Return input
    lua["return_input"] = [this](int uid) {
        if (script_input.find(uid) == script_input.end())
            return;
        Player *player = get_entity_ptr(uid)->as<Player>();
        if (player == nullptr)
            return;
        //DEBUG("Return input: {:x} -> {:x}", player->input_ptr, script_input[uid]->orig_input);
        player->input_ptr = script_input[uid]->orig_input;
        player->ai_func = script_input[uid]->orig_ai;
        script_input.erase(uid);
    };
    /// Send input
    lua["send_input"] = [this](int uid, uint16_t buttons) {
        if (script_input.find(uid) != script_input.end())
        {
            script_input[uid]->current = buttons;
            script_input[uid]->next = buttons;
        }
    };
    /// Read input
    lua["read_input"] = [this](int uid) {
        Player *player = get_entity_ptr(uid)->as<Player>();
        if (player == nullptr)
            return (uint16_t)0;
        ScriptInput *readinput = reinterpret_cast<ScriptInput *>(player->input_ptr);
        if (!IsBadReadPtr(readinput, 20))
        {
            return readinput->next;
        }
        return (uint16_t)0;
    };

    /// Returns: `bool` (false if the window was closed from the X)
    /// Create a new widget window. Put all win_ widgets inside the callback function. The window functions are just wrappers for the [ImGui](https://github.com/ocornut/imgui/) widgets, so read more about them there. Use screen position and distance, or `0, 0, 0, 0` to autosize in center. Use just a `##Label` as title to hide titlebar.
    /// **Important: Keep all your labels unique!** If you need inputs with the same label, add `##SomeUniqueLabel` after the text, or use pushid to give things unique ids. ImGui doesn't know what you clicked if all your buttons have the same text...
    /// The window api is probably evolving still, this is just the first draft. Felt cute, might delete later!
    lua["window"] = [this](std::string title, float x, float y, float w, float h, bool movable, sol::function callback) {
        bool win_open = true;
        ImGui::PushID("scriptwindow");
        ImGuiCond cond = (movable ? ImGuiCond_Appearing : ImGuiCond_Always);
        ImGuiCond flag = (movable ? 0 : ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        if (title == "" || title.find("##") == 0)
        {
            flag |= ImGuiWindowFlags_NoTitleBar;
        }
        if (x == 0.0f && y == 0.0f && w == 0.0f && h == 0.0f)
        {
            ImGui::SetNextWindowSize(ImVec2(400, -1), cond);
        }
        else
        {
            ImVec2 spos = screenify(ImVec2(x, y));
            ImVec2 ssa = screenify(ImVec2(w, h));
            ImVec2 ssb = screenify(ImVec2(0, 0));
            ImVec2 ssize = ImVec2(ssa.x - ssb.x, ssb.y - ssa.y);
            ImGui::SetNextWindowPos(spos, cond);
            ImGui::SetNextWindowSize(ssize, cond);
        }
        ImGui::Begin(title.c_str(), &win_open, flag);
        ImGui::PushItemWidth(-ImGui::GetWindowWidth() / 2);
        handle_function(callback);
        ImGui::PopItemWidth();
        if (x == 0.0f && y == 0.0f && w == 0.0f && h == 0.0f)
        {
            ImGui::SetWindowPos({ImGui::GetIO().DisplaySize.x / 2 - ImGui::GetWindowWidth() / 2,
                ImGui::GetIO().DisplaySize.y / 2 - ImGui::GetWindowHeight() / 2}, cond);
        }
        ImGui::End();
        ImGui::PopID();

        if (win_open && windows.count(title) == 0)
        {
            windows.insert(std::move(title));
            show_cursor();
        }
        else if (!win_open && windows.count(title) != 0)
        {
            windows.erase(title);
            hide_cursor();
        }

        return win_open;
    };
    /// Add some text to window, automatically wrapped
    lua["win_text"] = [](std::string text) { ImGui::TextWrapped(text.c_str()); };
    /// Add a separator line to window
    lua["win_separator"] = []() { ImGui::Separator(); };
    /// Add next thing on the same line. This is same as `win_sameline(0, -1)`
    lua["win_inline"] = []() { ImGui::SameLine(); };
    /// Add next thing on the same line, with an offset
    lua["win_sameline"] = [](float offset, float spacing) { ImGui::SameLine(offset, spacing); };
    /// Returns: `boolean`
    /// Add a button
    lua["win_button"] = [](std::string text) {
        if (ImGui::Button(text.c_str())) {
            return true;
        }
        return false;
    };
    /// Returns: `string`
    /// Add a text field
    lua["win_input_text"] = [](std::string label, std::string value) {
        InputString(label.c_str(), &value, 0, nullptr, nullptr);
        return value;
    };
    /// Returns: `int`
    /// Add an integer field
    lua["win_input_int"] = [](std::string label, int value) {
        ImGui::InputInt(label.c_str(), &value);
        return value;
    };
    /// Returns: `float`
    /// Add a float field
    lua["win_input_float"] = [](std::string label, float value) {
        ImGui::InputFloat(label.c_str(), &value);
        return value;
    };
    /// Returns: `int`
    /// Add an integer slider
    lua["win_slider_int"] = [](std::string label, int value, int min, int max) {
        ImGui::SliderInt(label.c_str(), &value, min, max);
        return value;
    };
    /// Returns: `int`
    /// Add an integer dragfield
    lua["win_drag_int"] = [](std::string label, int value, int min, int max) {
        ImGui::DragInt(label.c_str(), &value, 0.5f, min, max);
        return value;
    };
    /// Returns: `float`
    /// Add an float slider
    lua["win_slider_float"] = [](std::string label, float value, float min, float max) {
        ImGui::SliderFloat(label.c_str(), &value, min, max);
        return value;
    };
    /// Returns: `float`
    /// Add an float dragfield
    lua["win_drag_float"] = [](std::string label, float value, float min, float max) {
        ImGui::DragFloat(label.c_str(), &value, 0.5f, min, max);
        return value;
    };
    /// Returns: `boolean`
    lua["win_check"] = [](std::string label, bool value) {
        ImGui::Checkbox(label.c_str(), &value);
        return value;
    };
    /// Returns: `int`
    lua["win_combo"] = [](std::string label, int selected, std::string opts) {
        int reals = selected - 1;
        ImGui::Combo(label.c_str(), &reals, opts.c_str());
        return reals + 1;
    };
    /// Add unique identifier to the stack, to distinguish identical inputs from each other. Put before the input.
    lua["win_pushid"] = [](int id) { ImGui::PushID(id); };
    /// Pop unique identifier from the stack. Put after the input.
    lua["win_popid"] = []() { ImGui::PopID(); };
    /// Draw image to window.
    lua["win_image"] = [this](int image, int width, int height) {
        if (image < 0 || images.find(image) == images.end())
            return;
        if (width < 1)
            width = images[image]->width;
        if (height < 1)
            height = images[image]->height;
        ImGui::Image(images[image]->texture, ImVec2(width, height));
    };

    lua.new_usertype<Color>("Color", "r", &Color::r, "g", &Color::g, "b", &Color::b, "a", &Color::a);
    lua.new_usertype<Inventory>(
        "Inventory",
        "money",
        &Inventory::money,
        "bombs",
        &Inventory::bombs,
        "ropes",
        &Inventory::ropes,
        "kills_level",
        &Inventory::kills_level,
        "kills_total",
        &Inventory::kills_total);
    lua.new_usertype<Animation>(
        "Animation",
        "first_tile",
        &Animation::texture,
        "num_tiles",
        &Animation::count,
        "interval",
        &Animation::interval,
        "repeat_mode",
        &Animation::repeat);
    lua.new_usertype<EntityDB>(
        "EntityDB",
        "id",
        &EntityDB::id,
        "search_flags",
        &EntityDB::search_flags,
        "width",
        &EntityDB::width,
        "height",
        &EntityDB::height,
        "draw_depth",
        &EntityDB::draw_depth,
        "friction",
        &EntityDB::friction,
        "elasticity",
        &EntityDB::elasticity,
        "weight",
        &EntityDB::weight,
        "acceleration",
        &EntityDB::acceleration,
        "max_speed",
        &EntityDB::max_speed,
        "sprint_factor",
        &EntityDB::sprint_factor,
        "jump",
        &EntityDB::jump,
        "damage",
        &EntityDB::damage,
        "life",
        &EntityDB::life,
        "blood_content",
        &EntityDB::blood_content,
        "animations",
        &EntityDB::animations,
        "properties_flags",
        &EntityDB::properties_flags,
        "default_flags",
        &EntityDB::default_flags,
        "default_more_flags",
        &EntityDB::default_more_flags);

    lua.new_usertype<Entity>(
        "Entity",
        "type",
        &Entity::type,
        "overlay",
        &Entity::overlay,
        "flags",
        &Entity::flags,
        "more_flags",
        &Entity::more_flags,
        "uid",
        &Entity::uid,
        "animation_frame",
        &Entity::animation_frame,
        "x",
        &Entity::x,
        "y",
        &Entity::y,
        "width",
        &Entity::w,
        "height",
        &Entity::h,
        "angle",
        &Movable::angle,
        "topmost",
        &Entity::topmost,
        "topmost_mount",
        &Entity::topmost_mount,
        "as_movable",
        &Entity::as<Movable>,
        "as_door",
        &Entity::as<Door>,
        "as_container",
        &Entity::as<Container>,
        "as_mattock",
        &Entity::as<Mattock>,
        "as_mount",
        &Entity::as<Mount>,
        "as_player",
        &Entity::as<Player>,
        "as_monster",
        &Entity::as<Monster>,
        "as_gun",
        &Entity::as<Gun>,
        "as_bomb",
        &Entity::as<Bomb>,
        "as_crushtrap",
        &Entity::as<Crushtrap>,
        "as_arrowtrap",
        &Entity::as<Arrowtrap>,
        "as_olmec",
        &Entity::as<Olmec>,
        "as_olmec_floater",
        &Entity::as<OlmecFloater>,
        "as_cape",
        &Entity::as<Cape>,
        "as_vlads_cape",
        &Entity::as<VladsCape>,
        "as_ghost",
        &Entity::as<Ghost>);
    lua.new_usertype<Movable>(
        "Movable",
        "movex",
        &Movable::movex,
        "movey",
        &Movable::movey,
        "buttons",
        &Movable::buttons,
        "stand_counter",
        &Movable::stand_counter,
        "owner_uid",
        &Movable::owner_uid,
        "last_owner_uid",
        &Movable::last_owner_uid,
        "idle_counter",
        &Movable::idle_counter,
        "standing_on_uid",
        &Movable::standing_on_uid,
        "velocityx",
        &Movable::velocityx,
        "velocityy",
        &Movable::velocityy,
        "holding_uid",
        &Movable::holding_uid,
        "state",
        &Movable::state,
        "last_state",
        &Movable::last_state,
        "move_state",
        &Movable::move_state,
        "health",
        &Movable::health,
        "stun_timer",
        &Movable::stun_timer,
        "stun_state",
        &Movable::stun_state,
        "some_state",
        &Movable::some_state,
        "color",
        &Movable::color,
        "hitboxx",
        &Movable::hitboxx,
        "hitboxy",
        &Movable::hitboxy,
        "offsetx",
        &Movable::offsetx,
        "offsety",
        &Movable::offsety,
        "airtime",
        &Movable::airtime,
        "is_poisoned",
        &Movable::is_poisoned,
        "poison",
        &Movable::poison,
        "price",
        &Movable::price,
        sol::base_classes,
        sol::bases<Entity>());
        /* Movable
            bool is_poisoned()
            void poison(int16_t frames)
        */
    lua.new_usertype<Monster>("Monster", sol::base_classes, sol::bases<Entity, Movable>());
    lua.new_usertype<Player>(
        "Player",
        "inventory",
        &Player::inventory_ptr,
        "set_jetpack_fuel",
        &Player::set_jetpack_fuel,
        "kapala_blood_amount",
        &Player::kapala_blood_amount,
        sol::base_classes,
        sol::bases<Entity, Movable, Monster>());
    lua.new_usertype<Mount>("Mount", "carry", &Mount::carry, "tame", &Mount::tame, sol::base_classes, sol::bases<Entity, Movable, Monster>());
    lua.new_usertype<Bomb>("Bomb", "scale_hor", &Bomb::scale_hor, "scale_ver", &Bomb::scale_ver, sol::base_classes, sol::bases<Entity, Movable>());
    lua.new_usertype<Container>(
        "Container", "inside", &Container::inside, "timer", &Container::timer, sol::base_classes, sol::bases<Entity, Movable>());
    lua.new_usertype<Gun>(
        "Gun",
        "cooldown",
        &Gun::cooldown,
        "shots",
        &Gun::shots,
        "shots2",
        &Gun::shots2,
        "in_chamber",
        &Gun::in_chamber,
        sol::base_classes,
        sol::bases<Entity, Movable>());
    lua.new_usertype<Crushtrap>("Crushtrap", "dirx", &Crushtrap::dirx, "diry", &Crushtrap::diry, sol::base_classes, sol::bases<Entity, Movable>());
    lua.new_usertype<Arrowtrap>("Arrowtrap", "arrow_shot", &Arrowtrap::arrow_shot, "rearm", &Arrowtrap::rearm, sol::base_classes, sol::bases<Entity>());
    lua.new_usertype<Olmec>(
        "Olmec",
        "attack_phase",
        &Olmec::attack_phase,
        "attack_timer",
        &Olmec::attack_timer,
        "ai_timer",
        &Olmec::ai_timer,
        "move_direction",
        &Olmec::move_direction,
        "jump_timer",
        &Olmec::jump_timer,
        "phase1_amount_of_bomb_salvos",
        &Olmec::phase1_amount_of_bomb_salvos,
        "unknown_attack_state",
        &Olmec::unknown_attack_state,
        "broken_floaters",
        &Olmec::broken_floaters,
        sol::base_classes,
        sol::bases<Entity, Movable>());
        /* Olmec
            int broken_floaters()
        */
    lua.new_usertype<OlmecFloater>(
        "OlmecFloater",
        "both_floaters_broken",
        &OlmecFloater::both_floaters_broken,
        sol::base_classes,
        sol::bases<Entity, Movable>());
    lua.new_usertype<Cape>(
        "Cape",
        "floating_down",
        &VladsCape::floating_down,
        sol::base_classes,
        sol::bases<Entity, Movable>());
    lua.new_usertype<VladsCape>(
        "VladsCape",
        "can_double_jump",
        &VladsCape::can_double_jump,
        sol::base_classes,
        sol::bases<Entity, Movable, Cape>());
    lua.new_usertype<Ghost>(
        "Ghost",
        "chased_target_uid",
        &Ghost::chased_target_uid,
        "split_timer",
        &Ghost::split_timer,
        "velocity_multiplier",
        &Ghost::velocity_multiplier,
        sol::base_classes,
        sol::bases<Entity, Movable, Monster>());
    lua.new_usertype<StateMemory>(
        "StateMemory",
        "screen_last",
        &StateMemory::screen_last,
        "screen",
        &StateMemory::screen,
        "screen_next",
        &StateMemory::screen_next,
        "ingame",
        &StateMemory::ingame,
        "playing",
        &StateMemory::playing,
        "pause",
        &StateMemory::pause,
        "width",
        &StateMemory::w,
        "height",
        &StateMemory::h,
        "kali_favor",
        &StateMemory::kali_favor,
        "kali_status",
        &StateMemory::kali_status,
        "kali_altars_destroyed",
        &StateMemory::kali_altars_destroyed,
        "seed",
        &StateMemory::seed,
        "time_total",
        &StateMemory::time_total,
        "world",
        &StateMemory::world,
        "world_next",
        &StateMemory::world_next,
        "world_start",
        &StateMemory::world_start,
        "level",
        &StateMemory::level,
        "level_next",
        &StateMemory::level_next,
        "level_start",
        &StateMemory::level_start,
        "theme",
        &StateMemory::theme,
        "theme_next",
        &StateMemory::theme_next,
        "theme_start",
        &StateMemory::theme_start,
        "shoppie_aggro",
        &StateMemory::shoppie_aggro,
        "shoppie_aggro_next",
        &StateMemory::shoppie_aggro_levels,
        "merchant_aggro",
        &StateMemory::merchant_aggro,
        "kills_npc",
        &StateMemory::kills_npc,
        "level_count",
        &StateMemory::level_count,
        "journal_flags",
        &StateMemory::journal_flags,
        "time_last_level",
        &StateMemory::time_last_level,
        "time_level",
        &StateMemory::time_level,
        "level_flags",
        &StateMemory::hud_flags,
        "loading",
        &StateMemory::loading,
        "quest_flags",
        &StateMemory::quest_flags,
        "fadeout",
        &StateMemory::fadeout,
        "fadein",
        &StateMemory::fadein,
        "saved_dogs",
        &StateMemory::saved_dogs,
        "saved_cats",
        &StateMemory::saved_cats,
        "saved_hamsters",
        &StateMemory::saved_hamsters,
        "win_state",
        &StateMemory::win_state);
    auto play = sol::overload(
        static_cast<PlayingSound(CustomSound::*)()>(&CustomSound::play),
        static_cast<PlayingSound(CustomSound::*)(bool)>(&CustomSound::play),
        static_cast<PlayingSound(CustomSound::*)(bool, SoundType)>(&CustomSound::play));
    /// Handle to a loaded sound, can be used to play the sound and receive a PlayingSound for more control
    /// It is up to you to not release this as long as any sounds returned by CustomSound:play() are still playing
    lua.new_usertype<CustomSound>(
        "CustomSound",
        "play",
        play);
    /* CustomSound
    PlayingSound play(bool start_paused, SOUND_TYPE sound_type)
    */
    /// Handle to a playing sound, start the sound paused to make sure you can apply changes before playing it
    /// You can just discard this handle if you do not need extended control anymore
    lua.new_usertype<PlayingSound>(
        "PlayingSound",
        "is_playing",
        &PlayingSound::is_playing,
        "stop",
        &PlayingSound::stop,
        "set_pause",
        &PlayingSound::set_pause,
        "set_mute",
        &PlayingSound::set_mute,
        "set_pitch",
        &PlayingSound::set_pitch,
        "set_pan",
        &PlayingSound::set_pan,
        "set_volume",
        &PlayingSound::set_volume,
        "set_looping",
        &PlayingSound::set_looping,
        "set_callback",
        &PlayingSound::set_callback);
    /* PlayingSound
    bool is_playing()
    bool stop()
    bool set_pause(bool pause)
    bool set_mute(bool mute)
    bool set_pitch(float pitch)
    bool set_pan(float pan)
    bool set_volume(float volume)
    bool set_looping(SOUND_LOOP_MODE looping)
    bool set_callback(function callback)
    */

    #define table_of(T, name) sol::property([this]() { \
        return sol::as_table_ref(std::vector<T>(g_save->name, g_save->name + sizeof g_save->name / sizeof g_save->name[0])); \
    })

    lua.new_usertype<SaveData>(
        "SaveData",
        "places",
        table_of(bool, places),
        "bestiary",
        table_of(bool, bestiary),
        "people",
        table_of(bool, people),
        "items",
        table_of(bool, items),
        "traps",
        table_of(bool, traps),
        "last_daily",
        sol::readonly(&SaveData::last_daily),
        "characters",
        sol::readonly(&SaveData::characters),
        "shortcuts",
        sol::readonly(&SaveData::shortcuts),
        "bestiary_killed",
        table_of(int, bestiary_killed),
        "bestiary_killed_by",
        table_of(int, bestiary_killed_by),
        "people_killed",
        table_of(int, people_killed),
        "people_killed_by",
        table_of(int, people_killed_by),
        "plays",
        sol::readonly(&SaveData::plays),
        "deaths",
        sol::readonly(&SaveData::deaths),
        "wins_normal",
        sol::readonly(&SaveData::wins_normal),
        "wins_hard",
        sol::readonly(&SaveData::wins_hard),
        "wins_special",
        sol::readonly(&SaveData::wins_special),
        "score_total",
        sol::readonly(&SaveData::score_total),
        "score_top",
        sol::readonly(&SaveData::score_top),
        "deepest_area",
        sol::readonly(&SaveData::deepest_area),
        "deepest_level",
        sol::readonly(&SaveData::deepest_level));

    lua.create_named_table("ENT_TYPE");
    for (int i = 0; i < g_items.size(); i++)
    {
        auto name = g_items[i].name.substr(9, g_items[i].name.size());
        lua["ENT_TYPE"][name] = g_items[i].id;
    }
    lua.new_enum(
        "THEME",
        "DWELLING",
        1,
        "JUNGLE",
        2,
        "VOLCANA",
        3,
        "OLMEC",
        4,
        "TIDE_POOL",
        5,
        "TEMPLE",
        6,
        "ICE_CAVES",
        7,
        "NEO_BABYLON",
        8,
        "SUNKEN_CITY",
        9,
        "COSMIC_OCEAN",
        10,
        "CITY_OF_GOLD",
        11,
        "DUAT",
        12,
        "ABZU",
        13,
        "TIAMAT",
        14,
        "EGGPLANT_WORLD",
        15,
        "HUNDUN",
        16,
        "BASE_CAMP",
        17,
        "ARENA",
        18);
    lua.new_enum(
        "ON",
        "LOGO",
        0,
        "INTRO",
        1,
        "PROLOGUE",
        2,
        "TITLE",
        3,
        "MENU",
        4,
        "OPTIONS",
        5,
        "LEADERBOARD",
        7,
        "SEED_INPUT",
        8,
        "CHARACTER_SELECT",
        9,
        "TEAM_SELECT",
        10,
        "CAMP",
        11,
        "LEVEL",
        12,
        "TRANSITION",
        13,
        "DEATH",
        14,
        "SPACESHIP",
        15,
        "WIN",
        16,
        "CREDITS",
        17,
        "SCORES",
        18,
        "CONSTELLATION",
        19,
        "RECAP",
        20,
        "ARENA_MENU",
        21,
        "ARENA_INTRO",
        25,
        "ARENA_MATCH",
        26,
        "ARENA_SCORE",
        27,
        "ONLINE_LOADING",
        28,
        "ONLINE_LOBBY",
        29,
        "GUIFRAME",
        100,
        "FRAME",
        101,
        "SCREEN",
        102,
        "START",
        103,
        "LOADING",
        104,
        "RESET",
        105);
    lua.new_enum("LAYER", "FRONT", 0, "BACK", 1, "PLAYER", -1, "PLAYER1", -1, "PLAYER2", -2, "PLAYER3", -3, "PLAYER4", -4);
    lua.new_enum(
        "MASK",
        "PLAYER",
        0x1,
        "MOUNT",
        0x2,
        "MONSTER",
        0x4,
        "ITEM",
        0x8,
        "EXPLOSION",
        0x10,
        "FX",
        0x40,
        "ACTIVEFLOOR",
        0x80,
        "FLOOR",
        0x100,
        "DECORATION",
        0x200,
        "BG",
        0x400,
        "SHADOW",
        0x800,
        "LOGICAL",
        0x1000,
        "WATER",
        0x2000,
        "LAVA",
        0x4000);
    /// Third parameter to CustomSound:play(), specifies which group the sound will be played in and thus how the player controls its volume
    lua.new_enum("SOUND_TYPE", "SFX", 0, "MUSIC", 1);
    /// Paramater to PlayingSound:set_looping(), specifies what type of looping this sound should do
    lua.new_enum("SOUND_LOOP_MODE", "OFF", 0, "LOOP", 1, "BIDIRECTIONAL", 2);
    lua.new_enum("CONST", "ENGINE_FPS", 60);
}

bool SpelunkyScript::ScriptImpl::run()
{
    if (!enabled)
        return true;
    if (changed)
    {
        result = "";
        changed = false;
        // Compile & Evaluate the script if the script is changed
        try
        {
            // Clear all callbacks on script reload to avoid running them
            // multiple times.
            level_timers.clear();
            global_timers.clear();
            callbacks.clear();
            options.clear();
            required_scripts.clear();
            lua["on_guiframe"] = sol::lua_nil;
            lua["on_frame"] = sol::lua_nil;
            lua["on_camp"] = sol::lua_nil;
            lua["on_start"] = sol::lua_nil;
            lua["on_level"] = sol::lua_nil;
            lua["on_transition"] = sol::lua_nil;
            lua["on_death"] = sol::lua_nil;
            lua["on_win"] = sol::lua_nil;
            lua["on_screen"] = sol::lua_nil;
            if (meta.unsafe)
            {
                lua["package"]["path"] = meta.path + "/?.lua;" + meta.path + "/?/init.lua";
                lua["package"]["cpath"] = meta.path + "/?.dll;" + meta.path + "/?/init.dll";
                lua.open_libraries(sol::lib::io, sol::lib::os, sol::lib::ffi, sol::lib::debug);
            }
            else
            {
                lua["package"]["path"] = meta.path + "/?.lua;" + meta.path + "/?/init.lua";
                lua["package"]["cpath"] = "";
                lua["package"]["loadlib"] = sol::lua_nil;
            }
            auto lua_result = lua.safe_script(code);
            result = "OK";
        }
        catch (const sol::error& e)
        {
            result = e.what();
            messages.push_back({ result, std::chrono::system_clock::now(), ImVec4(1.0f, 0.2f, 0.2f, 1.0f) });
            DEBUG("{}", result);
            if (messages.size() > 20)
                messages.pop_front();
            return false;
        }
    }
    try
    {
        lua_sethook(lua.lua_state(), NULL, 0, 0);
        lua_sethook(lua.lua_state(), &infinite_loop, LUA_MASKCOUNT, 1000000000);
        sol::optional<std::string> meta_name = lua["meta"]["name"];
        sol::optional<std::string> meta_version = lua["meta"]["version"];
        sol::optional<std::string> meta_description = lua["meta"]["description"];
        sol::optional<std::string> meta_author = lua["meta"]["author"];
        sol::optional<std::string> meta_id = lua["meta"]["id"];
        meta.name = meta_name.value_or(meta.file);
        meta.version = meta_version.value_or("");
        meta.description = meta_description.value_or("");
        meta.author = meta_author.value_or("Anonymous");
        meta.id = script_id();

        /// Runs on every game engine frame.
        sol::optional<sol::function> on_frame = lua["on_frame"];
        /// Runs on entering the camp.
        sol::optional<sol::function> on_camp = lua["on_camp"];
        /// Runs on the start of every level.
        sol::optional<sol::function> on_level = lua["on_level"];
        /// Runs on the start of first level.
        sol::optional<sol::function> on_start = lua["on_start"];
        /// Runs on the start of level transition.
        sol::optional<sol::function> on_transition = lua["on_transition"];
        /// Runs on the death screen.
        sol::optional<sol::function> on_death = lua["on_death"];
        /// Runs on any ending cutscene.
        sol::optional<sol::function> on_win = lua["on_win"];
        /// Runs on any [screen change](#on).
        sol::optional<sol::function> on_screen = lua["on_screen"];
        g_players = get_players();
        lua["players"] = std::vector<Player*>(g_players.begin(), g_players.end());
        if (g_state->screen != state.screen && g_state->screen_last != 5)
        {
            level_timers.clear();
            script_input.clear();
            if (on_screen)
                on_screen.value()();
        }
        if (on_frame && g_state->time_level != state.time_level && g_state->screen == 12)
        {
            on_frame.value()();
        }
        if (g_state->screen == 11 && state.screen != 11)
        {
            if (on_camp)
                on_camp.value()();
        }
        if (g_state->screen == 12 && g_state->screen_last != 5 && !g_players.empty() && (state.player != g_players.at(0) || ((g_state->quest_flags & 1) == 0 && state.reset > 0)))
        {
            if (g_state->level_count == 0)
            {
                if (on_start)
                    on_start.value()();
            }
            if (on_level)
                on_level.value()();
        }
        if (g_state->screen == 13 && state.screen != 13)
        {
            if (on_transition)
                on_transition.value()();
        }
        if (g_state->screen == 14 && state.screen != 14)
        {
            if (on_death)
                on_death.value()();
        }
        if ((g_state->screen == 16 && state.screen != 16) || (g_state->screen == 19 && state.screen != 19))
        {
            if (on_win)
                on_win.value()();
        }

        for (auto id : clear_callbacks)
        {
            auto it = level_timers.find(id);
            if (it != level_timers.end())
                level_timers.erase(id);

            auto it2 = global_timers.find(id);
            if (it2 != global_timers.end())
                global_timers.erase(id);

            auto it3 = callbacks.find(id);
            if (it3 != callbacks.end())
                callbacks.erase(id);
        }
        clear_callbacks.clear();

        for (auto it = global_timers.begin(); it != global_timers.end();)
        {
            auto now = get_frame_count();
            if (auto cb = std::get_if<IntervalCallback>(&it->second))
            {
                if (now >= cb->lastRan + cb->interval)
                {
                    handle_function(cb->func);
                    cb->lastRan = now;
                }
                ++it;
            }
            else if (auto cb = std::get_if<TimeoutCallback>(&it->second))
            {
                if (now >= cb->timeout)
                {
                    handle_function(cb->func);
                    it = global_timers.erase(it);
                }
                else
                {
                    ++it;
                }
            }
            else
            {
                ++it;
            }
        }

        for (auto& [id, callback] : callbacks)
        {
            auto now = get_frame_count();
            if (auto* cb = std::get_if<ScreenCallback>(&callback))
            {
                if (g_state->screen == cb->screen && g_state->screen != state.screen && g_state->screen_last != 5) // game screens
                {
                    handle_function(cb->func);
                    cb->lastRan = now;
                }
                else if (cb->screen == 12 && g_state->screen == 12 && g_state->screen_last != 5 && !g_players.empty() && (state.player != g_players.at(0) || ((g_state->quest_flags & 1) == 0 && state.reset > 0)))
                { // run ON.LEVEL on instant restart too
                    handle_function(cb->func);
                    cb->lastRan = now;
                }
                else if (cb->screen == 101 && g_state->time_level != state.time_level && g_state->screen == 12) // ON.FRAME
                {
                    handle_function(cb->func);
                    cb->lastRan = now;
                }
                else if (cb->screen == 102 && g_state->screen != state.screen) // ON.SCREEN
                {
                    handle_function(cb->func);
                    cb->lastRan = now;
                }
                else if (
                    cb->screen == 103 && g_state->screen == 12 && g_state->level_count == 0 && !g_players.empty() &&
                    state.player != g_players.at(0)) // ON.START
                {
                    handle_function(cb->func);
                    cb->lastRan = now;
                }
                else if (cb->screen == 104 && g_state->loading > 0 && g_state->loading != state.loading) // ON.LOADING
                {
                    handle_function(cb->func);
                    cb->lastRan = now;
                }
                else if (cb->screen == 105 && (g_state->quest_flags & 1) > 0 && (g_state->quest_flags & 1) != state.reset) // ON.RESET
                {
                    handle_function(cb->func);
                    cb->lastRan = now;
                }
            }
        }

        for (auto it = level_timers.begin(); it != level_timers.end();)
        {
            auto now = g_state->time_level;
            if (auto cb = std::get_if<IntervalCallback>(&it->second))
            {
                if (now >= cb->lastRan + cb->interval)
                {
                    handle_function(cb->func);
                    cb->lastRan = now;
                }
                ++it;
            }
            else if (auto cb = std::get_if<TimeoutCallback>(&it->second))
            {
                if (now >= cb->timeout)
                {
                    handle_function(cb->func);
                    it = level_timers.erase(it);
                }
                else
                {
                    ++it;
                }
            }
            else
            {
                ++it;
            }
        }

        if (!g_players.empty())
            state.player = g_players.at(0);
        else
            state.player = nullptr;
        state.screen = g_state->screen;
        state.time_level = g_state->time_level;
        state.time_total = g_state->time_total;
        state.frame = get_frame_count();
        state.loading = g_state->loading;
        state.reset = (g_state->quest_flags & 1);
        state.quest_flags = g_state->quest_flags;
    }
    catch (const sol::error& e)
    {
        result = e.what();
        return false;
    }
    return true;
}

void SpelunkyScript::ScriptImpl::draw(ImDrawList* dl)
{
    if (!enabled)
        return;
    draw_list = dl;
    try {
        /// Runs on every screen frame. You need this to use draw functions.
        sol::optional<sol::function> on_guiframe = lua["on_guiframe"];

        if (on_guiframe)
        {
            on_guiframe.value()();
        }

        for (auto& [id, callback] : callbacks)
        {
            auto now = get_frame_count();
            if (auto* cb = std::get_if<ScreenCallback>(&callback))
            {
                if (cb->screen == 100) // ON.GUIFRAME
                {
                    handle_function(cb->func);
                    cb->lastRan = now;
                }
            }
        }
    }
    catch (const sol::error& e)
    {
        result = e.what();
    }
    draw_list = nullptr;
}

std::string SpelunkyScript::ScriptImpl::script_id()
{
    std::string newid = sanitize(meta.author) + "/" + sanitize(meta.name);
    return newid;
}

bool SpelunkyScript::ScriptImpl::handle_function(sol::function func)
{
    auto lua_result = func();
    if (!lua_result.valid())
    {
        sol::error e = lua_result;
        result = e.what();
        messages.push_back({ result, std::chrono::system_clock::now(), ImVec4(1.0f, 0.2f, 0.2f, 1.0f) });
        DEBUG("{}", result);
        if (messages.size() > 20)
            messages.pop_front();
        return false;
    }
    return true;
}

void SpelunkyScript::ScriptImpl::render_options()
{
    ImGui::PushID(meta.id.data());
    for (auto& name_option_pair : options)
    {
        std::visit(overloaded{
            [&](IntOption& option) {
                if (ImGui::DragInt(name_option_pair.second.desc.c_str(), &option.value, 0.5f, option.min, option.max))
                {
                    auto& name = name_option_pair.first;
                    lua["options"][name] = option.value;
                }
            },
            [&](FloatOption& option) {
                if (ImGui::DragFloat(name_option_pair.second.desc.c_str(), &option.value, 0.5f, option.min, option.max))
                {
                    auto& name = name_option_pair.first;
                    lua["options"][name] = option.value;
                }
            },
            [&](BoolOption& option) {
                if (ImGui::Checkbox(name_option_pair.second.desc.c_str(), &option.value))
                {
                    auto& name = name_option_pair.first;
                    lua["options"][name] = option.value;
                }
            },
            [&](StringOption& option) {
                if (InputString(name_option_pair.second.desc.c_str(), &option.value, 0, nullptr, nullptr))
                {
                    auto& name = name_option_pair.first;
                    lua["options"][name] = option.value;
                }
            },
            [&](ComboOption& option) {
                if (ImGui::Combo(name_option_pair.second.desc.c_str(), &option.value, option.options.c_str()))
                {
                    auto& name = name_option_pair.first;
                    lua["options"][name] = option.value + 1;
                }
            },
            [&](ButtonOption& option) {
                if (ImGui::Button(name_option_pair.second.desc.c_str()))
                {
                    uint64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                    auto& name = name_option_pair.first;
                    lua["options"][name] = now;
                    handle_function(option.on_click);
                }
            },
        }, name_option_pair.second.option_impl);
        if (!name_option_pair.second.long_desc.empty()) {
            ImGui::TextWrapped("%s", name_option_pair.second.long_desc.c_str());
        }
    }
    ImGui::PopID();
}


SpelunkyScript::SpelunkyScript(std::string script, std::string file, SoundManager* sound_manager, bool enable)
    : m_Impl{ new ScriptImpl(std::move(script), std::move(file), sound_manager, enable) }
{
}
SpelunkyScript::~SpelunkyScript() = default; // Has to be in the source file

std::deque<ScriptMessage>& SpelunkyScript::get_messages()
{
    return m_Impl->messages;
}
std::vector<std::string> SpelunkyScript::consume_requires()
{
    return std::move(m_Impl->required_scripts);
}

const std::string& SpelunkyScript::get_id() const
{
    return m_Impl->meta.id;
}
const std::string& SpelunkyScript::get_name() const
{
    return m_Impl->meta.name;
}
const std::string& SpelunkyScript::get_description() const
{
    return m_Impl->meta.description;
}
const std::string& SpelunkyScript::get_author() const
{
    return m_Impl->meta.author;
}
const std::string& SpelunkyScript::get_file() const
{
    return m_Impl->meta.file;
}
const std::string& SpelunkyScript::get_filename() const
{
    return m_Impl->meta.filename;
}
const std::string& SpelunkyScript::get_version() const
{
    return m_Impl->meta.version;
}
const std::string& SpelunkyScript::get_path() const
{
    return m_Impl->meta.path;
}
bool SpelunkyScript::get_unsafe() const
{
    return m_Impl->meta.unsafe;
}

#ifdef SPEL2_EDITABLE_SCRIPTS
char* SpelunkyScript::get_code() const
{
    return m_Impl->code;
}
std::size_t SpelunkyScript::get_code_size() const
{
    return sizeof(m_Impl->code);
}
#endif

void SpelunkyScript::update_code(std::string code)
{
#ifdef SPEL2_EDITABLE_SCRIPTS
    strcpy(m_Impl->code, code.c_str());
#else
    m_Impl->code_storage = std::move(code);
    m_Impl->code = m_Impl->code_storage.c_str();
#endif
    m_Impl->changed = true;
}

std::string& SpelunkyScript::get_result()
{
    return m_Impl->result;
}

bool SpelunkyScript::is_enabled() const
{
    return m_Impl->enabled;
}
void SpelunkyScript::set_enabled(bool enabled)
{
    m_Impl->enabled = enabled;
}

bool SpelunkyScript::is_changed() const
{
    return m_Impl->changed;
}
void SpelunkyScript::set_changed(bool changed)
{
    m_Impl->changed = changed;
}

bool SpelunkyScript::run()
{
    return m_Impl->run();
}
void SpelunkyScript::draw(ImDrawList* dl)
{
    m_Impl->draw(dl);
}
void SpelunkyScript::render_options()
{
    m_Impl->render_options();
}
