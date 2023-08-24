#pragma once

#include <boost/asio.hpp>

namespace net = boost::asio;
namespace sys = boost::system;

#include <memory>

class TcpAsyncService : public std::enable_shared_from_this<TcpAsyncService> {
public:
    explicit TcpAsyncService(net::ip::tcp::socket&& socket);

    void
    handle();

private:
    void
    onReadDone(const sys::error_code& ec, std::size_t bytesRead);

    void
    onWriteDone(const sys::error_code& ec, std::size_t bytesWritten);

private:
    net::ip::tcp::socket _socket;
    net::streambuf _buffer;
};