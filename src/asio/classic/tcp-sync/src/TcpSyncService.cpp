#include "TcpSyncService.h"

#include <thread>
#include <chrono>
#include <iostream>

namespace sys = boost::system;

namespace {

std::string
getResponse(net::streambuf& input)
{
    std::istream is{&input};
    std::string request;
    std::getline(is, request);
    if (request == "Ping") {
        return "Pong\n";
    }
    if (request == "Pong") {
        return "Ping\n";
    }
    return "^_^\n";
}

} // namespace

void
TcpSyncService::handle(net::ip::tcp::socket& socket)
{
    using namespace std::chrono_literals;

    try {
        net::streambuf buffer;
        net::read_until(socket, buffer, '\n');

        std::this_thread::sleep_for(1s);

        const std::string response{getResponse(buffer)};
        net::write(socket, net::buffer(response));
    } catch (const sys::system_error& e) {
        std::cerr << e.what() << std::endl;
    }
}