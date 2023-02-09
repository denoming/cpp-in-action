#include "Runner.hpp"

#include <cstdio>

void
Runner::run(std::size_t threadsNum)
{
    spawnThreads(threadsNum);
    waitForTermination();
}

void
Runner::waitForTermination()
{
    asio::signal_set signals{_context, SIGINT, SIGTERM};
    signals.async_wait([this](const sys::error_code& error, int signal) {
        if (!error) {
            _context.stop();
        }
    });
    _context.run();
}

void
Runner::spawnThreads(std::size_t threadsNum)
{
    assert(threadsNum >= 2);
    for (std::size_t n{1}; n <= threadsNum; ++n) {
        std::printf("Spawn <%zu> thread\n", n);
        _threads.emplace_back([this]() { _context.run(); });
    }
}