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

#include <iostream>
#include <mutex>
#include <barrier>
#include <thread>
#include <syncstream>

using namespace std::literals;

//--------------------------------------------------------------------------------------------------

static std::barrier workDone{6, []() { std::osyncstream{std::cout} << "Complete!\n"; }};

class FullTimeWorker {
public:
    explicit FullTimeWorker(std::string name)
        : _name{std::move(name)}
    {
    }

    void
    operator()()
    {
        // log(_name + ": Morning work is done\n");
        std::osyncstream{std::cout} << _name << ": Morning work is done\n";
        workDone.arrive_and_wait();
        // log(_name + ": Afternoon work is done\n");
        std::osyncstream{std::cout} << _name << ": Afternoon work is done\n";
        workDone.arrive_and_wait();
    }

private:
    std::string _name;
};

class PartTimeWorker {
public:
    explicit PartTimeWorker(std::string name)
        : _name{std::move(name)}
    {
    }

    void
    operator()()
    {
        // log(_name + ": Morning work is done\n");
        std::osyncstream{std::cout} << _name << ": Morning work is done\n";
        workDone.arrive_and_drop();
    }

private:
    std::string _name;
};

TEST(Barrier, Workers)
{
    std::osyncstream{std::cout} << "Start Working\n";
    std::jthread t1{FullTimeWorker{"Full time worker 1"}};
    std::jthread t2{FullTimeWorker{"Full time worker 2"}};
    std::jthread t3{FullTimeWorker{"Full time worker 3"}};
    std::jthread t4{PartTimeWorker{"Part time worker 1"}};
    std::jthread t5{PartTimeWorker{"Part time worker 2"}};
    std::jthread t6{PartTimeWorker{"Part time worker 3"}};
}

//--------------------------------------------------------------------------------------------------