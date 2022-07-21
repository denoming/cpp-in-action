#include "TcpClient.hpp"
#include "TcpServer.hpp"

#include <boost/program_options.hpp>

#include <string>

namespace po = boost::program_options;

static const char* DefaultHost{"127.0.0.1"};
static const char* DefaultPort{"3333"};

static void
executeClient(net::io_context& context,
              std::string_view host,
              std::string_view port,
              std::string_view message)
{
    TcpClient client{context};
    client.send(host, port, message);
}

[[noreturn]] static void
executeServer(net::io_context& context, std::string_view port)
{
    TcpServer server{context};
    server.listen(port);
}

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

    net::io_context context;
    if (runClient) {
        executeClient(context, host, port, data);
    }
    if (runServer) {
        executeServer(context, port);
    }
    return EXIT_SUCCESS;
}
