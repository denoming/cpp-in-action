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

#include <boost/asio.hpp>

namespace asio = boost::asio;

/**
 * Simple UDP echo server
 *
 * To send something as a client use following command:
 *  $ nc -u localhost 8080 <Enter>
 *  Hi
 *  Hi
 */

int
main()
{
    std::uint16_t port{8080};

    asio::io_context context;
    asio::ip::udp::endpoint receiver{asio::ip::udp::v4(), port};
    asio::ip::udp::socket socket{context, receiver};

    char buffer[65535];
    while (true) {
        asio::ip::udp::endpoint sender;
        const auto transferred = socket.receive_from(asio::buffer(buffer), sender);
        socket.send_to(asio::buffer(buffer, transferred), sender);
    }

    return EXIT_SUCCESS;
}
