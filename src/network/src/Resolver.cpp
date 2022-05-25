#include <boost/asio.hpp>

#include <gtest/gtest.h>

#include <iostream>

using namespace boost;

TEST(Resolver, Resolve)
{
    static std::string_view Host{"www.google.com"};
    static std::string_view Port{"80"};

    asio::io_context ioc;
    asio::ip::tcp::resolver resolver{ioc};
    system::error_code ec;
    const auto results = resolver.resolve( Host, Port, ec);
    if (!ec) {
        for (const auto& result : results) {
            std::cout << result.endpoint() << std::endl;
        }
    }
}
