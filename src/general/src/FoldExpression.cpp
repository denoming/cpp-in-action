#include <gtest/gtest.h>

#include <string>

using namespace std::string_literals;

/**
 * Available fold expressions:
 * ... op pack          => (... (p1 op p2) op p3) ... op pN)
 * init op ... op pack  => (... (init op p1) op p2) ... op pN)
 * pack op ...          => (p1 op (p2 op (... (pN-1 op pN) ...)
 * pack op ... op fini  => (p1 op (p2 op (... (pN op fini) ...)
 */

/**
 * Sum of all passed arguments (by unary left fold).
 *
 * Expand to: ((arg1 + arg2) + arg3) ...
 */
template<typename... T>
auto
sum(T... args)
{
    return (... + args);
}

/**
 * Sub of all passed arguments (by unary right fold).
 *
 * Expand to: (arg1 - (arg2 - arg3)) ...
 */
template<typename... T>
auto
sub(T... args)
{
    return (args - ...);
}

/**
 * Mul of all passed arguments (by unary left fold with init value).
 *
 * Expand to: ((value * arg1) * arg2) ...
 */
template<typename... T, int I = 1>
auto
mul(T... args)
{
    return (I * ... * args);
}

/**
 * Print by space (by folded function calls)
 */
template<typename T, typename... Args>
void
printBySpace(T&& v, Args&&... args)
{
    std::cout << std::forward<T>(v);
    auto outWithSpace = [](auto&& arg) { std::cout << ' ' << arg; };
    (..., outWithSpace(std::forward<Args>(args)));
    std::cout << std::endl;
}

template<auto Sep = ' ', typename T, typename... Args>
void
printBy(T&& v, Args&&... args)
{
    std::cout << std::forward<T>(v);
    auto outWithSep = [](auto&& arg) { std::cout << Sep << arg; };
    (..., outWithSep(std::forward<Args>(args)));
    std::cout << std::endl;
}

/**
 * Combine hash for multiple given values
 */
template<typename T>
void
hashCombine(std::size_t& seed, const T& value)
{
    seed ^= std::hash<T>()(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template<typename... Types>
std::size_t
hashCombine(const Types&... args)
{
    std::size_t seed{0};
    (..., hashCombine(seed, args));
    return seed;
}

TEST(FoldExpressionTest, Sum)
{
    EXPECT_EQ(sum(1, 2, 3), 6);
}

TEST(FoldExpressionTest, Sub)
{
    EXPECT_EQ(sub(3, 2, 1), 2);
}

TEST(FoldExpressionTest, Mul)
{
    EXPECT_EQ(mul(1, 2, 3), 6);
    EXPECT_EQ(mul(), 1); // We can pass an empty parameter pack
}

TEST(FoldExpressionTest, PrintBySpace)
{
    printBySpace("Hello", "world", "!");
    EXPECT_TRUE(true);
}

TEST(FoldExpressionTest, PrintBy)
{
    static const char SEP[] = ", ";
    printBy<SEP>("Hello", "world", "!");
    EXPECT_TRUE(true);
}

TEST(FoldExpressionTest, HashCombine)
{
    EXPECT_NE(hashCombine("Hi"s, "World"s, 42), 0);
}
