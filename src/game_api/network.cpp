#include "network.hpp"
#include <WinSock2.h>
#include <chrono>
#include <ctime>
#include <detours.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

static std::ofstream g_network_log_fs;
std::mutex g_network_log_mutex;

static int32_t g_port_filter = -1;
static bool g_dump_sendto = true;
static bool g_dump_recvfrom = true;

using SendToFun = int(SOCKET s, const char* buf, int len, int flags, const sockaddr* to, int tolen);
using RecvFromFun = int(SOCKET s, char* buf, int len, int flags, sockaddr* from, int* fromlen);
using InetNtoPFun = PCSTR(int family, const void* pAddr, PSTR pStringBuf, size_t StringBufSize);
using GetNameInfoFun = int(const SOCKADDR* pSockaddr, int SockaddrLength, PCHAR pNodeBuffer, DWORD NodeBufferSize, PCHAR pServiceBuffer, DWORD ServiceBufferSize, INT Flags);

SendToFun* g_sendto_trampoline{nullptr};
RecvFromFun* g_recvfrom_trampoline{nullptr};
InetNtoPFun* g_inet_ntop{nullptr};
GetNameInfoFun* g_getnameinfo{nullptr};

std::string get_hostname(const SOCKADDR* sock_addr, int sock_addr_len, const std::string& ipaddress)
{
    static std::unordered_map<std::string, std::string> cache;
    if (cache.count(ipaddress) > 0)
    {
        return cache.at(ipaddress);
    }
    DEBUG("Looking up {}", ipaddress);

    char buffer[1024] = {0};
    auto result = g_getnameinfo(sock_addr, sock_addr_len, buffer, 1024, nullptr, 0, NI_NAMEREQD);
    if (result != 0 || buffer[0] == 0)
    {
        cache[ipaddress] = "";
        return "";
    }
    auto s = std::string(buffer);
    cache[ipaddress] = s;
    return s;
}

std::string sockaddr_to_ipaddress(const sockaddr_in* s)
{
    char buffer[17] = {0};
    g_inet_ntop(s->sin_family, &(s->sin_addr), buffer, 17);
    if (buffer[0] == 0)
    {
        return "<no ip address>";
    }
    return std::string(buffer);
}

void dump_packet(const char* buf, int len)
{
    size_t column_count = 0;

    auto dump_ascii = [&](size_t index)
    {
        if (column_count < 16)
        {
            for (size_t x = 0; x < 16 - column_count; ++x)
            {
                g_network_log_fs << "   ";
            }
            if (column_count > 8)
            {
                g_network_log_fs << " ";
            }
        }
        g_network_log_fs << "\t";
        for (size_t x = index - column_count; x < index; ++x)
        {
            auto ch = static_cast<uint8_t>(buf[x]);
            if (ch >= 0x20 && ch <= 0x7E)
            {
                g_network_log_fs << buf[x];
            }
            else
            {
                g_network_log_fs << ".";
            }
        }
    };

    g_network_log_fs << "\t" << std::hex << std::setfill('0');
    int x;
    for (x = 0; x < len; ++x)
    {
        if (x > 0 && x % 8 == 0)
        {
            g_network_log_fs << " ";
        }
        if (x > 0 && x % 16 == 0)
        {
            dump_ascii(x);
            g_network_log_fs << "\n\t";
            column_count = 0;
        }
        g_network_log_fs << std::setw(2) << static_cast<uint32_t>(static_cast<uint8_t>(buf[x])) << " ";
        column_count++;
    }
    dump_ascii(x);
    g_network_log_fs << "\n\n";
}

int custom_send_to(SOCKET s, const char* buf, int len, int flags, const sockaddr* to, int tolen)
{
    auto to_in = reinterpret_cast<const sockaddr_in*>(to);
    if (g_dump_sendto && (g_port_filter == -1 || g_port_filter == to_in->sin_port))
    {
        auto now = std::chrono::system_clock::now();
        auto ipaddress = sockaddr_to_ipaddress(to_in);

        std::lock_guard<std::mutex> guard(g_network_log_mutex);
        g_network_log_fs << now << ": " << std::dec << len << "b ===> " << ipaddress << ":" << to_in->sin_port << " " << get_hostname(to, tolen, ipaddress) << "\n";
        dump_packet(buf, len);
    }
    return g_sendto_trampoline(s, buf, len, flags, to, tolen);
}

int custom_recv_from(SOCKET s, char* buf, int len, int flags, sockaddr* from, int* fromlen)
{
    auto retval = g_recvfrom_trampoline(s, buf, len, flags, from, fromlen);

    auto from_in = reinterpret_cast<const sockaddr_in*>(from);
    if (g_dump_recvfrom && (g_port_filter == -1 || g_port_filter == from_in->sin_port))
    {
        auto now = std::chrono::system_clock::now();
        auto ipaddress = sockaddr_to_ipaddress(from_in);

        std::lock_guard<std::mutex> guard(g_network_log_mutex);
        g_network_log_fs << now << ": " << std::dec << len << "b <=== " << ipaddress << ":" << from_in->sin_port << " " << get_hostname(from, *fromlen, ipaddress) << "\n";
        dump_packet(buf, len);
    }

    return retval;
}

void hook_network(int32_t port, bool dump_send, bool dump_receive)
{
    g_port_filter = port;
    g_dump_sendto = dump_send;
    g_dump_recvfrom = dump_receive;

    static bool function_hooked = false;
    if (!function_hooked)
    {
        g_network_log_fs = std::ofstream("spelunky_network.log", std::ofstream::app);
        HMODULE hMod = GetModuleHandle("ws2_32.dll");
        g_sendto_trampoline = (SendToFun*)GetProcAddress(hMod, "sendto");
        g_recvfrom_trampoline = (RecvFromFun*)GetProcAddress(hMod, "recvfrom");
        g_inet_ntop = (InetNtoPFun*)GetProcAddress(hMod, "inet_ntop");
        g_getnameinfo = (GetNameInfoFun*)GetProcAddress(hMod, "getnameinfo");

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());

        DetourAttach((void**)&g_sendto_trampoline, &custom_send_to);
        DetourAttach((void**)&g_recvfrom_trampoline, &custom_recv_from);

        const LONG error = DetourTransactionCommit();
        if (error != NO_ERROR)
        {
            DEBUG("Failed hooking sendto: {}\n", error);
        }

        function_hooked = true;
    }
}
