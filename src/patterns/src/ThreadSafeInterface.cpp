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


#include <mutex>

/* Use NVI (Non-Virtual Interface) to define thread safe interface */
class ThreadSafeInterface {
public:
    virtual ~ThreadSafeInterface() = default;

    void
    method1()
    {
        std::lock_guard lock{_mutex};
        onMethod1();
    }

    void
    method2()
    {
        std::lock_guard lock{_mutex};
        onMethod2();
    }

private:
    virtual void
    onMethod1()
    {
        // No implementation
    }

    virtual void
    onMethod2()
    {
        // No implementation
    }

private:
    mutable std::mutex _mutex;
};

/* Use thread safe interface in derived class with particular implementation */
class Derived1 final : public ThreadSafeInterface {
private:
    void
    onMethod1() final
    {
        // Some implementation
    }

    void
    onMethod2() final
    {
        // Some implementation
    }
};
