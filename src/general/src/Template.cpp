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

#include <vector>
#include <array>
#include <deque>
#include <string>
#include <numeric>
#include <type_traits>

#include <iostream>
#include <iomanip>

using namespace testing;

// Example of variable template with default type
template<typename T = long double>
constexpr T Pi = T{3.1415926535897932385};

// Example of variable template for data member
// Using: isSigned<char> instead of std::numeric_limits<char>::is_signed
template<typename T>
constexpr bool isSigned = std::numeric_limits<T>::is_signed;

// Simple max function implementation with auto (best solution)
// Simple max function implementation with auto (best solution)
template<typename T1, typename T2>
auto
max1(T1 a, T2 b)
{
    return b < a ? a : b;
}

// Simple max function implementation with common return type deducing
template<typename T1, typename T2>
std::common_type_t<T1, T2>
max2(T1 a, T2 b)
{
    return b < a ? a : b;
}

// Simple max function implementation with explicit template parameter for return type
template<typename T1, typename T2, typename R = std::common_type_t<T1, T2>>
R
max3(T1 a, T2 b)
{
    return b < a ? a : b;
}

// Simple example with default values for template parameter
template<auto V, typename T = decltype(V)>
T
add(T input)
{
    return (input + V);
}

template<typename T, std::size_t N, std::size_t M>
bool
less(const T (&a)[N], const T (&b)[M])
{
    for (std::size_t i = 0; i < N && i < M; ++i) {
        if (a[i] < b[i])
            return true;
        if (b[i] < a[i])
            return false;
    }
    return (N < M);
}

/* Basic class template */
template<typename T, template<typename E> typename C = std::vector>
class Stack {
public:
    static_assert(std::is_default_constructible_v<T>, "Default constructible elements required");

    // Makes possible to access to private fields of Stack<U> class in assigning operator
    template<typename, template<typename> typename>
    friend class Stack;

    void
    push(const T& e)
    {
        _elements.push_back(e);
    }

    void
    push(T&& e)
    {
        _elements.push_back(std::move(e));
    }

    void
    pop()
    {
        assert(!_elements.empty());
        _elements.pop_back();
    }

    [[nodiscard]] const T&
    top() const
    {
        assert(!_elements.empty());
        return _elements.back();
    }

    [[nodiscard]] bool
    empty() const
    {
        return _elements.empty();
    }

    void
    print(std::ostream& os) const
    {
        for (std::size_t i = 1; i <= _elements.size(); ++i) {
            os << std::setw(3) << i << ": " << _elements[_elements.size() - i] << '\n';
        }
    }

    template<typename U, template<typename> typename X>
    Stack&
    operator=(const Stack<U, X>& other)
    {
        _elements.clear();
        _elements.insert(_elements.begin(), other._elements.begin(), other._elements.end());
        return *this;
    }

    template<typename U>
    friend std::ostream&
    operator<<(std::ostream& os, const Stack<U>& stack);

private:
    C<T> _elements;
};

// Define outside to avoid multiple definition for different type of template parameter
template<typename U>
std::ostream&
operator<<(std::ostream& os, const Stack<U>& stack)
{
    stack.print(os);
    return os;
}

/* Class template specialization for T* */
template<typename T, template<typename E> typename C>
class Stack<T*, C> {
public:
    void
    push(T* e)
    {
        _elements.push_back(e);
    }

    void
    pop()
    {
        assert(!_elements.empty());
        _elements.pop_back();
    }

    [[nodiscard]] T*
    top() const
    {
        assert(!_elements.empty());
        return _elements.back();
    }

    [[nodiscard]] bool
    empty() const
    {
        return _elements.empty();
    }

    void
    print(std::ostream& os) const
    {
        for (std::size_t i = 1; i <= _elements.size(); ++i) {
            os << std::setw(3) << i << ": " << *_elements[_elements.size() - i] << '\n';
        }
    }

    template<typename U>
    friend std::ostream&
    operator<<(std::ostream& os, const Stack<U*>& stack)
    {
        stack.print(os);
        return os;
    }

private:
    C<T*> _elements;
};

/* Class template specialization for std::string */
template<>
class Stack<std::string, std::deque> {
public:
    Stack() = default;

    Stack(std::string e) // NOLINT
        : _elements{std::move(e)}
    {
    }

    void
    push(const std::string& e)
    {
        _elements.push_back(e);
    }

    void
    push(std::string&& e)
    {
        _elements.push_back(std::move(e));
    }

    void
    pop()
    {
        assert(!_elements.empty());
        _elements.pop_back();
    }

    [[nodiscard]] const std::string&
    top() const
    {
        assert(!_elements.empty());
        return _elements.back();
    }

    [[nodiscard]] bool
    empty() const
    {
        return _elements.empty();
    }

    void
    print(std::ostream& os) const
    {
        for (std::size_t i = 1; i <= _elements.size(); ++i) {
            os << std::setw(3) << i << ": " << _elements[_elements.size() - i] << '\n';
        }
    }

    friend std::ostream&
    operator<<(std::ostream& os, const Stack<std::string, std::deque>& stack)
    {
        stack.print(os);
        return os;
    }

private:
    std::deque<std::string> _elements;
};

/* Class template with non-type template parameter */
template<typename T, auto MaxSize>
class FixedStack {
public:
    using size_type = decltype(MaxSize);

    FixedStack()
        : _index{0}
    {
    }

    void
    push(const T& e)
    {
        assert(_index < MaxSize);
        _elements[_index] = e;
        _index++;
    }

    void
    push(T&& e)
    {
        assert(_index < MaxSize);
        _elements[_index] = std::move(e);
        _index++;
    }

    void
    pop()
    {
        assert(_index >= 0);
        _index--;
    }

    [[nodiscard]] const T&
    top() const
    {
        assert(_index > 0);
        return _elements[_index - 1];
    }

    [[nodiscard]] bool
    empty() const
    {
        return (_index == 0);
    }

    [[nodiscard]] std::size_t
    size() const
    {
        return _index;
    }

private:
    std::size_t _index;
    std::array<T, MaxSize> _elements;
};

template<typename T>
struct ValueWithComment {
    T value;
    std::string comment;

    template<typename U>
    friend std::ostream&
    operator<<(std::ostream& os, const ValueWithComment<U>& v)
    {
        os << "Value: " << v.value << " (" << v.comment << ")\n";
        return os;
    }
};

/* Deduction guides for Stack class template */
template<template<typename E> class C = std::deque>
Stack(const char*) -> Stack<std::string, C>;
/* Deduction guides for ValueWithComment class template */
ValueWithComment(const char*, const char*) -> ValueWithComment<std::string>;

class BoolString {
public:
    explicit BoolString(std::string value)
        : _value{std::move(value)}
    {
    }

    // Example of member function template with full specialization
    template<typename T = std::string>
    [[nodiscard]] T
    get() const
    {
        return _value;
    }

private:
    std::string _value;
};

// Full specialization of function member template
template<>
bool
BoolString::get<bool>() const
{
    return (_value == "true" || _value == "1" || _value == "on");
}

class JustBool {
public:
    template<typename T>
    explicit JustBool(T any)
    {
        if (any == "on") {
            _value = true;
        } else {
            _value = false;
        }
    }

    bool
    isTrue() const
    {
        return (_value == true);
    }

private:
    bool _value;
};

// Example of 'for each' implementation
// Do not use perfect forwarding (it might "steal" their values, leading to unexpected behaviour)
template<typename Iter, typename Callable, typename... Args>
void
forEach(Iter begin, Iter end, Callable op, const Args&... args)
{
    while (begin != end) {
        std::invoke(op, args..., *begin);
        ++begin;
    }
}

class Callable {
public:
    explicit Callable(int multiplier)
        : _multiplier{multiplier}
    {
    }

    void
    multiplyAndOut(int value) const
    {
        value *= _multiplier;
        std::cout << value << std::endl;
    }

    void
    operator()(int value) const
    {
        multiplyAndOut(value);
    }

private:
    int _multiplier;
};

TEST(Template, Function)
{
    bool b1 = std::is_same_v<decltype(::max1(7.0, 4)), decltype(::max1(4, 7.0))>;
    EXPECT_TRUE(b1);
    bool b2 = std::is_same_v<decltype(::max2(7.0, 4)), decltype(::max2(4, 7.0))>;
    EXPECT_TRUE(b2);
    bool b3 = std::is_same_v<decltype(::max3(7.0, 4)), decltype(::max3(4, 7.0))>;
    EXPECT_TRUE(b3);

    EXPECT_THAT(::max1(4, 7.0), DoubleEq(7.0));
    EXPECT_THAT(::max1(7.0, 4), DoubleEq(7.0));
    EXPECT_THAT(::max2(4, 7.0), DoubleEq(7.0));
    EXPECT_THAT(::max2(7.0, 4), DoubleEq(7.0));
    EXPECT_THAT(::max3(4, 7.0), DoubleEq(7.0));
    EXPECT_THAT(::max3(7.0, 4), DoubleEq(7.0));
}

TEST(Template, Class)
{
    using IntStack = Stack<int>;
    using StrStack = Stack<std::string>;
    using PtrStack = Stack<int*>;

    std::cout << "First stack elements: \n";

    IntStack stack1;
    stack1.push(1);
    stack1.push(2);
    std::cout << stack1;

    std::cout << "Second stack elements: \n";

    StrStack stack2;
    stack2.push("Hi");
    stack2.push("World");
    std::cout << stack2;

    std::cout << "Third stack elements: \n";

    auto v1 = std::make_unique<int>(1);
    auto v2 = std::make_unique<int>(2);
    auto v3 = std::make_unique<int>(3);

    PtrStack stack3;
    stack3.push(v1.get());
    stack3.push(v2.get());
    stack3.push(v3.get());
    std::cout << stack3;
}

TEST(Template, ClassWithNonTypeParameter)
{
    using IntFixedStack = FixedStack<int, 3>;

    IntFixedStack stack;
    stack.push(10);
    stack.push(20);
    stack.push(30);

    EXPECT_EQ(stack.size(), 3);
}

TEST(Template, DeductionGuide)
{
    const char* value1 = "Value1";
    const char* value2 = "Value2";
    const char* value3 = "Value3";

    Stack stack{value1};
    stack.push(value2);
    stack.push(value3);
    std::cout << stack;

    ValueWithComment value{"hello", "initial value"};
    std::cout << value;
}

TEST(Template, AssigningOperator)
{
    using Int16Stack = Stack<int16_t, std::vector>;
    using Int32Stack = Stack<int32_t, std::deque>;

    Int16Stack stack1;
    stack1.push(1);
    stack1.push(2);

    Int32Stack stack2;
    stack2 = stack1;
    EXPECT_EQ(stack2.top(), 2);
    stack2.pop();
    EXPECT_EQ(stack2.top(), 1);
    stack2.pop();
    EXPECT_TRUE(stack2.empty());
}

TEST(Template, Transformation)
{
    using IntValues = std::vector<int>;

    IntValues values1{1, 2, 3};
    IntValues values2;

    std::transform(values1.begin(), values1.end(), std::back_inserter(values2), add<1>);

    EXPECT_THAT(values2, ElementsAre(2, 3, 4));
}

TEST(Template, Less)
{
    int values1[] = {1, 2};
    int values2[] = {1, 2, 3};
    EXPECT_THAT(less(values1, values2), IsTrue());
    EXPECT_THAT(less(values2, values1), Not(IsTrue()));
}

TEST(Template, FunctionMember)
{
    BoolString bs1{"Hi"};
    EXPECT_EQ(bs1.get(), "Hi");
    BoolString bs2{"on"};
    EXPECT_THAT(bs2.get<bool>(), IsTrue());

    JustBool justBool{"on"};
    EXPECT_TRUE(justBool.isTrue());
}

TEST(Template, Invoke)
{
    std::vector values{1, 2, 3};

    Callable callable{2};
    forEach(values.cbegin(), values.cend(), &Callable::multiplyAndOut, &callable);

    forEach(
        values.cbegin(),
        values.cend(),
        [](const std::string& prefix, int value) {
            std::cout << prefix << value * value << std::endl;
        },
        "- value: ");
}
