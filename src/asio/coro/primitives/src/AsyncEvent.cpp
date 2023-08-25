#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <boost/asio.hpp>
#include <fmt/format.h>

#include <thread>
#include <atomic>

using namespace testing;

namespace io = boost::asio;
namespace sys = boost::system;

class AsyncEvent {
public:
    enum State { NotSet, Pending, Set };

    io::awaitable<void>
    wait(io::any_io_executor executor)
    {
        auto initiate = [this, executor](io::completion_token_for<void()> auto&& handler) mutable {
            _handler = [executor, handler = std::forward<decltype(handler)>(handler)]() mutable {
                io::post(executor, std::move(handler));
            };

            State oldState = State::NotSet;
            if (not _state.compare_exchange_strong(oldState, State::Pending)) {
                _handler();
            }
        };

        return io::async_initiate<decltype(io::use_awaitable), void()>(initiate, io::use_awaitable);
    }

    void
    set()
    {
        if (State oldState = _state.exchange(State::Set); oldState == State::Pending) {
            _handler();
        }
    }

private:
    std::atomic<State> _state{State::NotSet};
    std::move_only_function<void()> _handler;
};

class AsyncEventTest : public TestWithParam<std::size_t> {
public:
};

INSTANTIATE_TEST_SUITE_P(Coroutines, AsyncEventTest, testing::Values(1'000, 10'000));

TEST_P(AsyncEventTest, Test)
{
    io::thread_pool pool{2};
    io::any_io_executor executor1 = pool.get_executor();
    io::any_io_executor executor2 = pool.get_executor();

    for (std::size_t n = 0; n < GetParam(); ++n) {
        AsyncEvent event1;
        AsyncEvent event2;
        std::atomic<bool> flag1{false};
        std::atomic<bool> flag2{false};

        auto consumer = [&]() -> io::awaitable<void> {
            co_await event1.wait(co_await io::this_coro::executor);
            event2.set();
            flag1.store(true);
            co_return;
        };

        auto producer = [&]() -> io::awaitable<void> {
            event1.set();
            co_await event2.wait(co_await io::this_coro::executor);
            flag2.store(true);
            co_return;
        };

        io::co_spawn((n % 2) ? executor1 : executor2, consumer(), io::detached);
        io::co_spawn((n % 2) ? executor2 : executor1, producer(), io::detached);

        while (not flag1 or not flag2) { }
    }

    pool.join();
}