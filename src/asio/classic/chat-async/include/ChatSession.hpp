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

#include <string>
#include <queue>
#include <memory>

class ChatSession : public std::enable_shared_from_this<ChatSession> {
public:
    using Ptr = std::shared_ptr<ChatSession>;

    explicit ChatSession(asio::io_context& context, tcp::socket&& socket);

    void start(MessageHandler onMessage, ErrorHandler onError);

    void post(std::string message);

private:
    void read();

    void onRead(sys::error_code errorCode, std::size_t bytes);

    void write();

    void onWrite(sys::error_code errorCode, std::size_t bytes);

private:
    tcp::socket _socket;
    asio::io_context::strand _strandR;
    asio::io_context::strand _strandW;
    asio::streambuf _buffer;
    std::queue<std::string> _outgoing;
    MessageHandler _onMessage;
    ErrorHandler _onError;
};