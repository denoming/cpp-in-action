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

#include "TcpSyncServer.h"
#include "TcpSyncClient.h"

#include <boost/asio.hpp>
#include <boost/program_options.hpp>

#include <thread>
#include <iostream>

namespace net = boost::asio;
namespace po = boost::program_options;

using namespace std::chrono_literals;

static void
executeClient()
{
    TcpSyncClient client{"127.0.0.1", 3333};
    client.connect();
    const auto response = client.communicate();
    std::cout << "Response: " <<  response << std::endl;
    client.close();
}

static void
executeServer()
{
    TcpSyncServer server;
    server.start(3333);
    std::this_thread::sleep_for(30s);
    server.stop();
}

int
main(int argc, char* argv[])
{
    bool runClient{false};
    bool runServer{false};

    po::options_description desc("Allowed options");
    // clang-format off
    desc.add_options()
        ("help", "produce help message")
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

    if (runClient) {
        executeClient();
    }
    if (runServer) {
        executeServer();
    }
    return EXIT_SUCCESS;
}