#pragma once

#include <boost/asio.hpp>

#include <chrono>

namespace net = boost::asio;

class UdpSyncClient {
public:
    UdpSyncClient();

    [[nodiscard]] std::string
    communicate(std::chrono::seconds duration,
                std::string_view address,
                net::ip::port_type port);

private:
    void
    sendRequest(const net::ip::udp::endpoint& endpoint,
                const std::string& request);

    std::string
    receiveResponse(net::ip::udp::endpoint& endpoint);

private:
    net::io_context _context;
    net::ip::udp::socket _socket;
};