#pragma once

#include <boost/asio.hpp>

#include <atomic>
#include <optional>

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
    waitForConnection();

    void
    onAcceptDone(const sys::error_code& ec);

private:
    std::atomic<bool> _stop;
    net::io_context& _context;
    net::ip::tcp::acceptor _acceptor;
    net::ip::tcp::endpoint _endpoint;
    std::optional<net::ip::tcp::socket> _socket;
};