#include "console.hpp"

#include <exception>   // for exception
#include <fstream>     // for basic_ostream, basic_ios, basic_if...
#include <locale>      // for num_put, num_get
#include <new>         // for operator new
#include <sol/sol.hpp> // for table_proxy, protected_function_re...
#include <tuple>       // for get
#include <type_traits> // for move, _Identity_t
#include <utility>     // for max, min

#include "script.hpp"             // for ScriptMessage
#include "script/lua_console.hpp" // for LuaConsole, ConsoleHistoryItem

SpelunkyConsole::SpelunkyConsole(SoundManager* sound_manager)
    : m_Impl{new LuaConsole(sound_manager)}
{
}
SpelunkyConsole::~SpelunkyConsole() = default;

void SpelunkyConsole::loop_messages(std::function<void(const ScriptMessage&)> message_fun) const
{
    auto impl = m_Impl->Lock();
    for (const ScriptMessage& message : impl->messages)
    {
        message_fun(message);
    }
}
std::vector<std::string> SpelunkyConsole::consume_requires()
{
    return std::move(m_Impl->Lock()->required_scripts);
}
std::deque<ScriptMessage> SpelunkyConsole::consume_messages()
{
    return std::move(m_Impl->Lock()->messages);
}

bool SpelunkyConsole::is_enabled()
{
    // Note: Does not mean the console is showing, only that it should get updated
    return true;
}
bool SpelunkyConsole::is_toggled()
{
    // Directly access `enabled` here, the implementation of `get_enabled` is just faking it
    return m_Impl->Lock()->enabled;
}

void SpelunkyConsole::draw(ImDrawList* dl)
{
    m_Impl->Lock()->draw(dl);
}
void SpelunkyConsole::render_options()
{
    m_Impl->Lock()->render_options();
}

std::string SpelunkyConsole::execute(std::string code)
{
    return m_Impl->Lock()->execute(std::move(code));
}

bool SpelunkyConsole::has_new_history() const
{
    return m_Impl->Lock()->has_new_history;
}
void SpelunkyConsole::set_max_history_size(size_t max_history)
{
    m_Impl->Lock()->max_history = max_history;
}
void SpelunkyConsole::save_history(std::string_view path)
{
    if (std::ofstream history_file = std::ofstream(std::string{path}))
    {
        std::string line;
        auto impl = m_Impl->Lock();
        for (const auto& history_item : impl->history)
        {
            if (history_item.command.find("---") != 0)
                history_file << "> " + history_item.command << '\n';
        }
    }
}
void SpelunkyConsole::load_history(std::string_view path)
{
    if (std::ifstream history_file = std::ifstream(std::string{path}))
    {
        std::string line;
        std::string history_item;

        auto impl = m_Impl->Lock();
        while (std::getline(history_file, line))
        {
            if (line.starts_with("> "))
            {
                if (!history_item.empty())
                {
                    impl->push_history(std::move(history_item), {});
                }
                line = line.substr(2);
            }
            history_item += line;
            history_item += '\n';
        }

        if (!history_item.empty())
        {
            impl->push_history(std::move(history_item), {});
        }
    }
}
void SpelunkyConsole::push_history(std::string history_item, std::vector<ScriptMessage> result_item)
{
    m_Impl->Lock()->push_history(std::move(history_item), std::move(result_item));
}

void SpelunkyConsole::toggle()
{
    m_Impl->Lock()->toggle();
}

std::string SpelunkyConsole::dump_api()
{
    return m_Impl->Lock()->dump_api();
}

void SpelunkyConsole::set_selected_uid(uint32_t uid)
{
    auto impl = m_Impl->Lock();
    impl->lua["U"] = uid;
    impl->lua["E"] = impl->lua["get_entity"](uid);
    impl->lua["P"] = impl->lua["get_player"](1);
}

unsigned int SpelunkyConsole::get_input_lines()
{
    return m_Impl->Lock()->get_input_lines();
}

void SpelunkyConsole::set_geometry(float x, float y, float w, float h)
{
    m_Impl->Lock()->set_geometry(x, y, w, h);
}
