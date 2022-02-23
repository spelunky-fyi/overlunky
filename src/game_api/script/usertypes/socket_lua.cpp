#include "socket_lua.hpp"

#include "script/lua_backend.hpp"

#include <sol/sol.hpp>

void udp_data(sockpp::udp_socket socket, UdpServer* server)
{
    ssize_t n;
    char buf[500];
    sockpp::inet_address src;
    while ((n = socket.recv_from(buf, sizeof(buf), &src)) > 0)
    {
        sol::optional<std::string> ret = server->cb(std::string(buf, n));
        if (ret)
            socket.send_to(ret.value(), src);
    }
}

UdpServer::UdpServer(std::string host_, in_port_t port_, sol::function cb_)
    : host(host_), port(port_), cb(cb_)
{
    sock.bind(sockpp::inet_address(host, port));
    std::thread thr(udp_data, std::move(sock), this);
    thr.detach();
}

namespace NSocket
{
void register_usertypes(sol::state& lua)
{
    /// Start an UDP server on specified address and run callback when data arrives. Return a string from the callback to reply. Requires unsafe mode.
    lua["udp_listen"] = [](std::string host, in_port_t port, sol::function cb) -> UdpServer*
    {
        UdpServer* server = new UdpServer(std::move(host), std::move(port), std::move(cb));
        return server;
    };

    /// Send data to specified UDP address. Requires unsafe mode.
    lua["udp_send"] = [](std::string host, in_port_t port, std::string msg)
    {
        sockpp::udp_socket sock;
        sockpp::inet_address addr(host, port);
        sock.send_to(msg, addr);
    };
}
}; // namespace NSocket
