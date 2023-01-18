#include "TcpAsyncAcceptor.h"

#include "TcpAsyncService.h"

#include <iostream>

TcpAsyncAcceptor::TcpAsyncAcceptor(net::io_context& context, net::ip::port_type port)
    : _stop{false}
    , _context{context}
    , _acceptor{context}
    , _endpoint{net::ip::tcp::v4(), port}
{
}

void
TcpAsyncAcceptor::start()
{
    _stop = false;

    sys::error_code ec;
    _acceptor.open(_endpoint.protocol(), ec);
    if (ec) {
        std::cerr << "start(open): " << ec.what() << std::endl;
    }

    _acceptor.bind(_endpoint, ec);
    if (ec) {
        std::cerr << "start(bind): " << ec.what() << std::endl;
    }

    _acceptor.listen(net::socket_base::max_connections, ec);
    if (ec) {
        std::cerr << "start(listen): " << ec.what() << std::endl;
    }

    waitForConnection();
}

void
TcpAsyncAcceptor::stop()
{
    _stop = true;

    sys::error_code ec;
    _acceptor.cancel(ec);
    if (ec) {
        std::cerr << "stop(cancel): " << ec.what() << std::endl;
    }

    _acceptor.close(ec);
    if (ec) {
        std::cerr << "close(close): " << ec.what() << std::endl;
    }
}

void
TcpAsyncAcceptor::waitForConnection()
{
    /* Create connection socket and initialize async accept procedure */
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
        /* Handle current connection */
        (new TcpAsyncService(std::move(socket)))->handle();
    }

    if (_stop) {
        /* Close acceptor */
        _acceptor.close();
    } else {
        /* Wait for the next connection */
        waitForConnection();
    }
}
