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

#include "common/MemoryResourcesTracker.hpp"

#include <iostream>

MemoryResourcesTracker::MemoryResourcesTracker(std::pmr::memory_resource* upstream)
    : _upstream{upstream}
    , _allocated{0}
    , _deallocated{0}
    , _retain{0}
    , _trace{true}
{
}

MemoryResourcesTracker::MemoryResourcesTracker(std::string prefix,
                                               std::pmr::memory_resource* upstream)
    : _prefix{prefix}
    , _upstream{upstream}
{
}

void*
MemoryResourcesTracker::do_allocate(size_t bytes, size_t alignment)
{
    _retain += bytes;
    _allocated += bytes;
    if (_trace) {
        std::clog << _prefix << "allocate " << bytes << " Bytes\n";
    }
    return _upstream->allocate(bytes, alignment);
}

void
MemoryResourcesTracker::do_deallocate(void* p, size_t bytes, size_t alignment)
{
    _retain -= bytes;
    _deallocated += bytes;
    if (_trace) {
        std::cout << _prefix << "deallocate " << bytes << " Bytes\n";
    }
    _upstream->deallocate(p, bytes, alignment);
}

bool
MemoryResourcesTracker::do_is_equal(const memory_resource&) const noexcept
{
    return false;
}

void
MemoryResourcesTracker::trace(bool enabled)
{
    _trace = enabled;
}

uint64_t
MemoryResourcesTracker::allocated() const
{
    return _allocated;
}

uint64_t
MemoryResourcesTracker::deallocated() const
{
    return _deallocated;
}

uint64_t
MemoryResourcesTracker::retain() const
{
    return _retain;
}

void
MemoryResourcesTracker::reset()
{
    _allocated = 0;
    _deallocated = 0;
    _retain = 0;
}

void
MemoryResourcesTracker::status()
{
    std::clog << "(al: " << _allocated << ", de: " << _deallocated << ", re: " << _retain << ")"
              << std::endl;
}
