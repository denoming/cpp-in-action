#pragma once

#include <cstdint>
#include <iostream>

class MemoryDump {
public:
    static void
    dump(void* ptr, std::size_t size, std::ostream& out = std::cout);
};
