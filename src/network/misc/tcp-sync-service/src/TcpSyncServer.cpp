#include "TcpSyncServer.h"

#include "TcpSyncAcceptor.h"

TcpSyncServer::TcpSyncServer()
    : _stop{false}
{
}

void
TcpSyncServer::start(net::ip::port_type port)
{
    _thread = std::thread{[this, port]() { run(port); }};
}

void
TcpSyncServer::stop()
{
    _stop = true;
    if (_thread.joinable()) {
        _thread.join();
    }
}

void
TcpSyncServer::run(net::ip::port_type port)
{
    TcpSyncAcceptor acceptor{_context, port};
    while (!_stop) {
        acceptor.accept();
    }
}
