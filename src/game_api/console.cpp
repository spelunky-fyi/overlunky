#include "console.hpp"

#include "script/lua_console.hpp"

SpelunkyConsole::SpelunkyConsole(SoundManager* sound_manager)
    : m_Impl{new LuaConsole(sound_manager)}
{
}
SpelunkyConsole::~SpelunkyConsole() = default;

std::vector<std::string> SpelunkyConsole::consume_requires()
{
    return std::move(m_Impl->required_scripts);
}
std::deque<ScriptMessage> SpelunkyConsole::consume_messages()
{
    return std::move(m_Impl->messages);
}
const std::deque<ScriptMessage>& SpelunkyConsole::get_messages() const
{
    return m_Impl->messages;
}

bool SpelunkyConsole::is_enabled()
{
    // Note: Does not mean the console is showing, only that it should get updated
    return true;
}
bool SpelunkyConsole::is_toggled()
{
    return m_Impl->enabled;
}

bool SpelunkyConsole::run()
{
    return m_Impl->update();
}
void SpelunkyConsole::draw(ImDrawList* dl)
{
    m_Impl->draw(dl);
}
void SpelunkyConsole::render_options()
{
    m_Impl->render_options();
}

std::string SpelunkyConsole::execute(std::string code)
{
    return m_Impl->execute(std::move(code));
}

bool SpelunkyConsole::has_new_history() const
{
    return m_Impl->has_new_history;
}
void SpelunkyConsole::set_max_history_size(size_t max_history)
{
    m_Impl->max_history = max_history;
}
void SpelunkyConsole::save_history(std::string_view path)
{
    if (std::ofstream history_file = std::ofstream(path))
    {
        std::string line;
        std::string history_item;
        for (const auto& history_item : m_Impl->history)
        {
            history_file << "> " + history_item.command << '\n';
        }
    }
}
void SpelunkyConsole::load_history(std::string_view path)
{
    if (std::ifstream history_file = std::ifstream(path))
    {
        std::string line;
        std::string history_item;
        while (std::getline(history_file, line))
        {
            if (line.starts_with("> "))
            {
                if (!history_item.empty())
                {
                    m_Impl->push_history(std::move(history_item), {});
                }
                line = line.substr(2);
            }
            history_item += line;
            history_item += '\n';
        }

        if (!history_item.empty())
        {
            m_Impl->push_history(std::move(history_item), {});
        }
    }
}

void SpelunkyConsole::toggle()
{
    m_Impl->toggle();
}

std::string SpelunkyConsole::dump_api()
{
    return m_Impl->dump_api();
}
