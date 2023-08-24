#pragma once

#include <boost/asio.hpp>

namespace net = boost::asio;
namespace sys = boost::system;

class TcpClient {
public:
    explicit TcpClient(net::io_context& context);

    void
    connect(std::string_view address, net::ip::port_type port);

    void
    send(const void* requestData, std::size_t requestSize);

    std::size_t
    receive(net::streambuf& buffer);

    void
    shutdown();

private:
    net::ip::tcp::socket _socket;
};
