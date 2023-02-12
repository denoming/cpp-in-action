#include <gtest/gtest.h>

#include <boost/asio.hpp>

namespace asio = boost::asio;

#include <array>
#include <vector>
#include <iostream>

/*** Complexity ***
 *
 * DynamicBuffer (std::streambuf)
 *  + prepare - O(n)
 *  + commit - O(1)
 *  + consume - O(1)
 *
 * DynamicBuffer (based on std::vector or std::string):
 *  + prepare - O(1)
 *  + commit - O(1)
 *  + consume - O(n)
 *
 * Dynamic buffer must be used only in one asynchronous operation at one point in time.
 */

template<typename... Args>
auto
makeSequence(Args&&... args)
{
    return std::array<asio::mutable_buffer, sizeof...(args)>{asio::buffer(args)...};
}

template<typename... Args>
auto
makeConstSequence(Args&&... args)
{
    return std::array<asio::const_buffer, sizeof...(Args)>{asio::buffer(args)...};
}

TEST(Buffer, BufferSequence)
{
    char d1[] = {'H', 'e', 'l', 'l', 'o', ' '};
    std::vector<char> d2{'w', 'o', 'r', 'l', 'd', '!'};

    auto views = makeConstSequence(d1, d2);
    EXPECT_EQ(asio::buffer_size(views), 12);

    /* Iterate over views */
    std::string str1;
    for (auto view : views) {
        str1.insert(str1.size(), static_cast<const char*>(view.data()), view.size());
    }
    EXPECT_EQ(str1, "Hello world!");

    /* Iterate over each view's data */
    std::string str2;
    auto beg{asio::buffers_begin(views)};
    auto end{asio::buffers_end(views)};
    for (auto it = beg; it != end; ++it) {
        str2.push_back(*it);
    }
    EXPECT_EQ(str1, "Hello world!");
}

TEST(Buffer, BufferCopy)
{
    char d1[] = {'H', 'e', 'l', 'l', 'o', ' '};
    std::vector<char> d2{'w', 'o', 'r', 'l', 'd', '!'};
    auto seq1 = makeConstSequence(d1, d2);

    std::vector<char> d3;
    auto seq2 = makeSequence(d3);

    /* Copy data from const views to mutable views */
    asio::buffer_copy(seq2, seq1);

    /* Gather data to string */
    std::string str1;
    auto beg{asio::buffers_begin(seq2)};
    auto end{asio::buffers_end(seq2)};
    for (auto it = beg; it != end; ++it) {
        str1.push_back(*it);
    }
    EXPECT_EQ(str1, "Hello world!");
}

TEST(Buffer, DynamicBuffer)
{
    std::vector<char> m1;
    std::string m2;

    auto vectorBuffer = asio::dynamic_buffer(m1);
    auto stringBuffer = asio::dynamic_buffer(m2);
    EXPECT_EQ(vectorBuffer.size(), 0);
    EXPECT_EQ(stringBuffer.size(), 0);
}

TEST(Buffer, DynamicStreamBuffer)
{
    const std::string_view Data{"Hello world!"};

    asio::streambuf buffer{1024};

    auto view = buffer.prepare(128);
    std::memcpy(view.data(), Data.data(), Data.size());
    buffer.commit(Data.size());
    EXPECT_EQ(buffer.size(), 12 /* The length of Data in chars */);

    std::istream s{&buffer};
    std::string line;
    std::getline(s, line);
    EXPECT_EQ(line, Data);

    buffer.consume(Data.size());
    EXPECT_EQ(buffer.size(), 0 /* All data has been consumed */);
}
