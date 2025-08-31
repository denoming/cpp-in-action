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

#include "Common.hpp"
#include "CircularBuffer.hpp"

#include <memory>

class TcpEchoSession : public std::enable_shared_from_this<TcpEchoSession> {
public:
    explicit TcpEchoSession(tcp::socket&& socket);

    void
    start();

private:
    void
    doClose();

    void
    doRead();

    void
    onRead(const sys::error_code& error, std::size_t bytesTransferred);

    void
    doWrite();

    void
    onWrite(const sys::error_code& error, std::size_t bytesTransferred);

private:
    bool _writing{false};
    CircularBuffer<65536> _buffer;
    tcp::socket _socket;
};