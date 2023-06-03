#include <gtest/gtest.h>

#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <iostream>

using namespace std::literals;

//--------------------------------------------------------------------------------------------------

static std::mutex guard;
static std::condition_variable cv;
static std::atomic<bool> dataReady{false};

void
thread1()
{
    std::unique_lock lock{guard};
    cv.wait(lock, []() { return dataReady.load(); });
    std::cout << "Work with data\n";
}

TEST(Conditional, Simple)
{
    std::jthread t1{&thread1};

    std::this_thread::sleep_for(0.1s);
    {
        std::lock_guard lock{guard};
        std::cout << "Prepare data\n";
        dataReady.store(true);
        cv.notify_one();
    }
    std::this_thread::sleep_for(1s);
}

//--------------------------------------------------------------------------------------------------