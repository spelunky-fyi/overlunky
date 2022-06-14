#include "socket_lua.hpp"

#include "script/lua_backend.hpp"

#include <detours.h>
#include <sol/sol.hpp>
#include <winsock.h>

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

using NetFun = int(SOCKET, char*, int, int, sockaddr*, int*);
NetFun* g_sendto_trampoline{nullptr};
NetFun* g_recvfrom_trampoline{nullptr};
int mySendto(SOCKET s, char* buf, int len, int flags, struct sockaddr* to, int* tolen)
{
    auto ret = g_sendto_trampoline(s, buf, len, flags, to, tolen);
    DEBUG("SEND: {}", (ByteStr)buf);
    return ret;
}

int myRecvfrom(SOCKET s, char* buf, int len, int flags, struct sockaddr* from, int* fromlen)
{
    auto ret = g_recvfrom_trampoline(s, buf, len, flags, from, fromlen);
    DEBUG("RECV: {}", (ByteStr)buf);
    return ret;
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

    lua["dump_network"] = []()
    {
        g_sendto_trampoline = (NetFun*)GetProcAddress(GetModuleHandleA("ws2_32.dll"), "sendto");
        g_recvfrom_trampoline = (NetFun*)GetProcAddress(GetModuleHandleA("ws2_32.dll"), "recvfrom");
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach((void**)&g_sendto_trampoline, mySendto);
        DetourAttach((void**)&g_recvfrom_trampoline, myRecvfrom);
        const LONG error = DetourTransactionCommit();
        if (error != NO_ERROR)
        {
            DEBUG("Failed hooking network: {}\n", error);
        }
    };
}
}; // namespace NSocket
