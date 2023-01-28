#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <iostream>
#include <concepts>
#include <numeric>
#include <string>
#include <vector>
#include <forward_list>
#include <list>

//--------------------------------------------------------------------------------------------------
// Four ways to use predefined concepts to constrain type

// 1. Using 'requires' clause (+ concept as return type)

template<typename T>
    requires std::integral<T>
std::integral auto
gcd1(T a, T b)
{
    return (b == 0) ? a : std::gcd(b, a % b);
}

// 2. Using trailing 'requires' clause (+ concept as return type)

template<typename T>
std::integral auto
gcd2(T a, T b)
    requires std::integral<T>
{
    return (b == 0) ? a : std::gcd(b, a % b);
}

// 3. Using constrained template parameter (+ concept as return type)

template<std::integral T>
std::integral auto
gcd3(T a, T b)
{
    return (b == 0) ? a : std::gcd(b, a % b);
}

// 4. Using abbreviated function template (+ concept as return type)

std::integral auto
gcd4(std::integral auto a, std::integral auto b)
{
    return (b == 0) ? a : std::gcd(b, a % b);
}

TEST(Concepts, Using)
{
    EXPECT_TRUE(gcd1(1, 2) == gcd2(1, 2) == gcd3(1, 2) == gcd4(1, 2));
}

//--------------------------------------------------------------------------------------------------
// Use syntactic constraints

/* Prefer to use named (predefined or newly created) concepts but not syntactic constraints */
template<std::size_t Size>
    requires(Size <= 2)
std::size_t
sum(std::size_t n)
{
    return (Size + n);
}

TEST(Concepts, Sum)
{
    EXPECT_EQ(sum<2>(2), 4);
    // constrains not satisfied (Size > 2)
    // EXPECT_EQ(sum<3>(3), 4);
}

//--------------------------------------------------------------------------------------------------
// Use-cases for concepts

//
// 1. Declare compile-time predicates
//

struct Test { };

TEST(Concepts, UseCase1)
{
    EXPECT_TRUE(std::three_way_comparable<double>);
    EXPECT_TRUE(std::three_way_comparable<std::string>);
    EXPECT_FALSE(std::three_way_comparable<Test>);
}

//
// 2. Class templates
//

template<std::regular T>
class MyVector1 { };

TEST(Concepts, UseCase2)
{
    EXPECT_TRUE(true);
}

//
// 3. Generic member functions
//

struct NotCopyable {
    NotCopyable() = default;
    NotCopyable(const NotCopyable&) = delete;
    NotCopyable&
    operator=(const NotCopyable&)
        = delete;
};

template<typename T>
class MyVector2 {
public:
    void
    push(const T&)
        requires std::copyable<T>
    {
    }
};

TEST(Concepts, UseCase3)
{
    MyVector2<int> v1;
    v1.push(1);

    MyVector2<NotCopyable> v2;
    // constrains not satisfied (not copyable)
    // v2.push(NotCopyable{});
}

//
// 4. Variadic Templates
//

template<std::integral... Args>
bool
all(Args... args)
{
    return (... and args);
}

template<std::integral... Args>
bool
any(Args... args)
{
    return (... or args);
}

template<std::integral... Args>
bool
none(Args... args)
{
    return not(... or args);
}

TEST(Concepts, UseCase4)
{
    EXPECT_TRUE(all(true, true, true));
    EXPECT_FALSE(all(true, false, true));

    EXPECT_TRUE(any(false, false, true));
    EXPECT_FALSE(all(false, false, false));

    EXPECT_TRUE(none(false, false, false));
    EXPECT_FALSE(none(false, false, true));
}

//
// 5. Overloading
//

template<std::forward_iterator I>
void
advance(I& it, int n)
{
    std::cout << "advance(forward_iterator)" << std::endl;
}

template<std::bidirectional_iterator I>
void
advance(I& it, int n)
{
    std::cout << "advance(bidirectional_iterator)" << std::endl;
}

template<std::random_access_iterator I>
void
advance(I& it, int n)
{
    std::cout << "advance(random_access_iterator)" << std::endl;
}

TEST(Concepts, UseCase5)
{
    std::forward_list<int> c1;
    /* Use constrained auto placeholder */
    std::forward_iterator auto it1 = c1.begin();
    advance(it1, 1);

    std::list<int> c2;
    /* Use constrained auto placeholder */
    std::bidirectional_iterator auto it2 = c2.begin();
    advance(it2, 1);

    std::vector<int> c3;
    /* Use constrained auto placeholder */
    std::random_access_iterator auto it3 = c3.begin();
    advance(it3, 1);
}

//
// 6. Template Specialization
//

template<typename T>
struct Vector {
    Vector()
    {
        std::cout << "Vector<T> is chosen" << std::endl;
    }
};

template<std::regular T> // Specialize class template
struct Vector<T> {       //
    Vector()
    {
        std::cout << "Vector<std::regular> is chosen" << std::endl;
    }
};

TEST(Concepts, UseCase6)
{
    Vector<int> v1;
    Vector<int&> v2;
}

//
// 7. Using more than one concept
//

template<std::input_iterator I, typename V>
    requires std::equality_comparable_with<std::iter_value_t<I>, V>
I
findValue(I begin, I end, V value)
{
    return {};
}

TEST(Concepts, UseCase7)
{
    std::list<int> c1;
    std::input_iterator auto it = findValue(c1.begin(), c1.end(), 5);
}

//--------------------------------------------------------------------------------------------------
// Define concepts

/* Define concept using available type traits */
template<typename T>
concept Integral = std::is_integral_v<T>;
template<typename T>
concept SignedIntegral = Integral<T> and std::is_signed_v<T>;
template<typename T>
concept UnsignedIntegral = Integral<T> and not SignedIntegral<T>;

/* Define concepts using requires expression */

// Simple requirements
template<typename T>
concept Addable = requires(T a, T b) { a + b; };

// Type requirements
template<typename T>
struct Other;
template<>
struct Other<std::vector<int>>;

template<typename T>
concept TypeRequirement
    = requires {
          typename T::value_type; // Requires that type T has a nested member 'value_type'
          typename Other<T>;      // Requires that Other<T> can be instantiated
      };

TEST(Concepts, TypeRequirements)
{
    TypeRequirement auto v1{std::vector<int>{1, 2, 3}};
    // Not satisfied
    // TypeRequirement auto v2{5};
}

// Compound requirements
// { expression } noexcept(optional) return-type-requirement(optional);

// clang-format off
template<typename T>
concept Equal = requires(T a, T b) {
    { a == b } -> std::convertible_to<bool>;
    { a != b } -> std::convertible_to<bool>;
};
// clang-format on

bool
isEqual(Equal auto a, Equal auto b)
{
    return (a == b);
}

struct WithoutEqual {
    bool
    operator==(const WithoutEqual&)
        = delete;
};

TEST(Concepts, CompoundRequirements)
{
    EXPECT_TRUE(isEqual(1, 1));
    // Not satisfied
    // EXPECT_TRUE(isEqual(WithoutEqual{}, WithoutEqual{}));
}

// Nested requirements
// requires constraint-expression;

// clang-format off
template<typename T>
concept Unsigned = std::is_integral_v<T> &&
requires(T) {
   requires std::is_signed_v<T>;
};
// clang-format on

struct Data {
    [[nodiscard]] std::size_t
    count()
    {
        return 0U;
    }
};

TEST(Concepts, UsingRequires)
{
    // clang-format off
    static_assert(requires(Data d) {
        { d.count() } -> std::convertible_to<int>;
    });
    // clang-format on
}