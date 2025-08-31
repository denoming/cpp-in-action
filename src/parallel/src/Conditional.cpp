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