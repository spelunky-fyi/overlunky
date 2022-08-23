#include "script.hpp"

#include <type_traits> // for move

#include "console.hpp"            // for SpelunkyConsole
#include "script/script_impl.hpp" // for ScriptImpl

SpelunkyScript::SpelunkyScript(std::string script, std::string file, SoundManager* sound_manager, class SpelunkyConsole* console, bool enable)
    : m_Impl{new ScriptImpl(std::move(script), std::move(file), sound_manager, console ? console->get_impl() : nullptr, enable)}
{
}
SpelunkyScript::~SpelunkyScript() = default;

std::deque<ScriptMessage>& SpelunkyScript::get_messages()
{
    return m_Impl->messages;
}
std::deque<ScriptMessage> SpelunkyScript::consume_messages()
{
    return std::move(m_Impl->messages);
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
bool SpelunkyScript::get_online_safe() const
{
    return m_Impl->meta.online_safe;
}

#ifdef SPEL2_EDITABLE_SCRIPTS
std::string& SpelunkyScript::get_code() const
{
    return m_Impl->code;
}
std::size_t SpelunkyScript::get_code_size() const
{
    return m_Impl->code.size();
}
#endif

void SpelunkyScript::update_code(std::string code)
{
#ifdef SPEL2_EDITABLE_SCRIPTS
    m_Impl->code = code;
#else
    m_Impl->code = std::move(code);
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
    m_Impl->set_enabled(enabled);
}

bool SpelunkyScript::is_changed() const
{
    return m_Impl->changed;
}
void SpelunkyScript::set_changed(bool changed)
{
    m_Impl->changed = changed;
}

void SpelunkyScript::draw(ImDrawList* dl)
{
    m_Impl->draw(dl);
}
void SpelunkyScript::render_options()
{
    m_Impl->render_options();
}
