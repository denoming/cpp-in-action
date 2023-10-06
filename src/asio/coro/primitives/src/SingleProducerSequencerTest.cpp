#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Scheduler.hpp"
#include "Utils.hpp"
#include "SingleProducerSequencer.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_sinks.h>

#include <array>

using namespace testing;

using Barrier = SequenceBarrier<uint16_t>;
using Sequencer = SingleProducerSequencer<uint16_t>;

static const size_t kDefaultBufferSize{256};
static const size_t kMask{kDefaultBufferSize - 1};
static const size_t kIterations{kDefaultBufferSize * 3};
static const int32_t kStopValue = std::numeric_limits<int32_t>::max();
static const int32_t kNoneValue = -1;

class SingleProducerSequencerTest : public Test {
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

TEST_F(SingleProducerSequencerTest, ClaimOne)
{
    std::int64_t result{};
    std::int64_t expectedResult = kIterations * (kIterations + 1) / 2;

    std::array<int32_t, kDefaultBufferSize> values = {};
    std::fill(std::begin(values), std::end(values), kNoneValue);

    auto producer = [&](Sequencer& sequencer) -> io::awaitable<void> {
        for (int n = 0; n < kIterations; ++n) {
            const auto seq = co_await sequencer.claimOne();

            spdlog::debug("send: write<{}>", seq & kMask);
            values.at(seq & kMask) = n + 1;

            sequencer.publish(seq);
        }

        auto seq = co_await sequencer.claimOne();
        values.at(seq & kMask) = kStopValue;
        sequencer.publish(seq);

        co_return;
    };

    auto consumer = [&](Sequencer& sequencer, Barrier& barrier) -> io::awaitable<void> {
        size_t k{0};
        while (true) {
            const size_t available = co_await sequencer.wait(k);
            do {
                spdlog::debug("recv: read<{}>", k & kMask);
                if (const int32_t value = values[k % kDefaultBufferSize]; value == kStopValue) {
                    spdlog::debug("recv: exit");
                    co_return;
                } else {
                    result += value;
                }
            }
            while (k++ != available);
            barrier.publish(available);
        }
    };

    Barrier barrier;
    Sequencer sequencer{barrier, kDefaultBufferSize};

    io::thread_pool pool{2};
    io::any_io_executor executorA = pool.get_executor();
    io::any_io_executor executorB = pool.get_executor();
    co_spawn(executorA, consumer(sequencer, barrier), io::detached);
    co_spawn(executorB, producer(sequencer), io::detached);
    pool.join();

    EXPECT_EQ(result, expectedResult);
}

TEST_F(SingleProducerSequencerTest, ClaimUpTo)
{
    static const size_t kBatchSize{16};

    std::int64_t result{};
    std::int64_t expectedResult = kIterations * (kIterations + 1) / 2;

    std::array<int32_t, kDefaultBufferSize> values = {};
    std::fill(std::begin(values), std::end(values), kNoneValue);

    auto producer = [&](Sequencer& sequencer) -> io::awaitable<void> {
        int n{};
        while (n < kIterations) {
            const auto range = co_await sequencer.claimUpTo(kBatchSize);
            for (std::unsigned_integral auto seq : range) {
                spdlog::debug("send: write<{}>", seq & kMask);
                values.at(seq & kMask) = ++n;
            }
            sequencer.publish(range);
        }

        auto seq = co_await sequencer.claimOne();
        values.at(seq & kMask) = kStopValue;
        sequencer.publish(seq);

        co_return;
    };

    auto consumer = [&](Sequencer& sequencer, Barrier& barrier) -> io::awaitable<void> {
        size_t k{0};
        while (true) {
            const size_t available = co_await sequencer.wait(k);
            do {
                spdlog::debug("recv: read<{}>", k & kMask);
                if (const int32_t value = values[k % kDefaultBufferSize]; value == kStopValue) {
                    spdlog::debug("recv: exit");
                    co_return;
                } else {
                    result += value;
                }
            }
            while (k++ != available);
            barrier.publish(available);
        }
    };

    Barrier barrier;
    Sequencer sequencer{barrier, kDefaultBufferSize};

    io::thread_pool pool{2};
    io::any_io_executor executorA = pool.get_executor();
    io::any_io_executor executorB = pool.get_executor();
    co_spawn(executorA, consumer(sequencer, barrier), io::detached);
    co_spawn(executorB, producer(sequencer), io::detached);
    pool.join();

    EXPECT_EQ(result, expectedResult);
}

TEST_F(SingleProducerSequencerTest, CloseWhenClaimOne)
{
    auto producer = [&](Sequencer& sequencer) -> io::awaitable<void> {
        // Claim all slots first
        co_await sequencer.claimUpTo(kDefaultBufferSize);
        try {
            co_await sequencer.claimOne();
        } catch (const sys::system_error& e) {
            EXPECT_EQ(e.code(), io::error::operation_aborted);
        }
    };

    auto consumer = [&](Sequencer& sequencer, Barrier& barrier) -> io::awaitable<void> {
        try {
            co_await sequencer.wait(0);
        } catch (const sys::system_error& e) {
            EXPECT_EQ(e.code(), io::error::operation_aborted);
        }
    };

    Barrier barrier;
    Sequencer sequencer{barrier, kDefaultBufferSize};

    io::io_context context;
    co_spawn(context, consumer(sequencer, barrier), io::detached);
    co_spawn(context, producer(sequencer), io::detached);
    co_spawn(
        context,
        [&]() -> io::awaitable<void> {
            sequencer.close();
            co_return;
        },
        io::detached);
    context.run();
}

TEST_F(SingleProducerSequencerTest, CancelClaimOne)
{
    auto producer = [&](Sequencer& sequencer) -> io::awaitable<void> {
        // Claim all slots first
        co_await sequencer.claimUpTo(kDefaultBufferSize);
        try {
            using namespace ioe::awaitable_operators;
            auto rv = co_await (sequencer.claimOne() or asyncSleep(std::chrono::milliseconds(10)));
            EXPECT_EQ(rv.index(), 1 /* sleep end first */);
        } catch (const sys::system_error& e) {
            EXPECT_TRUE(false) << "Should be called";
        }
    };

    Barrier barrier;
    Sequencer sequencer{barrier, kDefaultBufferSize};

    io::io_context context;
    co_spawn(context, producer(sequencer), io::detached);
    context.run();
}

TEST_F(SingleProducerSequencerTest, CancelClaimUpTo)
{
    auto producer = [&](Sequencer& sequencer) -> io::awaitable<void> {
        // Claim all slots first
        co_await sequencer.claimUpTo(kDefaultBufferSize);
        try {
            using namespace ioe::awaitable_operators;
            auto rv = co_await (sequencer.claimUpTo(kDefaultBufferSize)
                                or asyncSleep(std::chrono::milliseconds(10)));
            EXPECT_EQ(rv.index(), 1 /* sleep end first */);
        } catch (const sys::system_error& e) {
            EXPECT_TRUE(false) << "Should be called";
        }
    };

    Barrier barrier;
    Sequencer sequencer{barrier, kDefaultBufferSize};

    io::io_context context;
    co_spawn(context, producer(sequencer), io::detached);
    context.run();
}

TEST_F(SingleProducerSequencerTest, CancelWait)
{
    auto producer = [&](Sequencer& sequencer) -> io::awaitable<void> {
        // Claim all slots first
        co_await sequencer.claimUpTo(kDefaultBufferSize);
        try {
            co_await sequencer.claimOne();
        } catch (const sys::system_error& e) {
            EXPECT_EQ(e.code(), io::error::operation_aborted);
        }
    };

    auto consumer = [&](Sequencer& sequencer) -> io::awaitable<void> {
        try {
            using namespace ioe::awaitable_operators;
            auto rv = co_await (sequencer.wait(0) or asyncSleep(std::chrono::milliseconds(10)));
            EXPECT_EQ(rv.index(), 1 /* sleep end first */);
        } catch (const sys::system_error& e) {
            EXPECT_TRUE(false) << "Should be called";
        }
    };

    Barrier barrier;
    Sequencer sequencer{barrier, kDefaultBufferSize};

    io::io_context context;
    co_spawn(context, producer(sequencer), io::detached);
    co_spawn(context, consumer(sequencer), io::detached);
    context.run();
}



