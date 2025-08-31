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

#pragma once

#include "Common.hpp"

#include <vector>
#include <thread>

class Runner {
public:
    void
    run(std::size_t threadsNum);

    [[nodiscard]] asio::io_context&
    context();

    [[nodiscard]] const asio::io_context&
    context() const;

private:
    void
    waitForTermination();

    void
    spawnThreads(std::size_t threadsNum);

private:
    asio::io_context _context;
    std::vector<std::jthread> _threads;
};

//
// Inlines
//

inline asio::io_context&
Runner::context()
{
    return _context;
}

inline const asio::io_context&
Runner::context() const
{
    return _context;
}