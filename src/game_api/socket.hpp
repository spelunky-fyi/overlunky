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

    /// Start/bind an UDP server on specified address. Set port to 0 to use a random ephemeral port.
    UdpServer(std::string host, in_port_t port);
    /// Host/IP + optional port `ip:port`
    UdpServer(std::string address);
    /// Short for `UdpServer:new("localhost")`
    UdpServer()
        : UdpServer("localhost"){};

    ~UdpServer();

    /// Closes the server.
    void close();

    /// Send message to given host and port from this socket, returns numbers of bytes send or -1 on failure
    ssize_t send(std::string message, std::string host, in_port_t port);

    /// Send to address in format `ip:port`
    ssize_t send(std::string message, std::string address);

    /// Read message from the socket buffer. Reads maximum of 32KiB at the time. If the message is longer, it will be split to portions of 32KiB.
    /// Will set last_error to "A non-blocking socket operation could not be completed immediately", can be ignored, that's normal operation.
    /// On failure or empty buffer returns -1, on success calls the function with signature `nil(message, source_address)`
    ssize_t read(std::function<ReadFun> fun);

    /// Returns true if the port was opened successfully and the server hasn't been closed yet.
    bool is_open() const;

    /// Value of the last raised exception
    int last_error() const;

    /// Returns a string explaining the last error
    std::string last_error_str() const;

    /// Returns string representation of the server address in format "ip:port"
    std::string address() const;

    static sockpp::inet_address address_from_string(std::string address);

    // added only for backwards compatibility, do not use or expose!
    void start_callback(std::function<SocketCb> cb_);

  private:
    void callback(sockpp::udp_socket sock, std::function<UdpServer::SocketCb> cb);

    std::thread m_thread;
    sockpp::udp_socket m_sock;
    sockpp::inet_address m_address;
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
ssize_t udp_send(std::string host, in_port_t port, std::string msg);
ssize_t udp_send(std::string address, std::string msg);
