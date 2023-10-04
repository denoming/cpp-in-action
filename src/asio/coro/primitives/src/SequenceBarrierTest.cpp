#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Asio.hpp"
#include "Utils.hpp"
#include "SequenceBarrier.hpp"

using namespace testing;

TEST(SequenceBarrierTest, WaitPublish)
{
    SequenceBarrier barrier;

    auto consumer = [&]() -> io::awaitable<void> {
        auto lastSeq = co_await barrier.wait(7u);
        EXPECT_THAT(lastSeq, Eq(7));
        lastSeq = co_await barrier.wait(11u);
        EXPECT_THAT(lastSeq, Gt(7));
    };

    auto producer = [&]() -> io::awaitable<void> {
        barrier.publish(7u);
        co_await asyncSleep(std::chrono::milliseconds{1});
        barrier.publish(20u);
        co_return;
    };

    io::io_context context;
    io::co_spawn(
        context,
        [&]() -> io::awaitable<void> {
            using namespace ioe::awaitable_operators;
            co_await (consumer() and producer());
            co_return;
        },
        io::detached);
    context.run();
}