#include "TcpSyncClient.h"

TcpSyncClient::TcpSyncClient(std::string_view address, net::ip::port_type port)
    : _endpoint{net::ip::make_address(address), port}
    , _socket{_context}
{
    _socket.open(_endpoint.protocol());
}

void
TcpSyncClient::connect()
{
    _socket.connect(_endpoint);
}

void
TcpSyncClient::close()
{
    _socket.shutdown(net::socket_base::shutdown_both);
    _socket.close();
}

std::string
TcpSyncClient::communicate()
{
    const std::string request{"Ping\n"};
    sendRequest(request);
    return receiveResponse();
}

void
TcpSyncClient::sendRequest(const std::string& request)
{
    net::write(_socket, net::buffer(request));
}

std::string
TcpSyncClient::receiveResponse()
{
    net::streambuf buffer;
    net::read_until(_socket, buffer, '\n');

    std::string response;
    std::istream is{&buffer};
    std::getline(is, response);
    return response;
}