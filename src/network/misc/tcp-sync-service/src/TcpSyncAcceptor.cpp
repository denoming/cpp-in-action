#include "TcpSyncAcceptor.h"

#include "TcpSyncService.h"

TcpSyncAcceptor::TcpSyncAcceptor(net::io_context& context, net::ip::port_type port)
    : _context{context}
    , _acceptor{context, net::ip::tcp::endpoint{net::ip::address_v4::any(), port}}
{
    _acceptor.listen();
}

void
TcpSyncAcceptor::accept()
{
    net::ip::tcp::socket socket{_context};
    _acceptor.accept(socket);
    TcpSyncService service;
    service.handle(socket);
}