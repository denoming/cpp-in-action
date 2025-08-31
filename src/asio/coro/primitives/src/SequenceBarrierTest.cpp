// Copyright 2025 Denys Asauliak
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Asio.hpp"
#include "Utils.hpp"
#include "SequenceBarrier.hpp"
#include "Scheduler.hpp"

#include <spdlog/spdlog.h>
#ifdef DEBUG
#include <spdlog/sinks/stdout_sinks.h>
#endif

using namespace testing;

class SequenceBarrierTest : public Test {
public:
#ifdef DEBUG
    static void
    SetUpTestSuite()
    {
        auto logger = spdlog::stderr_logger_mt("stderr");
        logger->set_pattern("[%E] %v");
        logger->set_level(spdlog::level::debug);
        spdlog::set_default_logger(logger);
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
        for (std::size_t n = 0; n <= kIterations; ++n) {
            if (SequenceTraits<std::size_t>::precedes(available, n)) {
                spdlog::debug("send: cursor<{}>, available<{}>");
                available = co_await barrier2.wait(n - kBufferSize) + kBufferSize;
            }

            if (n == kIterations) {
                values[n % kBufferSize] = kStopValue;
            } else {
                spdlog::debug("send: write<{}>", n % kBufferSize);
                values[n % kBufferSize] = int32_t(n) + 1;
            }

            barrier1.publish(n);
        }
    };

    auto consumer = [&]() -> io::awaitable<void> {
        std::size_t k{0};
        while (true) {
            const std::size_t available = co_await barrier1.wait(k);

            spdlog::debug("recv: available<{}>", available);
            do {
                spdlog::debug("recv: process<{}>", available);
                if (const int32_t value = values[k % kBufferSize]; value == kStopValue) {
                    spdlog::debug("recv: exit");
                    co_return;
                } else {
                    EXPECT_NE(value, kNoneValue);
                    result += value;
                }
            }
            while (k++ != available);
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

TEST_F(SequenceBarrierTest, Close)
{
    SequenceBarrier<size_t> barrier;

    size_t exceptions{};
    auto consumer = [&](size_t seq) -> io::awaitable<void> {
        try {
            co_await barrier.wait(seq);
        } catch (const sys::system_error& e) {
            exceptions++;
            EXPECT_EQ(e.code(), io::error::operation_aborted);
        }
    };

    io::io_context context;
    io::co_spawn(context, consumer(10), io::detached);
    io::co_spawn(context, consumer(15), io::detached);
    io::co_spawn(
        context,
        [&]() -> io::awaitable<void> {
            barrier.close();
            co_return;
        },
        io::detached);
    context.run();
    EXPECT_EQ(exceptions, 2);
}
