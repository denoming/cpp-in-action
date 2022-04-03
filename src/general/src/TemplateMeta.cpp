#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <iostream>
#include <type_traits>

using namespace testing;

//--------------------------------------------------------------------------------------------------

class Person {
public:
    template<typename T>
    using EnableIfString = std::enable_if_t<std::is_convertible_v<T, std::string>>;

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

    SomeType& operator=(const SomeType&)
    {
        std::cout << "Copy operator" << std::endl;
        return *this;
    }

    SomeType(SomeType&&) noexcept
    {
        std::cout << "Move constructor" << std::endl;
    }

    SomeType& operator=(SomeType&&) noexcept
    {
        std::cout << "Move operator" << std::endl;
        return *this;
    }
};

template<typename T>
auto get(T in)
{
    return in;
}

template<typename T>
void set(T in)
{
    // some work //
}

template<typename T>
void perfectForward(T value)
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
