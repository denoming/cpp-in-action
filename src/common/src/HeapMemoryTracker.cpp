#include "common/HeapMemoryTracker.hpp"

void
HeapMemoryTracker::reset()
{
    _allocNum = 0;
    _allocSize = 0;
}

void
HeapMemoryTracker::trace(bool b)
{
    _doTrace = b;
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
    }
    else {
#ifdef _MSC_VER
        /** Use specific method to allocate over-aligned memory (Windows API) */
        p = _aligned_malloc(size, align);
#else
        /** Use specific method to allocate over-aligned memory (C++17 API) */
        p = std::aligned_alloc(align, size);
#endif
    }

    if (_doTrace) {
        // DON'T use std::cout (might cause recursion)
        printf("#%lu %s ", _allocNum, call);
        printf("(%zu bytes, ", size);
        if (align > 0) {
            printf("%zu-byte aligned) ", align);
        }
        else {
            printf("non-aligned) ");
        }
        printf("=> %p (total: %zu bytes)\n", static_cast<void*>(p), _allocSize);
    }

    return p;
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

[[nodiscard]] void*
operator new(std::size_t size)
{
    return HeapMemoryTracker::allocate(size, 0, "::new");
}

[[nodiscard]] void*
operator new(std::size_t size, std::align_val_t align)
{
    return HeapMemoryTracker::allocate(size, static_cast<size_t>(align), "::new aligned");
}

[[nodiscard]] void*
operator new[](std::size_t size)
{
    return HeapMemoryTracker::allocate(size, 0, "::new[]");
}

[[nodiscard]] void*
operator new[](std::size_t size, std::align_val_t align)
{
    return HeapMemoryTracker::allocate(size, static_cast<size_t>(align), "::new[] aligned");
}

void
operator delete(void* p) noexcept
{
    std::free(p);
}

void
operator delete(void* p, std::size_t) noexcept
{
    ::operator delete(p);
}

void
operator delete(void* p, std::align_val_t) noexcept
{
#ifdef _MSC_VER
    /** Use specific method to deallocate over-aligned memory (Windows API) */
    _aligned_free(p);
#else
    // C++17 API
    std::free(p);
#endif
}

void
operator delete(void* p, std::size_t, std::align_val_t align) noexcept
{
    ::operator delete(p, align);
}
