// Copyright 2025 Denys Asauliak
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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