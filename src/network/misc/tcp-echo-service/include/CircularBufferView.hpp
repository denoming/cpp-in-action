#pragma once

#include "CircularBuffer.hpp"

template<std::size_t Capacity>
class CircularBufferView {
public:
    using buffer_type = CircularBuffer<Capacity>;
    using const_buffers_type = typename buffer_type::const_buffers_type;
    using mutable_buffers_type = typename buffer_type::mutable_buffers_type;

    explicit CircularBufferView(buffer_type& buffer)
        : _buffer{&buffer}
    {
    }

    auto
    prepare(std::size_t n)
    {
        return _buffer->prepare(n);
    }

    void
    commit(std::size_t n)
    {
        _buffer->commit(n);
    }

    void
    consume(std::size_t n)
    {
        _buffer->consume(n);
    }

    auto
    data()
    {
        return _buffer->data();
    }

    [[nodiscard]] std::size_t
    size() const
    {
        return _buffer->size();
    }

    [[nodiscard]] constexpr std::size_t
    max_size() const
    {
        return _buffer->max_size();
    }

    [[nodiscard]] constexpr std::size_t
    capacity() const
    {
        return _buffer->capacity();
    }

private:
    buffer_type* _buffer;
};

template<std::size_t Capacity>
CircularBufferView<Capacity>
makeView(CircularBuffer<Capacity>& buffer)
{
    return CircularBufferView<Capacity>{buffer};
}