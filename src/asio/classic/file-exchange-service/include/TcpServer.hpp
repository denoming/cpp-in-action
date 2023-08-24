#pragma once

#include "Http.hpp"

#include <string_view>

class TcpServer {
public:
    explicit TcpServer(net::io_context& context);

    [[noreturn]] void
    listen(net::ip::port_type port);

    [[noreturn]] void
    listen(std::string_view port);

private:
    static void
    handleSession(TcpServer* server, tcp::socket&& socket);

private:
    net::io_context& _context;
};