#pragma once

#include "Common.hpp"

#include <boost/container/static_vector.hpp>

#include <array>

template<std::size_t Capacity>
class CircularBuffer {
public:
    using const_buffers_type = boost::container::static_vector<asio::const_buffer, 2>;
    using mutable_buffers_type = boost::container::static_vector<asio::mutable_buffer, 2>;

    auto
    prepare(std::size_t n)
    {
        if (size() + n > max_size()) {
            throw std::length_error{"Overflow"};
        }
        return makeSequence<mutable_buffers_type>(_buffer, _t, _t + n);
    }

    void
    commit(std::size_t n)
    {
        _t += n;
    }

    void
    consume(std::size_t n)
    {
        _h += n;
    }

    auto
    data()
    {
        return makeSequence<const_buffers_type>(_buffer, _h, _t);
    }

    [[nodiscard]] std::size_t
    size() const
    {
        return (_t - _h);
    }

    [[nodiscard]] bool
    empty()
    {
        return (_h == _t);
    }

    [[nodiscard]] constexpr std::size_t
    max_size() const
    {
        return Capacity;
    }

    [[nodiscard]] constexpr std::size_t
    capacity() const
    {
        return Capacity;
    }

private:
    template<typename Sequence, typename Buffer>
    static Sequence
    makeSequence(Buffer& buffer, std::size_t begin, std::size_t end)
    {
        auto size{end - begin};
        begin %= Capacity;
        end %= Capacity;

        if (begin <= end) {
            /* Make a sequence with one buffer (flat buffer) */
            return {typename Sequence::value_type(&buffer[begin], size)};
        } else {
            const std::size_t ending{Capacity - begin};
            /* Make a sequence with two buffers (looped memory range) */
            return {typename Sequence::value_type(&buffer[begin], ending),
                    typename Sequence::value_type(&buffer[0], size - ending)};
        }
    }

private:
    std::array<char, Capacity> _buffer;
    std::size_t _h{0};
    std::size_t _t{0};
};