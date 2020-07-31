#include <gtest/gtest.h>

#include <functional>

TEST(StringSearchTest, BoyerMooreSearcherForString)
{
    const std::string source = R"(Lorem ipsum dolor sit amet, consectetur adipiscing elit,
 sed do eiusmod tempor incididunt ut labore et dolore magna aliqua
)";
    const std::string target{"Et"};

    std::boyer_moore_searcher bm{
        target.cbegin(), target.cend(),
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

TEST(StringSearchTest, BoyerMooreSearcherForData)
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
