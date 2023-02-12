#include "TcpClient.hpp"

#include <boost/asio/use_future.hpp>

#include <iostream>

namespace {

std::string
parseResponse(asio::streambuf& buffer)
{
    std::string dayTime;
    std::istream is{&buffer};
    std::getline(is, dayTime);
    return dayTime;
}

} // namespace

TcpClient::TcpClient(asio::io_context& context)
    : _socket{context}
{
}

void
TcpClient::connect(std::string_view address, std::string_view port)
{
    tcp::resolver resolver{_context};
    const auto endpoints = resolver.resolve(address, port);
    if (endpoints.empty()) {
        throw std::runtime_error{"No address has been resolved"};
    }

    auto endpoint = asio::async_connect(_socket, endpoints, asio::use_future);
    std::cout << "[TcpClient] connect...";
    endpoint.wait();
    std::cout << "done" << std::endl;
}

std::string
TcpClient::get()
{
    asio::streambuf buffer;
    auto length = asio::async_read_until(_socket, buffer, '\n', asio::use_future);
    std::cout << "[TcpClient] read...";
    length.wait();
    std::cout << length.get() << " bytes" << std::endl;
    return parseResponse(buffer);
}
