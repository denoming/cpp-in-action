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

#pragma once

#include <type_traits>
#include <numeric>

template<typename TSequence>
struct SequenceTraits {
    using value_type = TSequence;
    using difference_type = std::make_signed_t<TSequence>;
    using size_type = std::make_unsigned_t<TSequence>;

    static constexpr value_type initialSequence = std::numeric_limits<value_type>::max();

    static constexpr difference_type
    difference(value_type a, value_type b)
    {
        return static_cast<difference_type>(a - b);
    }

    static constexpr bool
    precedes(value_type a, value_type b)
    {
        return difference(a, b) < 0;
    }
};