#include <boost/asio.hpp>

namespace asio = boost::asio;

/**
 * Simple UDP echo server
 *
 * To send something as a client use following command:
 *  $ nc -u localhost 8080 <Enter>
 *  Hi
 *  Hi
 */

int
main()
{
    std::uint16_t port{8080};

    asio::io_context context;
    asio::ip::udp::endpoint receiver{asio::ip::udp::v4(), port};
    asio::ip::udp::socket socket{context, receiver};

    char buffer[65535];
    while (true) {
        asio::ip::udp::endpoint sender;
        const auto transferred = socket.receive_from(asio::buffer(buffer), sender);
        socket.send_to(asio::buffer(buffer, transferred), sender);
    }

    return EXIT_SUCCESS;
}
