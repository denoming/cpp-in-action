#include "TcpEchoSession.hpp"

#include "CircularBufferView.hpp"

TcpEchoSession::TcpEchoSession(tcp::socket&& socket)
    : _socket{std::move(socket)}
{
}

void
TcpEchoSession::start()
{
    // To start an echo session we should start to receive incoming data
    doRead();
}

void
TcpEchoSession::doClose()
{
    sys::error_code error;
    _socket.close(error);
}

void
TcpEchoSession::doRead()
{
    // Schedule asynchronous receiving of a data
    asio::async_read(_socket,
                     makeView(_buffer),
                     asio::transfer_at_least(1),
                     std::bind_front(&TcpEchoSession::onRead, shared_from_this()));
}

void
TcpEchoSession::onRead(const sys::error_code& error, std::size_t bytesTransferred)
{
    if (error) {
        // Close if an error has occurred
        doClose();
    } else {
        // Write data only if we aren't doing it already
        if (!_writing) {
            doWrite();
        }
        // Read next potion of data
        doRead();
    }
}

void
TcpEchoSession::doWrite()
{
    _writing = true;

    // Schedule asynchronous sending of the data
    asio::async_write(
        _socket, makeView(_buffer), std::bind_front(&TcpEchoSession::onWrite, shared_from_this()));
}

void
TcpEchoSession::onWrite(const sys::error_code& error, std::size_t bytesTransferred)
{
    _writing = false;

    if (error) {
        doClose();
    } else {
        // Check if there is something to send it back to the client
        if (!_buffer.empty()) {
            doWrite();
        }
    }
}
