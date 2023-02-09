#include "ChatSession.hpp"

#include <sstream>
#include <iostream>

ChatSession::ChatSession(asio::io_context& context, tcp::socket&& socket)
    : _socket{std::move(socket)}
    , _strandR{context}
    , _strandW{context}
{
}

void
ChatSession::start(MessageHandler onMessage, ErrorHandler onError)
{
    _onMessage = std::move(onMessage);
    _onError = std::move(onError);
    read();
}

void
ChatSession::post(std::string message)
{
    bool idle = _outgoing.empty();
    _outgoing.push(std::move(message));
    if (idle) {
        write();
    }
}

void
ChatSession::read()
{
    asio::async_read_until(
        _socket,
        _buffer,
        "\n",
        asio::bind_executor(_strandR, std::bind_front(&ChatSession::onRead, shared_from_this())));
}

void
ChatSession::onRead(sys::error_code errorCode, std::size_t bytes)
{
    if (errorCode) {
        std::cerr << "onRead: " << errorCode.message() << std::endl;
        _socket.close();
        _onError();
    } else {
        std::stringstream ss;
        ss << _socket.remote_endpoint(errorCode) << ": " << std::istream{&_buffer}.rdbuf();
        _buffer.consume(bytes);
        _onMessage(ss.str());
        read();
    }
}

void
ChatSession::write()
{
    asio::async_write(
        _socket,
        asio::buffer(_outgoing.front()),
        asio::bind_executor(_strandW, std::bind_front(&ChatSession::onWrite, shared_from_this())));
}

void
ChatSession::onWrite(sys::error_code errorCode, std::size_t /*bytes*/)
{
    if (errorCode) {
        std::cerr << "onWrite: " << errorCode.message() << std::endl;
        _socket.close();
        _onError();
    } else {
        _outgoing.pop();
        if (!_outgoing.empty()) {
            write();
        }
    }
}