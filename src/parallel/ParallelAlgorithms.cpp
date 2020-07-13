#include "common/Timer.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <filesystem>
#include <execution>
#include <vector>
#include <cmath>

namespace fs = std::filesystem;

using ::testing::Each;
using ::testing::Field;
using ::testing::Ne;

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

        auto size = std::transform_reduce(files.begin(), files.end(), std::uintmax_t{0}, std::plus{}, [](const fs::path& p) {
            return fs::file_size(p);
        });

        EXPECT_THAT(size, Ne(0));
    });
}
