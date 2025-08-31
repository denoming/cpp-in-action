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

#include <gtest/gtest.h>

#include <chrono>
#include <format>
#include <iostream>

using namespace testing;

namespace krn = std::chrono;

//--------------------------------------------------------------------------------------------------

static void
log(std::ostream& s, int value)
{
    s << std::format("Value: {:5}\n", value);
}

TEST(Formatting, SimpleLog)
{
    int v1{42};
    log(std::cout, v1);
}

//--------------------------------------------------------------------------------------------------

static void
log(std::ostream& s, krn::milliseconds value)
{
    s << std::format("Timeout: {}\n", value);
}

TEST(Formatting, ChronoLog)
{
    krn::milliseconds v1{250};
    log(std::cout, v1);

    std::cout << std::format("Logged at {:%F %T} UTC.", std::chrono::system_clock::now());
}

//--------------------------------------------------------------------------------------------------

struct Data {
    int value;
    std::string name;
};

template<>
struct std::formatter<Data> {
    /**
     * Parses format-spec for type T in the range [ctx.begin(), ctx.end()) until the first
     * unmatched character. Throw `format_error` unless the whole range is parsed or the
     * unmatched character is `}`.
     * Stores the parsed format specifiers in `*this` and return an iterator past the end
     * of the parsed range.
     *
     * "{0}"      // ctx.begin() points to `}`
     * "{0:d}"    // ctx.begin() points to `d`, begin-end is "d}"
     * "{:hello}" // ctx.begin points to 'h' and begin-end is "hello}"
     *
     * The `parse()` function has to return the iterator to the closing bracket.
     */
    constexpr auto
    parse(std::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    /**
     * Formats `object` according to the specifiers in `*this`, writes the output
     * to `ctx.out()` and returns an iterator past the end of the output range.
     * The output shall only depend on `object`, `ctx.locale()`, and the range
     * `[pc.begin(), pc.end())` from the last call to `.parse(ctx)`.
     */
    auto
    format(const Data& object, std::format_context& ctx) const
    {
        return std::format_to(ctx.out(), "{} - {:5}", object.name, object.value);
    }
};

TEST(Formatting, CustomType1)
{
    Data d{42, "Custom"};
    std::cout << std::format("{}", d);
}

//--------------------------------------------------------------------------------------------------

struct Color {
    int r{};
    int g{};
    int b{};
};

template<>
struct std::formatter<Color> : std::formatter<std::string_view> {
    auto format(const Color& col, std::format_context& ctx) const {
        std::string temp;
        std::format_to(std::back_inserter(temp), "({}, {}, {})",
                       col.r, col.g, col.b);
        return std::formatter<string_view>::format(temp, ctx);
    }
};

TEST(Formatting, CustomType2)
{
    Color d{1, 2, 3};
    std::cout << std::format("{}", d);
}

//--------------------------------------------------------------------------------------------------

void
doLogging(std::string_view format, std::format_args args)
{
    std::cout << std::vformat(format, args);
}

template<typename... Args>
inline void
logging(std::string_view format, const Args&... args)
{
    doLogging(format, std::make_format_args(args...));
}

TEST(Formatting, Manually)
{
    int value{1111};
    logging("{}", value);
}
