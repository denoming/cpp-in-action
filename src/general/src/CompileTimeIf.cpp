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

#include <gmock/gmock.h>

#include <iostream>
#include <functional>
#include <type_traits>

template<typename C, typename... Args>
decltype(auto)
call(C c, Args&&... args)
{
    if constexpr (std::is_void_v<std::invoke_result<C, Args...>>) {
        c(std::forward<Args>(args)...);
    } else {
        return c(std::forward<Args>(args)...);
    }
}

TEST(CompileTimeIfTest, Test1)
{
    auto vCall = [](std::string message) { std::cout << "Message: " << message << std::endl; };

    auto nCall = [](std::string message) -> int {
        std::cout << "Message: " << message << std::endl;
        return 0;
    };

    call(vCall, "Hello from void");
    EXPECT_EQ(call(nCall, "Hello from int"), 0);
}
