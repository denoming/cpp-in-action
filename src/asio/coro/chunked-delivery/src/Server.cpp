#include "Server.hpp"

#include <fmt/format.h>
#include <fmt/std.h>

class Session : public std::enable_shared_from_this<Session> {
public:
    using Channel = ioe::channel<void(sys::error_code, io::const_buffer)>;

    explicit Session(tcp::socket&& socket)
        : _stream{std::move(socket)}
    {
    }

    void
    run()
    {
        auto channel = std::make_shared<Channel>(io::make_strand(_stream.get_executor()));

        io::co_spawn(
            channel->get_executor(),
            [self = shared_from_this(), channel]() {
                fmt::print("Session: Spawn producer\n");
                return self->producer(*channel);
            },
            io::detached);

        io::co_spawn(
            channel->get_executor(),
            [self = shared_from_this(), channel]() {
                fmt::print("Session: Spawn consumer\n");
                return self->consumer(*channel);
            },
            io::detached);
    }

private:
    io::awaitable<void>
    producer(Channel& channel)
    {
        fmt::print(stderr, "Thread: {}\n", std::this_thread::get_id());

        beast::flat_buffer buffer;
        http::request_parser<http::empty_body> reqPar;
        co_await http::async_read_header(_stream, buffer, reqPar, io::use_awaitable);

        if (reqPar.get()[http::field::expect] == "100-continue") {
            http::response<http::empty_body> res{http::status::continue_, kHttpVersion11};
            res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
            co_await http::async_write(_stream, res, io::use_awaitable);
        } else {
            fmt::print(stderr, "Server: 100 continue expected\n");
            _stream.close();
            co_return;
        }

        std::string chunk;
        auto onHeader = [&](std::uint64_t size, std::string_view extensions, sys::error_code& ec) {
            fmt::print(stderr, "Server: Header chunk ({})\n", size);
            chunk.reserve(size);
            chunk.clear();
        };
        auto onBody = [&](std::uint64_t remain, std::string_view body, sys::error_code& ec) {
            fmt::print(stderr, "Server: Body chunk ({})\n", body.size());
            if (remain == body.size()) {
                ec = http::error::end_of_chunk;
            }
            chunk.append(body.data(), body.size());
            return body.size();
        };
        reqPar.on_chunk_header(onHeader);
        reqPar.on_chunk_body(onBody);

        sys::error_code ec;
        while (!reqPar.is_done()) {
            fmt::print(stderr, "Server: Read chunk\n");
            co_await http::async_read(
                _stream, buffer, reqPar, io::redirect_error(io::use_awaitable, ec));
            if (not ec) {
                continue;
            } else {
                if (ec != http::error::end_of_chunk) {
                    fmt::print(stderr, "Error: {}\n", ec.message());
                    break;
                } else {
                    fmt::print(stderr, "End of chunk\n");
                    ec = {};
                }
            }
            fmt::print(stderr, "Received chunk: {}\n", chunk);
            co_await channel.async_send(sys::error_code{}, io::buffer(chunk), io::use_awaitable);
        }

        co_await channel.async_send(http::error::end_of_stream, {}, io::use_awaitable);
        channel.close();

        _stream.close();
        co_return;
    }

    io::awaitable<void>
    consumer(Channel& channel)
    {
        std::string chunks;
        sys::error_code ec;
        while (channel.is_open()) {
            auto buffer = co_await channel.async_receive(io::redirect_error(io::use_awaitable, ec));
            if (ec) {
                if (ec != http::error::end_of_stream) {
                    fmt::print(stderr, "Error: {}\n", ec.message());
                }
                break;
            }
            chunks.append(static_cast<const char*>(buffer.data()), buffer.size());
        }
        fmt::print(stderr, "Chunks: {}\n", chunks);
    }

private:
    beast::tcp_stream _stream;
};

Server::Server(io::any_io_executor executor)
    : _executor{std::move(executor)}
{
}

void
Server::listen(io::ip::port_type port)
{
    listen(tcp::endpoint{tcp::v4(), port});
}

void
Server::listen(tcp::endpoint endpoint)
{
    io::co_spawn(
        _executor, listener(std::move(endpoint)), io::consign(io::detached, shared_from_this()));
}

io::awaitable<void>
Server::listener(tcp::endpoint endpoint)
{
    tcp::acceptor acceptor{co_await io::this_coro::executor, endpoint};
    for (;;) {
        tcp::socket socket = co_await acceptor.async_accept(io::use_awaitable);
        std::make_shared<Session>(std::move(socket))->run();
    }
}
