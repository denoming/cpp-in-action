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

#include "Runner.hpp"

#include <cstdio>

void
Runner::run(std::size_t threadsNum)
{
    spawnThreads(threadsNum);
    waitForTermination();
}

void
Runner::waitForTermination()
{
    asio::signal_set signals{_context, SIGINT, SIGTERM};
    signals.async_wait([this](const sys::error_code& error, int signal) {
        if (!error) {
            _context.stop();
        }
    });
    _context.run();
}

void
Runner::spawnThreads(std::size_t threadsNum)
{
    assert(threadsNum >= 2);
    for (std::size_t n{1}; n <= threadsNum; ++n) {
        std::printf("Spawn <%zu> thread\n", n);
        _threads.emplace_back([this]() { _context.run(); });
    }
}