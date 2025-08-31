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

#include <thread>
#include <iostream>

namespace asio = boost::asio;

using namespace std::literals;

TEST(Context, WorkGuard)
{
    asio::io_context context;

    /* Work guard prevents context to stop when no jobs available */
    auto guard = asio::make_work_guard(context.get_executor());

    std::jthread watchdog{[&]() {
        std::this_thread::sleep_for(3s);
        std::cout << ">>> Stop context" << std::endl;
        /* Stop context as soon as possible */
        context.stop();
    }};

    std::cout << ">>> Run context" << std::endl;
    context.run();
}

TEST(Context, WorkGuardWithComplete)
{
    asio::io_context context;

    /* Work guard prevents context to stop when no jobs available */
    auto guard = asio::make_work_guard(context.get_executor());

    std::jthread watchdog{[&]() {
        std::this_thread::sleep_for(3s);
        std::cout << ">>> Stop context" << std::endl;
        /* Let context stop when no new jobs available */
        guard.reset();
    }};

    std::cout << ">>> Run context" << std::endl;
    context.run();
}

TEST(Context, PostJob)
{
    asio::io_context context;

    asio::post(context, []() {
        /* Do the job inside pooling loop */
        std::cout << ">>> Task (" << std::this_thread::get_id() << ")" << std::endl;
    });

    auto guard = asio::make_work_guard(context);
    std::jthread watchdog{[&]() {
        std::this_thread::sleep_for(3s);
        std::cout << ">>> Stop context" << std::endl;
        /* Let context stop when no new jobs available */
        guard.reset();
    }};

    std::cout << ">>> Run context (" << std::this_thread::get_id() << ")" << std::endl;
    context.run();
}

TEST(Context, ManualPool)
{
    asio::io_context context;

    asio::post(context, []() {
        /* Do the job inside pooling loop */
        std::cout << ">>> Task" << std::endl;
    });

    for (;;) {
        if (context.poll() == 0) {
            /* Do one task and exit */
            std::cout << ">>> Exit" << std::endl;
            break;
        }
    }
}