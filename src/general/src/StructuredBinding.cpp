#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <string>
#include <map>
#include <utility>

using namespace testing;

//-------------------------------------------------------------------------------------------------

struct Some {
    int i{0};
    std::string s{"text"};
};

Some
getStruct()
{
    return Some{42, "hello"};
}

TEST(StructuredBindingTest, Decomposition)
{
    Some some;

    // declaration + decomposition
    auto [u1, v1] = some;

    EXPECT_EQ(u1, 0);
    EXPECT_EQ(v1, "text");

    // declaration + decomposition (another syntax)
    auto [u2, v2]{some};
    EXPECT_EQ(u2, 0);
    EXPECT_EQ(v2, "text");

    auto [u3, v3] = getStruct();
    EXPECT_EQ(u3, 42);
    EXPECT_EQ(v3, "hello");
}

//-----------------------------------------------------------------------------

TEST(StructuredBindingTest, MapDecomposition)
{
    using Map = std::map<unsigned, std::string>;

    Map map;
    map.emplace(0, "text1");
    map.emplace(1, "text2");
    map.emplace(2, "text3");

    std::vector<std::string> tokens;
    for (const auto& [key, value] : map) {
        tokens.push_back(std::to_string(key) + value);
    }

    EXPECT_THAT(tokens, ElementsAre("0text1", "1text2", "2text3"));
}

//-----------------------------------------------------------------------------

class Customer {
public:
    Customer(std::string firstName, std::string lastName, unsigned value)
        : _firstName{std::move(firstName)}
        , _lastName{std::move(lastName)}
        , _value{value}
    {
    }

    const std::string&
    firstName() const
    {
        return _firstName;
    }

    std::string&
    firstName()
    {
        return _firstName;
    }

    const std::string&
    lastName() const
    {
        return _lastName;
    }

    std::string&
    lastName()
    {
        return _lastName;
    }

    unsigned
    value() const
    {
        return _value;
    }

    unsigned&
    value()
    {
        return _value;
    }

private:
    std::string _firstName;
    std::string _lastName;
    unsigned _value;
};

/** Define the number of attributes (firstName + lastName + value) */
template<>
struct std::tuple_size<Customer> {
    static constexpr unsigned value = 3;
};

/** Define type of the last attribute (full template specialization)*/
template<>
struct std::tuple_element<2, Customer> {
    using type = unsigned;
};

/** Define type for first two attributes (partial template specialization) */
template<std::size_t I>
struct std::tuple_element<I, Customer> {
    using type = std::string;
};

/** Define getters */

template<std::size_t I>
decltype(auto)
get(Customer&& c)
{
    static_assert(I < 3);
    if constexpr (I == 0) {
        return std::move(c.firstName());
    } else if constexpr (I == 1) {
        return std::move(c.lastName());
    } else /* I == 2 */ {
        return std::move(c.value());
    }
}

template<std::size_t I>
decltype(auto)
get(Customer& c)
{
    static_assert(I < 3);
    if constexpr (I == 0) {
        return c.firstName();
    } else if constexpr (I == 1) {
        return c.lastName();
    } else /* I == 2 */ {
        return c.value();
    }
}

template<std::size_t I>
decltype(auto)
get(const Customer& c)
{
    static_assert(I < 3);
    if constexpr (I == 0) {
        return c.firstName();
    } else if constexpr (I == 1) {
        return c.lastName();
    } else /* I == 2 */ {
        return c.value();
    }
}

TEST(StructuredBindingTest, ProvideTupleLikeApi)
{
    Customer c{"Tim", "Starr", 42};
    auto& [f, l, v] = c;
    EXPECT_EQ(f, "Tim");
    EXPECT_EQ(l, "Starr");
    EXPECT_EQ(v, 42);
    v = 30;
    EXPECT_EQ(c.value(), 30);
}
