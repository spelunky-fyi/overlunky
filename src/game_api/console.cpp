#include "console.hpp"

#include "script/lua_console.hpp"

SpelunkyConsole::SpelunkyConsole(SoundManager* sound_manager)
    : m_Impl{ new LuaConsole(sound_manager) }
{
}
SpelunkyConsole::~SpelunkyConsole() = default;

std::deque<ScriptMessage>& SpelunkyConsole::get_messages()
{
    return m_Impl->messages;
}
std::vector<std::string> SpelunkyConsole::consume_requires()
{
    return std::move(m_Impl->required_scripts);
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

std::string SpelunkyConsole::dump_api()
{
    return m_Impl->dump_api();
}
