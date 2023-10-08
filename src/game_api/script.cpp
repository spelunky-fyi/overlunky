#include "script.hpp"

#include <type_traits> // for move

#include "console.hpp"            // for SpelunkyConsole
#include "script/script_impl.hpp" // for ScriptImpl

SpelunkyScript::SpelunkyScript(std::string script, std::string file, SoundManager* sound_manager, class SpelunkyConsole* console, bool enable)
    : m_Impl{new ScriptImpl(std::move(script), std::move(file), sound_manager, console ? console->get_impl() : nullptr, enable)}
{
}
SpelunkyScript::~SpelunkyScript() = default;

void SpelunkyScript::loop_messages(std::function<void(const ScriptMessage&)> message_fun) const
{
    auto impl = m_Impl->Lock();
    for (const ScriptMessage& message : impl->messages)
    {
        message_fun(message);
    }
}
std::deque<ScriptMessage> SpelunkyScript::consume_messages()
{
    return std::move(m_Impl->Lock()->messages);
}
std::vector<std::string> SpelunkyScript::consume_requires()
{
    return std::move(m_Impl->Lock()->required_scripts);
}

std::string SpelunkyScript::get_id() const
{
    return m_Impl->Lock()->meta.id;
}
std::string SpelunkyScript::get_name() const
{
    return m_Impl->Lock()->meta.name;
}
std::string SpelunkyScript::get_description() const
{
    return m_Impl->Lock()->meta.description;
}
std::string SpelunkyScript::get_author() const
{
    return m_Impl->Lock()->meta.author;
}
std::string SpelunkyScript::get_file() const
{
    return m_Impl->Lock()->meta.file;
}
std::string SpelunkyScript::get_filename() const
{
    return m_Impl->Lock()->meta.filename;
}
std::string SpelunkyScript::get_version() const
{
    return m_Impl->Lock()->meta.version;
}
std::string SpelunkyScript::get_path() const
{
    return m_Impl->Lock()->meta.path;
}
bool SpelunkyScript::get_unsafe() const
{
    return m_Impl->Lock()->meta.unsafe;
}
bool SpelunkyScript::get_online_safe() const
{
    return m_Impl->Lock()->meta.online_safe;
}

void SpelunkyScript::get_meta(std::function<void(const ScriptMeta& meta)> meta_fun)
{
    auto impl = m_Impl->Lock();
    meta_fun(impl->meta);
}

#ifdef SPEL2_EDITABLE_SCRIPTS
std::string& SpelunkyScript::get_code() const
{
    return m_Impl->Lock()->code;
}
std::size_t SpelunkyScript::get_code_size() const
{
    return m_Impl->Lock()->code.size();
}
#endif

void SpelunkyScript::update_code(std::string code)
{
    auto impl = m_Impl->Lock();
#ifdef SPEL2_EDITABLE_SCRIPTS
    impl->code = code;
#else
    impl->code = std::move(code);
#endif
    impl->changed = true;
}

std::string SpelunkyScript::get_result()
{
    return m_Impl->Lock()->result;
}

bool SpelunkyScript::is_enabled() const
{
    return m_Impl->Lock()->enabled;
}
void SpelunkyScript::set_enabled(bool enabled)
{
    m_Impl->Lock()->set_enabled(enabled);
}

bool SpelunkyScript::is_changed() const
{
    return m_Impl->Lock()->changed;
}
void SpelunkyScript::set_changed(bool changed)
{
    m_Impl->Lock()->changed = changed;
}

void SpelunkyScript::draw(ImDrawList* dl)
{
    m_Impl->Lock()->draw(dl);
}
void SpelunkyScript::render_options()
{
    m_Impl->Lock()->render_options();
}
std::string SpelunkyScript::execute(std::string code)
{
    return m_Impl->Lock()->execute(code);
}
