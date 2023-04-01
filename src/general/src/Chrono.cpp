#include <gtest/gtest.h>

#include <fmt/core.h>
#include <fmt/chrono.h>

#include <iostream>
#include <chrono>

/*

Clock        | Description                      | Epoch            |
-------------------------------------------------------------------
steady_clock | Monotonic clock for measurement  | Impl specific    |
system_clock | Clock of the operating system    | 1 January 1970   |
file_clock   | Alias of file_time_type          | Impl specific    |
gps_clock    | GPS time                         | 6 January 1980   |
local_t      | Pseudo clock for local time      | Without epoch    |
tai_clock    | TAI time                         | 1 January 1958   |
utc_clock    | Coordinated Universal Time (UTC) | 1 January 1970   |

*/

namespace krn = std::chrono;

using namespace std::literals;

TEST(Chrono, TimeOfDay)
{
    std::cout << std::boolalpha << '\n';

    auto timeOfDay = krn::hh_mm_ss(10.5h + 98min + 202s + 0.5s);

    EXPECT_EQ(timeOfDay.hours(), 12h);
    EXPECT_EQ(timeOfDay.minutes(), 11min);
    EXPECT_EQ(timeOfDay.seconds(), 22s);
}

TEST(Chrono, CalendarDate)
{
    // 1940.06.26
    constexpr auto d0{krn::year{1940} / 6 / 26};

    // 2010.03.31
    constexpr auto d1{2010y / krn::March / krn::last};

    // Create calendar data using year_month_weekday type (2010y / March / Thursday(first))
    constexpr krn::year_month_weekday d2{2010y, krn::month{3}, krn::Thursday[1]};

    // Create calendar data using year_month_weekday_last type (2010y / March / Monday(last))
    constexpr krn::year_month_weekday_last d3{2010y, krn::month{3}, krn::weekday_last{krn::Monday}};

    // Create calendar data using year_month_day_last type
    constexpr krn::year_month_day_last d4{2010y, krn::month_day_last{krn::month(3)}};
}

TEST(Chrono, Substraction)
{
    auto now = krn::system_clock::now();

    auto currentDate = krn::year_month_day{krn::floor<krn::days>(now)};
    auto currentYear = currentDate.year();

    auto hoursAfter = krn::floor<krn::hours>(now) - krn::sys_days{currentYear / krn::January / 1};
    fmt::print("It has been {} hours since New Year\n", hoursAfter.count());

    auto nextYear = currentYear + krn::years{1};
    auto hoursBefore = krn::sys_days{nextYear / krn::January / 1} - krn::floor<krn::hours>(now);
    fmt::print("It is {} hours before New Year\n", hoursBefore.count());
}
