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