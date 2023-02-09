#include "ChatServer.hpp"

#include <iostream>

ChatServer::ChatServer(asio::io_context& context, std::uint16_t port)
    : _context{context}
    , _endpoint{tcp::v4(), port}
    , _acceptor{context}
{
}

void
ChatServer::listen()
{
    doListen();
    doAccept();
}

void
ChatServer::doListen()
{
    try {
        /* Start listening step by step */
        _acceptor.open(_endpoint.protocol());
        _acceptor.set_option(tcp::acceptor::reuse_address{true});
        _acceptor.bind(_endpoint);
        _acceptor.listen();
    } catch (const sys::system_error& e) {
        std::cerr << "doListen: " << e.what() << std::endl;
    }
}

void
ChatServer::doAccept()
{
    _socket.emplace(_context);

    _acceptor.async_accept(*_socket, [this](sys::error_code errorCode) {
        if (errorCode) {
            std::cerr << "doAccept: " << errorCode.message() << std::endl;
            return;
        }

        auto client = std::make_shared<ChatSession>(std::move(*_socket));
        client->post("Welcome to chat\n\r");
        post("We have a newcomer\n\r");

        _clients.insert(client);

        client->start(
            [this](std::string message) {
                /* Post message for all clients */
                post(std::move(message));
            },
            [this, weakClient = std::weak_ptr{client}]() {
                if (auto client = weakClient.lock(); client && _clients.erase(client)) {
                    post("We are one less\n\r");
                }
            });

        doAccept();
    });
}

void
ChatServer::post(std::string message)
{
    for (const auto& client : _clients) {
        client->post(message);
    }
}
