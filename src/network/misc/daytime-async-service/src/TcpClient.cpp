#include "TcpClient.hpp"

namespace {

std::string
parseResponse(net::streambuf& buffer)
{
    std::string dayTime;
    std::istream is{&buffer};
    std::getline(is, dayTime);
    return dayTime;
}

} // namespace

TcpClient::TcpClient(net::io_context& context)
    : _socket{context}
{
}

void
TcpClient::connect(std::string_view address, std::string_view port)
{
    net::ip::tcp::resolver resolver{_context};
    const auto endpoints = resolver.resolve(address, port);
    if (endpoints.empty()) {
        throw std::runtime_error{"No address has been resolved"};
    }
    net::connect(_socket, endpoints);
}

std::string
TcpClient::get()
{
    sys::error_code ec;
    net::streambuf buffer;
    net::read(_socket, buffer, ec);
    if (ec.failed() && ec != net::error::eof) {
        throw sys::system_error{ec};
    }
    return parseResponse(buffer);
}
