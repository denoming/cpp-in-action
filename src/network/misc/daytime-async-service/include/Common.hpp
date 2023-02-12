#pragma once

#include <boost/asio.hpp>

namespace asio = boost::asio;
namespace sys = boost::system;
using tcp = asio::ip::tcp;

#define HANDLER_LOCATION \
    BOOST_ASIO_HANDLER_LOCATION((__FILE__, __LINE__, BOOST_CURRENT_FUNCTION))
