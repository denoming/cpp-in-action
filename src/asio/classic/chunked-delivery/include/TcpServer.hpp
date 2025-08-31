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

#include "Http.hpp"

#include <string_view>

class TcpServer {
public:
    explicit TcpServer(net::io_context& context);

    [[noreturn]] void
    listen(net::ip::port_type port);

    [[noreturn]] void
    listen(std::string_view port);

private:
    static void
    handleSession(TcpServer* server, tcp::socket&& socket);

private:
    net::io_context& _context;
};