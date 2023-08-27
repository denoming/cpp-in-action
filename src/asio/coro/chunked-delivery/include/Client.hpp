#pragma once

#include "Http.hpp"

#include <string>
#include <memory>

class Client : public std::enable_shared_from_this<Client> {
public:
    explicit Client(io::any_io_executor executor,
                    std::string host,
                    std::string port,
                    std::string data,
                    std::size_t step = 5);

    void
    send();

private:
    io::awaitable<void>
    doSend();

private:
    std::string _host;
    std::string _port;
    std::string _data;
    std::size_t _step;
    io::any_io_executor _executor;
};
