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