#pragma once

#include <string>
#include <memory>
#include <vector>

class SpelunkyConsole
{
  public:
    SpelunkyConsole(class SoundManager* sound_manager);
    ~SpelunkyConsole();

    std::vector<std::string> consume_requires();

    bool is_enabled();

    bool run();
    void draw(struct ImDrawList* dl);
    void render_options();
    
    void toggle();

    std::string execute(std::string code);

    std::string dump_api();

    class LuaConsole* get_impl() { return m_Impl.get(); }

  private:
    std::unique_ptr<class LuaConsole> m_Impl;
};
