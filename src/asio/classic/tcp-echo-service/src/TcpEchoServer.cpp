#include "TcpEchoServer.hpp"

#include "TcpEchoSession.hpp"

TcpEchoServer::TcpEchoServer(asio::io_context& context, std::uint16_t port)
    : _context{context}
    , _acceptor{context, tcp::endpoint{tcp::v4(), port}}
{
    accept();
}

void
TcpEchoServer::accept()
{
    _socket.emplace(_context);

    _acceptor.async_accept(*_socket, [this](const sys::error_code& error) {
        std::make_shared<TcpEchoSession>(std::move(*_socket))->start();
        accept();
    });
}
