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
        }
        else {
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
    }
    catch (...) {
        Request r2{commits, rollbacks};
    }

    EXPECT_EQ(commits, 1);
    EXPECT_EQ(rollbacks, 1);
}
