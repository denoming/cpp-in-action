#pragma once

#include "Common.hpp"
#include "ChatSession.hpp"

#include <optional>
#include <string>
#include <unordered_set>

class ChatServer {
public:
    ChatServer(asio::io_context& context, std::uint16_t port);

    void
    listen();

    void
    post(std::string message);

private:
    void
    doListen();

    void
    doAccept();

private:
    asio::io_context& _context;
    tcp::endpoint _endpoint;
    tcp::acceptor _acceptor;
    std::unordered_set<ChatSession::Ptr> _clients;
};