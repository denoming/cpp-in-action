#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <complex>
#include <optional>

using namespace testing;

//--------------------------------------------------------------------------------------------------

TEST(Optional, ChangingValue)
{
    using DoubleComplex = std::complex<double>;
    using DoubleComplexOrNull = std::optional<DoubleComplex>;

    DoubleComplexOrNull op{std::in_place, 3.0, 4.0};
    EXPECT_DOUBLE_EQ(op.value().real(), 3.0);
    EXPECT_DOUBLE_EQ(op.value().imag(), 4.0);
}

//--------------------------------------------------------------------------------------------------

struct UserProfile { };

static std::optional<UserProfile>
fetchFromCache(int)
{
    /* No such user profile in the cache */
    return std::nullopt;
}

static std::optional<UserProfile>
fetchFromServer(int)
{
    return UserProfile{};
}

static std::optional<int>
extractData(const UserProfile& profile)
{
    return 1;
}

TEST(Optional, Monadic1)
{
    const int userId{};
    const auto data = fetchFromCache(userId)
                          .or_else([&]() { return fetchFromServer(userId); })
                          .and_then(extractData)
                          .transform([](int data) { return data + 1; });

    EXPECT_THAT(data, Optional(2));
}

//--------------------------------------------------------------------------------------------------

static std::optional<int>
divide(int a, int b)
{
    if (b == 0) {
        return std::nullopt;
    }
    return a / b;
}

TEST(Optional, AndThen)
{
    const auto r1 = std::optional<int>(5).and_then([](int value) { return divide(20, value); });
    EXPECT_THAT(r1, Optional(4));
    const auto r2 = std::optional<int>(0).and_then([](int value) { return divide(20, value); });
    EXPECT_THAT(r2, std::nullopt);
}

//--------------------------------------------------------------------------------------------------

TEST(Optional, Transform)
{
    std::optional<int> n1 = 5;
    EXPECT_THAT(n1.transform([](int x) { return x * x; }), Optional(25));
    std::optional<int> n2;
    EXPECT_THAT(n2.transform([](int x) { return x * x; }), std::nullopt);
}

//--------------------------------------------------------------------------------------------------

static std::optional<int>
try1()
{
    return std::nullopt;
}

static std::optional<int>
try2()
{
    return 1;
}

TEST(Optional, OrElse)
{
    EXPECT_THAT(std::optional<int>{}.or_else(try1).or_else(try2), Optional(1));
}