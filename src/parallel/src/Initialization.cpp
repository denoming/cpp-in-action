#include <gtest/gtest.h>

#include <thread>
#include <mutex>

static std::once_flag onceFlag;

static void
doOnce1()
{
    std::call_once(onceFlag, []() { std::cout << "Only once (1)" << std::endl; });
}

static void
doOnce2()
{
    std::call_once(onceFlag, []() { std::cout << "Only once (2)" << std::endl; });
}

TEST(Initialization, Once)
{
    std::jthread t1{&doOnce1};
    std::jthread t2{&doOnce1};
    std::jthread t3{&doOnce2};
    std::jthread t4{&doOnce2};
}