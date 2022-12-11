#pragma once

#include <atomic>            // for atomic_flag
#include <functional>        // for function
#include <sockpp/platform.h> // for in_port_t
#include <sol/sol.hpp>       // for state, optional
#include <string>            // for string
#include <thread>            // for thread

#include "sockpp/udp_socket.h" // for udp_socket

class UdpServer
{
  public:
    using SocketCb = std::optional<std::string>(std::string);

    UdpServer(std::string host, in_port_t port, std::function<SocketCb> cb);
    ~UdpServer();

    std::string host;
    in_port_t port;
    std::function<SocketCb> cb;
    std::thread thr;
    std::atomic_flag kill_thr;
    sockpp::udp_socket sock;
};

namespace NSocket
{
void register_usertypes(sol::state& lua);
};
