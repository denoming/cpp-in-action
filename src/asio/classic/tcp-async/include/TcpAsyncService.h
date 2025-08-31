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

#include <boost/asio.hpp>

namespace net = boost::asio;
namespace sys = boost::system;

#include <memory>

class TcpAsyncService : public std::enable_shared_from_this<TcpAsyncService> {
public:
    explicit TcpAsyncService(net::ip::tcp::socket&& socket);

    void
    handle();

private:
    void
    onReadDone(const sys::error_code& ec, std::size_t bytesRead);

    void
    onWriteDone(const sys::error_code& ec, std::size_t bytesWritten);

private:
    net::ip::tcp::socket _socket;
    net::streambuf _buffer;
};