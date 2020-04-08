#include <gmock/gmock.h>
#include <gtest/gtest.h>

class MyClass {
public:
    MyClass(std::string value)
        : _value{std::move(value)}
    {
    }

    MyClass(const MyClass&) = delete;
    MyClass(MyClass&&) = delete;

    MyClass&
    operator=(const MyClass&)
        = delete;
    MyClass&
    operator=(MyClass&&)
        = delete;

    const std::string&
    value() const
    {
        return _value;
    }

private:
    std::string _value;
};

void
foo(MyClass param)
{
    EXPECT_EQ(param.value(), "Text");
}

MyClass
bar()
{
    return MyClass{"Text"};
}

TEST(CopyElisionTest, Test1)
{
    foo(bar());
}
