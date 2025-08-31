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

#pragma once

#include <string>
#include <memory_resource>

class MemoryResourcesTracker : public std::pmr::memory_resource {
public:
    explicit MemoryResourcesTracker(std::pmr::memory_resource* upstream
                                    = std::pmr::get_default_resource());

    explicit MemoryResourcesTracker(std::string prefix,
                                    std::pmr::memory_resource* upstream
                                    = std::pmr::get_default_resource());

    void
    trace(bool enabled);

    uint64_t
    allocated() const;

    uint64_t
    deallocated() const;

    uint64_t
    retain() const;

    void
    reset();

    void
    status();

private:
    void*
    do_allocate(size_t bytes, size_t alignment) override;

    void
    do_deallocate(void* p, size_t bytes, size_t alignment) override;

    bool
    do_is_equal(const memory_resource& other) const noexcept override;

private:
    std::string _prefix;
    std::pmr::memory_resource* _upstream;
    uint64_t _allocated;
    uint64_t _deallocated;
    uint64_t _retain;
    bool _trace;
};
