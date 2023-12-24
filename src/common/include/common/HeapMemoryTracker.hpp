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