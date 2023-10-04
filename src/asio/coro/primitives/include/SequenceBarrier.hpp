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

    io::awaitable<TSequence>
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
        : _lastPublished{initialSeq}
        , _awaiters{nullptr}
    {
    }

#ifdef DEBUG
    ~SequenceBarrier()
    {
        assert(_awaiters == nullptr);
    }
#endif

    io::awaitable<TSequence>
    waitUntil(TSequence targetSeq)
    {
        TSequence lastSeq = _lastPublished;
        if (not Traits::precedes(lastSeq, targetSeq)) {
            co_return lastSeq;
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
            if (Traits::precedes(prevSeq, nextSeq)) {
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
        while (toRequeue);

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
    std::atomic<TSequence> _lastPublished;
    mutable std::atomic<TAwaiter*> _awaiters;
};