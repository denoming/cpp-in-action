#include <gtest/gtest.h>

#include <iostream>

TEST(RelaxedEnumTest, Test)
{
    enum class Weekday { mon = 1,
                         tue,
                         wed,
                         thu,
                         fri,
                         sat,
                         sun };
    Weekday e1{1};
    EXPECT_EQ(e1, Weekday::mon);
    Weekday e2{5};
    EXPECT_EQ(e2, Weekday::fri);
    e2 = Weekday{4};
    EXPECT_EQ(e2, Weekday::thu);
}
