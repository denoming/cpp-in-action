#pragma once

#include "Common.hpp"

#include <vector>
#include <thread>

class Runner {
public:
    void
    run(std::size_t threadsNum);

    [[nodiscard]] asio::io_context&
    context();

    [[nodiscard]] const asio::io_context&
    context() const;

private:
    void
    waitForTermination();

    void
    spawnThreads(std::size_t threadsNum);

private:
    asio::io_context _context;
    std::vector<std::jthread> _threads;
};

//
// Inlines
//

inline asio::io_context&
Runner::context()
{
    return _context;
}

inline const asio::io_context&
Runner::context() const
{
    return _context;
}