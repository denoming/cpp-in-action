#include <gtest/gtest.h>

#include "Event.hpp"
#include "Utils.hpp"

#include <boost/asio/experimental/awaitable_operators.hpp>

#include <thread>

using namespace testing;
using namespace std::literals;

class EventTest : public TestWithParam<std::size_t> {
public:
};

INSTANTIATE_TEST_SUITE_P(Coroutines, EventTest, testing::Values(100, 500));

TEST_P(EventTest, Test)
{
    io::thread_pool pool{2};
    io::any_io_executor executor1 = pool.get_executor();
    io::any_io_executor executor2 = pool.get_executor();

    for (std::size_t n = 0; n < GetParam(); ++n) {
        Event event1;
        Event event2;
        std::atomic<bool> flag1{false};
        std::atomic<bool> flag2{false};

        auto consumer = [&]() -> io::awaitable<void> {
            co_await event1.wait(io::use_awaitable);
            event2.set();
            flag1.store(true);
            co_return;
        };

        auto producer = [&]() -> io::awaitable<void> {
            event1.set();
            co_await event2.wait(io::use_awaitable);
            flag2.store(true);
            co_return;
        };

        io::co_spawn((n % 2) ? executor1 : executor2, consumer(), io::detached);
        io::co_spawn((n % 2) ? executor2 : executor1, producer(), io::detached);

        while (not flag1 or not flag2) {
            std::this_thread::yield();
        }
    }

    pool.join();
}

TEST_F(EventTest, AutoCancel)
{
    Event event;
    auto main = [&]() -> io::awaitable<void> {
        using namespace ioe::awaitable_operators;
        const auto result = co_await (event.wait(io::use_awaitable) or asyncSleep(50ms));
        EXPECT_EQ(result.index(), 1);
    };

    io::io_context context;
    io::co_spawn(context, main(), io::detached);
    context.run();
}

TEST_P(EventTest, ManualCancel)
{
    io::thread_pool pool{2};
    io::any_io_executor executor1 = pool.get_executor();
    io::any_io_executor executor2 = pool.get_executor();

    for (std::size_t n = 0; n < GetParam(); ++n) {
        std::atomic<bool> flag{false};

        Event event;
        auto main = [&]() -> io::awaitable<void> {
            sys::error_code ec;
            co_await event.wait(io::redirect_error(io::use_awaitable, ec));
            EXPECT_EQ(event.state(), Event::State::Cancelled);
            EXPECT_EQ(ec.value(), io::error::operation_aborted);
            flag = true;
        };

        auto time = [&]() -> io::awaitable<void> {
            co_await asyncSleep(10ms);
            event.cancel();
        };

        io::co_spawn((n % 2) ? executor1 : executor2, main(), io::detached);
        io::co_spawn((n % 2) ? executor2 : executor1, time(), io::detached);

        while (not flag) {
            std::this_thread::yield();
        }
    }
}
