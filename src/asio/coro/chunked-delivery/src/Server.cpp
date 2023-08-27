#include "Server.hpp"

#include <fmt/format.h>

class Session : public std::enable_shared_from_this<Session> {
public:
    explicit Session(tcp::socket&& socket)
        : _stream{std::move(socket)}
    {
    }

    void
    run()
    {
        io::co_spawn(
            _stream.get_executor(),
            [self = shared_from_this()]() {
                fmt::print("Session: Spawn coroutine\n");
                return self->doRun();
            },
            io::detached);
    }

private:
    io::awaitable<void>
    doRun()
    {
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
            fmt::print("Server: Header chunk ({})\n", size);
            chunk.reserve(size);
            chunk.clear();
        };
        auto onBody = [&](std::uint64_t remain, std::string_view body, sys::error_code& ec) {
            fmt::print("Server: Body chunk ({})\n", body.size());
            if (remain == body.size()) {
                ec = http::error::end_of_chunk;
            }
            chunk.append(body.data(), body.size());
            return body.size();
        };
        reqPar.on_chunk_header(onHeader);
        reqPar.on_chunk_body(onBody);

        sys::error_code ec;
        std::string chunks;
        while (!reqPar.is_done()) {
            fmt::print("Server: Read chunk\n");
            co_await http::async_read(
                _stream, buffer, reqPar, io::redirect_error(io::use_awaitable, ec));
            if (not ec) {
                continue;
            } else {
                if (ec != http::error::end_of_chunk) {
                    fmt::print("Error: {}\n", ec.message());
                    break;
                } else {
                    fmt::print("End of chunk\n");
                    ec = {};
                }
            }
            fmt::print("Received chunk: {}\n", chunk);
            chunks += chunk;
        }

        fmt::print("Received chunks: {}\n", chunks);
        _stream.close();
        co_return;
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
