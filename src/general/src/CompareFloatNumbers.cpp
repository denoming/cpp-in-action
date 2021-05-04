#include <gtest/gtest.h>

#include <concepts>
#include <iostream>

template<std::floating_point T>
bool
almostEqual(T x, T y, T epsilon = 1.0e-05)
{
    T diff = std::abs(x - y);
    if (diff <= epsilon) {
        return true;
    }
    return diff <= epsilon * std::max(std::abs(x), std::abs(y));
}

template<std::floating_point T>
T
calculate(T start, T decrement, std::size_t count)
{
    for (std::size_t i = 0; i < count; ++i) {
        start -= decrement;
    }
    return start;
}

std::size_t
testQuality(std::size_t total)
{
    auto count = 0;
    for (std::size_t i = 0; i < total; ++i) {
        auto target = (i / 10.0f);
        std::floating_point auto actual = calculate(9.0f + target, 0.2f, 45);
        if (almostEqual(actual, target)) {
            ++count;
        } else {
            std::cout << std::setprecision(15) << actual << "!=" << target << std::endl;
        }
    }
    return count;
}

TEST(CompareFloatingNumbers, QualityCheck)
{
    auto count = testQuality(10000);
    EXPECT_EQ(count, 10000);
}
