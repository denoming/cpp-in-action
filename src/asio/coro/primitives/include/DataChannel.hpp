#pragma once

#include "Asio.hpp"

#include <boost/circular_buffer.hpp>

template<typename T>
class DataChannel {
public:
    using BufferType = boost::circular_buffer<T>;
    using HandlerType = std::move_only_function<void()>;

    explicit DataChannel(io::any_io_executor executor, std::size_t capacity)
        : _executor{std::move(executor)}
        , _buffer{capacity}
    {
    }

    io::awaitable<std::size_t>
    receive(io::mutable_buffer buffer)
    {
        return io::async_initiate<decltype(io::use_awaitable), void(std::size_t)>(
            [this](io::completion_handler_for<void(std::size_t)> auto handler,
                   io::mutable_buffer buffer) {
                saveRecvHandler(std::move(handler), buffer);
                if (_needRecv = buffer.size(); not _buffer.empty()) {
                    callRecvHandler();
                }
            },
            io::use_awaitable,
            buffer);
    }

    io::awaitable<std::size_t>
    send(io::const_buffer buffer)
    {
        return io::async_initiate<decltype(io::use_awaitable), void(std::size_t)>(
            [this](io::completion_handler_for<void(std::size_t)> auto handler,
                   io::const_buffer buffer) {
                saveSendHandler(std::move(handler), buffer);
                if (_needSend = buffer.size(); not _buffer.full()) {
                    callSendHandler();
                }
            },
            io::use_awaitable,
            buffer);
    }

private:
    std::size_t
    writeBuffer(io::const_buffer& buffer)
    {
        const T* ptr = static_cast<const T*>(buffer.data());
        const auto n = std::min(_buffer.capacity() - _buffer.size(), buffer.size());
        _buffer.insert(_buffer.end(), ptr, ptr + n);
        buffer += n;
        return n;
    }

    std::size_t
    readBuffer(io::mutable_buffer& buffer)
    {
        T* ptr = static_cast<T*>(buffer.data());
        const std::size_t n = std::min(_buffer.size(), buffer.size());
        std::copy(_buffer.cbegin(), _buffer.cbegin() + n, ptr);
        _buffer.erase_begin(n);
        buffer += n;
        return n;
    }

    void
    callRecvHandler()
    {
        if (_recvHandler) {
            _recvHandler();
        }
    }

    void
    callSendHandler()
    {
        if (_sendHandler) {
            _sendHandler();
        }
    }

    void
    saveRecvHandler(io::completion_handler_for<void(std::size_t)> auto handler,
                    io::mutable_buffer buffer)
    {
        _recvHandler = [this, handler = std::move(handler), buffer]() mutable {
            _recv += readBuffer(buffer);
            if (_recv == _needRecv) {
                io::post(_executor,
                         [handler = std::move(handler), recv = _recv]() mutable { handler(recv); });
                _needRecv = _recv = 0;
            }
            if (_needSend > 0) {
                callSendHandler();
            }
        };
    }

    void
    saveSendHandler(io::completion_handler_for<void(std::size_t)> auto handler,
                    io::const_buffer buffer)
    {
        _sendHandler = [this, handler = std::move(handler), buffer]() mutable {
            _send += writeBuffer(buffer);
            if (_send == _needSend) {
                io::post(_executor,
                         [handler = std::move(handler), send = _send]() mutable { handler(send); });
                _needSend = _send = 0;
            }
            if (_needRecv > 0) {
                callRecvHandler();
            }
        };
    }

private:
    io::any_io_executor _executor;
    HandlerType _recvHandler;
    HandlerType _sendHandler;
    std::size_t _needRecv{};
    std::size_t _recv{};
    std::size_t _needSend{};
    std::size_t _send{};
    BufferType _buffer;
};