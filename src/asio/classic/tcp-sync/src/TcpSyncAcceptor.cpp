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

#include "TcpSyncAcceptor.h"

#include "TcpSyncService.h"

TcpSyncAcceptor::TcpSyncAcceptor(net::io_context& context, net::ip::port_type port)
    : _context{context}
    , _acceptor{context, net::ip::tcp::endpoint{net::ip::address_v4::any(), port}}
{
    _acceptor.listen();
}

void
TcpSyncAcceptor::accept()
{
    net::ip::tcp::socket socket{_context};
    _acceptor.accept(socket);
    TcpSyncService service;
    service.handle(socket);
}