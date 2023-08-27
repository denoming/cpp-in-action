#include "Client.hpp"
#include "Server.hpp"

#include <boost/program_options.hpp>

#include <string>

namespace po = boost::program_options;

static const char* DefaultHost{"127.0.0.1"};
static const char* DefaultPort{"8080"};

int
main(int argc, char* argv[])
{
    bool runClient{false};
    bool runServer{false};
    std::string host;
    std::string port;
    std::string data;

    po::options_description desc("Allowed options");
    // clang-format off
    desc.add_options()
        ("client,c", po::bool_switch(&runClient), "Run client")
        ("server,s", po::bool_switch(&runServer), "Run server")
        ("host,h", po::value<std::string>(&host)->default_value(DefaultHost), "Set host")
        ("port,p", po::value<std::string>(&port)->default_value(DefaultPort), "Set port")
        ("data,d", po::value<std::string>(&data), "Set data")
        ;
    // clang-format on

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (!runClient && !runServer) {
        return EXIT_FAILURE;
    }

    io::io_context context;
    if (runClient) {
        auto client = std::make_shared<Client>(context.get_executor(), host, port, data);
        client->send();
        context.run();
    }
    if (runServer) {
        auto server = std::make_shared<Server>(context.get_executor());
        server->listen(8080);
        context.run();
    }
    return EXIT_SUCCESS;
}
