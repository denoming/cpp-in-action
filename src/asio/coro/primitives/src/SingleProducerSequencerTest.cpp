#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Scheduler.hpp"
#include "Utils.hpp"
#include "SingleProducerSequencer.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_sinks.h>

#include <array>

using namespace testing;

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
    using Barrier = SequenceBarrier<uint16_t>;
    using Sequencer = SingleProducerSequencer<uint16_t>;

    static const size_t kBufferSize{256};
    static const size_t kMask{kBufferSize - 1};
    static const size_t kIterations{kBufferSize * 3};
    static const int32_t kStopValue = std::numeric_limits<int32_t>::max();
    static const int32_t kNoneValue = -1;

    std::int64_t result{};
    std::int64_t expectedResult = kIterations * (kIterations + 1) / 2;

    std::array<int32_t, kBufferSize> values = {};
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
                if (const int32_t value = values[k % kBufferSize]; value == kStopValue) {
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
    Sequencer sequencer{barrier, kBufferSize};

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
    using Barrier = SequenceBarrier<uint16_t>;
    using Sequencer = SingleProducerSequencer<uint16_t>;

    static const size_t kBufferSize{256};
    static const size_t kBatchSize{16};
    static const size_t kMask{kBufferSize - 1};
    static const size_t kIterations{kBufferSize * 3};
    static const int32_t kStopValue = std::numeric_limits<int32_t>::max();
    static const int32_t kNoneValue = -1;

    std::int64_t result{};
    std::int64_t expectedResult = kIterations * (kIterations + 1) / 2;

    std::array<int32_t, kBufferSize> values = {};
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
                if (const int32_t value = values[k % kBufferSize]; value == kStopValue) {
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
    Sequencer sequencer{barrier, kBufferSize};

    io::thread_pool pool{2};
    io::any_io_executor executorA = pool.get_executor();
    io::any_io_executor executorB = pool.get_executor();
    co_spawn(executorA, consumer(sequencer, barrier), io::detached);
    co_spawn(executorB, producer(sequencer), io::detached);
    pool.join();

    EXPECT_EQ(result, expectedResult);
}
