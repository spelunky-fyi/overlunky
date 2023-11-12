#pragma once

#include <cstddef>     // for size_t
#include <cstdint>     // for uint32_t
#include <deque>       // for deque
#include <functional>  // for function
#include <memory>      // for unique_ptr
#include <string>      // for string
#include <string_view> // for string_view
#include <vector>      // for vector

struct ScriptMessage;

class SpelunkyConsole
{
  public:
    SpelunkyConsole(class SoundManager* sound_manager);
    ~SpelunkyConsole();

    void loop_messages(std::function<void(const ScriptMessage&)> message_fun) const;
    std::deque<ScriptMessage> consume_messages();
    std::vector<std::string> consume_requires();

    bool is_enabled();
    bool is_toggled();

    void draw(struct ImDrawList* dl);
    void render_options();

    void toggle();

    std::string execute(std::string code, bool raw = false);

    bool has_new_history() const;
    void set_max_history_size(size_t max_history);
    void save_history(std::string_view path);
    void load_history(std::string_view path);
    void push_history(std::string history_item, std::vector<ScriptMessage> result_item);

    std::string dump_api();

    class LuaConsole* get_impl()
    {
        return m_Impl.get();
    }

    void set_selected_uid(uint32_t uid);
    unsigned int get_input_lines();
    void set_geometry(float x, float y, float w, float h);
    void set_alt_keys(bool enable);

  private:
    std::unique_ptr<class LuaConsole> m_Impl;
};
