// Copyright 2025 Denys Asauliak
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
    _acceptor.async_accept(_context, [this](sys::error_code errorCode, tcp::socket socket) {
        if (errorCode) {
            std::cerr << "doAccept: " << errorCode.message() << std::endl;
            return;
        }

        auto client = std::make_shared<ChatSession>(_context, std::move(socket));
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
