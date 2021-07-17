#pragma once

#include "script.hpp"

class SpelunkyConsole
{
  public:
    SpelunkyConsole(class SoundManager* sound_manager);
    ~SpelunkyConsole();

    std::deque<ScriptMessage>& get_messages();
    std::vector<std::string> consume_requires();

    bool run();
    void draw(ImDrawList* dl);
    void render_options();

    std::string execute(std::string code);

    std::string dump_api();

    class LuaConsole* get_impl() { return m_Impl.get(); }

  private:
    std::unique_ptr<class LuaConsole> m_Impl;
};
