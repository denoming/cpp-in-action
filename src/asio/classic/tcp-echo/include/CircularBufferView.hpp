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