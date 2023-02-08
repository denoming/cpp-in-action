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

    try {
        _acceptor.open(_endpoint.protocol());
        _acceptor.set_option(net::ip::tcp::acceptor::reuse_address{true});
        _acceptor.bind(_endpoint);
        _acceptor.listen();
    } catch (const sys::system_error& e) {
        std::cerr << "start: " << e.what() << std::endl;
    }

    waitForConnection();
}

void
TcpAsyncAcceptor::stop()
{
    _stop = true;

    sys::error_code ec;
    if (_acceptor.cancel(ec); ec) {
        std::cerr << "stop(cancel): " << ec.what() << std::endl;
    }
    if (_acceptor.close(ec); ec) {
        std::cerr << "stop(close): " << ec.what() << std::endl;
    }
}

void
TcpAsyncAcceptor::waitForConnection()
{
    /* Create socket fo new connection */
    _socket.emplace(_context);

    /* Accept incoming connection asynchronously */
    _acceptor.async_accept(*_socket, std::bind_front(&TcpAsyncAcceptor::onAcceptDone, this));
}

void
TcpAsyncAcceptor::onAcceptDone(const sys::error_code& ec)
{
    if (ec) {
        std::cerr << "onAcceptDone: " << ec.what() << std::endl;
    } else {
        /* Handle current connection */
        std::make_shared<TcpAsyncService>(std::move(*_socket))->handle();
    }

    if (_stop) {
        /* Close acceptor */
        _acceptor.close();
    } else {
        /* Wait for the next connection */
        waitForConnection();
    }
}
