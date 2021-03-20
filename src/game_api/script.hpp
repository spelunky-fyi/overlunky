#pragma once

#include <chrono>
#include <deque>
#include <memory>
#include <string>
#include <vector>
#include <variant>
#include <filesystem>

#include "imgui.h"

ImVec2 screenify(ImVec2 pos);
float screenify(float dis);

struct ScriptOption
{
    std::string desc;
    std::variant<int, float, std::string, bool> value;
    std::variant<int, float> min;
    std::variant<int, float> max;
    std::string opts;
};

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
};

struct ScriptMessage
{
    std::string message;
    std::chrono::time_point<std::chrono::system_clock> time;
    ImVec4 color;
};

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
    void draw(ImDrawList *dl);
    void render_options();

private:
    class ScriptImpl;
    std::unique_ptr<ScriptImpl> m_Impl;
};
