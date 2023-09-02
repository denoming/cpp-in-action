#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "DataChannel.hpp"
#include "EventDataChannel.hpp"

#include <string_view>
#include <random>

using namespace testing;

static const std::size_t kChannelCapacity{128};

using StringEventDataChannel = EventDataChannel<char>;

static std::int32_t
randomInt(std::int32_t from = 0, std::int32_t to = std::numeric_limits<std::int32_t>::max())
{
    static std::random_device RandomDevice{};
    std::mt19937 gen{RandomDevice()};
    return std::uniform_int_distribution<std::int32_t>{from, to}(gen);
}

static std::string
randomString(std::size_t len)
{
    static const std::string_view in{"ABCDEFGHIKLMNOPQRSTVXYZ"};
    std::string out;
    out.reserve(len);
    while (len-- > 0) {
        const auto p = randomInt(0, std::int32_t(in.size()) - 1);
        out.push_back(in[p]);
    }
    return out;
}

static io::awaitable<std::string>
reader(StringEventDataChannel& channel)
{
    std::string result;
    io::streambuf buffer;
    while (channel.active() or not channel.empty()) {
        auto outputSeq = buffer.prepare(kChannelCapacity);
        std::size_t size = co_await channel.receive(outputSeq);
        buffer.commit(size);
        const auto inputSeq = buffer.data();
        result.append(static_cast<const char*>(inputSeq.data()), inputSeq.size());
        buffer.consume(size);
    }
    co_return result;
}

static io::awaitable<void>
writer(StringEventDataChannel& channel, std::string input)
{
    const auto size = static_cast<std::int32_t>(input.size());
    for (std::int32_t pos = 0; pos < size;) {
        std::int32_t step = std::min(randomInt(1, kChannelCapacity * 2), size - pos);
        auto buffer = io::buffer(&input[pos], step);
        co_await channel.send(buffer);
        pos += step;
    }
    channel.close();
}

TEST(DataChannelTest, Writing)
{
    io::io_context context{1};

    const std::size_t kInputDataSize = randomInt(1000, 2000);
    std::string input = randomString(kInputDataSize);
    MockFunction<void(std::exception_ptr, std::string)> token;
    EXPECT_CALL(token, Call(std::exception_ptr{}, Eq(input)));

    StringEventDataChannel channel{context.get_executor(), kChannelCapacity};
    io::co_spawn(context.get_executor(), writer(channel, input), io::detached);
    io::co_spawn(context.get_executor(), reader(channel), token.AsStdFunction());

    context.run();
}
