#include "TcpSession.hpp"

#include <boost/current_function.hpp>

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

TcpSession::TcpSession(tcp::socket&& socket)
    : _socket{std::move(socket)}
{
}

void
TcpSession::start()
{
    _message.assign(getDayTime());

    HANDLER_LOCATION;

    asio::async_write(_socket,
                      asio::buffer(_message),
                      std::bind_front(&TcpSession::onWriteDone, shared_from_this()));
}

void
TcpSession::onWriteDone(const sys::error_code& error, std::size_t bytesWritten)
{
    HANDLER_LOCATION;

    if (error) {
        std::cerr << "onWriteDone: " << error.message();
    } else {
        std::cout << "onWriteDone: " << bytesWritten << " bytes written" << std::endl;
    }
}
