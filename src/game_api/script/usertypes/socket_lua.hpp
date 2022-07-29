#pragma once

#include <sockpp/platform.h> // for in_port_t
#include <sol/sol.hpp>       // for function, basic_protected_function
#include <string>            // for string

#include "sockpp/udp_socket.h" // for udp_socket

class UdpServer
{
  public:
    UdpServer(std::string host, in_port_t port, sol::function cb);
    std::string host;
    in_port_t port;
    sol::function cb;
    sockpp::udp_socket sock;
};

namespace NSocket
{
void register_usertypes(sol::state& lua);
};
