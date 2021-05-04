#pragma once

#include <iostream>

class MemoryDumper {
public:
    static void
    dump(void* ptr, std::size_t size, std::ostream& out = std::cout);
};
