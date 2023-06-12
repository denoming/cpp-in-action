#include "common/Timer.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <iostream>
#include <filesystem>
#include <execution>
#include <vector>
#include <cmath>

using namespace testing;

namespace fs = std::filesystem;

struct Data {
    Data(double value, double sqrt)
        : value{value}
        , sqrt{sqrt}
    {
    }

    double value;
    double sqrt;
};

TEST(ParallelAlgorithms, ForEach)
{
    constexpr int NumOfElements = 1'000'000;

    std::vector<Data> elems;
    elems.reserve(NumOfElements);
    for (int i = 1; i <= NumOfElements; ++i) {
        elems.emplace_back(i * M_PI, 0.0);
    }

    for (int n = 0; n < 3; ++n) {
        Timer t;
        std::for_each(std::execution::seq, elems.begin(), elems.end(), [](Data& d) {
            d.sqrt = std::sqrt(d.value);
        });
        std::cout << "Diff (seq): " << t.diff(true) << std::endl;
        std::for_each(std::execution::par, elems.begin(), elems.end(), [](Data& d) {
            d.sqrt = std::sqrt(d.value);
        });
        std::cout << "Diff (par): " << t.diff() << std::endl;
    }

    EXPECT_THAT(elems, Each(Field(&Data::sqrt, Ne(0.0))));
}

TEST(ParallelAlgorithms, DISABLED_Path)
{
    static const fs::path root{"<path>"};
    EXPECT_NO_THROW({
        std::vector<fs::path> files;
        fs::recursive_directory_iterator dirpos{root};
        std::for_each(fs::begin(dirpos), fs::end(dirpos), [&](const fs::path& p) {
            if (fs::is_regular_file(p)) {
                files.push_back(p);
            }
        });

        auto size = std::transform_reduce(
            files.begin(), files.end(), std::uintmax_t{0}, std::plus{}, [](const fs::path& p) {
                return fs::file_size(p);
            });

        EXPECT_THAT(size, Ne(0));
    });
}

TEST(ParallelAlgorithms, Accumulate)
{
    constexpr int NumOfElements = 1'000'000;
    std::vector<double> nums(NumOfElements, 0);
    std::iota(nums.begin(), nums.end(), 1L);

    Timer t;
    std::cout << "Value (SEQ): "
              << std::accumulate(nums.begin(), nums.end(), 1.0, [](double a, double b) {
                     return std::sqrt(a) + std::sqrt(b);
                 });
    std::cout << " (time: " << t.diff() << ")" << std::endl;
}

TEST(ParallelAlgorithms, Reduce)
{
    constexpr int NumOfElements = 1'000'000;
    std::vector<double> nums(NumOfElements, 0);
    std::iota(nums.begin(), nums.end(), 1L);

    /* The behaviour is non-deterministic as Fn is not associative and not commutative */
    const auto Fn = [](double a, double b) { return std::sqrt(a) + std::sqrt(b); };

    Timer t1;
    std::reduce(std::execution::seq, nums.begin(), nums.end(), 1.0, Fn);
    std::cout << "Reduce (SEQ): " << t1.diff() << "\n";

    Timer t2;
    std::reduce(std::execution::par, nums.begin(), nums.end(), 1.0, Fn);
    std::cout << "Reduce (PAR): " << t2.diff() << "\n";
}

TEST(ParallelAlgorithms, ReduceString)
{
    std::vector<std::string> in{"1", "2", "3", "4", "5"};
    const auto result
        = std::reduce(in.begin(), in.end(), std::string{"INITIAL"}, [](auto&& s1, auto&& s2) {
              std::cout << "> " << s1 << " | " << s2 << std::endl;
              return s1 + ":" + s2;
          });
    std::cout << "Result: " << result << std::endl;
}

TEST(ParallelAlgorithms, ExclusiveScan)
{
    std::vector<int> in{1, 2, 3, 4, 5, 6, 7, 8, 9};

    std::vector<int> out;
    std::exclusive_scan(
        std::execution::par, in.begin(), in.end(), std::back_inserter(out), 0, [](int n1, int n2) {
            return n1 + n2;
        });

    std::cout << "std::exclusive_scan: ";
    for (auto v : out) {
        std::cout << v << " ";
    }
    std::cout << '\n';
}

TEST(ParallelAlgorithms, InclusiveScan)
{
    std::vector<int> in{1, 2, 3, 4, 5, 6, 7, 8, 9};

    std::vector<int> out;
    std::inclusive_scan(
        std::execution::par, in.begin(), in.end(), std::back_inserter(out), [](int n1, int n2) {
            return n1 + n2;
        });

    std::cout << "std::exclusive_scan: ";
    for (auto v : out) {
        std::cout << v << " ";
    }
    std::cout << '\n';
}