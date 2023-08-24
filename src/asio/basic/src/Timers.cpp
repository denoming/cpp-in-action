#include <gtest/gtest.h>

#include <boost/asio.hpp>

#include <iostream>

using namespace std::literals;

namespace asio = boost::asio;
namespace sys = boost::system;

static void
schedule(asio::io_context& context, std::size_t count)
{
    static asio::high_resolution_timer timer{context};
    timer.expires_after(1s);
    timer.async_wait([&context, count](const sys::error_code& error) {
        std::cout << "Tick: " << count << std::endl;
        if (count > 1) {
            schedule(context, count - 1);
        }
    });
}

TEST(Timers, OneShot)
{
    asio::io_context context;

    schedule(context, 2);

    context.run();
}