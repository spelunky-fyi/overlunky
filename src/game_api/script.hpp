#pragma once

#include <chrono>     // for system_clock, time_point
#include <cstddef>    // for size_t
#include <cstdint>    // for uint16_t
#include <deque>      // for deque
#include <functional> // for function
#include <imgui.h>    // for ImVec2, ImDrawList (ptr only), ImVec4
#include <memory>     // for unique_ptr
#include <string>     // for string
#include <vector>     // for vector

struct PlayerInputs;
class Ai;

ImVec2 screenify(ImVec2 pos);
float screenify(float dis);
ImVec2 normalize(ImVec2 pos);

struct ScriptMeta
{
    std::string file;
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string id;
    std::string path;
    std::string filename;
    std::string stem;
    bool unsafe;
    bool online_safe;
};

struct ScriptMessage
{
    std::string message;
    std::chrono::time_point<std::chrono::system_clock> time;
    ImVec4 color;
};

struct ScriptInput
{
    uint16_t next;
    uint16_t current;
    PlayerInputs* orig_input;
    Ai* orig_ai;
};

struct ScriptImage
{
    struct ID3D11ShaderResourceView* texture;
    int width;
    int height;
};

class ScriptImpl;

class SpelunkyScript
{
  public:
    SpelunkyScript(std::string script, std::string file, class SoundManager* sound_manager, class SpelunkyConsole* console, bool enable = true);
    ~SpelunkyScript();

    void loop_messages(std::function<void(const ScriptMessage&)> message_fun) const;
    std::deque<ScriptMessage> consume_messages();
    std::vector<std::string> consume_requires();

    std::string get_id() const;
    std::string get_name() const;
    std::string get_description() const;
    std::string get_author() const;
    std::string get_file() const;
    std::string get_filename() const;
    std::string get_path() const;
    std::string get_version() const;
    bool get_unsafe() const;
    bool get_online_safe() const;

    // Use for non-allocating access to meta
    void get_meta(std::function<void(const ScriptMeta& meta)> meta_fun);

#ifdef SPEL2_EDITABLE_SCRIPTS
    std::string& get_code() const;
    std::size_t get_code_size() const;
#endif

    void update_code(std::string code);

    std::string get_result();

    bool is_enabled() const;
    void set_enabled(bool enabled);

    bool is_changed() const;
    void set_changed(bool changed);

    void draw(ImDrawList* dl);
    void render_options();

  private:
    std::unique_ptr<ScriptImpl> m_Impl;
};
