#include <gtest/gtest.h>

#include <variant>

/**
 * Set of lambda overloads (as struct with multiple overloads of call operator)
 */
template<typename... Ts>
struct overload : Ts... //! Inherit all function call operator from variadic list of classes
{
    //! Without using declaration we will have ambiguous (two different overload of the same member function)
    using Ts::operator()...;
};

/**
 * Deduction guide
 * (base types are deduced from passed arguments)
 */
template<typename... Ts>
overload(Ts...)->overload<Ts...>;

TEST(ExtendedUsingDeclaration, VariadicUsingDeclarations)
{
    using Variant = std::variant<int, long, double, std::string>;
    std::vector<Variant> values = {10, 15l, 1.5, "hello"};

    // clang-format off
    for (auto& value : values) {
        std::visit(overload{
           [](auto) { std::cout << "Default one" << std::endl; },
           [](double) { std::cout << "Double one" << std::endl; },
           [](const std::string&) { std::cout << "String one" << std::endl; }},
        value);
    }
    // clang-format on

    EXPECT_TRUE(true);
}

//-----------------------------------------------------------------------------

template<typename T>
class Base {
public:
    Base() = default;

    Base(T value)
        : _value{value}
    {
    }

private:
    T _value{};
};

template<typename... Ts>
class Multi : Base<Ts>... {
public:
    using Base<Ts>::Base...;      /// derive all constructors
    using Base<Ts>::operator=...; /// derive all assignment operators
};

TEST(ExtendedUsingDeclaration, VariadicUsingForInheritingConstructors)
{
    using MultiType = Multi<int, std::string, bool>;
    MultiType m1 = 42;
    MultiType m2 = std::string("hello");
    MultiType m3 = true;

    EXPECT_TRUE(true);
}
