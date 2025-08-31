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
