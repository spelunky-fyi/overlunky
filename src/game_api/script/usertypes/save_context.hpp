#pragma once

#include <string>
#include <string_view>

#include <sol/forward.hpp>

using MakeSavePathCallback = std::string (*)(std::string_view script_path, std::string_view script_name);
void register_make_save_path(MakeSavePathCallback make_save_path_callback);

class SaveContext
{
  public:
    SaveContext(std::string_view script_path, std::string_view script_name);

    bool Save(std::string data) const;

  private:
    std::string_view script_path;
    std::string_view script_name;
};
class LoadContext
{
  public:
    LoadContext(std::string_view script_path, std::string_view script_name);

    std::string Load() const;

  private:
    std::string_view script_path;
    std::string_view script_name;
};

namespace NSaveContext
{
void register_usertypes(sol::state& lua);
}
