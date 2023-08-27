#pragma once

#include "Http.hpp"

#include <memory>

class Server : public std::enable_shared_from_this<Server> {
public:
    explicit Server(io::any_io_executor executor);

    void
    listen(io::ip::port_type port);

    void
    listen(tcp::endpoint endpoint);

private:
    io::awaitable<void>
    listener(tcp::endpoint endpoint);

private:
    io::any_io_executor _executor;
};