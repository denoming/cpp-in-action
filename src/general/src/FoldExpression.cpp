#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <string>
#include <optional>

using namespace testing;
using namespace std::string_literals;

/**
 * Available fold expressions:
 * <ul>
 *   <li> ( ... op pack )          => ((( pack1 op pack2 ) op pack3 ) ... op packN )
 *   <li> ( pack op ... )          => ( pack1 op ( ... ( packN-1 op packN )))
 *   <li> ( init op ... op pack )  => ((( init op pack1 ) op pack2 ) ... op packN )
 *   <li> ( pack op ... op fini )  => ( pack1 op ( ... ( packN op fini )))
 *  </ul>
 */

//--------------------------------------------------------------------------------------------------

/**
 * Sum of all passed arguments (by unary left fold).
 *
 * Expand to: ((arg1 + arg2) + arg3) ...
 */
template<typename... T>
static auto
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
static auto
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
static auto
mul(T... args)
{
    return (I * ... * args);
}

TEST(FoldExpression, Sum)
{
    EXPECT_EQ(sum(1, 2, 3), 6);
}

TEST(FoldExpression, Sub)
{
    EXPECT_EQ(sub(3, 2, 1), 2);
}

TEST(FoldExpression, Mul)
{
    EXPECT_EQ(mul(1, 2, 3), 6);
    EXPECT_EQ(mul(), 1); // We can pass an empty parameter pack
}

//--------------------------------------------------------------------------------------------------

/**
 * Print by given separator (by folded function calls)
 */
template<auto Sep = ' ', typename T, typename... Args>
static void
printBy(T&& v, Args&&... args)
{
    std::cout << std::forward<T>(v);
    auto outWithSep = [](auto&& arg) { std::cout << Sep << arg; };
    (..., outWithSep(std::forward<Args>(args)));
    std::cout << std::endl;
}

TEST(FoldExpression, PrintBySpace)
{
    printBy("Hello", "world", "!");
    EXPECT_TRUE(true);
}

TEST(FoldExpression, PrintBy)
{
    static const char SEP[] = ", ";
    printBy<SEP>("Hello", "world", "!");
    EXPECT_TRUE(true);
}

//--------------------------------------------------------------------------------------------------

/**
 * Combine hash for multiple given values
 */
template<typename T>
static void
hashCombine(std::size_t& seed, const T& value)
{
    seed ^= std::hash<T>()(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template<typename... Types>
static std::size_t
hashCombine(const Types&... args)
{
    std::size_t seed{0};
    (..., hashCombine(seed, args));
    return seed;
}

TEST(FoldExpression, HashCombine)
{
    EXPECT_NE(hashCombine("Hi"s, "World"s, 42), 0);
}

//--------------------------------------------------------------------------------------------------

struct Node {
    int value;
    Node* lh;
    Node* rh;

    explicit Node(int value)
        : value{value}
        , lh{nullptr}
        , rh{nullptr}
    {
    }
};

template<typename N, typename... Tp>
static Node*
traverse(N* node, Tp... paths)
{
    return (node->*...->*paths);
}

TEST(FoldExpression, Traverse)
{
    auto Node::*pLh = &Node::lh;
    auto Node::*pRh = &Node::rh;

    auto n1 = new Node{1};
    auto n2 = new Node{2};
    auto n3 = new Node{3};

    n1->lh = n2;
    n2->rh = n3;

    auto r = traverse(n1, pLh, pRh);
    EXPECT_EQ(r->value, 3);

    delete n3;
    delete n2;
    delete n1;
}

//--------------------------------------------------------------------------------------------------

template<typename T, typename... Tn>
constexpr bool
static isHomogeneous(T t, Tn... tn)
{
    return (std::is_same_v<T, Tn> && ...);
}

TEST(FoldExpression, isHomogeneous)
{
    EXPECT_FALSE(isHomogeneous(5, 5.1, "Hello"));
    EXPECT_TRUE(isHomogeneous(5, 1));
}

//--------------------------------------------------------------------------------------------------

template<typename Fn, typename... Ts>
static void
forEach(Fn fn, const Ts&... ts)
{
    (fn(ts), ...);
}

template<typename Fn, typename Predicate, typename... Ts>
static void
forEachWithPredicate(Fn fn, Predicate pred, const Ts&... ts)
{
    // Expands to: (    pred(ts[0]) ? false : (fn(ts[0]), true))
    //              && (pred(ts[1]) ? false : (fn(ts[1]), true))
    //              && ...
    ((pred(ts) ? false : (fn(ts), true)) && ...);
}

template<typename Fn, typename... Ts>
static void
forEachReverse(Fn fn, const Ts&... ts)
{
    // Expands to: std::ignore = ((fn(ts[0]), 0) = (fn(ts[1]), 0)) = ...
    // a = b = c (where c will be evaluated first)
    (std::ignore = ... = (fn(ts), 0));
}

static void
print(int value)
{
    std::cout << std::to_string(value) << " ";
}

TEST(FoldExpression, ForEach)
{
    forEach(print, 5, 1, 3);
}

TEST(FoldExpression, ForEachReverse)
{
    forEachReverse(print, 5, 1, 3);
}

TEST(FoldExpression, ForEachWithPredicate)
{
    auto predicate = [](signed n) { return (n < 0); };
    forEachWithPredicate(print, predicate, 1, 3, -2, 5);
}

//--------------------------------------------------------------------------------------------------

template<typename Predicate, typename... Ts>
static bool
anyOf(Predicate pred, const Ts&... ts)
{
    return (pred(ts) || ...);
}

TEST(FoldExpression, AnyOf)
{
    auto pred = [](signed n) { return (n < 0); };
    EXPECT_THAT(anyOf(pred, -1, 2, 3), IsTrue());
    EXPECT_THAT(anyOf(pred, +1, 2, 3), IsFalse());
}

//--------------------------------------------------------------------------------------------------

template<typename Predicate, typename... Ts>
static size_t
countOf(Predicate pred, const Ts&... ts)
{
    return (size_t{} + ... + (pred(ts) ? 1 : 0));
}

TEST(FoldExpression, CountOf)
{
    auto pred = [](signed n) { return (n < 0); };
    EXPECT_EQ(countOf(pred, -1, 2, 3), 1);
}

//--------------------------------------------------------------------------------------------------

template<typename Predicate, typename... Ts>
static auto
findIf(Predicate pred, const Ts&... ts) -> std::optional<std::common_type_t<Ts...>>
{
    std::optional<std::common_type_t<Ts...>> result;
    std::ignore = ((pred(ts) ? (result = ts, true) : false) || ...);
    return result;
}

TEST(FoldExpression, FindIf)
{
    auto pred = [](signed n) { return (n < 0); };
    EXPECT_THAT(findIf(pred, 0, 2, -1), Optional(-1));
}

//--------------------------------------------------------------------------------------------------

template<size_t N, typename... Ts>
static auto
getNth(const Ts&... ts) -> std::common_type_t<Ts...>
{
    std::common_type_t<Ts...> result;
    size_t n{};
    ((n++ == N ? (result = ts, true) : false) || ...);
    return result;
}

TEST(FoldExpression, GetNth)
{
    EXPECT_EQ(getNth<2>(0, 2, -1), -1);
}