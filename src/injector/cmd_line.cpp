#include "cmd_line.h"

#include <algorithm>   // for count_if
#include <string>      // for char_traits
#include <type_traits> // for move
#include <utility>     // for min, find_if

CmdLineParser::CmdLineParser(int argc, char** argv)
    : m_CmdLine(argv, argv + argc){};

std::vector<std::string_view> CmdLineParser::Get(std::string_view arg, has_args_tag) const
{
    auto it = std::find_if(
        m_CmdLine.begin(),
        m_CmdLine.end(),
        [arg](std::string_view cmd_line_arg)
        { return cmd_line_arg.size() > 2 && cmd_line_arg.at(0) == '-' && cmd_line_arg.at(1) == '-' && cmd_line_arg.substr(2) == arg; });

    std::vector<std::string_view> params;
    if (it != m_CmdLine.end())
    {
        ++it;
        for (; it != m_CmdLine.end(); ++it)
        {
            if (it->size() > 2 && it->at(0) == '-' && it->at(1) == '-')
            {
                // Next arg
                break;
            }
            params.push_back(*it);
        }
    }
    return params;
}

bool CmdLineParser::Get(std::string_view arg) const
{
    return std::count_if(
        m_CmdLine.begin(),
        m_CmdLine.end(),
        [arg](std::string_view cmd_line_arg)
        { return cmd_line_arg.size() > 2 && cmd_line_arg[0] == '-' && cmd_line_arg[1] == '-' && cmd_line_arg.substr(2) == arg; });
}
