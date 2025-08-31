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
#include <boost/asio/signal_set.hpp>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>

namespace net = boost::asio;
namespace po = boost::program_options;

static const char* DefaultHost{"127.0.0.1"};
static const char* DefaultPort{"3333"};

static void
print(net::streambuf& buffer)
{
    std::istream is{&buffer};
    std::string line;
    while (std::getline(is, line)) {
        std::cout << line << std::endl;
    }
}

std::thread
spawnWorker(net::io_context& context)
{
    return std::thread{[&context]() {
        const auto guard = net::make_work_guard(context);
        context.run();
    }};
}

void
waitForTermination(net::io_context& context)
{
    std::mutex closeGuard;
    std::condition_variable whenClose;
    bool close{false};

    net::signal_set signal{context, SIGINT, SIGTERM};
    signal.async_wait([&close, &closeGuard, &whenClose](sys::error_code ec, int) {
        std::unique_lock lock{closeGuard};
        close = true;
        lock.unlock();
        whenClose.notify_one();
    });

    std::unique_lock lock{closeGuard};
    whenClose.wait(lock, [&close]() { return close; });
}

static void
executeClient(net::io_context& context, const std::string& host, const std::string& port)
{
    TcpClient client{context};
    client.connect(host, port);
    std::cout << client.get() << std::endl;
    context.stop();
}

static void
executeServer(net::io_context& context, const std::string& port)
{
    TcpServer server{context, static_cast<net::ip::port_type>(std::stoi(port))};
    waitForTermination(context);
    context.stop();
}

int
main(int argc, char* argv[])
{
    bool runClient{false};
    bool runServer{false};
    std::string host;
    std::string port;

    po::options_description desc("Allowed options");
    // clang-format off
    desc.add_options()
        ("client,c", po::bool_switch(&runClient), "Run client")
        ("server,s", po::bool_switch(&runServer), "Run server")
        ("host,h", po::value<std::string>(&host)->default_value(DefaultHost), "Set host")
        ("port,p", po::value<std::string>(&port)->default_value(DefaultPort), "Set port")
        ;
    // clang-format on

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (!runClient && !runServer) {
        return EXIT_FAILURE;
    }

    net::io_context context;
    auto thread = spawnWorker(context);
    if (runClient) {
        executeClient(context, host, port);
    }
    if (runServer) {
        executeServer(context, port);
    }
    if (thread.joinable()) {
        thread.join();
    }
    return EXIT_SUCCESS;
}