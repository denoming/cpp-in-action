#include "TcpAsyncService.h"

#include <iostream>

namespace {

std::string
getResponse(net::streambuf& input)
{
    std::istream is{&input};
    std::string request;
    std::getline(is, request);
    if (request == "Ping") {
        return "Pong\n";
    }
    if (request == "Pong") {
        return "Ping\n";
    }
    return "^_^\n";
}

} // namespace

TcpAsyncService::TcpAsyncService(std::shared_ptr<net::ip::tcp::socket> socket)
    : _socket{std::move(socket)}
{
}

void
TcpAsyncService::handle()
{
    net::async_read_until(
        *_socket, _buffer, '\n', [this](sys::error_code ec, std::size_t bytesRead) {
            onReadDone(ec, bytesRead);
        });
}

void
TcpAsyncService::onReadDone(sys::error_code ec, std::size_t /*bytesRead*/)
{
    if (ec) {
        std::cerr << "onReadDone: " << ec.what() << std::endl;
        onFinish();
        return;
    }

    const auto response = process();

    net::async_write(
        *_socket, net::buffer(response), [this](sys::error_code ec, std::size_t bytesWritten) {
            onWriteDone(ec, bytesWritten);
        });
}

void
TcpAsyncService::onWriteDone(sys::error_code ec, std::size_t /*bytesWritten*/)
{
    if (ec) {
        std::cerr << "onWriteDone: " << ec.what() << std::endl;
    }
    onFinish();
}

void
TcpAsyncService::onFinish()
{
    delete this;
}

std::string
TcpAsyncService::process()
{
    return getResponse(_buffer);
}