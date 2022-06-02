#pragma once

#include <boost/asio.hpp>

#include <memory>

namespace net = boost::asio;
namespace sys = boost::system;

class TcpSession : public std::enable_shared_from_this<TcpSession> {
public:
    using Ptr = std::shared_ptr<TcpSession>;

    static Ptr
    create(net::io_context& context);

    [[nodiscard]] net::ip::tcp::socket&
    socket();

    [[nodiscard]] const net::ip::tcp::socket&
    socket() const;

    void
    process();

private:
    explicit TcpSession(net::io_context& context);

    void
    onWriteDone(sys::error_code ec, std::size_t bytesWritten);

private:
    net::ip::tcp::socket _socket;
    std::string _message;
};
