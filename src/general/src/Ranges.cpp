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

#include <iterator>
#include <ranges>
#include <vector>

/*

 Range - a group of items that you can iterate. It provides begin() and end() methods that
         return iterators. Each STL container is a range.

There are multiple refinements of basic range concept:
    * std::ranges::input_range
    * std::ranges::output_range
    * std::ranges::forward_range
    * std::ranges::bidirectional_range
    * std::ranges::random_access_range
    * std::ranges::contiguous_range

 Range concept:
    template<typename T>
    concept range = requires(T& t) {
        ranges::begin(t);
        ranges::end  (t);
    };

 View - a composable adaptation of range where the adaptation happens lazily in pull mode.
        View doesn't hold the data despite range does. Consequently, the containers
        of the STL are ranges but not views.

*/

TEST(Ranges, TransformAndFilter)
{
    std::vector<int> numbers = {1, 2, 3, 4, 5, 6};

    // clang-format off
    auto result = numbers | std::views::filter([](int n) { return (n % 2) == 0; })
                          | std::views::transform([](int n) { return n * 2; });
    // clang-format on

    std::ranges::copy(result, std::ostream_iterator<int>{std::cout, ", "});
}

TEST(Ranges, Grep)
{
    std::map<std::string, int> words{
        {"Witch", 25},
        {"Wizard", 33},
        {"Tale", 35},
    };

    std::cout << "All keys: ";
    std::ranges::copy(std::views::keys(words), std::ostream_iterator<std::string>(std::cout, " "));
    std::cout << '\n';

    std::cout << "All values: ";
    std::ranges::copy(std::views::values(words), std::ostream_iterator<int>(std::cout, " "));
    std::cout << '\n';

    std::cout << "All keys in revers order: ";
    std::ranges::copy(std::views::keys(words) | std::views::reverse,
                      std::ostream_iterator<std::string>(std::cout, " "));
    std::cout << '\n';

    std::cout << "Only two first values: ";
    std::ranges::copy(std::views::keys(words) | std::views::take(2),
                      std::ostream_iterator<std::string>(std::cout, " "));
    std::cout << '\n';
}

TEST(Ranges, Generate)
{
    // Generate five numbers by iterating over infinite iota range factory
    std::ranges::copy(std::views::iota(0) | std::views::take(5),
                      std::ostream_iterator<int>(std::cout, " "));
}

//--------------------------------------------------------------------------------------------------

static bool
isPrime(int i)
{
    for (int j = 2; j * j <= i; ++j) {
        if (i % j == 0) {
            return false;
        }
    }
    return true;
}

static bool
isOdd(int i)
{
    return (i % 2) != 0;
}

TEST(Ranges, Filter)
{
    // clang-format off
    std::ranges::copy(
        /* iota (generate) <= filter (delegate) <= filter (delegate) <= take (pull)  */
        std::views::iota(1'000'000, 1'001'000) | std::views::filter(isOdd)
                                               | std::views::filter(isPrime)
                                               | std::views::take(20) /* Pull 20 values */,
        std::ostream_iterator<int>(std::cout, "\n"));
    // clang-format on
}

//--------------------------------------------------------------------------------------------------

struct Space {
    bool
    operator==(auto pos) const
    {
        return *pos == ' ';
    }
};

struct NegativeNumber {
    bool
    operator==(auto num) const
    {
        return *num < 0;
    }
};

struct Sum {
    void
    operator()(auto n)
    {
        sum += n;
    }
    int sum{0};
};

TEST(Ranges, CustomSentinel)
{
    const char* kText{"Some text with space"};

    // Print 'Some' as the space is considered to be a sentinel
    std::ranges::for_each(kText, Space{}, [](char c) { std::cout << c; });
    std::cout << '\n';

    // Print 'Some' by getting sub-range
    std::ranges::copy(std::ranges::subrange{kText, Space{}},
                      std::ostream_iterator<char>{std::cout});
    std::cout << '\n';
}

TEST(Ranges, CustomAccumulator)
{
    std::vector<int> numbers{4, 10, 33, -5, 10};

    // Iterate over range with accumulator
    auto [tmp1, sum1] = std::ranges::for_each(numbers, Sum{});
    std::cout << "Sum1: " << sum1.sum << '\n';

    // Iterate over range with accumulator and custom sentinel
    auto [tmp2, sum2] = std::ranges::for_each(std::begin(numbers), NegativeNumber{}, Sum{});
    std::cout << "Sum2: " << sum2.sum << '\n';
}

TEST(Ranges, CustomTransform)
{
    std::vector<int> numbers{4, 10, 33, -5, 10};

    std::ranges::transform(std::begin(numbers) /* from */,
                           NegativeNumber{} /* to */,
                           std::begin(numbers) /* destination */,
                           [](auto n) { return n * n; } /* what to do */);

    /* Print: 16, 100, 1089, -5, 10 */
    std::ranges::copy(numbers, std::ostream_iterator<int>{std::cout, ", "});
}

//--------------------------------------------------------------------------------------------------

struct PhoneBookEntry {
    std::string name;
    int number;
};

TEST(Ranges, Projection)
{
    std::vector<PhoneBookEntry> phoneBooks{
        {"Brown", 3},
        {"Smith", 2},
        {"Grimm", 1},
    };

    struct {
        void
        operator()(const std::vector<PhoneBookEntry>& e) const
        {
            std::ranges::for_each(e, [](const PhoneBookEntry& e) {
                std::cout << '(' << e.name << ", " << e.number << ")\n";
            });
            std::cout.flush();
        }
    } printer;

    // Simple projector
    std::ranges::sort(phoneBooks, {}, &PhoneBookEntry::name);
    printer(phoneBooks);
    // Complex projector as lambda
    std::ranges::sort(phoneBooks, {}, [](const PhoneBookEntry& e) { return e.name; });
    printer(phoneBooks);
    // Using custom comparator with projector
    std::ranges::sort(
        phoneBooks,
        [](int e1, int e2) { return e1 < e2; } /* Comparator */,
        [](const PhoneBookEntry& e) { return e.number; } /* Projector */);
    printer(phoneBooks);
}