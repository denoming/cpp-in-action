#include "TcpEchoServer.hpp"

int
main()
{
    asio::io_context context;
    TcpEchoServer server{context, 8080};
    context.run();
    return EXIT_SUCCESS;
}