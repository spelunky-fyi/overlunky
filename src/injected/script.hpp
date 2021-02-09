#include <algorithm>
#include <array>
#include <chrono>
#include <codecvt>
#include <fstream>
#include <iomanip>
#include <locale>
#include <map>
#include <string>
#include <deque>

#include "entity.hpp"
#include "logger.h"
#include "rpc.hpp"
#include "state.hpp"

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

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
    Player *player;
    uint32_t screen;
    uint32_t time_level;
    uint32_t time_total;
    uint32_t frame;
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

Movable *get_entity(uint32_t id);
std::tuple<float, float, int> get_position(uint32_t id);

using Callback = std::variant<IntervalCallback, TimeoutCallback, ScreenCallback>;

class Script
{
  public:
    sol::state lua;
    char code[204800];
    std::string result = "";
    ScriptState state = {nullptr, 0, 0, 0, 0};
    bool changed = true;
    bool enabled = true;
    ScriptMeta meta = {"", "", "", "", ""};
    int cbcount = 0;

    std::map<std::string, ScriptOption> options;
    std::deque<std::pair<std::string, std::chrono::time_point<std::chrono::system_clock>>> messages;
    std::map<int, Callback> level_timers;
    std::map<int, Callback> global_timers;
    std::map<int, Callback> callbacks;
    std::vector<int> clear_callbacks;

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
