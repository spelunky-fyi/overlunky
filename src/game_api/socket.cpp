#include "socket.hpp"

#include <Windows.h>             // for GetModuleHandleA, GetProcAddress
#include <algorithm>             // for max
#include <detours.h>             // for DetourAttach, DetourTransactionBegin
#include <exception>             // for exception
#include <new>                   // for operator new
#include <sockpp/inet_address.h> // for inet_address
#include <sockpp/udp_socket.h>   // for udp_socket
#include <thread>                // for thread
#include <tuple>                 // for get
#include <type_traits>           // for move
#include <utility>               // for max, min
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

void udp_data(sockpp::udp_socket socket, UdpServer* server)
{
    ssize_t n;
    char buf[500];
    sockpp::inet_address src;
    while (server->kill_thr.test(std::memory_order_acquire) && (n = socket.recv_from(buf, sizeof(buf), &src)) > 0)
    {
        std::optional<std::string> ret = server->cb(std::string(buf, n));
        if (ret)
        {
            socket.send_to(ret.value(), src);
        }
    }
}

UdpServer::UdpServer(std::string host_, in_port_t port_, std::function<SocketCb> cb_)
    : host(host_), port(port_), cb(cb_)
{
    sock.bind(sockpp::inet_address(host, port));
    kill_thr.test_and_set();
    thr = std::thread(udp_data, std::move(sock), this);
}
void UdpServer::clear() // TODO: fix and expose: this and the destructor causes deadlock
{
    kill_thr.clear(std::memory_order_release);
    thr.join();
}
UdpServer::~UdpServer()
{
    if (thr.joinable())
    {
        kill_thr.clear(std::memory_order_release);
        thr.join();
    }
}

bool http_get(const char* sURL, std::string& out, std::string& err)
{
    const int BUFFER_SIZE = 32768;
    DWORD iFlags;
    const char* sAgent = "curl";
    const char* sHeader = NULL;
    HINTERNET hInternet;
    HINTERNET hConnect;
    char acBuffer[BUFFER_SIZE];
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
