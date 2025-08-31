// Copyright 2025 Denys Asauliak
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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