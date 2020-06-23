#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <string>
#include <list>

using ::testing::Gt;

/**
 * Lambda wrapper that provide counting of calls.
 */
template<typename C>
class CountCalls {
public:
    CountCalls(C callable)
        : _callable{std::move(callable)}
        , _count{0}
    {
    }

    template<typename... Args>
    decltype(auto)
    operator()(Args&&... args)
    {
        _count++;
        return _callable(std::forward<Args>(args)...);
    }

    std::size_t
    count() const
    {
        return _count;
    }

private:
    C _callable;
    std::size_t _count;
};

/**
 * Some data
 */
struct Person {
    std::string name;
    std::size_t age;
};

using Persons = std::list<Person>;

TEST(TemplateClassDeduction, LambdaWrapper)
{
    // clang-format off
    Persons persons{
        {"John", 21},
        {"Dave", 16},
        {"Kate", 18}
    };

    // Wrap data specific comparator
    CountCalls wrapper{
        [](const Person& p1, const Person& p2) {
            return std::lexicographical_compare(p1.name.cbegin(), p1.name.cend(), p2.name.cbegin(), p2.name.cend());
        }
    };
    // clang-format on

    // Provide wrapped comparator
    persons.sort(std::ref(wrapper));

    EXPECT_THAT(wrapper.count(), Gt(1));
}
