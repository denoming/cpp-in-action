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
#include <string>
#include <thread>
#include <vector>
#include <cmath>

using namespace testing;

//--------------------------------------------------------------------------------------------------

class Data {
public:
    explicit Data(std::string name)
        : _name{std::move(name)}
    {
    }

    [[nodiscard]] std::thread // Using 'nodiscard' attribute, which warn if you miss return value
    startThreadWithCopyOfThis()
    {
        using namespace std::literals;
        return std::thread([*this]() {
            std::this_thread::sleep_for(3s);
            std::cout << "Name: " << _name << std::endl;
        });
    }

    [[deprecated]] std::thread // Using 'deprecated' attribute (warn if you are trying to use it)
    startThreadWithCopyOfThis([[maybe_unused]] int timeout)
    {
        return std::thread{};
    }

private:
    std::string _name;
};

TEST(Attributes, NoDiscard)
{
    std::thread t;
    {
        Data d{"Denys"};
        t = d.startThreadWithCopyOfThis();
    }
    if (t.joinable()) {
        t.join();
    }
}

//--------------------------------------------------------------------------------------------------

enum class [[nodiscard]] ErrorCode { Ok, Failed };

ErrorCode
errorProneFunction()
{
    return ErrorCode::Failed;
}

TEST(Attributes, NoDiscardWithEnum)
{
    // Warning: ignoring returned value of type ‘ErrorCode’
    // errorProneFunction();

    EXPECT_EQ(errorProneFunction(), ErrorCode::Failed);
}

//--------------------------------------------------------------------------------------------------

struct MyType {
    /* */
    [[nodiscard("Implicit destroying of temporary object")]] MyType(int, bool)
    {
    }
};

TEST(Attributes, NoDiscardWithConstructor)
{
    // Warning: ignoring return value of ‘MyType::MyType(int, bool)’
    // MyType{5, true};
}

//--------------------------------------------------------------------------------------------------

TEST(Attributes, LikelyNoLikely)
{
    std::vector<double> numbers{-1.0, -3.0, -5.0, -7.0};

    double sum{0};
    for (double number : numbers) {
        if (number < 0) [[likely]] { // Specify that this branch is more likely
            sum -= std::sqrt(-number);
        } else {
            sum += std::sqrt(number);
        }
    }
}

//--------------------------------------------------------------------------------------------------

struct Empty { };

struct NoUniqueAddress {
    int d{};
    [[no_unique_address]] Empty e{}; // Do not allocate dedicated memory address fo class member
};

struct UniqueAddress {
    int d{};
    Empty e{};
};

TEST(Attributes, NoUniqueAddress)
{
    EXPECT_EQ(sizeof(int), sizeof(NoUniqueAddress));
    EXPECT_NE(sizeof(int), sizeof(UniqueAddress));

    NoUniqueAddress noUnique;
    std::cout << "&NoUnique.d: " <<  &noUnique.d << '\n';
    std::cout << "&NoUnique.e: " <<  &noUnique.e << '\n';

    UniqueAddress unique;
    std::cout << "&unique.d: " <<  &unique.d << '\n';
    std::cout << "&unique.e: " <<  &unique.e << '\n';

    EXPECT_EQ(static_cast<void*>(&noUnique.d), &noUnique.e);
    EXPECT_NE(static_cast<void*>(&unique.d), &unique.e);
}
