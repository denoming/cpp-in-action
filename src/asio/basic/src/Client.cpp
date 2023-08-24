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

/* Custom matcher for .async_read_until() */
class MyMatcher {
public:
    template<typename Iterator>
    std::pair<Iterator, bool>
    operator()(Iterator begin, Iterator end) const
    {
        std::size_t cnt{0};

        Iterator it = begin;
        while (it != end && cnt < 3) {
            if (*it++ == 'X') {
                cnt++;
            }
        }

        if (cnt == 3) {
            return {it, true};
        } else {
            return {end, false};
        }
    }
};

namespace boost::asio {
template<>
struct is_match_condition<MyMatcher> : public boost::true_type { };
} // namespace boost::asio

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

    asio::streambuf buffer{1024 /* Prevent growing buffer too much */};
    const std::string request = "GET / HTTP/1.1\n"
                                "Host: 127.0.0.1\n"
                                "Connection: close\n\n";

    const auto onRead = [&](const sys::error_code& error, std::size_t bytesTransferred) {
        std::cout << ">>> Read: " << error.message() << ", bytes = " << bytesTransferred << "\n\n";
        if (!error || error == asio::error::eof) {
            std::cout << std::istream{&buffer}.rdbuf();
            /* Do .consume() after we read prepared data */
            buffer.consume(bytesTransferred);
        }
    };
    const auto onWrite = [&](const sys::error_code& error, std::size_t bytesTransferred) {
        std::cout << ">>> Write: " << error.message() << ", bytes = " << bytesTransferred
                  << std::endl;
        if (!error) {
            /* Read data into dynamic buffer (.prepare() + .commit() will be made internally) */
            asio::async_read_until(socket, buffer, MyMatcher{}, onRead);
        }
    };
    const auto onConnect = [&](const sys::error_code& error, const tcp::endpoint& endpoint) {
        std::cout << ">>> Connect: " << error.message() << ", endpoint = " << endpoint << std::endl;
        if (!error) {
            /* Do .prepare() + .commit() steps to prepare data to write into socket */
            auto view = buffer.prepare(256);
            std::memcpy(view.data(), request.data(), request.size());
            buffer.commit(request.size());
            /**
             * Write data into socket (.consume() will be made internally).
             *
             * Three default completion conditions are available:
             *  + transfer_all
             *  + transfer_at_least
             *  + transfer_exactly
             * Custom completion condition can be made.
             */
            asio::async_write(socket, buffer, asio::transfer_all(), onWrite);
        }
    };
    const auto onResolve
        = [&](const sys::error_code& error, const tcp::resolver::results_type& endpoints) {
              std::cout << ">>> Resolve: " << error.message() << std::endl;
              if (!error) {
                  asio::async_connect(socket, endpoints, onConnect);
              }
          };

    tcp::resolver resolver{context};
    resolver.async_resolve("127.0.0.1", "8080", onResolve);

    context.run();
}
