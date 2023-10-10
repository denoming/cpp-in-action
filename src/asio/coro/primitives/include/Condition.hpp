#pragma once

#include "Event.hpp"

#include <functional>

class Condition {
public:
    using Predicate = std::move_only_function<bool()>;

    explicit Condition(const io::any_io_executor& executor)
        : _channel{executor}
    {
    }

    io::awaitable<sys::error_code>
    wait(Predicate predicate)
    {
        if (not _channel.is_open()) {
            /* Channel is closed */
            co_return sys::error_code{io::error::operation_aborted, sys::system_category()};
        }

        while (not predicate()) {
            auto [status, _] = co_await _channel.async_receive(io::as_tuple(io::use_awaitable));
            if (status) {
                const int code = status.value();
                if (status.category() == ioe::error::get_channel_category()) {
                    /* Channel is closed or cancelled while waiting */
                    status.assign(io::error::operation_aborted, sys::system_category());
                }
                co_return status;
            }
        }

        co_return sys::error_code{};
    }

    io::awaitable<void>
    notify(sys::error_code status = {})
    {
        co_await _channel.async_send(status, unsigned{}, io::use_awaitable);
    }

    void
    tryNotify(sys::error_code status = {})
    {
        _channel.try_send(status, unsigned{});
    }

    void
    close()
    {
        _channel.close();
    }

private:
    ioe::channel<void(sys::error_code, unsigned)> _channel;
};