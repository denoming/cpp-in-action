#include "Client.hpp"

#include <fmt/format.h>

Client::Client(io::any_io_executor executor,
               std::string host,
               std::string port,
               std::string data,
               std::size_t step)
    : _executor{std::move(executor)}
    , _host{std::move(host)}
    , _port{std::move(port)}
    , _data{std::move(data)}
    , _step{step}
{
    assert(!_host.empty());
    assert(!_port.empty());
    assert(!_data.empty());
    assert(_step > 0);
    assert(_data.size() > step);
}

void
Client::send()
{
    io::co_spawn(
        _executor, [self = shared_from_this()]() { return self->doSend(); }, io::detached);
}

io::awaitable<void>
Client::doSend()
{
    auto executor = co_await io::this_coro::executor;

    tcp::resolver resolver(executor);
    auto const results = co_await resolver.async_resolve(_host, _port, io::use_awaitable);

    fmt::print("Client: Connect to server\n");
    beast::tcp_stream stream(executor);
    co_await stream.async_connect(results, io::use_awaitable);

    http::request<http::empty_body> req{http::verb::post, "/message", kHttpVersion11};
    req.set(http::field::host, _host);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    req.set(http::field::transfer_encoding, "chunked");
    req.set(http::field::expect, "100-continue");
    {
        fmt::print("Client: Write initial request\n");
        http::request_serializer<http::empty_body, http::fields> reqSer{req};
        co_await http::async_write_header(stream, reqSer, io::use_awaitable);
    }

    fmt::print("Client: Read response to initial request\n");
    beast::flat_buffer buffer;
    http::response<http::empty_body> res;
    co_await http::async_read(stream, buffer, res, io::use_awaitable);
    if (res.result() != http::status::continue_) {
        fmt::print("Client: 100 continue expected\n");
        stream.close();
        co_return;
    }

    std::size_t pos = 0;
    while (pos < _data.size()) {
        const std::string message = _data.substr(pos, _step);
        fmt::print("Client: Write chunk\n");
        const auto chunk = http::make_chunk(io::buffer(message));
        co_await io::async_write(stream.socket(), chunk, io::use_awaitable);
        pos += _step;
    }
    fmt::print("Client: Write last chunk\n");
    co_await io::async_write(stream.socket(), http::make_chunk_last(), io::use_awaitable);

    fmt::print("Client: Close\n");
    stream.close();
}