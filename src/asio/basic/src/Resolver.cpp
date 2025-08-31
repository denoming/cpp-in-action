// Copyright 2025 Denys Asauliak
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <gtest/gtest.h>

#include <boost/asio.hpp>

#include <iostream>

namespace asio = boost::asio;
namespace sys = boost::system;

using tcp = asio::ip::tcp;

static std::string_view Host{"www.google.com"};
static std::string_view Port{"80"};

TEST(Resolver, Resolve)
{
    asio::io_context context;
    asio::ip::tcp::resolver resolver{context};
    sys::error_code error;
    const auto results = resolver.resolve(Host, Port, error);
    if (error) {
        std::cerr << "Something went wrong" << std::endl;
    } else {
        for (const auto& result : results) {
            std::cout << result.endpoint() << std::endl;
        }
    }
}

TEST(Resolver, AsyncResolve)
{
    asio::io_context context;
    asio::ip::tcp::resolver resolver{context};

    resolver.async_resolve(
        Host, Port, [](const sys::error_code& error, const tcp::resolver::results_type& results) {
            if (error) {
                std::cerr << "Something went wrong" << std::endl;
            } else {
                for (const auto& result : results) {
                    std::cout << result.endpoint() << std::endl;
                }
            }
        });

    context.run();
}