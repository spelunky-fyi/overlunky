#pragma once

#include <chrono>
#include <deque>
#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>

#include <imgui.h>

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
    size_t orig_input;
    size_t orig_ai;
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
    SpelunkyScript(std::string script, std::string file, class SoundManager* sound_manager, bool enable = true);
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

#ifdef SPEL2_EDITABLE_SCRIPTS
    char* get_code() const;
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

    bool pre_level_gen_spawn(std::string_view tile_code, float x, float y, int layer);
    void post_level_gen_spawn(std::string_view tile_code, float x, float y, int layer);

    std::string dump_api();

    static void for_each_script(std::function<bool(SpelunkyScript&)> fun);

  private:
    std::unique_ptr<ScriptImpl> m_Impl;
};
