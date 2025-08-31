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

#include <iomanip>

template<typename T, typename... Types>
void print(T firstArg, Types... args)
{
    std::cout << std::setw(10) << firstArg << ": " << sizeof...(args) << " left\n";
    if constexpr (sizeof...(args) > 0) {
        print(args...);
    }
}

template<typename... Types>
void doublePrint(Types... args)
{
    print(args + args...);
}
template<typename C, typename... Idx>
void indexPrint1(const C& container, Idx... idx)
{
    print(container[idx]...);
}

template<std::size_t... Idx, typename C>
void indexPrint2(const C& container)
{
    print(container[Idx]...);
}

TEST(VariadicTemplate, Print)
{
    print(5.75, "Hi", 5u);
}

TEST(VariadicTemplate, DoublePrint)
{
    doublePrint(1, std::string{"Hi"});
}

TEST(VariadicTemplate, IndexPrint)
{
    using IntVector = std::vector<std::string>;
    IntVector values{"go", "Please", "don't"};
    indexPrint1(values, 1, 2, 0);
    indexPrint2<1, 2, 0>(values);
}

