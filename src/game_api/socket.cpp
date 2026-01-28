#include "socket.hpp"

#include <Windows.h>             // for GetModuleHandleA, GetProcAddress
#include <chrono>                // for chrono
#include <detours.h>             // for DetourAttach, DetourTransactionBegin
#include <new>                   // for operator new
#include <sockpp/inet_address.h> // for inet_address
#include <sockpp/udp_socket.h>   // for udp_socket
#include <thread>                // for thread
#include <tuple>                 // for get
#include <type_traits>           // for move
#include <wininet.h>             // for InternetCloseHandle, InternetOpenA, InternetG...
#include <winsock2.h>            // for sockaddr_in, SOCKET
#include <ws2tcpip.h>            // for inet_ntop

#pragma comment(lib, "wininet.lib")

using NetFun = int(SOCKET, char*, int, int, sockaddr_in*, int*);
NetFun* g_sendto_trampoline{nullptr};
NetFun* g_recvfrom_trampoline{nullptr};
int mySendto(SOCKET s, char* buf, int len, int flags, sockaddr_in* addr, int* tolen)
{
    auto ret = g_sendto_trampoline(s, buf, len, flags, addr, tolen);
    char ip[16] = "";
    inet_ntop(addr->sin_family, &addr->sin_addr, ip, sizeof(ip));
    DEBUG("SEND: {}:{} | {}", ip, addr->sin_port, ByteStr{buf});
    return ret;
}

int myRecvfrom(SOCKET s, char* buf, int len, int flags, sockaddr_in* addr, int* fromlen)
{
    auto ret = g_recvfrom_trampoline(s, buf, len, flags, addr, fromlen);
    char ip[16] = "";
    inet_ntop(addr->sin_family, &addr->sin_addr, ip, sizeof(ip));
    DEBUG("RECV: {}:{} | {}", ip, addr->sin_port, ByteStr{buf});
    return ret;
}

void dump_network()
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
}

void UdpServer::callback(sockpp::udp_socket sock, std::function<UdpServer::SocketCb> cb)
{
    static thread_local char buf[32768];
    while (m_opened.load(std::memory_order::relaxed) && m_sock.is_open())
    {
        sockpp::inet_address src;
        ssize_t n;
        while ((n = sock.recv_from(buf, sizeof(buf), &src)) > 0)
        {
            std::optional<std::string> ret = cb(std::string(buf, n), src.to_string());
            if (ret.has_value())
                sock.send_to(ret.value(), src);
        }
        std::this_thread::sleep_for(std::chrono::microseconds(1));
    }
    m_opened = false;
    sock.shutdown();
}
UdpServer::UdpServer(std::string host, in_port_t port)
    : m_address(host, port)
{
    if (m_sock.bind(m_address))
    {
        m_address = m_sock.address(); // update port etc.
        m_opened = true;
        m_sock.set_non_blocking();
    }
}
UdpServer::UdpServer(std::string address)
    : m_address(address_from_string(address))
{
    if (m_sock.bind(m_address))
    {
        m_address = m_sock.address(); // update port etc.
        m_opened = true;
        m_sock.set_non_blocking();
    }
}
void UdpServer::close()
{
    m_opened = false;
    if (!m_thread.joinable())
        m_sock.close();
}
ssize_t UdpServer::send(std::string message, std::string host, in_port_t port)
{
    if (!is_open())
        return -1;

    return m_sock.send_to(message, sockpp::inet_address(host, port));
}
ssize_t UdpServer::send(std::string message, std::string address)
{
    if (!is_open())
        return -1;

    return m_sock.send_to(message, address_from_string(std::move(address)));
}
sockpp::inet_address UdpServer::address_from_string(std::string address)
{
    auto colon = address.rfind(':');
    sockpp::inet_address final_address;
    if (colon == std::string::npos)
    {
        final_address.create(address, 0);
    }
    else
    {
        std::string_view port_str(address);
        port_str = port_str.substr(colon + 1);
        in_port_t port = 0;
        std::from_chars(port_str.data(), port_str.data() + port_str.size(), port);
        address.resize(colon);
        final_address.create(address, port);
    }
    return final_address;
}
ssize_t UdpServer::read(std::function<ReadFun> fun)
{
    if (!is_open())
        return -1;

    static char buf[32768];
    sockpp::inet_address src;
    auto ret = m_sock.recv_from(buf, sizeof(buf), &src);

    if (ret > -1)
        fun(std::string(buf, static_cast<size_t>(ret)), src.to_string());

    return ret;
}
bool UdpServer::is_open() const
{
    return m_opened.load(std::memory_order::memory_order_relaxed) && m_sock.is_open() && m_sock.last_error() > -1;
}
void UdpServer::start_callback(std::function<SocketCb> cb)
{
    if (!m_thread.joinable())
    {
        auto sock_copy = m_sock.clone();
        m_thread = std::thread(&UdpServer::callback, this, std::move(sock_copy), std::move(cb));
    }
}
std::string UdpServer::address() const
{
    // using variable instead of getting address from socket each time since that overrides the last error flag
    return m_address.to_string();
}
std::string UdpServer::last_error_str() const
{
    auto err = m_sock.last_error_str();
    err.resize(err.size() - 2);
    return err;
}
int UdpServer::last_error() const
{
    return m_sock.last_error();
}
UdpServer::~UdpServer()
{
    m_opened = false;
    if (m_thread.joinable())
        m_thread.join();

    m_sock.close();
}

bool http_get(const char* sURL, std::string& out, std::string& err)
{
    const int BUFFER_SIZE = 32768;
    DWORD iFlags;
    const char* sAgent = "curl";
    const char* sHeader = NULL;
    HINTERNET hInternet;
    HINTERNET hConnect;
    static thread_local char acBuffer[BUFFER_SIZE];
    DWORD iReadBytes;
    DWORD iBytesToRead = 0;
    DWORD iReadBytesOfRq = 4;

    // Get connection state
    InternetGetConnectedState(&iFlags, 0);
    if (iFlags & INTERNET_CONNECTION_OFFLINE)
    {
        err = "Can't connect to the internet";
        return false;
    }

    // Open internet session
    if (!(iFlags & INTERNET_CONNECTION_PROXY))
    {
        hInternet = InternetOpenA(sAgent, INTERNET_OPEN_TYPE_PRECONFIG_WITH_NO_AUTOPROXY, NULL, NULL, 0);
    }
    else
    {
        hInternet = InternetOpenA(sAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    }
    if (hInternet)
    {
        if (sHeader == NULL)
        {
            sHeader = "Accept: */*\r\n\r\n";
        }

        hConnect = InternetOpenUrlA(hInternet, sURL, sHeader, lstrlenA(sHeader), INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_RELOAD, 0);
        if (!hConnect)
        {
            InternetCloseHandle(hInternet);
            err = "Can't connect to the url";
            return false;
        }

        // Get content size
        if (!HttpQueryInfo(hConnect, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, (LPVOID)&iBytesToRead, &iReadBytesOfRq, NULL))
        {
            iBytesToRead = 0;
        }

        do
        {
            if (!InternetReadFile(hConnect, acBuffer, BUFFER_SIZE, &iReadBytes))
            {
                InternetCloseHandle(hInternet);
                err = "GET request failed";
                return false;
            }
            if (iReadBytes > 0)
            {
                out += std::string(acBuffer, iReadBytes);
            }
            if (iReadBytes <= 0)
            {
                break;
            }
        } while (TRUE);
        InternetCloseHandle(hInternet);
    }
    else
    {
        err = "Can't connect to the internet";
        return false;
    }

    return true;
}

void http_get_async(HttpRequest* req)
{
    if (http_get(req->url.c_str(), req->response, req->error))
    {
        req->cb(req->response, std::nullopt);
    }
    else
    {
        req->cb(std::nullopt, req->error);
    }
    delete req;
}

HttpRequest::HttpRequest(std::string url_, std::function<HttpCb> cb_)
    : url(url_), cb(cb_)
{
    std::thread thr(http_get_async, this);
    thr.detach();
}

ssize_t udp_send(std::string host, in_port_t port, std::string msg)
{
    sockpp::udp_socket sock;
    sockpp::inet_address addr(host, port);
    return sock.send_to(msg, addr);
}
ssize_t udp_send(std::string address, std::string msg)
{
    sockpp::udp_socket sock;
    sockpp::inet_address addr(UdpServer::address_from_string(address));
    return sock.send_to(msg, addr);
}
