#include <gtest/gtest.h>

#include <thread>
#include <syncstream>

static thread_local std::string local{"Hello from "};

void thread1(const std::string& name)
{
    local += name;
    std::osyncstream scout{std::cout};
    scout << local << '(' << &local << ')' << std::endl;
}

TEST(ThreadLocal, Local)
{
    std::jthread t1{&thread1, "T1"};
    std::jthread t2{&thread1, "T2"};
    std::jthread t3{&thread1, "T3"};
}
