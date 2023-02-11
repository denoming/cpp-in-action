#include <gtest/gtest.h>

#include <boost/asio.hpp>

#include <iostream>

namespace asio = boost::asio;
namespace sys = boost::system;

using tcp = asio::ip::tcp;

static std::string_view Host{"www.google.com"};
static std::string_view Port{"80"};

TEST(Resolver, Resolve)
{
    asio::io_context context;
    asio::ip::tcp::resolver resolver{context};
    sys::error_code error;
    const auto results = resolver.resolve(Host, Port, error);
    if (error) {
        std::cerr << "Something went wrong" << std::endl;
    } else {
        for (const auto& result : results) {
            std::cout << result.endpoint() << std::endl;
        }
    }
}

TEST(Resolver, AsyncResolve)
{
    asio::io_context context;
    asio::ip::tcp::resolver resolver{context};

    resolver.async_resolve(
        Host, Port, [](const sys::error_code& error, const tcp::resolver::results_type& results) {
            if (error) {
                std::cerr << "Something went wrong" << std::endl;
            } else {
                for (const auto& result : results) {
                    std::cout << result.endpoint() << std::endl;
                }
            }
        });

    context.run();
}