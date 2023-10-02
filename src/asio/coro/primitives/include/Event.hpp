#pragma once

#include "Asio.hpp"

class Event {
public:
    enum State { NotSet, Waiting, Set };

    io::awaitable<void>
    wait(io::any_io_executor executor);

    bool
    pending();

    void
    set();

    void
    reset();

private:
    io::any_io_executor _executor;
    std::atomic<State> _state{State::NotSet};
    std::move_only_function<void()> _handler;
};