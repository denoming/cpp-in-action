#pragma once

#include <boost/asio.hpp>

namespace net = boost::asio;
namespace sys = boost::system;

class SyncServer {
public:
    explicit SyncServer(net::io_context& context);

    void
    listen(net::ip::port_type port);

    void
    send(const void* data, std::size_t size);

    std::size_t
    receive(net::streambuf& buffer);

    void
    finalize();

private:
    net::io_context& _context;
    net::ip::tcp::acceptor _acceptor;
    net::ip::tcp::socket _socket;
};
