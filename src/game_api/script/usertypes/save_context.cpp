#include "save_context.hpp"

#include <fstream>

#include <sol/sol.hpp>

MakeSavePathCallback g_MakeSavePathCallback{nullptr};
void register_make_save_path(MakeSavePathCallback make_save_path_callback)
{
    g_MakeSavePathCallback = make_save_path_callback;
}

SaveContext::SaveContext(std::string_view _script_path, std::string_view _script_name)
    : script_path{_script_path}, script_name{_script_name}
{
}
bool SaveContext::Save(std::string data) const
{
    const auto save_file_path = g_MakeSavePathCallback(script_path, script_name);
    if (auto data_file = std::ofstream{save_file_path})
    {
        data_file << data << std::flush;
        return data_file.good();
    }
    return false;
}

LoadContext::LoadContext(std::string_view _script_path, std::string_view _script_name)
    : script_path{_script_path}, script_name{_script_name}
{
}
std::string LoadContext::Load() const
{
    std::string data;

    const auto save_file_path = g_MakeSavePathCallback(script_path, script_name);
    if (auto data_file = std::ifstream{save_file_path})
    {
        data_file.seekg(0, std::ios::end);
        data.reserve(data_file.tellg());
        data_file.seekg(0, std::ios::beg);

        data.assign((std::istreambuf_iterator<char>(data_file)), std::istreambuf_iterator<char>());
    }

    return data;
}

namespace NSaveContext
{
void register_usertypes(sol::state& lua)
{
    // Context received in ON.SAVE
    // Used to save a string to some form of save_{}.dat
    // Future calls to this will override the save
    lua.new_usertype<SaveContext>("SaveContext", sol::no_constructor, "save", &SaveContext::Save);
    /* SaveContext
        bool save(string data)
        */

    // Context received in ON.LOAD
    // Used to load from save_{}.dat into a string
    lua.new_usertype<LoadContext>("LoadContext", sol::no_constructor, "load", &LoadContext::Load);
    /* LoadContext
        string load()
        */
}
} // namespace NSaveContext
