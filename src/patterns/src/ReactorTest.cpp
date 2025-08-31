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

#include "patterns/Reactor.hpp"

#include <iostream>
#include <chrono>

using namespace std::literals;

TEST(ReactorTest, Reactor)
{
    class MyEventHandler final : public EventHandler {
    public:
        void
        process(Handle handle, short what) final
        {
            std::cout << "Handle Me!\n";
        }
    };

    Reactor reactor;
    reactor.run();
    EXPECT_TRUE(reactor.active());

    auto event = reactor.registerHandler(std::make_shared<MyEventHandler>());
    std::this_thread::sleep_for(100ms);
    reactor.signal(event);
    std::this_thread::sleep_for(100ms);
    reactor.removeHandler(event);
}
