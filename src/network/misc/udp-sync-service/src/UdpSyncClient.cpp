#include "UdpSyncClient.h"

#include <vector>

UdpSyncClient::UdpSyncClient()
    : _socket{_context}
{
    _socket.open(net::ip::udp::v4());
}

std::string
UdpSyncClient::communicate(std::chrono::seconds duration,
                           std::string_view address,
                           net::ip::port_type port)
{
    const std::string request{"EMULATE " + std::to_string(duration.count()) + "\n"};
    net::ip::udp::endpoint endpoint{net::ip::make_address(address), port};
    sendRequest(endpoint, request);
    return receiveResponse(endpoint);
}

void
UdpSyncClient::sendRequest(const net::ip::udp::endpoint& endpoint, const std::string& request)
{
    _socket.send_to(net::buffer(request), endpoint);
}

std::string
UdpSyncClient::receiveResponse(net::ip::udp::endpoint& endpoint)
{
    std::vector<char> buffer;
    _socket.receive_from(net::buffer(buffer), endpoint);
    _socket.shutdown(net::socket_base::shutdown_both);
    return std::string{buffer.begin(), buffer.end()};
}
