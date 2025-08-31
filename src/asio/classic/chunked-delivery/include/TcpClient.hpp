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

#include <string_view>

class TcpClient {
public:
    explicit TcpClient(net::io_context& context);

    void
    send(std::string_view host,
         std::string_view port,
         std::string_view message,
         std::size_t step = 5);

private:
    net::io_context& _context;
};