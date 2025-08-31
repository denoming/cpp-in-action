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

class TcpSyncClient {
public:
    TcpSyncClient(std::string_view address, net::ip::port_type port);

    void
    connect();

    void
    close();

    [[nodiscard]] std::string
    communicate();

private:
    void
    sendRequest(const std::string& request);

    std::string
    receiveResponse();

private:
    net::io_context _context;
    net::ip::tcp::endpoint _endpoint;
    net::ip::tcp::socket _socket;
};