#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::IsEmpty;

#include <string>

//-----------------------------------------------------------------------------

struct Data {
    std::string name;
    double value;
};

struct DataEx : Data {
    bool done;
};

TEST(AggreagateExtensionTest, DefaultInit1)
{
    DataEx ex1{"Text", 0.5, true};
    EXPECT_EQ(ex1.name, "Text");
    EXPECT_NEAR(ex1.value, 0.5, 0.1);
    EXPECT_TRUE(ex1.done);

    DataEx ex2{};
    EXPECT_THAT(ex2.name, IsEmpty());
    EXPECT_NEAR(ex2.value, 0.0, 0.1);
    EXPECT_FALSE(ex2.done);
}

//-----------------------------------------------------------------------------

struct Part1 {
    double someValue1;
};

struct Part2 {
    bool someValue2;
};

struct Unit : Part1, Part2 {
    int someValue3;
    int someValue4;
};

TEST(AggreagateExtensionTest, DefaultInit2)
{
    Unit unit1{{0.5}, {false}, 15, 5};
    EXPECT_NEAR(unit1.someValue1, 0.5, 0.1);
    EXPECT_FALSE(unit1.someValue2);
    EXPECT_EQ(unit1.someValue3, 15);
    EXPECT_EQ(unit1.someValue4, 5);

    Unit unit2{1.5, true, 30, 10};
    EXPECT_NEAR(unit2.someValue1, 1.5, 0.1);
    EXPECT_TRUE(unit2.someValue2);
    EXPECT_EQ(unit2.someValue3, 30);
    EXPECT_EQ(unit2.someValue4, 10);
}
