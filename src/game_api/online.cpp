#include "online.hpp"
#include "memory.hpp"
#include "search.hpp"

Online* get_online()
{
    static Online* o = *(Online**)get_address("online");
    return o;
}

std::string OnlineLobby::get_code()
{
    return fmt::format("{:X}", code);
}
