// Copyright 2025 Denys Asauliak
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "common/MemoryDump.hpp"

#include <string>

#include <boost/format.hpp>

void
MemoryDump::dump(void* ptr, std::size_t size, std::ostream& out)
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