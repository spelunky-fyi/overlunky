#include "socket_lua.hpp"
#include "socket.hpp"

#include <memory>      // for unique_ptr
#include <sol/sol.hpp> // for global_table, proxy_key_t, function
#include <sys/types.h> // for ssize_t

#include "logger.h"               // for DEBUG, ByteStr
#include "script/lua_backend.hpp" // for LuaBackend
#include "script/safe_cb.hpp"     // for make_safe_cb

class UdpServer_lua
{
    std::unique_ptr<UdpServer> m_ptr;

  public:
    UdpServer_lua(std::unique_ptr<UdpServer> ptr)
        : m_ptr(std::move(ptr)){};
};

namespace NSocket
{
void register_usertypes(sol::state& lua)
{
    auto udpserver_type = lua.new_usertype<UdpServer>(
        "UdpServer",
        sol::constructors<UdpServer(std::string, in_port_t)>(),
        "close",
        &UdpServer::close,
        "is_open",
        &UdpServer::is_open,
        "last_error",
        &UdpServer::last_error,
        "host",
        &UdpServer::get_host,
        "port",
        &UdpServer::get_port,
        "send",
        &UdpServer::send);

    /// Read message from the socket buffer. Reads maximum of 32KiB at the time. If the message is longer, it will be split to portions of 32KiB. On failure or empty buffer returns -1, on success calls the function with signature `nil(message, source)`
    udpserver_type["read"] = [](UdpServer& srv, sol::function fun) -> ssize_t
    { return srv.read(std::move(fun)); };

    /// Deprecated
    /// Start an UDP server on specified address and run callback when data arrives. Set port to 0 to use a random ephemeral port. Return a string from the callback to reply.
    /// The server will be closed lazily by garbage collection once the handle is released, or immediately by calling close(). Requires unsafe mode.
    /// <br/>The callback signature is optional<string> on_message(string msg, string src)
    lua["udp_listen"] = [](std::string host, in_port_t port, sol::function cb) //-> sol::any
    {
        auto server = std::unique_ptr<UdpServer>{new UdpServer(std::move(host), port)};
        server->start_callback(make_safe_cb<UdpServer::SocketCb>(std::move(cb)));
        return UdpServer_lua(std::move(server));
    };

    /// Send data to specified UDP address. Requires unsafe mode.
    lua["udp_send"] = [](std::string host, in_port_t port, std::string msg)
    {
        sockpp::udp_socket sock;
        sockpp::inet_address addr(host, port);
        sock.send_to(msg, addr);
    };

    /// Hook the sendto and recvfrom functions and start dumping network data to terminal
    lua["dump_network"] = dump_network;

    /// Send a synchronous HTTP GET request and return response as a string or nil on an error
    lua["http_get"] = [&lua](std::string url) -> sol::optional<std::string>
    {
        std::string out;
        std::string err;
        if (http_get(url.c_str(), out, err))
        {
            return out;
        }
        else
        {
            luaL_error(lua, err.c_str());
        }
        return std::nullopt;
    };

    /// Send an asynchronous HTTP GET request and run the callback when done. If there is an error, response will be nil and vice versa.
    /// The callback signature is nil on_data(string response, string error)
    lua["http_get_async"] = [](std::string url, sol::function on_data)
    {
        new HttpRequest(std::move(url), make_safe_cb<HttpRequest::HttpCb>(std::move(on_data)));
    };
}
}; // namespace NSocket
