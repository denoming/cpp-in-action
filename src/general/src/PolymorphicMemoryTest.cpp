#include "common/HeapMemoryTracker.hpp"
#include "common/MemoryResourcesTracker.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <vector>
#include <string>
#include <memory_resource>

using ::testing::Lt;

/** Size of memory chunk */
static const std::size_t BufferSize = 200000;

/** Memory chunk of monotonic buffer  */
static std::array<std::byte, BufferSize> Buffer;

/** Example of custom type with polymorphic allocator supporting */
class PolymorphicType {
public:
    using allocator_type = std::pmr::polymorphic_allocator<std::byte>;

    PolymorphicType(std::pmr::string data, allocator_type alloc = {})
        : _data{std::move(data), alloc}
    {
    }

    PolymorphicType(const PolymorphicType& other, allocator_type alloc)
        : _data{other._data, alloc}
    {
    }

    PolymorphicType(PolymorphicType&& other, allocator_type alloc)
        : _data{std::move(other._data), alloc}
    {
    }

    void
    setName(std::pmr::string data)
    {
        _data = std::move(data);
    }

    std::pmr::string
    getName() const
    {
        return _data;
    }

    std::string
    getNameAsString() const
    {
        return std::string{_data};
    }

private:
    std::pmr::string _data; // Also used to hold allocator
};

/**
 * Memory of std::string allocated in heap.
 * Memory of std::vector allocated in heap.
 */
std::tuple<std::size_t, std::size_t>
commonWay(std::size_t allocNum = 1000)
{
    HeapMemoryTracker::reset();
    std::vector<std::string> data;
    for (std::size_t i = 0; i < allocNum; ++i) {
        data.emplace_back("just a non-SSO string");
    }
    HeapMemoryTracker::status();
    return std::make_tuple(HeapMemoryTracker::allocSize(), HeapMemoryTracker::allocSize());
}

/**
 * Memory of std::string allocated in heap
 * Memory of std::vector allocated in monotonic buffer.
 */
std::tuple<std::size_t, std::size_t>
hybridWay(std::size_t allocNum = 1000)
{
    HeapMemoryTracker::reset();
    std::pmr::monotonic_buffer_resource pool{Buffer.data(), Buffer.size()};
    std::pmr::vector<std::string> data{&pool};
    for (std::size_t i = 0; i < allocNum; ++i) {
        data.emplace_back("just a non-SSO string");
    }
    HeapMemoryTracker::status();
    return std::make_tuple(HeapMemoryTracker::allocSize(), HeapMemoryTracker::allocSize());
}

/**
 * Memory of std::string allocated in monotonic buffer.
 * Memory of std::vector allocated in monotonic buffer.
 */
std::tuple<std::size_t, std::size_t>
entireWay(std::size_t allocNum = 1000)
{
    HeapMemoryTracker::reset();
    std::pmr::monotonic_buffer_resource pool{Buffer.data(), Buffer.size()};
    std::pmr::vector<std::pmr::string> data{&pool};
    for (std::size_t i = 0; i < allocNum; ++i) {
        data.emplace_back("just a non-SSO string");
    }
    HeapMemoryTracker::status();
    return std::make_tuple(HeapMemoryTracker::allocSize(), HeapMemoryTracker::allocSize());
}

TEST(PolymorphicMemoryTest, MemoryPool)
{
    auto [commonAllocNum, commonAllocSize] = commonWay();
    auto [hybridAllocNum, hybridAllocSize] = hybridWay();
    auto [entireAllocNum, entireAllocSize] = entireWay();

    EXPECT_THAT(hybridAllocNum, Lt(commonAllocNum));
    EXPECT_THAT(entireAllocNum, Lt(hybridAllocNum));

    EXPECT_THAT(hybridAllocSize, Lt(commonAllocSize));
    EXPECT_THAT(entireAllocSize, Lt(hybridAllocSize));
}

TEST(PolymorphicMemoryTest, NestedMemoryPool)
{
    MemoryResourcesTracker exTracker{"keeppool:"};
    exTracker.trace(true);

    /** Used as external quick memory pool (without deallocation before destruction) */
    std::pmr::monotonic_buffer_resource keeppool{10000, &exTracker};
    {
        MemoryResourcesTracker inTracker{"   pool:", &keeppool};
        inTracker.trace(true);

        /** Used as internal memory pool with less fragmentation and fake deallocation (memory not
         *  deallocated due to upstream monotonic memory pool) */
        std::pmr::synchronized_pool_resource pool{&inTracker};
        for (int j = 0; j < 100; ++j) {
            std::pmr::vector<std::pmr::string> coll{&pool};
            coll.reserve(100);
            for (int i = 0; i < 100; ++i) {
                coll.emplace_back("just a non-SSO string");
            }
            std::clog << "--- iteration" << std::endl;
        }

        std::clog << "--- leave scope of pool" << std::endl;
    }
    std::clog << "--- leave scope of keeppool" << std::endl;
}

TEST(PolymorphicMemoryTest, CustomType)
{
    MemoryResourcesTracker tracker;
    tracker.trace(true);

    {
        std::pmr::vector<PolymorphicType> coll;
        coll.reserve(100);

        /** Allocates with get_default_resource() */
        PolymorphicType c1{"just a non-SSO string"};
        /** Allocates copy by vector allocator (tracker) */
        coll.push_back(c1);
        /** Copies (allocators not interchangeable) */
        coll.push_back(std::move(c1));

        /** Allocates with tracker */
        PolymorphicType c2{"just a non-SSO string", &tracker};
        /** Allocates copy by vector allocator (tracker) */
        coll.push_back(c2);
        /** Moves (same allocator) */
        coll.push_back(std::move(c2));
    }

    tracker.status();
}
