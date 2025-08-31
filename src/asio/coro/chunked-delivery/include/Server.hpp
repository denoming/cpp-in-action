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

#include "Http.hpp"

#include <memory>

class Server : public std::enable_shared_from_this<Server> {
public:
    explicit Server(io::any_io_executor executor);

    void
    listen(io::ip::port_type port);

    void
    listen(tcp::endpoint endpoint);

private:
    io::awaitable<void>
    listener(tcp::endpoint endpoint);

private:
    io::any_io_executor _executor;
};