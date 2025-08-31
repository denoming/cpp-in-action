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

#include <string>
#include <list>

using namespace testing;

/**
 * Lambda wrapper that provide counting of calls.
 */
template<typename C>
class CountCalls {
public:
    CountCalls(C callable)
        : _callable{std::move(callable)}
        , _count{0}
    {
    }

    template<typename... Args>
    decltype(auto)
    operator()(Args&&... args)
    {
        _count++;
        return _callable(std::forward<Args>(args)...);
    }

    std::size_t
    count() const
    {
        return _count;
    }

private:
    C _callable;
    std::size_t _count;
};

/**
 * Some data
 */
struct Person {
    std::string name;
    std::size_t age;
};

using Persons = std::list<Person>;

TEST(TemplateClassDeductionTest, LambdaWrapper)
{
    // clang-format off
    Persons persons{
        {"John", 21},
        {"Dave", 16},
        {"Kate", 18}
    };

    // Wrap data specific comparator
    CountCalls wrapper{
        [](const Person& p1, const Person& p2) {
            return std::lexicographical_compare(p1.name.cbegin(), p1.name.cend(), p2.name.cbegin(), p2.name.cend());
        }
    };
    // clang-format on

    // Provide wrapped comparator
    persons.sort(std::ref(wrapper));

    EXPECT_THAT(wrapper.count(), Gt(1));
}
