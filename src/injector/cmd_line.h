#pragma once

#include <charconv>
#include <string_view>
#include <type_traits>
#include <vector>

class CmdLineParser
{
  public:
    CmdLineParser(int argc, char** argv);

    struct has_args_tag
    {
    };
    inline static constexpr has_args_tag has_args{};
    std::vector<std::string_view> Get(std::string_view arg, has_args_tag) const;
    bool Get(std::string_view arg) const;

  private:
    std::vector<std::string_view> m_CmdLine;
};

template <class T>
T GetCmdLineParam(const CmdLineParser& parser, std::string_view arg, const T& default_value);
template <>
inline std::vector<std::string_view>
GetCmdLineParam<std::vector<std::string_view>>(const CmdLineParser& parser, std::string_view arg, const std::vector<std::string_view>& default_value)
{
    const auto ret = parser.Get(arg, CmdLineParser::has_args);
    return ret.empty() ? default_value : ret;
}
template <>
inline std::string_view GetCmdLineParam<std::string_view>(const CmdLineParser& parser, std::string_view arg, const std::string_view& default_value)
{
    const auto ret = parser.Get(arg, CmdLineParser::has_args);
    return ret.empty() ? default_value : ret.front();
}
template <>
inline int GetCmdLineParam<int>(const CmdLineParser& parser, std::string_view arg, const int& default_value)
{
    int ret;
    std::string_view param = GetCmdLineParam<std::string_view>(parser, arg, "none");
    std::from_chars_result char_conv_result = std::from_chars(param.data(), param.data() + param.size(), ret);
    if (char_conv_result.ptr == arg.data())
    {
        return default_value;
    }
    return ret;
}
template <>
inline bool GetCmdLineParam<bool>(const CmdLineParser& parser, std::string_view arg, const bool& default_value)
{
    return default_value || parser.Get(arg);
}
