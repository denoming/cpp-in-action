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

#include "TcpSyncServer.h"

#include "TcpSyncAcceptor.h"

TcpSyncServer::TcpSyncServer()
    : _stop{false}
{
}

void
TcpSyncServer::start(net::ip::port_type port)
{
    _thread = std::thread{[this, port]() { run(port); }};
}

void
TcpSyncServer::stop()
{
    _stop = true;
    if (_thread.joinable()) {
        _thread.join();
    }
}

void
TcpSyncServer::run(net::ip::port_type port)
{
    TcpSyncAcceptor acceptor{_context, port};
    while (!_stop) {
        acceptor.accept();
    }
}
