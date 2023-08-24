#pragma once

#include "Common.hpp"

class TcpClient {
public:
    explicit TcpClient(asio::io_context& context);

    void
    connect(std::string_view address, std::string_view port);

    [[nodiscard]] std::string
    get();

private:
    asio::io_context _context;
    tcp::socket _socket;
};