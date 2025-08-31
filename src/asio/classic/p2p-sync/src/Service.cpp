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

#include "TcpClient.hpp"
#include "TcpServer.hpp"

#include <boost/asio.hpp>
#include <boost/program_options.hpp>

#include <iostream>

namespace net = boost::asio;
namespace po = boost::program_options;

static const std::string_view Address{"127.0.0.1"};
static const net::ip::port_type Port{3333};

static void
print(net::streambuf& buffer)
{
    std::istream is{&buffer};
    std::string line;
    while (std::getline(is, line)) {
        std::cout << line << std::endl;
    }
}

static void
executeClient(net::io_context& context)
{
    static const std::string_view Request{"Request Payload\n"};

    TcpClient client{context};
    client.connect(Address, Port);
    client.send(Request.data(), Request.size());
    client.shutdown();

    net::streambuf buffer;
    const auto bytesRead = client.receive(buffer);
    if (bytesRead > 0) {
        print(buffer);
    }
}

static void
executeServer(net::io_context& context)
{
    SyncServer server{context};
    server.listen(Port);

    net::streambuf buffer;
    server.receive(buffer);
    print(buffer);

    static const std::string_view Response{"Response Payload\n"};
    server.send(Response.data(), Response.size());

    server.finalize();
}

int
main(int argc, char* argv[])
{
    bool runClient{false};
    bool runServer{false};

    po::options_description desc("Allowed options");
    // clang-format off
    desc.add_options()
        ("client,c", po::bool_switch(&runClient), "Run client")
        ("server,s", po::bool_switch(&runServer), "Run server")
        ;
    // clang-format on

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (!runClient && !runServer) {
        return EXIT_FAILURE;
    }

    net::io_context context;
    if (runClient) {
        executeClient(context);
    }
    if (runServer) {
        executeServer(context);
    }
    return EXIT_SUCCESS;
}