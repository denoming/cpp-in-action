#pragma once

#include <boost/asio.hpp>

#include <atomic>

namespace net = boost::asio;
namespace sys = boost::system;

class TcpAsyncAcceptor {
public:
    TcpAsyncAcceptor(net::io_context& context, net::ip::port_type port);

    void
    start();

    void
    stop();

private:
    void
    waitConnection();

    void
    onAcceptDone(sys::error_code ec, std::shared_ptr<net::ip::tcp::socket> socket);

private:
    std::atomic<bool> _stop;
    net::io_context& _context;
    net::ip::tcp::acceptor _acceptor;
};