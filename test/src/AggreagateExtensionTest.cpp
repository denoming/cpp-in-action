#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::IsEmpty;

#include <string>

struct Data {
    std::string name;
    double value;
};

struct DataEx : Data {
    bool done;
};

TEST(AggreagateExtensionTest, DefaultInit)
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
