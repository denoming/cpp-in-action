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
#include <syncstream>

static thread_local std::string local{"Hello from "};

void thread1(const std::string& name)
{
    local += name;
    std::osyncstream scout{std::cout};
    scout << local << '(' << &local << ')' << std::endl;
}

TEST(ThreadLocal, Local)
{
    std::jthread t1{&thread1, "T1"};
    std::jthread t2{&thread1, "T2"};
    std::jthread t3{&thread1, "T3"};
}
