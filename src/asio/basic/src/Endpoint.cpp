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

#include <boost/asio.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace testing;
using namespace boost;

TEST(Endpoint, Client)
{
    /* IPv4 address and port to connect */
    static const std::string_view RawIpAddress{"127.0.0.1"};
    static const asio::ip::port_type Port{3333};

    system::error_code ec;
    auto ipaddr = asio::ip::make_address(RawIpAddress.data(), ec);
    ASSERT_FALSE(ec);

    asio::ip::tcp::endpoint clientEndpoint{ipaddr, Port};
}

TEST(Endpoint, Server)
{
    /* IPv6 address and port to listen */
    static const auto IpAddress{asio::ip::address_v6::any()};
    static const asio::ip::port_type Port{3333};

    asio::ip::tcp::endpoint serverEndpoint{IpAddress, Port};
}