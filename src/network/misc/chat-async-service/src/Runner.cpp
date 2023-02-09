#include "Runner.hpp"

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
    for (std::size_t n{0}; n < threadsNum; ++n) {
        _threads.emplace_back([this]() { _context.run(); });
    }
}