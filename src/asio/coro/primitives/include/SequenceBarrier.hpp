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
#include "Event.hpp"
#include "SequenceTraits.hpp"

namespace detail {

template<std::unsigned_integral TSequence, typename Traits = SequenceTraits<TSequence>>
struct Awaiter {
public:
    Awaiter* next{nullptr};
    TSequence targetSeq{};
    TSequence publishedSeq{};

    explicit Awaiter(TSequence target)
        : targetSeq{target}
    {
    }

    [[nodiscard]] io::awaitable<TSequence>
    wait()
    {
        co_await _event.wait(io::use_awaitable);
        co_return publishedSeq;
    }

    void
    resume(TSequence published)
    {
        assert(not Traits::precedes(published, targetSeq));
        publishedSeq = published;
        _event.set();
    }

    void
    cancel()
    {
        _event.cancel();
    }

private:
    Event _event;
};

} // namespace detail

template<std::unsigned_integral TSequence = std::size_t,
         typename Traits = SequenceTraits<TSequence>,
         typename TAwaiter = detail::Awaiter<TSequence>>
class SequenceBarrier {
public:
    explicit SequenceBarrier(TSequence initialSeq = Traits::initialSequence)
        : _closed{false}
        , _lastPublished{initialSeq}
        , _awaiters{nullptr}
    {
    }

#ifdef DEBUG
    ~SequenceBarrier()
    {
        assert(_awaiters == nullptr);
    }
#endif

    [[nodiscard]] TSequence
    lastPublished() const
    {
        return _lastPublished;
    }

    void
    close()
    {
        _closed = true;

        TAwaiter* awaiters = _awaiters.exchange(nullptr);
        while (awaiters) {
            awaiters->cancel();
            awaiters = awaiters->next;
        }
    }

    [[nodiscard]] io::awaitable<TSequence>
    wait(TSequence targetSeq)
    {
        TSequence lastSeq = lastPublished();
        if (not Traits::precedes(lastSeq, targetSeq)) {
            co_return lastSeq;
        }

        io::cancellation_state cs = co_await io::this_coro::cancellation_state;
        if (auto slot = cs.slot(); slot.is_connected() and not slot.has_handler()) {
            slot.assign([this](auto) { close(); });
        }

        TAwaiter awaiter{targetSeq};
        addAwaiter(&awaiter);
        lastSeq = co_await awaiter.wait();
        co_return lastSeq;
    }

    void
    publish(TSequence nextSeq)
    {
        _lastPublished.store(nextSeq);

        TAwaiter* awaiter = _awaiters.exchange(nullptr);
        if (not awaiter) {
            return;
        }

        // Define two lists (one with awaiters to requeue and another one to resume)
        TAwaiter* toRequeue{};
        TAwaiter** toRequeueTail = &toRequeue;
        TAwaiter* toResume{};
        TAwaiter** toResumeTail = &toResume;

        do {
            if (Traits::precedes(nextSeq, awaiter->targetSeq)) {
                *toRequeueTail = awaiter;
                toRequeueTail = &(awaiter->next);
            } else {
                *toResumeTail = awaiter;
                toResumeTail = &(awaiter->next);
            }
            awaiter = awaiter->next;
        }
        while (awaiter);

        // Null-terminate both lists
        *toRequeueTail = nullptr;
        *toResumeTail = nullptr;

        if (toRequeue) {
            TAwaiter* oldHead{nullptr};
            while (not _awaiters.compare_exchange_weak(oldHead, toRequeue)) {
                *toRequeueTail = oldHead;
            }
        }

        while (toResume) {
            TAwaiter* next = toResume->next;
            toResume->resume(nextSeq);
            toResume = next;
        }
    }

private:
    void
    addAwaiter(TAwaiter* awaiter)
    {
        // Define two lists (one with awaiters to requeue and another one to resume)
        TAwaiter* toRequeue = awaiter;
        TAwaiter** toRequeueTail = &(toRequeue->next);
        TAwaiter* toResume;
        TAwaiter** toResumeTail = &toResume;

        TSequence prevSeq{}, nextSeq = awaiter->targetSeq;
        do {
            // Enqueue the awaiter(s)
            TAwaiter* oldHead = _awaiters;
            do {
                *toRequeueTail = oldHead;
            }
            while (not _awaiters.compare_exchange_weak(oldHead, toRequeue));

            // Check if the sequence we were waiting for wasn't published yet
            prevSeq = _lastPublished;
            if (Traits::precedes(prevSeq, nextSeq) and not _closed) {
                // None of the awaiters we enqueued have been satisfied yet
                break;
            }

            // Reset the requeue list to empty
            toRequeueTail = &toRequeue;

            auto minDiff = std::numeric_limits<typename Traits::difference_type>::max();
            auto* awaiters = _awaiters.exchange(nullptr);
            while (awaiters) {
                const auto diff = Traits::difference(awaiters->targetSeq, prevSeq);
                if (diff > 0) {
                    *toRequeueTail = awaiters;
                    toRequeueTail = &(awaiters->next);
                    minDiff = std::min(diff, minDiff);
                } else {
                    *toResumeTail = awaiters;
                    toResumeTail = &(awaiters->next);
                }
                awaiters = awaiters->next;
            }

            // Null-terminate the list of awaiters to requeue.
            *toRequeueTail = nullptr;

            // Calculate the earliest target sequence required by any of the awaiters to requeue.
            nextSeq = static_cast<TSequence>(prevSeq + minDiff);
        }
        while (toRequeue and not _closed);

        // Null-terminate the list of awaiters to resume
        *toResumeTail = nullptr;

        // Resume the awaiters that are ready
        while (toResume != nullptr) {
            TAwaiter* next = toResume->next;
            toResume->resume(prevSeq);
            toResume = next;
        }
    }

private:
    std::atomic<bool> _closed;
    std::atomic<TSequence> _lastPublished;
    std::atomic<TAwaiter*> _awaiters;
};