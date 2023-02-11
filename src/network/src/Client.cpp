#include <gtest/gtest.h>

#include <boost/asio.hpp>

#include <iostream>

using namespace testing;

namespace asio = boost::asio;
namespace ip = asio::ip;
namespace sys = boost::system;

using tcp = asio::ip::tcp;

/* Running test server to connect with
$ tee hello.http > /dev/null <<EOF
HTTP/1.0 200 OK

<html>
    <body>
    <h1>Hello, world!</h1>
    </body>
</html>
XXX
EOF
$ nc -l localhost 8080 < hello.http
*/

static std::string_view Host{"127.0.0.1"};
static std::string_view Port{"80"};

TEST(Client, Connect)
{
    asio::io_context context;
    tcp::socket socket{context};

    ip::address address = ip::make_address("127.0.0.1");
    tcp::endpoint endpoint{address, 8080};

    sys::error_code error;
    socket.connect(endpoint, error);
    if (error) {
        std::cerr << "Something went wrong" << std::endl;
    } else {
        std::cout << "Connection has been established" << std::endl;
    }
}

TEST(Client, AsyncConnect)
{
    asio::io_context context;
    tcp::socket socket{context};
    tcp::resolver resolver{context};

    asio::streambuf buffer;
    const std::string request = "GET / HTTP/1.1\n"
                                "Host: 127.0.0.1\n"
                                "Connection: close\n\n";

    const auto onRead = [&](const sys::error_code& error, std::size_t bytesTransferred) {
        std::cout << ">>> Read: " << error.message() << ", bytes = " << bytesTransferred << "\n\n";
        if (!error || error == asio::error::eof) {
            std::cout << std::istream{&buffer}.rdbuf();
        }
    };
    const auto onWrite = [&](const sys::error_code& error, std::size_t bytesTransferred) {
        std::cout << ">>> Write: " << error.message() << ", bytes = " << bytesTransferred
                  << std::endl;
        if (!error) {
            asio::async_read_until(socket, buffer, "XXX", onRead);
        }
    };
    const auto onConnect = [&](const sys::error_code& error, const tcp::endpoint& endpoint) {
        std::cout << ">>> Connect: " << error.message() << ", endpoint = " << endpoint << std::endl;
        if (!error) {
            asio::async_write(socket, asio::buffer(request), onWrite);
        }
    };
    const auto onResolve
        = [&](const sys::error_code& error, const tcp::resolver::results_type& endpoints) {
              std::cout << ">>> Resolve: " << error.message() << std::endl;
              if (!error) {
                  asio::async_connect(socket, endpoints, onConnect);
              }
          };

    resolver.async_resolve("127.0.0.1", "8080", onResolve);

    context.run();
}