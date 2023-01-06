#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <algorithm>
#include <iostream>

using namespace testing;

//--------------------------------------------------------------------------------------------------

struct MyDouble1 {
    double value;

    explicit constexpr MyDouble1(double value)
        : value{value}
    {
    }

    /**
     * Request the tree-way operator from the compiler with '= default'
     * (all six comparison operators become available: ==, !=, <, <=, >, >=)
     */
    auto
    operator<=>(const MyDouble1& other) const
        = default;
};

TEST(VariantTest, DefaultOperators)
{
    MyDouble1 d1{0.1};
    MyDouble1 d2{0.2};

    EXPECT_THAT(d1 != d2, IsTrue());
    EXPECT_THAT(d1 == d2, IsFalse());
    EXPECT_THAT(d1 < d2, IsTrue());
    EXPECT_THAT(d1 > d2, IsFalse());
}

//--------------------------------------------------------------------------------------------------

struct MyInt1 {
    int value;

    explicit constexpr MyInt1(int value)
        : value{value}
    {
    }

    /**
     * Define three-way comparison operator
     * (only four comparison operators become available: <, <=, >, >=)
     */
    auto
    operator<=>(const MyInt1& other) const
    {
        return (value <=> other.value);
    }

    /**
     * Define equal (unequal implicitly) operator
     * (need it because we manually define three-way comparison)
     */
    bool
    operator==(const MyInt1& other) const
    {
        return (value == other.value);
    }
};

TEST(ThreeWayComparison, ManuallyDefining)
{
    MyInt1 n1{1};
    MyInt1 n2{2};

    EXPECT_THAT(n1 != n2, IsTrue());
    EXPECT_THAT(n1 == n2, IsFalse());
    EXPECT_THAT(n1 < n2, IsTrue());
    EXPECT_THAT(n1 > n2, IsFalse());
}

//--------------------------------------------------------------------------------------------------

/*

Comparison Categories:
    1. T supports all six relational operators: ==, !=, <, <=, >, and >=
       (Relational Operator)
       Example: integral types or strings.
    2. All equivalent values of T are indistinguishable
       (Equivalence)
       Example: Comparing absolute value of signed integral types or string case-insensitive.
    3. All values of T are comparable. For arbitrary values a and b of T,
       one of the three relations a < b, a == b , and a > b must be true
       (Comparable)
       Example: Comparing two arbitrary floating-point values need not be comparable, a = 5.5 and
                b = NaN, neither of the following expressions returns true: a < NaN, a == NaN,
                a > NaN.

|---------------------------------------------------------------------------|
| Comparison Category	Relational Operator  |  Equivalence    | Comparable |
| --------------------------------------------------------------------------|
| Strong Ordering	yes	             |   yes           |  yes       |
| Weak Ordering	yes	yes                  |                 |  yes       |
| Partial Ordering	yes                  |                 |            |
|---------------------------------------------------------------------------|

*/

struct Strong {
    std::strong_ordering
    operator<=>(const Strong&) const
        = default;
};

struct Weak {
    std::weak_ordering
    operator<=>(const Weak&) const
        = default;
};

struct Partial {
    std::partial_ordering
    operator<=>(const Partial&) const
        = default;
};

struct StrongWeakPartial {
    Strong s;
    Weak w;
    Partial p;

    auto
    operator<=>(const StrongWeakPartial&) const
        = default;

    // Fine
    // std::partial_ordering operator <=> (const StrongWeakPartial&) = default;
    // Error
    // std::strong_ordering operator <=> (const StrongWeakPartial&) const = default;
    // std::weak_ordering operator <=> (const StrongWeakPartial&) const = default;
};

//--------------------------------------------------------------------------------------------------

struct MyInt2 {
    int value;

    explicit MyInt2(int value)
        : value{value}
    {
    }

    /* Manually define three-way comparison operator specifying ordering explicitly */
    std::strong_ordering
    operator<=>(const MyInt2& other) const
    {
        if (value < other.value) {
            return std::strong_ordering::less;
        }
        if (value > other.value) {
            return std::strong_ordering::greater;
        }
        return std::strong_ordering::equal;
    }
};

struct MyString1 {
    std::string value;

    explicit MyString1(std::string value)
        : value{std::move(value)}
    {
    }

    std::weak_ordering
    operator<=>(const MyString1& other) const
    {
        static const auto kLessThan = [](const char ch1, const char ch2) {
            return (std::tolower(ch1) < std::tolower(ch2));
        };

        const bool isLess = std::lexicographical_compare(
            value.cbegin(), value.cend(), other.value.cbegin(), other.value.cend(), kLessThan);
        if (isLess) {
            return std::weak_ordering::less;
        }

        const bool isGreater = std::lexicographical_compare(
            other.value.cbegin(), other.value.cend(), value.cbegin(), value.cend(), kLessThan);
        if (isGreater) {
            return std::weak_ordering::greater;
        }

        return std::weak_ordering::equivalent;
    }

    bool
    operator==(const MyString1& other) const
    {
        static const auto kEqualThan = [](const char ch1, const char ch2) {
            return (std::tolower(ch1) == std::tolower(ch2));
        };

        return (value.size() == other.value.size()
                && std::equal(value.cbegin(), value.cend(), other.value.cbegin(), kEqualThan));
    }
};

TEST(ThreeWayComparison, ManuallyDefiningWithOrdering)
{
    MyInt2 n1{1};
    MyInt2 n2{2};

    // EXPECT_THAT(n1 != n2, IsTrue());
    // EXPECT_THAT(n1 == n2, IsFalse());
    EXPECT_THAT(n1 < n2, IsTrue());
    EXPECT_THAT(n1 > n2, IsFalse());
}

TEST(ThreeWayComparison, LexicographicalCompare)
{
    MyString1 str1{"hello world!"};
    MyString1 str2{"Hello World!"};

    EXPECT_THAT(str1 < str2, IsFalse());
    EXPECT_THAT(str1 > str2, IsFalse());
    EXPECT_THAT(str1 == str2, IsTrue());
}
