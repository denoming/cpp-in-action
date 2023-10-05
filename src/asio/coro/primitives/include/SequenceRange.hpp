#pragma once

#include "SequenceTraits.hpp"

#include <iterator>

template<std::unsigned_integral TSequence, typename Traits = SequenceTraits<TSequence>>
class SequenceRange {
public:
    using value_type = TSequence;
    using difference_type = typename Traits::difference_type;
    using size_type = typename Traits::size_type;

    class ConstIterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = TSequence;
        using difference_type = typename Traits::difference_type;
        using reference = const TSequence&;
        using pointer = const TSequence*;

        explicit constexpr ConstIterator(TSequence value) noexcept
            : _value(value)
        {
        }

        const TSequence&
        operator*() const noexcept
        {
            return _value;
        }
        const TSequence*
        operator->() const noexcept
        {
            return std::addressof(_value);
        }

        ConstIterator&
        operator++() noexcept
        {
            ++_value;
            return *this;
        }
        ConstIterator&
        operator--() noexcept
        {
            --_value;
            return *this;
        }

        ConstIterator
        operator++(int) noexcept
        {
            return ConstIterator(_value++);
        }
        ConstIterator
        operator--(int) noexcept
        {
            return ConstIterator(_value--);
        }

        constexpr difference_type
        operator-(ConstIterator other) const noexcept
        {
            return Traits::difference(_value, other._value);
        }
        constexpr ConstIterator
        operator-(difference_type delta) const noexcept
        {
            return ConstIterator{static_cast<TSequence>(_value - delta)};
        }
        constexpr ConstIterator
        operator+(difference_type delta) const noexcept
        {
            return ConstIterator{static_cast<TSequence>(_value + delta)};
        }

        constexpr bool
        operator==(ConstIterator other) const noexcept
        {
            return _value == other._value;
        }
        constexpr bool
        operator!=(ConstIterator other) const noexcept
        {
            return _value != other._value;
        }

    private:
        TSequence _value;
    };

    constexpr SequenceRange() noexcept
        : _begin{}
        , _end{}
    {
    }
    constexpr SequenceRange(TSequence begin, TSequence end) noexcept
        : _begin{begin}
        , _end{end}
    {
    }

    constexpr ConstIterator
    begin() const noexcept
    {
        return ConstIterator(_begin);
    }
    constexpr ConstIterator
    end() const noexcept
    {
        return ConstIterator(_end);
    }

    constexpr TSequence
    front() const noexcept
    {
        return _begin;
    }
    constexpr TSequence
    back() const noexcept
    {
        return _end - 1;
    }

    constexpr size_type
    size() const noexcept
    {
        return static_cast<size_type>(Traits::difference(_end, _begin));
    }

    constexpr bool
    empty() const noexcept
    {
        return _begin == _end;
    }

    constexpr TSequence
    operator[](size_type index) const noexcept
    {
        return _begin + index;
    }

    constexpr SequenceRange
    first(size_type count) const noexcept
    {
        return SequenceRange{_begin, static_cast<TSequence>(_begin + std::min(size(), count))};
    }

    constexpr SequenceRange
    skip(size_type count) const noexcept
    {
        return SequenceRange{_begin + std::min(size(), count), _end};
    }

private:
    TSequence _begin;
    TSequence _end;
};