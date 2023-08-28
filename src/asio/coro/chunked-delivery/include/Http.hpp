#pragma once

#include <boost/asio.hpp>
#include <boost/beast.hpp>

#include <boost/asio/experimental/channel.hpp>
#include <boost/asio/experimental/concurrent_channel.hpp>

namespace io = boost::asio;
namespace ioe = boost::asio::experimental;
namespace sys = boost::system;
namespace beast = boost::beast;
namespace http = beast::http;

using tcp = boost::asio::ip::tcp;

const int kHttpVersion11 = 11;