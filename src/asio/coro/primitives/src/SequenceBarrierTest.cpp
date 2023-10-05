#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Asio.hpp"
#include "Utils.hpp"
#include "SequenceBarrier.hpp"
#include "Scheduler.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

using namespace testing;

class SequenceBarrierTest : public Test {
public:
#ifdef DEBUG
    static void
    SetUpTestSuite()
    {
        spdlog::set_pattern("[%E] %v");
        spdlog::set_level(spdlog::level::debug);
    }
#endif
};

TEST_F(SequenceBarrierTest, WaitPublish)
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

TEST_F(SequenceBarrierTest, OneProducerOneConsumer)
{
    SequenceBarrier barrier;

    bool pointA{false};
    bool pointB{false};
    bool pointC{false};

    auto consumer = [&]() -> io::awaitable<void> {
        EXPECT_EQ(co_await barrier.wait(0), 0);
        pointA = true;
        EXPECT_EQ(co_await barrier.wait(5), 5);
        pointB = true;
        EXPECT_EQ(co_await barrier.wait(10), 20);
        pointC = true;

        // Returns immediately
        EXPECT_EQ(co_await barrier.wait(15), 20);
        EXPECT_EQ(barrier.lastPublished(), 20);
    };

    auto producer = [&]() -> io::awaitable<void> {
        EXPECT_FALSE(pointA);
        EXPECT_FALSE(pointB);
        EXPECT_FALSE(pointC);

        barrier.publish(0);
        co_await scheduler(co_await io::this_coro::executor);
        EXPECT_TRUE(pointA);

        barrier.publish(5);
        co_await scheduler(co_await io::this_coro::executor);
        EXPECT_TRUE(pointB);

        barrier.publish(20);
        co_await scheduler(co_await io::this_coro::executor);
        EXPECT_TRUE(pointC);
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

TEST_F(SequenceBarrierTest, OneProducerMultipleConsumers)
{
    SequenceBarrier barrier;

    bool pointA{false};
    bool pointB{false};
    bool pointC{false};
    bool pointD{false};
    bool pointE{false};
    bool pointF{false};

    auto consumer1 = [&]() -> io::awaitable<void> {
        EXPECT_EQ(co_await barrier.wait(0), 0);
        pointA = true;
        EXPECT_EQ(co_await barrier.wait(5), 10);
        pointB = true;
    };

    auto consumer2 = [&]() -> io::awaitable<void> {
        EXPECT_EQ(co_await barrier.wait(5), 10);
        pointC = true;
        EXPECT_EQ(co_await barrier.wait(20), 25);
        pointD = true;
    };

    auto consumer3 = [&]() -> io::awaitable<void> {
        EXPECT_EQ(co_await barrier.wait(15), 15);
        pointE = true;
        EXPECT_EQ(co_await barrier.wait(25), 25);
        pointF = true;
    };

    auto producer = [&]() -> io::awaitable<void> {
        barrier.publish(0);
        co_await scheduler(co_await io::this_coro::executor);
        EXPECT_TRUE(pointA);

        barrier.publish(10);
        co_await scheduler(co_await io::this_coro::executor);
        EXPECT_TRUE(pointB);
        EXPECT_TRUE(pointC);

        barrier.publish(15);
        co_await scheduler(co_await io::this_coro::executor);
        EXPECT_TRUE(pointE);

        barrier.publish(25);
        co_await scheduler(co_await io::this_coro::executor);
        EXPECT_TRUE(pointD);
        EXPECT_TRUE(pointF);
    };

    io::io_context context;
    io::co_spawn(
        context,
        [&]() -> io::awaitable<void> {
            using namespace ioe::awaitable_operators;
            co_await (consumer1() and consumer2() and consumer3() and producer());
            co_return;
        },
        io::detached);
    context.run();
}

TEST_F(SequenceBarrierTest, MultipleThreads)
{
    SequenceBarrier barrier1;
    SequenceBarrier barrier2;

#ifdef DEBUG
    spdlog::set_pattern("[%E] %v");
    spdlog::set_level(spdlog::level::debug);
#endif

    static const std::size_t kBufferSize{256};
    static const std::size_t kIterations{kBufferSize * 3};
    static const int32_t kStopValue = std::numeric_limits<int32_t>::max();
    static const int32_t kNoneValue = -1;

    std::int64_t result{};
    std::int64_t expectedResult = kIterations * (kIterations + 1) / 2;

    std::array<int32_t, kBufferSize> values = {};
    std::fill(std::begin(values), std::end(values), kNoneValue);

    auto producer = [&]() -> io::awaitable<void> {
        std::size_t available = (barrier2.lastPublished() + kBufferSize);
        for (std::size_t cursor = 0; cursor <= kIterations; ++cursor) {
            if (SequenceTraits<std::size_t>::precedes(available, cursor)) {
                spdlog::debug("send: cursor<{}>, available<{}>");
                available = co_await barrier2.wait(cursor - kBufferSize) + kBufferSize;
            }

            if (cursor == kIterations) {
                values[cursor % kBufferSize] = kStopValue;
            } else {
                spdlog::debug("send: write<{}>", cursor % kBufferSize);
                values[cursor % kBufferSize] = int32_t(cursor) + 1;
            }

            barrier1.publish(cursor);
        }
    };

    auto consumer = [&]() -> io::awaitable<void> {
        std::size_t cursor{0};
        while (true) {
            const std::size_t available = co_await barrier1.wait(cursor);

            spdlog::debug("recv: available<{}>", available);
            do {
                spdlog::debug("recv: process<{}>", available);
                if (const int32_t value = values[cursor % kBufferSize]; value == kStopValue) {
                    spdlog::debug("recv: exit");
                    co_return;
                } else {
                    EXPECT_NE(value, kNoneValue);
                    result += value;
                }
            }
            while (cursor++ != available);
            spdlog::debug("recv: publish<{}>");
            barrier2.publish(available);
        }
    };

    io::thread_pool pool{2};
    io::any_io_executor executor1 = pool.get_executor();
    io::any_io_executor executor2 = pool.get_executor();
    io::co_spawn(executor1, producer(), io::detached);
    io::co_spawn(executor2, consumer(), io::detached);
    pool.join();

    EXPECT_EQ(result, expectedResult);
}
