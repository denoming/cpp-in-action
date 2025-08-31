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

#include <string>
#include <memory>

class Client : public std::enable_shared_from_this<Client> {
public:
    explicit Client(io::any_io_executor executor,
                    std::string host,
                    std::string port,
                    std::string data,
                    std::size_t step = 5);

    void
    send();

private:
    io::awaitable<void>
    doSend();

private:
    std::string _host;
    std::string _port;
    std::string _data;
    std::size_t _step;
    io::any_io_executor _executor;
};
