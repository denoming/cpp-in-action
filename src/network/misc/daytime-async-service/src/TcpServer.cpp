#include "TcpServer.hpp"

TcpServer::TcpServer(net::io_context& context, net::ip::port_type port)
    : _context{context}
    , _acceptor{context, net::ip::tcp::endpoint{net::ip::tcp::v4(), port}}
{
    waitConnection();
}

void
TcpServer::waitConnection()
{
    auto connection = TcpSession::create(_context);
    _acceptor.async_accept(connection->socket(), [this, connection](sys::error_code ec) {
        onAcceptDone(connection, ec);
    });
}

void
TcpServer::onAcceptDone(TcpSession::Ptr connection, sys::error_code ec)
{
    if (!ec) {
        connection->process();
    }
    waitConnection();
}