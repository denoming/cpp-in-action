#include <gtest/gtest.h>

#include "Event.hpp"

#include <boost/asio.hpp>

using namespace testing;

class EventTest : public TestWithParam<std::size_t> {
public:
};

INSTANTIATE_TEST_SUITE_P(Coroutines, EventTest, testing::Values(1'000, 10'000));

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
            co_await event1.wait();
            event2.set();
            flag1.store(true);
            co_return;
        };

        auto producer = [&]() -> io::awaitable<void> {
            event1.set();
            co_await event2.wait();
            flag2.store(true);
            co_return;
        };

        io::co_spawn((n % 2) ? executor1 : executor2, consumer(), io::detached);
        io::co_spawn((n % 2) ? executor2 : executor1, producer(), io::detached);

        while (not flag1 or not flag2) { }
    }

    pool.join();
}