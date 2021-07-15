#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <algorithm>

using namespace testing;

template<typename T>
class BigArray {
public:
    BigArray(std::size_t size)
        : _size{size}
        , _data{new int[size]}
    {
    }

    BigArray(const BigArray& other)
        : _size{other._size}
        , _data{new int[other._size]}
    {
        std::copy(other._data, other._data + _size, _data);
    }

    ~BigArray()
    {
        delete[] _data;
    }

    BigArray&
    operator=(BigArray other)
    {
        swap(*this, other);
        return *this;
    }

    friend void
    swap(BigArray& a1, BigArray& a2) noexcept
    {
        std::swap(a1._data, a2._data);
        std::swap(a1._size, a2._size);
    }

    std::size_t
    size() const noexcept
    {
        return _size;
    }

public:
    std::size_t _size;
    T* _data;
};

TEST(CopyAndSwap, Check)
{
    BigArray<int> a1{100};
    EXPECT_THAT(a1, SizeIs(100));
    BigArray<int> a2{200};
    EXPECT_THAT(a2, SizeIs(200));

    a1 = a2;

    EXPECT_THAT(a1, SizeIs(200));
    EXPECT_THAT(a2, SizeIs(200));
}