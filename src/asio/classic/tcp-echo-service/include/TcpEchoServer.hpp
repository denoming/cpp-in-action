#pragma once

#include "Common.hpp"

#include <optional>

class TcpEchoServer {
public:
    TcpEchoServer(asio::io_context& context, std::uint16_t port);

private:
    void
    accept();

private:
    asio::io_context& _context;
    tcp::acceptor _acceptor;
    std::optional<tcp::socket> _socket;
};