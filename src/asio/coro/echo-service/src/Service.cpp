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
