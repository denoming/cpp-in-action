#pragma once

#include <boost/asio.hpp>

#include <atomic>
#include <thread>

namespace net = boost::asio;

class TcpSyncServer {
public:
    TcpSyncServer();

    void start(net::ip::port_type port);

    void stop();

private:
    void run(net::ip::port_type port);

private:
    net::io_context _context;
    std::atomic<bool> _stop;
    std::thread _thread;

};