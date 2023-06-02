#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <atomic>
#include <thread>
#include <numeric>
#include <iostream>

using namespace std::literals;

//--------------------------------------------------------------------------------------------------
//       Simple spinlock implementation using ASC/REL memory order
// (.test_and_set (ACK) operation synchronize-with .clear(REL) operation)

class Spinlock {
public:
    void
    lock()
    {
        while (_flag.test_and_set(std::memory_order_acquire)) { };
    }

    void
    unlock()
    {
        _flag.clear(std::memory_order_release);
    }

private:
    std::atomic_flag _flag = ATOMIC_FLAG_INIT;
};

static Spinlock spin;

static void
work(int n)
{
    spin.lock();
    std::this_thread::sleep_for(50ms);
    std::cout << "From " << n << " thread" << std::endl;
    std::this_thread::sleep_for(50ms);
    spin.unlock();
}

TEST(Atomic, Spinlock)
{
    std::jthread t1{&work, 1};
    std::jthread t2{&work, 2};
    std::jthread t3{&work, 3};
    std::jthread t4{&work, 4};
    std::jthread t5{&work, 5};
}

//--------------------------------------------------------------------------------------------------

static std::vector<int> data(10);
static std::atomic_flag flag;

static void
produce(int n)
{
    flag.wait(true);

    std::iota(std::begin(data), std::end(data), n);

    std::cout << "--- Notify data is complete" << std::endl;
    flag.test_and_set();
    flag.notify_one();
}

static void
consume()
{
    std::cout << "--- Waiting for data" << std::endl;
    flag.wait(false);

    std::transform(std::cbegin(data),
                   std::cend(data),
                   std::cbegin(data),
                   std::begin(data),
                   [](int v1, int v2) { return v1 * v2; });
    std::cout << "--- Complete the work" << std::endl;

    for (int value : data) {
        std::cout << "Value: " << value << std::endl;
    }
}

TEST(Atomic, ThreadSync)
{
    std::jthread t1{&produce, 20};
    std::jthread t2{&consume};
}

//--------------------------------------------------------------------------------------------------
// Transitivity of happens-before relation between three threads

static std::vector<int> tData;
static std::atomic<bool> tSync1;
static std::atomic<bool> tSync2;

static void
dataProducer()
{
    tData = {1, 2, 3};
    // REL operation (inter-thread happens before *1)
    tSync1.store(true, std::memory_order_release);
}

static void
dataDelivery()
{
    // ACQ operation [*1]
    while (!tSync1.load(std::memory_order_acquire)) { };
    // REL operation (inter-thread happens before *2)
    tSync2.store(true, std::memory_order_release);
}

static void
dataConsumer()
{
    // ACK operation [*2]
    while (!tSync2.load(std::memory_order_acquire)) { };
    for (int d : tData) {
        std::cout << d << " ";
    }
    std::cout << std::endl;
}

TEST(Atomic, Transitivity)
{
    std::jthread t1{dataProducer};
    std::jthread t2{dataDelivery};
    std::jthread t3{dataConsumer};
}

//--------------------------------------------------------------------------------------------------
// Release Sequence

static std::atomic<int> na{0};
int somethingShared{};

static void
writeShared()
{
    somethingShared = 1;
    // REL operation sync-with *1 on both threads
    na.store(2, std::memory_order_release);
}

static void
readShared()
{
    // ACK operation
    while (na.fetch_sub(1, std::memory_order_acquire) <= 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds{200});
    }
    std::cout << "somethingShared: " << somethingShared << '\n';
}

TEST(Atomic, ReleaseSequecence)
{
    std::jthread t1{writeShared};
    std::jthread t2{readShared};
    std::jthread t3{readShared};
}

//--------------------------------------------------------------------------------------------------
// Test combination of memory orders
//
// As (2) "synchronized-with" (3) and (3) "sequenced-before" (4), so (2)
//   "inter-thread happens-before" (4)
// As (1) "sequenced-before" (2) and (2) "inter-thread happens-before" (4), therefore (1)
//    "inter-thread happens-before" (4)
// Therefore (4) never fails
//

std::atomic<bool> a, b;

// clang-format off

static void
thread1()
{
    a.store(true, std::memory_order_relaxed); // (1)
    b.store(true, std::memory_order_release); // (2)
}

static void
thread2()
{
    while (!b.load(std::memory_order_acquire)); // (3)
    assert(a.load(std::memory_order_relaxed));  // (4)
}

// clang-format on

TEST(Atomic, MemoryOderCombination)
{
    std::jthread t1{&thread1}, t2{&thread2};
}

//--------------------------------------------------------------------------------------------------
// Test the most relaxed memory order

std::atomic<int> counter;

void
incrementor()
{
    for (int n = 0; n < 1000; ++n) {
        counter.fetch_add(1, std::memory_order_relaxed);
    }
}

TEST(Atomic, RelaxedMemoryOrder)
{
    {
        std::vector<std::jthread> threads;
        for (int n = 0; n < 10; ++n) {
            threads.emplace_back(&incrementor);
        }
    }
    std::cout << "Final counter value: " << counter << std::endl;
}