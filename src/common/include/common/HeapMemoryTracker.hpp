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

#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-redundant-declaration"
#pragma once

#include <new>
#include <cstdio>
#include <cstdlib>
#ifdef _MSC_VER
#include <malloc.h>
#endif

class HeapMemoryTracker {
public:
    static void
    reset();

    static void
    trace(bool enable);

    static void
    status();

    static void*
    allocate(std::size_t size, std::size_t align, const char* call);

    static void
    deallocate(void* p, const char* call);

    static size_t
    allocNumber();

    static size_t
    allocSize();

private:
    /** The number of malloc calls */
    static inline size_t _allocNum{0};
    /** The number of allocated bytes */
    static inline size_t _allocSize{0};
    /** Is tracing enabled */
    static inline bool _doTrace{false};
    /** don't track output inside new overloads  */
    static inline bool _inNew{false};
};

/** ========================================== */
/** Override global memory allocation function */

#ifndef DEFAULT_ALLOCATION

[[nodiscard]] extern void*
operator new(std::size_t size);

[[nodiscard]] extern void*
operator new[](std::size_t size);

[[nodiscard]] extern void*
operator new(std::size_t size, std::align_val_t align);

[[nodiscard]] extern void*
operator new[](std::size_t size, std::align_val_t align);

#endif

/** ============================================ */
/** Override global memory de-allocation function */

#ifndef DEFAULT_ALLOCATION

extern void
operator delete(void* p) noexcept;

extern void
operator delete[](void* p) noexcept;

extern void
operator delete(void* p, std::size_t) noexcept;

extern void
operator delete[](void* p, std::size_t) noexcept;

extern void
operator delete(void* p, std::align_val_t) noexcept;

extern void
operator delete[](void* p, std::align_val_t) noexcept;

extern void
operator delete(void* p, std::size_t, std::align_val_t align) noexcept;

extern void
operator delete[](void* p, std::size_t, std::align_val_t align) noexcept;

#endif

#pragma clang diagnostic pop