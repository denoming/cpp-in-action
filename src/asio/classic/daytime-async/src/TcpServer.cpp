#include "TcpServer.hpp"

#include "TcpSession.hpp"

#include <boost/current_function.hpp>

TcpServer::TcpServer(asio::io_context& context, std::uint16_t port)
    : _context{context}
    , _acceptor{context, tcp::endpoint{tcp::v4(), port}}
{
    accept();
}

void
TcpServer::accept()
{
    HANDLER_LOCATION;

    _socket.emplace(_context);

    _acceptor.async_accept(*_socket, [this](const sys::error_code& error) {
        HANDLER_LOCATION;
        if (!error) {
            std::make_shared<TcpSession>(std::move(*_socket))->start();
        }
        accept();
    });
}
