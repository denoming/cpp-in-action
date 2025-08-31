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

#include "TcpAsyncServer.h"
#include "TcpAsyncClient.h"

#include <boost/asio.hpp>
#include <boost/program_options.hpp>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>

namespace net = boost::asio;
namespace sys = boost::system;
namespace po = boost::program_options;

using namespace std::chrono_literals;

static void
executeClient(std::string message)
{
    bool exit{false};
    std::mutex exitMutex;
    std::condition_variable whenExit;

    TcpAsyncClient client;
    client.communicate(std::move(message),
                       "127.0.0.1",
                       3333,
                       [&](const auto requestId, std::string response, const sys::error_code ec) {
                           if (ec) {
                               std::cerr << "Error: " << ec.what() << std::endl;
                           } else {
                               std::cout << "Response: " << response << std::endl;
                           }

                           std::unique_lock lock{exitMutex};
                           exit = true;
                           lock.unlock();
                           whenExit.notify_one();
                       });

    std::unique_lock lock{exitMutex};
    whenExit.wait(lock, [&]() { return exit; });

    client.close();
}

static void
executeServer()
{
    bool exit{false};
    std::mutex exitMutex;
    std::condition_variable whenExit;

    net::io_context context;
    boost::asio::signal_set signals(context, SIGINT, SIGTERM);
    signals.async_wait([&](sys::error_code ec, int) {
        if (ec) {
            std::cerr << "executeServer: " << ec.what() << std::endl;
        }

        std::unique_lock lock{exitMutex};
        exit = true;
        lock.unlock();
        whenExit.notify_one();
    });

    TcpAsyncServer server{context};
    server.start(3333);

    std::unique_lock lock{exitMutex};
    whenExit.wait(lock, [&]() { return exit; });

    server.stop();
}

int
main(int argc, char* argv[])
{
    std::string message;
    bool runClient{false};
    bool runServer{false};

    po::options_description desc("Allowed options");
    // clang-format off
    desc.add_options()
        ("help", "produce help message")
        ("client,c", po::bool_switch(&runClient), "Run client")
        ("server,s", po::bool_switch(&runServer), "Run server")
        ("message,m", po::value<std::string>(&message)->default_value("Ping"), "Message to send")
        ;
    // clang-format on

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (!runClient && !runServer) {
        return EXIT_FAILURE;
    }

    if (runClient) {
        executeClient(std::move(message));
    }
    if (runServer) {
        executeServer();
    }
    return EXIT_SUCCESS;
}