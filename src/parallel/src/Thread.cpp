#include <gtest/gtest.h>

#include <thread>
#include <syncstream>
#include <iostream>

using namespace std::literals;

//--------------------------------------------------------------------------------------------------

static void
fn1()
{
    std::cout << "Thread 1" << std::endl;
    std::this_thread::sleep_for(1s);
}

class Functor {
public:
    void
    operator()() const
    {
        std::cout << "Thread 2" << std::endl;
        std::this_thread::sleep_for(1s);
    }
};

TEST(Thread, StartThread)
{
    // Using external function
    std::thread t1{&fn1};

    // Using callable functor
    std::thread t2{Functor{}};

    // Using lambda
    std::thread t3{[]() {
        std::cout << "Thread 3" << std::endl;
        std::this_thread::sleep_for(1s);
    }};

    t1.join();
    t2.join();
    t3.join();
}

//--------------------------------------------------------------------------------------------------

static void
fn2()
{
    std::cout << "(J)Thread 2" << std::endl;
    std::this_thread::sleep_for(1s);
}

static void
fn3(std::stop_token /*stoken*/)
{
    std::cout << "(J)Thread 3" << std::endl;
    std::this_thread::sleep_for(1s);
}

TEST(Thread, StartImprovedThread)
{
    std::jthread t2{&fn2};
    std::jthread t3{&fn3};
}

//--------------------------------------------------------------------------------------------------

TEST(Thread, CooperativeInterruption)
{
    std::jthread t1{[]() {
        std::osyncstream scout{std::cout};
        int cnt{};
        while (cnt++ < 10) {
            std::this_thread::sleep_for(0.2s);
            scout << "t1: " << cnt << std::endl;
        }
    }};

    std::jthread t2{[](std::stop_token stoken) {
        std::osyncstream scout{std::cout};
        int cnt{};
        while (cnt++ < 10) {
            std::this_thread::sleep_for(0.2s);
            if (stoken.stop_requested()) {
                break;
            }
            scout << "t2: " << cnt << std::endl;
        }
    }};

    std::this_thread::sleep_for(1s);

    std::osyncstream scout{std::cout};
    scout << "Interrupt both threads" << std::endl;
    t1.request_stop();
    t2.request_stop();
}