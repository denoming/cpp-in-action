#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <boost/asio.hpp>
#include <boost/circular_buffer.hpp>

#include <fmt/format.h>

#include <string_view>
#include <random>

using namespace testing;

namespace io = boost::asio;
namespace sys = boost::system;

static std::random_device RandomDevice{};

template<typename T>
class DataChannel {
public:
    using BufferType = boost::circular_buffer<T>;
    using HandlerType = std::move_only_function<void()>;

    explicit DataChannel(io::any_io_executor executor, std::size_t capacity)
        : _executor{std::move(executor)}
        , _buffer{capacity}
    {
    }

    io::awaitable<std::size_t>
    receive(io::mutable_buffer buffer)
    {
        return io::async_initiate<decltype(io::use_awaitable), void(std::size_t)>(
            [this](io::completion_handler_for<void(std::size_t)> auto handler,
                   io::mutable_buffer buffer) {
                saveRecvHandler(std::move(handler), buffer);
                if (_needRecv = buffer.size(); not _buffer.empty()) {
                    callRecvHandler();
                }
            },
            io::use_awaitable,
            buffer);
    }

    io::awaitable<std::size_t>
    send(io::const_buffer buffer)
    {
        return io::async_initiate<decltype(io::use_awaitable), void(std::size_t)>(
            [this](io::completion_handler_for<void(std::size_t)> auto handler,
                   io::const_buffer buffer) {
                saveSendHandler(std::move(handler), buffer);
                if (_needSend = buffer.size(); not _buffer.full()) {
                    callSendHandler();
                }
            },
            io::use_awaitable,
            buffer);
    }

private:
    std::size_t
    writeBuffer(io::const_buffer& buffer)
    {
        const T* ptr = static_cast<const T*>(buffer.data());
        const auto n = std::min(_buffer.capacity() - _buffer.size(), buffer.size());
        _buffer.insert(_buffer.end(), ptr, ptr + n);
        buffer += n;
        fmt::print(stderr, "send: n<{}>, remain<{}>\n", n, buffer.size());
        return n;
    }

    std::size_t
    readBuffer(io::mutable_buffer& buffer)
    {
        T* ptr = static_cast<T*>(buffer.data());
        const std::size_t n = std::min(_buffer.size(), buffer.size());
        std::copy(_buffer.cbegin(), _buffer.cbegin() + n, ptr);
        _buffer.erase_begin(n);
        buffer += n;
        fmt::print(stderr, "recv: n<{}>, remain<{}>\n", n, buffer.size());
        return n;
    }

    void
    callRecvHandler()
    {
        if (_recvHandler) {
            _recvHandler();
        }
    }

    void
    callSendHandler()
    {
        if (_sendHandler) {
            _sendHandler();
        }
    }

    void
    saveRecvHandler(io::completion_handler_for<void(std::size_t)> auto handler,
                    io::mutable_buffer buffer)
    {
        _recvHandler = [this, handler = std::move(handler), buffer]() mutable {
            _recv += readBuffer(buffer);
            if (_recv == _needRecv) {
                io::post(_executor,
                         [handler = std::move(handler), recv = _recv]() mutable { handler(recv); });
                _needRecv = _recv = 0;
            }
            if (_needSend > 0) {
                callSendHandler();
            }
        };
    }

    void
    saveSendHandler(io::completion_handler_for<void(std::size_t)> auto handler,
                    io::const_buffer buffer)
    {
        _sendHandler = [this, handler = std::move(handler), buffer]() mutable {
            _send += writeBuffer(buffer);
            if (_send == _needSend) {
                io::post(_executor,
                         [handler = std::move(handler), send = _send]() mutable { handler(send); });
                _needSend = _send = 0;
            }
            if (_needRecv > 0) {
                callRecvHandler();
            }
        };
    }

private:
    io::any_io_executor _executor;
    HandlerType _recvHandler;
    HandlerType _sendHandler;
    std::size_t _needRecv{};
    std::size_t _recv{};
    std::size_t _needSend{};
    std::size_t _send{};
    BufferType _buffer;
};

using StringDataChannel = DataChannel<char>;

static std::int32_t
randomInt(std::int32_t from = 0, std::int32_t to = std::numeric_limits<std::int32_t>::max())
{
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
reader(StringDataChannel& channel)
{
    std::string out;

    char chunk[100];
    while (out.size() < 1000) {
        auto buffer = io::buffer(chunk);
        std::size_t size = co_await channel.receive(buffer);
        out.append(&chunk[0], size);
    }

    co_return out;
}

static io::awaitable<void>
writer(StringDataChannel& channel, std::string input)
{
    auto remain = static_cast<std::int32_t>(input.size());
    for (std::size_t pos = 0; pos < input.size() and remain > 0;) {
        std::int32_t step = std::min(randomInt(1, 128), remain);
        auto buffer = io::buffer(&input[pos], step);
        co_await channel.send(buffer);
        pos += step, remain -= step;
    }

    co_return;
}

TEST(DataChannel, Writing)
{
    io::io_context context{1};

    std::string input = randomString(1000);
    MockFunction<void(std::exception_ptr, std::string)> token;
    EXPECT_CALL(token, Call(std::exception_ptr{}, Eq(input)));

    StringDataChannel channel{context.get_executor(), 128};
    io::co_spawn(context.get_executor(), writer(channel, input), io::detached);
    io::co_spawn(context.get_executor(), reader(channel), token.AsStdFunction());

    context.run();
}