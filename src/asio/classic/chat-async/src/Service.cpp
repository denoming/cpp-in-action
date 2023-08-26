
#include "Common.hpp"
#include "ChatServer.hpp"
#include "Runner.hpp"

int
main()
{
    Runner runner;
    ChatServer server{runner.context(), 8080};
    server.listen();
    runner.run(4);
    return EXIT_SUCCESS;
}