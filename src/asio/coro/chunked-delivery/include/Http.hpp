#pragma once

#include <boost/asio.hpp>
#include <boost/beast.hpp>

namespace io = boost::asio;
namespace sys = boost::system;
namespace beast = boost::beast;
namespace http = beast::http;

using tcp = boost::asio::ip::tcp;

const int kHttpVersion11 = 11;