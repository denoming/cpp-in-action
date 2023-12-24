#include <gtest/gtest.h>

#include "common/HeapMemoryTracker.hpp"
#include "common/MemoryDump.hpp"

#include <boost/format.hpp>
#include <boost/mpl/arithmetic.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/bitwise.hpp>
#include <boost/mpl/comparison.hpp>
#include <boost/mpl/int.hpp>

#include <iostream>

#include <cstring>

extern "C" {
#include <malloc.h>
}

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
    Data1()
    {
        std::cout << "ctor: Data1" << std::endl;
    }

    ~Data1()
    {
        std::cout << "dtor: Data1" << std::endl;
    }

    char a{0x11};
    int b{0x22222222};
    short c{0x3333};
    long d{0x4444444444444444};
};

struct Data2 {
    Data2()
    {
        std::cout << "ctor: Data2" << std::endl;
    }

    ~Data2()
    {
        std::cout << "dtor: Data2" << std::endl;
    }

    long d{0x0102030405060708};
    int b{0x11223344};
    short c{0x5566};
    char a{0x77};
};

//----------------------------------------------------------------------------------------------------------------------

TEST(NewDeleteOperators, LegacyAllocation)
{
    /* Allocate raw memory with given size */
    int* ptr1 = static_cast<int*>(malloc(sizeof(int)));
    *ptr1 = 7;
    free(ptr1);

    /* Allocate raw memory with given size and initialize to zero */
    int* ptr2 = static_cast<int*>(calloc(1, sizeof(int)));
    *ptr2 = 0x1EFEFEFE;

    std::cout << "Initial mem size: " << malloc_usable_size(ptr2) << std::endl;
    int* ptr3 = static_cast<int*>(realloc(ptr2, sizeof(int) * 10));
    if (ptr3 == nullptr) {
        std::cout << "Unable to realloc" << std::endl;
        free(ptr2);
    } else {
        std::cout << "After realloc mem size: " << malloc_usable_size(ptr3) << std::endl;
        free(ptr3);
    }
}

//----------------------------------------------------------------------------------------------------------------------

TEST(NewDeleteOperators, ModernAllocation)
{
    int* ptr1 = new int[5]{1, 2, 3, 4 /* the last element will have 0 value */};
    std::cout << *ptr1 << std::endl;
    delete[] ptr1;
}

//----------------------------------------------------------------------------------------------------------------------

static void
onBadAllocationHandler()
{
    static uint32_t index{};
    std::cout << "Unable to allocate memory: " << ++index << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds{15});
}

// Add -DDEFAULT_ALLOCATION=ON to cmake options
TEST(NewDeleteOperators, AllocationFailure)
{
    std::set_new_handler(onBadAllocationHandler);
    for (int n = 0; n < 50000; ++n) {
        std::ignore = new int[INT32_MAX];
    }
}

//----------------------------------------------------------------------------------------------------------------------

TEST(NewDeleteOperators, PlacementNew)
{
    /* On stack */
    uint8_t mem1[100];
    std::memset(mem1, 0xFF, 100); // Fill by 0xFF bytes
    auto* pData1 = new (mem1) uint32_t[5]{0, 1, 2, 3, 4}; // NOLINT
    MemoryDump::dump(mem1, 100);

    /* On heap */
    auto* mem2 = static_cast<uint8_t*>(operator new(100));
    auto* pData2 = new (mem2) Data2[2]{}; // NOLINT
    MemoryDump::dump(mem2, 100);
    pData2[0].~Data2();
    pData2[1].~Data2();
    operator delete(mem2);
}

//----------------------------------------------------------------------------------------------------------------------

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

//----------------------------------------------------------------------------------------------------------------------

TEST(NewDeleteOperators, BadCaseOfAlignment)
{
    /* Use placement new */
    std::uint8_t memory[sizeof(Data1)];
    std::memset(memory, 0xFF, sizeof(Data1)); // Fill by 0xFF bytes

    std::cout << "Bad case of alignment:\n";
    Data1* pData = new (memory) Data1{};
    MemoryDump::dump(pData, sizeof(Data1));
    pData->~Data1();
}

//----------------------------------------------------------------------------------------------------------------------

TEST(NewDeleteOperators, GoodCaseOfAlignment)
{
    /* Use placement new */
    std::uint8_t memory[sizeof(Data2)];
    std::memset(memory, 0xFF, sizeof(Data2)); // Fill by 0xFF bytes

    std::cout << "Good case of alignment:\n";
    Data2* pData = new (memory) Data2;
    MemoryDump::dump(pData, sizeof(Data2));
    pData->~Data2();
}

//----------------------------------------------------------------------------------------------------------------------

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
    MemoryDump::dump(memory, realSize);

    // Destruct object
    ptr->~Data1();

    // Free allocated memory
    std::free(memory);
}

//----------------------------------------------------------------------------------------------------------------------