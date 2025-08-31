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
#include <atomic>

/**
 * The simplest way to implement thread safe singleton
 * (the quickest implementation)
 */
class MeyersSingleton {
public:
    static MeyersSingleton&
    instance()
    {
        /** Static variable with local scope will be initialised in a thread-safe way */
        static MeyersSingleton object;
        return object;
    }

public:
    MeyersSingleton() = default;
    ~MeyersSingleton() = default;

    MeyersSingleton(const MeyersSingleton&) = delete;
    MeyersSingleton&
    operator=(const MeyersSingleton&)
        = delete;
};

/**
 * Singleton implementation based on atomic and double check
 */
class DoubleLockSingleton {
public:
    static DoubleLockSingleton&
    instance()
    {
        auto* s = s_instance.load(std::memory_order_acquire);
        // 1st check
        if (s == nullptr) {
            std::lock_guard lock{s_mutex};
            s = s_instance.load(std::memory_order_relaxed);
            // 2nd (double) check
            if (s != nullptr) {
                s = new DoubleLockSingleton;
                s_instance.store(s, std::memory_order_release);
            }
        }
        return *s;
    }

    static void destroy()
    {
        auto* s = s_instance.load(std::memory_order_acquire);
        if (s != nullptr) {
            std::lock_guard lock{s_mutex};
            s = s_instance.load(std::memory_order_relaxed);
            if (s != nullptr) {
                delete s;
                s_instance.store(nullptr, std::memory_order_release);
            }
        }
    }

private:
    DoubleLockSingleton() = default;
    ~DoubleLockSingleton() = default;

    DoubleLockSingleton(const DoubleLockSingleton&) = delete;
    DoubleLockSingleton&
    operator=(const DoubleLockSingleton&)
        = delete;

private:
    static std::atomic<DoubleLockSingleton*> s_instance;
    static std::mutex s_mutex;
};

std::atomic<DoubleLockSingleton*> DoubleLockSingleton::s_instance;
std::mutex DoubleLockSingleton::s_mutex;