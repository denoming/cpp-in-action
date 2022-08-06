#include "TcpAsyncAcceptor.h"

#include "TcpAsyncService.h"

#include <iostream>

TcpAsyncAcceptor::TcpAsyncAcceptor(net::io_context& context, net::ip::port_type port)
    : _stop{false}
    , _context{context}
    , _acceptor{context, net::ip::tcp::endpoint{net::ip::tcp::v4(), port}}
{
}

void
TcpAsyncAcceptor::start()
{
    _stop = false;
    _acceptor.listen();
    waitConnection();
}

void
TcpAsyncAcceptor::stop()
{
    _stop = true;

    sys::error_code ec;
    _acceptor.cancel(ec);
    if (ec) {
        std::cerr << "AsyncTcpAcceptor::stop: " << ec.what() << std::endl;
    }
    _acceptor.close(ec);
    if (ec) {
        std::cerr << "AsyncTcpAcceptor::stop: " << ec.what() << std::endl;
    }
}

void
TcpAsyncAcceptor::waitConnection()
{
    auto socket = std::make_shared<net::ip::tcp::socket>(_context);
    _acceptor.async_accept(*socket,
                           [this, socket](sys::error_code ec) { onAcceptDone(ec, socket); });
}

void
TcpAsyncAcceptor::onAcceptDone(sys::error_code ec, std::shared_ptr<net::ip::tcp::socket> socket)
{
    if (ec) {
        std::cerr << "onAcceptDone: " << ec.what() << std::endl;
    } else {
        (new TcpAsyncService(std::move(socket)))->handle();
    }

    if (_stop) {
        _acceptor.close();
    } else {
        waitConnection();
    }
}
