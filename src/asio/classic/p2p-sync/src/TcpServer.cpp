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

#include "TcpServer.hpp"

SyncServer::SyncServer(net::io_context& context)
    : _context{context}
    , _acceptor{context}
    , _socket{context}
{
}

void
SyncServer::listen(net::ip::port_type port)
{
    net::ip::tcp::endpoint endpoint{net::ip::tcp::v4(), port};
    _acceptor.open(endpoint.protocol());
    _acceptor.bind(endpoint);
    _acceptor.listen();
    _socket = _acceptor.accept();
}

void
SyncServer::send(const void* data, std::size_t size)
{
    net::write(_socket, net::buffer(data, size));
}

std::size_t
SyncServer::receive(net::streambuf& buffer)
{
    sys::error_code ec;
    const auto bytesRead = net::read(_socket, buffer, ec);
    if (ec != net::error::eof) {
        throw sys::system_error{ec};
    }
    return bytesRead;
}

void
SyncServer::finalize()
{
    _socket.shutdown(net::socket_base::shutdown_send);
    _socket.close();
}
