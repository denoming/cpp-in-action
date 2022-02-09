#include <gtest/gtest.h>

#include "common/HeapMemoryTracker.hpp"
#include "common/MemoryDumper.hpp"

#include <boost/format.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/comparison.hpp>
#include <boost/mpl/bitwise.hpp>
#include <boost/mpl/arithmetic.hpp>

#include <iostream>

#include <cstring>

template<std::size_t bytes>
struct boundary {
    // If an alignment is specified, it must be greater than or equal to
    // the size of a pointer.
    BOOST_MPL_ASSERT(
        (boost::mpl::greater_equal<boost::mpl::int_<bytes>, boost::mpl::int_<sizeof(void*)>>) );

    // The alignment bytes must be a power of two
    // (n & (n-1)) == 0
    BOOST_MPL_ASSERT(
        (boost::mpl::equal_to<
            boost::mpl::bitand_<boost::mpl::int_<bytes>,
                                boost::mpl::minus<boost::mpl::int_<bytes>, boost::mpl::int_<1>>>,
            boost::mpl::int_<0>>) );

    // In order for this to work, must allocate additional bytes
    enum { value = bytes };

    // Get the next aligned pointer
    static void*
    next(void* ptr)
    {
        // Round down to the previous multiple of X,
        // then move to the next multiple of X.
        return reinterpret_cast<void*>(
            (reinterpret_cast<std::size_t>(ptr) & ~(std::size_t(value - 1))) + value);
    }

    // Return an aligned pointer
    static void*
    align(void* ptr)
    {
        // Get the next aligned pointer
        void* aligned_ptr = next(ptr);

        // Save the original pointer in the space we skipped over
        *(reinterpret_cast<void**>(aligned_ptr) - 1) = ptr;

        // Return the aligned pointer
        return aligned_ptr;
    }

    // Retrieve the original pointer (without alignment)
    static void*
    unalign(void* ptr)
    {
        return *(reinterpret_cast<void**>(ptr) - 1);
    }
};

// Specialize to not attempt alignment
template<>
struct boundary<0> {
    enum { value = 0 };

    static void*
    next(void* ptr)
    {
        return ptr;
    }

    static void*
    align(void* ptr)
    {
        return ptr;
    }

    static void*
    unalign(void* ptr)
    {
        return ptr;
    }
};

struct Data1 {
    char a{0x11};
    int b{0x22222222};
    short c{0x3333};
    long d{0x4444444444444444};
};

struct Data2 {
    long d{0x4444444444444444};
    int b{0x22222222};
    short c{0x3333};
    char a{0x11};
};

TEST(NewDeleteOperators, UseOperatorsWithoutThrow)
{
    HeapMemoryTracker::trace(true);
    const Data1* data = new (std::nothrow) Data1;
    if (data == nullptr) {
        // Mandatory checking for nullptr
        FAIL();
    }
    delete data;
    HeapMemoryTracker::trace(false);
}

TEST(NewDeleteOperators, BadCaseOfAlignment)
{
    std::uint8_t memory[sizeof(Data1)];
    std::memset(memory, 0xFF, sizeof(Data1)); // Fill by 0xFF bytes

    std::cout << "Bad case of alignment:\n";
    Data1* pData = new (memory) Data1{};
    MemoryDumper::dump(pData, sizeof(Data1));
    pData->~Data1();
}

TEST(NewDeleteOperators, GoodCaseOfAlignment)
{
    std::uint8_t memory[sizeof(Data2)];
    std::memset(memory, 0xFF, sizeof(Data2)); // Fill by 0xFF bytes

    std::cout << "Good case of alignment:\n";
    Data2* pData = new (memory) Data2;
    MemoryDumper::dump(pData, sizeof(Data2));
    pData->~Data2();
}

TEST(NewDeleteOperators, CustomAlignment)
{
    static const std::string_view format1{"Memory:          0x%016lX\n"};
    static const std::string_view format2{"Memory + Offset: 0x%016lX\n"};
    static const std::string_view format3{"Ptr:             0x%016lX\n"};
    static const std::string_view format4{"Unaligned Ptr:   0x%016lX\n"};

    std::size_t size = sizeof(Data1);
    std::size_t offset = 8;
    std::size_t realSize = size + boundary<16>::value + offset;

    // Reserve size for structure
    // + alignment (16 bytes)
    // + offset (8 bytes)
    unsigned char* memory = static_cast<unsigned char*>(std::malloc(realSize));

    // Fill offset
    std::memset(memory, 0xFF, offset);

    // Fill memory that will occupied by structure
    std::memset(memory + offset, 0xAA, size + boundary<16>::value);

    // Construct object into memory with placement new
    Data1* ptr = new (boundary<16>::align(memory + offset)) Data1{};

    // Dump all memory
    std::cout << boost::format(format1.data()) % reinterpret_cast<void*>(memory);
    std::cout << boost::format(format2.data()) % reinterpret_cast<void*>(memory + offset);
    std::cout << boost::format(format3.data()) % ptr;
    std::cout << boost::format(format4.data()) % boundary<16>::unalign(ptr);
    MemoryDumper::dump(memory, realSize);

    // Destruct object
    ptr->~Data1();

    // Free allocated memory
    std::free(memory);
}
