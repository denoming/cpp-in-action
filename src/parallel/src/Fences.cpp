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

#include <atomic>
#include <thread>
#include <csignal>
#include <cassert>

//--------------------------------------------------------------------------------------------------

static std::atomic<std::string*> ptr;
static int data{};

static void
thread1()
{
    auto* n = new std::string{"John Dow"};
    data = 1;
    std::atomic_thread_fence(std::memory_order_release);
    ptr.store(n, std::memory_order_relaxed);
}

static void
thread2()
{
    std::string* n{};
    while (!(n = ptr.load(std::memory_order_relaxed))) { };
    std::atomic_thread_fence(std::memory_order_acquire);
    std::cout << data << ": " << *n << std::endl;
}

TEST(Fences, Using)
{
    std::jthread t1{&thread1};
    std::jthread t2{&thread2};
}

//--------------------------------------------------------------------------------------------------

static std::atomic<bool> a{false};
static std::atomic<bool> b{false};

extern "C" void
handler(int)
{
    if (a.load(std::memory_order_relaxed)) {
        std::atomic_signal_fence(std::memory_order_acquire);
        assert(b.load(std::memory_order_relaxed));
    }
}

//int
//main()
//{
//    std::signal(SIGTERM, handler);
//    b.store(true, std::memory_order_relaxed);
//    std::atomic_signal_fence(std::memory_order_release);
//    a.store(true, std::memory_order_relaxed);
//}
