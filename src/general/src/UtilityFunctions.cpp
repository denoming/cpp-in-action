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
#include <gmock/gmock.h>

#include <iostream>
#include <iterator>
#include <random>

using namespace testing;

/* (1) For other cases */
template<typename T>
void
printElements(const T& input)
{
    auto size{std::size(input)};
    std::cout << size << " elems: ";
    for (auto it = std::begin(input); it != std::end(input); ++it) {
        std::cout << *it << ' ';
    }
    std::cout << std::endl;
}

/* (2) For inline initializer list */
template<typename T>
void
printElements(const std::initializer_list<T>& input)
{
    auto size{std::size(input)};
    std::cout << size << " elems: ";
    for (auto it = std::begin(input); it != std::end(input); ++it) {
        std::cout << *it << ' ';
    }
    std::cout << std::endl;
}

TEST(UtilityFunctions, Size)
{
    printElements({0, 3, 5, 4} /* (2) Inline initializer list */);

    printElements("hello world" /* (1) Inline string array */);

    std::array arr{27, 3, 5, 8, 7, 12, 22, 0, 55};
    printElements(arr /* (1) */);

    std::vector v{0.0, 8.8, 15.15};
    printElements(v /* (1) */);

    std::initializer_list<std::string> il{"just", "five", "small", "string", "literals"};
    printElements(il /* (1) */);
}

TEST(UtilityFunctions, EverySecond)
{
    std::vector<int> elems{8, 15, 7, 42};

    auto printEverySend = [&elems = std::as_const(elems)]() {
        std::cout << "coll: ";
        for (int elem : elems /* Capture by const above prevent iterate by non-const reference */) {
            std::cout << elem << ' ';
        }
        std::cout << '\n';
    };

    printEverySend();
}

TEST(UtilityFunctions, ClampValues)
{
    const auto predicate = [](auto a, auto b) { return std::abs(a) < std::abs(b); };

    std::vector<int> elems;
    for (int i : {-7, 0, 8, 15}) {
        elems.push_back(std::clamp(i, 5, 13, predicate));
    }

    EXPECT_THAT(elems, ElementsAre(-7, 5, 8, 13));
}

TEST(UtilityFunctions, SampleValues)
{
    std::vector<int> input;
    for (int i = 0; i < 100; ++i) {
        input.push_back(i);
    }

    std::random_device rd;
    std::vector<int> output;
    std::sample(
        input.cbegin(), input.cend(), std::back_insert_iterator{output}, 10, std::mt19937{rd()});

    EXPECT_THAT(output, Not(IsEmpty()));
}

TEST(UtilityFunctions, SampleString)
{
    static const std::size_t kLength{15};

    std::string in{"ABCDEFGHIKLMNOPQRSTVXYZ"}, out;
    std::sample(in.cbegin(),
                in.cend(),
                std::back_inserter(out),
                kLength,
                std::mt19937{std::random_device{}()});

    EXPECT_THAT(out, Not(IsEmpty()));
}
