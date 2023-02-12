#pragma once

#include "Common.hpp"

#include <memory>

class TcpSession : public std::enable_shared_from_this<TcpSession> {
public:
    using Ptr = std::shared_ptr<TcpSession>;

    explicit TcpSession(tcp::socket&& socket);

    void
    start();

private:
    void
    onWriteDone(const sys::error_code& error, std::size_t bytesWritten);

private:
    tcp::socket _socket;
    std::string _message;
};
