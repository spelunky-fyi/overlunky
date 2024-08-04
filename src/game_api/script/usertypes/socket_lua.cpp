#include "socket_lua.hpp"
#include "socket.hpp"

#include <sol/sol.hpp> // for global_table, proxy_key_t, function
#include <sys/types.h> // for ssize_t

#include "logger.h"               // for DEBUG, ByteStr
#include "script/lua_backend.hpp" // for LuaBackend
#include "script/safe_cb.hpp"     // for make_safe_cb

namespace NSocket
{
void register_usertypes(sol::state& lua)
{
    lua.new_usertype<UdpServer>(
        "UdpServer",
        sol::no_constructor);
    /// Start an UDP server on specified address and run callback when data arrives. Return a string from the callback to reply. Requires unsafe mode.
    /// The server will be closed once the handle is released.
    lua["udp_listen"] = [](std::string host, in_port_t port, sol::function cb) -> UdpServer*
    {
        // TODO: change the return to std::unique_ptr after fixing the dead lock with the destructor
        UdpServer* server = new UdpServer(std::move(host), std::move(port), make_safe_cb<UdpServer::SocketCb>(std::move(cb)));
        return server;
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
