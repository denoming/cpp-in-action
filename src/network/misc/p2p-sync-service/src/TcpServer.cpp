#include "TcpServer.hpp"

SyncServer::SyncServer(net::io_context& context)
    : _context{context}
    , _acceptor{context}
    , _socket{context}
{
}

void
SyncServer::listen(net::ip::port_type port)
{
    net::ip::tcp::endpoint endpoint{net::ip::tcp::v4(), port};
    _acceptor.open(endpoint.protocol());
    _acceptor.bind(endpoint);
    _acceptor.listen();
    _socket = _acceptor.accept();
}

void
SyncServer::send(const void* data, std::size_t size)
{
    net::write(_socket, net::buffer(data, size));
}

std::size_t
SyncServer::receive(net::streambuf& buffer)
{
    sys::error_code ec;
    const auto bytesRead = net::read(_socket, buffer, ec);
    if (ec != net::error::eof) {
        throw sys::system_error{ec};
    }
    return bytesRead;
}

void
SyncServer::finalize()
{
    _socket.shutdown(net::socket_base::shutdown_send);
    _socket.close();
}
