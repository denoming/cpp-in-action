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
