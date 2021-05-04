#include "common/MemoryDumper.hpp"

#include <string>

#include <boost/format.hpp>

void
MemoryDumper::dump(void* ptr, std::size_t size, std::ostream& out)
{
    auto iptr = reinterpret_cast<std::size_t>(ptr);

    out << "-----------------------------------------------------------------------\n";

    const std::string label = std::to_string(size) + " bytes";
    out << boost::format("%-19s") % label;
    for (std::size_t i{0}; i < 16; ++i) {
        if (i % 4 == 0) {
            out << ' ';
        }
        out << boost::format(" %2hhX") % i;
    }

    if (iptr % 16 != 0) {
        out << boost::format("\n0x%016lX:") % (iptr & ~15);
        for (std::size_t i{0}; i < iptr % 16; ++i) {
            out << "   ";
            if (i % 4 == 0) {
                out << ' ';
            }
        }
    }

    for (std::size_t i{0}; i < size; ++i, ++iptr) {
        if (iptr % 16 == 0) {
            out << boost::format("\n0x%016lX:") % iptr;
        }
        if (iptr % 4 == 0) {
            out << ' ';
        }
        const auto byte = static_cast<std::size_t>(*reinterpret_cast<std::uint8_t*>(iptr));
        out << boost::format(" %02hhX") % byte;
    }

    out << "\n----------------------------------------------------------------------";
    out << std::endl;
}