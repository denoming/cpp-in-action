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
