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

#include <concepts>
#include <iomanip>
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
