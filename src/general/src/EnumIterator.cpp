#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <iterator>

using namespace testing;

template<typename T, T... args>
class EnumIterator {
public:
    static constexpr T Values[] = {args...};
    static constexpr std::size_t Size = sizeof...(args);

    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = const T*;
    using reference = const T&;
    using iterator_category = std::bidirectional_iterator_tag;

    explicit EnumIterator(bool isEnd = false)
        : _p{isEnd ? static_cast<std::ptrdiff_t>(Size) : 0}
    {
    }

    const T&
    operator*() const
    {
        return Values[_p];
    }

    EnumIterator
    operator++(int)
    {
        EnumIterator copy{*this};
        this->operator++();
        return copy;
    }

    EnumIterator&
    operator++()
    {
        _p++;
        return *this;
    }

    EnumIterator
    operator--(int)
    {
        EnumIterator copy{*this};
        this->operator--();
        return copy;
    }

    EnumIterator&
    operator--()
    {
        _p--;
        return *this;
    }

    bool
    operator==(const EnumIterator& other) const
    {
        return (_p == other._p);
    }

    bool
    operator!=(const EnumIterator& other) const
    {
        return (_p != other._p);
    }

private:
    difference_type _p;
};

template<typename T, T... args>
struct EnumRange {
    static EnumIterator<T, args...>
    begin()
    {
        return EnumIterator<T, args...>(false);
    }

    static EnumIterator<T, args...>
    end()
    {
        return EnumIterator<T, args...>(true);
    }
};

/* Example of enumerate */
enum class Colors { Red, Green, Blue };

/* For class iterator usage */
using ColorsIterator = EnumIterator<Colors, Colors::Red, Colors::Green, Colors::Blue>;

/* For range usage */
using ColorsRange = EnumRange<Colors, Colors::Red, Colors::Green, Colors::Blue>;

TEST(EnumIterator, Iterate)
{
    std::vector<int> values;
    for (auto colorIt = ColorsRange::begin(); colorIt != ColorsRange::end(); ++colorIt) {
        values.push_back(static_cast<int>(*colorIt));
    }
    EXPECT_THAT(values, ElementsAre(0, 1, 2));

    values.clear();
    for (auto color : ColorsRange{}) {
        values.push_back(static_cast<int>(color));
    }
    EXPECT_THAT(values, ElementsAre(0, 1, 2));
}