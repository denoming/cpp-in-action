#include "TcpSession.hpp"

#include <iostream>

#include <ctime>

namespace {

std::string
getDayTime()
{
    std::time_t now = std::time(nullptr);
    return std::ctime(&now);
}

} // namespace

TcpSession::TcpSession(net::io_context& context)
    : _socket{context}
{
}

TcpSession::Ptr
TcpSession::create(net::io_context& context)
{
    return std::shared_ptr<TcpSession>(new TcpSession{context});
}

net::ip::tcp::socket&
TcpSession::socket()
{
    return _socket;
}

const net::ip::tcp::socket&
TcpSession::socket() const
{
    return _socket;
}

void
TcpSession::process()
{
    _message.assign(getDayTime());

    HANDLER_LOCATION;

    net::async_write(_socket,
                     net::buffer(_message),
                     [self = shared_from_this()](sys::error_code ec, std::size_t bytesWritten) {
                         self->onWriteDone(ec, bytesWritten);
                     });
}

void
TcpSession::onWriteDone(sys::error_code ec, std::size_t bytesWritten)
{
    HANDLER_LOCATION;

    if (ec) {
        std::cerr << "onWriteDone: " << ec.what();
    } else {
        std::cout << "onWriteDone: " << bytesWritten << " bytes written" << std::endl;
    }
}
