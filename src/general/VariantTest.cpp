#include <gtest/gtest.h>

#include <variant>

template<typename... Ts>
struct overload : Ts... {
    using Ts::operator()...;
};

template<typename... Ts>
overload(Ts...)->overload<Ts...>;

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

    auto twicer = [](auto& val) {
        if constexpr (std::is_convertible_v<decltype(val), std::string>) {
            val += val;
        }
        else {
            val *= 2;
        }
    };

    std::visit(twicer, v1);
    std::visit(twicer, v2);

    EXPECT_EQ(std::get<0>(v1), 20);
    EXPECT_EQ(std::get<1>(v2), "ww");
}

TEST(VariantTest, OverloadTwice)
{
    using IntOrString = std::variant<int, std::string>;

    IntOrString v1{10};
    IntOrString v2{"w"};

    auto overloader = overload{
        [](int& i) { i += i; },
        [](std::string& s) { s += s; },
    };

    std::visit(overloader, v1);
    std::visit(overloader, v2);

    EXPECT_EQ(std::get<0>(v1), 20);
    EXPECT_EQ(std::get<1>(v2), "ww");
}
