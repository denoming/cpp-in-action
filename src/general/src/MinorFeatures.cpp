#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <fmt/core.h>
#include <fmt/format.h>

#include <type_traits>
#include <source_location>

using namespace testing;

TEST(MinorFeatures, RelaxedEnum)
{
    enum class Weekday { mon = 1, tue, wed, thu, fri, sat, sun };
    Weekday e1{1};
    EXPECT_EQ(e1, Weekday::mon);
    Weekday e2{5};
    EXPECT_EQ(e2, Weekday::fri);
    e2 = Weekday{4};
    EXPECT_EQ(e2, Weekday::thu);
}

//--------------------------------------------------------------------------------------------------

TEST(MinorFeatures, DirectlListInitialization)
{
    [[maybe_unused]] auto a1{1}; // <= OK (int)
    // auto b1{1, 2, 3};    <= Error in C++17

    [[maybe_unused]] auto a2 = 1;         // <= OK (int)
    [[maybe_unused]] auto b2 = {1, 2, 3}; // <= std::initializer_list<int>
}

//--------------------------------------------------------------------------------------------------

TEST(MinorFeatures, HexadecimalFloatingPointLiterals)
{
    double v{0xAp2}; // 10 multiplied by "2 to the power of 2" => 40
    EXPECT_THAT(v, DoubleNear(40.0, 0.1));
}

//--------------------------------------------------------------------------------------------------

TEST(MinorFeatures, Utf8CharacterLiterals)
{
    std::u8string s = u8"ᴥ";
    EXPECT_GT(s.size(), 1);
}

//--------------------------------------------------------------------------------------------------

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

TEST(MinorFeatures, StaticAssertWithSingleArgument)
{
    [[maybe_unused]] C<int> c1;
    // C<NotDefaultConstructible> c2; <= Failed to compile due static assert
}

//--------------------------------------------------------------------------------------------------

static int
plusFn(int a, int b)
{
    return a + b;
}

static const auto plusLambda = [](int a, int b) { return a + b; };

TEST(MinorFeatures, BindFront)
{
    auto p1 = std::bind_front(plusFn, 1000);
    fmt::print("Fn: 1000 + 20 = {}\n", p1(20));

    auto p2 = std::bind_front(plusLambda, 1000);
    fmt::print("Lambda: 1000 + 20 = {}\n", p2(20));
}

//--------------------------------------------------------------------------------------------------

static void
log(std::string_view text, std::source_location loc = std::source_location::current())
{
    fmt::print("{}:{} {}", loc.file_name(), loc.line(), text);
}

TEST(MinorFeatures, SourceLocation)
{
    log("Some log");
}

//--------------------------------------------------------------------------------------------------

TEST(MinorFeatures, GetAddress)
{
    int n1{5};

    int* pn1{&n1};
    fmt::print("std::to_address(pn1): {}\n", fmt::ptr(std::to_address(pn1)));

    std::unique_ptr<int> pn2{new int{5}};
    fmt::print("std::to_address(pn2): {}\n", fmt::ptr(std::to_address(pn2)));

    std::shared_ptr<int> pn3{new int{5}};
    fmt::print("std::to_address(pn3): {}\n", fmt::ptr(std::to_address(pn3)));
}