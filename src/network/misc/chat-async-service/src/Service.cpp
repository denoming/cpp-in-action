#include "Common.hpp"

#include "ChatServer.hpp"

int
main()
{
    asio::io_context context;
    ChatServer server{context, 8080};
    server.listen();
    context.run();
    return EXIT_SUCCESS;
}