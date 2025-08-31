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

#include <semaphore>
#include <thread>
#include <iostream>
#include <deque>
#include <atomic>

using namespace testing;
using namespace std::literals;

static std::atomic_flag done;
static std::deque<int> data;
static std::counting_semaphore<1> dataSignal{0};

//--------------------------------------------------------------------------------------------------

static void
producer()
{
    int32_t n{0};
    while (!done.test()) {
        std::cout << "Data[P]: " << ++n << std::endl;
        data.push_back(n);
        dataSignal.release();
        std::this_thread::sleep_for(0.1s);
    }
}

static void
consumer()
{
    while (!done.test()) {
        if (!dataSignal.try_acquire_for(0.1s)) {
            continue;
        }
        int n = data.back();
        data.pop_back();
        std::cout << "Data[C]: " << n << std::endl;
    }
}

TEST(Semaphore, Counting)
{
    std::jthread t1{&producer};
    std::jthread t2{&consumer};

    std::this_thread::sleep_for(1s);

    done.test_and_set();
}

//--------------------------------------------------------------------------------------------------