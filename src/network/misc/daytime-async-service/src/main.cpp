#include "TcpClient.hpp"
#include "TcpServer.hpp"

#include <boost/asio.hpp>
#include <boost/program_options.hpp>

#include <iostream>

namespace net = boost::asio;
namespace po = boost::program_options;

static const char* DefaultHost{"127.0.0.1"};
static const char* DefaultPort{"3333"};

static void
print(net::streambuf& buffer)
{
    std::istream is{&buffer};
    std::string line;
    while (std::getline(is, line)) {
        std::cout << line << std::endl;
    }
}

static void
executeClient(const std::string& host, const std::string& port)
{
    net::io_context context;
    TcpClient client{context};
    client.connect(host, port);
    std::cout << client.get() << std::endl;
}

static void
executeServer(const std::string& port)
{
    net::io_context context;
    TcpServer server{context, static_cast<net::ip::port_type>(std::stoi(port))};
    context.run();
}

int
main(int argc, char* argv[])
{
    bool runClient{false};
    bool runServer{false};
    std::string host;
    std::string port;

    po::options_description desc("Allowed options");
    // clang-format off
    desc.add_options()
        ("client,c", po::bool_switch(&runClient), "Run client")
        ("server,s", po::bool_switch(&runServer), "Run server")
        ("host,h", po::value<std::string>(&host)->default_value(DefaultHost), "Set host")
        ("port,p", po::value<std::string>(&port)->default_value(DefaultPort), "Set port")
        ;
    // clang-format on

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (!runClient && !runServer) {
        return EXIT_FAILURE;
    }

    if (runClient) {
        executeClient(host, port);
    }
    if (runServer) {
        executeServer(port);
    }

    return EXIT_SUCCESS;
}