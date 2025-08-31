// Copyright 2025 Denys Asauliak
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
