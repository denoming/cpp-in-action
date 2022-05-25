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
    auto ipaddr = asio::ip::address::from_string(RawIpAddress.data(), ec);
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