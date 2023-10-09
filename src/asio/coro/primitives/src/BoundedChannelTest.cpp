#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "BoundedChannel.hpp"
#include "Scheduler.hpp"
#include "Utils.hpp"

#include <spdlog/spdlog.h>
#ifdef DEBUG
#include <spdlog/sinks/stdout_sinks.h>
#endif

using namespace testing;

using TypedBoundedChannel = BoundedChannel<char>;

class BoundedChannelTest : public Test {
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

TEST_F(BoundedChannelTest, Transfer)
{
    static const size_t kChannelCapacity{3000};
    static const size_t kChunkSize{300};
    static const size_t kDataSize{113 * 1024 /* 100 Kb */};

    std::array<char, kDataSize> dataFrom = {};
    std::array<char, kDataSize> dataTo = {};

    // Fill array by random data
    std::generate(std::begin(dataFrom), std::end(dataFrom), []() { return generate(); });

    auto send = [&](TypedBoundedChannel& channel) -> io::awaitable<void> {
        size_t n = 0;
        const char* ptr = dataFrom.data();
        do {
            auto buffer = io::buffer(ptr + n, std::min(kChunkSize, kDataSize - n));
            EXPECT_THAT(buffer, SizeIs(Le(kDataSize)));
            co_await channel.send(buffer);
            n += buffer.size();
        }
        while (n < kDataSize);
        EXPECT_EQ(n, kDataSize);
        co_await channel.send(io::error::eof);
        channel.close();
    };

    auto recv = [&](TypedBoundedChannel& channel) -> io::awaitable<void> {
        std::string chunk(kChunkSize, 0);
        size_t n = 0;
        while (true) {
            auto [error, size] = co_await channel.recv(io::buffer(chunk));
            if (size > 0) {
                std::copy(std::begin(chunk), std::begin(chunk) + int32_t(size), &dataTo[n]);
                n += size;
            }
            if (error) {
                break;
            }
        }
        EXPECT_EQ(n, kDataSize);
        co_return;
    };

    io::io_context context;
    TypedBoundedChannel channel{context.get_executor(), kChannelCapacity};
    io::co_spawn(context, send(channel), io::detached);
    io::co_spawn(context, recv(channel), io::detached);
    context.run();

    EXPECT_EQ(dataFrom, dataTo);
}

TEST_F(BoundedChannelTest, Close)
{
    static const size_t kChannelCapacity{3000};
    static const size_t kChunkSize{300};

    auto send = [&](TypedBoundedChannel& channel) -> io::awaitable<void> {
        std::string chunk(kChunkSize, 0);
        while(true) {
            const auto [ec, size] = co_await channel.send(io::buffer(chunk));
            if (ec) {
                break;
            }
            co_await scheduler(co_await io::this_coro::executor);
        }
    };

    auto recv = [&](TypedBoundedChannel& channel) -> io::awaitable<void> {
        std::string chunk(kChunkSize, 0);
        while(true) {
            const auto [ec, size] = co_await channel.recv(io::buffer(chunk));
            if (ec) {
                break;
            }
            co_await scheduler(co_await io::this_coro::executor);
        }
    };

    io::io_context context;
    TypedBoundedChannel channel{context.get_executor(), kChannelCapacity};
    io::co_spawn(context, send(channel), io::detached);
    io::co_spawn(context, recv(channel), io::detached);
    io::co_spawn(
        context,
        [&]() -> io::awaitable<void> {
            co_await asyncSleep(std::chrono::milliseconds{50});
            channel.close();
        },
        io::detached);
    context.run();
}