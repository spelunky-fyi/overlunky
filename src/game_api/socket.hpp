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
    using SocketCb = std::optional<std::string>(std::string, std::string);
    using ReadFun = void(std::string, std::string);

    UdpServer(std::string host, in_port_t port);
    ~UdpServer();

    /// Closes the server.
    void close();

    /// Send message to given host and port, returns numbers of bytes send, or -1 on failure
    ssize_t send(std::string message, std::string host, in_port_t port);

    /// Read message from the socket buffer. Reads maximum of 32KiB at the time. If the message is longer, it will be split to portions of 32KiB.
    /// On failure or empty buffer returns -1, on success calls the function with signature `nil(message, source)`
    ssize_t read(std::function<ReadFun> fun);

    /// Returns true if the port was opened successfully and the server hasn't been closed yet.
    bool is_open() const;

    /// Returns a string explaining the last error
    std::string last_error() const;

    std::string get_host() const
    {
        return m_host;
    }
    in_port_t get_port() const
    {
        return m_port;
    }

    // added only for backwards compatibility, do not use or expose
    void start_callback(std::function<SocketCb> cb_);

  private:
    void callback(sockpp::udp_socket sock, std::function<UdpServer::SocketCb> cb);
    std::string m_host;
    in_port_t m_port;
    std::thread m_thread;
    sockpp::udp_socket m_sock;

    std::atomic<bool> m_opened{false};
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
