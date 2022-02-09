#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <iostream>
#include <type_traits>

using namespace testing;

TEST(MinorFeaturesTest, RelaxedEnum)
{
    enum class Weekday { mon = 1, tue, wed, thu, fri, sat, sun };
    Weekday e1{1};
    EXPECT_EQ(e1, Weekday::mon);
    Weekday e2{5};
    EXPECT_EQ(e2, Weekday::fri);
    e2 = Weekday{4};
    EXPECT_EQ(e2, Weekday::thu);
}

//-----------------------------------------------------------------------------

TEST(MinorFeaturesTest, DirectlListInitialization)
{
    [[maybe_unused]] auto a1{1}; // <= OK (int)
    // auto b1{1, 2, 3};    <= Error in C++17

    [[maybe_unused]] auto a2 = 1;         // <= OK (int)
    [[maybe_unused]] auto b2 = {1, 2, 3}; // <= std::initializer_list<int>
}

//-----------------------------------------------------------------------------

TEST(MinorFeaturesTest, HexadecimalFloatingPointLiterals)
{
    double v{0xAp2}; // 10 multiplied by "2 to the power of 2" => 40
    EXPECT_THAT(v, DoubleNear(40.0, 0.1));
}

//-----------------------------------------------------------------------------

TEST(MinorFeaturesTest, Utf8CharacterLiterals)
{
    std::u8string s = u8"ᴥ";
    EXPECT_THAT(s, SizeIs(Gt(1)));
}

//-----------------------------------------------------------------------------

class NotDefaultConstructible {
public:
    NotDefaultConstructible(const std::string& name)
        : _name{name}
    {
    }

private:
    std::string _name;
};

template<typename T>
class C {
public:
    static_assert(std::is_default_constructible_v<T>);

private:
    T _value;
};

TEST(MinorFeaturesTest, StaticAssertWithSingleArgument)
{
    [[maybe_unused]] C<int> c1;
    // C<NotDefaultConstructible> c2; <= Failed to compile due static assert
}
