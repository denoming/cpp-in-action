#pragma once

#include <boost/asio.hpp>

namespace net = boost::asio;
namespace sys = boost::system;

#include <atomic>
#include <memory>

class TcpAsyncService {
public:
    explicit TcpAsyncService(std::shared_ptr<net::ip::tcp::socket> socket);

    void
    handle();

private:
    void
    onReadDone(sys::error_code ec, std::size_t bytesRead);

    void
    onWriteDone(sys::error_code ec, std::size_t bytesWritten);

    void
    onFinish();

    std::string
    process();

private:
    std::shared_ptr<net::ip::tcp::socket> _socket;
    net::streambuf _buffer;
};