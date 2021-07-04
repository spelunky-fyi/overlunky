#include "script.hpp"

#include "script/script_impl.hpp"

std::vector<SpelunkyScript*> g_all_scripts;

SpelunkyScript::SpelunkyScript(std::string script, std::string file, SoundManager* sound_manager, bool enable)
    : m_Impl{new ScriptImpl(std::move(script), std::move(file), sound_manager, enable)}
{
    g_all_scripts.push_back(this);
}
SpelunkyScript::~SpelunkyScript()
{
    std::erase(g_all_scripts, this);
}

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

bool SpelunkyScript::pre_level_gen_spawn(std::string_view tile_code, float x, float y, int layer)
{
    return m_Impl->pre_level_gen_spawn(tile_code, x, y, layer);
}
void SpelunkyScript::post_level_gen_spawn(std::string_view tile_code, float x, float y, int layer)
{
    m_Impl->post_level_gen_spawn(tile_code, x, y, layer);
}

void SpelunkyScript::post_room_generation()
{
    m_Impl->post_room_generation();
}

Entity* SpelunkyScript::pre_entity_spawn(std::uint32_t entity_type, float x, float y, int layer, Entity* overlay, int spawn_type_flags)
{
    return m_Impl->pre_entity_spawn(entity_type, x, y, layer, overlay, spawn_type_flags);
}
void SpelunkyScript::post_entity_spawn(Entity* entity, int spawn_type_flags)
{
    m_Impl->post_entity_spawn(entity, spawn_type_flags);
}

std::string SpelunkyScript::dump_api()
{
    return m_Impl->dump_api();
}

void SpelunkyScript::for_each_script(std::function<bool(SpelunkyScript&)> fun)
{
    for (auto* script : g_all_scripts)
    {
        if (!fun(*script))
        {
            break;
        }
    }
}
