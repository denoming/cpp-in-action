#include "TcpAsyncService.h"

#include <iostream>
#include <tuple>

namespace {

std::tuple<bool, std::string>
getResponse(net::streambuf& input, std::size_t bytes)
{
    std::istream is{&input};
    std::string request;
    std::getline(is, request);
    if (request == "Ping") {
        return std::make_tuple(true, "Pong\n");
    }
    if (request == "Pong") {
        return std::make_tuple(true, "Ping\n");
    }
    input.consume(bytes);
    return std::make_tuple(false, "");
}

} // namespace

TcpAsyncService::TcpAsyncService(net::ip::tcp::socket&& socket)
    : _socket{std::move(socket)}
    , _buffer{65535} // Set upper bound size for the buffer
{
}

void
TcpAsyncService::handle()
{
    std::cout << "Connection initiated: \n";
    std::cout << "Local  :" << _socket.local_endpoint() << '\n';
    std::cout << "Remote :" << _socket.remote_endpoint() << '\n';

    net::async_read_until(_socket,
                          _buffer,
                          '\n',
                          [self = shared_from_this()](sys::error_code ec, std::size_t bytesRead) {
                              self->onReadDone(ec, bytesRead);
                          });
}

void
TcpAsyncService::onReadDone(const sys::error_code& ec, std::size_t bytesRead)
{
    if (ec) {
        if (ec == net::error::eof) {
            std::cout << "onReadDone: EoS" << std::endl;
        } else {
            std::cerr << "onReadDone: " << ec.what() << std::endl;
        }
    } else {
        const auto [ok, response] = getResponse(_buffer, bytesRead);
        if (ok) {
            net::async_write(
                _socket,
                net::buffer(response),
                [self = shared_from_this()](sys::error_code ec, std::size_t bytesWritten) {
                    self->onWriteDone(ec, bytesWritten);
                });
        } else {
            std::cout << "Invalid request string" << std::endl;
            _socket.close();
        }
    }
}

void
TcpAsyncService::onWriteDone(const sys::error_code& ec, std::size_t /*bytesWritten*/)
{
    if (ec) {
        if (ec == net::error::eof) {
            std::cout << "onWriteDone: EoS" << std::endl;
        } else {
            std::cerr << "onWriteDone: " << ec.what() << std::endl;
        }
    }
}
