#pragma once

#include "Common.hpp"

#include <string>
#include <queue>
#include <memory>

class ChatSession : public std::enable_shared_from_this<ChatSession> {
public:
    using Ptr = std::shared_ptr<ChatSession>;

    explicit ChatSession(tcp::socket&& socket);

    void start(MessageHandler onMessage, ErrorHandler onError);

    void post(std::string message);

private:
    void read();

    void onRead(sys::error_code errorCode, std::size_t bytes);

    void write();

    void onWrite(sys::error_code errorCode, std::size_t bytes);

private:
    tcp::socket _socket;
    asio::streambuf _buffer;
    std::queue<std::string> _outgoing;
    MessageHandler _onMessage;
    ErrorHandler _onError;
};