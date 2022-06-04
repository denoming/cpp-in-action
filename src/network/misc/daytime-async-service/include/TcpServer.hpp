#pragma once

#include "Common.hpp"
#include "TcpSession.hpp"

class TcpServer {
public:
    explicit TcpServer(net::io_context& context, net::ip::port_type port = 13);

private:
    void
    waitConnection();

    void
    onAcceptDone(TcpSession::Ptr connection, sys::error_code ec);

private:
    net::io_context& _context;
    net::ip::tcp::acceptor _acceptor;
};