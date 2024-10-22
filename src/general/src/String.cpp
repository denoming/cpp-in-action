#include <gtest/gtest.h>

#include <functional>
#include <sstream>
#include <spanstream>
#include <string>

TEST(String, Contains)
{
    const std::string url = "https://isocpp.org";

    EXPECT_TRUE(url.contains("https"));
    EXPECT_TRUE(url.contains(".org"));
    EXPECT_FALSE(url.contains(".com"));
}

TEST(String, StartEndWith)
{
    const std::string url = "https://isocpp.org";

    EXPECT_TRUE(url.starts_with("https") and url.ends_with(".org"));
}

TEST(String, Stream)
{
    // Use rvalue ctor
    std::stringstream ss{std::string{" SOME ALREADY ALLOCATED BIG STRING "}};
    ss << "I'm Denys";

    // Use .view() instead of .str() to avoid copying internal string
    EXPECT_TRUE(ss.view().starts_with("I'm Denys"));
}

TEST(String, SpanStream)
{
    // Allocate buffer and span to use for placing a string
    char buffer[128] { 0 };
    std::span<char> sb(buffer);

    // Declare span stream to use previously allocated buffer
    std::spanstream ss{sb};
    ss << "one string that doesn't fit into SSO";

    // Check the value
    std::string_view view{buffer};
    EXPECT_EQ(view, "one string that doesn't fit into SSO");
}

TEST(String, BoyerMooreSearcherForString)
{
    const std::string source = R"(Lorem ipsum dolor sit amet, consectetur adipiscing elit,
 sed do eiusmod tempor incididunt ut labore et dolore magna aliqua
)";
    const std::string target{"Et"};

    std::boyer_moore_searcher bm{
        target.cbegin(),
        target.cend(),
        /* Custom hash method */
        [](char ch) { return std::hash<char>{}(std::toupper(ch)); },
        /* Custom compare method */
        [](char c1, char c2) { return (std::toupper(c1) == std::toupper(c2)); }};

    for (auto [beg, end] = bm(source.cbegin(), source.cend()); beg != source.cend();
         std::tie(beg, end) = bm(end, source.cend())) {
        std::cout << "Found at: [" << (beg - source.cbegin()) << ":" << (end - source.cbegin())
                  << ")" << std::endl;
    }
}

TEST(String, BoyerMooreSearcherForData)
{
    std::vector<int> source = {1, 3, 4, 0, 6, 1, 7, 5, 0, 6, 2};
    std::vector<int> target = {0, 6};

    std::boyer_moore_searcher bm{target.cbegin(), target.cend()};
    for (auto [beg, end] = bm(source.cbegin(), source.cend()); beg != source.cend();
         std::tie(beg, end) = bm(end, source.cend())) {
        std::cout << "Found at: [" << (beg - source.cbegin()) << ":" << (end - source.cbegin())
                  << ")" << std::endl;
    }
}
