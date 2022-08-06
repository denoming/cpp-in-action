#pragma once

#include "TcpAsyncAcceptor.h"

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>

#include <thread>
#include <memory>

namespace net = boost::asio;
namespace sys = boost::system;

class TcpAsyncServer final : boost::noncopyable {
public:
    explicit TcpAsyncServer(net::io_context& context);

    ~TcpAsyncServer();

    void
    start(net::ip::port_type port,
          std::size_t numberOfThread = std::thread::hardware_concurrency());

    void
    stop();

private:
    net::io_context& _context;
    std::vector<std::thread> _threads;
    std::unique_ptr<TcpAsyncAcceptor> _acceptor;
};