#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "SequenceTraits.hpp"

using namespace testing;

TEST(SequenceTraitsTest, CheckPrecedes)
{
    using Traits = SequenceTraits<uint8_t>;
    EXPECT_THAT(Traits::precedes(0u, 127u), IsTrue());
    EXPECT_THAT(Traits::precedes(0u, 128u), IsTrue());
    EXPECT_THAT(Traits::precedes(0u, 129u), IsFalse());

    EXPECT_THAT(Traits::precedes(128u, 255u), IsTrue());
    EXPECT_THAT(Traits::precedes(128u, 0u), IsTrue());
    EXPECT_THAT(Traits::precedes(128u, 1u), IsFalse());
}
