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

#include "TcpEchoServer.hpp"

#include "TcpEchoSession.hpp"

TcpEchoServer::TcpEchoServer(asio::io_context& context, std::uint16_t port)
    : _context{context}
    , _acceptor{context, tcp::endpoint{tcp::v4(), port}}
{
    accept();
}

void
TcpEchoServer::accept()
{
    _socket.emplace(_context);

    _acceptor.async_accept(*_socket, [this](const sys::error_code& error) {
        std::make_shared<TcpEchoSession>(std::move(*_socket))->start();
        accept();
    });
}
