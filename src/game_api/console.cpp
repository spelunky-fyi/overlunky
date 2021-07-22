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

void SpelunkyConsole::toggle()
{
    m_Impl->toggle();
}

std::string SpelunkyConsole::dump_api()
{
    return m_Impl->dump_api();
}
