#include <algorithm>
#include <array>
#include <chrono>
#include <codecvt>
#include <fstream>
#include <iomanip>
#include <locale>
#include <map>
#include <string>

#include "entity.hpp"
#include "logger.h"
#include "rpc.hpp"
#include "state.hpp"

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

struct LuaIntervalCallback
{
    sol::function func;
    int interval;
    int lastRan;
};

struct LuaTimeoutCallback
{
    sol::function func;
    int timeout;
};

struct ScriptState
{
    Player *player;
    uint32_t screen;
};

struct ScriptOption
{
    std::string desc;
    std::variant<int, float, std::string, bool> value;
    std::variant<int, float> min;
    std::variant<int, float> max;
};

struct ScriptMeta
{
    std::string file;
    std::string name;
    std::string version;
    std::string description;
    std::string author;
};

std::vector<Movable *> lua_get_players();
Movable *lua_get_entity(uint32_t id);
std::tuple<float, float, int> lua_get_position(uint32_t id);

class Script
{
  public:
    sol::state lua;
    char code[204800];
    std::string result = "";
    ScriptState state = {0, 0};
    bool changed = false;
    bool enabled = true;
    ScriptMeta meta = {"", "", "", "", ""};

    std::map<std::string, ScriptOption> options;
    std::vector<std::variant<LuaIntervalCallback, LuaTimeoutCallback>> callbacks;
    std::vector<std::pair<std::string, std::chrono::time_point<std::chrono::system_clock>>> messages;

    StateMemory *g_state = nullptr;
    std::vector<EntityItem> g_items;
    std::vector<Player *> g_players;

    Script(std::string script, std::string file);
    ~Script();

    void add_message(std::string message);
    void register_option_int(std::string name, std::string desc, int value, int min, int max);
    void register_option_bool(std::string name, std::string desc, bool value);

    bool run();
};
