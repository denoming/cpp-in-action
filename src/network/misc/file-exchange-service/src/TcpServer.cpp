#include "TcpServer.hpp"

#include <thread>
#include <iostream>
#include <charconv>

TcpServer::TcpServer(net::io_context& context)
    : _context{context}
{
}

[[noreturn]] void
TcpServer::listen(net::ip::port_type port)
{
    tcp::acceptor acceptor{_context, {net::ip::address_v4::any(), port}};
    for (;;) {
        tcp::socket socket{_context};
        acceptor.accept(socket);
        std::cout << "Server: New connection incoming\n";
        std::thread{&TcpServer::handleSession, this, std::move(socket)}.detach();
    }
}

[[noreturn]] void
TcpServer::listen(std::string_view port)
{
    net::ip::port_type i3port;
    const auto rv = std::from_chars(port.begin(), port.begin() + port.size(), i3port);
    assert(rv.ec == std::errc{});
    assert(i3port > 1024);
    listen(i3port);
}

void
TcpServer::handleSession(TcpServer* /*server*/, tcp::socket&& socket)
{
    beast::tcp_stream stream{std::move(socket)};

    beast::flat_buffer buffer;
    http::request_parser<http::empty_body> reqPar;
    std::cout << "Server: Read initial request\n";
    http::read_header(stream, buffer, reqPar);

    std::cout << "Server: Check initial request field\n";
    if (reqPar.get()[http::field::expect] == "100-continue") {
        http::response<http::empty_body> res{http::status::continue_, kHttpVersion11};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        std::cout << "Server: Write response to initial request\n";
        http::write(stream, res);
    } else {
        std::cout << "Server: 100 continue expected\n";
        stream.close();
        return;
    }

    std::string chunks;
    auto onHeader = [&](std::uint64_t size, std::string_view extensions, sys::error_code& error) {
        std::cout << "Server: Header chunk (" << size << " size)\n";
        chunks.reserve(chunks.size() + size);
    };
    auto onBody = [&](std::uint64_t remain, std::string_view body, sys::error_code& error) {
        std::cout << "Server: Body chunk (" << body.size() << " size)\n";
        chunks.append(body.data(), body.size());
        return body.size();
    };
    reqPar.on_chunk_header(onHeader);
    reqPar.on_chunk_body(onBody);

    sys::error_code error;
    while (!reqPar.is_done()) {
        std::cout << "Server: Read chunk\n";
        http::read(stream.socket(), buffer, reqPar, error);
        if (error) {
            if (error == http::error::end_of_chunk) {
                error = {};
            } else {
                std::cout << "Server: Reading chunk error (" << error.what() << ")\n";
                break;
            }
        } else {
            continue;
        }
    }

    std::cout << "Ready to work with chunks\n";
    std::cout << "> " << chunks << std::endl;

    std::cout << "Server: Close\n";
    stream.socket().shutdown(tcp::socket::shutdown_send);
}