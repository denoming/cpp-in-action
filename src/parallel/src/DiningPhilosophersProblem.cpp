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

#include <chrono>
#include <random>
#include <mutex>
#include <semaphore>
#include <thread>
#include <iostream>
#include <syncstream>

static const int kWorkers{5};

enum class States {
    Idle,
    Preparing,
    Busy,
};

#define LEFT (n + kWorkers - 1) % kWorkers // number of i's left neighbor
#define RIGHT (n + 1) % kWorkers           // number of i's right neighbor

static States state[kWorkers];
static std::mutex guard;
static std::binary_semaphore s[kWorkers]{std::binary_semaphore{0},
                                         std::binary_semaphore{0},
                                         std::binary_semaphore{0},
                                         std::binary_semaphore{0},
                                         std::binary_semaphore{0}}; // one semaphore per worker

int
random(int min, int max)
{
    static std::mt19937 rnd(std::time(nullptr));
    return std::uniform_int_distribution<>(min, max)(rnd);
}

void
test(int n)
{
    // Worker 0: check if state(4) == Idle and state(1) == Idle
    // Worker 1: check if state(0) == Idle and state(2) == Idle
    // Worker 2: check if state(1) == Idle and state(3) == Idle
    // Worker 3: check if state(2) == Idle and state(4) == Idle
    // Worker 4: check if state(3) == Idle and state(0) == Idle
    if (state[n] == States::Idle && state[LEFT] != States::Busy && state[RIGHT] != States::Busy) {
        /* Acquire resource only if partial ordering is satisfied */
        state[n] = States::Busy;
        s[n].release();
    }
}

/* Acquire some mutual exclusive resource */
void
acquire(int n)
{
    std::unique_lock lock{guard};
    /* Record the fact that worker n is idle */
    state[n] = States::Idle;
    /* Try to acquire mutual exclusive resource */
    test(n);
    lock.unlock();
    /* Block if mutual exclusive resource were not acquired */
    s[n].acquire();
}

/* Release some mutual exclusive resource */
void
release(int n)
{
    std::lock_guard lock{guard};
    /* Worker has finished eating */
    state[n] = States::Preparing; // worker has finished eating
    /* See if left neighbor can now work */
    test(LEFT);
    /* See if right neighbor can now work */
    test(RIGHT);
}

void
preparing(int n)
{
    int duration = random(1000, 2000);
    std::osyncstream{std::cout} << n << " preparing " << duration << "ms\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(duration));
}

void
working(int n)
{
    int duration = random(1000, 2000);
    std::osyncstream{std::cout} << "\t\t\t\t" << n << " working " << duration << "ms\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(duration));
}

/* n: worker number, from 0 to N-1 */
[[noreturn]] void
worker(int n)
{
    while (true) {
        preparing(n);
        acquire(n);
        working(n);
        release(n);
    }
}

/**
 * Each worker need acquire mutual exclusive resource as well as his neighbor (left and right). This,
 * we have circular dependency between workers.
 *
 * Tha way to avoid the circular wait is to provide a global numbering of all the resources.
 * Now the rule is this: processes can request resources whenever they want to, but all requests
 * must be made in numerical order (partial order).
 *
 * This solution is known as resource hierarchy or partial ordering. The first worker taken has to be
 * the worker with the lower number. For workers 1 to 3 the resources are taken in the correct order.
 * Only worker 4 needs a change for correct partial ordering.
 */
TEST(DiningPhilosophersProblem, DISABLED_Run)
{
    std::jthread t1{&worker, 0};
    std::jthread t2{&worker, 1};
    std::jthread t3{&worker, 2};
    std::jthread t4{&worker, 3};
    std::jthread t5{&worker, 4};
}
