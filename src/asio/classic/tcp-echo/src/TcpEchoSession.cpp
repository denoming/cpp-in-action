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

#include "TcpEchoSession.hpp"

#include "CircularBufferView.hpp"

TcpEchoSession::TcpEchoSession(tcp::socket&& socket)
    : _socket{std::move(socket)}
{
}

void
TcpEchoSession::start()
{
    // To start an echo session we should start to receive incoming data
    doRead();
}

void
TcpEchoSession::doClose()
{
    sys::error_code error;
    _socket.close(error);
}

void
TcpEchoSession::doRead()
{
    // Schedule asynchronous receiving of a data
    asio::async_read(_socket,
                     makeView(_buffer),
                     asio::transfer_at_least(1),
                     std::bind_front(&TcpEchoSession::onRead, shared_from_this()));
}

void
TcpEchoSession::onRead(const sys::error_code& error, std::size_t bytesTransferred)
{
    if (error) {
        // Close if an error has occurred
        doClose();
    } else {
        // Write data only if we aren't doing it already
        if (!_writing) {
            doWrite();
        }
        // Read next potion of data
        doRead();
    }
}

void
TcpEchoSession::doWrite()
{
    _writing = true;

    // Schedule asynchronous sending of the data
    asio::async_write(
        _socket, makeView(_buffer), std::bind_front(&TcpEchoSession::onWrite, shared_from_this()));
}

void
TcpEchoSession::onWrite(const sys::error_code& error, std::size_t bytesTransferred)
{
    _writing = false;

    if (error) {
        doClose();
    } else {
        // Check if there is something to send it back to the client
        if (!_buffer.empty()) {
            doWrite();
        }
    }
}
