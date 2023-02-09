#pragma once

#include <boost/asio.hpp>

#include <functional>

namespace asio = boost::asio;
namespace sys = boost::system;
using tcp = asio::ip::tcp;

using MessageHandler = std::function<void(std::string message)>;
using ErrorHandler = std::function<void()>;