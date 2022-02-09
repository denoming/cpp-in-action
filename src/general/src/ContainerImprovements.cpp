#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <map>

using namespace testing;

class Node {
private:
    std::string value;
    std::vector<Node> children; // OK since C++17 (Node is an incomplete type here)
public:
    Node(std::string s)
        : value{std::move(s)}
        , children{}
    {
    }

    void
    add(Node n)
    {
        children.push_back(std::move(n));
    }

    Node&
    operator[](std::size_t idx)
    {
        return children.at(idx);
    }

    void
    print(int indent = 0) const
    {
        std::cout << std::string(indent, ' ') << value << '\n';
        for (const auto& n : children) {
            n.print(indent + 2);
        }
    }
};

TEST(ContainerImprovementsTest, ModifyingKey)
{
    std::map<int, std::string> elems{{1, "mango"}, {2, "papaya"}, {3, "guava"}};

    auto node = elems.extract(2);
    node.key() = 4;
    elems.insert(std::move(node));

    EXPECT_THAT(elems, UnorderedElementsAre(Pair(1, "mango"), Pair(4, "papaya"), Pair(3, "guava")));
}

TEST(ContainerImprovementsTest, MovingNode)
{
    std::multimap<double, std::string> src{{1.1, "one"}, {2.2, "two"}, {3.3, "three"}};
    std::map<double, std::string> dst{{3.3, "old data"}};

    dst.insert(src.extract(src.find(1.1))); // splice using an iterator
    dst.insert(src.extract(2.2));           // splice using the key

    EXPECT_THAT(dst, Contains(Pair(1.1, "one")));
    EXPECT_THAT(dst, Contains(Pair(2.2, "two")));

    auto [pos, done, node] = dst.insert(src.extract(3.3));
    EXPECT_FALSE(done);
    EXPECT_THAT(src, IsEmpty());

    /* So node was extraxted, but not inserted, due too elements already exists */
    const auto& [key, value] = *pos;
    EXPECT_EQ(key, 3.3);
    EXPECT_EQ(value, "old data");
}

TEST(ContainerImprovementsTest, Merging)
{
    std::multimap<double, std::string> src{{1.1, "one"}, {2.2, "two"}, {3.3, "three"}};
    std::map<double, std::string> dst{{3.3, "old data"}};

    dst.merge(src);

    EXPECT_THAT(dst,
                UnorderedElementsAre(Pair(1.1, "one"), Pair(2.2, "two"), Pair(3.3, "old data")));
    EXPECT_THAT(src, Contains(Pair(3.3, "three")));
}

TEST(ContainerImprovementsTest, IncompleteTypes)
{
    Node root{"top"};
    root.add(Node{"elem1"});
    root.add(Node{"elem2"});
    root[0].add(Node{"elem1.1"});
    root.print();
}
