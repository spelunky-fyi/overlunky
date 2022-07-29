#include "online.hpp"

#include <fmt/format.h> // for check_format_string, format, vformat

#include "search.hpp" // for get_address

Online* get_online()
{
    static Online* o = *(Online**)get_address("online");
    return o;
}

std::string OnlineLobby::get_code()
{
    return fmt::format("{:X}", code);
}
