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
#include <condition_variable>
#include <functional>
#include <iostream>

using namespace testing;

/**
 * Concept for monitor lock type
 * @tparam T any team satisfying following requirements
 */
template<typename T>
concept MonitorLock = requires(T object) {
    object.lock();
    object.unlock();
};

/**
 * Concept for monitor condition type
 * @tparam T any team satisfying following requirements
 * @tparam P any predicate callable object
 */
template<typename T, typename P = std::function<bool()>>
concept MonitorCondition = requires(T object, P predicate) {
    object.notify();
    object.wait(predicate);
};

/**
 * Ensures that at most one client can access the monitor object at aby given point in time.
 */
class SimpleLock {
public:
    void
    lock() const
    {
        std::cout << "> SimpleLock::lock\n";
        _mutex.lock();
    }

    void
    unlock() const
    {
        std::cout << "> SimpleLock::unlock\n";
        _mutex.unlock();
    }

private:
    mutable std::mutex _mutex;
};

/**
 * Allows separate threads to schedule member function invocations on the monitor object.
 */
template<MonitorLock LockType>
class SimpleCondition {
public:
    explicit SimpleCondition(LockType& lock)
        : _lock{lock}
    {
    }

    void
    notify()
    {
        std::cout << "> SimpleLock::notify\n";
        _cv.notify_one();
    }

    void
    wait(std::function<bool()> predicate)
    {
        std::cout << "> SimpleLock::wait\n";
        _cv.wait(_lock, std::move(predicate));
    }

private:
    LockType& _lock;
    std::condition_variable_any _cv;
};

/**
 * Monitor object class supports one or more member functions. Each client must access the object
 * through these member functions. Each member function runs in the client's thread.
 *
 * Each monitor object has one monitor lock, which ensures that at most one client can access the
 * monitor object at any given point in time.
 *
 * Advantages:
 * - The client is not aware of the implicit synchronization of the Monitor Object
 *   (the synchronization is fully encapsulated in the implementation)
 * - The invoked synchronized member functions will eventually be automatically scheduled
 *   (waiting mechanism of the monitor condition behaves as a simple scheduler)
 *
 * Disadvantages:
 * - It is often quite challenging to change the synchronization mechanism of the synchronization
 *   member functions because the functionality and the synchronization are strongly coupled.
 * - When a synchronized member function invokes directly or indirectly the same Monitor Object,
 *   a deadlock may occur.
 */
template<MonitorLock LockType, MonitorCondition ConditionType>
class MonitorObject {
public:
    MonitorObject()
        : _condition{_lock}
    {
    }

    void
    method1()
    {
        // Lock (getting exclusive access)
        _lock.lock();

        // Do some stuff

        // Unlock (other thread can invoke member function)
        _lock.unlock();

        // Notify other threads so that the next thread can use the monitor object
        _condition.notify(); // Notify
    }

    void
    method2()
    {
        // Waiting some condition (allows to schedule member function invocations)
        _condition.wait([]() { return true; });

        // Lock (getting exclusive access)
        _lock.lock();

        // Do other stuff;

        // Unlock (other thread can invoke member function)
        _lock.unlock();
    }

private:
    LockType _lock;
    ConditionType _condition;
};

TEST(MonitorObjectTest, Test)
{
    MonitorObject<SimpleLock, SimpleCondition<SimpleLock>> object;
    object.method1();
    object.method2();
}
