#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <vector>
#include <concepts>
#include <numeric>

using namespace testing;

TEST(LambdaExtension, DifferentVersions)
{
    // Typed Lambda (C++ 11)
    auto sumInt = [](int fir, int sec) { return fir + sec; };
    // Generic Lambda (C++ 14)
    auto sumGen = [](auto fir, auto sec) { return fir + sec; };
    // Generic Lambda + Type deduction (C++ 11)
    auto sumDec = [](auto fir, decltype(fir) sec) { return fir + sec; };
    // Template Lambda (C++ 20)
    auto sumTem = []<typename T>(T fir, T sec) { return fir + sec; };

    EXPECT_THAT(sumInt(2000, 11), 2011);
    EXPECT_THAT(sumGen(2000, 11), 2011);
    EXPECT_THAT(sumDec(2000, 11), 2011);
    EXPECT_THAT(sumTem(2000, 11), 2011);

    std::string hello{"Hello "};
    std::string world{"world"};

    EXPECT_THAT(sumGen(hello, world), Eq("Hello world"));
    EXPECT_THAT(sumDec(hello, world), Eq("Hello world"));
    EXPECT_THAT(sumTem(hello, world), Eq("Hello world"));
}

TEST(LambdaExtension, UsingContainers)
{
    auto lambdaVector = []<std::integral T>(const std::vector<T>& values) {
        return std::accumulate(values.cbegin(), values.cend(), 0U);
    };

    const std::vector<std::uint32_t> values{1U, 5U, 7U};
    EXPECT_EQ(lambdaVector(values), 13U);
}

TEST(LambdaExtension, ThisCopyDetection)
{
    struct LambdaFactory {
        [[nodiscard]] auto
        foo() const
        {
            // Warning: implicit capture of `this` via ‘[=]’ is deprecated in C++20
            // return [=] { std::cout << str << &std::endl; };

            // No warning: capture `this` pointer explicitly
            return [this] { std::cout << str << &std::endl; };
        }

        ~LambdaFactory()
        {
            std::cout << "Goodbye" << &std::endl;
        }

        std::string str{"LambdaFactory"};
    };

    // LambdaFactory lambdaFactory;
    // lambdaFactory.foo();
}

TEST(LambdaExtension, UnEvaluatedContext)
{
    auto Compare = [](const std::string& lh, const std::string& rh) {
        return std::lexicographical_compare(
            lh.cbegin(), lh.cend(), rh.cbegin(), rh.cend(), [](const char lhc, const char rhc) {
                return (std::toupper(lhc) < std::toupper(rhc));
            });
    };

    using SetCaseInsensitive = std::set<std::string, decltype(Compare)>;

    SetCaseInsensitive authors{"scott", "Bjarne", "Herb", "Dave", "michael"};
    EXPECT_THAT(authors, ElementsAre("Bjarne", "Dave", "Herb", "michael", "scott"));
}