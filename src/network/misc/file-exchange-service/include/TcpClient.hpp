#pragma once

#include "Http.hpp"

#include <string_view>

class TcpClient {
public:
    explicit TcpClient(net::io_context& context);

    void
    send(std::string_view host,
         std::string_view port,
         std::string_view message,
         std::size_t step = 5);

private:
    net::io_context& _context;
};