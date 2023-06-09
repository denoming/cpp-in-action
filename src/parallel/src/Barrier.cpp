#include <gtest/gtest.h>

#include <iostream>
#include <mutex>
#include <barrier>
#include <thread>

using namespace std::literals;

static std::mutex coutGuard;

static void
log(const std::string& line)
{
    std::lock_guard lock{coutGuard};
    std::cout << line;
}

//--------------------------------------------------------------------------------------------------

static std::barrier workDone{6, [](){
    log("Complete!\n");
}};

class FullTimeWorker {
public:
    explicit FullTimeWorker(std::string name)
        : _name{std::move(name)}
    {
    }

    void
    operator()()
    {
        log(_name + ": Morning work is done\n");
        workDone.arrive_and_wait();
        log(_name + ": Afternoon work is done\n");
        workDone.arrive_and_wait();
    }

private:
    std::string _name;
};

class PartTimeWorker {
public:
    explicit PartTimeWorker(std::string name)
        : _name{std::move(name)}
    {
    }

    void
    operator()()
    {
        log(_name + ": Morning work is done\n");
        workDone.arrive_and_drop();
    }

private:
    std::string _name;
};

TEST(Barrier, Workers)
{
    log("Start Working\n");
    std::jthread t1{FullTimeWorker{"Full time worker 1"}};
    std::jthread t2{FullTimeWorker{"Full time worker 2"}};
    std::jthread t3{FullTimeWorker{"Full time worker 3"}};
    std::jthread t4{PartTimeWorker{"Part time worker 1"}};
    std::jthread t5{PartTimeWorker{"Part time worker 2"}};
    std::jthread t6{PartTimeWorker{"Part time worker 3"}};
}

//--------------------------------------------------------------------------------------------------