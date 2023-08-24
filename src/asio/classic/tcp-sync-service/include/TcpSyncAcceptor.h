#pragma once

#include <boost/asio.hpp>

namespace net = boost::asio;

class TcpSyncAcceptor {
public:
    TcpSyncAcceptor(net::io_context& context, net::ip::port_type port);

    void accept();

private:
    net::io_context& _context;
    net::ip::tcp::acceptor _acceptor;
};