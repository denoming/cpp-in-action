#include <gtest/gtest.h>

#include <iomanip>

template<typename T, typename... Types>
void print(T firstArg, Types... args)
{
    std::cout << std::setw(10) << firstArg << ": " << sizeof...(args) << " left\n";
    if constexpr (sizeof...(args) > 0) {
        print(args...);
    }
}

template<typename... Types>
void doublePrint(Types... args)
{
    print(args + args...);
}
template<typename C, typename... Idx>
void indexPrint1(const C& container, Idx... idx)
{
    print(container[idx]...);
}

template<std::size_t... Idx, typename C>
void indexPrint2(const C& container)
{
    print(container[Idx]...);
}

TEST(VariadicTemplate, Print)
{
    print(5.75, "Hi", 5u);
}

TEST(VariadicTemplate, DoublePrint)
{
    doublePrint(1, std::string{"Hi"});
}

TEST(VariadicTemplate, IndexPrint)
{
    using IntVector = std::vector<std::string>;
    IntVector values{"go", "Please", "don't"};
    indexPrint1(values, 1, 2, 0);
    indexPrint2<1, 2, 0>(values);
}

