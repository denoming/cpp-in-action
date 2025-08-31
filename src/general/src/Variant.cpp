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

#include <variant>

using namespace testing;

/**
 * Overload helper
 *  - inherits all lambdas
 *  - uses lambda's call operator (make them as subject of overload resolution)
 */
template<typename... Ts>
struct overload : Ts... {
    using Ts::operator()...;
};

/**
 * Deduction guide for overload helper
 * (all given objects types given in ctor initialize variadic template)
 */
template<typename... Ts>
overload(Ts...) -> overload<Ts...>;

struct SimpleLabel {
    [[nodiscard]] std::string
    build() const
    {
        return "<p class=\"date\">Date: " + data + "</p>";
    }

    std::string data;
};

struct DateLabel {
    [[nodiscard]] std::string
    build() const
    {
        return "<p class=\"date\">Date: " + data + "</p>";
    }

    std::string data;
};

TEST(VariantTest, Monostate)
{
    using IntOrString = std::variant<std::monostate, int, std::string>;

    IntOrString v1;
    EXPECT_TRUE(std::holds_alternative<std::monostate>(v1));

    IntOrString v2{10};
    EXPECT_TRUE(std::holds_alternative<int>(v2));
    EXPECT_EQ(v2.index(), 1);
    EXPECT_EQ(std::get<1>(v2), 10);

    IntOrString v3{"value"};
    EXPECT_TRUE(std::holds_alternative<std::string>(v3));
    EXPECT_EQ(v3.index(), 2);
    EXPECT_EQ(std::get<2>(v3), "value");
}

TEST(VariantTest, Twice)
{
    using IntOrString = std::variant<int, std::string>;

    IntOrString v1{10};
    IntOrString v2{"w"};

    // Our callable object for std::visit
    auto doubler = [](auto& val) {
        if constexpr (std::is_convertible_v<decltype(val), std::string>) {
            // Concatenation for strings
            val += val;
        } else {
            // Multiple by for numbers
            val *= 2;
        }
    };

    std::visit(doubler, v1);
    std::visit(doubler, v2);

    EXPECT_EQ(std::get<0>(v1), 20);
    EXPECT_EQ(std::get<1>(v2), "ww");
}

TEST(VariantTest, OverloadTwice)
{
    using IntOrString = std::variant<int, std::string>;

    IntOrString v1{10};
    IntOrString v2{"w"};

    auto visitor = overload{
        [](int& i) { i += i; },
        [](std::string& s) { s += s; },
    };

    std::visit(visitor, v1);
    std::visit(visitor, v2);

    EXPECT_EQ(std::get<0>(v1), 20);
    EXPECT_EQ(std::get<1>(v2), "ww");
}

TEST(VariantTest, Generic)
{
    using LabelVariant = std::variant<SimpleLabel, DateLabel>;
    using Labels = std::vector<LabelVariant>;

    Labels labels;
    labels.emplace_back(SimpleLabel{"Hello World"});
    labels.emplace_back(DateLabel{"19th September 2020"});

    const auto callBuild = [](const auto& label) -> std::string { return label.build(); };

    std::string content;
    for (const auto& label : labels) {
        content += std::visit(callBuild, label);
    }

    EXPECT_THAT(content, Not(IsEmpty()));
}