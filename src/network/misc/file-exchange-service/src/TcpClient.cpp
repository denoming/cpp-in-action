#include "TcpClient.hpp"

#include <iostream>

TcpClient::TcpClient(net::io_context& context)
    : _context{context}
{
}

void
TcpClient::send(std::string_view host,
                std::string_view port,
                std::string_view message,
                std::size_t step)
{
    assert(!host.empty());
    assert(!port.empty());
    assert(!message.empty());
    assert(step > 0);
    assert(message.size() > step);

    tcp::resolver resolver(_context);
    auto const results = resolver.resolve(host, port);

    std::cout << "Client: Connect to server\n";
    beast::tcp_stream stream(_context);
    stream.connect(results);

    http::request<http::empty_body> req{http::verb::post, "/speech", kHttpVersion11};
    req.set(http::field::host, host);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    req.set(http::field::transfer_encoding, "chunked");
    req.set(http::field::expect, "100-continue");

    std::cout << "Client: Write initial request\n";
    http::request_serializer<http::empty_body, http::fields> hs{req};
    http::write_header(stream, hs);

    http::response<http::empty_body> res;
    beast::flat_buffer buffer;
    std::cout << "Client: Read response to initial request\n";
    http::read(stream, buffer, res);
    if (res.result() != http::status::continue_) {
        std::cout << "Client: 100 continue expected\n";
        stream.close();
        return;
    }

    const std::size_t kStep = 5;
    std::size_t pos = 0;
    while (pos < message.size()) {
        net::const_buffer bf = net::buffer(message.substr(pos, kStep));
        std::cout << "Client: Write chunk\n";
        net::write(stream.socket(), http::make_chunk(bf));
        pos += kStep;
    }
    std::cout << "Client: Write chunk last\n";
    net::write(stream.socket(), http::make_chunk_last());

    std::cout << "Client: Close\n";
    sys::error_code error;
    stream.socket().shutdown(tcp::socket::shutdown_both, error);
}