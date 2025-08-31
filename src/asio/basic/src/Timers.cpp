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

using namespace std::literals;

namespace asio = boost::asio;
namespace sys = boost::system;

static void
schedule(asio::io_context& context, std::size_t count)
{
    static asio::high_resolution_timer timer{context};
    timer.expires_after(1s);
    timer.async_wait([&context, count](const sys::error_code& error) {
        std::cout << "Tick: " << count << std::endl;
        if (count > 1) {
            schedule(context, count - 1);
        }
    });
}

TEST(Timers, OneShot)
{
    asio::io_context context;

    schedule(context, 2);

    context.run();
}