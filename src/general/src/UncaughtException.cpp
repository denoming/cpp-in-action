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

#include <iostream>

class Request {
public:
    Request(unsigned& commits, unsigned& rollbacks)
        : _commits{commits}
        , _rollbacks{rollbacks}
    {
    }

    virtual void
    commit()
    {
        _commits++;
    };

    virtual void
    rollback()
    {
        _rollbacks++;
    };

    virtual ~Request()
    {
        if (std::uncaught_exceptions() > initialUncaught) {
            rollback();
        } else {
            commit();
        }
    }

private:
    unsigned& _commits;
    unsigned& _rollbacks;
    int initialUncaught{std::uncaught_exceptions()};
};

TEST(UncaughtExceptionTest, Dummy1)
{
    unsigned commits{0}, rollbacks{0};
    {
        Request r1{commits, rollbacks};
    }
    EXPECT_EQ(commits, 1);
    EXPECT_EQ(rollbacks, 0);
}

TEST(UncaughtExceptionTest, Dummy2)
{
    unsigned commits{0}, rollbacks{0};
    try {
        Request r1{commits, rollbacks};
        throw std::runtime_error{""};
    } catch (...) {
        Request r2{commits, rollbacks};
    }

    EXPECT_EQ(commits, 1);
    EXPECT_EQ(rollbacks, 1);
}
