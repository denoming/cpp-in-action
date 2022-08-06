#pragma once

#include <boost/asio.hpp>

namespace net = boost::asio;

class TcpSyncService {
public:
    TcpSyncService() = default;

    void
    handle(net::ip::tcp::socket& socket);
};