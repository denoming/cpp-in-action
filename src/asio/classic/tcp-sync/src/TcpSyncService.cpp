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

#include "TcpSyncService.h"

#include <thread>
#include <chrono>
#include <iostream>

namespace sys = boost::system;

namespace {

std::string
getResponse(net::streambuf& input)
{
    std::istream is{&input};
    std::string request;
    std::getline(is, request);
    if (request == "Ping") {
        return "Pong\n";
    }
    if (request == "Pong") {
        return "Ping\n";
    }
    return "^_^\n";
}

} // namespace

void
TcpSyncService::handle(net::ip::tcp::socket& socket)
{
    using namespace std::chrono_literals;

    try {
        net::streambuf buffer;
        net::read_until(socket, buffer, '\n');

        std::this_thread::sleep_for(1s);

        const std::string response{getResponse(buffer)};
        net::write(socket, net::buffer(response));
    } catch (const sys::system_error& e) {
        std::cerr << e.what() << std::endl;
    }
}