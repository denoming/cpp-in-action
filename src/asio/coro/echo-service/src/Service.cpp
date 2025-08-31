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

#include <fmt/format.h>

namespace io = boost::asio;
namespace sys = boost::system;

using tcp = boost::asio::ip::tcp;

io::awaitable<void>
session(tcp::socket socket)
{
    try {
        char data[1024];
        for (;;) {
            std::size_t n = co_await socket.async_read_some(io::buffer(data), io::use_awaitable);
            co_await io::async_write(socket, io::buffer(data, n), io::use_awaitable);
        }
    } catch (const sys::system_error& e) {
        if (e.code() == io::error::eof) {
            fmt::print(stderr, "Session done\n");
        } else {
            fmt::print(stderr, "Exception: {}\n", e.what());
        }

    }
}

io::awaitable<void>
listener()
{
    auto executor = co_await io::this_coro::executor;
    tcp::acceptor acceptor{executor, {tcp::v4(), 8080}};
    for (;;) {
        tcp::socket socket = co_await acceptor.async_accept(io::use_awaitable);
        io::co_spawn(executor, session(std::move(socket)), io::detached);
    }
}

int
main()
{
    try {
        io::io_context context{1};

        io::signal_set signals{context, SIGINT, SIGTERM};
        signals.async_wait([&](auto, auto) { context.stop(); });

        /* Spawn a new coroutine-based thread of execution */
        io::co_spawn(context, listener(), io::detached /* explicitly ignore the result */);

        context.run();
    } catch (const std::exception& e) {
        fmt::print(stderr, "Exception: {}", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
