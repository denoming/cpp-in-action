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

#include "Asio.hpp"

class Event {
public:
    enum class State { NotSet, Waiting, Set, Cancelled };

    template<io::completion_token_for<void(sys::error_code)> CompletionToken>
    auto
    wait(CompletionToken&& token)
    {
        auto initiate
            = [this](io::completion_handler_for<void(sys::error_code)> auto&& handler) mutable {
                  if (auto slot = io::get_associated_cancellation_slot(handler);
                      slot.is_connected() and not slot.has_handler()) {
                      slot.assign([this](auto) { cancel(); });
                  }

                  _handler = [executor = io::get_associated_executor(handler),
                              handler = std::forward<decltype(handler)>(handler)](auto ec) mutable {
                      io::post(executor,
                               [handler = std::move(handler), ec]() mutable { handler(ec); });
                  };

                  State oldState = State::NotSet;
                  if (not _state.compare_exchange_strong(oldState,
                                                         State::Waiting,
                                                         std::memory_order_release,
                                                         std::memory_order_acquire)) {
                      _handler((oldState == State::Cancelled)
                                   ? sys::error_code{io::error::operation_aborted}
                                   : sys::error_code{});
                  }
              };

        return io::async_initiate<CompletionToken, void(sys::error_code)>(initiate, token);
    }

    [[nodiscard]] State
    state() const
    {
        return _state;
    }

    void
    set()
    {
        State oldState = State::NotSet;
        if (not _state.compare_exchange_strong(
                oldState, State::Set, std::memory_order_release, std::memory_order_acquire)) {
            /* wait(...) call was first */
            if (oldState == State::Waiting
                and _state.compare_exchange_strong(
                    oldState,
                    State::Set,
                    std::memory_order_relaxed,
                    std::memory_order_relaxed) /* Try to set again */) {
                _handler(sys::error_code{});
            }
        }
    }

    void
    cancel()
    {
        State oldState = State::NotSet;
        if (not _state.compare_exchange_strong(
                oldState, State::Cancelled, std::memory_order_release, std::memory_order_acquire)) {
            /* wait(...) call was first */
            if (oldState == State::Waiting
                and _state.compare_exchange_strong(
                    oldState,
                    State::Cancelled,
                    std::memory_order_release,
                    std::memory_order_acquire) /* Try to set again */) {
                _handler(sys::error_code{io::error::operation_aborted});
            }
        }
    }

    void
    reset()
    {
        _state = State::NotSet;
        _handler = {};
    }

private:
    std::atomic<State> _state{State::NotSet};
    std::move_only_function<void(sys::error_code)> _handler;
};