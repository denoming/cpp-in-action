#pragma once

#include <boost/asio.hpp>

namespace net = boost::asio;
namespace sys = boost::system;

#define HANDLER_LOCATION \
    BOOST_ASIO_HANDLER_LOCATION((__FILE__, __LINE__, BOOST_CURRENT_FUNCTION))
