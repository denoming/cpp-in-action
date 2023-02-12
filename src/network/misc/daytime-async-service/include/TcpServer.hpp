#pragma once

#include "Common.hpp"

#include <optional>

class TcpServer {
public:
    explicit TcpServer(asio::io_context& context, std::uint16_t port = 13);

private:
    void
    accept();

private:
    asio::io_context& _context;
    tcp::acceptor _acceptor;
    std::optional<tcp::socket> _socket;
};