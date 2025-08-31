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

#include <string>
#include <iostream>
#include <type_traits>

using namespace testing;

//--------------------------------------------------------------------------------------------------

template<unsigned p, unsigned g>
struct DoIsPrime {
    static constexpr bool value = (p % g != 0) && DoIsPrime<p, g - 1>::value;
};

template<unsigned p>
struct DoIsPrime<p, 2> {
    static constexpr bool value = (p % 2 != 0);
};

/* Prior to C++11 way to do compile time computation */
template<unsigned p>
struct IsPrime {
    static constexpr bool value = DoIsPrime<p, p / 2>::value;
};

template<>
struct IsPrime<0> {
    static constexpr bool value = false;
};
template<>
struct IsPrime<1> {
    static constexpr bool value = false;
};
template<>
struct IsPrime<2> {
    static constexpr bool value = true;
};
template<>
struct IsPrime<3> {
    static constexpr bool value = true;
};

// or

/* C++14 and above way to do compile time computation */
constexpr bool
isPrime(unsigned p)
{
    for (unsigned d = 2; d <= p / 2; ++d) {
        if (p % d == 0) {
            return false;
        }
    }
    return (p > 1);
}

//--------------------------------------------------------------------------------------------------

class Person {
public:
    template<typename T>
    using EnableIfString = std::enable_if_t<std::is_constructible_v<std::string, T>>;

    // Generic ctor for passed initial name
    template<typename S, typename = EnableIfString<S>>
    explicit Person(S&& n)
        : _name{std::forward<S>(n)}
    {
        std::cout << "Template constructor" << std::endl;
    }

    Person(const Person& other)
        : _name{other._name}
    {
        std::cout << "Copy constructor" << std::endl;
    }

    Person(Person&& other) noexcept
        : _name{std::move(other._name)}
    {
        std::cout << "Move constructor" << std::endl;
    }

    [[nodiscard]] const std::string&
    name() const
    {
        return _name;
    }

private:
    std::string _name;
};

//--------------------------------------------------------------------------------------------------

template<typename Callable, typename... Args>
decltype(auto)
call(Callable&& c, Args&&... args)
{
    if constexpr (std::is_same_v<std::result_of_t<Callable(Args...)>, void>) {
        std::invoke(std::forward<Callable>(c), std::forward<Args>(args)...);
        return;
    } else {
        return std::invoke(std::forward<Callable>(c), std::forward<Args>(args)...);
        // decltype(auto) rv = std::invoke(std::forward<Callable>(c), std::forward<Args>(args)...);
        // ... do some staff ...
        // return rv;
    }
}

//--------------------------------------------------------------------------------------------------

template<typename T>
class Ptr {
public:
    Ptr()
        : _data{new T}
    {
    }

    ~Ptr()
    {
        delete _data;
    }

    // NOLINTNEXTLINE
    T**
    operator&()
    {
        return &_data;
    }

private:
    T* _data;
};

//--------------------------------------------------------------------------------------------------

class SomeType {
public:
    SomeType()
    {
        std::cout << "Constructor" << std::endl;
    }

    SomeType(const SomeType&)
    {
        std::cout << "Copy constructor" << std::endl;
    }

    SomeType&
    operator=(const SomeType&)
    {
        std::cout << "Copy operator" << std::endl;
        return *this;
    }

    SomeType(SomeType&&) noexcept
    {
        std::cout << "Move constructor" << std::endl;
    }

    SomeType&
    operator=(SomeType&&) noexcept
    {
        std::cout << "Move operator" << std::endl;
        return *this;
    }
};

template<typename T>
auto
get(T in)
{
    return in;
}

template<typename T>
void
set(T in)
{
    // some work //
}

template<typename T>
void
perfectForward(T value)
{
    auto&& temp = get(std::move(value));
    // do some work with temp //
    set(std::forward<decltype(temp)>(temp)); // Perfect forwarding without extra copies
}

//--------------------------------------------------------------------------------------------------

template<typename T>
union DataChunks {
    T data;
    std::uint8_t chunks[sizeof(T)];
};

//--------------------------------------------------------------------------------------------------

template<int S, bool = isPrime(S)>
struct Helper {
    static void
    f1()
    {
        /* Some helper function for default case */
    }
};

template<int S>
struct Helper<S, true> {
    static void
    f1()
    {
        /* Specific helper function for case when S is prime */
    }
};

template<int S>
struct Helper<S, false> {
    static void
    f1()
    {
        /* Specific helper function for case when S is not prime */
    }
};

//--------------------------------------------------------------------------------------------------

TEST(TemplateMeta, EnableIf)
{
    std::string name{"name1"};
    Person p1{name};
    Person p2{"name2"};
    Person p3{p1};
    EXPECT_EQ(p3.name(), "name1");
    Person p4{std::move(p1)};
}

TEST(TemplateMeta, Traits)
{
    static_assert(std::is_same_v<std::remove_const_t<std::remove_reference_t<const int&>>,
                                 std::remove_const_t<std::remove_reference_t<int const&>>>);
    static_assert(std::is_same_v<std::remove_const_t<std::remove_reference_t<const int&>>,
                                 std::decay_t<int const&>>);
}

TEST(TemplateMeta, AddressOf)
{
    Ptr<int> ptr;

    auto* p1 = &ptr;
    auto* p2 = std::addressof(ptr);

    static_assert(std::is_same_v<int**, decltype(p1)>);
    static_assert(std::is_same_v<Ptr<int>*, decltype(p2)>);
}

TEST(TemplateMeta, PerfectForwarding)
{
    /* Only default constructors and multiple move constructors should be present */
    SomeType value;
    perfectForward(std::move(value));
}

TEST(TemplateMeta, Union)
{
    DataChunks<int> d{0};
    d.data = 258;
    EXPECT_EQ(d.chunks[0], 2);
    EXPECT_EQ(d.chunks[1], 1);
}

TEST(TemplateMeta, CompileComuting)
{
    static_assert(IsPrime<7>::value);
    static_assert(IsPrime<3>::value);

    static_assert(!IsPrime<8>::value);
    static_assert(!IsPrime<4>::value);

    static_assert(isPrime(7));
    static_assert(isPrime(3));

    static_assert(!isPrime(8));
    static_assert(!isPrime(4));
}
