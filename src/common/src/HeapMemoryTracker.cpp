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

#include "common/HeapMemoryTracker.hpp"

void
HeapMemoryTracker::reset()
{
    _allocNum = 0;
    _allocSize = 0;
}

void
HeapMemoryTracker::trace(bool enable)
{
    _doTrace = enable;
}

void
HeapMemoryTracker::status()
{
    printf("%lu allocations for %zu bytes\n", _allocNum, _allocSize);
}

void*
HeapMemoryTracker::allocate(std::size_t size, std::size_t align, const char* call)
{
    ++_allocNum;
    _allocSize += size;
    void* p;
    if (align == 0) {
        /** Use default way to allocate memory */
        p = std::malloc(size);
    } else {
#ifdef _MSC_VER
        /** Use specific method to allocate over-aligned memory (Windows API) */
        p = _aligned_malloc(size, align);
#else
        /** Use specific method to allocate over-aligned memory (C++17 API) */
        p = std::aligned_alloc(align, align * size /* an integral multiple of alignment */);
#endif
    }

    if (_doTrace) {
        // DON'T use std::cout (might cause recursion)
        printf("#%lu %s ", _allocNum, call);
        printf("(%zu bytes, ", size);
        if (align > 0) {
            printf("%zu-byte aligned) ", align);
        } else {
            printf("non-aligned) ");
        }
        printf("=> %p (total: %zu bytes)\n", static_cast<void*>(p), _allocSize);
    }

    return p;
}

void
HeapMemoryTracker::deallocate(void* p, const char* call)
{
    if (_doTrace) {
        printf("%s => %p\n", call, static_cast<void*>(p));
    }

#ifdef _MSC_VER
    /** Use specific method to deallocate over-aligned memory (Windows API) */
    _aligned_free(p);
#else
    // C++17 API
    std::free(p);
#endif
}

size_t
HeapMemoryTracker::allocNumber()
{
    return _allocNum;
}

size_t
HeapMemoryTracker::allocSize()
{
    return _allocSize;
}

#ifndef DEFAULT_ALLOCATION

/**
 * void* operator new(std::size_t)                    T* p = new T
 * void* operator new(std::size_t, std::nothrow)      T* p = new(std::nothrow) T
 * void* operator new(std::size_t, void* address)     T* p = new(address) T
 * void* operator new(std::size_t count, args...)     T* p = new(args...) T
 *
 * void* operator new[](std::size_t)                  T* p = new[n] T
 * void* operator new[](std::size_t, std::nothrow)    T* p = new(std::nothrow) T[n]
 * void* operator new[](std::size_t, void* address)   T* p = new(address) T[n]
 * void* operator new[](std::size_t count, args...)   T* p = new(args...) T[n]
 *
 * void operator delete(void*)                        delete p;
 * void operator delete(void*, std::nothrow)          <special cases>
 * void operator delete(void*, void*)                 <special cases>
 * void operator delete(void*, args...)               <special cases>
 * void operator delete[](void*)                      delete[] p;
 * void operator delete[](void*, std::nothrow)        <special cases>
 * void operator delete[](void*, void*)               <special cases>
 * void operator delete[](void*, args)                <special cases>
 */

[[nodiscard]] void*
operator new(std::size_t size)
{
    return HeapMemoryTracker::allocate(size, 0, "::new");
}

[[nodiscard]] void*
operator new(std::size_t size, const std::nothrow_t&) noexcept
{
    return HeapMemoryTracker::allocate(size, 0, "::new(nothrow)");
}

[[nodiscard]] void*
operator new[](std::size_t size, const std::nothrow_t&) noexcept
{
    return HeapMemoryTracker::allocate(size, 0, "::new[](nothrow)");
}

[[nodiscard]] void*
operator new[](std::size_t size)
{
    return HeapMemoryTracker::allocate(size, 0, "::new[]");
}

[[nodiscard]] void*
operator new(std::size_t size, std::align_val_t align)
{
    return HeapMemoryTracker::allocate(size, static_cast<size_t>(align), "::new aligned");
}

[[nodiscard]] void*
operator new[](std::size_t size, std::align_val_t align)
{
    return HeapMemoryTracker::allocate(size, static_cast<size_t>(align), "::new[] aligned");
}

void
operator delete(void* p) noexcept
{
    HeapMemoryTracker::deallocate(p, "::delete");
}

void
operator delete[](void* p) noexcept
{
    HeapMemoryTracker::deallocate(p, "::delete[]");
}

void
operator delete(void* p, std::size_t) noexcept
{
    HeapMemoryTracker::deallocate(p, "::delete sized");
}

void
operator delete[](void* p, std::size_t) noexcept
{
    HeapMemoryTracker::deallocate(p, "::delete[] sized");
}

void
operator delete(void* p, std::align_val_t) noexcept
{
    HeapMemoryTracker::deallocate(p, "::delete aligned");
}

void
operator delete[](void* p, std::align_val_t) noexcept
{
    HeapMemoryTracker::deallocate(p, "::delete[] aligned");
}

void
operator delete(void* p, std::size_t, std::align_val_t) noexcept
{
    HeapMemoryTracker::deallocate(p, "::delete sized aligned");
}

void
operator delete[](void* p, std::size_t, std::align_val_t) noexcept
{
    HeapMemoryTracker::deallocate(p, "::delete[] sized aligned");
}

#endif
