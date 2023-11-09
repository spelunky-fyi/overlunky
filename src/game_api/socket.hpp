#pragma once

#include <atomic>            // for atomic_flag
#include <functional>        // for function
#include <sockpp/platform.h> // for in_port_t
#include <string>            // for string
#include <thread>            // for thread

#include "sockpp/udp_socket.h" // for udp_socket

class UdpServer
{
  public:
    using SocketCb = std::optional<std::string>(std::string);

    UdpServer(std::string host, in_port_t port, std::function<SocketCb> cb);
    ~UdpServer();
    void clear();

    std::string host;
    in_port_t port;
    std::function<SocketCb> cb;
    std::thread thr;
    std::atomic_flag kill_thr;
    sockpp::udp_socket sock;
};

class HttpRequest
{
  public:
    using HttpCb = void(std::optional<std::string>, std::optional<std::string>);

    HttpRequest(std::string url, std::function<HttpCb> cb);
    std::string url;
    std::function<HttpCb> cb;
    std::string response;
    std::string error;
};

void dump_network();
bool http_get(const char* sURL, std::string& out, std::string& err);
