#include <gtest/gtest.h>

#include <iostream>
#include <mutex>
#include <latch>
#include <thread>
#include <syncstream>

using namespace std::literals;

//--------------------------------------------------------------------------------------------------

static std::latch workDone{3};

class Worker {
public:
    explicit Worker(std::string name)
        : _name{std::move(name)}
    {
    }

    void
    operator()()
    {
        std::osyncstream{std::cout} << _name << ": Done\n";
        workDone.arrive_and_wait();
        std::osyncstream{std::cout} << _name << ": Good Bye!\n";
    }

private:
    std::string _name;
};

TEST(Latches, Workers)
{
    std::osyncstream{std::cout} << "Start Working\n";
    std::jthread t1{Worker{"Worker 1"}};
    std::jthread t2{Worker{"Worker 2"}};
    std::jthread t3{Worker{"Worker 3"}};

    /* When all workers have done own part of work until that we blocked */
    workDone.wait();

    std::osyncstream{std::cout} << "All work is done, let's go home\n";
}

//--------------------------------------------------------------------------------------------------