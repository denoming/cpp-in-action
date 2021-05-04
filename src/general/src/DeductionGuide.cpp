#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::TypedEq;
using ::testing::UnorderedElementsAre;

//-----------------------------------------------------------------------------

template<typename T1, typename T2>
struct Pair {
    /**
     * As we see parameter passed by reference, therefore they are not decay.
     *
     * Problem: Arguments, such as raw array doesn't decay and we will have and error
     *          to initialize internal fields by lvalue.
     *          This example: "Pair pair{"Value1", "Value2"};" without particular
     *          deduction guide cause an error.
     *
     */
    Pair(const T1& value1, const T2& value2)
        : value1{value1}
        , value2{value2}
    {
    }

    T1 value1;
    T2 value2;
};

/**
 * This deduction guide fix a problem and enable decay despite arguments passed by reference.
 * It only tell that we need to decay passed arguments as it usually done when arguments
 * passed by value.
 */
template<typename T1, typename T2>
Pair(T1, T2) -> Pair<T1, T2>;

TEST(DeductionGuideTest, TemplateDeductionGuide)
{
    Pair pair{"Value1", "Value2"};
    EXPECT_EQ(pair.value1, "Value1");
    EXPECT_EQ(pair.value2, "Value2");
}

//-----------------------------------------------------------------------------

template<typename V>
struct Data {
    V value;
};

/**
 *  Our deduction guide says to use std::string as a type
 *  of template parameter instead of raw pointer when
 *  string literal was passed.
 */
Data(const char*) -> Data<std::string>;

TEST(DeductionGuideTest, NonTemplateDeductionGuide)
{
    Data d{"Text"};
    EXPECT_THAT(d.value, TypedEq<std::string>(std::string{"Text"}));
}

//-----------------------------------------------------------------------------

/**
 * We have simple aggregate. The next examples without deduction guide cause an error:
 *  - A i1{42};
 *  - A s1("hi");
 */

template<typename T>
struct A {
    T value;
};

A(int) -> A<int>;
A(const char*) -> A<std::string>;

TEST(DeductionGuideTest, DeductionGuideForSimpleAggregate)
{
    A a{"Text"};
    EXPECT_THAT(a.value, TypedEq<std::string>(std::string{"Text"}));
}

//-----------------------------------------------------------------------------

template<typename T>
struct B {
    template<typename It>
    B(It begin, It end)
        : value(begin, end)
    {
    }

    std::vector<T> value;
};

template<typename It>
B(It, It) -> B<typename std::iterator_traits<It>::value_type>;

TEST(DeductionGuideTest, DeductionGuideWithIterator)
{
    std::set<int> values;
    values.insert(1);
    values.insert(2);
    values.insert(3);

    B b{values.cbegin(), values.cend()};
    EXPECT_THAT(b.value, UnorderedElementsAre(1, 2, 3));
}
