#pragma once

#include "Asio.hpp"

#include <chrono>

template<typename Rep, typename Period>
io::awaitable<void>
asyncSleep(std::chrono::duration<Rep, Period> duration)
{
    auto timer = io::system_timer(co_await io::this_coro::executor);
    timer.expires_after(duration);
    co_await timer.async_wait(io::use_awaitable);
}