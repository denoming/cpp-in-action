#pragma once

#include "Common.hpp"
#include "CircularBuffer.hpp"

#include <memory>

class TcpEchoSession : public std::enable_shared_from_this<TcpEchoSession> {
public:
    explicit TcpEchoSession(tcp::socket&& socket);

    void
    start();

private:
    void
    doClose();

    void
    doRead();

    void
    onRead(const sys::error_code& error, std::size_t bytesTransferred);

    void
    doWrite();

    void
    onWrite(const sys::error_code& error, std::size_t bytesTransferred);

private:
    bool _writing{false};
    CircularBuffer<65536> _buffer;
    tcp::socket _socket;
};