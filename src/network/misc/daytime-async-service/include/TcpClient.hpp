#pragma once

#include <boost/asio.hpp>

namespace net = boost::asio;
namespace sys = boost::system;

class TcpClient {
public:
    explicit TcpClient(net::io_context& context);

    void
    connect(std::string_view address, std::string_view port);

    [[nodiscard]] std::string
    get();

private:
    net::io_context _context;
    net::ip::tcp::socket _socket;
};