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

#pragma once

#include "TcpAsyncAcceptor.h"

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>

#include <thread>
#include <memory>

namespace net = boost::asio;
namespace sys = boost::system;

class TcpAsyncServer final : boost::noncopyable {
public:
    explicit TcpAsyncServer(net::io_context& context);

    ~TcpAsyncServer();

    void
    start(net::ip::port_type port,
          std::size_t threadsNum = std::thread::hardware_concurrency());

    void
    stop();

private:
    net::io_context& _context;
    std::vector<std::thread> _threads;
    std::unique_ptr<TcpAsyncAcceptor> _acceptor;
};