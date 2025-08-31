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

#include "TcpSession.hpp"

#include <boost/current_function.hpp>

#include <iostream>
#include <ctime>

namespace {

std::string
getDayTime()
{
    std::time_t now = std::time(nullptr);
    return std::ctime(&now);
}

} // namespace

TcpSession::TcpSession(tcp::socket&& socket)
    : _socket{std::move(socket)}
{
}

void
TcpSession::start()
{
    _message.assign(getDayTime());

    HANDLER_LOCATION;

    asio::async_write(_socket,
                      asio::buffer(_message),
                      std::bind_front(&TcpSession::onWriteDone, shared_from_this()));
}

void
TcpSession::onWriteDone(const sys::error_code& error, std::size_t bytesWritten)
{
    HANDLER_LOCATION;

    if (error) {
        std::cerr << "onWriteDone: " << error.message();
    } else {
        std::cout << "onWriteDone: " << bytesWritten << " bytes written" << std::endl;
    }
}
