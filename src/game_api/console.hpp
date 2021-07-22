#pragma once

#include <deque>
#include <memory>
#include <string>
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

    std::string dump_api();

    class LuaConsole* get_impl()
    {
        return m_Impl.get();
    }

  private:
    std::unique_ptr<class LuaConsole> m_Impl;
};
