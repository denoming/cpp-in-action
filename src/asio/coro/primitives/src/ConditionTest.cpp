#include <gtest/gtest.h>

#include "Condition.hpp"
#include "Scheduler.hpp"

using namespace testing;

TEST(ConditionTest, Checking)
{
    int32_t v1{-1}, v2{+0};

    auto producer = [&](Condition& condition) -> io::awaitable<void> {
        co_await condition.notify();
        co_await scheduler(co_await io::this_coro::executor);

        v1 = +1;
        co_await condition.notify();
        co_await scheduler(co_await io::this_coro::executor);
    };

    auto consumer = [&](Condition& condition) -> io::awaitable<void> {
        EXPECT_EQ(co_await condition.wait([&]() { return (v1 > 0); }), sys::error_code{});
        v2++;
        co_await scheduler(co_await io::this_coro::executor);
    };

    io::io_context context;
    Condition condition{context.get_executor()};
    io::co_spawn(context, producer(condition), io::detached);
    io::co_spawn(context, consumer(condition), io::detached);
    context.run();

    EXPECT_EQ(v1, 1);
    EXPECT_EQ(v2, 1);
}

TEST(ConditionTest, Cancel)
{
    int32_t v1{-1};

    auto producer = [&](Condition& condition) -> io::awaitable<void> {
        co_await scheduler(co_await io::this_coro::executor);
        condition.close();
    };

    auto consumer = [&](Condition& condition) -> io::awaitable<void> {
        const auto ec = co_await condition.wait([&]() { return (v1 > 0); });
        EXPECT_EQ(ec.value(), io::error::operation_aborted);
    };

    io::io_context context;
    Condition condition{context.get_executor()};
    io::co_spawn(context, producer(condition), io::detached);
    io::co_spawn(context, consumer(condition), io::detached);
    context.run();
}
