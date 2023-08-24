#pragma once

#include <boost/asio.hpp>

namespace net = boost::asio;

class TcpSyncClient {
public:
    TcpSyncClient(std::string_view address, net::ip::port_type port);

    void
    connect();

    void
    close();

    [[nodiscard]] std::string
    communicate();

private:
    void
    sendRequest(const std::string& request);

    std::string
    receiveResponse();

private:
    net::io_context _context;
    net::ip::tcp::endpoint _endpoint;
    net::ip::tcp::socket _socket;
};