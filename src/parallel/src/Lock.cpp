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

#include <mutex>
#include <shared_mutex>
#include <thread>
#include <syncstream>
#include <atomic>

using namespace testing;
using namespace std::literals;

//--------------------------------------------------------------------------------------------------

TEST(Lock, UniqueLock)
{
    static std::mutex m1;
    std::unique_lock lock1{m1, std::defer_lock};
    std::cout << "No lock (1)" << std::endl;
    lock1.lock();
    std::cout << "With lock (1)" << std::endl;
    std::this_thread::sleep_for(1s);
    lock1.release();

    std::unique_lock lock2{m1, std::adopt_lock};
    std::cout << "With lock (2)" << std::endl;
    std::this_thread::sleep_for(1s);
    lock2.unlock();
    std::cout << "No lock (2)" << std::endl;
}

//--------------------------------------------------------------------------------------------------

static std::timed_mutex m4;

static void
thread3()
{
    std::unique_lock lock{m4, std::defer_lock};
    std::cout << "(1) Try to lock" << std::endl;
    if (lock.try_lock()) {
        std::this_thread::sleep_for(1s);
    } else {
        std::cout << "(1) Lock didn't obtained" << std::endl;
        while (true) {
            if (lock.try_lock_for(0.1s)) {
                std::cout << "(1) Locked" << std::endl;
                break;
            } else {
                std::cout << "(1) Not lucky" << std::endl;
            }
        }
    }
}

static void
thread4()
{
    std::unique_lock lock{m4, std::defer_lock};
    std::cout << "(2) Try to lock" << std::endl;
    if (lock.try_lock()) {
        std::this_thread::sleep_for(1s);
    } else {
        std::cout << "(2) Lock didn't obtained" << std::endl;
        while (true) {
            if (lock.try_lock_for(0.1s)) {
                std::cout << "(2) Locked" << std::endl;
                break;
            } else {
                std::cout << "(2) Not lucky" << std::endl;
            }
        }
    }
}

TEST(Lock, TimedLock)
{
    std::jthread t1{&thread3};
    std::jthread t2{&thread4};
}

//--------------------------------------------------------------------------------------------------

static std::shared_mutex m5;

static void
thread5()
{
    std::osyncstream scout{std::cout};
    int cnt{};
    while (cnt++ < 5) {
        {
            std::lock_guard lock{m5};
            scout << "Exclusive lock" << std::endl;
        }
        std::this_thread::sleep_for(0.1s);
    }
}

static void
thread6()
{
    std::osyncstream scout{std::cout};
    int cnt{};
    while (cnt++ < 10) {
        {
            std::shared_lock lock{m5};
            scout << "Read lock (1)" << std::endl;
        }
        std::this_thread::sleep_for(50ms);
    }
}

static void
thread7()
{
    std::osyncstream scout{std::cout};
    int cnt{};
    while (cnt++ < 10) {
        {
            std::shared_lock lock{m5};
            scout << "Read lock (2)" << std::endl;
        }
        std::this_thread::sleep_for(50ms);
    }
}

TEST(Lock, SharedLock)
{
    std::jthread t1{&thread5};
    std::jthread t2{&thread6};
    std::jthread t3{&thread7};
}

//--------------------------------------------------------------------------------------------------

struct CriticalData {
    std::mutex guard;
};

void
deadLock1(CriticalData& d1, CriticalData& d2)
{
    std::unique_lock lock1{d1.guard, std::defer_lock}; // Become owner of mutex
    std::cout << "T: " << std::this_thread::get_id() << " (1)\n";

    std::unique_lock lock2{d2.guard, std::defer_lock}; // Become owner of mutex
    std::cout << "T: " << std::this_thread::get_id() << " (2)\n";

    std::lock(lock1, lock2);
}

void
deadLock2(CriticalData& d1, CriticalData& d2)
{
    std::scoped_lock lock{d1.guard, d2.guard};
    std::cout << "T: " << std::this_thread::get_id() << " (1+2)\n";
}

TEST(Lock, MultipleLocks1)
{
    CriticalData d1;
    CriticalData d2;
    std::jthread t1{[&]() { deadLock1(d1, d2); }};
    std::jthread t2{[&]() { deadLock1(d2, d1); }};
}

TEST(Lock, MultipleLocks2)
{
    CriticalData d1;
    CriticalData d2;
    std::jthread t1{[&]() { deadLock2(d1, d2); }};
    std::jthread t2{[&]() { deadLock2(d2, d1); }};
}


//--------------------------------------------------------------------------------------------------
