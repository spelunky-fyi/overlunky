#pragma once

#include <deque>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

class SpelunkyConsole
{
  public:
    SpelunkyConsole(class SoundManager* sound_manager);
    ~SpelunkyConsole();

    std::vector<std::string> consume_requires();
    std::deque<struct ScriptMessage> consume_messages();
    const std::deque<ScriptMessage>& get_messages() const;

    bool is_enabled();
    bool is_toggled();

    bool run();
    void draw(struct ImDrawList* dl);
    void render_options();

    void toggle();

    std::string execute(std::string code);

    bool has_new_history() const;
    void set_max_history_size(size_t max_history);
    void save_history(std::string_view path);
    void load_history(std::string_view path);

    std::string dump_api();

    class LuaConsole* get_impl()
    {
        return m_Impl.get();
    }

  private:
    std::unique_ptr<class LuaConsole> m_Impl;
};
