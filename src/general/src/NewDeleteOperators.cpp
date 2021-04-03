#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "common/HeapMemoryTracker.hpp"

class Data {
public:
    Data();

    ~Data();

private:
    int* value;
};

Data::Data()
    : value{nullptr}
{
    std::cout << "ctor" << std::endl;
}

Data::~Data()
{
    std::cout << "dtor" << std::endl;
}

TEST(NewDeleteOperators, UseOperatorsWithoutThrow)
{
    HeapMemoryTracker::trace(true);
    const Data* data = new (std::nothrow) Data;
    if (data == nullptr) {
        // Mandatory checking for nullptr
        FAIL();
    }
    delete data;
    HeapMemoryTracker::trace(false);
}
