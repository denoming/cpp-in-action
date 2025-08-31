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
#include <map>
#include <filesystem>
#include <vector>

TEST(IfSwitchRangeForWithInit, InsertWithCheck)
{
    std::map<std::string, unsigned> coll;
    coll.insert({"New", 42});
    if (auto [p, ok] = coll.insert({"New", 38}); !ok) {
        const auto& [key, val] = *p;
        EXPECT_EQ(val, 42);
        return;
    }
    FAIL() << "Not expected";
}

TEST(IfSwitchRangeForWithInit, Switch)
{
    namespace fs = std::filesystem;
    switch (fs::path p{"/tmp"}; status(p).type()) {
    case fs::file_type::not_found:
        SUCCEED() << "Expected => Directory not found";
        break;
    case fs::file_type::directory:
        SUCCEED() << "Expected => Directory found";
        break;
    default:
        FAIL() << "Not expected";
    }
}

TEST(IfSwitchRangeForWithInit, RangeFor)
{
    int32_t sum{0};
    for (auto numbers = std::vector{1, 5, 7}; auto number : numbers) {
        sum += number;
    }
    EXPECT_EQ(sum, 13);
}
