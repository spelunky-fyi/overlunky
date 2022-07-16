#pragma once

#include <chrono>  // for system_clock, time_point
#include <cstddef> // for size_t
#include <cstdint> // for uint16_t
#include <deque>   // for deque
#include <imgui.h> // for ImVec2, ImDrawList (ptr only), ImVec4
#include <memory>  // for unique_ptr
#include <string>  // for string
#include <vector>  // for vector

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

    std::deque<ScriptMessage>& get_messages();
    std::vector<std::string> consume_requires();

    const std::string& get_id() const;
    const std::string& get_name() const;
    const std::string& get_description() const;
    const std::string& get_author() const;
    const std::string& get_file() const;
    const std::string& get_filename() const;
    const std::string& get_path() const;
    const std::string& get_version() const;
    bool get_unsafe() const;
    bool get_online_safe() const;

#ifdef SPEL2_EDITABLE_SCRIPTS
    std::string& get_code() const;
    std::size_t get_code_size() const;
#endif

    void update_code(std::string code);

    std::string& get_result(); // Thanks for non-const imgui

    bool is_enabled() const;
    void set_enabled(bool enabled);

    bool is_changed() const;
    void set_changed(bool changed);

    bool run();
    void draw(ImDrawList* dl);
    void render_options();

  private:
    std::unique_ptr<ScriptImpl> m_Impl;
};
