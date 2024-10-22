#include <gtest/gtest.h>

#include <tuple>
#include <ostream>

template<typename TupleT, std::size_t... Is>
void
printTuple(std::ostream& os, const TupleT& t, std::index_sequence<Is...> seq)
{
    auto print = [&os](const auto& v, size_t id) {
        if (id > 0) {
            os << ", ";
        }
        os << id << ": " << v;
    };
    os << '(';
    (print(std::get<Is>(t), Is), ...);
    os << ')';
}

template<typename TupleT, std::size_t TupleS = std::tuple_size_v<TupleT>>
void
printTuple(std::ostream& os, const TupleT& t)
{
    printTuple(os, t, std::make_index_sequence<TupleS>());
}

TEST(Tuples, Dummy)
{
    std::tuple values{10, 20, 3.14, 42, "hello"};
    printTuple(std::cout, values);
}
