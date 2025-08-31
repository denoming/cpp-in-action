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

#include "TcpAsyncServer.h"

namespace {

std::thread
spawnContextThread(net::io_context& context)
{
    return std::thread{[&]() {
        const auto guard = net::make_work_guard(context);
        context.run();
    }};
}

} // namespace

TcpAsyncServer::TcpAsyncServer(net::io_context& context)
    : _context{context}
{
}

TcpAsyncServer::~TcpAsyncServer()
{
    stop();
}

void
TcpAsyncServer::start(net::ip::port_type port, std::size_t threadsNum)
{
    _acceptor = std::make_unique<TcpAsyncAcceptor>(_context, port);

    assert(threadsNum > 0);
    while (threadsNum--) {
        _threads.push_back(spawnContextThread(_context));
    }

    _acceptor->start();
}

void
TcpAsyncServer::stop()
{
    if (_acceptor) {
        _acceptor->stop();
        _acceptor.reset();
    }

    if (!_context.stopped()) {
        _context.stop();
    }

    for (auto& thread : _threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}