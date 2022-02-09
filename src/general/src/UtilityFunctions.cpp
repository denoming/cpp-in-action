#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <iostream>
#include <iterator>
#include <random>

using namespace testing;

/* (1) For other cases */
template<typename T>
void
printElems(const T& input)
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
printElems(const std::initializer_list<T>& input)
{
    auto size{std::size(input)};
    std::cout << size << " elems: ";
    for (auto it = std::begin(input); it != std::end(input); ++it) {
        std::cout << *it << ' ';
    }
    std::cout << std::endl;
}

TEST(UtilityFunctionsTest, Size)
{
    printElems({0, 3, 5, 4} /* (2) Inline initializer list */);

    printElems("hello world" /* (1) Inline string array */);

    std::array arr{27, 3, 5, 8, 7, 12, 22, 0, 55};
    printElems(arr /* (1) */);

    std::vector v{0.0, 8.8, 15.15};
    printElems(v /* (1) */);

    std::initializer_list<std::string> il{"just", "five", "small", "string", "literals"};
    printElems(il /* (1) */);
}

TEST(UtilityFunctionsTest, EverySecond)
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

TEST(UtilityFunctionsTest, ClampValues)
{
    const auto predicate = [](auto a, auto b) { return std::abs(a) < std::abs(b); };

    std::vector<int> elems;
    for (int i : {-7, 0, 8, 15}) {
        elems.push_back(std::clamp(i, 5, 13, predicate));
    }

    EXPECT_THAT(elems, ElementsAre(-7, 5, 8, 13));
}

TEST(UtilityFunctionsTest, SampleValues)
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
